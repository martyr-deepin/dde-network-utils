#-------------------------------------------------
#
# Project created by QtCreator 2018-06-13T15:28:29
#
#-------------------------------------------------

QT       -= gui
QT       += dbus

TARGET = dde-network-utils
TEMPLATE = lib
CONFIG += link_pkgconfig c++11 create_pc create_prl no_install_prl
PKGCONFIG      += dframeworkdbus

DEFINES += DDENETWORKUTILS_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

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
