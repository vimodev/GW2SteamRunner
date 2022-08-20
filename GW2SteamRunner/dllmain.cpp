// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

gw2al_addon_dsc  gAddonDsc = { L"gw2_steam_runner", L"Tells Steam Guild Wars 2 is running.", 0, 1, 1, NULL };
gw2al_addon_dsc* gw2addon_get_description() { return &gAddonDsc; }
gw2al_api_ret gw2addon_load(gw2al_core_vtable* core_api) { return GW2AL_OK;}
gw2al_api_ret gw2addon_unload(int gameExiting) { return GW2AL_OK; }

/**
* Initialize the Steam API
*/
void enable_steam() {

    // File containing app id to use (1284210 for Guild Wars 2 (default))
    std::wstring appid_path = L"\\addons\\gw2_steam_runner\\appid.cfg";
    // Library with Steam API
    std::wstring steam_api_path = L"\\addons\\gw2_steam_runner\\steam_api64.dll";

    // Get working directory
    TCHAR buffer[MAX_PATH];
    DWORD len = GetCurrentDirectory(MAX_PATH, buffer);
    std::wstring current_dir = buffer;

    // Read app id from appid file
    std::ifstream ifs(current_dir + appid_path);
    std::string appid((std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>()));
    std::wstring appidw(appid.begin(), appid.end());

    // Set the SteamAppId environment variable to the app id, Steam API will read this
    SetEnvironmentVariable(L"SteamAppId", appidw.c_str());

    // Load the Steam API's init function
    std::wstring dll_path = current_dir + steam_api_path;
    HMODULE steam_api = LoadLibrary(dll_path.c_str());
    typedef bool (*SteamAPI_Init_Proc)();
    SteamAPI_Init_Proc steam_api_init = (SteamAPI_Init_Proc)GetProcAddress(steam_api, "SteamAPI_Init");

    // Initialize the Steam API
    steam_api_init();

}

bool WINAPI DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            enable_steam();
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

