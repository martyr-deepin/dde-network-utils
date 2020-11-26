#include <gtest/gtest.h>

#include "networkdevice.h"

#include <QMimeData>

using namespace dde::network;

class TstNetworkDevice : public testing::Test
{
public:
    void SetUp() override
    {
//        obj = new NetworkDevice();
    }

    void TearDown() override
    {
//        delete obj;
//        obj = nullptr;
    }

public:
    NetworkDevice *obj = nullptr;
};

TEST_F(TstNetworkDevice, coverageTest)
{

}
