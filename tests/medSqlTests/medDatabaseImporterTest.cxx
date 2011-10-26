#include <QtTest/QtTest>

// file configured by cmake
// with required paths
#include "medSqlTest.config.h"

#include "medSqlHelpers.cxx"

#include <dtkCore/dtkPluginManager.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseImporter.h>
#include <medCore/medStorage.h>
#include <medCore/medDataIndex.h>



// we need these lines to be able to use the types
// for injecting data in the tests

typedef QMap<QString, int> VolumeToImageCountMap;
Q_DECLARE_METATYPE(VolumeToImageCountMap)

typedef QString (*GetOriginalFileName)(QString sessionName, int index);
Q_DECLARE_METATYPE(GetOriginalFileName)

void checkDirectoryHierarchyAndThumbnails(QString dbPath, QString patientName, QString studyName, VolumeToImageCountMap volumeMap);

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

     // load medinria plugins, we will need data types, readers and writers
     dtkPluginManager::instance()->setPath(pluginsPath);
     dtkPluginManager::instance()->initialize();

     // create temporary directory for new database
     this->dbPath = QDir::tempPath() + "/medinria-test-db";

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

// This function is specifically for the test data
// does not work with every Dicom, of course.
QString getOriginalFileName(QString sessionName, int index)
{
    QString end = "0000";
    QString ind = QString::number(index+1);

    QString ending = end.replace(4 - ind.size(), ind.size(), ind);

    return QString(MEDINRIA_TEST_DATA_ROOT) + "/OSIRIX/WRIX/WRIST RIGHT/" + sessionName + "/IM-0001-" + ending + ".dcm";
}

void medDatabaseImporterTest::import_data()
{
    // here we will prepare data for running "import" test
    // many times with different input

    QTest::addColumn<QString>("pathToFileOrDirectory"); // what we want to import
    QTest::addColumn<QString>("patientName");
    QTest::addColumn<QString>("studyName");
    QTest::addColumn<VolumeToImageCountMap>("volumes");
    QTest::addColumn<GetOriginalFileName>("getOriginalFileName");

    QString fileOrDirToImport;
    VolumeToImageCountMap volumes;

    // test with NRRD
    volumes.clear();
    volumes["I1"] = 64;
    fileOrDirToImport = QDir::separator() + QString("Nrrd") + QDir::separator() + QString("I1.nhdr");
    QTest::newRow("I1.nhdr") << fileOrDirToImport << "John Doe" << "EmptyStudy" << volumes;

    // test with NIfTI
    volumes.clear();
    volumes["1-session01"] = 26;
    fileOrDirToImport = QDir::separator() + QString("Nifti") + QDir::separator()+ QString("1-session01.nii");
    QTest::newRow("1-session01.nii") << fileOrDirToImport << "John Doe" << "EmptyStudy" << volumes;

    // test with Dicoms
    // Dicoms are a horrible thing to test, really, I hate Dicoms. I needed to say that. Thanks.
    volumes.clear();
    volumes["SCOUT AXIAL LG FOV RT."] = 7;
    volumes["STIR COR. RT."] = 12;
    volumes["T1 TSE COR RT."] = 12;
    fileOrDirToImport = QDir::separator() + QString("OSIRIX") + QDir::separator()+ QString("WRIX");
    QTest::newRow("WRIX") << fileOrDirToImport << "WRIX" << "WRIST^RIGHT" << volumes << &getOriginalFileName;

    // test dicoms with 4D volume (diffusion MRI)
    volumes.clear();
    volumes["e.t2/2mm/C"] = 35;
    volumes["e.t2/2mm/S"] = 17;
    volumes["e.t2/2mm/A"] = 35;
    volumes["DTI_TENSOR"] = 60;
    volumes["T1/3D/6.5ML GADOVIST"] = 85;
    fileOrDirToImport = QDir::separator() + QString("dicom3");
    QTest::newRow("dicom3") << fileOrDirToImport << "001" << "MR Gehirnschädel" << volumes;
}

void medDatabaseImporterTest::import()
{
    QFETCH(QString, pathToFileOrDirectory);
    QString fullPathToFileOrDirectory = dataPath + pathToFileOrDirectory;
    QFileInfo info(fullPathToFileOrDirectory);

    if(!info.exists())
        QFAIL("Test data does not exists.");

    bool indexWithoutCopying = false;
    // import
    medDatabaseImporter *importer = new medDatabaseImporter(info.absoluteFilePath(), indexWithoutCopying);
    importer->run();

    /* CHECK
    * We won't do a full and exhaustive check in here, we will
    * see if the appropriate folders were created, we will
    * check thumbnails image files (only by name) and then we
    * will take a look at the database tables.
    */

    QFETCH(QString, patientName);
    QFETCH(QString, studyName);
    QFETCH(VolumeToImageCountMap, volumes);

    checkDirectoryHierarchyAndThumbnails(this->dbPath, patientName, studyName, volumes);

    QPointer<medDatabaseControllerImpl> dbc = medDatabaseController::instance();
    QSqlDatabase db = *(dbc->database());

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
    QMapIterator<QString, int> i(volumes);

    int totalAmountOfImages = 0;
    int index = -1;
    // now for each volume we need to check the images and the session
    while (i.hasNext())
    {
        i.next();
        index++;

        sessionName = i.key(); // name of this session
        imageCount = i.value(); // amount of images in this session

        totalAmountOfImages += imageCount;

        // check that the proper path is in series table
        QString seriesPath = QDir::separator() + patientName + QDir::separator()  + studyName + QDir::separator() + sessionName + QString::number(index+1) + QString(".mha");

        QVERIFY(medSqlHelpers::checkIfRowExists(db, "series", "path", seriesPath));

        // check every row inside image table
        for (int i = 0; i < imageCount ; i++)
        {
            QHash<QString, QString> columnValues;
            columnValues.insert("instance_path", seriesPath);
            QString thumbnailPath = QDir::separator() + patientName + QDir::separator()  + studyName + QDir::separator() + sessionName + QString::number(index+1) + QDir::separator() + QString::number(i) + ".png";
            columnValues.insert("thumbnail", thumbnailPath);

            // if we are importing a directory then is WRIX
            if(info.isDir())
            {
                QFETCH(GetOriginalFileName, getOriginalFileName);
                QString origFileName = getOriginalFileName(sessionName, i);

                // if we are importing just a file then the name if each image
                // is the filename plus and index
                columnValues.insert("name", origFileName.right(16));
                columnValues.insert("path", origFileName);
            }
            else
            {
                columnValues.insert("name", info.fileName() + QString::number(i));
                columnValues.insert("path", fullPathToFileOrDirectory);
            }

            QVERIFY(medSqlHelpers::checkIfRowExists(db, "image", columnValues));
        }
    }
    // instead of writing a new function we count the total amount of images, it's easier
    QVERIFY(totalAmountOfImages == medSqlHelpers::countRowsInTable(db, "image"));
}

void checkDirectoryHierarchyAndThumbnails(QString dbPath, QString patientName, QString studyName, VolumeToImageCountMap volumeMap)
{
    // We do not check the content of the files
    // we just see whether they exist

    // Patient
    QString patientPath = dbPath + QDir::separator() + patientName;
    if (QDir(patientPath).exists())
    {
        QString studyPath = patientPath + QDir::separator() + studyName;
        if (QDir(studyPath).exists())
        {
            QString sessionName;
            int thumbnailsCount;
            QMapIterator<QString, int> i(volumeMap);

            int index = -1;
            // now for each volume we need to check the thumbnails and the aggregated file
            while (i.hasNext())
            {
                i.next();
                index++;

                sessionName = i.key(); // name of this session
                thumbnailsCount = i.value(); // amount of images in this session

                QString sessionPath = studyPath + QDir::separator() + sessionName + QString::number(index+1);
                QString sessionFile = sessionPath + QString(".mha");

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
            QFAIL("Study path does not exist.");
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
