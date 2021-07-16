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
#include <medSQLite.h>
#include <QSqlDatabase>

static bool driverAvailability = true;
static bool valid;
static bool isOpen;

//namespace
//{
    class FakeEngine
    {
    public:
        FakeEngine &operator=(const FakeEngine &other) { return *this; };

        static bool isDriverAvailable(const QString &name) { return driverAvailability; }

        bool isValid() { return valid; };

        bool open() { return isOpen; };
        MOCK_METHOD(void, setDatabaseName, (const QString &));
        void close(){};
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
        void changeDatabasePath(const QString& value) {
            m_DbPath->setValue(value);
        }
    };

    class medSQliteTest : public ::testing::Test
    {

    protected:
        FakeMedSQLite *m_;

        medSQliteTest()
        {
        }

        virtual ~medSQliteTest()
        {
        }

        void SetUp() override
        {
            m_ = new FakeMedSQLite();
        }

        void TearDown() override
        {
            delete m_;
        }

    };

    TEST_F(medSQliteTest, test_init_failed_if_instance_id_empty)
    {
        // parameters initialization
        QString instanceId = "";
        driverAvailability = true;
        valid = true;

        // expectations
        EXPECT_EQ(false, m_->initialization(instanceId));
        EXPECT_EQ(m_->getInstanceId(), "");
    }

    TEST_F(medSQliteTest, test_init_failed_if_driver_unavailable)
    {
        // parameters initialization
        QString instanceId = "foo";
        driverAvailability = false;
        valid = true;
        // expectations
        EXPECT_EQ(false, m_->initialization(instanceId));
        EXPECT_EQ(m_->getInstanceId(), "");
    }

    TEST_F(medSQliteTest, test_init_failed_if_engine_not_valid)
    {
        // parameters initialization
        driverAvailability = true;
        QString instanceId = "foo";
        valid = false;
        // expectations
        EXPECT_EQ(false, m_->initialization(instanceId));
        EXPECT_EQ(m_->getInstanceId(), "");
    }

    TEST_F(medSQliteTest, test_init_success_if_engine_and_driver_are_ok)
    {
        // parameters initialization
        driverAvailability = true;
        QString instanceId = "foo";
        valid = true;
        // expectations
        EXPECT_EQ(true, m_->initialization(instanceId));
        EXPECT_EQ(m_->getInstanceId(), instanceId);
    }


    TEST_F(medSQliteTest, test_connect_failed_if_db_not_valid)
    {
        valid = false;
        // expectations
        EXPECT_EQ(false, m_->connect(true));
    }

    TEST_F(medSQliteTest, test_disconnect_failed_if_db_not_valid)
    {
        valid = false;
        // expectations
        EXPECT_EQ(false, m_->connect(false));
    }

    TEST_F(medSQliteTest, test_connect_failed_if_db_path_is_empty)
    {
        m_->changeDatabasePath("");
        valid = true;
        // expectations
        EXPECT_EQ(false, m_->connect(true));
        EXPECT_EQ(true, m_->connect(false));
    }

    TEST_F(medSQliteTest, test_disconnect_success_if_db_path_is_empty)
    {
        m_->changeDatabasePath("");
        valid = true;
        // expectations
        EXPECT_EQ(true, m_->connect(false));
    }

//}


//using ::testing::NiceMock;

//typedef ::testing::Types<QSqlDatabase> MyTypes ;

/*TEST(medSQlite, is_writeable_test)
{
    medSQlite m = medSQlite();
    EXPECT_EQ(true, m.isWriteable());
}

TEST(medSQlite, is_local_test)
{
    medSQlite m = medSQlite();
    EXPECT_EQ(true, m.isLocal());
}

TEST(medSQlite, is_cached_test)
{
    medSQlite m = medSQlite();
    EXPECT_EQ(false, m.isCached());
}

TEST(medSQlite, is_online_test)
{
    medSQlite m = medSQlite();
    EXPECT_EQ(false, m.isOnline());
}
*/