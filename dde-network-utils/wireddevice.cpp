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

#include "wireddevice.h"

#include <QDebug>

using namespace dde::network;

WiredDevice::WiredDevice(const QJsonObject &info, QObject *parent)
    : NetworkDevice(NetworkDevice::Wired, info, parent)
{

}

const QList<QJsonObject> WiredDevice::connections() const
{
    return m_connections;
}

void WiredDevice::setConnections(const QList<QJsonObject> &connections)
{
    m_connections = connections;

    Q_EMIT connectionsChanged(m_connections);
}

const QList<QJsonObject> WiredDevice::activeConnections() const
{
    return m_activeConnections;
}

const QList<QJsonObject> WiredDevice::activeConnectionsInfo() const
{
    return m_activeConnectionsInfo;
}

void WiredDevice::setActiveConnections(const QList<QJsonObject> &activeConns)
{
    m_activeConnections = activeConns;

    Q_EMIT activeConnectionsChanged(m_activeConnections);
}

void WiredDevice::setActiveConnectionsInfo(const QList<QJsonObject> &activeConnInfoList)
{
    m_activeConnectionsInfo = activeConnInfoList;

    Q_EMIT activeWiredConnectionInfoChanged(activeWiredConnectionInfo());
    Q_EMIT activeConnectionsInfoChanged(m_activeConnectionsInfo);
}

const QList<QJsonObject> WiredDevice::activeVpnConnectionsInfo() const
{
    QList<QJsonObject> activeVpns;
    for (const QJsonObject &activeConn : m_activeConnectionsInfo) {
        if (activeConn.value("ConnectionType").toString().startsWith("vpn-")) {
            activeVpns.append(activeConn);
        }
    }

    return activeVpns;
}

const QJsonObject WiredDevice::activeWiredConnectionInfo() const
{
    QJsonObject activeWired;
    for (const QJsonObject &activeConn : m_activeConnectionsInfo) {
        // 当开启DSL连接时，类型为 pppoe
        if (activeConn.value("ConnectionType").toString() == "wired"
            || activeConn.value("ConnectionType").toString() == "pppoe") {
            activeWired = activeConn;
            break;
        }
    }

    return activeWired;
}

const QString WiredDevice::activeWiredConnName() const
{
    const QJsonObject &conn = activeWiredConnectionInfo();
    return conn.isEmpty() ? QString() : conn.value("ConnectionName").toString();
}

const QString WiredDevice::activeWiredConnUuid() const
{
    const QJsonObject &conn = activeWiredConnectionInfo();
    return conn.isEmpty() ? QString() : conn.value("ConnectionUuid").toString();
}

const QString WiredDevice::activeWiredConnSettingPath() const
{
    const QJsonObject &conn = activeWiredConnectionInfo();
    return conn.isEmpty() ? QString() : conn.value("SettingPath").toString();
}
