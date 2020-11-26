#include <gtest/gtest.h>

#include "connectivitychecker.h"

#include <QMimeData>

using namespace dde::network;

class TstConnectivityChecker : public testing::Test
{
public:
    void SetUp() override
    {
//        obj = new ConnectivityChecker();
    }

    void TearDown() override
    {
//        delete obj;
//        obj = nullptr;
    }

public:
    ConnectivityChecker *obj = nullptr;
};

TEST_F(TstConnectivityChecker, coverageTest)
{

}
