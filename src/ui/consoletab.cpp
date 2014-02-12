#include <QColorDialog>
#include <QSerialPort>
#include <QDebug>
#include <iostream>
#include <QMenu>
#include <QMouseEvent>
#include <cstdio>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QFontDialog>
#include <QSettings>
#include <QFile>
#if defined(Q_OS_WIN)
#include <windows.h>
#endif

#include "globals.h"
#include "consoletab.h"
#include "ui_consoletab.h"
#include "consoletabwidget.h"
#include "highlightsframe.h"

quint32   CConsoleTab::m_u32counter = 1;

Q_DECLARE_METATYPE(QSerialPort::DataBits)
Q_DECLARE_METATYPE(QSerialPort::StopBits)
Q_DECLARE_METATYPE(QSerialPort::Parity)
Q_DECLARE_METATYPE(QSerialPort::FlowControl)

void dumpDCB(const char *szFileName)
{
#if defined(Q_OS_WIN)
    HANDLE h = CreateFileA(szFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING,  FILE_ATTRIBUTE_NORMAL, 0);

    DCB dcb;

    GetCommState(h, &dcb);

    printf("dcb.DCBlength:         0x%08x\n", dcb.DCBlength);
    printf("dcb.BaudRate:          0x%08x (%lu)\n", dcb.BaudRate, dcb.BaudRate);
    printf("dcb.fBinary:           0x%08x\n", dcb.fBinary);
    printf("dcb.fParity:           0x%08x\n", dcb.fParity);
    printf("dcb.fOutxCtsFlow:      0x%08x\n", dcb.fOutxCtsFlow);
    printf("dcb.fOutxDsrFlow:      0x%08x\n", dcb.fOutxDsrFlow);
    printf("dcb.fDtrControl:       0x%08x\n", dcb.fDtrControl);
    printf("dcb.fDsrSensitivity:   0x%08x\n", dcb.fDsrSensitivity);
    printf("dcb.fTXContinueOnXoff: 0x%08x\n", dcb.fTXContinueOnXoff);
    printf("dcb.fOutX:             0x%08x\n", dcb.fOutX);
    printf("dcb.fInX:              0x%08x\n", dcb.fInX);
    printf("dcb.fErrorChar:        0x%08x\n", dcb.fErrorChar);
    printf("dcb.fNull:             0x%08x\n", dcb.fNull);
    printf("dcb.fRtsControl:       0x%08x\n", dcb.fRtsControl);
    printf("dcb.fAbortOnError:     0x%08x\n", dcb.fAbortOnError);
    printf("dcb.XonLim:            0x%04x\n", dcb.XonLim);
    printf("dcb.XoffLim:           0x%04x\n", dcb.XoffLim);
    printf("dcb.ByteSize:          0x%02x\n", dcb.ByteSize);
    printf("dcb.Parity:            0x%02x\n", dcb.Parity);
    printf("dcb.StopBits:          0x%02x\n", dcb.StopBits);

    printf("dcb.XonChar:           0x%02x\n", dcb.XonChar);
    printf("dcb.XoffChar:          0x%02x\n", dcb.XoffChar);
    printf("dcb.ErrorChar:         0x%02x\n", dcb.ErrorChar);
    printf("dcb.EofChar:           0x%02x\n", dcb.EofChar);
    printf("dcb.EvtChar:           0x%02x\n", dcb.EvtChar);

    CloseHandle(h);
#endif
}

CConsoleTab::CConsoleTab(CPortEnumerator* pe, CConsoleTabWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::CConsoleTab),
    m_pe(pe),
    m_parent(parent),
    m_port(NULL),
    m_logFile(NULL),
    m_lastTabIndex(0)
{
    m_ui->setupUi(this);

    fillComboBoxes();

    // load font from settings
    QSettings settings;
    QFont consoleFont;
    QColor color = QColor(settings.value("background").toString());
    consoleFont.fromString(settings.value("font").toString());
    setConsoleFont(consoleFont);
    setBackgroundColor(color);
}

CConsoleTab::~CConsoleTab()
{
    delete m_ui;
    delete m_port;
}

void CConsoleTab::fillComboBoxes(void)
{
    QComboBox *combo = NULL;

    m_ui->comboPorts->setPortEnumerator(m_pe);

    // fill baud rates
    combo = m_ui->comboBaudRates;
#if 0
    foreach (const qint32 baudrate, QSerialPortInfo::standardBaudRates())
    {
        combo->addItem(QString::number(baudrate), QVariant(baudrate));
    }
#endif
    // just use more common baud rates for now
    combo->clear();
    combo->addItem(tr("Select baud rate"));
    combo->addItem("9600", QVariant(9600));
    combo->addItem("19200", QVariant(19200));
    combo->addItem("38400", QVariant(38400));
    combo->addItem("57600", QVariant(57600));
    combo->addItem("115200", QVariant(115200));
    combo->addItem("custom", QVariant(0));


    // fill data bits
    combo = m_ui->comboDataBits;
    combo->clear();
    combo->addItem("5", qVariantFromValue(QSerialPort::Data5));
    combo->addItem("6", qVariantFromValue(QSerialPort::Data6));
    combo->addItem("7", qVariantFromValue(QSerialPort::Data7));
    combo->addItem("8", qVariantFromValue(QSerialPort::Data8));
    combo->setCurrentIndex(3);

    // fill parity
    combo = m_ui->comboParity;
    combo->clear();
    combo->addItem(tr("None"), qVariantFromValue(QSerialPort::NoParity));
    combo->addItem(tr("Even"), qVariantFromValue(QSerialPort::OddParity));
    combo->addItem(tr("Space"), qVariantFromValue(QSerialPort::SpaceParity));
    combo->addItem(tr("Mark"), qVariantFromValue(QSerialPort::MarkParity));

    // fill stop bits
    combo = m_ui->comboStopBits;
    combo->clear();
    combo->addItem("1", qVariantFromValue(QSerialPort::OneStop));
    combo->addItem("1.5", qVariantFromValue(QSerialPort::OneAndHalfStop));
    combo->addItem("2", qVariantFromValue(QSerialPort::TwoStop));
    combo->setCurrentIndex(0);

    // fill flow control
    combo = m_ui->comboFlowControl;
    combo->clear();
    combo->addItem(tr("None"), qVariantFromValue(QSerialPort::NoFlowControl));
    combo->addItem(tr("Xon/Xoff"), qVariantFromValue(QSerialPort::SoftwareControl));
    combo->addItem(tr("Hardware"), qVariantFromValue(QSerialPort::HardwareControl));
    combo->setCurrentIndex(0);
}

void CConsoleTab::toggleFullScreen(void)
{
    static QString sLastTitle;
    if (!isFullScreen())
    {
        m_lastTabIndex = m_parent->currentIndex();
        sLastTitle = m_parent->tabText(m_lastTabIndex);
        m_parent->addNewTab(); // add dummy tab
        setParent(0);
        showFullScreen();
    }
    else
    {
        setParent(m_parent);
        m_parent->insertTab(m_lastTabIndex, this, sLastTitle);
        m_parent->setCurrentIndex(m_lastTabIndex);
        m_parent->closeTab(m_parent->count()-1);
        m_ui->consoleView->setFocus();
    }
}

void CConsoleTab::showContextMenu(const QPoint &pt)
{
    QMenu *menu = new QMenu(this);
    menu->addAction(m_ui->actionConfiguration);
    menu->addAction(m_ui->actionChangeColor);
    menu->addAction(m_ui->actionLogging);
    menu->addSeparator();
    menu->addAction(m_ui->actionChangeFont);
    menu->addSeparator();
    menu->addAction(m_ui->actionHighlight);
    menu->addSeparator();
    menu->addAction(m_ui->actionClear);
    menu->addSeparator();
    menu->addAction(m_ui->actionFullscreen);
    menu->addSeparator();
    menu->addAction(m_ui->actionAbout);
    menu->exec(mapToGlobal(pt));
    delete menu;
}


void CConsoleTab::updateHighlighting()
{
    QList<CHighlightsFrame::Highlighting> h = m_ui->highlightsFrame->getItems();
    m_ui->consoleView->setHighlighting(h);
}


void CConsoleTab::showConnectBar(void)
{
    m_ui->btnBar->show();
}

void CConsoleTab::showColorDialog(void)
{
    QSettings settings;
    QColor initial(settings.value("background").toString());
    QColor rgb = QColorDialog::getColor(initial, this);
    if (rgb.isValid())
    {
        setBackgroundColor(rgb);
        settings.setValue("background", rgb.name());
    }
}

void CConsoleTab::showFontDialog(void)
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, m_ui->consoleView->font(), this);
    if (ok)
    {
        QSettings settings;
        settings.setValue("font", font.toString());
        setConsoleFont(font);
    }
}

void CConsoleTab::setConsoleFont(const QFont &font)
{
    m_ui->consoleView->setFont(font);
    m_ui->consoleView->refreshCursor();
}

void CConsoleTab::setBackgroundColor(const QColor &color)
{
    setStyleSheet(QString("QPlainTextEdit { background-color: %1; }").arg(color.name()));
}

void CConsoleTab::onConnectClicked(void)
{
    const QString sDeviceName = m_ui->comboPorts->currentData().toString();

    if (!m_port)
    {
        m_port = new QSerialPort(sDeviceName);

        m_port->setSettingsRestoredOnClose(false);

        dumpDCB(sDeviceName.toStdString().c_str());

        if (m_port->open(QIODevice::ReadWrite))
        {
            // configure serial port
            const QString sPortName = m_ui->comboPorts->currentText();


            m_port->setBaudRate(m_ui->comboBaudRates->currentData().toInt());
            m_port->setDataBits(m_ui->comboDataBits->currentData().value<QSerialPort::DataBits>());
            m_port->setParity(m_ui->comboParity->currentData().value<QSerialPort::Parity>());
            m_port->setStopBits(m_ui->comboStopBits->currentData().value<QSerialPort::StopBits>());
            m_port->setFlowControl(m_ui->comboFlowControl->currentData().value<QSerialPort::FlowControl>());

            m_port->clear(QSerialPort::AllDirections);

            connect(m_port, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));

            m_ui->btnBar->hide();
            m_ui->consoleView->setEnabled(true);
            m_ui->consoleView->setFocus();
            m_parent->setCurrentTabText(sDeviceName);

            m_ui->comboPorts->setEnabled(false);
            m_ui->btnConnect->setText(tr("&Disconnect"));

            m_ui->statusBar->showMessage(tr("Successfully connected to %1.").arg(sDeviceName), 3000);

        }
        else
        {
            qDebug() << m_port->error();
            delete m_port;
            m_port = NULL;
            m_ui->statusBar->showMessage(tr("Error. Failed to open port."));
        }

        connect(m_port, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(showError(QSerialPort::SerialPortError)));
    }
    else
    {
        m_port->disconnect();
        delete m_port;
        m_port = NULL;
        m_ui->comboPorts->setEnabled(true);
        m_ui->btnConnect->setText("&Connect");
        m_ui->consoleView->setEnabled(false);
    }
}

void CConsoleTab::showError(QSerialPort::SerialPortError error)
{
    qDebug() << "ERROR: " << error;
    m_port->close();
    m_ui->statusBar->showMessage("ERROR");
}

void CConsoleTab::onComboChanged(void)
{
    if (m_ui->comboPorts->currentIndex() != 0 && m_ui->comboBaudRates->currentIndex() != 0)
    {
        m_ui->btnConnect->setEnabled(true);
    }
    else
    {
        m_ui->btnConnect->setEnabled(false);
    }
}


void CConsoleTab::showAboutDialog(void)
{
    const QString contents = QString(
        "<p><font color=#000080><font size=6><b>%1</b></font> <font size=4>(revision %2)</font></font></p>"
        "<p align=left>Copyright &copy; 2014 Stefan Scheler. %3</p>"
        "<p><a href=\"%4\">%5</a></p>"
        "<p>The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.</p>")
        .arg(g_sAppFullName, QString::number(g_u32revision), tr("All rights reserved."), g_sAppHomepage, tr("Visit superterm website"));

    QMessageBox::about(this, tr("About superterm"), contents);
}

void CConsoleTab::onDataAvailable(void)
{
    QByteArray data = m_port->readAll();
    if (m_logFile)
    {
        m_logFile->write(data);
    }
#if 0
    for (int p = 0; p < data.size(); p++)
    {
        printf("0x%02x ", data.at(p));
    }
    printf("\n");
#endif
    QString str = data;
    //std::cout << "NEW DATA [" << str.toStdString() << "]" << std::endl;
    str = str.replace("\r\n", "\n");
    str = str.replace("\r", "\n");
    //str = str.replace("\t", "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
#if 1
    //highlight.cursor = m_ui->consoleView->textCursor();

#endif

    m_ui->consoleView->insertPlainText(str);
}

void CConsoleTab::onKeyPressed(QString text)
{
    if (text.isEmpty())
        return;

    QByteArray data;
    data.append(text.toLatin1());
    printf("KEY [%s] (0x%02x, 0x%02x)\n", text.toLatin1().constData(), text.toLatin1().constData()[0], data.constData()[0]);
    m_port->write(data.constData());
}

void CConsoleTab::startLogging(void)
{
    qDebug() << "LOGGING STARTED";
    const QString sFileName = m_ui->logPanel->getLogFileName();
    m_logFile = new QFile(sFileName);
    if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Append))
    {
        m_ui->statusBar->showMessage(tr("Logging to %1 failed.").arg(sFileName));
    }

    m_ui->statusBar->showMessage(tr("Logging to %1 started.").arg(sFileName), 3000);

}

void CConsoleTab::stopLogging(void)
{
    m_logFile->close();
    delete m_logFile;
    m_logFile = NULL;
}

// EOF <stefan@scheler.com>
