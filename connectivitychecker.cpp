/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#include "connectivitychecker.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QScopedPointer>
#include <QGSettings>

static const QStringList CheckUrls {
    "https://www.baidu.com",
    "https://www.bing.com",
    "https://www.google.com",
    "https://www.amazon.com",
    "https://github.com",
};

#define TIMERINTERVAL (60 * 1000) // 一分钟
#define TIMEOUT (30 * 1000) // 30s超时

using namespace dde::network;

ConnectivityChecker::ConnectivityChecker(QObject *parent) : QObject(parent)
{
    if(QGSettings::isSchemaInstalled("com.deepin.dde.network-utils")) {
        m_settings = new QGSettings("com.deepin.dde.network-utils","/com/deepin/dde/network-utils/", this);
        m_checkUrls = m_settings->get("network-checker-urls").toStringList();
        connect(m_settings,&QGSettings::changed, [ = ] (const QString key) {
            if (key == "network-checker-urls") {
                m_checkUrls = m_settings->get("network-checker-urls").toStringList();
            }
        });
    }

   m_checkConnectivityTimer =  new QTimer(this);
   m_checkConnectivityTimer->setInterval(TIMERINTERVAL);

   connect(m_checkConnectivityTimer, &QTimer::timeout, this,
               &ConnectivityChecker::startCheck);

      m_checkConnectivityTimer->start();
}

void ConnectivityChecker::startCheck()
{
    QNetworkAccessManager nam;
    if (m_checkUrls.isEmpty()) {
        m_checkUrls = CheckUrls;
    }
    for (auto url : m_checkUrls) {
        QScopedPointer<QNetworkReply> reply(nam.get(QNetworkRequest(QUrl(url))));
        qDebug() << "Check connectivity using url:" << url;

        // Do not use waitForReadyRead to block thread,
        // the QNetworkReply is not implement this virtual method
        // and it will just return false immediately
//        reply->waitForReadyRead(-1);

        // Blocking, about 30 second to timeout
        QTimer timer;
        timer.setSingleShot(true);
        QEventLoop synchronous;
        connect(&timer, &QTimer::timeout, &synchronous, &QEventLoop::quit);
        connect(&nam, &QNetworkAccessManager::finished, &synchronous, &QEventLoop::quit);
        timer.start(TIMEOUT);
        synchronous.exec();

        reply->close();
        if (timer.isActive()) {
            timer.stop();
            if (reply->error() == QNetworkReply::NoError &&
                    (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200 ||
                    reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 204)) {
                qDebug() << "Connected to url:" << url;
                Q_EMIT checkFinished(true);
                return;
            }
        } else {
            qDebug() << "Timeout";
        }

        qDebug() << "Failed to connect url:" << url;
    }

    Q_EMIT checkFinished(false);
}
