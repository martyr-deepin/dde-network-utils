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

#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include "networkdevice.h"
#include "connectivitychecker.h"

#include <QMap>
#include <QTimer>
#include <QDBusObjectPath>
#include <QThread>

namespace dde {

namespace network {

struct ProxyConfig
{
    uint port;
    QString type;
    QString url;
    QString username;
    QString password;
};

enum Connectivity
{
    UnknownConnectivity = 0,
    NoConnectivity = 1,
    Portal = 2,
    Limited = 3,
    Full = 4
};

class NetworkDevice;
class NetworkWorker;
class WirelessDevice;
class NetworkModel : public QObject
{
    Q_OBJECT

    friend class NetworkWorker;

public:
    explicit NetworkModel(QObject *parent = nullptr);
    ~NetworkModel();
    ProxyConfig getChainsProxy() { return m_chainsProxy;}

    bool vpnEnabled() const { return m_vpnEnabled; }
    bool appProxyExist() const { return m_appProxyExist; }

    static Connectivity connectivity() { return m_Connectivity; }

    const ProxyConfig proxy(const QString &type) const { return m_proxies[type]; }
    const QString autoProxy() const { return m_autoProxy; }
    const QString proxyMethod() const { return m_proxyMethod; }
    const QString ignoreHosts() const { return m_proxyIgnoreHosts; }
    const QList<NetworkDevice *> devices() const { return m_devices; }
    const QList<QJsonObject> vpns() const { return m_connections.value("vpn"); }
    const QList<QJsonObject> wireds() const { return m_connections.value("wired"); }
    const QList<QJsonObject> wireless() const { return m_connections.value("wireless"); }
    const QList<QJsonObject> pppoes() const { return m_connections.value("pppoe"); }
    const QList<QJsonObject> hotspots() const { return m_connections.value("wireless-hotspot"); }
    const QList<QJsonObject> activeConnInfos() const { return m_activeConnInfos; }
    const QList<QJsonObject> activeConns() const { return m_activeConns; }
    const QString connectionUuidByPath(const QString &connPath) const;
    const QString connectionNameByPath(const QString &connPath) const;
    const QString connectionUuidByApInfo(const QJsonObject &apInfo) const;
    const QString activeConnUuidByInfo(const QString &devPath, const QString &id) const;
    const QJsonObject connectionByUuid(const QString &uuid) const;
    const QJsonObject connectionByPath(const QString &connPath) const;
    const QJsonObject activeConnObjectByUuid(const QString &uuid) const;

Q_SIGNALS:
    void connectionListChanged() const;
    void deviceEnableChanged(const QString &device, const bool enabled) const;
    void autoProxyChanged(const QString &proxy) const;
    void proxyChanged(const QString &type, const ProxyConfig &config) const;
    void proxyMethodChanged(const QString &proxyMethod) const;
    void proxyIgnoreHostsChanged(const QString &hosts) const;
    void requestDeviceStatus(const QString &devPath) const;
    void activeConnectionsChanged(const QList<QJsonObject> &conns) const;
    void activeConnInfoChanged(const QList<QJsonObject> &infos) const;
    void vpnEnabledChanged(const bool enabled) const;
    void deviceListChanged(const QList<NetworkDevice *> devices) const;
    void unhandledConnectionSessionCreated(const QString &device, const QString &sessionPath) const;
    void chainsTypeChanged(const QString &type) const;
    void chainsAddrChanged(const QString &addr) const;
    void chainsPortChanged(const uint port) const;
    void chainsUsernameChanged(const QString &username) const;
    void chainsPasswdChanged(const QString &passwd) const;
    void appProxyExistChanged(const bool appProxyExist) const;
    void needSecrets(const QString &info);
    void needSecretsFinished(const QString &info0, const QString &info1);
    void connectivityChanged(const Connectivity connectivity) const;

    // Private Signals
    // Need ensure the checker thread is running
    // before emit this signal
    void needCheckConnectivitySecondary() const;

private Q_SLOTS:
    void onActivateAccessPointDone(const QString &devPath, const QString &apPath, const QString &uuid, const QDBusObjectPath path);
    void onVPNEnabledChanged(const bool enabled);
    void onAppProxyExistChanged(bool appProxyExist);
    void onProxiesChanged(const QString &type, const QString &url, const uint port);
    void onAutoProxyChanged(const QString &proxy);
    void onProxyMethodChanged(const QString &proxyMethod);
    void onProxyIgnoreHostsChanged(const QString &hosts);
    void onDevicesChanged(const QString &devices);
    void onConnectionListChanged(const QString &conns);
    void onActiveConnInfoChanged(const QString &conns);
    void onActiveConnectionsChanged(const QString &conns);
    void onConnectionSessionCreated(const QString &device, const QString &sessionPath);
    void onDeviceAPListChanged(const QString &device, const QString &apList);
    void onDeviceAPInfoChanged(const QString &device, const QString &apInfo);
    void onDeviceAPRemoved(const QString &device, const QString &apInfo);
    void onDeviceEnableChanged(const QString &device, const bool enabled);
    void onChainsTypeChanged(const QString &type);
    void onChainsAddrChanged(const QString &addr);
    void onChainsPortChanged(const uint port);
    void onChainsUserChanged(const QString &user);
    void onChainsPasswdChanged(const QString &passwd);
    void onNeedSecrets(const QString &info);
    void onNeedSecretsFinished(const QString &info0, const QString &info1);
    void onConnectivityChanged(int connectivity);
    void onConnectivitySecondaryCheckFinished(bool connectivity);
    /**
     * @def WirelessAccessPointsChanged
     * @brief 后端数据入口处,属性的修改会调用该函数
     * @param WirelessList
     */
    void WirelessAccessPointsChanged(const QString &WirelessList);
private:
    bool containsDevice(const QString &devPath) const;
    NetworkDevice *device(const QString &devPath) const;
    void updateWiredConnInfo();

private:
    NetworkDevice *m_lastSecretDevice;
    ConnectivityChecker *m_connectivityChecker;
    QThread *m_connectivityCheckThread;

    bool m_vpnEnabled;
    bool m_appProxyExist;

    QString m_proxyMethod;
    QString m_proxyIgnoreHosts;
    QString m_autoProxy;
    ProxyConfig m_chainsProxy;
    QList<NetworkDevice *> m_devices;
    QList<QJsonObject> m_activeConnInfos;
    QList<QJsonObject> m_activeConns;
    QMap<QString, ProxyConfig> m_proxies;
    QMap<QString, QList<QJsonObject>> m_connections;

    static Connectivity m_Connectivity;
};

}   // namespace network

}   // namespace dde

#endif // NETWORKMODEL_H
