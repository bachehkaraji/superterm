#include <QMoveEvent>
#include <QDebug>
#include <QRect>
#include <QString>

#if defined(Q_OS_WIN)
#include "windows.h"
#include "math.h"
#endif

#include "ui_mainwindow.h"
#include "ui/mainwindow.h"
#include "ui/consoletab.h"
#include "ui/consoletabbar.h"
#include "ui/consoletabfactory.h"
#include "misc/globals.h"
#include "misc/softwareversion.h"
#include "session/session.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    qDebug() << "CMainWindow::CMainWindow()";

    m_ui->setupUi(this);
    m_ui->notificationBar->hide();
    QWidget::resize(800, 600);

    setWindowTitle(Globals::ApplicationFullName);
}

MainWindow::~MainWindow()
{
    qDebug() << "CMainWindow::~CMainWindow()" << this;
    delete m_ui;
}

QSize MainWindow::sizeHint() const
{
    return QSize(800, 600);
}

void MainWindow::addExistingTabsFromFile()
{
    QStringList fileNames = Session::getSessionList();

    if (!fileNames.isEmpty())
    {
        for (const QString& fileName : fileNames)
        {
            Session* session = Session::createSessionFromFile(fileName);

            m_ui->tabWidget->addTab(ConsoleTabFactory::createTabFromSession(session));
        }
    }
    else
    {
        m_ui->tabWidget->addTab(ConsoleTabFactory::createTab());
    }
}

void MainWindow::attachTab(ConsoleTab* tab)
{
    qDebug() << "attaching tab" << tab->getLabel() << "to" << this;

    m_ui->tabWidget->addTab(tab);
}

ConsoleTab* MainWindow::detachTab()
{
    int curIndex = m_ui->tabWidget->currentIndex();

    ConsoleTab* tab = m_ui->tabWidget->widget(curIndex);
    m_ui->tabWidget->removeTab(curIndex);

    qDebug() << "detaching tab" << tab->getLabel() << "from" << this;

    return tab;
}

void MainWindow::showUpdateInfo(const SoftwareVersion& version)
{
    m_ui->notificationBar->setNotificationText(QString(tr("A software update is available. Click here to download superterm %1.")).arg(version.toString()));
    m_ui->notificationBar->setLink(Globals::ApplicationWebsite);
    m_ui->notificationBar->show();
}

void MainWindow::onSecondaryInstanceLaunched()
{
    qDebug() << "[slot] onSecondaryInstanceLaunched";

    MainWindow* mainWindow = new MainWindow;

    mainWindow->show();
    mainWindow->attachTab(ConsoleTabFactory::createTab());
}

bool MainWindow::nativeEvent(const QByteArray& eventType, void* message, long*)
{
    bool bEventHandled = false;

#if defined(Q_OS_WIN)
    if (eventType == "windows_generic_MSG")
    {
        MSG* msg = static_cast< MSG* >( message );

        switch ( msg->message )
        {
            case WM_SIZING:
                {
                    RECT* r = reinterpret_cast<RECT*>(msg->lParam);

                    long widthWindow = r->right - r->left;
                    long heightWindow = r->bottom - r->top;

                    qDebug() << widthWindow << heightWindow;
                    CConsoleTab* tab = m_ui->tabWidget->widget(0); // FIXME: ugly

                    QSize decorationSize = QWidget::frameSize() - QWidget::size();
                    QSize charSize = tab->getCharWidth();

                    int tabBarWidth = QWidget::width() - tab->getViewPortSize().width();

                    // calculate adjusted view dimensions
                    int adjustedViewWidth = floor((widthWindow - decorationSize.width() - tabBarWidth) / charSize.width()) * charSize.width() + 3;
                    int adjustedViewHeight = floor((heightWindow - decorationSize.height()) / charSize.height()) * charSize.height() + 3;

                    // manipulate event
                    r->right = r->left + decorationSize.width() + tabBarWidth + adjustedViewWidth;
                    r->bottom = r->top + decorationSize.height() + adjustedViewHeight;
                }
                break;
            default:
                break;
        }
    }
#elif defined(Q_OS_LINUX)
    if (eventType == "xcb_generic_event_t")
    {
        Q_UNUSED(message);
    }
#else
    Q_UNUSED(eventType)
    Q_UNUSED(message);
#endif

    return bEventHandled;
}

int MainWindow::getTabCount() const
{
    return m_ui->tabWidget->count();
}

QRect MainWindow::getTabBarRect() const
{
    ConsoleTabBar* tabBar = static_cast<ConsoleTabBar*>(m_ui->tabWidget->tabBar());
    return QRect(tabBar->mapToGlobal(QPoint(0, 0)), tabBar->size());
}

// EOF <stefan@scheler.com>
