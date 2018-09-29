#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ConsoleTab;
class SoftwareVersion;
class QShortcut;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    QSize       sizeHint() const;
    void        attachTab(ConsoleTab* tab);
    void        addExistingTabsFromFile();
    ConsoleTab* detachTab();
    int         getTabCount() const;
    QRect       getTabBarRect() const;

private:
    bool nativeEvent(const QByteArray& eventType, void* message, long* result);

public slots:
    void onSecondaryInstanceLaunched();
    void showUpdateInfo(const SoftwareVersion& version);

signals:
    void willQuit();

private:
    Ui::MainWindow* ui_;
    QShortcut*      shortcutNextTab_;
    QShortcut*      shortcutPrevTab_;
};

#endif // MAINWINDOW_H
