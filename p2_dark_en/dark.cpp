/*
The MIT License (MIT)
Copyright © 2025 Matt Wells

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
#include "dark.h"

char* p_p2_szAppName = nullptr;
//HINSTANCE* pp_dark_hinst = nullptr;

HWND* p_p2_hWinMain = nullptr;

HANDLE* p_p2_hThread_main = nullptr;

HCURSOR* p_p2_cursor = nullptr;

BOOL* p_p2_is_app_active = nullptr;
BOOL* p_p2_is_app_movie_suspended = nullptr;

BYTE* p_p2_movie_flag_unk = nullptr;

DWORD* p_p2_game_exit_flag = nullptr;
LONG** pp_p2_movie_active_flag = nullptr;

WORD* p_p2_current_CD_loaded = nullptr;


void** pp_p2_wail32_sample_handle = nullptr;
void** pp_p2_wail32_midi_sequence_handle = nullptr;

void(*p2_error_message_box)(const char* msg) = nullptr;

void(*p2_wail32_sample_suspend)() = nullptr;
void(*p2_wail32_sample_resume)() = nullptr;

void(*p2_wail32_midi_sequence_suspend)() = nullptr;
void(*p2_wail32_midi_sequence_resume)() = nullptr;

void(*p2_movie_active)(LONG* p_active_flag) = nullptr;
BOOL(*p2_movie_play)(const char* path, BOOL clear_on_start, BOOL fade_out) = nullptr;

void* p_p2_check_key_state = nullptr;
void* p_p2_space_main = nullptr;
void* p_p2_options_screen = nullptr;


//__________________________________________________________________
bool p2_check_key_state(DWORD scan_code, DWORD flag_1, DWORD flag_2) {
	bool ret_val = false;
	__asm {
		mov ebx, flag_2
		mov edx, flag_1
		mov eax, scan_code
		call p_p2_check_key_state
		mov ret_val, al
	}
	return ret_val;
}


//_______________
void DARK_Setup() {
	p_p2_hWinMain = (HWND*)0x55D2C0;
	p_p2_szAppName = (char*)0x4A2B35;
	
	p_p2_hThread_main = (HANDLE*)0x55D5F4;
	
	p_p2_cursor = (HCURSOR*)0x55D2C8;

	p_p2_is_app_active = (BOOL*)0x55D620;
	p_p2_is_app_movie_suspended = (BOOL*)0x55D618;

	p_p2_game_exit_flag = (DWORD*)0x55D2EC;
	pp_p2_movie_active_flag = (LONG**)0x542664;

	p_p2_movie_flag_unk = (BYTE*)0x5426FC;

	p_p2_current_CD_loaded = (WORD*)0x52D314;

	pp_p2_wail32_sample_handle = (void**)0x4F01B4;
	pp_p2_wail32_midi_sequence_handle = (void**)0x4F01B8;


	p2_error_message_box = (void(*)(const char*))0x46B5A0;

	p2_wail32_sample_suspend = (void(*)())0x419E08;
	p2_wail32_sample_resume = (void(*)())0x419E48;

	p2_wail32_midi_sequence_suspend = (void(*)())0x419E08;
	p2_wail32_midi_sequence_resume = (void(*)())0x419E48;

	p2_movie_active = (void(*)(LONG*))0x474E64;
	p2_movie_play = (BOOL(*)(const char*, BOOL, BOOL))0x469194;

	p_p2_check_key_state = (void*)0x46A1C8;

	p_p2_space_main = (void*)0x438137;

	p_p2_options_screen = (void*)0x45CE5C;
}
