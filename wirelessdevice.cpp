/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "wirelessdevice.h"

using namespace dde::network;

#include <QSet>
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QTimer>

#define WIRELESS_PATH  "Path"
#define WIRELESS_STRENGTH  "Strength"

WirelessDevice::WirelessDevice(const QJsonObject &info, QObject *parent)
    : NetworkDevice(NetworkDevice::Wireless, info, parent)
    , m_networkInter("com.deepin.daemon.Network", "/com/deepin/daemon/Network", QDBusConnection::sessionBus(), this)
{
}

bool WirelessDevice::supportHotspot() const
{
    return info()["SupportHotspot"].toBool();
}

const QString WirelessDevice::activeHotspotUuid() const
{
    Q_ASSERT(hotspotEnabled());

    return m_activeHotspotInfo.value("ConnectionUuid").toString();
}

const QList<QJsonObject> WirelessDevice::activeConnections() const
{
    return m_activeConnections;
}

const QList<QJsonObject> WirelessDevice::activeConnectionsInfo() const
{
    return m_activeConnectionsInfo;
}

const QList<QJsonObject> WirelessDevice::activeVpnConnectionsInfo() const
{
    QList<QJsonObject> activeVpns;
    for (const QJsonObject &activeConn : m_activeConnectionsInfo) {
        if (activeConn.value("ConnectionType").toString().startsWith("vpn-")) {
            activeVpns.append(activeConn);
        }
    }

    return activeVpns;
}

const QJsonObject WirelessDevice::activeWirelessConnectionInfo() const
{
    QJsonObject activeWireless;
    for (const QJsonObject &activeConn : m_activeConnectionsInfo) {
        if (activeConn.value("ConnectionType").toString() == "wireless") {
            activeWireless = activeConn;
            break;
        }
    }

    return activeWireless;
}

const QString WirelessDevice::activeWirelessConnName() const
{
    const QJsonObject &conn = activeWirelessConnectionInfo();
    return conn.isEmpty() ? QString() : conn.value("ConnectionName").toString();
}

const QString WirelessDevice::activeWirelessConnUuid() const
{
    const QJsonObject &conn = activeWirelessConnectionInfo();
    return conn.isEmpty() ? QString() : conn.value("ConnectionUuid").toString();
}

const QString WirelessDevice::activeWirelessConnSettingPath() const
{
    const QJsonObject &conn = activeWirelessConnectionInfo();
    return conn.isEmpty() ? QString() : conn.value("SettingPath").toString();
}

const QJsonArray WirelessDevice::apList() const
{
    QJsonArray apArray;
    for (auto ap : m_ssidDatas.values()) {
        apArray.append(QJsonDocument::fromJson(ap.toUtf8()).object());
    }
    return apArray;
}

void WirelessDevice::updateWirlessAp()
{
    m_networkInter.RequestWirelessScan();
}

void WirelessDevice::setAPList(const QString &apList)
{
    QMap<QString, QJsonObject> apsMapOld = m_apsMap;
    m_apsMap.clear();

    const QJsonArray &apArray = QJsonDocument::fromJson(apList.toUtf8()).array();
    for (auto item : apArray) {
        const QJsonObject &ap = item.toObject();
        const QString &path = ap.value(WIRELESS_PATH).toString();

        if (!path.isEmpty()) {
            if (ap.value("Ssid").toString() == activeApSsid() &&
                    ap.value(WIRELESS_STRENGTH).toInt() > activeApStrength()) {
                m_activeApInfo = ap;
                Q_EMIT activeApInfoChanged(m_activeApInfo);
            }

            if (!apsMapOld.contains(path)) {
                Q_EMIT apAdded(ap);
            } else {
                if (apsMapOld.value(path) != ap) {
                    Q_EMIT apInfoChanged(ap);
                }
            }
            m_apsMap.insert(path, ap);
        }
    }

    for (auto path : apsMapOld.keys()) {
        if (!m_apsMap.contains(path)) {
            Q_EMIT apRemoved(apsMapOld.value(path));
        }
    }

    setActiveApBySsid(activeApSsidByActiveConnUuid(activeWirelessConnUuid()));
}

void WirelessDevice::updateAPInfo(const QString &apInfo)
{
    const auto &ap = QJsonDocument::fromJson(apInfo.toUtf8()).object();
    const auto &path = ap.value(WIRELESS_PATH).toString();

    if (!path.isEmpty()) {
        if (ap.value("Ssid").toString() == activeApSsid() &&
                ap.value(WIRELESS_STRENGTH).toInt() > activeApStrength()) {
            m_activeApInfo = ap;
            Q_EMIT activeApInfoChanged(m_activeApInfo);
        }

        if (m_apsMap.contains(path)) {
            Q_EMIT apInfoChanged(ap);
        } else {
            Q_EMIT apAdded(ap);
        }
        // QMap will replace existing key-value
        m_apsMap.insert(path, ap);
    }
}

void WirelessDevice::deleteAP(const QString &apInfo)
{
    const auto &ap = QJsonDocument::fromJson(apInfo.toUtf8()).object();
    const auto &path = ap.value(WIRELESS_PATH).toString();

    if (!path.isEmpty()) {
        if (m_apsMap.contains(path)) {
            m_apsMap.remove(path);
            Q_EMIT apRemoved(ap);
        }
    }
}

void WirelessDevice::setActiveConnections(const QList<QJsonObject> &activeConns)
{
    m_activeConnections = activeConns;

    Q_EMIT activeConnectionsChanged(m_activeConnections);
}

void WirelessDevice::setActiveConnectionsInfo(const QList<QJsonObject> &activeConnsInfo)
{
    m_activeConnectionsInfo = activeConnsInfo;

    if (activeWirelessConnectionInfo().isEmpty()) {
        m_activeApInfo = QJsonObject();
        Q_EMIT activeApInfoChanged(m_activeApInfo);
    } else {
        setActiveApBySsid(activeApSsidByActiveConnUuid(activeWirelessConnUuid()));
    }

    Q_EMIT activeWirelessConnectionInfoChanged(activeWirelessConnectionInfo());
    Q_EMIT activeConnectionsInfoChanged(m_activeConnectionsInfo);
}

void WirelessDevice::setActiveHotspotInfo(const QJsonObject &hotspotInfo)
{
    const bool changed = m_activeHotspotInfo.isEmpty() != hotspotInfo.isEmpty();

    m_activeHotspotInfo = hotspotInfo;

    if (changed)
        Q_EMIT hotspotEnabledChanged(hotspotEnabled());
}

void WirelessDevice::setActiveApBySsid(const QString &ssid)
{
    if (m_apsMap.size() > 0) {
        // get the use same ssid Aps
        QList<QJsonObject> sameSsidAps;
        for (const auto &ap : m_apsMap.values()) {
            if (ap.value("Ssid").toString() == ssid) {
                sameSsidAps.append(ap);
            }
        }

        // set active Ap by strength
        if (sameSsidAps.size() > 0) {
            for (const auto &ap : sameSsidAps) {
                if (activeApStrength() < ap.value("Strength").toInt()) {
                    m_activeApInfo = ap;
                }
            }
            Q_EMIT activeApInfoChanged(m_activeApInfo);
        }
    }
}

void WirelessDevice::setConnections(const QList<QJsonObject> &connections)
{
    m_connections = connections;

    Q_EMIT connectionsChanged(m_connections);
}

void WirelessDevice::setHotspotConnections(const QList<QJsonObject> &hotspotConnections)
{
    m_hotspotConnections = hotspotConnections;

    Q_EMIT hostspotConnectionsChanged(m_hotspotConnections);
}

QString WirelessDevice::activeApSsidByActiveConnUuid(const QString &activeConnUuid)
{
    QString activeApSsid("");

    for (const QJsonObject &conn : m_connections) {
        if (conn.value("Uuid").toString() == activeConnUuid) {
            activeApSsid = conn.value("Ssid").toString();
            break;
        }
    }

    return activeApSsid;
}

void WirelessDevice::WirelessUpdate(const QJsonValue &WirelessList)
{
    //临时变量保存当前全部无限网络状态使用
    QMap<QString, QJsonObject> PathDatas;
    QJsonArray WirelessDatas = WirelessList.toArray();
    for (QJsonValue data : WirelessDatas) {
        //数据为空则进行下一个循环
        if (data.isNull()) continue;
        //数据转换
        QJsonObject apInfo = data.toObject();
        //当不存在两个Key的时候,则进行下一个循环
        if (!apInfo.contains(WIRELESS_PATH) && !apInfo.contains(WIRELESS_STRENGTH)) continue;

        QString Path = apInfo.value(WIRELESS_PATH).toString();
        //修改了的会直接更新,没有的会直接插入
        PathDatas.insert(Path,apInfo);  
    }

    //由于改变和增加,都是调用updateAPInfo，所以可以直接将改变了的全部直接发送出去
    for (QString Pathkey : PathDatas.keys()) {
        QString apInfo = QString(QJsonDocument(PathDatas.value(Pathkey)).toJson());
        //这一步会根据当前有的做修改，没有的会加上,如果有的则会更新
        m_ssidDatas.insert(Pathkey, apInfo);
        this->updateAPInfo(apInfo);
    }
    for (QString PathKey : m_ssidDatas.keys()) {
        if (!PathDatas.contains(PathKey)) {
            this->deleteAP(m_ssidDatas.value(PathKey));
            m_ssidDatas.remove(PathKey);
        }
    }
    PathDatas.clear();
}
