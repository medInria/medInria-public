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

#include <QSqlDatabase>

#define private public

#include <medSQLite.h>

static bool driverAvailability = true;
static bool valid;
static bool isOpen;
static QStringList fakeTables = QStringList();

class FakeEngine
{
public:
    FakeEngine(){};
    FakeEngine(const FakeEngine &other){};
    FakeEngine &operator=(const FakeEngine &other) { return *this; };

    static bool isDriverAvailable(const QString &name) { return driverAvailability; }

    bool isValid() { return valid; };
    bool open() { return isOpen; };
    void setDatabaseName(const QString &name){};
    MOCK_METHOD(void, close, ());
//    void close(){};
    QSqlQuery exec(){return QSqlQuery();};
    QStringList tables(){return fakeTables;};

    static FakeEngine &addDatabase(const QString &, const QString &)
    {
        static FakeEngine db;
        return db;
    };

    static FakeEngine &database(const QString &)
    {
        static FakeEngine db;
        return db;
    };
};

class FakeMedSQLite : public medSQlite<FakeEngine>
{
public:
    FakeMedSQLite &operator=(const FakeMedSQLite &other) { return *this; };
    void changeDatabasePath(const QString& value) {
        m_DbPath->setValue(value);
    }

    MOCK_METHOD(bool, createTable, (const QString &));
};

class RealMedSQLite : public medSQlite<QSqlDatabase>
{
public:
    void changeDatabasePath(const QString& value) {
        m_DbPath->setValue(value);
    }

};

class medSQliteTest : public ::testing::Test
{

protected:
//    FakeMedSQLite m_;
    ::testing::NiceMock<FakeMedSQLite> m_;

    medSQliteTest()
    {
    }

    virtual ~medSQliteTest()
    {
    }

    void SetUp() override
    {
//        m_ = FakeMedSQLite();
    }

    void TearDown() override
    {
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
    EXPECT_EQ(false, m_.connect(true));
    EXPECT_EQ(false, m_.isOnline());

}

TEST_F(medSQliteTest, test_connect_failed_db_not_open)
{
    m_.changeDatabasePath("foo");
    isOpen = false;
    EXPECT_CALL(m_.m_Engine, close()).Times(0);
    EXPECT_EQ(false, m_.connect(true));
    EXPECT_EQ(false, m_.isOnline());
}


TEST_F(medSQliteTest, test_connect_success_db_structure_empty)
{
    fakeTables = QStringList();
    m_.changeDatabasePath("foo");
    isOpen = true;

    ON_CALL(m_, createTable(patientQuery)).WillByDefault(::testing::Return(true));
    ON_CALL(m_, createTable(studyQuery)).WillByDefault(::testing::Return(true));
    ON_CALL(m_, createTable(seriesQuery)).WillByDefault(::testing::Return(true));
    EXPECT_CALL(m_.m_Engine, close()).Times(0);
    EXPECT_EQ(true, m_.connect(true));
    EXPECT_EQ(true, m_.isOnline());
}

TEST_F(medSQliteTest, test_connect_failed_db_structure_empty_unable_create_patient_table)
{
    fakeTables = QStringList();
    m_.changeDatabasePath("foo");
    isOpen = true;
    valid = true;
    ON_CALL(m_, createTable(patientQuery)).WillByDefault(::testing::Return(false));
    ON_CALL(m_, createTable(studyQuery)).WillByDefault(::testing::Return(true));
    ON_CALL(m_, createTable(seriesQuery)).WillByDefault(::testing::Return(true));
    EXPECT_CALL(m_.m_Engine, close()).Times(1);
    EXPECT_EQ(false, m_.connect(true));
    EXPECT_EQ(false, m_.isOnline());
}

TEST_F(medSQliteTest, test_connect_failed_db_structure_empty_unable_create_study_table)
{
    fakeTables = QStringList();
    m_.changeDatabasePath("foo");
    isOpen = true;

    ON_CALL(m_, createTable(patientQuery)).WillByDefault(::testing::Return(true));
    ON_CALL(m_, createTable(studyQuery)).WillByDefault(::testing::Return(false));
    ON_CALL(m_, createTable(seriesQuery)).WillByDefault(::testing::Return(true));
    EXPECT_CALL(m_.m_Engine, close()).Times(1);
    EXPECT_EQ(false, m_.connect(true));
    EXPECT_EQ(false, m_.isOnline());
}

TEST_F(medSQliteTest, test_connect_failed_db_structure_empty_unable_create_series_table)
{
    fakeTables = QStringList();
    m_.changeDatabasePath("foo");
    isOpen = true;

    ON_CALL(m_, createTable(patientQuery)).WillByDefault(::testing::Return(true));
    ON_CALL(m_, createTable(studyQuery)).WillByDefault(::testing::Return(true));
    ON_CALL(m_, createTable(seriesQuery)).WillByDefault(::testing::Return(false));
    EXPECT_CALL(m_.m_Engine, close()).Times(1);
    EXPECT_EQ(false, m_.connect(true));
    EXPECT_EQ(false, m_.isOnline());
}


TEST_F(medSQliteTest, test_connect_failed_db_structure_invalid)
{
    fakeTables << "patient" << "studyyyyy" << "series";
    m_.changeDatabasePath("foo");
    isOpen = true;
    EXPECT_CALL(m_.m_Engine, close()).Times(1);
    EXPECT_EQ(false, m_.connect(true));
    EXPECT_EQ(false, m_.isOnline());
}

TEST_F(medSQliteTest, test_connect_success_valid_db_structure)
{
    fakeTables << "patient" << "study" << "series";
    m_.changeDatabasePath("foo");
    isOpen = true;
    // expectations
    EXPECT_EQ(true, m_.connect(true));
    EXPECT_EQ(true, m_.isOnline());
}

//TEST(toto, test_connect_qdb)
//{
//    RealMedSQLite t = RealMedSQLite();
//    t.initialization("foo");
////    t.changeDatabasePath("/Users/castelne/Library/Application Support/inria/medInria");
//    t.changeDatabasePath("/Users/castelne/developer/tmp/db_test_plugin");
//
//    t.connect(true);
//}


TEST_F(medSQliteTest, test_disconnect_success_if_db_path_is_empty)
{
    m_.changeDatabasePath("");
    // expectations
    EXPECT_EQ(true, m_.connect(false));
    EXPECT_EQ(false, m_.isOnline());
}

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
    mandatory_keys << "id" << "name"<< "birthdate" << "gender";
    EXPECT_EQ(mandatory_keys, m.getMandatoryAttributesKeys(level));
}

TEST(AccessTest, test_mandatory_attr_keys_2nd_level_success)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 1;
    QStringList mandatory_keys;
    mandatory_keys << "id" << "name";
    EXPECT_EQ(mandatory_keys, m.getMandatoryAttributesKeys(level));
}

TEST(AccessTest, test_mandatory_attr_keys_3rd_level_success)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 2;
    QStringList mandatory_keys;
    mandatory_keys << "id" << "name";
    EXPECT_EQ(mandatory_keys, m.getMandatoryAttributesKeys(level));
}

TEST(AccessTest, test_mandatory_attr_keys_negative_level_failed)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = -1;
    QStringList mandatory_keys;
    EXPECT_EQ(mandatory_keys, m.getMandatoryAttributesKeys(level));
}

TEST(AccessTest, test_mandatory_attr_keys_invalid_level_failed)
{
    medSQlite<QSqlDatabase> m = medSQlite<QSqlDatabase>();
    int level = 3;
    EXPECT_EQ("", m.getLevelName(level));
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
    isOpen = true;
    m.connect(true);
    EXPECT_EQ(true, m.isOnline());
}
