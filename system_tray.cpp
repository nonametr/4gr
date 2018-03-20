#include "system_tray.h"
#include <QAction>
#include <QApplication>
#include <QBitmap>
#include <QDebug>
#include <QMenu>

SystemTray::SystemTray()
{
    trayIcon = new QSystemTrayIcon(nullptr);

    trayIcon->setIcon(QIcon(":/img/tray-icon.ico"));
    trayIcon->setToolTip("4GR");

    QMenu* menu = new QMenu(nullptr);
    QAction* startGameAction = new QAction(tr("Start game"), this);
    QAction* showAction = new QAction(tr("Show\\Hide"), this);
    QAction* updateAction = new QAction(tr("Update"), this);
    QAction* quitAction = new QAction(tr("Quit"), this);

    connect(startGameAction, &QAction::triggered, this, &SystemTray::startGame);
    connect(showAction, &QAction::triggered, this, &SystemTray::showHide);
    connect(updateAction, &QAction::triggered, this, &SystemTray::update);
    connect(quitAction, &QAction::triggered, []() { QApplication::quit(); });

    menu->addAction(startGameAction);
    menu->addAction(showAction);
    menu->addAction(updateAction);
    menu->addAction(quitAction);

    trayIcon->setContextMenu(menu);
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

SystemTray::~SystemTray()
{
    trayIcon->deleteLater();
}

void SystemTray::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        showHide();
        break;
    default:
        break;
    }
}
