#include "winapi.h"
#include "backend.h"
#include <QDebug>

#include <psapi.h>

// Global variables
DWORD pid;
HWINEVENTHOOK g_hook;

// Callback function that handles events.
//
void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    Backend* backend = Backend::get();
    auto it_match = backend->gameProfileMap.find(hwnd);
    if (it_match != backend->gameProfileMap.end())
    {
        ProfileConfig* profile = (ProfileConfig*)backend->config.getProfiles()[it_match->second];
        profile->setIsRunning(false);
        backend->gameProfileMap.erase(it_match);
        backend->profileGameMap.erase(it_match->second);
    }
}

// Initializes COM and sets up the event hook.
//
void initializeHook()
{
    pid = GetCurrentProcessId();
    CoInitialize(NULL);
    g_hook = SetWinEventHook(
        EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, // Range of events (4 to 5).
        NULL, // Handle to DLL.
        HandleWinEvent, // The callback.
        0, 0, // Process and thread IDs of interest (0 = all)
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS); // Flags.
}

// Unhooks the event and shuts down COM.
//
void shutdownHook()
{
    UnhookWinEvent(g_hook);
    CoUninitialize();
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
    std::set<HWND> windows = *((std::set<HWND>*)lParam);
    Backend* backend = Backend::get();

    DWORD lpdwProcessId;
    GetWindowThreadProcessId(hwnd, &lpdwProcessId);
    if (lpdwProcessId != pid)
    {
        GetWindowTextA(hwnd, (LPSTR)title, sizeof(title));
        if (QString(title) == backend->config.getTargetWindowName())
        {
            windows.insert(hwnd);
        }
    }
    return TRUE;
}

std::set<HWND> listGameWindows()
{
    std::set<HWND> result;
    EnumWindows(enumWindowsCallback, (LPARAM)&result);
    return result;
}

void terminateGameProcess(uint64_t processId)
{
    HANDLE hProc;
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
