#include <QtTest/QtTest>

// file configured by cmake
// with required paths
#include "medSqlTest.config.h"

#include <dtkCore/dtkPluginManager.h>

#include <medSql/medDatabaseController.h>
#include <medSql/medDatabaseImporter.h>
#include <medCore/medStorage.h>
#include <medCore/medDataIndex.h>

void checkDirectoryHierarchyAndThumbnails(QString dbPath, QString patientName, QString studyName, QString sessionName, unsigned int thumbnailsCount);
int countRowsInTable(QSqlDatabase db, QString tableName);
bool checkIfRowExists(QSqlDatabase db, QString tableName, QString columnName, QString value);
bool checkIfRowExists(QSqlDatabase db, QString tableName, QHash<QString, QString> columnValues);

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
    // in this case "importSingleVolume_data()" will prepare some data
    // to run "importSingleVolume()" different times
    // with different input values
    void importSingleVolume_data();
    void importSingleVolume();
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

void medDatabaseImporterTest::importSingleVolume_data()
{
    // here we will prepare data for running "importSingleVolume" test
    // many times with different input

    QTest::addColumn<QString>("pathToFileOrDirectory"); // what we want to import
    QTest::addColumn<QString>("patientName");
    QTest::addColumn<QString>("studyName");
    QTest::addColumn<QString>("sessionName");
    QTest::addColumn<int>("imageCount"); // number of images inside the single volume

    // if we are importing a directory this helps
    // building the filenames inside (e.g. prefix+023+suffix = cta_022.dcm)
    QTest::addColumn<QString>("filesPrefix");
    QTest::addColumn<QString>("filesSuffix");

    QString fileOrDirToImport;

    // test with NRRD
    fileOrDirToImport = QDir::separator() + QString("Clatz") + QDir::separator() + QString("I1.nhdr");
    QTest::newRow("I1.nhdr") << fileOrDirToImport << "John Doe" << "EmptyStudy" << "I11" << 64 << "" << "";

    // test with NIfTI
    fileOrDirToImport = QDir::separator() + QString("mask") + QDir::separator()+ QString("1-session01.nii");
    QTest::newRow("1-session01.nii") << fileOrDirToImport << "John Doe" << "EmptyStudy" << "1-session011" << 26 << "" << "";

    // test with a directory full of DICOMs
    fileOrDirToImport = QDir::separator() + QString("Dicoms") + QDir::separator()+ QString("CTA");
    QTest::newRow("Dicoms-CTA") << fileOrDirToImport << "Anonymized-JA." << "Anonymized with DicomWorks" << "Anonymized with DicomWorks1" << 202 << "CT_" << ".dcm";
}

void medDatabaseImporterTest::importSingleVolume()
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
    QFETCH(QString, sessionName);
    QFETCH(int, imageCount);

    checkDirectoryHierarchyAndThumbnails(this->dbPath, patientName, studyName, sessionName, imageCount);

    QPointer<medDatabaseControllerImpl> dbc = medDatabaseController::instance();
    QSqlDatabase db = *(dbc->database());

    // check that there is only one row in patient table
    QVERIFY(1 == countRowsInTable(db, "patient"));

    // check that patient named patientName exists
    // we do not check for all the other columns
    QVERIFY(checkIfRowExists(db, "patient", "name", patientName));

    // check that there is only one row in study table
    QVERIFY(1 == countRowsInTable(db, "study"));

    // check that there is a study named studyName
    // we do not check for all the other columns
    QVERIFY(checkIfRowExists(db, "study", "name", studyName));

    // check that there is only one row in series table
    QVERIFY(1 == countRowsInTable(db, "series"));

    // check that the proper path is in series table
    QString seriesPath = QDir::separator() + patientName + QDir::separator()  + studyName + QDir::separator() + sessionName + QString(".mha");

    QVERIFY(checkIfRowExists(db, "series", "path", seriesPath));

    // check that there are #imageCount rows in image table
    QVERIFY(imageCount == countRowsInTable(db, "image"));

    QFETCH(QString, filesPrefix);
    QFETCH(QString, filesSuffix);
    // check every row inside image table
    for (int i = 0; i < imageCount ; i++)
    {
       QHash<QString, QString> columnValues;
       columnValues.insert("instance_path", seriesPath);
       QString thumbnailPath = QDir::separator() + patientName + QDir::separator()  + studyName + QDir::separator() + sessionName + QDir::separator() + QString::number(i) + ".png";
       columnValues.insert("thumbnail", thumbnailPath);

       QString fileName = info.fileName();
       if(info.isDir())
       {
           // if we are importing a directory with many files
           // we will use prefix and suffix to build the proper file name
           // note that we know this becaue we know the files we are importing
           QString originalFileName = filesPrefix + QString::number(i) + filesSuffix;
           columnValues.insert("name", originalFileName);
           columnValues.insert("path", fullPathToFileOrDirectory + QDir::separator() + originalFileName);
       }
       else
       {
           // if we are importing just a file then the name if each image
           // is the filename plus and index
           columnValues.insert("name", fileName + QString::number(i));
           columnValues.insert("path", fullPathToFileOrDirectory);
       }

       QVERIFY(checkIfRowExists(db, "image", columnValues));
    }
}

void checkDirectoryHierarchyAndThumbnails(QString dbPath, QString patientName, QString studyName, QString sessionName, unsigned int thumbnailsCount)
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
            QString sessionPath = studyPath + QDir::separator() + sessionName;
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

int countRowsInTable(QSqlDatabase db, QString tableName)
{
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) FROM " + tableName);

    if(!query.exec())
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        return -1;
    }

    if(query.first()) {
        QVariant rowsCount = query.value(0);
        return rowsCount.toInt();
    }
    else
        return -1;
}

bool checkIfRowExists(QSqlDatabase db, QString tableName, QString columnName, QString value)
{
    QSqlQuery query(db);

    query.prepare("SELECT id FROM " + tableName + " WHERE " + columnName + " = :value");
    query.bindValue(":value", value);

    if(!query.exec())
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        return false;
    }

    if(query.first()) {
        QVariant id = query.value(0);
        return true;
    }
    else
        return false;
}

bool checkIfRowExists(QSqlDatabase db, QString tableName, QHash<QString, QString> columnValues)
{
    // TODO this way of building the query string is just awful
    // try to find another...

    QString queryString = "SELECT id FROM " + tableName + " WHERE ";

    QHashIterator<QString, QString> it(columnValues);
    bool firstTime = true;

    while (it.hasNext())
    {
        it.next();

        if (!firstTime)
        {
            queryString.append(" AND ");
        }
        else
            firstTime = false;

        queryString.append(it.key()).append( " = :").append(it.key());
    }

    QSqlQuery query(db);
    query.prepare(queryString);

    it.toFront();

    while (it.hasNext())
    {
        it.next();
        query.bindValue(":"+it.key(), it.value());
    }

    if(!query.exec())
    {
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;
        return false;
    }


    // see executed query
    //qDebug() << query.executedQuery().toStdString().c_str();

    if(query.first()) {
        QVariant id = query.value(0);
        return true;
    }
    else
        return false;
}


QTEST_MAIN(medDatabaseImporterTest)
#include "moc_medDatabaseImporterTest.cxx"
