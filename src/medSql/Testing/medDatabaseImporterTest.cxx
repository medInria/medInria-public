#include <QtTest/QtTest>

class medDatabaseImporterTest: public QObject
{
    Q_OBJECT

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
    void toUpper();
};

void medDatabaseImporterTest::initTestCase()
{
    qDebug() << "initTestCase()";}

void medDatabaseImporterTest::init()
{
    qDebug() << "init()";
}

void medDatabaseImporterTest::cleanup()
{
    qDebug() << "cleanup()";
}


void medDatabaseImporterTest::cleanupTestCase()
{
    qDebug() << "cleanupTestCase()";
}

void medDatabaseImporterTest::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

QTEST_MAIN(medDatabaseImporterTest)
#include "moc_medDatabaseImporterTest.cxx"
