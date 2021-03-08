#include <gtest/gtest.h>

#include "wirelessdevice.h"

#include <QMimeData>

using namespace dde::network;

class TstWirelessDevice : public testing::Test
{
public:
    void SetUp() override
    {
//        obj = new WirelessDevice();
    }

    void TearDown() override
    {
//        delete obj;
//        obj = nullptr;
    }

public:
    WirelessDevice *obj = nullptr;
};

TEST_F(TstWirelessDevice, coverageTest)
{

}
