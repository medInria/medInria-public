#include <QtTest/QtTest>
#include <QSignalSpy>

#include <medAPHP.h>
#include <QtDcmAPHP.h>
#include <sphereDicomWeb/medAnnotation.h>
#include <medGroupParameter.h>
#include <medStringParameter.h>
#include <medIntParameter.h>

class TestmedAPHP: public QObject
{
    Q_OBJECT
    private slots:
    void initTestCase()
    {
        _m = new medAPHP(new QtDcmAPHP(), new medAnnotation());
        QList<medAbstractParameter*> params = _m->getAllParameters();
        medGroupParameter *localSettings = dynamic_cast<medGroupParameter*>(params[0]);
        QList<medAbstractParameter*> localP = localSettings->value();
        medStringParameter *aetTitle = dynamic_cast<medStringParameter*>(localP[0]);
        aetTitle->setValue("MEDINRIA");
        medStringParameter *host = dynamic_cast<medStringParameter*>(localP[1]);
        host->setValue("localhost");
        medIntParameter *port = dynamic_cast<medIntParameter*>(localP[2]);
        port->setValue(2010);

        medGroupParameter *remoteSettings = dynamic_cast<medGroupParameter*>(params[1]);
        QList<medAbstractParameter*> remoteP = remoteSettings->value();
        medStringParameter *serverAetTitle = dynamic_cast<medStringParameter*>(remoteP[0]);
        serverAetTitle->setValue("SERVER");
        medStringParameter *sHost = dynamic_cast<medStringParameter*>(remoteP[1]);
        sHost->setValue("localhost");
        medIntParameter *sPort = dynamic_cast<medIntParameter*>(remoteP[2]);
        sPort->setValue(11112);
        medGroupParameter *restFulSettings = dynamic_cast<medGroupParameter*>(params[2]);
        medStringParameter *url = dynamic_cast<medStringParameter*>(restFulSettings->value()[0]);
        url->setValue("http://127.0.0.1:5555");

    };

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

    // WARN This test can work only with SPHERE deployed (AET : SPHERE hostname : localhost port : 11112
    void test_integration_connect_success()
    {
        QCOMPARE(_m->connect(true), true);
    }

    void test_integration_get_patient_minimal_entries_success()
    {
        QList<medAbstractSource::levelMinimalEntries> entries = _m->getMinimalEntries(0, "key");
        QVERIFY(entries.size() > 0);
        for (const auto& entry : entries)
        {
            qDebug()<<"entry : "<<entry.key<<" "<<entry.name<<" "<<entry.description;
            QVERIFY(!entry.key.isEmpty());
            QVERIFY(!entry.name.isEmpty());
            QVERIFY(!entry.description.isEmpty());
        }
    }

    void test_integration_get_study_minimal_entries_success()
    {
        // Warn : We need to define a valid key ie. an existing patientID in conncted PACS
        QString key = "0000000000";
        QList<medAbstractSource::levelMinimalEntries> entries = _m->getMinimalEntries(1, key);
        QVERIFY(entries.size() > 0);
        for (const auto& entry : entries)
        {
            qDebug()<<"entry : "<<entry.key<<" "<<entry.name<<" "<<entry.description;
            QVERIFY(!entry.key.isEmpty());
            QVERIFY(!entry.name.isEmpty());
            QVERIFY(!entry.description.isEmpty());
        }
    }

    void test_integration_get_series_minimal_entries_success()
    {
        // Warn : We need to define a valid key ie. an existing studyInstanceUID in conncted PACS
        QString key = "1.3.6.1.4.1.14519.5.2.1.3671.4754.288848219213026850354055725664";
        QList<medAbstractSource::levelMinimalEntries> entries = _m->getMinimalEntries(2, key);
        QVERIFY(entries.size() > 0);
        for (const auto& entry : entries)
        {
            qDebug()<<"entry : "<<entry.key<<" "<<entry.name<<" "<<entry.description;
            QVERIFY(!entry.key.isEmpty());
            QVERIFY(!entry.name.isEmpty());
            QVERIFY(!entry.description.isEmpty());
        }
    }

    void test_integration_get_annotation_data_success()
    {
        // Warn : We need to define a valid key ie. an existing studyInstanceUID in conncted PACS
        QString key = "1.2.826.0.1.3680043.2.1143.5929888294927686301021609009274607687";
        _m->getAssyncData(3, key);

    }

    void cleanupTestCase()
    {
        //delete _m;
    }
private:
    medAPHP *_m;
};



QTEST_MAIN(TestmedAPHP)

#include "medAPHPQTest.moc"