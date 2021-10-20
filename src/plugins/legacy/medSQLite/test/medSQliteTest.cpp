/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QSqlDatabase>
#include <QSqlRecord>

#include "medSQLiteTest.h"

class medSQliteTest : public ::testing::Test
{

protected:
    ::testing::NiceMock<FakeMedSQLite> m_;

    medSQliteTest()
    {
    }

    virtual ~medSQliteTest()
    {
    }

    void SetUp() override
    {
        removeCount = 0;
    }

    void TearDown() override
    {
        removeCount = 0;
    }
    QString patientQuery = "CREATE TABLE IF NOT EXISTS patient ("
                           " id       INTEGER PRIMARY KEY,"
                           " name        TEXT,"
                           " thumbnail   TEXT,"
                           " birthdate   TEXT,"
                           " gender      TEXT,"
                           " patientId   TEXT"
                           ");";
    QString studyQuery = "CREATE TABLE IF NOT EXISTS study ("
                         " id        INTEGER      PRIMARY KEY,"
                         " patient   INTEGER," // FOREIGN KEY
                         " name         TEXT,"
                         " uid          TEXT,"
                         " thumbnail    TEXT,"
                         " studyId      TEXT"
                         ");";
    QString seriesQuery = "CREATE TABLE IF NOT EXISTS series ("
                          " id       INTEGER      PRIMARY KEY,"
                          " study    INTEGER," // FOREIGN KEY
                          " size     INTEGER,"
                          " name            TEXT,"
                          " path            TEXT,"
                          " uid             TEXT,"
                          " seriesId        TEXT,"
                          " orientation     TEXT,"
                          " seriesNumber    TEXT,"
                          " sequenceName    TEXT,"
                          " sliceThickness  TEXT,"
                          " rows            TEXT,"
                          " columns         TEXT,"
                          " thumbnail       TEXT,"
                          " age             TEXT,"
                          " description     TEXT,"
                          " modality        TEXT,"
                          " protocol        TEXT,"
                          " comments        TEXT,"
                          " status          TEXT,"
                          " acquisitiondate TEXT,"
                          " importationdate TEXT,"
                          " referee         TEXT,"
                          " performer       TEXT,"
                          " institution     TEXT,"
                          " report          TEXT,"
                          " origin          TEXT,"
                          " flipAngle       TEXT,"
                          " echoTime        TEXT,"
                          " repetitionTime  TEXT,"
                          " acquisitionTime TEXT"
                          ");";

};

// Test Initialization
TEST_F(medSQliteTest, test_init_failed_if_instance_id_empty)
{
    // parameters initialization
    QString instanceId = "";
    driverAvailability = true;
    valid = true;
    // expectations
    EXPECT_EQ(false, m_.initialization(instanceId));
    EXPECT_EQ(m_.getInstanceId(), "");
}

TEST_F(medSQliteTest, test_init_failed_if_driver_unavailable)
{
    // parameters initialization
    QString instanceId = "foo";
    driverAvailability = false;
    valid = true;
    // expectations
    EXPECT_EQ(false, m_.initialization(instanceId));
    EXPECT_EQ(m_.getInstanceId(), "");
}

TEST_F(medSQliteTest, test_init_failed_if_engine_not_valid)
{
    // parameters initialization
    driverAvailability = true;
    QString instanceId = "foo";
    valid = false;
    // expectations
    EXPECT_EQ(false, m_.initialization(instanceId));
    EXPECT_EQ(m_.getInstanceId(), "");
}

TEST_F(medSQliteTest, test_init_success_if_engine_and_driver_are_ok)
{
    // parameters initialization
    driverAvailability = true;
    QString instanceId = "foo";
    valid = true;
    // expectations
    EXPECT_EQ(true, m_.initialization(instanceId));
    EXPECT_EQ(m_.getInstanceId(), instanceId);
}

// Test Connect/Disconnect
TEST_F(medSQliteTest, test_connect_failed_if_db_path_is_empty)
{
    m_.changeDatabasePath("");
    // expectations
    EXPECT_CALL(m_.m_Engine, close()).Times(0);
    EXPECT_CALL(m_, optimizeSpeedSQLiteDB).Times(0);
    EXPECT_EQ(false, m_.connect(true));
    EXPECT_EQ(false, m_.isOnline());
}

TEST_F(medSQliteTest, test_connect_failed_db_not_open)
{
    m_.changeDatabasePath("foo");
    flagOpen = false;
    EXPECT_CALL(m_.m_Engine, close()).Times(0);
    EXPECT_CALL(m_, optimizeSpeedSQLiteDB).Times(0);
    EXPECT_EQ(false, m_.connect(true));
    EXPECT_EQ(false, m_.isOnline());
    EXPECT_EQ(removeCount, 1);
}


TEST_F(medSQliteTest, test_connect_success_db_structure_empty)
{
    fakeTables = QStringList();
    m_.changeDatabasePath("foo");
    flagOpen = true;

    ON_CALL(m_, createTable(patientQuery)).WillByDefault(::testing::Return(true));
    ON_CALL(m_, createTable(studyQuery)).WillByDefault(::testing::Return(true));
    ON_CALL(m_, createTable(seriesQuery)).WillByDefault(::testing::Return(true));
    EXPECT_CALL(m_.m_Engine, close()).Times(0);
    EXPECT_CALL(m_, optimizeSpeedSQLiteDB).Times(1);
    EXPECT_EQ(true, m_.connect(true));
    EXPECT_EQ(true, m_.isOnline());
    EXPECT_EQ(removeCount, 0);
}

TEST_F(medSQliteTest, test_connect_failed_db_structure_empty_unable_create_patient_table)
{
    fakeTables = QStringList();
    m_.changeDatabasePath("foo");
    flagOpen = true;
    valid = true;
    ON_CALL(m_, createTable(patientQuery)).WillByDefault(::testing::Return(false));
    ON_CALL(m_, createTable(studyQuery)).WillByDefault(::testing::Return(true));
    ON_CALL(m_, createTable(seriesQuery)).WillByDefault(::testing::Return(true));
    EXPECT_CALL(m_.m_Engine, close()).Times(1);
    EXPECT_CALL(m_, optimizeSpeedSQLiteDB).Times(0);
    EXPECT_EQ(false, m_.connect(true));
    EXPECT_EQ(false, m_.isOnline());
    EXPECT_EQ(removeCount, 1);
}

TEST_F(medSQliteTest, test_connect_failed_db_structure_empty_unable_create_study_table)
{
    fakeTables = QStringList();
    m_.changeDatabasePath("foo");
    flagOpen = true;

    ON_CALL(m_, createTable(patientQuery)).WillByDefault(::testing::Return(true));
    ON_CALL(m_, createTable(studyQuery)).WillByDefault(::testing::Return(false));
    ON_CALL(m_, createTable(seriesQuery)).WillByDefault(::testing::Return(true));
    EXPECT_CALL(m_.m_Engine, close()).Times(1);
    EXPECT_CALL(m_, optimizeSpeedSQLiteDB).Times(0);
    EXPECT_EQ(false, m_.connect(true));
    EXPECT_EQ(false, m_.isOnline());
    EXPECT_EQ(removeCount, 1);
}

TEST_F(medSQliteTest, test_connect_failed_db_structure_empty_unable_create_series_table)
{
    fakeTables = QStringList();
    m_.changeDatabasePath("foo");
    flagOpen = true;

    ON_CALL(m_, createTable(patientQuery)).WillByDefault(::testing::Return(true));
    ON_CALL(m_, createTable(studyQuery)).WillByDefault(::testing::Return(true));
    ON_CALL(m_, createTable(seriesQuery)).WillByDefault(::testing::Return(false));
    EXPECT_CALL(m_.m_Engine, close()).Times(1);
    EXPECT_CALL(m_, optimizeSpeedSQLiteDB).Times(0);
    EXPECT_EQ(false, m_.connect(true));
    EXPECT_EQ(false, m_.isOnline());
    EXPECT_EQ(removeCount, 1);
}


TEST_F(medSQliteTest, test_connect_failed_db_structure_invalid)
{
    fakeTables << "patient" << "studyyyyy" << "series";
    m_.changeDatabasePath("foo");
    flagOpen = true;
    EXPECT_CALL(m_.m_Engine, close()).Times(1);
    EXPECT_CALL(m_, optimizeSpeedSQLiteDB).Times(0);
    EXPECT_EQ(false, m_.connect(true));
    EXPECT_EQ(false, m_.isOnline());
    EXPECT_EQ(removeCount, 1);
}

TEST_F(medSQliteTest, test_connect_success_valid_db_structure)
{
    fakeTables << "patient" << "study" << "series";
    m_.changeDatabasePath("foo");
    flagOpen = true;
    // expectations
    EXPECT_CALL(m_.m_Engine, close()).Times(0);
    EXPECT_CALL(m_, optimizeSpeedSQLiteDB).Times(1);
    EXPECT_EQ(true, m_.connect(true));
    EXPECT_EQ(true, m_.isOnline());
    EXPECT_EQ(removeCount, 0);
}

TEST_F(medSQliteTest, test_disconnect_success_if_db_path_is_empty)
{
    m_.changeDatabasePath("");
    // expectations
    EXPECT_CALL(m_.m_Engine, close()).Times(1);
    EXPECT_CALL(m_, optimizeSpeedSQLiteDB).Times(0);
    EXPECT_EQ(true, m_.connect(false));
    EXPECT_EQ(false, m_.isOnline());
    EXPECT_EQ(removeCount, 1);
}


TEST(integrationTest, test_plugin_connection_success_with_existing_db)
{
    RealMedSQLite t = RealMedSQLite();
    QString instanceId = "bar";
    t.changeDatabasePath("/Users/castelne/Library/Application Support/inria/medInria");

    // expectations
    EXPECT_EQ(true, t.initialization(instanceId));
    EXPECT_EQ(t.getInstanceId(), instanceId);
    EXPECT_EQ(true, t.connect(true));
    EXPECT_EQ(true, t.isOnline());

    QSqlRecord record = t.m_Engine.record("patient");
    QStringList keys = {"id", "name", "thumbnail", "birthdate", "gender", "patientId"};
    for(int i=0;i<record.count();i++){
        QSqlField field=record.field(i);
        EXPECT_EQ(keys.at(i), field.name() );
    }
    record = t.m_Engine.record("study");
    keys.clear();
    keys << "id" << "patient" << "name" << "uid" << "thumbnail" << "studyId";
    for(int i=0;i<record.count();i++){
        QSqlField field=record.field(i);
        EXPECT_EQ(keys.at(i), field.name() );
    }
    record = t.m_Engine.record("series");
    keys.clear();
    keys << "id" << "study" << "size" << "name" << "path"<< "uid" << "seriesId" << "orientation"
         << "seriesNumber" << "sequenceName" << "sliceThickness" << "rows" << "columns" << "thumbnail"
         << "age" << "description" << "modality" << "protocol" << "comments" << "status" << "acquisitiondate"
         << "importationdate" << "referee" << "performer" << "institution" << "report" << "origin"
         << "flipAngle" << "echoTime" << "repetitionTime" << "acquisitionTime" << "isIndexed";
    for(int i=0;i<record.count();i++){
        QSqlField field=record.field(i);
        EXPECT_EQ(keys.at(i), field.name() );
    }

    EXPECT_EQ(true, t.connect(false));
    EXPECT_EQ(false, t.isOnline());
}

TEST(integrationTest, test_plugin_connection_success_with_empty_db)
{
    RealMedSQLite t = RealMedSQLite();
    QString instanceId = "bar";
    QTemporaryDir dir;
    if (dir.isValid()) {
        t.changeDatabasePath(dir.path());

        // expectations
        EXPECT_EQ(true, t.initialization(instanceId));
        EXPECT_EQ(t.getInstanceId(), instanceId);

        EXPECT_EQ(true, t.connect(true));
        EXPECT_EQ(true, t.isOnline());

        QSqlRecord record = t.m_Engine.record("patient");
        QStringList expected_keys = {"id", "name", "thumbnail", "birthdate", "gender", "patientId"};
        for(int i=0;i<record.count();i++){
            QSqlField field=record.field(i);
            EXPECT_EQ(expected_keys.at(i), field.name() );
        }
        record = t.m_Engine.record("study");
        expected_keys.clear();
        expected_keys << "id" << "patient" << "name" << "uid" << "thumbnail" << "studyId";
        for(int i=0;i<record.count();i++){
            QSqlField field=record.field(i);
            EXPECT_EQ(expected_keys.at(i), field.name() );
        }
        record = t.m_Engine.record("series");
        expected_keys.clear();
        expected_keys << "id" << "study" << "size" << "name" << "path" << "uid" << "seriesId" << "orientation"
                      << "seriesNumber" << "sequenceName" << "sliceThickness" << "rows" << "columns" << "thumbnail"
                      << "age" << "description" << "modality" << "protocol" << "comments" << "status" << "acquisitiondate"
                      << "importationdate" << "referee" << "performer" << "institution" << "report" << "origin"
                      << "flipAngle" << "echoTime" << "repetitionTime" << "acquisitionTime" << "isIndexed";
        for(int i=0;i<record.count();i++){
            QSqlField field=record.field(i);
            EXPECT_EQ(expected_keys.at(i), field.name() );
        }

        EXPECT_EQ(true, t.connect(false));
        EXPECT_EQ(false, t.isOnline());
        dir.remove();
    }
}

// Test Accessors (get/set)
TEST(AccessTest, test_levelcount_success)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int levelCount = 3;
    EXPECT_EQ(levelCount, m.getLevelCount());
}

TEST(AccessTest, test_levelnames_success)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    QStringList names;
    names << "patient" << "study" << "series";
    EXPECT_EQ(names, m.getLevelNames());
}

TEST(AccessTest, test_levelname_1st_level_success)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 0;
    EXPECT_EQ("patient", m.getLevelName(level));
}

TEST(AccessTest, test_levelname_2nd_level_success)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 1;
    EXPECT_EQ("study", m.getLevelName(level));
}

TEST(AccessTest, test_levelname_3rd_level_success)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 2;
    EXPECT_EQ("series", m.getLevelName(level));
}

TEST(AccessTest, test_levelname_negative_level_failed)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = -1;
    EXPECT_EQ("", m.getLevelName(level));
}

TEST(AccessTest, test_levelname_invalid_level_failed)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 3;
    EXPECT_EQ("", m.getLevelName(level));
}

TEST(AccessTest, test_mandatory_attr_keys_1st_level_success)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 0;
    QStringList mandatory_keys;
    mandatory_keys << "id" << "name"<< "patientId";
    EXPECT_EQ(mandatory_keys, m.getMandatoryAttributesKeys(level));
}

TEST(AccessTest, test_mandatory_attr_keys_2nd_level_success)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 1;
    QStringList mandatory_keys;
    mandatory_keys << "id" << "name" << "uid";
    EXPECT_EQ(mandatory_keys, m.getMandatoryAttributesKeys(level));
}

TEST(AccessTest, test_mandatory_attr_keys_3rd_level_success)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 2;
    QStringList mandatory_keys;
    mandatory_keys << "id" << "name" << "uid";
    EXPECT_EQ(mandatory_keys, m.getMandatoryAttributesKeys(level));
}

TEST(AccessTest, test_mandatory_attr_keys_negative_level_failed)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = -1;
    EXPECT_EQ(QStringList(), m.getMandatoryAttributesKeys(level));
}

TEST(AccessTest, test_mandatory_attr_keys_invalid_level_failed)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 3;
    EXPECT_EQ(QStringList(), m.getMandatoryAttributesKeys(level));
}

TEST(AccessTest, test_additional_attr_keys_1st_level_success)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 0;
    QStringList additional_keys;
    additional_keys << "thumbnail" << "birthdate"<< "gender";
    EXPECT_EQ(additional_keys, m.getAdditionalAttributesKeys(level));
}

TEST(AccessTest, test_additional_attr_keys_2nd_level_success)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 1;
    QStringList additional_keys;
    additional_keys << "patient";
    EXPECT_EQ(additional_keys, m.getAdditionalAttributesKeys(level));
}

TEST(AccessTest, test_additional_attr_keys_3rd_level_success)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 2;
    QStringList additional_keys;
    additional_keys << "size" << "age" << "modality" << "acquisitiondate" << "importationdate" << "referee"
                    <<"performer" << "institution" << "report" << "thumbnail";
    EXPECT_EQ(additional_keys, m.getAdditionalAttributesKeys(level));
}

TEST(AccessTest, test_additional_attr_keys_negative_level_failed)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = -1;
    EXPECT_EQ(QStringList(), m.getAdditionalAttributesKeys(level));
}

TEST(AccessTest, test_additional_attr_keys_invalid_level_failed)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 3;
    EXPECT_EQ(QStringList(), m.getAdditionalAttributesKeys(level));
}

TEST(AccessTest, is_writeable_true)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    EXPECT_EQ(true, m.isWriteable());
}

TEST(AccessTest, is_local_true)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    EXPECT_EQ(true, m.isLocal());
}

TEST(AccessTest, is_cached_false)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    EXPECT_EQ(false, m.isCached());
}

TEST(AccessTest, is_online_true)
{
    FakeMedSQLite m = FakeMedSQLite();
    fakeTables << "patient" << "study" << "series";
    m.changeDatabasePath("foo");
    flagOpen = true;
    EXPECT_CALL(m, optimizeSpeedSQLiteDB).Times(1);
    m.connect(true);
    EXPECT_EQ(true, m.isOnline());
}

