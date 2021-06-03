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

#include "networkdevice.h"
#include "networkmodel.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>

#define MaxQueueSize 4

using namespace dde::network;

NetworkDevice::NetworkDevice(const DeviceType type, const QJsonObject &info, QObject *parent)
    : QObject(parent),

      m_type(type),
      m_status(Unknown),
      m_deviceInfo(info),
      m_enabled(true)
{
    updateDeviceInfo(info);
}

void NetworkDevice::setDeviceStatus(const int status)
{
    DeviceStatus stat = Unknown;

    switch (status)
    {
    case 10:    stat = Unmanaged;       break;
    case 20:    stat = Unavailable;     break;
    case 30:    stat = Disconnected;    break;
    case 40:    stat = Prepare;         break;
    case 50:    stat = Config;          break;
    case 60:    stat = NeedAuth;        break;
    case 70:    stat = IpConfig;        break;
    case 80:    stat = IpCheck;         break;
    case 90:    stat = Secondaries;     break;
    case 100:   stat = Activated;       break;
    case 110:   stat = Deactivation;    break;
    case 120:   stat = Failed;          break;
    }

    if (m_status != stat)
    {
        m_status = stat;

        enqueueStatus(m_status);

        Q_EMIT statusChanged(m_status);
        Q_EMIT statusChanged(statusString());
        Q_EMIT statusQueueChanged(m_statusQueue);
    }
}

// prepre-status, pre-status, now-status
void NetworkDevice::enqueueStatus(NetworkDevice::DeviceStatus status)
{
    if (m_statusQueue.size() == MaxQueueSize) {
        m_statusQueue.dequeue();
    }

    m_statusQueue.enqueue(status);
}

const QString NetworkDevice::statusString() const
{
    switch (m_status)
    {
    case Unmanaged:
    case Unavailable:
    case Disconnected:  return tr("Disconnected");
    case Prepare:
    case Config:        return tr("Connecting");
    case NeedAuth:      return tr("Authenticating");
    case IpConfig:
    case IpCheck:       return tr("Obtaining Address");
    case Activated:     return tr("Connected");
    case Deactivation:
        return tr("Disconnected");
    case Failed:
        return tr("Failed");
    default:;
    }

    return QString();
}

const QString NetworkDevice::statusStringDetail() const
{
    if (!m_enabled) {
        return tr("Device disabled");
    }

    if (m_status == DeviceStatus::Activated && NetworkModel::connectivity() != Connectivity::Full) {
        return tr("Connected but no Internet access");
    }

    if (obtainIpFailed()) {
        //确认 没有获取IP显示未连接状态（DHCP服务关闭）
        return tr("Not connected");
    }

    switch (m_status)
    {
    case Unknown:
    case Unmanaged:
    case Unavailable: {
        switch (m_type) {
        case DeviceType::None: return QString();
        case DeviceType::Wired: return tr("Network cable unplugged");
        default:;
        }
    }
    case Disconnected:  return tr("Not connected");
    case Prepare:
    case Config:        return tr("Connecting");
    case NeedAuth:      return tr("Authenticating");
    case IpConfig:
    case IpCheck:
    case Secondaries:   return tr("Obtaining IP address");
    case Activated:     return tr("Connected");
    case Deactivation:
        return tr("Disconnected");
    case Failed:
        return tr("Failed");
    default:;
    }

    return QString();
}

NetworkDevice::~NetworkDevice()
{
    Q_EMIT removed();
}

bool NetworkDevice::obtainIpFailed() const
{
    if (m_statusQueue.isEmpty()) {
        return false;
    }

    // 判断为获取IP地址失败需要以下条件
    return (m_statusQueue.size() == MaxQueueSize
            && m_statusQueue.at(MaxQueueSize - 1) == DeviceStatus::Disconnected // 最后(当前)一个状态为未连接
            && m_statusQueue.at(MaxQueueSize - 2) == DeviceStatus::Failed // 上一个状态为失败
            && m_statusQueue.contains(DeviceStatus::Config) // 包含Config和IpConfig
            && m_statusQueue.contains(DeviceStatus::IpConfig));
}

void NetworkDevice::setEnabled(const bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        m_statusQueue.clear();
        Q_EMIT enableChanged(m_enabled);
    }
}

const QString NetworkDevice::path() const
{
    return m_deviceInfo.value("Path").toString();
}

const QString NetworkDevice::realHwAdr() const
{
    return m_deviceInfo.value("HwAddress").toString();
}

const QString NetworkDevice::usingHwAdr() const
{
    const auto &hwAdr = m_deviceInfo.value("HwAddress").toString();
    const auto &clonedAdr = m_deviceInfo.value("ClonedAddress").toString();

    return clonedAdr.isEmpty() ? hwAdr : clonedAdr;
}

const QString NetworkDevice::interfaceName() const
{
    return m_deviceInfo.value("Interface").toString();
}

void NetworkDevice::updateDeviceInfo(const QJsonObject &devInfo)
{
    m_deviceInfo = devInfo;

    setDeviceStatus(m_deviceInfo.value("State").toInt());
}
