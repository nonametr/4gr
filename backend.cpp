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

BOOL CALLBACK enumTerminateWindowsCallback(HWND hwnd, LPARAM lParam)
{
    DWORD dwID;

    GetWindowThreadProcessId(hwnd, &dwID);

    if (dwID == (DWORD)lParam)
    {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }

    return TRUE;
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
    ProfileConfig* profile = (ProfileConfig*)config.getProfiles()[id];
    if (profile->getEnabled())
    {
        EnumWindows(enumWindowsCallback, (LPARAM)this);
        knownWindows.insert(newWindows.begin(), newWindows.end());
        emit readyToLaunch(id);
    }
    else
    {
        gameIntercepted(id);
    }
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

    activeGames.emplace(id, *newWindows.begin());
    knownWindows.insert(newWindows.begin(), newWindows.end());
    newWindows.clear();
    emit gameIntercepted(id);
}

void Backend::interceptGame(int id)
{
    std::thread(&Backend::waitForGame, this, id).detach();
}

void Backend::killGame(int id)
{
    auto it_match = activeGames.find(id);
    if (it_match != activeGames.end())
    {
        HANDLE hProc;
        DWORD processId;

        GetWindowThreadProcessId(it_match->second, &processId);

        // If we can't open the process with PROCESS_TERMINATE rights,
        // then we give up immediately.
        hProc = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, processId);

        if (hProc == NULL)
        {
            return;
        }

        // TerminateAppEnum() posts WM_CLOSE to all windows whose PID
        // matches game process's.
        EnumWindows(enumTerminateWindowsCallback, (LPARAM)processId);

        // Wait 1s on the handle. If it signals, great. If it times out,
        // then it.
        if (WaitForSingleObject(hProc, 1000) != WAIT_OBJECT_0)
        {
            TerminateProcess(hProc, 0);
        }

        CloseHandle(hProc);
    }
}

Q_INVOKABLE bool Backend::isInGame(int id)
{
    return activeGames.find(id) != activeGames.end();
}

void Backend::restart()
{
    QProcess::startDetached(qApp->applicationDirPath() + "/4grunner.exe", qApp->arguments());
    QCoreApplication::quit();
}
