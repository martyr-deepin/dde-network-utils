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

#ifndef CONNECTIVITYCHECKER_H
#define CONNECTIVITYCHECKER_H

#include <QObject>
#include <QStringList>
#include <QTimer>

class QGSettings;

namespace dde {

namespace network {


class ConnectivityChecker : public QObject
{
    Q_OBJECT

public:
    explicit ConnectivityChecker(QObject *parent = nullptr);

Q_SIGNALS:
    void checkFinished(bool connectivity) const;

public Q_SLOTS:
    void startCheck();
private:
    QGSettings* m_settings;
    QStringList m_checkUrls;
    QTimer *m_checkConnectivityTimer;
};

}   // namespace network

}   // namespace dde

#endif // CONNECTIVITYCHECKER_H
