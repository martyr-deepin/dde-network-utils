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

#ifndef WIREDDEVICE_H
#define WIREDDEVICE_H

#include "networkdevice.h"

#include <QJsonObject>
#include <QDBusObjectPath>

namespace dde {

namespace network {

class WiredDevice : public NetworkDevice
{
    Q_OBJECT

public:
    explicit WiredDevice(const QJsonObject &info, QObject *parent = nullptr);

    const QList<QJsonObject> connections() const;
    void setConnections(const QList<QJsonObject> &connections);
    const QList<QJsonObject> activeConnections() const;
    const QList<QJsonObject> activeConnectionsInfo() const;
    void setActiveConnections(const QList<QJsonObject> &activeConns);
    void setActiveConnectionsInfo(const QList<QJsonObject> &activeConnInfoList);
    const QList<QJsonObject> activeVpnConnectionsInfo() const;
    const QJsonObject activeWiredConnectionInfo() const;
    const QString activeWiredConnName() const;
    const QString activeWiredConnUuid() const;
    const QString activeWiredConnSettingPath() const;

Q_SIGNALS:
    void connectionsChanged(const QList<QJsonObject> &connections) const;
    void activeWiredConnectionInfoChanged(const QJsonObject &connInfo) const;
    void activeConnectionsChanged(const QList<QJsonObject> &activeConns) const;
    void activeConnectionsInfoChanged(const QList<QJsonObject> &activeConnInfoList) const;

private:
    QList<QJsonObject> m_activeConnections;
    QList<QJsonObject> m_activeConnectionsInfo;
    QList<QJsonObject> m_connections;
};

}

}

#endif // WIREDDEVICE_H
