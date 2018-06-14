QT       -= gui
QT       += dbus

TARGET = dde-network-utils
TEMPLATE = lib
CONFIG += link_pkgconfig c++11 create_pc create_prl no_install_prl
PKGCONFIG      += dframeworkdbus

DEFINES += DDENETWORKUTILS_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    networkmodel.cpp \
    networkworker.cpp \
    networkdevice.cpp \
    wirelessdevice.cpp \
    wireddevice.cpp

HEADERS += \
    dde-network-utils_global.h \
    networkmodel.h \
    networkworker.h \
    networkdevice.h \
    wirelessdevice.h \
    wireddevice.h

unix {
    target.path = /usr/lib

    includes.files = *.h
    includes.path = /usr/include/libddenetworkutils

    QMAKE_PKGCONFIG_NAME = libddenetworkutils
    #QMAKE_PKGCONFIG_VERSION = $$VERSION
    QMAKE_PKGCONFIG_DESCRIPTION = libddenetworkutils
    QMAKE_PKGCONFIG_INCDIR = $$includes.path
    QMAKE_PKGCONFIG_LIBDIR = $$target.path
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig

    INSTALLS += includes target
}

SUBDIRS += \
    dde-network-utils.pro

DISTFILES += \
    dde-network-utils.pro.user
