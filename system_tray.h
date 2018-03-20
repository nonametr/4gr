#ifndef SYSTEM_TRAY_H
#define SYSTEM_TRAY_H

#include <QSystemTrayIcon>

class SystemTray : public QObject
{
    Q_OBJECT

public:
    SystemTray();
    virtual ~SystemTray();

signals:
    void showHide();
    void startGame();
    void update();

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    QSystemTrayIcon* trayIcon;
};

#endif // SYSTEM_TRAY_H
