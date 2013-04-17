/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medTestDbApp.h"


#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkLog/dtkLog.h>



#include <medDbControllerFactory.h>

#include <medMetaDataKeys.h>
#include <medPluginManager.h>
#include <medStorage.h>

#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDatabaseRemover.h>


#include <medTest/medQtDataImage.h>
#include <medTest/medQtDataImageReader.h>
#include <medTest/medQtDataImageWriter.h>

#include <iostream>
#include <stdexcept>
#include <vector>

#include <QHash>


// Constructor : initialise the test environment.
medTestDbApp::medTestDbApp( int argc, char *argv[] ) :
    medTestApplication(argc,argv),testResult(DTK_FAILURE)
{
    if ( argc < 2 ) {
        throw std::runtime_error("Usage : $0 database_location [plugin_path]");
    }

    medStorage::setDataLocation( argv[1] );

    QString pluginPath;
    if ( argc >= 3 )
        pluginPath = argv[2];
    else
        pluginPath = "";

    // Remove any traces of db.
    QFile dbFile( QString(argv[1]) + "/" + "db");
    dbFile.remove();
    medStorage::rmpath(QString(argv[1]) + "/" + "TestPatient");

    medPluginManager::instance()->setPath( pluginPath );
    medPluginManager::instance()->initialize();

    if(!medDatabaseController::instance()->createConnection())
        CHECK_TEST_RESULT( "Unable to create a connection to the database" );

    //Register dbController
    medDbControllerFactory::instance()->registerDbController("DbController", createDbController);
    medDbControllerFactory::instance()->registerDbController("NonPersistentDbController", createNonPersistentDbController);

    CHECK_TEST_RESULT( medQtDataImage::registered() );
    CHECK_TEST_RESULT( medQtDataImageReader::registered() );
    CHECK_TEST_RESULT( medQtDataImageWriter::registered() );

}

medTestDbApp::~medTestDbApp()
{

}

void medTestDbApp::onNonPersistentDataImported(const medDataIndex& index) {
    importedIndex = index;
    handleNonPersistentDataImported();
}

void medTestDbApp::onPersistentDataImported(const medDataIndex& index) {
    importedIndex = index;
    handlePersistentDataImported();
}

void medTestDbApp::onImportFailed(const medDataIndex& index,const QString & uuid) {
    Q_UNUSED(uuid);
    importedIndex = index;
    exit();
}


void medTestDbApp::run()
{
    dataManager = medDataManager::instance();
    CHECK_TEST_RESULT( dataManager );

    testData =  CreateTestData() ;
    CHECK_TEST_RESULT(testData);

    connect(dataManager, SIGNAL(importFailed(medDataIndex,QString)), this, SLOT(onImportFailed(const medDataIndex&,const QString&)));
    connect(dataManager, SIGNAL(dataAdded(const medDataIndex&)), this, SLOT(onNonPersistentDataImported(const medDataIndex&)));

    // Initially should be empty
    CHECK_TEST_RESULT( dataManager->nonPersistentDataCount() == 0 );

    // Test import of new data
    dataManager->importNonPersistent( testData );

    //  Wait until the data is actually imported.
}

void medTestDbApp::handleNonPersistentDataImported()
{
    CHECK_TEST_RESULT(importedIndex.isValid());
    CHECK_TEST_RESULT(importedIndex.isValidForSeries());

    const int persistentSourceId = 1;
    const int nonPersistentSourceId = 2;

    // Check freshly imported data exists.
    npDb = dataManager->controllerForDataSource(nonPersistentSourceId);
    CHECK_TEST_RESULT( npDb );
    CHECK_TEST_RESULT(npDb->dataSourceId() == nonPersistentSourceId);
    CHECK_TEST_RESULT( !npDb->isPersistent() );

    typedef QList<medDataIndex> IndexList;
    IndexList patients = npDb->patients();
    qDebug() << "patient size:"<< patients.size();
    CHECK_TEST_RESULT(patients.size() == 1);
    CHECK_TEST_RESULT(patients[0].patientId() == importedIndex.patientId());
    IndexList studies = npDb->studies(patients[0]);
    CHECK_TEST_RESULT(studies.size() == 1);
    CHECK_TEST_RESULT(studies[0].studyId() == importedIndex.studyId());
    IndexList series = npDb->series(studies[0]);
    CHECK_TEST_RESULT(series.size() == 1);
    CHECK_TEST_RESULT(series[0].seriesId() == importedIndex.seriesId());

    // Ensure persistent DB is empty at first
    db = dataManager->controllerForDataSource(persistentSourceId);
    CHECK_TEST_RESULT( db );

    CHECK_TEST_RESULT( db->patients().size() == 0 );

    // Test import from non-Persistent to persistent
    //disconnect(dataManager);
    disconnect(dataManager, 0, this, 0);
    connect(dataManager, SIGNAL(dataAdded(const medDataIndex&)), this, SLOT(onPersistentDataImported(const medDataIndex&)));
    //TODO: reeanble this test when fixing supported extension bug,
    //and priority list!
    //dataManager->storeNonPersistentSingleDataToDatabase(importedIndex);
    //when reenabling, remove these two lines
    testResult = DTK_SUCCEED;
    exit();
}

void medTestDbApp::handlePersistentDataImported()
{
    CHECK_TEST_RESULT( db->patients().size() == 1 );
    CHECK_TEST_RESULT( npDb->patients().size() == 0 );
    typedef QList<medDataIndex> IndexList;
    IndexList patients = db->patients();
    CHECK_TEST_RESULT(patients.size() == 1);
    IndexList studies = db->studies(patients[0]);
    CHECK_TEST_RESULT(studies.size() == 1);
    IndexList series = db->series(studies[0]);
    CHECK_TEST_RESULT(series.size() == 1);

    const medDataIndex persImportedIndex = series[0];

    this->processEvents();

    // clear cache.
    dataManager->clearCache();

    // Check data in db matches original.
    dtkSmartPointer<dtkAbstractData> testDataFromDb = dataManager->data( persImportedIndex );
    CHECK_TEST_RESULT(testDataFromDb->identifier() == testData->identifier());
    CHECK_TEST_RESULT(medMetaDataKeys::PatientName.getFirstValue(testDataFromDb) ==
                      medMetaDataKeys::PatientName.getFirstValue(testData));

    // Check removing works ok - need to use synchronous version.
    //dataManager->removeData( persImportedIndex );
    //this->processEvents();
    medDatabaseRemover remover( persImportedIndex );
    remover.run();

    // Should be no patients now.
    patients = db->patients();
    CHECK_TEST_RESULT(patients.size() == 0);

    //close db?
    medDatabaseController::instance()->closeConnection();
    testResult = DTK_SUCCEED;
    exit();
}

dtkSmartPointer<dtkAbstractData> medTestDbApp::CreateTestData( )
{
    // Create a data.
    dtkAbstractDataFactory *dataFactory = dtkAbstractDataFactory::instance();
    CHECK_TEST_RESULT( dataFactory );

    dtkSmartPointer<dtkAbstractData> testData = dataFactory->createSmartPointer(medQtDataImage::s_description());
    CHECK_TEST_RESULT( testData );

    medMetaDataKeys::PatientName.set(testData,"TestPatient");
    medMetaDataKeys::StudyDescription.set(testData,"TestStudy");
    medMetaDataKeys::SeriesDescription.set(testData,"TestSeries");

    QImage testImage(QSize( 800, 500 ), QImage::Format_Mono );

    QPainter painter(&testImage);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::gray);
    painter.fillRect(testImage.rect(), Qt::black);

    painter.drawEllipse(QPoint(400,250), 300, 100);

    testData->setData( &testImage );
    return testData;
}


