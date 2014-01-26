#include <cstdio>
#include <QMessageBox>
#include <QSerialPort>
#include <QPushButton>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "consoletab.h"
#include "consoleview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    m_ui->statusBar->showMessage("Ready");
}


MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::showAbout(void)
{
    QMessageBox::about(this, tr("About"),
                 tr("<p>The <b>Scribble</b> example shows how to use QMainWindow as the "
                    "base widget for an application, and how to reimplement some of "
                    "QWidget's event handlers to receive the events generated for "
                    "the application's widgets:</p><p> We reimplement the mouse event "
                    "handlers to facilitate drawing, the paint event handler to "
                    "update the application and the resize event handler to optimize "
                    "the application's appearance. In addition we reimplement the "
                    "close event handler to intercept the close events before "
                    "terminating the application.</p><p> The example also demonstrates "
                    "how to use QPainter to draw an image in real time, as well as "
                    "to repaint widgets.</p>"));
}

// EOF <stefan@scheler.com>
