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

#ifndef NETWORKDEVICE_H
#define NETWORKDEVICE_H

#include <QObject>
#include <QJsonObject>
#include <QSet>
#include <QQueue>

namespace dde {

namespace network {

class NetworkDevice : public QObject
{
    Q_OBJECT

    friend class NetworkModel;

public:
    enum DeviceType
    {
        None,
        Wired,
        Wireless,
    };

    enum DeviceStatus
    {
        Unknown         = 0,
        Unmanaged       = 10,
        Unavailable     = 20,
        Disconnected    = 30,
        Prepare         = 40,
        Config          = 50,
        NeedAuth        = 60,
        IpConfig        = 70,
        IpCheck         = 80,
        Secondaries     = 90,
        Activated       = 100,
        Deactivation    = 110,
        Failed          = 120,
    };

public:
    virtual ~NetworkDevice();

    bool enabled() const { return m_enabled; }
    bool obtainIpFailed() const;
    DeviceType type() const { return m_type; }
    DeviceStatus status() const { return m_status; }
    QQueue<DeviceStatus> statusQueue() const { return m_statusQueue; }
    const QString statusString() const;
    const QString statusStringDetail() const;
    const QJsonObject info() const { return m_deviceInfo; }
    const QString path() const;
    const QString realHwAdr() const;
    const QString usingHwAdr() const;
    const QString interfaceName() const;

Q_SIGNALS:
    void removed() const;
    void statusChanged(DeviceStatus stat) const;
    void statusChanged(const QString &statStr) const;
    void statusQueueChanged(const QQueue<DeviceStatus> &statusQueue) const;
    void enableChanged(const bool enabled) const;
    void sessionCreated(const QString &sessionPath) const;

private Q_SLOTS:
    void setEnabled(const bool enabled);
    void updateDeviceInfo(const QJsonObject &devInfo);

protected:
    explicit NetworkDevice(const DeviceType type, const QJsonObject &info, QObject *parent = nullptr);

private Q_SLOTS:
    void setDeviceStatus(const int status);
    void enqueueStatus(DeviceStatus status);

private:
    const DeviceType m_type;
    DeviceStatus m_status;
    QQueue<DeviceStatus> m_statusQueue;
    QJsonObject m_deviceInfo;

    bool m_enabled;
};

}   // namespace network

}   // namespace dde

#endif // NETWORKDEVICE_H
