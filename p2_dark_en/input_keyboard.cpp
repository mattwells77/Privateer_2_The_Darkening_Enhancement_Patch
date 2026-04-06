/*
The MIT License (MIT)
Copyright © 2026 Matt Wells

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
#include "dark.h"
#include "input.h"
#include "version.h"

using namespace std;


BYTE P2_ACTIONS_KEYS[][4]{
    0x00, 0x00, 0x00, 0x00,// None,
    'X', 0x04, 0x00, 0x00,// ALT X,  exit game
    VK_BACK, 0x08, 0x00, 0x00,// BCKSPC, drop mine/decoy
    'Q', 0x00, 0x10, 0x00,// Q,      select target in crosshairs
    'A', 0x08, 0x10, 0x00,// A,      cycle targets
    'Z', 0x08, 0x10, 0x00,// Z,      cycle targets back
    'C', 0x08, 0x10, 0x00,// C,      VDW Comms
    'D', 0x08, 0x00, 0x00,// D,      VDW Damage
    'O', 0x04, 0x00, 0x00,// ALT O,  Game_Options
    'N', 0x08, 0x00, 0x00,// N,      Select/Deselect Weapon in VDW
    'H', 0x08, 0x00, 0x00,// H,      Cycle Weapons in VDW
    'G', 0x08, 0x10, 0x00,// G,      VDW Guns
    'M', 0x08, 0x10, 0x00,// M,      VDW Missiles
    'B', 0x04, 0x00, 0x00,// ALT B,  Activate BSE
    'B', 0x08, 0x00, 0x00,// B,      VDW Mines/Decoys
    VK_RETURN, 0x08, 0x10, 0x00,// ENTR,   Fire Missile
    'E', 0x08, 0x10, 0x00,// E,      Cycle Radar Ranges
    VK_TAB, 0x08, 0x00, 0x00,// TAB,    Afterburner
    'R', 0x04, 0x00, 0x00,// ALT R,  Toggle Radar Type
    VK_OEM_PLUS, 0x08, 0x00, 0x00,// =+,     Speed Increase
    VK_ADD, 0x08, 0x00, 0x00,// +,      Speed Increase 2
    VK_OEM_MINUS, 0x08, 0x00, 0x00,// -_,     Speed Decrease
    VK_SUBTRACT, 0x08, 0x00, 0x00,// -,      Speed Decrease 2
    'J', 0x08, 0x10, 0x00,// J,      Jump
    'O', 0x08, 0x10, 0x00,// O,      Toggle SOS
    'F', 0x08, 0x10, 0x00,// F,      Toggle Full/No Guns
    'T', 0x04, 0x00, 0x00,// ALT T,  Targeting Off
    'T', 0x08, 0x00, 0x00,// T,      Tractor Targeted Object
    'Y', 0x08, 0x10, 0x00,// Y,      Drop Tractored Object
    'N', 0x04, 0x00, 0x00,// ALT N,  Nav Map
    'S', 0x04, 0x00, 0x00,// ALT S,  Drop Nuke'em
    'D', 0x04, 0x00, 0x00,// ALT D,  Electronic Diary
    'X', 0x08, 0x10, 0x00,// X,      Match Target Speed
    'H', 0x04, 0x00, 0x00,// ALT H,  Hotkey List
    'O', 0x04, 0x00, 0x00,// ALT O,  Game_Options 2??
    'W', 0x04, 0x00, 0x00,// ALT W,  Warp Shields
    'W', 0x08, 0x00, 0x00,// W,      Target Nearest Hostile
    'S', 0x08, 0x00, 0x00,// S,      Target Nearest Friendly
    VK_LEFT, 0x08, 0x00, 0x00,// LT,     Exterior_Ship_View_Left
    VK_RIGHT, 0x08, 0x00, 0x00,// RT,     Exterior_Ship_View_Right
    VK_UP, 0x08, 0x00, 0x00,// UP,     Exterior_Ship_View_Up
    VK_DOWN, 0x08, 0x00, 0x00,// DN,     Exterior_Ship_View_Down
    VK_OEM_4, 0x08, 0x00, 0x00,// [{,     speed zero
    VK_OEM_6, 0x08, 0x00, 0x00,// ]},     speed max

    VK_OEM_PERIOD, 0x08, 0x00, 0x00,// >,      Roll Right (Joystick)
    VK_OEM_COMMA, 0x08, 0x00, 0x00,// < ,     Roll Left (Joystick)
    VK_SPACE, 0x08, 0x00, 0x00,// space,  Fire Guns


    VK_F1, 0x08, 0x00, 0x00,// F1
    VK_F2, 0x08, 0x00, 0x00,// F2
    VK_F3, 0x08, 0x00, 0x00,// F3
    VK_F4, 0x08, 0x00, 0x00,// F4
    VK_F5, 0x08, 0x00, 0x00,// F5
    VK_F6, 0x08, 0x00, 0x00,// F6
    VK_F7, 0x08, 0x00, 0x00,// F7
    VK_F8, 0x08, 0x00, 0x00,// F8
    VK_F9, 0x08, 0x00, 0x00,// F9
    VK_F10, 0x08, 0x00, 0x00,// F10


    '1', 0x08, 0x00, 0x00,// Recall Target 1
    '2', 0x08, 0x00, 0x00,// Recall Target 2
    '3', 0x08, 0x00, 0x00,// Recall Target 3
    '4', 0x08, 0x00, 0x00,// Recall Target 4
    '5', 0x08, 0x00, 0x00,// Recall Target 5
    '6', 0x08, 0x00, 0x00,// Recall Target 6
    '7', 0x08, 0x00, 0x00,// Recall Target 7
    '8', 0x08, 0x00, 0x00,// Recall Target 8
    '9', 0x08, 0x00, 0x00,// Recall Target 9

    //movie choice
    VK_UP, 0x08, 0x00, 0x00,// Up,
    VK_DOWN, 0x08, 0x00, 0x00,// Down,
    VK_RETURN, 0x08, 0x10, 0x00,// ENTR,


    //gen gui//
    'P', 0x08, 0x10, 0x00,// P,        PAD
    'X', 0x04, 0x10, 0x00,// ALT X,  exit game
    'N', 0x08, 0x00, 0x00,// No
    'Y', 0x08, 0x00, 0x00,// Yes

    //main load scrn
    VK_ESCAPE, 0x08, 0x00, 0x00,// ESC
    VK_RETURN, 0x08, 0x00, 0x00,// ENTR

    //unk01
    VK_RETURN, 0x08, 0x00, 0x00,// ENTR

    //space_exit
    'N', 0x08, 0x00, 0x00,// No
    'Y', 0x08, 0x00, 0x00,// Yes

    //nav
    'F', 0x08, 0x10, 0x00,// F,      type nav point num
    VK_OEM_PLUS, 0x08, 0x00, 0x00,// =+,     zoom in
    VK_ADD, 0x08, 0x00, 0x00,// +,      zoom in 2
    VK_OEM_MINUS, 0x08, 0x00, 0x00,// -_,     zoom out
    VK_SUBTRACT, 0x08, 0x00, 0x00,// -,      zoom out 2
    'C', 0x08, 0x10, 0x00,// C,      reset view
    'R', 0x08, 0x10, 0x00,// R,      roll view
    '1', 0x08, 0x10, 0x00,// 1
    '2', 0x08, 0x10, 0x00,// 2
    '3', 0x08, 0x10, 0x00,// 3
    '4', 0x08, 0x10, 0x00,// 4
    VK_ESCAPE, 0x08, 0x00, 0x00,// ESC

    //email?
    VK_ESCAPE, 0x08, 0x00, 0x00,// ESC 
    //diary
    VK_ESCAPE, 0x08, 0x00, 0x00,// ESC
    //options
    VK_ESCAPE, 0x08, 0x00, 0x00,// ESC
    VK_ESCAPE, 0x08, 0x00, 0x00,// ESC 2
    //hot keys
    VK_ESCAPE, 0x08, 0x00, 0x00,// ESC
    VK_ESCAPE, 0x08, 0x00, 0x00,// ESC 2

    //movie playback
    VK_ESCAPE, 0x08, 0x10, 0x00,// ESC
    VK_OEM_PLUS, 0x08, 0x10, 0x00,// =+,
    VK_OEM_MINUS, 0x08, 0x10, 0x00,// -_,

    //joy keys
    VK_UP, 0x08, 0x00, 0x00,// Pitch_Down,
    VK_DOWN, 0x08, 0x00, 0x00,// Pitch_Up,
    VK_LEFT, 0x08, 0x00, 0x00,// Yaw_Left,
    VK_RIGHT, 0x08, 0x00, 0x00,// Yaw_Right,

    VK_NUMPAD1, 0x08, 0x00, 0x00,// Pitch Up, Yaw Left,
    VK_NUMPAD2, 0x08, 0x00, 0x00,// Pitch Up,
    VK_NUMPAD3, 0x08, 0x00, 0x00,// Pitch Up, Yaw Right,
    VK_NUMPAD4, 0x08, 0x00, 0x00,// Yaw Left,
    VK_NUMPAD6, 0x08, 0x00, 0x00,// Yaw Right,
    VK_NUMPAD7, 0x08, 0x00, 0x00,// Pitch Down, Yaw Left,
    VK_NUMPAD8, 0x08, 0x00, 0x00,// Pitch Down,
    VK_NUMPAD9, 0x08, 0x00, 0x00,// Pitch Down, Yaw Right,

    VK_NUMPAD0, 0x08, 0x00, 0x00,// Roll Left,
    VK_DECIMAL, 0x08, 0x00, 0x00,// Roll Right,

    '0', 0x01, 0x00, 0x00,// store target 1,

    //mouse keys
    VK_SPACE, 0x08, 0x10, 0x00,// SPACE,    Left Click
    VK_TAB, 0x08, 0x10, 0x00,// TAB,      Right Click

    0x00, 0x00, 0x00, 0x00,// combo1
    0x00, 0x00, 0x00, 0x00,//  combo2
    0x00, 0x00, 0x00, 0x00,//  combo2


    VK_LBUTTON, 0x00, 0x00, 0x00,
    VK_RBUTTON, 0x00, 0x00, 0x00,

    VK_CONTROL, 0x2, 0x00, 0x00,// Double_Yaw_Pitch_Roll_Rates,

    0x00, 0x00, 0x00, 0x00,//  joy_roll_modifier

    '1', 0x01, 0x00, 0x00,// Store Target 1
    '2', 0x01, 0x00, 0x00,// Store Target 2
    '3', 0x01, 0x00, 0x00,// Store Target 3
    '4', 0x01, 0x00, 0x00,// Store Target 4
    '5', 0x01, 0x00, 0x00,// Store Target 5
    '6', 0x01, 0x00, 0x00,// Store Target 6
    '7', 0x01, 0x00, 0x00,// Store Target 7
    '8', 0x01, 0x00, 0x00,// Store Target 8
    '9', 0x01, 0x00, 0x00,// Store Target 9
    '0', 0x08, 0x00, 0x00,// Recall Target 0
};


//______________
BOOL Keys_Save() {

    wstring w_path = GetAppDataPath();

    if (!w_path.empty())
        w_path.append(L"\\");
    w_path.append(VER_PRODUCTNAME_STR);
    w_path.append(L"_keys");
    w_path.append(L".dat");

    FILE* fileCache = nullptr;
    DWORD profile_type = GAME_CODE;
    DWORD keys_dat_txt = KEYS_DAT_CODE;
    int i_data = 10;
    DWORD dw_data = 0;

    if (_wfopen_s(&fileCache, w_path.c_str(), L"wb") == 0 && fileCache) {
        fwrite(&keys_dat_txt, sizeof(DWORD), 1, fileCache);
        fwrite(&profile_type, sizeof(DWORD), 1, fileCache);
        dw_data = sizeof(P2_ACTIONS_KEYS);
        fwrite(&dw_data, sizeof(DWORD), 1, fileCache);
        fwrite(P2_ACTIONS_KEYS, sizeof(P2_ACTIONS_KEYS), 1, fileCache);
        fclose(fileCache);
    }
    else {
        Debug_Info_Error("Keys_Save(), _wfopen_s failed: %S", w_path);
        return FALSE;
    }
    //Debug_Info("Keys_Save(), Save Success: %S", w_path);
    return TRUE;
}


//______________
BOOL Keys_Load() {
    wstring w_path = GetAppDataPath();
    bool is_app_folder = false;

    if (w_path.empty())
        is_app_folder = true;
    else
        w_path.append(L"\\");

    w_path.append(VER_PRODUCTNAME_STR);
    w_path.append(L"_keys");
    w_path.append(L".dat");


    if (GetFileAttributes(w_path.c_str()) == INVALID_FILE_ATTRIBUTES) {
        if (!is_app_folder) { //if no ini file exists on the UAC path, first attempt to copy from the local path.
            wstring local_path;
            local_path.assign(VER_PRODUCTNAME_STR);
            local_path.append(L"_keys");
            local_path.append(L".dat");
            if (GetFileAttributes(local_path.c_str()) != INVALID_FILE_ATTRIBUTES)
                CopyFile(local_path.c_str(), w_path.c_str(), TRUE);
        }
        //save the current keys if no files are found in the local or appdata paths.
        if (GetFileAttributes(w_path.c_str()) == INVALID_FILE_ATTRIBUTES)
            Keys_Save();
    }

    FILE* fileCache = nullptr;

    DWORD keys_dat_txt = KEYS_DAT_CODE;
    int i_data = 10;
    DWORD d_data = 0;
    if (_wfopen_s(&fileCache, w_path.c_str(), L"rb") == 0 && fileCache) {
        fread(&keys_dat_txt, sizeof(DWORD), 1, fileCache);
        if (keys_dat_txt > KEYS_DAT_CODE) {
            fclose(fileCache);
            Debug_Info_Error("Keys_Load(), Not KEYS dat");
            return FALSE;
        }
        fread(&d_data, sizeof(DWORD), 1, fileCache);
        if (d_data != GAME_CODE) {
            fclose(fileCache);
            DWORD type = GAME_CODE;
            Debug_Info_Error("Keys_Load(), type mismatch, found: %c%c%c%c, expected: %c%c%c%c", ((BYTE*)&d_data)[0], ((BYTE*)&d_data)[1], ((BYTE*)&d_data)[2], ((BYTE*)&d_data)[3], ((BYTE*)&type)[0], ((BYTE*)&type)[1], ((BYTE*)&type)[2], ((BYTE*)&type)[3]);
            return FALSE;
        }
        //dw_data = sizeof(P2_ACTIONS_KEYS);
        fread(&d_data, sizeof(DWORD), 1, fileCache);
        if (d_data != sizeof(P2_ACTIONS_KEYS)) {
            fclose(fileCache);
            Debug_Info_Error("Keys_Load(), data size mismatch, file :%d, expected: %d", d_data, sizeof(P2_ACTIONS_KEYS));
            return FALSE;
        }

        fread(P2_ACTIONS_KEYS, sizeof(BYTE), sizeof(P2_ACTIONS_KEYS), fileCache);
        fclose(fileCache);
    }
    else {
        Debug_Info_Error("Keys_Load(), _wfopen_s failed: %S", w_path);
        return FALSE;
    }
    //Debug_Info("Keys_Load(), Load Success: %S", w_path);
    return TRUE;
}


//_____________________________________________________________
bool Get_Key_State(BYTE key, BYTE mod_key_flags, BYTE run_once) {

    bool ret_val = false;
    mod_key_flags &= 0xF7;// ignore flag 0x8

    BYTE mod_keys = p2_keyboard_state_main[VK_SHIFT] >> 7;
    mod_keys |= p2_keyboard_state_main[VK_CONTROL] >> 6;
    mod_keys |= p2_keyboard_state_main[VK_MENU] >> 5;

    if (run_once & 0x10) {
        if (p2_keyboard_state_last[key] != 0)
            ret_val |= true;
        p2_keyboard_state_last[key] = 0;
    }
    else {
        if (p2_keyboard_state_main[key] != 0)
            ret_val |= true;
    }

    //always allow these actions to pass whether a mod key is down or not. 
    if (key == P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Fire_Guns)][0] ||
        key == P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Fire_Missile)][0] ||
        key == P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Afterburner)][0])
        return ret_val;

    //if mod keys don't match expected mod keys but mod keys expected or if there are mod keys but no mod keys expected.
    if ((!(mod_keys & mod_key_flags) && mod_key_flags) || (mod_keys && !mod_key_flags)) {
        return false;
    }
    return ret_val;
}

