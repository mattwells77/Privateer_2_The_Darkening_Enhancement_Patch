/*
The MIT License (MIT)
Copyright © 2025-2026 Matt Wells

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the “Software”), to deal in the
Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the
following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "pch.h"
#include "configTools.h"
#include "version.h"
#include "input.h"

#include <iomanip>
using namespace std;

wstring movies_path_override;
wstring movies_ext_override;


//________________________
static BOOL Build_Config() {
    wofstream config_stream;

    wstring wConfigPath;
    wConfigPath.assign(GetAppPath());

    wConfigPath.append(L"\\");
    wConfigPath.append(VER_PRODUCTNAME_STR);
    wConfigPath.append(L".ini");

    config_stream.open(wConfigPath.c_str());
    if (!config_stream.is_open())
        return FALSE;

    config_stream << "; " << VER_FILE_DESCRIPTION_STR << endl;
    config_stream << "; Check " << VER_PRODUCTNAME_STR << "_readme.txt for further information regarding the below settings." << endl << endl;

    config_stream << endl << "[MAIN]" << endl;
    config_stream << "VERSION=" << VER_FILE_VERSION_STR << endl;
    config_stream << "UAC_AWARE=" << CONFIG_MAIN_UAC_AWARE << endl;
    config_stream << "ENABLE_DPI_AWARENESS=" << CONFIG_MAIN_ENABLE_DPI_AWARENESS << endl;

    config_stream << "ENABLE_CONTROLLER_ENHANCEMENTS=" << CONFIG_MAIN_ENABLE_CONTROLLER_ENHANCEMENTS << endl;

    config_stream << "DEAD_ZONE=" << CONFIG_MAIN_DEAD_ZONE << endl;
    config_stream << "WINDOWED=" << CONFIG_MAIN_WINDOWED << endl;

    config_stream << "ENABLE_LINEAR_UPSCALING_GUI=" << CONFIG_MAIN_ENABLE_LINEAR_UPSCALING_GUI << endl;
    config_stream << "ENABLE_LINEAR_UPSCALING_HUD=" << CONFIG_MAIN_ENABLE_LINEAR_UPSCALING_HUD << endl;


    config_stream << "SUBS_ENABLED=" << CONFIG_MAIN_SUBS_ENABLED << endl;
    config_stream << "SUBS_LANG=" << CONFIG_MAIN_SUBS_LANG << endl;
    config_stream << "AUDIO_LANG=" << CONFIG_MAIN_AUDIO_LANG << endl;

    config_stream << "NO_CD=" << CONFIG_MAIN_NO_CD << endl;
    config_stream << "NO_CD_UNRESTRICTED_MISSIONS=" << CONFIG_MAIN_NO_CD_UNRESTRICTED_MISSIONS << endl;

    config_stream << "CYCLE_HZ_GUI=" << CONFIG_MAIN_CYCLE_HZ_GUI << endl;


    config_stream << endl << "[SPACE]" << endl;
    config_stream << "CYCLE_HZ_SPACE=" << CONFIG_SPACE_CYCLE_HZ_SPACE << endl;
    config_stream << "ALT_FLIGHT_MODE" << CONFIG_SPACE_ALT_FLIGHT_MODE << endl;
    config_stream << "ROTATION_SPEED_KEY_REVERSE_ACTION" << CONFIG_SPACE_ROTATION_SPEED_KEY_REVERSE_ACTION << endl;
    config_stream << "MAX_SPEED_AFTER_AFTERBURNER_JUMPING=" << CONFIG_SPACE_MAX_SPEED_AFTER_AFTERBURNER_JUMPING << endl;


    config_stream << endl << "[MOVIES]" << endl;
    config_stream << "SHOW_ORIGINAL_MOVIES_INTERLACED=" << CONFIG_MOVIES_SHOW_ORIGINAL_INTERLACED << endl;

    config_stream << "PATH=" << CONFIG_MOVIES_PATH << endl;
    config_stream << "EXT=" << CONFIG_MOVIES_EXT << endl;
 

    config_stream << endl << "[MOUSE]" << endl;
    config_stream << "DEAD_ZONE=" << CONFIG_MOUSE_DEAD_ZONE << endl;
    config_stream << "AXIS_LIMIT_PERCENTAGE=" << CONFIG_MOUSE_AXIS_LIMIT_PERCENTAGE << endl;
    config_stream << "INVERT_Y_AXIS=" << CONFIG_MOUSE_INVERT_Y_AXIS << endl;

    for (int i = 0; i < NUM_JOY_PROFILES; i++) {
        switch (i) {
        case static_cast<int>(PROFILE_TYPE::GUI):
            config_stream << endl << "[MOUSE_GUI]" << endl;
            config_stream << "BUTTON_01=" << static_cast<int>(P2_ACTIONS::Left_Click) << endl;
            config_stream << "BUTTON_02=" << static_cast<int>(P2_ACTIONS::Right_Click) << endl;
            break;
        case static_cast<int>(PROFILE_TYPE::Space):
            config_stream << endl << "[MOUSE_SPACE]" << endl;
            config_stream << "BUTTON_01=" << static_cast<int>(P2_ACTIONS::Left_Click) << endl;
            config_stream << "BUTTON_02=" << static_cast<int>(P2_ACTIONS::Right_Click) << endl;
            break;
        case static_cast<int>(PROFILE_TYPE::ReMap_1):
        case static_cast<int>(PROFILE_TYPE::ReMap_2):
        case static_cast<int>(PROFILE_TYPE::ReMap_3):
            config_stream << endl << "[MOUSE_REMAP_" << setfill(L'0') << setw(2) << i - static_cast<int>(PROFILE_TYPE::ReMap_1) + 1 << "]" << endl;
            config_stream << "BUTTON_01=" << static_cast<int>(P2_ACTIONS::None) << endl;
            config_stream << "BUTTON_02=" << static_cast<int>(P2_ACTIONS::None) << endl;
            break;
        default:
            continue;
        }
        config_stream << "BUTTON_03=" << static_cast<int>(P2_ACTIONS::None) << endl;
        config_stream << "BUTTON_04=" << static_cast<int>(P2_ACTIONS::None) << endl;
        config_stream << "BUTTON_05=" << static_cast<int>(P2_ACTIONS::None) << endl;
        config_stream << "MOUSE_WHEEL_UP=" << static_cast<int>(P2_ACTIONS::None) << endl;
        config_stream << "MOUSE_WHEEL_DOWN=" << static_cast<int>(P2_ACTIONS::None) << endl;
        config_stream << "MOUSE_WHEEL_LEFT=" << static_cast<int>(P2_ACTIONS::None) << endl;
        config_stream << "MOUSE_WHEEL_RIGHT=" << static_cast<int>(P2_ACTIONS::None) << endl;
    }

    config_stream << endl << "[DEBUG]" << endl;
    //config_stream << "ERRORS=" << 1 << endl;
    config_stream << "GENERAL=" << 0 << endl;
    config_stream << "FLIGHT=" << 0 << endl;
    config_stream << "MOVIE=" << 0 << endl;
    config_stream << "CONTROLLER=" << 0 << endl;

    config_stream << endl;
    config_stream.close();

    return TRUE;
}


//_____________________________
static BOOL Is_Config_Updated() {

    wchar_t wchar_buff[_countof(VER_FILE_VERSION_STR)];
    ConfigReadString(L"MAIN", L"VERSION", L"", wchar_buff, MAX_PATH);
    if (wcsncmp(wchar_buff, VER_FILE_VERSION_STR, _countof(VER_FILE_VERSION_STR)) == 0)
        return TRUE;
    return FALSE;
}


//_________________________
static BOOL Update_Config() {

    if (Is_Config_Updated())
        return TRUE;

    wchar_t* wchar_buff = new wchar_t[MAX_PATH] {0};
    UINT val = 0;

    ConfigWriteString(L"MAIN", L"VERSION", VER_FILE_VERSION_STR);

    val = ConfigReadInt(L"MAIN", L"UAC_AWARE", CONFIG_MAIN_UAC_AWARE);
    ConfigWriteInt(L"MAIN", L"UAC_AWARE", val);

    val = ConfigReadInt(L"MAIN", L"ENABLE_DPI_AWARENESS", CONFIG_MAIN_ENABLE_DPI_AWARENESS);
    ConfigWriteInt(L"MAIN", L"ENABLE_DPI_AWARENESS", val);

    val = ConfigReadInt(L"MAIN", L"ENABLE_CONTROLLER_ENHANCEMENTS", CONFIG_MAIN_ENABLE_CONTROLLER_ENHANCEMENTS);
    ConfigWriteInt(L"MAIN", L"ENABLE_CONTROLLER_ENHANCEMENTS", val);

    val = ConfigReadInt(L"MAIN", L"ENABLE_LINEAR_UPSCALING_GUI", CONFIG_MAIN_ENABLE_LINEAR_UPSCALING_GUI);
    ConfigWriteInt(L"MAIN", L"ENABLE_LINEAR_UPSCALING_GUI", val);
    val = ConfigReadInt(L"MAIN", L"ENABLE_LINEAR_UPSCALING_HUD", CONFIG_MAIN_ENABLE_LINEAR_UPSCALING_HUD);
    ConfigWriteInt(L"MAIN", L"ENABLE_LINEAR_UPSCALING_HUD", val);

    val = ConfigReadInt(L"MAIN", L"WINDOWED", CONFIG_MAIN_WINDOWED);
    ConfigWriteInt(L"MAIN", L"WINDOWED", val);
    val = ConfigReadInt(L"MAIN", L"DEAD_ZONE", CONFIG_MAIN_DEAD_ZONE);
    ConfigWriteInt(L"MAIN", L"DEAD_ZONE", val);

    val = ConfigReadInt(L"MAIN", L"SUBS_ENABLED", CONFIG_MAIN_SUBS_ENABLED);
    ConfigWriteInt(L"MAIN", L"SUBS_ENABLED", val);

    ConfigReadString(L"MAIN", L"SUBS_LANG", CONFIG_MAIN_SUBS_LANG, wchar_buff, MAX_PATH);
    ConfigWriteString(L"MAIN", L"SUBS_LANG", wchar_buff);
    ConfigReadString(L"MAIN", L"AUDIO_LANG", CONFIG_MAIN_AUDIO_LANG, wchar_buff, MAX_PATH);
    ConfigWriteString(L"MAIN", L"AUDIO_LANG", wchar_buff);

    val = ConfigReadInt(L"MAIN", L"NO_CD", CONFIG_MAIN_NO_CD);
    ConfigWriteInt(L"MAIN", L"NO_CD", val);
    val = ConfigReadInt(L"MAIN", L"NO_CD_UNRESTRICTED_MISSIONS", CONFIG_MAIN_NO_CD_UNRESTRICTED_MISSIONS);
    ConfigWriteInt(L"MAIN", L"NO_CD_UNRESTRICTED_MISSIONS", val);
 
    val = ConfigReadInt(L"MAIN", L"CYCLE_HZ_GUI", CONFIG_MAIN_CYCLE_HZ_GUI);
    ConfigWriteInt(L"MAIN", L"CYCLE_HZ_GUI", val);

    val = ConfigReadInt(L"SPACE", L"CYCLE_HZ_SPACE", CONFIG_SPACE_CYCLE_HZ_SPACE);
    ConfigWriteInt(L"SPACE", L"CYCLE_HZ_SPACE", val);
    val = ConfigReadInt(L"SPACE", L"ALT_FLIGHT_MODE", CONFIG_SPACE_ALT_FLIGHT_MODE);
    ConfigWriteInt(L"SPACE", L"ALT_FLIGHT_MODE", val);
    val = ConfigReadInt(L"SPACE", L"ROTATION_SPEED_KEY_REVERSE_ACTION", CONFIG_SPACE_ROTATION_SPEED_KEY_REVERSE_ACTION);
    ConfigWriteInt(L"SPACE", L"ROTATION_SPEED_KEY_REVERSE_ACTION", val);
    val = ConfigReadInt(L"SPACE", L"MAX_SPEED_AFTER_AFTERBURNER_JUMPING", CONFIG_SPACE_MAX_SPEED_AFTER_AFTERBURNER_JUMPING);
    ConfigWriteInt(L"SPACE", L"MAX_SPEED_AFTER_AFTERBURNER_JUMPING", val);


    val = ConfigReadInt(L"MOVIES", L"SHOW_ORIGINAL_MOVIES_INTERLACED", CONFIG_MOVIES_SHOW_ORIGINAL_INTERLACED);
    ConfigWriteInt(L"MOVIES", L"SHOW_ORIGINAL_MOVIES_INTERLACED", val);

    ConfigReadString(L"MOVIES", L"PATH", CONFIG_MOVIES_PATH, wchar_buff, MAX_PATH);
    ConfigWriteString(L"MOVIES", L"PATH", wchar_buff);
    ConfigReadString(L"MOVIES", L"EXT", CONFIG_MOVIES_EXT, wchar_buff, MAX_PATH);
    ConfigWriteString(L"MOVIES", L"EXT", wchar_buff);


    val = ConfigReadInt(L"MOUSE", L"DEAD_ZONE", CONFIG_MOUSE_DEAD_ZONE);
    ConfigWriteInt(L"MOUSE", L"DEAD_ZONE", val);
    val = ConfigReadInt(L"MOUSE", L"AXIS_LIMIT_PERCENTAGE", CONFIG_MOUSE_AXIS_LIMIT_PERCENTAGE);
    ConfigWriteInt(L"MOUSE", L"AXIS_LIMIT_PERCENTAGE", val);
    val = ConfigReadInt(L"MOUSE", L"INVERT_Y_AXIS", CONFIG_MOUSE_INVERT_Y_AXIS);
    ConfigWriteInt(L"MOUSE", L"INVERT_Y_AXIS", val);

    wchar_t profile_name[16];
    for (int i = 0; i < NUM_JOY_PROFILES; i++) {
        switch (i) {
        case static_cast<int>(PROFILE_TYPE::GUI):
            swprintf(profile_name, _countof(profile_name), L"MOUSE_GUI");
            val = ConfigReadInt(profile_name, L"BUTTON_01", static_cast<int>(P2_ACTIONS::Left_Click));
            ConfigWriteInt(profile_name, L"BUTTON_01", val);
            val = ConfigReadInt(profile_name, L"BUTTON_02", static_cast<int>(P2_ACTIONS::Right_Click));
            ConfigWriteInt(profile_name, L"BUTTON_02", val);
            break;
        case static_cast<int>(PROFILE_TYPE::Space):
            swprintf(profile_name, _countof(profile_name), L"MOUSE_SPACE");
            val = ConfigReadInt(profile_name, L"BUTTON_01", static_cast<int>(P2_ACTIONS::Left_Click));
            ConfigWriteInt(profile_name, L"BUTTON_01", val);
            val = ConfigReadInt(profile_name, L"BUTTON_02", static_cast<int>(P2_ACTIONS::Right_Click));
            ConfigWriteInt(profile_name, L"BUTTON_02", val);
            break;
        case static_cast<int>(PROFILE_TYPE::ReMap_1):
        case static_cast<int>(PROFILE_TYPE::ReMap_2):
        case static_cast<int>(PROFILE_TYPE::ReMap_3):
            swprintf(profile_name, _countof(profile_name), L"MOUSE_REMAP_%02d", i - static_cast<int>(PROFILE_TYPE::ReMap_1) + 1);
            val = ConfigReadInt(profile_name, L"BUTTON_01", static_cast<int>(P2_ACTIONS::None));
            ConfigWriteInt(profile_name, L"BUTTON_01", val);
            val = ConfigReadInt(profile_name, L"BUTTON_02", static_cast<int>(P2_ACTIONS::None));
            ConfigWriteInt(profile_name, L"BUTTON_02", val);
            break;
        default:
            continue;
        }
        val = ConfigReadInt(profile_name, L"BUTTON_03", static_cast<int>(P2_ACTIONS::None));
        ConfigWriteInt(profile_name, L"BUTTON_03", val);
        val = ConfigReadInt(profile_name, L"BUTTON_04", static_cast<int>(P2_ACTIONS::None));
        ConfigWriteInt(profile_name, L"BUTTON_04", val);
        val = ConfigReadInt(profile_name, L"BUTTON_05", static_cast<int>(P2_ACTIONS::None));
        ConfigWriteInt(profile_name, L"BUTTON_05", val);
        val = ConfigReadInt(profile_name, L"MOUSE_WHEEL_UP", static_cast<int>(P2_ACTIONS::None));
        ConfigWriteInt(profile_name, L"MOUSE_WHEEL_UP", val);
        val = ConfigReadInt(profile_name, L"MOUSE_WHEEL_DOWN", static_cast<int>(P2_ACTIONS::None));
        ConfigWriteInt(profile_name, L"MOUSE_WHEEL_DOWN", val);
        val = ConfigReadInt(profile_name, L"MOUSE_WHEEL_LEFT", static_cast<int>(P2_ACTIONS::None));
        ConfigWriteInt(profile_name, L"MOUSE_WHEEL_LEFT", val);
        val = ConfigReadInt(profile_name, L"MOUSE_WHEEL_RIGHT", static_cast<int>(P2_ACTIONS::None));
        ConfigWriteInt(profile_name, L"MOUSE_WHEEL_RIGHT", val);
    }


    //ConfigWriteInt(L"DEBUG", L"ERRORS", 1);
    val = ConfigReadInt(L"DEBUG", L"GENERAL", 0);
    ConfigWriteInt(L"DEBUG", L"GENERAL", val);
    val = ConfigReadInt(L"DEBUG", L"FLIGHT", 0);
    ConfigWriteInt(L"DEBUG", L"FLIGHT", val);
    val = ConfigReadInt(L"DEBUG", L"MOVIE", 0);
    ConfigWriteInt(L"DEBUG", L"MOVIE", val);
    val = ConfigReadInt(L"DEBUG", L"CONTROLLER", 0);
    ConfigWriteInt(L"DEBUG", L"CONTROLLER", val);

    delete[]wchar_buff;
    return TRUE;
}


//______________________________
static void Admin_Build_Config() {

    wchar_t exeName[MAX_PATH];
    GetModuleFileName(nullptr, exeName, MAX_PATH);

    SHELLEXECUTEINFO shExInfo = { 0 };
    shExInfo.cbSize = sizeof(shExInfo);
    shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shExInfo.hwnd = 0;
    shExInfo.lpVerb = L"runas";                 // Launch as Administrator, User Account Control (UAC) will prompt the user for consent.
    shExInfo.lpFile = exeName;
    shExInfo.lpParameters = L"/build_config";
    shExInfo.lpDirectory = 0;
    shExInfo.nShow = SW_SHOW;
    shExInfo.hInstApp = 0;

    Debug_Info_Error("Upgrading UAC privileges to Build/Update config files for: %S.", exeName);

    if (ShellExecuteEx(&shExInfo) && shExInfo.hProcess != nullptr) {
        WaitForSingleObject(shExInfo.hProcess, INFINITE);
        CloseHandle(shExInfo.hProcess);
    }
}


//_______________________________
static BOOL IsAppInProgramFiles() {

    static BOOL is_app_in_programfiles = FALSE;
    static bool app_in_programfiles_run_once = false;
    if (app_in_programfiles_run_once)
        return is_app_in_programfiles;

    wstring current_path = GetAppPath();

    wchar_t* p_program_files_path = nullptr;
    SHGetKnownFolderPath(FOLDERID_ProgramFiles, SHGFP_TYPE_CURRENT, nullptr, &p_program_files_path);

    size_t loc = current_path.find(p_program_files_path);
    CoTaskMemFree(p_program_files_path);

    if (loc == string::npos)
        is_app_in_programfiles = FALSE;
    else
        is_app_in_programfiles = TRUE;

    app_in_programfiles_run_once = true;
    return is_app_in_programfiles;
}


//_______________________
const char* GetAppPathA() {
    static string appPath;
    static bool app_path_set = false;

    if (!app_path_set) {
        DWORD currentDirSize = GetCurrentDirectoryA(0, nullptr);//get the path size
        char* dirCurrent = new char[currentDirSize];
        GetCurrentDirectoryA(currentDirSize, dirCurrent);
        appPath.assign("\\\\?\\");
        appPath.append(dirCurrent);
        delete[] dirCurrent;
        app_path_set = true;
    }
    return appPath.c_str();
}

//_________________________
const wchar_t* GetAppPath() {
    static wstring wAppPath;
    static bool app_path_set = false;

    if (!app_path_set) {
        DWORD currentDirSize = GetCurrentDirectory(0, nullptr);//get the path size
        wchar_t* dirCurrent = new wchar_t[currentDirSize];
        GetCurrentDirectory(currentDirSize, dirCurrent);
        wAppPath.assign(L"\\\\?\\");
        wAppPath.append(dirCurrent);
        delete[] dirCurrent;
        app_path_set = true;
    }
    return wAppPath.c_str();
}


//_____________________________
const wchar_t* GetAppDataPath() {
    static wstring wAppDataPath;
    static bool app_data_path_set = false;

    if (!app_data_path_set) {
        //This setting is only valid in the p2_dark_en.ini file located in the privateer 2 app folder.
        wstring local_ini = GetAppPath();
        local_ini.append(L"\\");
        local_ini.append(VER_PRODUCTNAME_STR);
        local_ini.append(L".ini");
        LONG UAC_AWARE = GetPrivateProfileInt(L"MAIN", L"UAC_AWARE", CONFIG_MAIN_UAC_AWARE, local_ini.c_str());

        if (UAC_AWARE == 2 || (UAC_AWARE == 1 && IsAppInProgramFiles())) {
            wchar_t* pRoamingAppData = nullptr;
            SHGetKnownFolderPath(FOLDERID_RoamingAppData, SHGFP_TYPE_CURRENT, nullptr, &pRoamingAppData);
            wAppDataPath.assign(L"\\\\?\\");// to allow for strings longer than MAX_PATH

            wAppDataPath.append(pRoamingAppData);
            CoTaskMemFree(pRoamingAppData);

            wAppDataPath.append(L"\\");
            wAppDataPath.append(VER_PRODUCTNAME_STR);

            if (GetFileAttributes(wAppDataPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
                if (!CreateDirectory(wAppDataPath.c_str(), nullptr)) {
                    wAppDataPath.clear();
                    //Debug_Info_Error("AppDataPath: folder creation FAILED!: %S", wAppDataPath.c_str());
                }
            }
        }
        else
            wAppDataPath.clear();
        app_data_path_set = true;
        //Debug_Info("AppDataPath: %S", wAppDataPath.c_str());
    }

    return wAppDataPath.c_str();
}


//_______________________________
static void ConfigCreate_InGame() {

    ConfigWriteInt_InGame(L"MAIN", L"WINDOWED", CONFIG_MAIN_WINDOWED);
    ConfigWriteInt_InGame(L"MAIN", L"WIN_DATA", CONFIG_MAIN_WIN_DATA);

    ConfigWriteInt_InGame(L"MAIN", L"MIDI_DEVICE_ID", CONFIG_MAIN_MIDI_DEVICE_ID);

    ConfigWriteInt_InGame(L"MAIN", L"DEAD_ZONE", CONFIG_MAIN_DEAD_ZONE);
    ConfigWriteInt_InGame(L"SPACE", L"ALT_FLIGHT_MODE", CONFIG_SPACE_ALT_FLIGHT_MODE);
    ConfigWriteInt_InGame(L"SPACE", L"ROTATION_SPEED_KEY_REVERSE_ACTION", CONFIG_SPACE_ROTATION_SPEED_KEY_REVERSE_ACTION);
}


//___________________________________________
static const wchar_t* Get_ConfigPath_InGame() {

    static bool config_path_set = false;
    static wstring wConfigPath;
    if (!config_path_set) {
        config_path_set = true;

        wConfigPath.assign(GetAppDataPath());
        bool is_app_folder = false;
        if (wConfigPath.empty()) {
            is_app_folder = true;
            wConfigPath.assign(GetAppPath());
        }

        wConfigPath.append(L"\\");
        wConfigPath.append(VER_PRODUCTNAME_STR);
        wConfigPath.append(L"_ingame");
        wConfigPath.append(L".ini");

        if (GetFileAttributes(wConfigPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
            if (!is_app_folder) { //if no ini file exists on the UAC path, first attempt to copy the ini from the local path.
                wstring local_ini = GetAppPath();
                local_ini.append(L"\\");
                local_ini.append(VER_PRODUCTNAME_STR);
                local_ini.append(L"_ingame");
                local_ini.append(L".ini");
                if (GetFileAttributes(local_ini.c_str()) != INVALID_FILE_ATTRIBUTES && CopyFile(local_ini.c_str(), wConfigPath.c_str(), TRUE))
                    return wConfigPath.c_str();
            }
            ConfigCreate_InGame();
        }
    }
    return wConfigPath.c_str();
}


//____________________________________
static const wchar_t* Get_ConfigPath() {
    
    static bool config_path_set = false;
    static wstring wConfigPath;
    if (!config_path_set) {
        config_path_set = true;

        wConfigPath.assign(GetAppPath());

        wConfigPath.append(L"\\");
        wConfigPath.append(VER_PRODUCTNAME_STR);
        wConfigPath.append(L".ini");
    }
    return wConfigPath.c_str();
}


//____________________________________
static void Check_Config_Files_Exist() {

    if (GetFileAttributes(Get_ConfigPath()) == INVALID_FILE_ATTRIBUTES) {
        Debug_Info_Error("Building: %S.ini.", VER_PRODUCTNAME_STR);
        if (IsAppInProgramFiles())
            Admin_Build_Config();
        else
            Build_Config();
    }
    else if (!Is_Config_Updated()) {
        Debug_Info_Error("Updating: %S.ini", VER_PRODUCTNAME_STR);
        if (IsAppInProgramFiles())
            Admin_Build_Config();
        else
            Update_Config();
    }
}


//_________________________________
void Check_Command_Line_Overrides() {

    LPWSTR* szArglist;
    int nArgs;

    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (szArglist == nullptr) {
        Debug_Info_Error("CommandLineToArgvW failed.");
        return;
    }

    for (int i = 1; i < nArgs; i++) {
        //Debug_Info("%d: %S", i, szArglist[i]);
        if (wcsncmp(szArglist[i], L"/movies_path=", 13) == 0)
            movies_path_override.assign(szArglist[i] + 13);
        else if (wcsncmp(szArglist[i], L"/movies_ext=", 12) == 0)
            movies_ext_override.assign(szArglist[i] + 12);
        else if (wcsncmp(szArglist[i], L"/build_config", 13) == 0) {
            if (GetFileAttributes(Get_ConfigPath()) == INVALID_FILE_ATTRIBUTES)
                Build_Config();
            else
                Update_Config();
            ExitProcess(0);
        }
    }
    LocalFree(szArglist);

    Check_Config_Files_Exist();
}


//_______________________
void ConfigRefreshCache() {
    WritePrivateProfileString(nullptr, nullptr, nullptr, Get_ConfigPath());
}


//__________________________________________________________________________________________________________________________________________
DWORD ConfigReadString(const wchar_t* lpAppName, const wchar_t* lpKeyName, const wchar_t* lpDefault, wchar_t* lpReturnedString, DWORD nSize) {
    return GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, Get_ConfigPath());
}


//_________________________________________________________________________________________________
BOOL ConfigWriteString(const wchar_t* lpAppName, const wchar_t* lpKeyName, const wchar_t* lpString) {
    return WritePrivateProfileString(lpAppName, lpKeyName, lpString, Get_ConfigPath());
}


//__________________________________________________________________________________
UINT ConfigReadInt(const wchar_t* lpAppName, const wchar_t* lpKeyName, int nDefault) {
    return GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, Get_ConfigPath());
}


//_________________________________________________________________________________________
UINT ConfigReadInt_InGame(const wchar_t* lpAppName, const wchar_t* lpKeyName, int nDefault) {

    nDefault = ConfigReadInt(lpAppName, lpKeyName, nDefault);// check for defaults in main ini file;
    return GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, Get_ConfigPath_InGame());
}


//_________________________________________________________________________________
BOOL ConfigWriteInt(const wchar_t* lpAppName, const wchar_t* lpKeyName, int intVal) {
    wchar_t lpString[64]{0};
    swprintf_s(lpString, 64, L"%d", intVal);
    return WritePrivateProfileString(lpAppName, lpKeyName, lpString, Get_ConfigPath());
}


//________________________________________________________________________________________
BOOL ConfigWriteInt_InGame(const wchar_t* lpAppName, const wchar_t* lpKeyName, int intVal) {
    wchar_t lpString[64]{ 0 };
    swprintf_s(lpString, 64, L"%d", intVal);
    return WritePrivateProfileString(lpAppName, lpKeyName, lpString, Get_ConfigPath_InGame());
}


//___________________________________________________________________________________________________
BOOL ConfigReadWinData(const wchar_t* lpAppName, const wchar_t* lpKeyName, WINDOWPLACEMENT* pWinData) {
    pWinData->length = sizeof(WINDOWPLACEMENT);
    return GetPrivateProfileStruct(lpAppName, lpKeyName, pWinData, sizeof(WINDOWPLACEMENT), Get_ConfigPath_InGame());
}


//____________________________________________________________________________________________________
BOOL ConfigWriteWinData(const wchar_t* lpAppName, const wchar_t* lpKeyName, WINDOWPLACEMENT* pWinData) {
    return WritePrivateProfileStruct(lpAppName, lpKeyName, pWinData, sizeof(WINDOWPLACEMENT), Get_ConfigPath_InGame());
}


//__________________________________________________________________________________________________________
BOOL ConfigReadStruct(const wchar_t* lpszSection, const wchar_t* lpszKey, LPVOID lpStruct, UINT uSizeStruct) {
    return GetPrivateProfileStruct(lpszSection, lpszKey, lpStruct, uSizeStruct, Get_ConfigPath());
}


//___________________________________________________________________________________________________________
BOOL ConfigWriteStruct(const wchar_t* lpszSection, const wchar_t* lpszKey, LPVOID lpStruct, UINT uSizeStruct) {
    return WritePrivateProfileStruct(lpszSection, lpszKey, lpStruct, uSizeStruct, Get_ConfigPath());
}
