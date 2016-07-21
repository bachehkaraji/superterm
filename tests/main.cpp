#include <QApplication>

#include "gmock/gmock.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

// EOF <stefan@scheler.com>
