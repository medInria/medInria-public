/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "gtest/gtest.h"
#include "gmock/gmock.h"

//#include "medSQLite.h"
#include "medSQLiteTest.h"


class medRequestTest : public ::testing::Test
{

protected:
    ::testing::NiceMock<FakeMedSQLite> m_;

    medRequestTest()
    {
    }

    virtual ~medRequestTest()
    {
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};


TEST_F(medRequestTest, get_minimal_entries_failed_level_negative)
{
    QList<medAbstractSource::levelMinimalEntries> entries;
    entries = m_.getMinimalEntries(-1,"patient");
    ASSERT_EQ(0, entries.size());
}

TEST_F(medRequestTest, get_minimal_entries_failed_level_greater_than_level_count)
{
    QList<medAbstractSource::levelMinimalEntries> entries;
//    EXPECT_CALL(m_, getPatientMinimalEntries()).Times(0);
    entries = m_.getMinimalEntries(4,"patient");
    ASSERT_EQ(0, entries.size());
}

TEST_F(medRequestTest, get_minimal_entries_level_patient_call_getPatientMinimalEntries)
{
    QList<medAbstractSource::levelMinimalEntries> entries;
    QString id = "";
    EXPECT_CALL(m_, getPatientMinimalEntries(id)).Times(1);
    entries = m_.getMinimalEntries(0,id);
}

TEST_F(medRequestTest, get_minimal_entries_level_study_call_getStudyMinimalEntries)
{
    QList<medAbstractSource::levelMinimalEntries> entries;
    QString patientId = "1";
    EXPECT_CALL(m_, getStudyMinimalEntries(patientId)).Times(1);
    entries = m_.getMinimalEntries(1,patientId);
}

TEST_F(medRequestTest, get_minimal_entries_level_series_call_getSeriesMinimalEntries)
{
    QList<medAbstractSource::levelMinimalEntries> entries;
    QString studyId = "0";
    EXPECT_CALL(m_, getSeriesMinimalEntries(studyId)).Times(1);
    entries = m_.getMinimalEntries(2,studyId);
}

TEST(requestDBTest, get_minimal_entries_level_patient_integration)
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
        QList<QString> ids = {"1", "11", "12"};
        QList<QString> names = {"Maxime", "Florent", "Vicky"};
        QList<QString> patientIds = {"101", "103", "104"};
        for (int i = 0; i<3; i++)
        {
            query.prepare("INSERT INTO patient (id, name, patientId) "
                          "VALUES (:id, :name, :patientId)");
            query.bindValue(":id", ids[i]);
            query.bindValue(":name", names[i]);
            query.bindValue(":patientId", patientIds[i]);
            query.exec();
        }
        QList<medAbstractSource::levelMinimalEntries> entries;
        entries = t.getMinimalEntries(0,"");

        EXPECT_EQ(entries.size(), 3);
        int i = 0;
        for (auto entry : entries)
        {
            EXPECT_EQ(ids[i], entry.key);
            EXPECT_EQ(names[i], entry.name);
            EXPECT_EQ(patientIds[i++], entry.description);
        }
        t.connect(false);
        dir.remove();
    }
}

TEST(requestDBTest, get_minimal_entries_level_patient_invalid_key_succeed)
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
        QList<QString> ids = {"1", "11", "12"};
        QList<QString> names = {"Maxime", "Florent", "Vicky"};
        QList<QString> patientIds = {"101", "103", "104"};
        for (int i = 0; i<3; i++)
        {
            query.prepare("INSERT INTO patient (id, name, patientId) "
                          "VALUES (:id, :name, :patientId)");
            query.bindValue(":id", ids[i]);
            query.bindValue(":name", names[i]);
            query.bindValue(":patientId", patientIds[i]);
            query.exec();
        }
        QList<medAbstractSource::levelMinimalEntries> entries;
        entries = t.getMinimalEntries(0,"patient");

        EXPECT_EQ(entries.size(), 0);
//        int i = 0;
//        for (auto entry : entries)
//        {
//            EXPECT_EQ(ids[i], entry.key);
//            EXPECT_EQ(names[i], entry.name);
//            EXPECT_EQ(patientIds[i++], entry.description);
//        }
        t.connect(false);
        dir.remove();
    }
}

TEST(requestDBTest, get_minimal_entries_level_study_invalid_key_failed)
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
        QList<QString> patients = {"1", "1", "40"};
        QList<QString> names = {"Study10", "Study20", "Study30"};
        QList<QString> studyInstancesUIds = {"1.2.3.105.2454434", "1.2.3.105.2454435", "1.2.3.105.2454436"};
        for (int i = 0; i<3; i++)
        {
            query.prepare("INSERT INTO study (id, patient, name, uid) "
                          "VALUES (:id, :patient, :name, :studyInstanceUID)");
            query.bindValue(":id", ids[i]);
            query.bindValue(":patient", patients[i]);
            query.bindValue(":name", names[i]);
            query.bindValue(":studyInstanceUID", studyInstancesUIds[i]);
            query.exec();
        }
        QList<medAbstractSource::levelMinimalEntries> entries;
        QString patientId = "invalid_key";
        entries = t.getMinimalEntries(1,patientId);

        EXPECT_EQ(entries.size(), 0);
        t.connect(false);
        dir.remove();
    }
}

TEST(requestDBTest, get_minimal_entries_level_study_integration)
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
        QList<QString> patients = {"1", "1", "40"};
        QList<QString> names = {"Study10", "Study20", "Study30"};
        QList<QString> studyInstancesUIds = {"1.2.3.105.2454434", "1.2.3.105.2454435", "1.2.3.105.2454436"};
        for (int i = 0; i<3; i++)
        {
            query.prepare("INSERT INTO study (id, patient, name, uid) "
                          "VALUES (:id, :patient, :name, :studyInstanceUID)");
            query.bindValue(":id", ids[i]);
            query.bindValue(":patient", patients[i]);
            query.bindValue(":name", names[i]);
            query.bindValue(":studyInstanceUID", studyInstancesUIds[i]);
            query.exec();
        }
        QList<medAbstractSource::levelMinimalEntries> entries;
        QString patientId = "1";
        entries = t.getMinimalEntries(1,patientId);

        EXPECT_EQ(entries.size(), 2);
        int i = 0;
        for (auto entry : entries)
        {
            if (patients[i]==patientId)
            {
                EXPECT_EQ(ids[i], entry.key);
                EXPECT_EQ(names[i], entry.name);
                EXPECT_EQ(studyInstancesUIds[i++], entry.description);
            }
        }
        t.connect(false);
        dir.remove();
    }
}

TEST(requestDBTest, get_minimal_entries_level_series_integration)
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
        QList<QString> studies = {"2", "1", "40"};
        QList<QString> names = {"Series10", "Series20", "Series30"};
        QList<QString> seriesInstancesUIds = {"1.2.3.105.2454434", "1.2.3.105.2454435", "1.2.3.105.2454436"};
        for (int i = 0; i<3; i++)
        {
            query.prepare("INSERT INTO series (id, study, name, uid) "
                          "VALUES (:id, :study, :name, :seriesInstanceUID)");
            query.bindValue(":id", ids[i]);
            query.bindValue(":study", studies[i]);
            query.bindValue(":name", names[i]);
            query.bindValue(":seriesInstanceUID", seriesInstancesUIds[i]);
            query.exec();
        }
        QList<medAbstractSource::levelMinimalEntries> entries;
        QString studyId = "2";
        entries = t.getMinimalEntries(2,studyId);

        EXPECT_EQ(entries.size(), 1);
        int i = 0;
        for (auto entry : entries)
        {
            if (studies[i]==studyId)
            {
                EXPECT_EQ(ids[i], entry.key);
                EXPECT_EQ(names[i], entry.name);
                EXPECT_EQ(seriesInstancesUIds[i++], entry.description);
            }
        }
        t.connect(false);
        dir.remove();
    }
}

TEST(requestDBTest, get_minimal_entries_level_series_invalid_key_failed)
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
        QList<QString> studies = {"2", "1", "40"};
        QList<QString> names = {"Series10", "Series20", "Series30"};
        QList<QString> seriesInstancesUIds = {"1.2.3.105.2454434", "1.2.3.105.2454435", "1.2.3.105.2454436"};
        for (int i = 0; i<3; i++)
        {
            query.prepare("INSERT INTO series (id, study, name, uid) "
                          "VALUES (:id, :study, :name, :seriesInstanceUID)");
            query.bindValue(":id", ids[i]);
            query.bindValue(":study", studies[i]);
            query.bindValue(":name", names[i]);
            query.bindValue(":seriesInstanceUID", seriesInstancesUIds[i]);
            query.exec();
        }
        QList<medAbstractSource::levelMinimalEntries> entries;
        QString studyId = "invalid_key";
        entries = t.getMinimalEntries(2,studyId);

        EXPECT_EQ(entries.size(), 0);
        t.connect(false);
        dir.remove();
    }
}

TEST_F(medRequestTest, get_direct_data_invalid_level_failed)
{
    QString path;
    path = m_.getDirectData(1, "key");
    EXPECT_EQ(path, "");
}

TEST_F(medRequestTest, get_direct_data_series_level_right_function_call)
{
    int ui_level = 2;
    QString key = "key";
    QString expected_value = "foo";
    ON_CALL(m_, getSeriesDirectData(key)).WillByDefault(::testing::Return(expected_value));
    EXPECT_CALL(m_, getSeriesDirectData(key)).Times(1);
    EXPECT_EQ(m_.getDirectData(ui_level, key), m_.m_DbPath->value() + expected_value);
}

TEST(requestDBTest, get_direct_data_level_series_invalid_key_failed)
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
        QList<QString> ids = {"10", "10", "30"};
        QList<QString> paths = {"/path/to/data1", "/path/to/data2", "/path/to/data3"};
        for (int i = 0; i<3; i++)
        {
            query.prepare("INSERT INTO series (id, path) "
                          "VALUES (:id, :path)");
            query.bindValue(":id", ids[i]);
            query.bindValue(":study", paths[i]);
            query.exec();
        }
        QString key = "invalid_key";
        QString path;
        EXPECT_EQ(dir.path(), t.getDirectData(2, key));
        t.connect(false);
        dir.remove();
    }
}

TEST(requestDBTest, get_direct_data_level_series_valid_key_success)
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
        EXPECT_EQ(dir.path() + "/path/to/data1", t.getDirectData(2, key));
        t.connect(false);
        dir.remove();
    }
}

TEST(requestDBTest, get_mandatory_data_level_patient_success)
{
    RealMedSQLite t = RealMedSQLite();
    QString instanceId = "bar";

    // expectations
    t.changeDatabasePath("/Users/castelne/Library/ApplicationSupport/inria/medInria/");
    t.initialization(instanceId);
    t.connect(true);

    QList<QMap<QString, QString>>  resList = t.getMandatoryAttributes(0, "");
    qDebug()<<"patients : ";
    for (auto map : resList)
    {
        for (const auto& key: map.keys())
        {
            qDebug()<<key<<" : "<<map[key];
        }
    }

    resList = t.getMandatoryAttributes(0, "1");
    qDebug()<<"\nfirst patient : ";
    for (auto map : resList)
    {
        for (const auto& key: map.keys())
        {
            qDebug()<<key<<" : "<<map[key];
        }
    }

    resList = t.getMandatoryAttributes(0, "foo");
    qDebug()<<"\ninvalid patient : ";
    for (auto map : resList)
    {
        for (const auto& key: map.keys())
        {
            qDebug()<<key<<" : "<<map[key];
        }
    }

    resList = t.getMandatoryAttributes(1, "1");
    qDebug()<<"\n studies : ";
    for (auto map : resList)
    {
        for (const auto& key: map.keys())
        {
            qDebug()<<key<<" : "<<map[key];
        }
    }

    resList = t.getMandatoryAttributes(1, "bar");
    qDebug()<<"\n invalid studies : ";
    for (auto map : resList)
    {
        for (const auto& key: map.keys())
        {
            qDebug()<<key<<" : "<<map[key];
        }
    }

    resList = t.getMandatoryAttributes(2, "1");
    qDebug()<<"\n series : ";
    for (auto map : resList)
    {
        for (const auto& key: map.keys())
        {
            qDebug()<<key<<" : "<<map[key];
        }
    }

    resList = t.getMandatoryAttributes(2, "qix");
    qDebug()<<"\n invalid series : ";
    for (auto map : resList)
    {
        for (const auto& key: map.keys())
        {
            qDebug()<<key<<" : "<<map[key];
        }
    }

}
