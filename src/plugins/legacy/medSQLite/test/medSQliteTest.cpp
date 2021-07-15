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

namespace
{
    class MockDB
    {
    public:
        MockDB &operator=(const MockDB &other) { return *this; };

        static bool isDriverAvailable(const QString &name) { return driverAvailability; }

        bool isValid() { return valid; };

        bool open() { return isOpen; };
        MOCK_METHOD(void, setDatabaseName, (const QString &));
        MOCK_METHOD(void, close, ());

        static MockDB &addDatabase(const QString &, const QString &)
        {
            static MockDB db;
            return db;
        };

        static MockDB &database(const QString &)
        {
            static MockDB db;
            return db;
        };
    };


    class medSQliteTest : public ::testing::Test
    {

    protected:
        medSQlite<MockDB> *m_;

        medSQliteTest()
        {
        }

        virtual ~medSQliteTest()
        {
        }

        void SetUp() override
        {
            m_ = new medSQlite<MockDB>();
        }

        void TearDown() override
        {
            delete m_;
        }

    };

    TEST_F(medSQliteTest, test_initialization_failed_if_instance_id_empty)
    {
        // parameters initialization
        QString instanceId = "";
        driverAvailability = true;
        valid = true;

        // expectations
        EXPECT_EQ(false, m_->initialization(instanceId));
        EXPECT_EQ(m_->getInstanceId(), "");
    }

    TEST_F(medSQliteTest, test_initialization_failed_if_driver_unavailable)
    {
        // parameters initialization
        QString instanceId = "foo";
        driverAvailability = false;
        valid = true;
        // expectations
        EXPECT_EQ(false, m_->initialization(instanceId));
        EXPECT_EQ(m_->getInstanceId(), "");
    }

    TEST_F(medSQliteTest, test_initialization_failed_if_engine_not_valid)
    {
        // parameters initialization
        driverAvailability = true;
        QString instanceId = "foo";
        valid = false;
        // expectations
        EXPECT_EQ(false, m_->initialization(instanceId));
        EXPECT_EQ(m_->getInstanceId(), "");
    }

    TEST_F(medSQliteTest, test_initialization_success_if_engine_and_driver_are_ok)
    {
        // parameters initialization
        driverAvailability = true;
        QString instanceId = "foo";
        valid = true;
        // expectations
        EXPECT_EQ(true, m_->initialization(instanceId));
        EXPECT_EQ(m_->getInstanceId(), instanceId);
    }

    class MockMedSQLite : public medSQlite<MockDB>
    {
    public:
        void changeDatabasePath() {
            m_DbPath->setValue("");
        }
    };

    TEST(medSQliteTest, test_connect_failed_if_db_path_is_empty)
    {
        // expectations
        MockMedSQLite m = MockMedSQLite();
        m.changeDatabasePath();
        EXPECT_EQ(false, m.connect(true));
        EXPECT_EQ(false, m.connect(false));
    }

}


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