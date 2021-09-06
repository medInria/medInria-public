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
//signals:
//    void changeDbPath(const QString &updatedDbPath);

};
class TestMedSQLite: public QObject
{
    Q_OBJECT
    private slots:
    void getDirectDataLevelSeriesValidKeySuccess();

};

Q_DECLARE_METATYPE(medAbstractSource::eRequestStatus);

void TestMedSQLite::getDirectDataLevelSeriesValidKeySuccess()
{
    RealMedSQLite t = RealMedSQLite();
    qRegisterMetaType<medAbstractSource::eRequestStatus>("eRequestStatus");
    QSignalSpy spy(&t, SIGNAL(progress(int, eRequestStatus)));

    QString instanceId = "bar";
    QTemporaryDir dir;
    if (dir.isValid()) {
//        emit t.changeDbPath(dir.path());
//        t.changeDatabasePath(dir.path());

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
        QCOMPARE("/path/to/data1", t.getDirectData(2, key));

        QCOMPARE(spy.count(), 1);
        auto requestStatus = qvariant_cast<medAbstractSource::eRequestStatus>(spy.at(0).at(1));
        qDebug()<<"requestStatus "<<requestStatus;
        QCOMPARE(requestStatus, medAbstractSource::finish );
        QVERIFY(requestStatus == medAbstractSource::finish);
        QVERIFY(spy.at(0).at(0) == 10);

//        medAbstractSource::eRequestStatus result = qvariant_cast<medAbstractSource::eRequestStatus>(spy.at(1).at(0));
//        int id = qRegisterMetaType<medAbstractSource::eRequestStatus>("eRequestStatus");
//        qDebug()<<"tpe "<<arguments.at(0).type();
////        QVERIFY(arguments.at(0).type() == id); // verify the first argument
//        EXPECT_EQ("/path/to/data1", t.getDirectData(2, key));
        t.connect(false);
        dir.remove();
    }
}


QTEST_MAIN(TestMedSQLite)
#include "medSQLiteIntegrationQTest.moc"