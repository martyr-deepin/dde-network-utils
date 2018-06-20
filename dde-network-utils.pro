QT       -= gui
QT       += dbus

TARGET = dde-network-utils
TEMPLATE = lib
CONFIG += link_pkgconfig c++11 create_pc create_prl no_install_prl no_keywords
PKGCONFIG      += dframeworkdbus

DEFINES += DDENETWORKUTILS_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    $$PWD/networkmodel.cpp \
    $$PWD/networkworker.cpp \
    $$PWD/networkdevice.cpp \
    $$PWD/wirelessdevice.cpp \
    $$PWD/wireddevice.cpp

HEADERS += \
    $$PWD/networkmodel.h \
    $$PWD/networkworker.h \
    $$PWD/networkdevice.h \
    $$PWD/wirelessdevice.h \
    $$PWD/wireddevice.h

target.path = /usr/lib

includes.files += *.h
includes.files += \
    $$PWD/NetworkModel \
    $$PWD/NetworkWorker \
    $$PWD/NetworkDevice \
    $$PWD/WirelessDevice \
    $$PWD/WiredDevice

includes.path = /usr/include/libddenetworkutils

QMAKE_PKGCONFIG_NAME = libddenetworkutils
QMAKE_PKGCONFIG_DESCRIPTION = libddenetworkutils
QMAKE_PKGCONFIG_INCDIR = $$includes.path
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_DESTDIR = pkgconfig

INSTALLS += includes target

SUBDIRS += \
    dde-network-utils.pro
