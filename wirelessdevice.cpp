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

WirelessDevice::WirelessDevice(const QJsonObject &info, QObject *parent)
    : NetworkDevice(NetworkDevice::Wireless, info, parent)
{
}

bool WirelessDevice::supportHotspot() const
{
    return info()["SupportHotspot"].toBool();
}

const QString WirelessDevice::hotspotUuid() const
{
    Q_ASSERT(hotspotEnabled());

    return m_activeHotspotInfo.value("ConnectionUuid").toString();
}

const QJsonArray WirelessDevice::apList() const
{
    QJsonArray apArray;
    for (auto ap : m_apsMap.values()) {
        apArray.append(ap);
    }
    return apArray;
}

void WirelessDevice::setAPList(const QString &apList)
{
    QMap<QString, QJsonObject> apsMapOld = m_apsMap;
    m_apsMap.clear();

    const QJsonArray &apArray = QJsonDocument::fromJson(apList.toUtf8()).array();
    for (auto item : apArray) {
        const QJsonObject &ap = item.toObject();
        const QString &path = ap.value("Path").toString();

        if (!path.isEmpty()) {
            if (ap.value("Ssid").toString() == activeApSsid() &&
                    ap.value("Strength").toInt() > activeApStrength()) {
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

    setActiveApBySsid(activeApSsidByActiveConnUuid(activeConnUuid()));
}

void WirelessDevice::updateAPInfo(const QString &apInfo)
{
    const auto &ap = QJsonDocument::fromJson(apInfo.toUtf8()).object();
    const auto &path = ap.value("Path").toString();

    if (!path.isEmpty()) {
        if (ap.value("Ssid").toString() == activeApSsid() &&
                ap.value("Strength").toInt() > activeApStrength()) {
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
    const auto &path = ap.value("Path").toString();

    if (!path.isEmpty()) {
        if (m_apsMap.contains(path)) {
            m_apsMap.remove(path);
            Q_EMIT apRemoved(ap);
        }
    }
}

void WirelessDevice::setActiveConnectionInfo(const QJsonObject &activeConnInfo)
{
    if (m_activeConnInfo != activeConnInfo)
    {
        const QJsonObject oldConnInfo = m_activeConnInfo;
        m_activeConnInfo = activeConnInfo;

        if (m_activeConnInfo.isEmpty()) {
            m_activeApInfo = QJsonObject();
            Q_EMIT activeApInfoChanged(m_activeApInfo);
        } else {
            setActiveApBySsid(activeApSsidByActiveConnUuid(activeConnUuid()));
        }

        Q_EMIT activeConnectionChanged(oldConnInfo, m_activeConnInfo);
    }
}

void WirelessDevice::setActiveHotspotInfo(const QJsonObject &hotspotInfo)
{
    Q_ASSERT(supportHotspot());

    const bool changed = m_activeHotspotInfo.isEmpty() != hotspotInfo.isEmpty();

    m_activeHotspotInfo = hotspotInfo;

    if (changed)
        Q_EMIT hotspotEnabledChanged(hotspotEnabled());
}

void WirelessDevice::setConnections(const QList<QJsonObject> connections)
{
    m_connections = connections;
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
