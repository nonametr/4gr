#ifndef BACKEND_H
#define BACKEND_H

#include "config.h"
#include "singleton.h"
#include <qwindowdefs_win.h>
#include <set>
#include <stdint.h>

class Backend : public QObject
{
    Q_OBJECT
public:
    Backend(QObject* parent = 0);
    virtual ~Backend();

    static Backend* get() { return _instance; }

    static Q_INVOKABLE QUrl fromUserInput(const QString& userInput);
    static Q_INVOKABLE void restart();
    Q_INVOKABLE void beginStartGame(int id);
    Q_INVOKABLE void interceptGame(int id);
    Q_INVOKABLE void killGame(int id);

    void waitForGame(int id);

    Config config;

    void addGameWindow(int profile_id, HWND hwnd);
    void removeGameWindow(int profile_id, HWND hwnd);

    std::set<HWND> knownWindows;
    std::map<int, HWND> profileGameMap; //<profile_id, GameWindow>
    std::map<HWND, int> gameProfileMap; //<GameWindow, profile_id>

signals:
    void readyToLaunch(int id);
    void gameIntercepted(int id);
    void endLaunch();

private:
    static Backend* _instance;
};

#endif // BACKEND_H
