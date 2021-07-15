#include <gtest/gtest.h>

#include "connectivitychecker.h"

#include <QMimeData>

using namespace dde::network;

class TstConnectivityChecker : public testing::Test
{
public:
    void SetUp() override
    {
        m_connectivityChecker = new ConnectivityChecker();
    }

    void TearDown() override
    {
        delete m_connectivityChecker;
        m_connectivityChecker = nullptr;
    }

public:
    ConnectivityChecker *m_connectivityChecker = nullptr;
};

TEST_F(TstConnectivityChecker, coverageTest)
{
    m_connectivityChecker->startCheck();
}
