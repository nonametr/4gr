#include "backend.h"

#include <QProcess>
#include <future>
#include <iostream>
#include <stdio.h>
#include <string>
#include <tchar.h>

#include <windows.h>

#include <psapi.h>

Backend::Backend(QObject* parent) : QObject(parent)
{
    pid = GetCurrentProcessId();
    config.load(CONFIG_PATH);
}

QUrl Backend::fromUserInput(const QString& userInput)
{
    QFileInfo fileInfo(userInput);
    if (fileInfo.exists())
        return QUrl::fromLocalFile(fileInfo.absoluteFilePath());
    return QUrl::fromUserInput(userInput);
}

BOOL CALLBACK enumWindowsCallback(HWND hwnd, LPARAM lParam)
{
    char title[50];
    Backend* backend = (Backend*)lParam;

    DWORD lpdwProcessId;
    GetWindowThreadProcessId(hwnd, &lpdwProcessId);
    if (lpdwProcessId != backend->pid)
    {
        GetWindowTextA(hwnd, (LPSTR)title, sizeof(title));
        if (QString(title) == backend->config.getTargetWindowName())
        {
            auto it_match = backend->knownWindows.find(hwnd);
            if (it_match == backend->knownWindows.end())
            {
                backend->newWindows.insert(hwnd);
            }
        }
    }
    return TRUE;
}

void Backend::beginStartGame(int id)
{
    EnumWindows(enumWindowsCallback, (LPARAM)this);
    knownWindows.insert(newWindows.begin(), newWindows.end());
    emit readyToLaunch(id);
}

void Backend::waitForGame(int id)
{
    int max_attempts = 30;
    do
    {
        qDebug() << "waiting for game window...";
        std::this_thread::sleep_for(2s);
        EnumWindows(enumWindowsCallback, (LPARAM)this);
    } while (newWindows.empty() && --max_attempts > 0);

    if (max_attempts <= 0)
    {
        endLaunch();
        return;
    }
    ProfileConfig* profile = (ProfileConfig*)config.getProfiles()[id];
    HWND hwnd = *newWindows.begin();
    QVector4D win_pos = profile->getWindowPos();

    std::async(launch::async, [&win_pos, hwnd]() {
        std::this_thread::sleep_for(3s);
        SetWindowPos(hwnd, HWND_TOP, win_pos.x(), win_pos.y(), win_pos.w(), win_pos.z(), SWP_SHOWWINDOW);
    });

    knownWindows.insert(newWindows.begin(), newWindows.end());
    newWindows.clear();
    emit gameIntercepted(id);
}

void Backend::interceptGame(int id)
{
    std::thread(&Backend::waitForGame, this, id).detach();
}

void Backend::restart()
{
    QProcess::startDetached(qApp->applicationDirPath() + "/4grunner.exe", qApp->arguments());
    QCoreApplication::quit();
}
