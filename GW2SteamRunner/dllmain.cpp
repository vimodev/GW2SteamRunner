// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <stdlib.h>
#include <processthreadsapi.h>
#include <cstdlib>
#include <string>

PROCESS_INFORMATION process_information;

gw2al_core_vtable* gAPI;
gw2al_addon_dsc  gAddonDsc = { L"gw2_steam_runner", L"Tells Steam Guild Wars 2 is running.", 0, 1, 1, NULL };

/**
* Start the steam-idle.exe runner for Guild Wars 2
**/
void startRunner() {
    // Locate executable and form command
    TCHAR buffer[MAX_PATH];
    DWORD len = GetCurrentDirectory(MAX_PATH, buffer);
    std::wstring quote = L"\"";
    std::wstring arg = L" 1284210";
    std::wstring relative = L"\\addons\\gw2_steam_runner\\steam-idle\\steam-idle.exe";
    std::wstring working = buffer;

    std::wstring exec = buffer + relative;
    std::wstring cmd = quote + exec + quote + arg;

    // Start the process
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    ZeroMemory(&process_information, sizeof(process_information));
    CreateProcess(exec.c_str(), (LPWSTR)cmd.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &process_information);
}

void endRunner() {
    TerminateProcess(process_information.hProcess, 0);
    CloseHandle(process_information.hThread);
    CloseHandle(process_information.hProcess);
}

gw2al_addon_dsc* gw2addon_get_description()
{
    return &gAddonDsc;
}

gw2al_api_ret gw2addon_load(gw2al_core_vtable* core_api)
{
    gAPI = core_api;
    gAPI->log_text(LL_INFO, gAddonDsc.name, (wchar_t*)L"Started steam-idle process");
    return GW2AL_OK;
}

gw2al_api_ret gw2addon_unload(int gameExiting)
{
    gAPI->log_text(LL_INFO, gAddonDsc.name, (wchar_t*)L"Stopping steam-idle process");
    return GW2AL_OK;
}

bool WINAPI DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            startRunner();
            break;
        case DLL_PROCESS_DETACH:
            endRunner();
            break;
    }
    return TRUE;
}

