#include <gtest/gtest.h>

#include "wireddevice.h"

#include <QMimeData>

using namespace dde::network;

class TstWiredDevice : public testing::Test
{
public:
    void SetUp() override
    {
//        obj = new WiredDevice();
    }

    void TearDown() override
    {
//        delete obj;
//        obj = nullptr;
    }

public:
    WiredDevice *obj = nullptr;
};

TEST_F(TstWiredDevice, coverageTest)
{

}
