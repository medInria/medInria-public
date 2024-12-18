#include <QtTest/QtTest>
#include <QSqlDatabase>
#include <QSignalSpy>

#define private public
#include "medSQLite.h"



class QSqlDatabase;

class RealMedSQLite : public medSQlite<QSqlDatabase>
{
public:
    RealMedSQLite()
    {
//        QObject::connect(this, SIGNAL(changeDbPath(const QString &)), this, SLOT(updateDatabaseName(const QString &)));

    }
    void changeDatabasePath(QString path)
    {
        m_DbPath->setValue(path);
    }
//signals:
//    void changeDbPath(const QString &updatedDbPath);

};
class TestMedSQLite: public QObject
{
    Q_OBJECT
    private slots:
    void getDirectDataLevelSeriesValidKeySuccess();
    void addDataThenGetGetMandatoryAndAdditionalLevelPatientSuccess();

};

Q_DECLARE_METATYPE(medAbstractSource::eRequestStatus);

void TestMedSQLite::getDirectDataLevelSeriesValidKeySuccess()
{
    RealMedSQLite t = RealMedSQLite();
    QString instanceId = "bar";
    QTemporaryDir dir;
    if (dir.isValid()) {
        t.changeDatabasePath(dir.path());

        // expectations
        t.initialization(instanceId);
        t.connect(true);
        QSqlQuery query = t.m_Engine.exec();
        QList<QString> ids = {"10", "20", "30"};
        QList<QString> paths = {"/path/to/data1", "/path/to/data2", "/path/to/data3"};
        for (int i = 0; i<3; i++)
        {
            query.prepare("INSERT INTO series (id, path) "
                          "VALUES (:id, :path)");
            query.bindValue(":id", ids[i]);
            query.bindValue(":path", paths[i]);
            query.exec();
        }
        QString key = "10";
        QCOMPARE(QVariant(dir.path() + "/path/to/data1"), t.getDirectData(2, key));

        t.connect(false);
        dir.remove();
    }
}

void TestMedSQLite::addDataThenGetGetMandatoryAndAdditionalLevelPatientSuccess()
{
//    RealMedSQLite t = RealMedSQLite();
//    QString instanceId = "bar";
//    QTemporaryDir dir;
//    if (dir.isValid()) {
//        t.changeDatabasePath(dir.path());
//
//        // expectations
//        t.initialization(instanceId);
//        t.connect(true);
//        QString uri = "medSQlite666:";
//        QMap<QString, QString> mandatoryAttributes;
//        mandatoryAttributes[]
//        t.addData(QVariant(), uri, )
//        QSqlQuery query = t.m_Engine.exec();
//        QList<QString> ids = {"10", "20", "30"};
//        QList<QString> paths = {"/path/to/data1", "/path/to/data2", "/path/to/data3"};
//        for (int i = 0; i<3; i++)
//        {
//            query.prepare("INSERT INTO series (id, path) "
//                          "VALUES (:id, :path)");
//            query.bindValue(":id", ids[i]);
//            query.bindValue(":path", paths[i]);
//            query.exec();
//        }
//        QString key = "10";
//        QCOMPARE(QVariant(dir.path() + "/path/to/data1"), t.getDirectData(2, key));
//
//        t.connect(false);
//        dir.remove();
//    }
}

QTEST_MAIN(TestMedSQLite)
#include "medSQLiteIntegrationQTest.moc"