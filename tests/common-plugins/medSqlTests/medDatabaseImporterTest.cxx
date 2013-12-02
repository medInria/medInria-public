/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtTest/QtTest>

// file configured by cmake with required paths
#include "medSqlTest.config.h"

#include "medSqlHelpers.cxx"

#include <dtkCore/dtkPluginManager.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseImporter.h>
#include <medCore/medStorage.h>
#include <medCore/medDataIndex.h>


// we need these lines to be able to use the types
// for injecting data in the tests

typedef QMultiMap<QString, int> VolumeNameToImagesCountMap;
Q_DECLARE_METATYPE(VolumeNameToImagesCountMap)

void checkDirectoryHierarchyAndThumbnails(QSqlDatabase db, QString dbPath, QString patientName, QString importedFileExtension, VolumeNameToImagesCountMap volumeMap);

void importImagesAndCheckIntegrity(QString pathToImport, QString dbPath, QString patientName,QString studyName,QString importedFileExtension, VolumeNameToImagesCountMap volumes);

class medDatabaseImporterTest: public QObject
{
    Q_OBJECT

private:
    QString dataPath;
    QString dbPath;

private slots:

    /*
     * initTestCase() is called before the
     * execution of all the test cases.
     * If it fails, no test is executed.
     */
    void initTestCase();

    /*
     * init() is called before each test.
     * If it fails, the following
     * test is not executed.
     */
    void init();

    /*
     * cleanup() is called after each test.
     */
    void cleanup();

    /*
     * cleanupTestCase() is called
     * after all test have been executed.
     */
    void cleanupTestCase();


private slots:

    // every function here is a test, unless they end with "_data"
    // in this case "import_data()" will prepare some data
    // to run "import()" different times
    // with different input values
    void import_data();
    void import();

    // combinations of partial Import and Index
    // a partial import is when you try to import images
    // belonging to the same volume in 2 separate steps
    void partialImportOrIndex_data();
    void partialImportOrIndex();
};

void medDatabaseImporterTest::initTestCase()
{
    QString pluginsPath = MEDINRIA_PLUGINS_PATH;
    QDir pluginsDir(pluginsPath);
    if (!pluginsDir.exists())
        QFAIL("Medinria plugins are required to run this test.");

    this->dataPath = MEDINRIA_TEST_DATA_ROOT;
    QDir dataDir(this->dataPath);
    if (!dataDir.exists())
        QFAIL("Medinria data directory is required to run this test.");

     // load medInria plugins, we will need data types, readers and writers
     dtkPluginManager::instance()->setPath(pluginsPath);
     dtkPluginManager::instance()->initialize();

     // create temporary directory for new database
     this->dbPath = QDir::tempPath() + "/medInria-test-db";

     medStorage::setDataLocation(this->dbPath);
}

void medDatabaseImporterTest::init()
{
    // create one new clean database for each test
    QPointer<medDatabaseControllerImpl> dbc = medDatabaseController::instance();

    // create connection and create database schema
    if(!dbc->createConnection())
        QFAIL("Unable to create a connection to the database");
}

void medDatabaseImporterTest::cleanup()
{
    // clean objects and destroy database after each test
    // a new database is going to be used
    medDatabaseController::instance()->closeConnection();
    medDatabaseController::destroy();
    medStorage::removeDir(dbPath, NULL);
}

void medDatabaseImporterTest::cleanupTestCase()
{
    //qDebug() << "cleanupTestCase()";
}

void medDatabaseImporterTest::import_data()
{
    // here we will prepare data for running "import" test
    // many times with different input

    QTest::addColumn<QString>("pathToFileOrDirectory"); // what we want to import
    QTest::addColumn<QString>("patientName");
    QTest::addColumn<QString>("studyName");
    QTest::addColumn<QString>("importedFileExtension");
    QTest::addColumn<VolumeNameToImagesCountMap>("volumes");

    QString fileOrDirToImport;
    VolumeNameToImagesCountMap volumes;

    //test with NRRD
    volumes.clear();
    volumes.insert("I1",64);
    fileOrDirToImport = QDir::separator() + QString("Nrrd") + QDir::separator() + QString("I1.nhdr");
    QTest::newRow("I1.nhdr") << fileOrDirToImport << "John Doe" << "EmptyStudy" << "mha" << volumes;

    // test with NIfTI
    volumes.clear();
    volumes.insert("1-session01",26);
    fileOrDirToImport = QDir::separator() + QString("Nifti") + QDir::separator()+ QString("1-session01.nii");
    QTest::newRow("1-session01.nii") << fileOrDirToImport << "John Doe" << "EmptyStudy" << "mha" << volumes;

    // test with Dicoms
    // Dicoms are a horrible thing to test, really, I hate Dicoms. I needed to say that. Thanks.
    volumes.clear();
    volumes.insert("SCOUT AXIAL LG FOV RT.",7);
    volumes.insert("STIR COR. RT.",12);
    volumes.insert("T1 TSE COR RT.",12);
    fileOrDirToImport = QDir::separator() + QString("OSIRIX") + QDir::separator()+ QString("WRIX");
    QTest::newRow("WRIX") << fileOrDirToImport << "WRIX" << "WRIST^RIGHT" << "mha" << volumes;

    // test dicoms with 4D volume (diffusion MRI)
    volumes.clear();
    volumes.insert("e.t2/2mm/C", 35);
    volumes.insert("e.t2/2mm/S", 17);
    volumes.insert("e.t2/2mm/A", 35);
    volumes.insert("DTI_TENSOR", 2040); // 60 is the series 'size', but 2040 is the image count
    volumes.insert("T1/3D/6.5ML GADOVIST", 85);
    volumes.insert(".survHead.", 7);
    volumes.insert(".survHead.", 3);
    volumes.insert(".survHead.", 3);
    fileOrDirToImport = QDir::separator() + QString("dicom3");
    QTest::newRow("dicom3") << fileOrDirToImport << "001" << QString::fromUtf8("MR Gehirnschädel") << "mha" << volumes;
}

void medDatabaseImporterTest::import()
{
    QFETCH(QString, pathToFileOrDirectory);
    QString fullPathToFileOrDirectory = dataPath + pathToFileOrDirectory;
    QFileInfo info(fullPathToFileOrDirectory);

    if(!info.exists())
        QFAIL("Test data does not exist.");

    QFETCH(QString, patientName);
    QFETCH(QString, studyName);
    QFETCH(QString, importedFileExtension);
    QFETCH(VolumeNameToImagesCountMap, volumes);

    importImagesAndCheckIntegrity(this->dbPath, info.absoluteFilePath(), patientName, studyName, importedFileExtension, volumes);
}

void importImagesAndCheckIntegrity(QString dbPath, QString pathToImport, QString patientName, QString studyName, QString importedFileExtension, VolumeNameToImagesCountMap volumes)
{
    bool indexWithoutCopying = false;
    // import
    medDatabaseImporter *importer = new medDatabaseImporter(pathToImport, indexWithoutCopying);
    importer->run();

    /* CHECK
    * We won't do a full and exhaustive check in here, we will
    * see if the appropriate folders were created, we will
    * check thumbnails image files (only by name) and then we
    * will take a look at the database tables.
    */

    QPointer<medDatabaseControllerImpl> dbc = medDatabaseController::instance();
    QSqlDatabase db = *(dbc->database());

    checkDirectoryHierarchyAndThumbnails(db, dbPath, patientName, importedFileExtension, volumes);

    // check that there is only one row in patient table
    QVERIFY(1 == medSqlHelpers::countRowsInTable(db, "patient"));

    // check that patient named patientName exists
    // we do not check for all the other columns
    QVERIFY(medSqlHelpers::checkIfRowExists(db, "patient", "name", patientName));

    // check that there is only one row in study table
    QVERIFY(1 == medSqlHelpers::countRowsInTable(db, "study"));

    // check that there is a study named studyName
    // we do not check for all the other columns
    QVERIFY(medSqlHelpers::checkIfRowExists(db, "study", "name", studyName));

    int volumeCount = volumes.size();
    // check that there is only #volumeCount rows in series table
    QVERIFY(volumeCount == medSqlHelpers::countRowsInTable(db, "series"));

    QString sessionName;
    int imageCount;

    // removing duplicates
    QSet<QString> keys = volumes.keys().toSet();

    int totalAmountOfImages = 0;
    // now for each volume we need to check the images and the series
    foreach( QString seriesName, keys)
    {
        QList<int> imagesCount = volumes.values(seriesName);

        QListIterator<int> i(imagesCount);
        while (i.hasNext())
            totalAmountOfImages += i.next();;

        // check there are that many rows in series table with the given name as indicated by the input
        QList<int> seriesIds = medSqlHelpers::getSeriesIdsWithName(db, seriesName);
        if(seriesIds.count() != imagesCount.count())
            QFAIL(QString("Should be exactly %1 rows in 'series' table with name %2").arg(imagesCount.count()).arg(seriesName).toLatin1());

        // now we need to check that for each one of these series ids
        // there must be in the image table as many rows as indicated in the input volumeNameToImagesCount map

        // let's say there are 7 images in image table associated with series id 34 and 14 with series id 153
        // and the input value seriesIds is (34, 153) then this function will return (7,14)
        QList<int> imagesCountForEachSeriesIds = medSqlHelpers::getImagesCountGroupedBySeriesIds(db, seriesIds);

        // we sort them so as it's easy to compare them by using QList == operator
        qSort(imagesCountForEachSeriesIds.begin(), imagesCountForEachSeriesIds.end());
        qSort(imagesCount.begin(), imagesCount.end());

        QVERIFY2(imagesCountForEachSeriesIds == imagesCount, "The amount of rows in the image table is of the one expected.");
    }
    // instead of writing a new function we count the total amount of images, it's easier
    QVERIFY(totalAmountOfImages == medSqlHelpers::countRowsInTable(db, "image"));
}

void checkDirectoryHierarchyAndThumbnails(QSqlDatabase db, QString dbPath, QString patientName,  QString importedFileExtension, VolumeNameToImagesCountMap volumeMap)
{
    // We do not check the content of the files
    // we just see whether they exist

    // Patient

    QString patientId = medSqlHelpers::getPatientId(db, patientName);

    QString patientPath = dbPath + QDir::separator() + patientId;
    if (QDir(patientPath).exists())
    {
        QString sessionName;
        int thumbnailsCount;
        QMapIterator<QString, int> i(volumeMap);

        int index = -1;
        // now for each volume we need to check the path, the thumbnails and the aggregated file
        while (i.hasNext())
        {
            i.next();
            index++;

            sessionName = i.key(); // name of this session
            thumbnailsCount = i.value(); // amount of images in this session

            QString sessionPath = dbPath + QDir::separator() + medSqlHelpers::getSeriesRelativePathFromThumbnail(db, sessionName);
            QString sessionFile = sessionPath + QString(".") + importedFileExtension;

            if (QDir(sessionPath).exists() && QFile(sessionFile).exists())
            {
                // check if thumbnails exist
                for (unsigned int i = 0; i <= thumbnailsCount-1; i++) {
                    QFile thumbnail(sessionPath + QDir::separator() + QString::number(i) + QString(".png"));
                    if (!thumbnail.exists())
                        QFAIL("At least one thumbnail is missing.");
                }

                // finally check reference thumbnail
                QFile ref(sessionPath + QDir::separator() + QString("ref.png"));
                if (!ref.exists())
                    QFAIL("Reference thumbnail is missing.");
            }
            else
            {
                QFAIL("Session path does not exist.");
            }
        }
    }
    else
    {
        QFAIL("Patient path does not exist.");
    }
}

void medDatabaseImporterTest::partialImportOrIndex_data()
{
    QString firstBatch = this->dataPath + "/OSIRIX/WRIX_PartialImport/WRIST RIGHT/SCOUT AXIAL LG FOV RT.1-4";
    QString secondBatch = this->dataPath + "/OSIRIX/WRIX_PartialImport/WRIST RIGHT/SCOUT AXIAL LG FOV RT.5-7";

    QFileInfo info1(firstBatch);
    QFileInfo info2(secondBatch);

    if(!info1.exists() || !info2.exists())
        QFAIL("Test data does not exists.");


    QTest::addColumn<QString>("firstPath");
    QTest::addColumn<QString>("secondPath");
    QTest::addColumn<bool>("isIndexFirst");
    QTest::addColumn<bool>("isIndexSecond");

    QTest::newRow("IndexIndex") << firstBatch << secondBatch << true << true;
    QTest::newRow("IndexImport") << firstBatch << secondBatch << true << false;
    QTest::newRow("ImportIndex") << firstBatch << secondBatch << false << true;
    QTest::newRow("ImportImport") << firstBatch << secondBatch << false << false;
}

void medDatabaseImporterTest::partialImportOrIndex()
{
    QFETCH(QString, firstPath);
    QFETCH(QString, secondPath);
    QFETCH(bool, isIndexFirst);
    QFETCH(bool, isIndexSecond);

    medDatabaseImporter *importer1 = new medDatabaseImporter(firstPath, isIndexFirst);
    importer1->run();

    medDatabaseImporter *importer2 = new medDatabaseImporter(secondPath, isIndexSecond);

    QSignalSpy spy(importer2, SIGNAL(partialImportAttempted(const QString&)));

    importer2->run();

    QVERIFY2(1 == spy.count(), "No error signal emitted.");
}

#undef QT_GUI_LIB
QTEST_MAIN(medDatabaseImporterTest)
#include "moc_medDatabaseImporterTest.cxx"
