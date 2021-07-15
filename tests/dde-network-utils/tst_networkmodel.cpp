#include <gtest/gtest.h>

#include "networkmodel.h"

#include <QMimeData>
#include <QDebug>

using namespace dde::network;

class TstNetworkModel : public testing::Test
{
public:
    void SetUp() override
    {
        model = new NetworkModel();
    }

    void TearDown() override
    {
        delete model;
        model = nullptr;
    }

public:
    NetworkModel *model = nullptr;
};

TEST_F(TstNetworkModel, coverageTest)
{
    QList<NetworkDevice *> devices = model->devices();
    for (NetworkDevice *device : devices)
        qDebug() << device->statusQueue() << "," << device->statusString()
                 << "," << device->path();

    QList<QJsonObject> vpns = model->vpns();
    for (QJsonObject vpn : vpns)
        qDebug() << vpn;

    QList<QJsonObject> wireds = model->wireds();
    for (QJsonObject wired : wireds)
        qDebug() << wired;

    QList<QJsonObject> wirelessList = model->wireless();
    for (QJsonObject wireless : wirelessList)
        qDebug() << wireless;

    QList<QJsonObject> pppoes = model->pppoes();
    for (QJsonObject pppoe : pppoes)
        qDebug() << pppoe;

    QList<QJsonObject> hotspots = model->hotspots();
    for (QJsonObject hotspot : hotspots)
        qDebug() << hotspot;

    QList<QJsonObject> activeConnInfos = model->activeConnInfos();
    for (QJsonObject activeConnInfo : activeConnInfos)
        qDebug() << activeConnInfo;

    QList<QJsonObject> activeConns = model->activeConns();
    for (QJsonObject activeConn : activeConns)
        qDebug() << activeConn;

    QStringList deviceInterfaces = model->deviceInterface();
    for (QString deviceInterface : deviceInterfaces)
        qDebug() << deviceInterface;
}
