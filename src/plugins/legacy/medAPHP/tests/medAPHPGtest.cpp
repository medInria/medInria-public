//
// Created by Julien Castelneau on 11/10/2021.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QString>
#include "medAPHP.h"
class FakeQtDcmAPHP : public QtDcmInterface
{
public:
    MOCK_METHOD(int, sendEcho, (const QString &, const QString &, const QString &,
            const QString &, int ));
};

class medAPHPGtest : public ::testing::Test
{
protected:
    ::testing::NiceMock<FakeQtDcmAPHP> f_;
    medAPHP *m_;

    medAPHPGtest()
    {
        m_ = new medAPHP(&f_);
    }

    ~medAPHPGtest() {};

    void SetUp() override
    {

    }

    void TearDown() override
    {

    }
};

TEST_F(medAPHPGtest, test_connect_failed_send_echo_return_minus_1)
{
    ON_CALL(f_, sendEcho).WillByDefault(::testing::Return(-1));
    EXPECT_EQ(m_->connect(true), false);
}

TEST_F(medAPHPGtest, test_connect_success_send_echo_return_0)
{
    ON_CALL(f_, sendEcho).WillByDefault(::testing::Return(0));
    EXPECT_EQ(m_->connect(true), true);
}

TEST_F(medAPHPGtest, test_disconnect_success)
{
    EXPECT_EQ(m_->connect(false), true);
}
