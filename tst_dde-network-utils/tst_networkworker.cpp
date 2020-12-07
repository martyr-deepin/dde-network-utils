#include <gtest/gtest.h>

#include "networkworker.h"

#include <QMimeData>

using namespace dde::network;

class TstNetworkWorker : public testing::Test
{
public:
    void SetUp() override
    {
//        obj = new NetworkWorker();
    }

    void TearDown() override
    {
//        delete obj;
//        obj = nullptr;
    }

public:
    NetworkWorker *obj = nullptr;
};

TEST_F(TstNetworkWorker, coverageTest)
{

}
