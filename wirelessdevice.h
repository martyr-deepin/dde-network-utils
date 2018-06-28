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

namespace dde {

namespace network {

class WirelessDevice : public NetworkDevice
{
    Q_OBJECT

public:
    explicit WirelessDevice(const QJsonObject &info, QObject *parent = 0);

    bool supportHotspot() const;
    const QString hotspotUuid() const;
    inline bool hotspotEnabled() const { return !m_activeHotspotInfo.isEmpty(); }
    const QJsonObject activeConnectionInfo() const { return m_activeConnInfo; }
    inline const QString activeConnName() const { return m_activeConnInfo.value("ConnectionName").toString(); }
    const QList<QJsonObject> connections() const { return m_connections; }

    const QJsonArray apList() const;
    inline const QJsonObject activeApInfo() const { return m_activeApInfo; }

Q_SIGNALS:
    void apAdded(const QJsonObject &apInfo) const;
    void apInfoChanged(const QJsonObject &apInfo) const;
    void apRemoved(const QJsonObject &apInfo) const;
    void activeApInfoChanged(const QJsonObject &activeApInfo) const;
    void activeConnectionChanged(const QJsonObject &oldConnInfo, const QJsonObject &newConnInfo) const;
    void hotspotEnabledChanged(const bool enabled) const;
    void needSecrets(const QString &info);
    void needSecretsFinished(const QString &info0, const QString &info1);

public Q_SLOTS:
    void setAPList(const QString &apList);
    void updateAPInfo(const QString &apInfo);
    void deleteAP(const QString &apInfo);
    void setActiveConnectionInfo(const QJsonObject &activeConnInfo);
    void setActiveHotspotInfo(const QJsonObject &hotspotInfo);
    void setConnections(const QList<QJsonObject> connections);
    void updateActiveApByName(const QString &ssid);

private:
    QJsonObject m_activeConnInfo;
    QJsonObject m_activeApInfo;
    QJsonObject m_activeHotspotInfo;
    QMap<QString, QJsonObject> m_apsMap;
    QList<QJsonObject> m_connections;
};

}

}

#endif // WIRELESSDEVICE_H
