// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <stdlib.h>
#include <processthreadsapi.h>
#include <cstdlib>
#include <string>

/**
* Start the steam-idle.exe runner for Guild Wars 2
**/
void startRunner() {
    // Locate executable and form command
    TCHAR buffer[MAX_PATH];
    DWORD len = GetCurrentDirectory(MAX_PATH, buffer);
    std::wstring quote = L"\"";
    std::wstring arg = L" 4000";
    std::wstring relative = L"\\addons\\gw2_steam_runner\\steam-idle\\steam-idle.exe";
    std::wstring working = buffer;

    std::wstring exec = buffer + relative;
    std::wstring cmd = quote + exec + quote + arg;

    // Start the process
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    CreateProcess(exec.c_str(), (LPWSTR) cmd.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    // Clean up handles
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}

bool WINAPI DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            startRunner();
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

