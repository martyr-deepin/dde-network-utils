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

const QString WirelessDevice::activeWirelessConnSpecificObject() const
{
    const QJsonObject &conn = activeWirelessConnectionInfo();
    return conn.isEmpty() ? QString() : conn.value("SpecificObject").toString();
}

const QJsonArray WirelessDevice::apList() const
{
    QJsonArray apArray;
    for (auto ap : m_apsMap.values()) {
        apArray.append(ap);
    }
    return apArray;
}

void WirelessDevice::updateWirlessAp()
{
    m_networkInter.RequestWirelessScan();
}

void WirelessDevice::setAPList(const QJsonValue &wirelessList)
{
    QMap<QString, QJsonObject> apsMapOld = m_apsMap;
    m_apsMap.clear();

    const QJsonArray &apArray = wirelessList.toArray();
    for (auto item : apArray) {
        const QJsonObject &ap = item.toObject();
        const QString &path = ap.value(WIRELESS_PATH).toString();

        if (!path.isEmpty()) {
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

    setActiveApByPath(activeWirelessConnSpecificObject());
}

void WirelessDevice::updateAPInfo(const QString &apInfo)
{
    const auto &ap = QJsonDocument::fromJson(apInfo.toUtf8()).object();
    const auto &path = ap.value(WIRELESS_PATH).toString();

    if (!path.isEmpty()) {
        if (ap.value("Path").toString() == activeApPath()) {
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
        setActiveApByPath(activeWirelessConnSpecificObject());
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

void WirelessDevice::setActiveApByPath(const QString &path)
{
    if (path == "") {
        m_activeApInfo = QJsonObject();
    } else {
        auto it = m_apsMap.find(path);
        if (it == m_apsMap.end()) return;

        m_activeApInfo = it.value();
    }

    Q_EMIT activeApInfoChanged(m_activeApInfo);
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
