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
#include "modifications.h"
#include "memwrite.h"
#include "dark.h"
#include "input.h"


//____________________________________________________
static void __declspec(naked) key_check_exit_win(void) {

    __asm {
        mov eax, 1
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_____________________________________________________
static void __declspec(naked) key_check_mine_drop(void) {

    __asm {
        mov eax, 2
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_______________________________________________________________________
static void __declspec(naked) key_check_select_target_in_crosshairs(void) {

    __asm {

        mov eax, 3
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_________________________________________________________
static void __declspec(naked) key_check_cycle_targets(void) {

    __asm {
        mov eax, 4
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//______________________________________________________________
static void __declspec(naked) key_check_cycle_targets_back(void) {

    __asm {
        mov eax, 5
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_____________________________________________________
static void __declspec(naked) key_check_vdw_comms(void) {

    __asm {
        mov eax, 6
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//______________________________________________________
static void __declspec(naked) key_check_vdw_damage(void) {

    __asm {
        mov eax, 7
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//____________________________________________________
static void __declspec(naked) key_check_settings(void) {

    __asm {
        mov eax, 8
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//___________________________________________________________
static void __declspec(naked) key_check_select_item_vdu(void) {

    __asm {
        mov eax, 9
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//___________________________________________________________
static void __declspec(naked) key_check_cycle_items_vdu(void) {

    __asm {
        mov eax, 10
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//____________________________________________________
static void __declspec(naked) key_check_vdw_guns(void) {

    __asm {
        mov eax, 11
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//________________________________________________________
static void __declspec(naked) key_check_vdw_missiles(void) {

    __asm {
        mov eax, 12
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//________________________________________________________
static void __declspec(naked) key_check_activate_bse(void) {

    __asm {
        mov eax, 13
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//____________________________________________________________
static void __declspec(naked) key_check_vdw_mines_decoys(void) {

    __asm {
        mov eax, 14
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//________________________________________________________
static void __declspec(naked) key_check_fire_missile(void) {

    __asm {
        mov eax, 15
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//______________________________________________________________
static void __declspec(naked) key_check_cycle_radar_ranges(void) {

    __asm {
        mov eax, 16
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_______________________________________________________
static void __declspec(naked) key_check_afterburner(void) {

    __asm {
        mov eax, 17
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_____________________________________________________________
static void __declspec(naked) key_check_toggle_radar_type(void) {

    __asm {
        mov eax, 18
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//__________________________________________________________
static void __declspec(naked) key_check_speed_increase(void) {

    __asm {
        mov eax, 19
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//____________________________________________________________
static void __declspec(naked) key_check_speed_increase_2(void) {

    __asm {
        mov eax, 20
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//__________________________________________________________
static void __declspec(naked) key_check_speed_decrease(void) {

    __asm {
        mov eax, 21
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//____________________________________________________________
static void __declspec(naked) key_check_speed_decrease_2(void) {

    __asm {
        mov eax, 22
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//________________________________________________
static void __declspec(naked) key_check_jump(void) {

    __asm {
        mov eax, 23
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//______________________________________________________
static void __declspec(naked) key_check_toggle_sos(void) {

    __asm {
        mov eax, 24
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_______________________________________________________________
static void __declspec(naked) key_check_toggle_full_no_guns(void) {

    __asm {
        mov eax, 25
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_________________________________________________________
static void __declspec(naked) key_check_targeting_off(void) {

    __asm {
        mov eax, 26
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//___________________________________________________________________
static void __declspec(naked) key_check_tractor_targeted_object(void) {

    __asm {
        mov eax, 27
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_________________________________________________________________
static void __declspec(naked) key_check_drop_tractored_object(void) {

    __asm {
        mov eax, 28
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//___________________________________________________
static void __declspec(naked) key_check_nav_map(void) {

    __asm {
        mov eax, 29
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//________________________________________________________
static void __declspec(naked) key_check_drop_nuke_em(void) {

    __asm {
        mov eax, 30
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//____________________________________________________________
static void __declspec(naked) key_check_electronic_diary(void) {

    __asm {
        mov eax, 31
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//______________________________________________________________
static void __declspec(naked) key_check_match_target_speed(void) {

    __asm {
        mov eax, 32
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_______________________________________________________
static void __declspec(naked) key_check_hotkey_list(void) {

    __asm {
        mov eax, 33
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//______________________________________________________
static void __declspec(naked) key_check_settings_2(void) {

    __asm {
        mov eax, 34
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//________________________________________________________
static void __declspec(naked) key_check_warp_shields(void) {

    __asm {
        mov eax, 35
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//__________________________________________________________________
static void __declspec(naked) key_check_target_nearest_hostile(void) {

    __asm {
        mov eax, 36
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//___________________________________________________________________
static void __declspec(naked) key_check_target_nearest_friendly(void) {

    __asm {
        mov eax, 37
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//______________________________________________
static void __declspec(naked) key_check_CB(void) {

    __asm {
        mov eax, 38
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//______________________________________________
static void __declspec(naked) key_check_CD(void) {

    __asm {
        mov eax, 39
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//______________________________________________
static void __declspec(naked) key_check_C8(void) {

    __asm {
        mov eax, 40
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//______________________________________________
static void __declspec(naked) key_check_D0(void) {

    __asm {
        mov eax, 41
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//______________________________________________________
static void __declspec(naked) key_check_speed_zero(void) {

    __asm {
        mov eax, 42
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_____________________________________________________
static void __declspec(naked) key_check_speed_max(void) {

    __asm {
        mov eax, 43
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//______________________________________________________
static void __declspec(naked) key_check_roll_right(void) {

    __asm {
        mov eax, 44
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_____________________________________________________
static void __declspec(naked) key_check_roll_left(void) {

    __asm {
        mov eax, 45
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_____________________________________________________
static void __declspec(naked) key_check_fire_guns(void) {

    __asm {
        mov eax, 46
        shl eax, 0x2
        lea eax, [P2_ACTIONS_KEYS + eax]

        mov bl, [eax + 2]
        mov dl, [eax + 1]
        mov al, [eax + 0]
        call p_p2_check_key_state
        ret
    }
}


//_______________________________________________
static void __declspec(naked) get_key_state(void) {

    __asm {

        push ecx
        push esi
        push edi
        push ebp

        push ebx
        push edx
        push eax
        call Get_Key_State
        add esp, 0xC

        pop ebp
        pop edi
        pop esi
        pop ecx

        ret
    }
}


//_____________________________________________________________________________
static bool Get_Key_State_Scancode(BYTE key, BYTE mod_key_flags, BYTE run_once) {

    return Get_Key_State((BYTE)MapVirtualKeyA(key, MAPVK_VSC_TO_VK), mod_key_flags, run_once);
}


//__________________________________________________________
static void __declspec(naked) check_key_state_sc_to_vk(void) {

    __asm {

        push ecx
        push esi
        push edi
        push ebp

        push ebx
        push edx
        push eax
        call Get_Key_State_Scancode
        add esp, 0xC

        pop ebp
        pop edi
        pop esi
        pop ecx

        ret
    }
}


//__________________________________________________________________________________________
static bool Get_Key_State_Scancode_Ignore_Shift(BYTE key, BYTE mod_key_flags, BYTE run_once) {
    if (p2_keyboard_state_main[VK_SHIFT] & 0x80)
        mod_key_flags |= p2_keyboard_state_main[VK_SHIFT] >> 7;
    return Get_Key_State((BYTE)MapVirtualKeyA(key, MAPVK_VSC_TO_VK), mod_key_flags, run_once);
}


//_______________________________________________________________________
static void __declspec(naked) check_key_state_sc_to_vk_ignore_shift(void) {

    __asm {

        push ecx
        push esi
        push edi
        push ebp

        push ebx
        push edx
        push eax
        call Get_Key_State_Scancode_Ignore_Shift
        add esp, 0xC

        pop ebp
        pop edi
        pop esi
        pop ecx

        ret
    }
}


//___________________________
void Modifications_KeyBoard() {

    //Replace the function for checking the state of keys, for greater control.
    MemWrite8(0x46A1C8, 0x51, 0xE9);
    FuncWrite32(0x46A1C9, 0xE381C488, (DWORD)&get_key_state);
    MemWrite32(0x46A1CD, 0xFF, 0x90909090);

    //Jump over DirectInput GetDeviceState. Keyboard state is now filled in WinProc_Main func.
    MemWrite16(0x46A0DA, 0x158B, 0x30EB); //JMP SHORT 0046A10C
    MemWrite32(0x46A0DC, 0x55D2D4, 0x90909090);

    //space key checks
    //00440B97 | .E8 2C960200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 2D X 4 0
    FuncReplace32(0x440B98, 0x02962C, (DWORD)&key_check_exit_win);
    //00440BE0 | .E8 E3950200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 0E BCKSPC 8 0
    FuncReplace32(0x440BE1, 0x0295E3, (DWORD)&key_check_mine_drop);
    //00440C88 | .E8 3B950200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 10 Q 0 10
    FuncReplace32(0x440C89, 0x02953B, (DWORD)&key_check_select_target_in_crosshairs);
    //00440EE2 | .E8 E1920200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 1E A 8 10
    FuncReplace32(0x440EE3, 0x0292E1, (DWORD)&key_check_cycle_targets);
    //00440F09 | .E8 BA920200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 2C Z 8 10
    FuncReplace32(0x440F0A, 0x0292BA, (DWORD)&key_check_cycle_targets_back);
    //00440F33 | .E8 90920200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 2E C 8 10
    FuncReplace32(0x440F34, 0x029290, (DWORD)&key_check_vdw_comms);
    //00440F55 | .E8 6E920200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 20 D 8 0
    FuncReplace32(0x440F56, 0x02926E, (DWORD)&key_check_vdw_damage);
    //00440F9A | .E8 29920200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 18 O 4 0
    FuncReplace32(0x440F9B, 0x029229, (DWORD)&key_check_settings);
    //0044101C | .E8 A7910200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 31 N 8 0
    FuncReplace32(0x44101D, 0x0291A7, (DWORD)&key_check_select_item_vdu);
    //00441099 | .E8 2A910200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 23 H 8 0
    FuncReplace32(0x44109A, 0x02912A, (DWORD)&key_check_cycle_items_vdu);
    //00441140 | .E8 83900200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 22 G 8 10
    FuncReplace32(0x441141, 0x029083, (DWORD)&key_check_vdw_guns);
    //00441203 | .E8 C08F0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 32 M 8 10
    FuncReplace32(0x441204, 0x028FC0, (DWORD)&key_check_vdw_missiles);
    //004412C3 | .E8 008F0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 30 B 4 0
    FuncReplace32(0x4412C4, 0x028F00, (DWORD)&key_check_activate_bse);
    //00441525 | .E8 9E8C0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 30 B 8 0
    FuncReplace32(0x441526, 0x028C9E, (DWORD)&key_check_vdw_mines_decoys);
    //0044160F | .E8 B48B0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 1C ENTR 8 10
    FuncReplace32(0x441610, 0x028BB4, (DWORD)&key_check_fire_missile);
    //00441634 | .E8 8F8B0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 12 E 8 10
    FuncReplace32(0x441635, 0x028B8F, (DWORD)&key_check_cycle_radar_ranges);
    //004416D5 | .E8 EE8A0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 0F TAB 8 0
    FuncReplace32(0x4416D6, 0x028AEE, (DWORD)&key_check_afterburner);
    //00441747 | .E8 7C8A0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 13 R 4 0
    FuncReplace32(0x441748, 0x028A7C, (DWORD)&key_check_toggle_radar_type);
    //0044178E | .E8 358A0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 0D =+ 8 0
    FuncReplace32(0x44178F, 0x028A35, (DWORD)&key_check_speed_increase);
    //004417A3 | .E8 208A0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 4E + 8 0
    FuncReplace32(0x4417A4, 0x028A20, (DWORD)&key_check_speed_increase_2);
    //0044188F | .E8 34890200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 0C -_ 8 0
    FuncReplace32(0x441890, 0x028934, (DWORD)&key_check_speed_decrease);
    //004418A4 | .E8 1F890200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 4A - 8 0
    FuncReplace32(0x4418A5, 0x02891F, (DWORD)&key_check_speed_decrease_2);
    //00441993 | .E8 30880200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 24 J 8 10
    FuncReplace32(0x441994, 0x028830, (DWORD)&key_check_jump);
    //00441B0C | .E8 B7860200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 18 O 8 10
    FuncReplace32(0x441B0D, 0x0286B7, (DWORD)&key_check_toggle_sos);
    //00441B77 | .E8 4C860200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 21 F 8 10
    FuncReplace32(0x441B78, 0x02864C, (DWORD)&key_check_toggle_full_no_guns);
    //00441C73 | .E8 50850200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 14 T 4 0
    FuncReplace32(0x441C74, 0x028550, (DWORD)&key_check_targeting_off);
    //00441CF6 | .E8 CD840200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 14 T 8 0
    FuncReplace32(0x441CF7, 0x0284CD, (DWORD)&key_check_tractor_targeted_object);
    //00442043 | .E8 80810200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 15 Y 8 10
    FuncReplace32(0x442044, 0x028180, (DWORD)&key_check_drop_tractored_object);
    //004420CB | .E8 F8800200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 31 N 4 0
    FuncReplace32(0x4420CC, 0x0280F8, (DWORD)&key_check_nav_map);
    //0044214D | .E8 76800200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 1F S 4 0
    FuncReplace32(0x44214E, 0x028076, (DWORD)&key_check_drop_nuke_em);
    //00442540 | .E8 837C0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 20 D 4 0
    FuncReplace32(0x442541, 0x027C83, (DWORD)&key_check_electronic_diary);
    //004425C5 | .E8 FE7B0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 2D X 8 10
    FuncReplace32(0x4425C6, 0x027BFE, (DWORD)&key_check_match_target_speed);
    //004426D3 | .E8 F07A0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 23 H 4 0
    FuncReplace32(0x4426D4, 0x027AF0, (DWORD)&key_check_hotkey_list);
    //00442755 | .E8 6E7A0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 18 O 4 0
    FuncReplace32(0x442756, 0x027A6E, (DWORD)&key_check_settings_2);
    //004427B4 | .E8 0F7A0200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 11 W 4 0
    FuncReplace32(0x4427B5, 0x027A0F, (DWORD)&key_check_warp_shields);
    //004428EF | .E8 D4780200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 11 W 8 0
    FuncReplace32(0x4428F0, 0x0278D4, (DWORD)&key_check_target_nearest_hostile);
    //00442BB1 | .E8 12760200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 1F S 8 0
    FuncReplace32(0x442BB2, 0x027612, (DWORD)&key_check_target_nearest_friendly);
    //00442E75 | .E8 4E730200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), CB 8 0
    FuncReplace32(0x442E76, 0x02734E, (DWORD)&key_check_CB);
    //00442EAD | .E8 16730200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), CD 8 0
    FuncReplace32(0x442EAE, 0x027316, (DWORD)&key_check_CD);
    //00442EE8 | .E8 DB720200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), C8 8 0
    FuncReplace32(0x442EE9, 0x0272DB, (DWORD)&key_check_C8);
    //00442F20 | .E8 A3720200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), D0 8 0
    FuncReplace32(0x442F21, 0x0272A3, (DWORD)&key_check_D0);
    //00442F5B | .E8 68720200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 1A [{ 8 0
    FuncReplace32(0x442F5C, 0x027268, (DWORD)&key_check_speed_zero);
    //00442FC7 | .E8 FC710200   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD fl; [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 1B ]} 8 0
    FuncReplace32(0x442FC8, 0x0271FC, (DWORD)&key_check_speed_max);
    //0044FF59 | .E8 6AA20100   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag); [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 34 > 8 0
    FuncReplace32(0x44FF5A, 0x01A26A, (DWORD)&key_check_roll_right);
    //0044FFB7 | .E8 0CA20100   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag); [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 33 < 8 0
    FuncReplace32(0x44FFB8, 0x01A20C, (DWORD)&key_check_roll_left);
    //00450118 | .E8 ABA00100   CALL bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag); [DARK.bool CHECK_KEY_STATE(EAX DWORD scan_code, EDX, EBX DWORD flag), 39 space 8 0
    FuncReplace32(0x450119, 0x01A0AB, (DWORD)&key_check_fire_guns);

    //Function key checks F1 to F10 - set start position to VK_DIVIDE, which is one before VK_F1 
    MemWrite8(0x443042, 0x3A, VK_DIVIDE);
    //Function key selection F1 to F10
    //FuncReplace32(0x443049, 0x02717B, (DWORD)&check_key_state_sc_to_vk);

    //Choose Comm signal 1 to 4.
    FuncReplace32(0x4433A0, 0x026E24, (DWORD)&check_key_state_sc_to_vk);

    //Select stored target with number key, Check numbers 0-9.
    FuncReplace32(0x44388F, 0x026935, (DWORD)&check_key_state_sc_to_vk);
    //Store current target to number key. Check numbers 0-9 with shift key pressed.
    FuncReplace32(0x4438CF, 0x0268F5, (DWORD)&check_key_state_sc_to_vk);

    //Interactive Movie Choice Screen 
    MemWrite8(0x41B840, 0xD0, VK_DOWN);
    MemWrite8(0x41B845, 0xC8, VK_UP);
    MemWrite8(0x41BA41, 0x1C, VK_RETURN);

    //P.A.D. Screen
    MemWrite8(0x41D53B, 0x19, 'P');

    //GUI exit game
    MemWrite8(0x41D703, 0x2D, 'X');
    //GUI exit game choice No and Yes 
    MemWrite8(0x41D8B8, 0x31, 'N');
    MemWrite8(0x41D916, 0x15, 'Y');

    //Load Screen
    //New Game
    MemWrite8(0x422257, 0x01, VK_ESCAPE);
    //Load Selected
    MemWrite8(0x4225B1, 0x1C, VK_RETURN);

    //P.A.D. Load game confirmation.
    MemWrite8(0x435DB9, 0x1C, VK_RETURN);

    //Space exit game choice No
    MemWrite8(0x439911, 0x31, 'N');
    //Space exit game choice Yes
    MemWrite8(0x439934, 0x15, 'Y');

    //In function at 0x43B563 - mostly letters and numbers "BYTE RETURN_PRESSED_KEY_SCANCODE_1_TO_0x53()"
   //Used in Nav Point type selection box, Option Screen +- joystick deadzone, etc.
    FuncReplace32(0x43B5A3, 0x02EC21, (DWORD)&check_key_state_sc_to_vk_ignore_shift);

    //For Debug Screen??
    FuncReplace32(0x43B61C, 0x02EBA8, (DWORD)&check_key_state_sc_to_vk);

    //nav keys
    //Open text box for typing nav point selection.
    MemWrite8(0x45B818, 0x21, 'F');
    //zoom in '=+'
    MemWrite8(0x45B852, 0x0D, VK_OEM_PLUS);
    ///zoom in '+'
    MemWrite8(0x45B867, 0x4E, VK_ADD);
    ///zoom out '-_'
    MemWrite8(0x45B89C, 0x0C, VK_OEM_MINUS);
    //zoom out '-'
    MemWrite8(0x45B8B1, 0x4A, VK_SUBTRACT);
    //reset view rotation
    MemWrite8(0x45B8EB, 0x2E, 'C');
    //toggle view rotation
    MemWrite8(0x45B96F, 0x13, 'R');
    //toggle planets
    MemWrite8(0x45B98F, 0x02, '1');
    //toggle nave points
    MemWrite8(0x45B9AE, 0x03, '2');
    //toggle nav point numbers
    MemWrite8(0x45B9CD, 0x04, '3');
    //toggle space stations
    MemWrite8(0x45B9EC, 0x05, '4');
    //exit nav screen
    MemWrite8(0x45BA06, 0x01, VK_ESCAPE);

    //exit email??
    MemWrite8(0x45C353, 0x01, VK_ESCAPE);

    //diary exit, escape key check
    FuncReplace32(0x45CD7D, 0xD447, (DWORD)&check_key_state_sc_to_vk);

    //options screen exit
    MemWrite8(0x45D8E8, 0x01, VK_ESCAPE);

    //options screen exit2
    MemWrite8(0x45EAB5, 0x01, VK_ESCAPE);

    //hot key screen exit
    FuncReplace32(0x45F59A, 0xAC2A, (DWORD)&check_key_state_sc_to_vk);
    //hot key screen exit2
    FuncReplace32(0x45F8F1, 0xA8D3, (DWORD)&check_key_state_sc_to_vk);

    //movie keys
    //exit movie playback
    MemWrite8(0x469299, 0x01, VK_ESCAPE);
    //increase gamma '=+'
    MemWrite8(0x4692B5, 0x0D, VK_OEM_PLUS);
    //decrease gamma '-_
    MemWrite8(0x4692EE, 0x0C, VK_OEM_MINUS);

    //???
    FuncReplace32(0x469561, 0x0C63, (DWORD)&check_key_state_sc_to_vk);

    //fade out any key exit
    FuncReplace32(0x46963D, 0x0B87, (DWORD)&check_key_state_sc_to_vk);
    //fade in any key exit
    FuncReplace32(0x469748, 0x0A7C, (DWORD)&check_key_state_sc_to_vk);

    //credits key checks
    FuncReplace32(0x469A8B, 0x0739, (DWORD)&check_key_state_sc_to_vk);
    FuncReplace32(0x469AE2, 0x06E2, (DWORD)&check_key_state_sc_to_vk);
    FuncReplace32(0x469C24, 0x05A0, (DWORD)&check_key_state_sc_to_vk);
    FuncReplace32(0x469D69, 0x045B, (DWORD)&check_key_state_sc_to_vk);
    FuncReplace32(0x469E62, 0x0362, (DWORD)&check_key_state_sc_to_vk);
}
