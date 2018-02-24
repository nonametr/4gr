#ifndef BACKEND_H
#define BACKEND_H

#include "config.h"
#include "singleton.h"
#include <qwindowdefs_win.h>
#include <set>
#include <stdint.h>

class Backend : public QObject, public Singleton<Backend>
{
    Q_OBJECT
public:
    Backend(QObject* parent = 0);
    virtual ~Backend();

    static Q_INVOKABLE QUrl fromUserInput(const QString& userInput);
    static Q_INVOKABLE void restart();
    Q_INVOKABLE void beginStartGame(int id);
    Q_INVOKABLE void interceptGame(int id);
    Q_INVOKABLE void killGame(int id);

    void waitForGame(int id);

    uint64_t pid;
    Config config;

    std::set<HWND> knownWindows;
    std::map<int, HWND> profileGameMap; //<profile_id, GameWindow>
    std::map<HWND, int> gameProfileMap; //<GameWindow, profile_id>

signals:
    void readyToLaunch(int id);
    void gameIntercepted(int id);
    void endLaunch();
};

#endif // BACKEND_H
