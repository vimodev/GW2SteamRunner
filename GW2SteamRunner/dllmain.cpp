// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <stdlib.h>
#include <processthreadsapi.h>
#include <cstdlib>
#include <string>

gw2al_core_vtable* gAPI;
gw2al_addon_dsc  gAddonDsc = { L"gw2_steam_runner", L"Tells Steam Guild Wars 2 is running.", 0, 1, 1, NULL };
gw2al_addon_dsc* gw2addon_get_description() { return &gAddonDsc; }
gw2al_api_ret gw2addon_load(gw2al_core_vtable* core_api) {
    gAPI = core_api;
    gAPI->log_text(LL_INFO, gAddonDsc.name, (wchar_t*)L"Started steam-idle process");
    return GW2AL_OK;
}
gw2al_api_ret gw2addon_unload(int gameExiting) {
    gAPI->log_text(LL_INFO, gAddonDsc.name, (wchar_t*)L"Stopping steam-idle process");
    return GW2AL_OK;
}

PROCESS_INFORMATION process_information;

void startRunner() {

    // Get current working directory
    TCHAR buffer[MAX_PATH];
    DWORD len = GetCurrentDirectory(MAX_PATH, buffer);
    // Locate steam-idle executable in addon directory
    std::wstring quote = L"\"";
    std::wstring arg = L" 1284210";
    std::wstring relative = L"\\addons\\gw2_steam_runner\\steam-idle\\steam-idle.exe";
    std::wstring working = buffer;
    std::wstring exec = buffer + relative;
    std::wstring cmd = quote + exec + quote + arg;

    // Start the steam-idle process
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW; // Process should look at the SHOWWINDOW flags passed below
    ZeroMemory(&process_information, sizeof(process_information));
    CreateProcess(exec.c_str(), (LPWSTR)cmd.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &process_information);

}

void stopRunner() {

    // Kill the process
    TerminateProcess(process_information.hProcess, 0);
    // Close handles
    CloseHandle(process_information.hThread);
    CloseHandle(process_information.hProcess);

}

bool WINAPI DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved) {

    switch (fdwReason) {
        // Start steam-idle on library load
        case DLL_PROCESS_ATTACH:
            startRunner();
            break;
        // Stop steam-idle on library free
        case DLL_PROCESS_DETACH:
            stopRunner();
            break;
    }
    return TRUE;

}

