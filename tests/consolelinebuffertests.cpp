#include <QElapsedTimer>
#include <QDebug>

#include "gmock/gmock.h"
#include "models/consolelinebuffer.h"
#include "models/highlightingsmodel.h"
#include "ui/consoleview/consoleview.h"

TEST(ConsoleLineBufferTests, append_performanceTesting)
{
    HighlightingsModel model;
    ConsoleLineBuffer lineBuffer(&model);
    ConsoleView view;
    view.show();
    view.setModel(&lineBuffer);

    QElapsedTimer timer;

    quint64 timeElapsedMin = 0xFFFFFFFFFFFFFFFF;
    quint64 timeElapsedMax = 0;
    quint64 timeElapsedTotal = 0;

    for (int i = 0; i < 10000; i++)
    {
        timer.start();
        lineBuffer.append(QByteArray("012345679012345679012345679012345679012345679012345679012345679\r\n"));
        quint64 timeElapsed = timer.nsecsElapsed();

        if (timeElapsed < timeElapsedMin)
            timeElapsedMin = timeElapsed;

        if (timeElapsed > timeElapsedMax)
            timeElapsedMax = timeElapsed;

        timeElapsedTotal += timeElapsed;
    }

    qDebug() << timeElapsedMin << timeElapsedMax << timeElapsedTotal / 10000;

    ASSERT_TRUE(true);
}