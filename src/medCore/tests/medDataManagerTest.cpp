/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDataManagerTest.h>

#include <medDataManager.h>

#include <medAbstractDataFactory.h>
#include <medMetaDataKeys.h>
#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medStorage.h>

#include <medTest/medQtDataImage.h>
#include <medTest/medQtDataImageReader.h>
#include <medTest/medQtDataImageWriter.h>

#include <QtTest/QSignalSpy>



medDataManagerTestObject::medDataManagerTestObject(void)
{
    m_storagePath =  QCoreApplication::applicationDirPath() + "/testSqlTempDb";
    m_currentId = 0;
    
    connect(this, SIGNAL(dataAdded()), &m_eventLoop, SLOT(quit()));
}

medDataManagerTestObject::~medDataManagerTestObject(void)
{

}

void medDataManagerTestObject::initTestCase(void)
{
    qRegisterMetaType<medDataIndex>();
    
    QVERIFY( medQtDataImage::registered() );
    QVERIFY( medQtDataImageReader::registered() );
    QVERIFY( medQtDataImageWriter::registered() );
 
    medStorage::rmpath(m_storagePath);
    medStorage::setDataLocation( m_storagePath );
    
    QVERIFY( medDatabaseController::instance()->createConnection() );
    
    const int persistentSourceId = 1;
    const int nonPersistentSourceId = 2;

    db = medDataManager::instance()->controllerForDataSource(persistentSourceId);
    QVERIFY( db );
    
    npDb = medDataManager::instance()->controllerForDataSource(nonPersistentSourceId);
    QVERIFY( npDb );  
    
    connect(medDataManager::instance(), SIGNAL(dataAdded(const medDataIndex&)), this, SLOT(onDataAdded(const medDataIndex&)));
    connect(medDataManager::instance(), SIGNAL(dataRemoved(medDataIndex)), this, SLOT(onDataRemoved(const medDataIndex&)));

}

void medDataManagerTestObject::init(void)
{

}

void medDataManagerTestObject::cleanup(void)
{
}

void medDataManagerTestObject::cleanupTestCase(void)
{
    removeDir(m_storagePath);   
}

dtkSmartPointer<medAbstractData> medDataManagerTestObject::createTestData(void)
{
    // Create a data.
    medAbstractDataFactory *dataFactory = medAbstractDataFactory::instance();
    dtkSmartPointer<medAbstractData> testData = dataFactory->createSmartPointer(medQtDataImage::staticDescription());
    
    QString sDatetime = QDateTime::currentDateTime().toString();

    medMetaDataKeys::PatientName.set(testData,"TestPatient" + QString::number(m_currentId));
    medMetaDataKeys::StudyDescription.set(testData,"TestStudy" + QString::number(m_currentId));
    medMetaDataKeys::SeriesDescription.set(testData,"TestSeries" + QString::number(m_currentId));
    medMetaDataKeys::BirthDate.set(testData, sDatetime);
    m_currentId++;

    QImage testImage(QSize( 800, 500 ), QImage::Format_Mono );

    QPainter painter(&testImage);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::gray);
    painter.fillRect(testImage.rect(), Qt::black);

    painter.drawEllipse(QPoint(400,250), 300, 100);

    testData->setData( &testImage );
    return testData;
}


void medDataManagerTestObject::testImport(void)
{
    m_currentData = createTestData();
    
    IndexList prevPatients = db->patients();
    IndexList prevNPPatients = npDb->patients();

    medDataManager::instance()->import(m_currentData);

    waitForInsertions();
    
    QVERIFY(m_lastInsertedIndex.isValid());
   
    IndexList patients = db->patients();
    QCOMPARE(patients.size(), prevPatients.size()+1);
    IndexList studies = db->studies(m_lastInsertedIndex);
    QCOMPARE(studies.size(), 1);
    IndexList series = db->series(studies[0]);
    QCOMPARE(series.size(), 1);
    
    QCOMPARE( npDb->patients().size(), prevNPPatients.size() );

    const medDataIndex importedIndex = series[0];
    QVERIFY(importedIndex.isValid());

    // Check data in db matches original.
    compareData();
}

void medDataManagerTestObject::testImportNonPersistent(void)
{
    m_currentData = createTestData();
    
    IndexList prevNPPatients = npDb->patients();
    
    medDataManager::instance()->importNonPersistent(m_currentData);
    
    waitForInsertions();
    
    QVERIFY(m_lastInsertedIndex.isValid());
    
    IndexList patients = npDb->patients();
    QCOMPARE(patients.size(), prevNPPatients.size()+1);
    IndexList studies = npDb->studies(m_lastInsertedIndex);
    QCOMPARE(studies.size(), 1);
    IndexList series = npDb->series(m_lastInsertedIndex);
    QCOMPARE(series.size(), 1);

    const medDataIndex importedIndex = series[0];
    QVERIFY(importedIndex.isValid());

    compareData();
}

void medDataManagerTestObject::testImport2NonPersistentSeries(void)
{
    // first, import a serie
    testImportNonPersistent();
    
    IndexList prevNPPatients = npDb->patients();
    
    //and import a second serie, copied from the first one, just changing series description
    
    //going back by one to dupplicate the data
    m_currentId--;
    m_currentData = createTestData();
    medMetaDataKeys::SeriesDescription.set(m_currentData,"TestSeries" + QString::number(m_currentId-1) + "bis");
    
    medDataManager::instance()->importNonPersistent(m_currentData);
    
    waitForInsertions();
    
    IndexList patients = npDb->patients();
    //patients size should be the same
    QCOMPARE(patients.size(), prevNPPatients.size());
    IndexList studies = npDb->studies(m_lastInsertedIndex);
    //studies size should be the same
    QCOMPARE(studies.size(), 1);
    IndexList series = npDb->series(studies[0]);
    //but there should be 2 series
    QCOMPARE(series.size(), 2);

    compareData();   
}

void medDataManagerTestObject::testImportFile(void)
{    
    //create a file to be imported 
    m_currentData = createTestData();
    
    medQtDataImageWriter writer;
    writer.setData(m_currentData);
    m_fileToImport = m_storagePath + "/fileToImport.png";
    
    QVERIFY( writer.write(m_fileToImport) == true );
    
    IndexList prevPatients = db->patients();

    medDataManager::instance()->import(m_fileToImport, false);
    
    waitForInsertions();
    
    QVERIFY(m_lastInsertedIndex.isValid());
  
    IndexList patients = db->patients();
    QCOMPARE(patients.size(), prevPatients.size()+1);
    IndexList studies = db->studies(m_lastInsertedIndex);
    QCOMPARE(studies.size(), 1);
    IndexList series = db->series(studies[0]);
    QCOMPARE(series.size(), 1);

    const medDataIndex importedIndex = series[0];
    QVERIFY(importedIndex.isValid());

    // Check data in db matches original.
    compareData();
    
    dtkSmartPointer<medAbstractData> insertedData = medDataManager::instance()->data( m_lastInsertedIndex );
    QString patientID = medMetaDataKeys::PatientID.getFirstValue(insertedData);
    QString serieID = medMetaDataKeys::SeriesID.getFirstValue(insertedData);
    QFileInfo fileInfo(m_storagePath + "/" + patientID + "/" + serieID );
    
    //check that image file is created
    QDir dir(m_storagePath + "/" + patientID);
    QFileInfoList fileInfoList = dir.entryInfoList( QDir::Files );
    
    QStringList fileList;
    foreach(QFileInfo file, fileInfoList)
        fileList.append(file.fileName());
    
    //filename is no exactly serie's ID
    QStringList filter = fileList.filter(fileInfo.fileName());
    
    QVERIFY( filter.size() == 1 );   
}

void medDataManagerTestObject::testIndexFile(void)
{
    //create a file to be indexed 
    m_currentData = createTestData();
    
    medQtDataImageWriter writer;
    writer.setData(m_currentData);
    m_fileToImport = m_storagePath + "/fileToIndex.png";
    
    QVERIFY( writer.write(m_fileToImport) == true );
    
    IndexList prevPatients = db->patients();
   
    medDataManager::instance()->import(m_fileToImport, true);
    
    waitForInsertions();
    
    QVERIFY(m_lastInsertedIndex.isValid());
   
    IndexList patients = db->patients();
    QCOMPARE(patients.size(), prevPatients.size()+1);
    IndexList studies = db->studies(m_lastInsertedIndex);
    QCOMPARE(studies.size(), 1);
    IndexList series = db->series(studies[0]);
    QCOMPARE(series.size(), 1);

    const medDataIndex importedIndex = series[0];
    QVERIFY(importedIndex.isValid());

    // Check data in db matches original.
    compareData();
    
    dtkSmartPointer<medAbstractData> insertedData = medDataManager::instance()->data( m_lastInsertedIndex );
    QString patientID = medMetaDataKeys::PatientID.getFirstValue(insertedData);
    QString serieID = medMetaDataKeys::SeriesID.getFirstValue(insertedData);
    
    QDir dir(m_storagePath + "/" + patientID);
    QFileInfoList fileInfoList = dir.entryInfoList( QDir::Files );
    
    //check that there is no image file created
    QCOMPARE(fileInfoList.size(), 0);
    
}

void medDataManagerTestObject::testImportNonPersistentFile(void)
{
    //create a file to be indexed 
    m_currentData = createTestData();
    
    medQtDataImageWriter writer;
    writer.setData(m_currentData);
    m_fileToImport = m_storagePath + "/fileToImportInNPDb.png";
    
    QVERIFY(  writer.write(m_fileToImport) == true );
    
    IndexList prevPatients = npDb->patients();
    
    medDataManager::instance()->importNonPersistent(m_fileToImport);
    
    waitForInsertions();
    
    QVERIFY(m_lastInsertedIndex.isValid());
   
    IndexList patients = npDb->patients();
    QCOMPARE(patients.size(), prevPatients.size()+1);
    IndexList studies = npDb->studies(m_lastInsertedIndex);
    QCOMPARE(studies.size(), 1);
    IndexList series = npDb->series(studies[0]);
    QCOMPARE(series.size(), 1);

    const medDataIndex importedIndex = series[0];
    QVERIFY(importedIndex.isValid());

    // Check data in db matches original.
    compareData();    
}

void medDataManagerTestObject::testClearNonPersistentData(void)
{
    IndexList prevNPPatients = npDb->patients();
    
    if( prevNPPatients.size() == 0)
    {
        testImportNonPersistent();
        testImportNonPersistentFile();   
    }
    
    prevNPPatients = npDb->patients();
    
    QVERIFY( prevNPPatients.size() > 0 );
    
    medDataManager::instance()->clearNonPersistentData();
    
    IndexList patients = npDb->patients();
    
    QCOMPARE( patients.size(), 0 );
    QCOMPARE( medDataManager::instance()->nonPersistentDataCount(), 0 );
       
    //TODO: to complete   ?
     
}

void medDataManagerTestObject::testStoreNonPersistentDataToDatabase (void)
{   
    IndexList prevNPPatients = npDb->patients();
    
    if( prevNPPatients.size() == 0 )
    {
        testImportNonPersistent();
        testImportNonPersistentFile();   
    }
    
    IndexList prevPatients = db->patients();
    prevNPPatients = npDb->patients();
    
    QVERIFY( prevNPPatients.size() > 0 );
    
    medDataManager::instance()->storeNonPersistentDataToDatabase();

    waitForInsertions(prevNPPatients.size());
    
    IndexList currentPatients = db->patients();
    IndexList currentNPPatients = npDb->patients();
    
    QCOMPARE( currentPatients.size(), prevPatients.size() + prevNPPatients.size());
    QCOMPARE( currentNPPatients.size(), 0);
    
    QCOMPARE( medDataManager::instance()->nonPersistentDataCount(), 0);
      
}

void medDataManagerTestObject::testStoreNonPersistentMultipleDataToDatabase(void)
{  
    IndexList prevNPPatients = npDb->patients();
    medDataIndex indexFor1Serie;
    medDataIndex indexFor2Series;
     
    if( prevNPPatients.size() == 0)
    {
        testImportNonPersistent();
        indexFor1Serie = m_lastInsertedIndex;
        testImport2NonPersistentSeries();   
        indexFor2Series = m_lastInsertedIndex;
    }
    
    IndexList prevPatients = db->patients();
    prevNPPatients = npDb->patients();
    
    QVERIFY( prevNPPatients.size() > 1 );
    
    // Test 1 - store just a serie
    
    // retrieve indexes for patient, study,serie 
    medDataIndex patient1;
    patient1.setDataSourceId(indexFor1Serie.dataSourceId());
    patient1.setPatientId(indexFor1Serie.patientId());
    medDataIndex study1 = npDb->studies(patient1)[0];
    medDataIndex serie1 = npDb->series(study1)[0];
    
    QCOMPARE(serie1, indexFor1Serie);
    
    dtkSmartPointer<medAbstractData> originalData = medDataManager::instance()->data( serie1 );
    
    medDataManager::instance()->storeNonPersistentMultipleDataToDatabase(serie1);
    
    waitForInsertions();
    
    QVERIFY( m_lastInsertedIndex.isValidForSeries());
    dtkSmartPointer<medAbstractData> insertedData = medDataManager::instance()->data( m_lastInsertedIndex );
    
    compareData(originalData, insertedData);
    
    // Test 2 - store a patient and its associated series
    
    // retrieve indexes for patient, study, serie 
    medDataIndex patient2;
    patient2.setDataSourceId(indexFor2Series.dataSourceId());
    patient2.setPatientId(indexFor2Series.patientId());
    medDataIndex study2 = npDb->studies(patient2)[0];
    IndexList series2 =  npDb->series(study2);
    QVERIFY( series2.size()>1 );
    medDataIndex serie2_1 = npDb->series(study2)[0];
    medDataIndex serie2_2 = npDb->series(study2)[1];
   
    dtkSmartPointer<medAbstractData> originalDataSerie2_1 = medDataManager::instance()->data( serie2_1 );
    dtkSmartPointer<medAbstractData> originalDataSerie2_2 = medDataManager::instance()->data( serie2_2 );
    
    medDataManager::instance()->storeNonPersistentMultipleDataToDatabase(patient2);
    
    waitForInsertions(series2.size());
    
    QVERIFY( m_insertedIndexes.size() > 1);
    
    dtkSmartPointer<medAbstractData> insertedDataSerie2_1 = medDataManager::instance()->data( m_insertedIndexes[0] );
    dtkSmartPointer<medAbstractData> insertedDataSerie2_2 = medDataManager::instance()->data( m_insertedIndexes[1] );
    
    // we don't know the order of insertion
    if( medMetaDataKeys::SeriesDescription.getFirstValue(insertedDataSerie2_1) ==  
        medMetaDataKeys::SeriesDescription.getFirstValue(originalDataSerie2_1))
    {
        compareData(insertedDataSerie2_1, originalDataSerie2_1);
        compareData(insertedDataSerie2_2, originalDataSerie2_2);
    }
    else if( medMetaDataKeys::SeriesDescription.getFirstValue(insertedDataSerie2_1) ==  
        medMetaDataKeys::SeriesDescription.getFirstValue(originalDataSerie2_2))
    {
        compareData(insertedDataSerie2_1, originalDataSerie2_2);
        compareData(insertedDataSerie2_2, originalDataSerie2_1);
    }
    else QVERIFY( false );
}


void medDataManagerTestObject::testStoreNonPersistentSingleDataToDatabase(void)
{
    IndexList prevNPPatients = npDb->patients();
    medDataIndex indexFor1Serie;
    medDataIndex indexFor2Series;
     
    if( prevNPPatients.size() == 0)
    {
        testImportNonPersistent();
        indexFor1Serie = m_lastInsertedIndex;
        testImport2NonPersistentSeries();   
        indexFor2Series = m_lastInsertedIndex;
    }
    
    IndexList prevPatients = db->patients();
    prevNPPatients = npDb->patients();
    
    QVERIFY( prevNPPatients.size() > 1 );
    
    // retrieve indexes for patient, study,serie 
    medDataIndex patient1;
    patient1.setDataSourceId(indexFor1Serie.dataSourceId());
    patient1.setPatientId(indexFor1Serie.patientId());
    medDataIndex study1 = npDb->studies(patient1)[0];
    medDataIndex serie1 = npDb->series(study1)[0];
    
    QCOMPARE(serie1, indexFor1Serie);
    
    dtkSmartPointer<medAbstractData> originalData = medDataManager::instance()->data( serie1 );
    
    medDataManager::instance()->storeNonPersistentMultipleDataToDatabase(serie1);
    
    waitForInsertions();
    
    QVERIFY( m_lastInsertedIndex.isValidForSeries());
    dtkSmartPointer<medAbstractData> insertedData = medDataManager::instance()->data( m_lastInsertedIndex );
    
    compareData(originalData, insertedData);
  
}


void medDataManagerTestObject::testRemoveData(void)
{
    foreach(int datasource, medDataManager::instance()->dataSourceIds())
    {
        medAbstractDbController *controller = medDataManager::instance()->controllerForDataSource(datasource);

        IndexList prevPatients = controller->patients();

        //inserting at least 1 patient in the db
        if( prevPatients.size() == 0 )
        {
            if( controller->isPersistent())
                testImport();
            else testImportNonPersistent();
        }

        prevPatients = controller->patients();
        
        connect(this, SIGNAL(dataRemoved()), &m_eventLoop, SLOT(quit()));
        int nbPatients = prevPatients.size();
        
        foreach(medDataIndex patient, prevPatients)
        {
            medDataIndex study, serie;
            IndexList studies = controller->studies(patient);
            if( studies.size( )> 0 )
            {
                study = controller->studies(patient)[0];
                if( controller->series(study).size() > 0 )
                {
                    serie = controller->series(study)[0];
                }
                
            }
            int nbDeletions = 0;
            foreach(medDataIndex tmpStudy, studies)
            {
                nbDeletions += controller->series(tmpStudy).count();
            }
            nbDeletions += studies.count() + 1;

            medDataManager::instance()->removeData(patient);
            if(datasource==1)
            {
                //this only work for persisitent controller, since, NPcontroller remove() is synchronous
                waitForDeletions(nbDeletions);
            }


            IndexList patients = controller->patients();
            QCOMPARE( patients.size(), nbPatients-1 );
            nbPatients--;
            dtkSmartPointer<medAbstractData> data = medDataManager::instance()->data( patient );
            QVERIFY(!data);
            data = medDataManager::instance()->data( study );
            QVERIFY(!data);
            data = medDataManager::instance()->data( serie );
            QVERIFY(!data);
        }
    }

    // let's check that image files are deleted
    QDir dir(m_storagePath);
    QFileInfoList fileInfoList = dir.entryInfoList( QDir::Dirs | QDir::NoDotAndDotDot );

    QCOMPARE (fileInfoList.size(), 0); 
    
}

void medDataManagerTestObject::testMoveStudy(void)
{
    //let's clean up everything
    testRemoveData();
    
    foreach(int datasource, medDataManager::instance()->dataSourceIds())
    {
        medAbstractDbController *controller = medDataManager::instance()->controllerForDataSource(datasource);

        IndexList prevPatients = controller->patients();

        //inserting at least 2 patients in the db
        if(prevPatients.size()<2)
        {
            for(int i=prevPatients.size(); i<2; i++)
            {
                if( controller->isPersistent())
                    testImport();
                else testImportNonPersistent();
            }
        }

        prevPatients = controller->patients();

        IndexList prevPatient1Studies = controller->studies(prevPatients[0]);
        IndexList prevPatient2Studies = controller->studies(prevPatients[1]);

        medDataIndex study1 = prevPatient1Studies[0];

        IndexList newIndexes = medDataManager::instance()->moveStudy(study1, prevPatients[1]);

        IndexList patient1Studies = controller->studies(prevPatients[0]);
        IndexList patient2Studies = controller->studies(prevPatients[1]);

        QCOMPARE( patient2Studies.size(), prevPatient2Studies.size()+1 );
        QCOMPARE( patient1Studies.size(), prevPatient1Studies.size()-1 );
    }
    
}

void medDataManagerTestObject::testMoveSerie(void)
{
    //let's clean up everything
    testRemoveData();
    
    IndexList testPatients = db->patients();
    IndexList testPatients1 = npDb->patients();
    
    foreach(int datasource, medDataManager::instance()->dataSourceIds())
    {
        medAbstractDbController *controller = medDataManager::instance()->controllerForDataSource(datasource);

        IndexList prevPatients = controller->patients();

        //inserting at least 2 patients in the db
        if(prevPatients.size()<2)
        {
            for(int i=prevPatients.size(); i<2; i++)
            {
                if( controller->isPersistent())
                    testImport();
                else testImportNonPersistent();
            }
        }

        prevPatients = controller->patients();

        IndexList prevPatient1Studies = controller->studies(prevPatients[0]);
        IndexList prevPatient2Studies = controller->studies(prevPatients[1]);

        medDataIndex study1 = prevPatient1Studies[0];
        medDataIndex serie1 =  controller->series(study1)[0];
        medDataIndex study2 = prevPatient2Studies[0];
        
        IndexList prevStudy1Series = controller->series(study1);
        IndexList prevStudy2Series = controller->series(study2);

        dtkSmartPointer<medAbstractData> originalData = medDataManager::instance()->data( serie1 );
        QString originalPatientName = medMetaDataKeys::PatientName.getFirstValue(originalData);
        
        medDataIndex newIndex = medDataManager::instance()->moveSerie(serie1, study2);

        IndexList study1Series = controller->series(study1);
        IndexList study2Series = controller->series(study2);

        QCOMPARE( study2Series.size(), prevStudy2Series.size()+1 );
        QCOMPARE( study1Series.size(), prevStudy1Series.size()-1 );
        
        QVERIFY (newIndex.isValid());
        
        dtkSmartPointer<medAbstractData> movedData = medDataManager::instance()->data( newIndex );
        QString newPatientName = medMetaDataKeys::PatientName.getFirstValue(movedData);
        
        compareData(originalData, movedData);
        
    } 
    
    //test to see if everything is deleted properly
    testRemoveData();
}
   
   
void medDataManagerTestObject::onDataAdded(const medDataIndex& index)
{ 
    if(index.isValidForSeries())
    {
        m_lastInsertedIndex = index;
        qDebug() << "A serie has been inserted.";
        emit dataAdded();
    }
    else qDebug() << "patient or study inserted";
}

void medDataManagerTestObject::onDataRemoved(const medDataIndex& index)
{ 
    if(index.isValidForSeries())
    {
        qDebug() << "A serie has been removed.";
    }
    else qDebug() << "patient or study removed";
    
    emit dataRemoved();
}

bool medDataManagerTestObject::removeDir(const QString & dirName)
{
    bool result = false;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

void medDataManagerTestObject::compareData()
{
    dtkSmartPointer<medAbstractData> insertedData = medDataManager::instance()->data( m_lastInsertedIndex );
    QVERIFY(insertedData);
    QVERIFY(m_currentData);
    QCOMPARE(insertedData->identifier(), m_currentData->identifier());
    QCOMPARE(medMetaDataKeys::PatientName.getFirstValue(insertedData),
        medMetaDataKeys::PatientName.getFirstValue(m_currentData));
    QCOMPARE(medMetaDataKeys::StudyDescription.getFirstValue(insertedData),
        medMetaDataKeys::StudyDescription.getFirstValue(m_currentData));
    QCOMPARE(medMetaDataKeys::SeriesDescription.getFirstValue(insertedData), 
        medMetaDataKeys::SeriesDescription.getFirstValue(m_currentData));      
}

void medDataManagerTestObject::compareData(dtkSmartPointer<medAbstractData> data1, dtkSmartPointer<medAbstractData> data2)
{
    QVERIFY(data1);
    QVERIFY(data2);
    QCOMPARE(data1->identifier(), data2->identifier());
    QCOMPARE(medMetaDataKeys::PatientName.getFirstValue(data1),
        medMetaDataKeys::PatientName.getFirstValue(data2));
    QCOMPARE(medMetaDataKeys::StudyDescription.getFirstValue(data1), 
        medMetaDataKeys::StudyDescription.getFirstValue(data2));
    QCOMPARE(medMetaDataKeys::SeriesDescription.getFirstValue(data1), 
        medMetaDataKeys::SeriesDescription.getFirstValue(data2)); 
}

void medDataManagerTestObject::waitForInsertions(int numberOfInsertions, int timeout )
{
    m_insertedIndexes.clear();
    QSignalSpy spy1 (medDataManager::instance(), SIGNAL(dataAdded(const medDataIndex&)));
    
    QTimer timer1, timer2;
    timer1.setSingleShot(true);
    timer1.start(timeout);
    
    connect(&timer2, SIGNAL(timeout()), &m_eventLoop, SLOT(quit()));
    
    while( spy1.count() < numberOfInsertions && timer1.isActive() )
    {
        timer2.start(5000);
        m_eventLoop.exec();
        m_insertedIndexes.append(m_lastInsertedIndex);
    }
    
    //TODO: in some cases it seems that there are more signals, need to check that
    QVERIFY(spy1.count() >= numberOfInsertions);  
}

void medDataManagerTestObject::waitForDeletions(int numberOfDeletions, int timeout )
{
    m_insertedIndexes.clear();
    QSignalSpy spy1 (medDataManager::instance(), SIGNAL(dataRemoved(const medDataIndex&)));
    
    QTimer timer1, timer2;
    timer1.setSingleShot(true);
    timer1.start(timeout);
    
    connect(&timer2, SIGNAL(timeout()), &m_eventLoop, SLOT(quit()));
    
    while( spy1.count() < numberOfDeletions && timer1.isActive() )
    {
        timer2.start(5000);
        m_eventLoop.exec();
    }
    
    QVERIFY(spy1.count() >= numberOfDeletions);
}

DTKTEST_MAIN(medDataManagerTest,medDataManagerTestObject)
