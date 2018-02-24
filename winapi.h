#ifndef WINAPI_H
#define WINAPI_H

#include <set>
#include <stdint.h>
#include <windows.h>

void initializeHook();
void shutdownHook();
std::set<HWND> listGameWindows();
void terminateGameProcess(uint64_t processId);

#endif // WINAPI_H
