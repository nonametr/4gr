#include "backend.h"
#include "profile_config.h"
#include "winapi.h"
#include <QFileInfo>
#include <QGuiApplication>
#include <QList>
#include <QMouseEvent>
#include <QObject>
#include <QProcess>
#include <QQuickItem>
#include <QTimer>
#include <QtDebug>
#include <assert.h>
#include <future>
#include <iostream>
#include <stdio.h>
#include <string>
#include <tchar.h>

Backend Backend::_instance;

Backend::Backend(QObject* parent) : QObject(parent)
{
    initializeHook();
}

Backend::~Backend()
{
    shutdownHook();
}

QUrl Backend::fromUserInput(const QString& userInput)
{
    QFileInfo fileInfo(userInput);
    if (fileInfo.exists())
        return QUrl::fromLocalFile(fileInfo.absoluteFilePath());
    return QUrl::fromUserInput(userInput);
}

void Backend::beginStartGame(int id)
{
    ProfileConfig* profile = (ProfileConfig*)config.getProfiles()[id];
    if (profile->isEnabled() && !profile->isRunning())
    {
        std::set<HWND> gameWindows = listGameWindows();
        knownWindows.insert(gameWindows.begin(), gameWindows.end());
        emit readyToLaunch(id);
    }
    else
    {
        gameIntercepted(id);
    }
}

void Backend::waitForGame(int id)
{
    std::set<HWND> gameWindows;
    int max_attempts = 30;
    do
    {
        qDebug() << "waiting for game window...";
        std::this_thread::sleep_for(2s);
        gameWindows = listGameWindows();
    } while (gameWindows.empty() && --max_attempts > 0);

    if (max_attempts <= 0)
    {
        endLaunch();
    }
    else
    {
        ProfileConfig* profile = (ProfileConfig*)config.getProfiles()[id];
        HWND hwnd = *gameWindows.begin();
        QVector4D win_pos = profile->getWindowPos();

        std::async(launch::async, [&win_pos, hwnd]() {
            std::this_thread::sleep_for(3s);
            SetWindowPos(hwnd, HWND_TOP, win_pos.x(), win_pos.y(), win_pos.w(), win_pos.z(), SWP_SHOWWINDOW);
        });

        addGameWindow(id, hwnd);

        knownWindows.insert(gameWindows.begin(), gameWindows.end());
        std::async(launch::async, [id, this]() {
            if (config.getDelay() > 0)
            {
                chrono::milliseconds ms_delay((int)(config.getDelay() * 1000.0f));
                std::this_thread::sleep_for(ms_delay);
            }
            emit gameIntercepted(id);
        });
    }
}

void Backend::addGameWindow(int profile_id, HWND hwnd)
{
    ProfileConfig* profile = (ProfileConfig*)config.getProfiles()[profile_id];

    profile->setIsRunning(true);
    profileGameMap.emplace(profile_id, hwnd);
    gameProfileMap.emplace(hwnd, profile_id);
}

void Backend::removeGameWindow(int profile_id, HWND hwnd)
{
    ProfileConfig* profile = (ProfileConfig*)config.getProfiles()[profile_id];

    profile->setIsRunning(false);
    profileGameMap.erase(profile_id);
    gameProfileMap.erase(hwnd);

    if (config.getKeepAlive())
    {
        beginStartGame(profile_id);
    }
}

void Backend::interceptGame(int id)
{
    std::thread(&Backend::waitForGame, this, id).detach();
}

void Backend::killGame(int id)
{
    auto it_match = profileGameMap.find(id);
    if (it_match != profileGameMap.end())
    {
        DWORD processId;
        GetWindowThreadProcessId(it_match->second, &processId);
        terminateGameProcess(processId);
    }
}

void Backend::restart()
{
    QProcess::startDetached(qApp->applicationDirPath() + "/4gr.exe", qApp->arguments());
    QCoreApplication::quit();
}
