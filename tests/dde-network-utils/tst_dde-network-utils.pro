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

ARCH = $$QMAKE_HOST.arch
message(Current Architecture)
message($$ARCH)

isEqual(ARCH, sw_64) | isEqual(ARCH, mips64) | isEqual(ARCH, mips32) | isEqual(ARCH, mips64el) {
    message(not support sanitize)
} else {
    DEFINES += SANITIZER_CHECK
    QMAKE_CXXFLAGS += -g -fsanitize=address -fsanitize-recover=address
    QMAKE_LFLAGS += -g -fsanitize=address -fsanitize-recover=address
}
