#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QDebug>

#include <sanitizer/asan_interface.h>

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QCoreApplication app(argc, argv);
    qInfo() << "start dde-network-utils test cases ..............";

    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    qInfo() << "end dde-network-utils test cases ..............";

    __sanitizer_set_report_path("asan.log");

    return ret;
}
