#include <QtTest/QtTest>
#include <QSignalSpy>

#include <medAPHP.h>
#include <PluginAPHP/QtDcmAPHP.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    medAPHP m = medAPHP(new QtDcmAPHP()) ;
    m.connect(true);

    return a.exec();
}
//Q_DECLARE_METATYPE(medAbstractSource::eRequestStatus);
//
//class TestAPHP: public QObject
//{
//    Q_OBJECT
//    private slots:
//    void initTestCase()
//    {
//        _m = new medAPHP(new QtDcmAPHP());
//    };
//
//    void test_init_failed_if_instance_id_empty()
//    {
//        QCOMPARE(_m->initialization(""), false);
//    }
//
//    void test_init_success_if_instance_id_not_empty()
//    {
//        QString instanceId = "instanceId";
//        QCOMPARE(_m->initialization(instanceId), true);
//    }
//
//    void test_set_instance_name_failed_if_instance_id_empty()
//    {
//        QCOMPARE(_m->setInstanceName(""), false);
//    }
//
//    void test_set_instance_name_success_if_instance_id_not_empty()
//    {
//        QString instanceName = "instanceName";
//        QCOMPARE(_m->setInstanceName(instanceName), true);
//    }
//
//    // WARN This test can work only with SPHERE deployed (AET : SPHERE hostname : localhost port : 11112
//    void test_integration_connect_success()
//    {
//        _m->connect(true);
////        qRegisterMetaType<medAbstractSource::eRequestStatus>("eRequestStatus");
////        QSignalSpy spy(_m, SIGNAL(progress(int, eRequestStatus)));
////        QCOMPARE(_m->connect(true), true);
////
////        QCOMPARE(spy.count(), 1);
////        auto requestStatus = qvariant_cast<medAbstractSource::eRequestStatus>(spy.at(0).at(1));
////        QCOMPARE(requestStatus, medAbstractSource::finish );
////        QVERIFY(spy.at(0).at(0) == 0);
//    }
//
////    void cleanupTestCase();
//private:
//    medAPHP *_m;
//};
//
//
//
//QTEST_MAIN(TestAPHP)
//
//#include "medAPHPQTest.moc"