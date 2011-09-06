#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

#include <dtkCore/dtkSmartPointer.h>

#include <medCore/medDbControllerFactory.h>
#include <medCore/medDataManager.h>
#include <medCore/medMetaDataKeys.h>
#include <medCore/medPluginManager.h>
#include <medCore/medStorage.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseNonPersistentController.h>
#include <medSql/medDatabaseRemover.h>

#include <medTestCommon/medTestApplication.h>
#include <medTestCommon/medQtDataImage.h>
#include <medTestCommon/medQtDataImageReader.h>
#include <medTestCommon/medQtDataImageWriter.h>

#include <iostream>
#include <stdexcept>
#include <vector>

#include <QHash>

class medTestDbApp : public medTestApplication 
{
public:
    medTestDbApp( int argc, char *argv[]);
    virtual ~medTestDbApp();
    int run();

    dtkSmartPointer<dtkAbstractData> CreateTestData( );

private slots:

    void onNonPersistentDataImported(const medDataIndex& index) {
        importedIndex = index;
        mutex.unlock();
    }

private:

    QMutex         mutex;
    medDataIndex   importedIndex;
};

// Constructor : initialise the test environment.
medTestDbApp::medTestDbApp( int argc, char *argv[] ) :
    medTestApplication(argc,argv)
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

int medTestDbApp::run()
{
    int ret = DTK_FAILURE;
    medDataManager *dataManager = medDataManager::instance();
    CHECK_TEST_RESULT( dataManager );

    dtkSmartPointer<dtkAbstractData> testData( CreateTestData() );
    CHECK_TEST_RESULT(testData);

    // Initially should be empty
    CHECK_TEST_RESULT( dataManager->nonPersistentDataCount() == 0 );

    // Test import of new data
    dataManager->importNonPersistent( testData );

    //  Wait untill the data is actually imported.

    mutex.lock();
    QWaitCondition NPDI;
    NPDI.wait(&mutex);

    CHECK_TEST_RESULT(importedIndex.isValid());
    CHECK_TEST_RESULT(importedIndex.isValidForSeries());

    const int persistentSourceId = 1;
    const int nonPersistentSourceId = 2;

    // Check freshly imported data exists.
    medAbstractDbController * npDb = dataManager->controllerForDataSource(nonPersistentSourceId);
    CHECK_TEST_RESULT( npDb );
    CHECK_TEST_RESULT(npDb->dataSourceId() == nonPersistentSourceId);
    CHECK_TEST_RESULT( !npDb->isPersistent() );

    typedef QList<medDataIndex> IndexList;
    IndexList patients = npDb->patients();
    CHECK_TEST_RESULT(patients.size() == 1);
    CHECK_TEST_RESULT(patients[0].patientId() == importedIndex.patientId());
    IndexList studies = npDb->studies(patients[0]);
    CHECK_TEST_RESULT(studies.size() == 1);
    CHECK_TEST_RESULT(studies[0].studyId() == importedIndex.studyId());
    IndexList series = npDb->series(studies[0]);
    CHECK_TEST_RESULT(series.size() == 1);
    CHECK_TEST_RESULT(series[0].seriesId() == importedIndex.seriesId());

    // Ensure persistent DB is empty at first
    medAbstractDbController * db = dataManager->controllerForDataSource(persistentSourceId);
    CHECK_TEST_RESULT( db );

    CHECK_TEST_RESULT( db->patients().size() == 0 );

    // Test import from non-Persistent to persistent
    dataManager->storeNonPersistentSingleDataToDatabase(importedIndex);

    CHECK_TEST_RESULT( db->patients().size() == 1 );
    CHECK_TEST_RESULT( npDb->patients().size() == 0 );

    patients = db->patients();
    CHECK_TEST_RESULT(patients.size() == 1);
    studies = db->studies(patients[0]);
    CHECK_TEST_RESULT(studies.size() == 1);
    series = db->series(studies[0]);
    CHECK_TEST_RESULT(series.size() == 1);

    const medDataIndex persImportedIndex = series[0];

    this->processEvents();

    // clear cache.
    dataManager->clearCache();

    // Check data in db matches original.
    dtkSmartPointer<dtkAbstractData> testDataFromDb = dataManager->data( persImportedIndex );
    CHECK_TEST_RESULT(testDataFromDb->description() == testData->description());
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

    ret = DTK_SUCCEED;
    return ret;
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






int main(int argc, char *argv[])
{
    int ret = DTK_FAILURE;
    try {
        medTestDbApp theTest(argc,argv);
        ret = theTest.run();
    }
    catch ( std::exception & e ){
        dtkDebug() << "Exception caught : "<< e.what();
        ret = DTK_FAILURE;
    }
    catch ( ... ){
        dtkDebug() << "Exception caught : " << "[Unknown exception type]";
        ret = DTK_FAILURE;
    }
    return ret;
}

