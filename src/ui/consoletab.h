#ifndef CONSOLETAB_H
#define CONSOLETAB_H

#include <QWidget>
#include <QSerialPort>
#include <QTextEdit>
#include <QProcess>

namespace Ui {
class CConsoleTab;
}

class QSerialPort;
class CConsoleTabWidget;
class QFile;
class CPortEnumerator;
class QMenu;
class CPortEndpoint;
class CSession;
class CMainWindow;
class CMessage;

class CConsoleTab : public QWidget
{
    Q_OBJECT

public:
    CConsoleTab(CPortEnumerator* pe, CSession* session = NULL);
    ~CConsoleTab();

    void hideButton(int index);

    void setConsoleFont(const QFont& font);
    void setColor(const QColor& backGroundColor, const QColor& foreGroundColor);
    QString getLabel() const;
    void setLabel(const QString& label);
    QSize getViewPortSize() const;
    QSize getCharSize() const;

public slots:
    void onConnectClicked();
    void onKeyPressed(QKeyEvent* e);
    void showContextMenu(const QPoint& pt);
    void showColorDialog();
    void showFontDialog();
    void showFontColorDialog();
    void showError(QSerialPort::SerialPortError error);
    void updateHighlighting();
    void showAboutDialog();
    void toggleFullScreen();
    void toggleAutoScroll();
    void startLogging();
    void stopLogging();
    void showSaveDialog();
    void onEndpointData(const CMessage& message);
    void onEndpointConnected();
    void onEndpointDisconnected(int returnCode);
    void onReconnectionSignal(const CMessage& message);
    void onReconnectionCancel();
    void onConfigurationChanged(const QString& config);
    void showRenameTabDialog();
    void onRenameTab();
    void clearTab();

signals:
    void labelChanged(const QString&);

private:
    Ui::CConsoleTab* m_ui;
    CMainWindow*     mMainWindow;
    QString          mTabLabel;
    CPortEndpoint*   m_portEndpoint;
    CSession*        m_session;
    QFile*           m_logFile;
    QMenu*           m_contextMenu;
    int              m_lastTabIndex;

    static quint32 m_u32counter;
    QStringList    m_highlightings;
    bool             m_bSkipTimeStamp;

    void fillComboBoxes();
    void createContextMenu();
    void insertTimeStamps(QByteArray& data);
};

#endif // CONSOLETAB_H
