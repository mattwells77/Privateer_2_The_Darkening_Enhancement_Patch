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

#pragma once

#define GUI_WIDTH 640
#define GUI_HEIGHT 480

struct IMAGE_BUFFER {
    BYTE* buff;
    LONG right;
    LONG bottom;
    LONG left;
    LONG top;
};

struct IMAGE_BUFFER_RECT {
    IMAGE_BUFFER* img_buff;
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
};

void DARK_Setup();

extern char* p_p2_szAppName;
//extern HINSTANCE* pp_dark_hinst;

extern HWND* p_p2_hWinMain;

extern HANDLE* p_p2_hThread_main;

extern HCURSOR* p_p2_cursor;

extern BOOL* p_p2_is_app_active;
extern BOOL* p_p2_is_app_movie_suspended;

extern DWORD* p_p2_game_exit_flag;
extern LONG** pp_p2_movie_active_flag;

extern BYTE* p_p2_movie_flag_unk;

extern WORD* p_p2_current_CD_loaded;

extern void* p_p2_check_key_state;
extern void* p_p2_space_main;
extern void* p_p2_options_screen;

extern void** pp_p2_wail32_sample_handle;
extern void** pp_p2_wail32_midi_sequence_handle;

bool p2_check_key_state(DWORD scan_code, DWORD flag_1, DWORD flag_2);

DWORD p2_music_start(DWORD flag_1, DWORD flag_2, DWORD flag_3);
extern DWORD(*p2_music_stop)();

extern void(*p2_error_message_box)(const char* msg);

extern void(*p2_wail32_sample_suspend)();
extern void(*p2_wail32_sample_resume)();

extern void(*p2_wail32_midi_sequence_suspend)();
extern void(*p2_wail32_midi_sequence_resume)();

extern void(*p2_movie_active)(LONG* p_active_flag);
extern BOOL(*p2_movie_play)(const char* path, BOOL clear_on_start, BOOL fade_out);

