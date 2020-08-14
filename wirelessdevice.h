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

#ifndef WIRELESSDEVICE_H
#define WIRELESSDEVICE_H

#include "networkdevice.h"

#include <QMap>
#include <QJsonArray>

#include <com_deepin_daemon_network.h>

using NetworkInter = com::deepin::daemon::Network;

namespace dde {

namespace network {

class WirelessDevice : public NetworkDevice
{
    Q_OBJECT

public:
    explicit WirelessDevice(const QJsonObject &info, QObject *parent = nullptr);

    bool supportHotspot() const;
    inline bool hotspotEnabled() const { return !m_activeHotspotInfo.isEmpty(); }
    inline QJsonObject activeHotspotInfo() const { return m_activeHotspotInfo; }
    const QString activeHotspotUuid() const;

    const QList<QJsonObject> activeConnections() const;
    const QList<QJsonObject> activeConnectionsInfo() const;
    const QList<QJsonObject> activeVpnConnectionsInfo() const;
    const QJsonObject activeWirelessConnectionInfo() const;
    const QString activeWirelessConnName() const;
    const QString activeWirelessConnUuid() const;
    const QString activeWirelessConnSettingPath() const;

    const QList<QJsonObject> connections() const { return m_connections; }
    const QList<QJsonObject> hotspotConnections() const { return m_hotspotConnections; }

    const QJsonArray apList() const;
    inline const QJsonObject activeApInfo() const { return m_activeApInfo; }
    inline const QString activeApSsid() const { return m_activeApInfo.value("Ssid").toString(); }
    inline const QString activeApPath() const { return m_activeApInfo.value("Path").toString(); }
    inline int activeApStrength() const { return m_activeApInfo.value("Strength").toInt(); }
    void updateWirlessAp();
    void WirelessUpdate(const QJsonValue &WirelessData); //该接口给networkmodel使用
    
Q_SIGNALS:
    void apAdded(const QJsonObject &apInfo) const;
    void apInfoChanged(const QJsonObject &apInfo) const;
    void apRemoved(const QJsonObject &apInfo) const;
    void activeApInfoChanged(const QJsonObject &activeApInfo) const;
    void activeWirelessConnectionInfoChanged(const QJsonObject &connInfo) const;
    void activeConnectionsChanged(const QList<QJsonObject> &activeConns) const;
    void activeConnectionsInfoChanged(const QList<QJsonObject> &activeConnInfoList) const;
    void hotspotEnabledChanged(const bool enabled) const;
    void needSecrets(const QString &info);
    void needSecretsFinished(const QString &info0, const QString &info1);
    void activateAccessPointFailed(const QString &apPath, const QString &uuid);
    void connectionsChanged(const QList<QJsonObject> &connections) const;
    void hostspotConnectionsChanged(const QList<QJsonObject> &connections) const;

public Q_SLOTS:
    void setAPList(const QString &apList);
    void updateAPInfo(const QString &apInfo);
    void deleteAP(const QString &apInfo);
    void setActiveConnections(const QList<QJsonObject> &activeConns);
    void setActiveConnectionsInfo(const QList<QJsonObject> &activeConnsInfo);
    void setActiveHotspotInfo(const QJsonObject &hotspotInfo);
    void setActiveApBySsid(const QString &ssid);
    void setConnections(const QList<QJsonObject> &connections);
    void setHotspotConnections(const QList<QJsonObject> &hotspotConnections);

private:
    QString activeApSsidByActiveConnUuid(const QString &activeWirelessConnUuid);

private:
    QList<QJsonObject> m_activeConnections;
    QList<QJsonObject> m_activeConnectionsInfo;
    QJsonObject m_activeApInfo;
    QJsonObject m_activeHotspotInfo;
    QMap<QString, QJsonObject> m_apsMap;
    QList<QJsonObject> m_connections;
    QList<QJsonObject> m_hotspotConnections;
    QMap<QString,QString> m_ssidDatas;

    NetworkInter m_networkInter;
};

}

}

#endif // WIRELESSDEVICE_H
