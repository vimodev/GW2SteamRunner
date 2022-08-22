// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <future>

// GW2 Addon loader stuff
gw2al_addon_dsc  gAddonDsc = { L"gw2_steam_runner", L"Tells Steam Guild Wars 2 is running.", 0, 1, 1, NULL };
gw2al_addon_dsc* gw2addon_get_description() { return &gAddonDsc; }
gw2al_api_ret gw2addon_load(gw2al_core_vtable* core_api) { return GW2AL_OK; }
gw2al_api_ret gw2addon_unload(int gameExiting) { return GW2AL_OK; }

// Constants
// File containing app id to use (1284210 for Guild Wars 2 (default))
const std::wstring appid_path = L"\\addons\\gw2_steam_runner\\appid.cfg";
// Library with Steam API
const std::wstring steam_api_path = L"\\addons\\gw2_steam_runner\\steam_api64.dll";
// Steam registry lookup
LPCWSTR steam_reg_key = L"SOFTWARE\\WOW6432Node\\Valve\\Steam";
LPCWSTR steam_reg_install_path = L"InstallPath";
// Message shown at error prompt
LPCWSTR error_prompt_title = L"ERROR: GW2 Steam Runner";
LPCWSTR error_prompt_message = L"Failed to initialize Steam API. Most likely Guild Wars 2 is not in your account library.";

// Functions to be loaded from Steam API
typedef bool (*Init_Proc)();
Init_Proc steam_api_init;
typedef bool (*IsSteamRunning_Proc)();
IsSteamRunning_Proc is_steam_running;

/**
* Get the current working directory
*/
std::wstring get_working_directory() {
    TCHAR buffer[MAX_PATH];
    DWORD len = GetCurrentDirectory(MAX_PATH, buffer);
    std::wstring current_dir = buffer;
    return current_dir;
}

/**
* Set the SteamAppId env variable based on contents of appid file
*/
void set_app_id() {
    std::ifstream ifs(get_working_directory() + appid_path);
    std::string appid((std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>()));
    std::wstring appidw(appid.begin(), appid.end());
    SetEnvironmentVariable(L"SteamAppId", appidw.c_str());
}

/**
* Load the Steam API library and the functions we require
*/
void load_steam_api() {
    std::wstring dll_path = get_working_directory() + steam_api_path;
    HMODULE steam_api = LoadLibrary(dll_path.c_str());
    steam_api_init = (Init_Proc)GetProcAddress(steam_api, "SteamAPI_Init");
    is_steam_running = (IsSteamRunning_Proc)GetProcAddress(steam_api, "SteamAPI_IsSteamRunning");
}

/**
* Try to initialize the Steam API to simulate the app running, prompt if fail
*/
void initialize_steam_api() {
    bool success = steam_api_init();
    if (!success) { MessageBox(NULL, error_prompt_message, error_prompt_title, MB_OK | MB_ICONERROR); }
}

/**
* Start an instance of Steam on the local computer
*/
void start_steam_process() {
    // Get the installation path
    DWORD buffer_size = MAX_PATH;
    TCHAR buffer[MAX_PATH];
    RegGetValue(HKEY_LOCAL_MACHINE, steam_reg_key, steam_reg_install_path, RRF_RT_REG_SZ, NULL, buffer, &buffer_size);
    std::wstring steam_dir = buffer;
    std::wstring steam_exe = L"\\steam.exe";
    std::wstring silent_arg = L" -silent";
    std::wstring steam_location = buffer + steam_exe;
    std::wstring steam_command = buffer + steam_exe + silent_arg;
    // And start steam
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    CreateProcess((LPWSTR)steam_location.c_str(), (LPWSTR)steam_command.c_str(), NULL, NULL, false, 0, NULL, NULL, &si, &pi);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

/**
* Try to detect steam running, return if found within timeout window
*/
bool wait_for_steam_startup(int timeout, int interval) {
    int time_waited = 0;
    while (!is_steam_running() && time_waited < timeout) {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        time_waited += interval;
    }
    if (time_waited >= timeout) { return false; }
    return true;
}

/**
* Run the addon
*/
void run() {
    set_app_id();
    load_steam_api();
    if (is_steam_running()) {
        initialize_steam_api();
    } else { 
        start_steam_process();
        if (!wait_for_steam_startup(10 * 1000, 333)) { return; }
        initialize_steam_api();
    }

}

bool WINAPI DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            run();
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

