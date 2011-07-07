#include <QtTest/QtTest>

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

    // every function here is a test
    void importNhdr();
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
    QPointer<medDatabaseControllerImpl> dbc = medDatabaseController::instance();

    // create connection and create database schema
    if(!dbc->createConnection())
        QFAIL("Unable to create a connection to the database");
}

void medDatabaseImporterTest::cleanup()
{
    //clean
    medStorage::removeDir(dbPath, NULL);
    medDatabaseController::destroy();
}


void medDatabaseImporterTest::cleanupTestCase()
{
    qDebug() << "cleanupTestCase()";
}

void medDatabaseImporterTest::importNhdr()
{
    QString file = dataPath + QString("/Clatz/I1.nhdr");
    QFileInfo info(file);
    bool indexWithoutCopying = false;

    // import
    medDatabaseImporter *importer = new medDatabaseImporter(info.absoluteFilePath(), indexWithoutCopying);
    importer->run();

    /* CHECK
    * We won't do a full and exhaustive check in here, we will
    * see if the appropriate folders were created, we will
    * check thumbnails image files (only by name) and then we
    * will take a deep look at the database tables.
    */

    QString patientName = "John Doe";
    QString studyName = "EmptyStudy";
    QString sessionName = "I11";

    checkDirectoryHierarchyAndThumbnails(this->dbPath, patientName, studyName, sessionName, 63);

    // super, so we reached a point that all the files we expect are there
    // however we didn't check the content, but still...
    // now onto db check

    QPointer<medDatabaseControllerImpl> dbc = medDatabaseController::instance();
    QSqlDatabase db = *(dbc->database());

    // check that there is only one row in patient table
    QVERIFY(1 == countRowsInTable(db, "patient"));

    // check that patient named John Doe exists
    // we do not check for all the other columns
    QVERIFY(checkIfRowExists(db, "patient", "name", patientName));

    // check that there is only one row in study table
    QVERIFY(1 == countRowsInTable(db, "study"));

    // check that there is a study named EmptyStudy
    // we do not check for all the other columns
    QVERIFY(checkIfRowExists(db, "study", "name", studyName));

    // check that there is only one row in series table
    QVERIFY(1 == countRowsInTable(db, "series"));

    // check that the proper path is in series table
    QString seriesPath = QDir::separator() + patientName + QDir::separator()  + studyName + QDir::separator() + sessionName + QString(".mha");

    QVERIFY(checkIfRowExists(db, "series", "path", seriesPath));

    int imageCount = 64;

    // check that there are #imageCount rows in image table
    QVERIFY(imageCount == countRowsInTable(db, "image"));

    // check every row
    for (int i = 0; i < imageCount ; i++)
    {
       QHash<QString, QString> columnValues;
       columnValues.insert("name", "I1.nhdr" + QString::number(i));
       columnValues.insert("path", file);
       columnValues.insert("instance_path", seriesPath);
       QString thumbnailPath = QDir::separator() + patientName + QDir::separator()  + studyName + QDir::separator() + sessionName + QDir::separator() + QString::number(i) + ".png";
       columnValues.insert("thumbnail", thumbnailPath);

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
                for (unsigned int i = 0; i <= thumbnailsCount; i++) {
                    QFile thumbnail(sessionPath + QDir::separator() + QString::number(i) + QString(".png"));
                    if (!thumbnail.exists())
                        QFAIL("At least one thumbnail is missing.");
                }

                // finally check ref.png
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
