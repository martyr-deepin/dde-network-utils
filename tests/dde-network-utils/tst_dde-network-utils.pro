QT       += dbus network

TARGET = tst_dde-network-utils
TEMPLATE = app

# 执行 make check 会运行 tests
CONFIG += testcase no_testcase_installs
# 生成统计信息
QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0

PKGCONFIG += dframeworkdbus gsettings-qt
CONFIG += c++11 link_pkgconfig
CONFIG -= app_bundle

LIBS += -lgtest

DEFINES += QT_DEPRECATED_WARNINGS

include(../../dde-network-utils/src.pri)

SOURCES += \
    main.cpp \
    tst_connecttivitychecker.cpp \
    tst_networkdevice.cpp \
    tst_networkmodel.cpp \
    tst_networkworker.cpp \
    tst_wireddevice.cpp \
    tst_wirelessdevice.cpp
INCLUDEPATH += ../../dde-network-utils

RESOURCES +=
