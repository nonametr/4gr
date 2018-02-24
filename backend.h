#ifndef BACKEND_H
#define BACKEND_H

#include "config.h"
#include <QFileInfo>
#include <QGuiApplication>
#include <QList>
#include <QMouseEvent>
#include <QObject>
#include <QQuickItem>
#include <QTimer>
#include <QtDebug>
#include <memory>
#include <set>
#include <stdint.h>

class Backend : public QObject
{
    Q_OBJECT
public:
    Backend(QObject* parent = 0);
    virtual ~Backend() = default;

    static Q_INVOKABLE QUrl fromUserInput(const QString& userInput);
    Q_INVOKABLE void beginStartGame(int id);
    Q_INVOKABLE void interceptGame(int id);
    Q_INVOKABLE void killGame(int id);
    Q_INVOKABLE bool isInGame(int id);
    static Q_INVOKABLE void restart();

    void waitForGame(int id);

    uint64_t pid;
    Config config;
    std::set<HWND> newWindows;
    std::set<HWND> knownWindows;
    std::map<int, HWND> activeGames; //<profile_id, GameWindow>

signals:
    void readyToLaunch(int id);
    void gameIntercepted(int id);
    void endLaunch();
};

#endif // BACKEND_H
