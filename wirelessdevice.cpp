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

    return m_hotspotInfo.value("ConnectionUuid").toString();
}

const QJsonArray WirelessDevice::apList() const
{
    QJsonArray apArray;
    for (auto ap : m_apsMap.values()) {
        apArray.append(ap);
    }
    return apArray;
}

int WirelessDevice::activeApStrgength() const
{
    if (!m_apsMap.isEmpty() && !m_activeApInfo.isEmpty()) {
        /* TODO: should use the Path of AP as a condition instead of a name here */
        const QString &activeConnectionName = m_activeApInfo.value("ConnectionName").toString();
        for (auto ap : m_apsMap.values()) {
            if (ap.value("Ssid").toString() == activeConnectionName) {
                return ap.value("Strength").toInt();
            }
        }
    }
    return 0;
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
}

void WirelessDevice::updateAPInfo(const QString &apInfo)
{
    const auto &ap = QJsonDocument::fromJson(apInfo.toUtf8()).object();
    const auto &path = ap.value("Path").toString();

    if (!path.isEmpty()) {
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

void WirelessDevice::setActiveApInfo(const QJsonObject &apInfo)
{
    if (m_activeApInfo != apInfo)
    {
        const QJsonObject oldApInfo = m_activeApInfo;
        m_activeApInfo = apInfo;

        Q_EMIT activeApChanged(oldApInfo, m_activeApInfo);
    }
}

void WirelessDevice::setHotspotInfo(const QJsonObject &hotspotInfo)
{
    Q_ASSERT(supportHotspot());

    const bool changed = m_hotspotInfo.isEmpty() != hotspotInfo.isEmpty();

    m_hotspotInfo = hotspotInfo;

    if (changed)
        Q_EMIT hotspotEnabledChanged(hotspotEnabled());
}
