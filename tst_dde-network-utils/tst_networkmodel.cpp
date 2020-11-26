#include <gtest/gtest.h>

#include "networkmodel.h"

#include <QMimeData>

using namespace dde::network;

class TstNetworkModel : public testing::Test
{
public:
    void SetUp() override
    {
//        obj = new NetworkModel();
    }

    void TearDown() override
    {
//        delete obj;
//        obj = nullptr;
    }

public:
    NetworkModel *obj = nullptr;
};

TEST_F(TstNetworkModel, coverageTest)
{

}
