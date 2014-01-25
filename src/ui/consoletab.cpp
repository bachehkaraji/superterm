#include <QSerialPort>
#include <QDebug>
#include <iostream>
#include <cstdio>
#include <QSerialPortInfo>

#include "consoletab.h"
#include "ui_consoletab.h"
#include "consoletabwidget.h"

ConsoleTab::ConsoleTab(QTabWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ConsoleTab),
    m_parent(parent),
    m_port(NULL),
    m_lastTabIndex(0)
{
    m_ui->setupUi(this);

    // fill port combo box
    refreshPorts();

    // fill baud combo box
    foreach (const qint32 baudrate, QSerialPortInfo::standardBaudRates())
    {
        m_ui->comboBaudrates->addItem(QString::number(baudrate));
    }

}

ConsoleTab::~ConsoleTab()
{
    delete m_ui;
    delete m_port;
}

void ConsoleTab::refreshPorts(void)
{
    m_ui->comboPorts->clear();
    m_ui->comboPorts->addItem(tr("Select port"));

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        m_ui->comboPorts->addItem(QString("%1 (%2) [%3]").arg(info.portName(), info.description(), info.manufacturer()));
        qDebug() << info.systemLocation();
        qDebug() << info.vendorIdentifier();
        qDebug() << info.productIdentifier();
    }
}

void ConsoleTab::toggleFullScreen(void)
{
    if (!isFullScreen())
    {
        m_lastTabIndex = m_parent->currentIndex();
        setParent(0);
        showFullScreen();
    }
    else
    {
        setParent(m_parent);
        m_parent->insertTab(m_lastTabIndex, this, "foo");
        m_parent->setCurrentIndex(m_lastTabIndex);
        m_ui->consoleView->setFocus();
    }
}

void ConsoleTab::onConnectClicked(void)
{
    const QString portName = m_ui->comboPorts->currentText();

    m_port = new QSerialPort(portName);

    if (m_port->open(QIODevice::ReadWrite)){
        puts("PORT OPEN");
    }
    else
    {
        qDebug() << portName;
    }
    connect(m_port, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));

    m_ui->btnBar->hide();
    m_ui->consoleView->setEnabled(true);
    m_ui->consoleView->setFocus();
}

void ConsoleTab::onDataAvailable(void)
{
    QByteArray data = m_port->readAll();
    for (int p = 0; p < data.size(); p++)
    {
        printf("0x%02x ", data.at(p));
    }
    printf("\n");

    QString str = data;
    std::cout << "NEW DATA [" << str.toStdString() << "]" << std::endl;
    str = str.replace("\r", "<br>");
    str = str.replace(" ", "&nbsp;");

    m_ui->consoleView->moveCursor(QTextCursor::End);
    m_ui->consoleView->textCursor().insertHtml(str);
    m_ui->consoleView->moveCursor(QTextCursor::End);
}

void ConsoleTab::onKeyPressed(QString text)
{
    puts("KEY");
    QByteArray data;
    data.append(text);
    m_port->write(data);
}
