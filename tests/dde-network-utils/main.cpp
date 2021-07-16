#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QDebug>

#ifdef SANITIZER_CHECK
#include <sanitizer/asan_interface.h>
#endif

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QCoreApplication app(argc, argv);
    qInfo() << "start dde-network-utils test cases ..............";

    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    qInfo() << "end dde-network-utils test cases ..............";

#ifdef SANITIZER_CHECK
    __sanitizer_set_report_path("asan.log");
#endif

    return ret;
}
