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

extern IMAGE_BUFFER* p_p2_main_image_buffer;
extern IMAGE_BUFFER_RECT* p_p2_main_image_buffer_rect;

extern BYTE* p_p2_controller_flags;//0 = mouse, 1 = joy without throttle, 2 = joy with throttle, etc.

extern BYTE* p2_keyboard_state_main;
extern BYTE* p2_keyboard_state_last;


extern void* p_p2_check_key_state;
extern void* p_p2_space_main;
extern void* p_p2_options_screen;
extern void* p_p2_navigation_screen;
extern void* p_p2_diary_screen;
extern void* p_p2_hotkeys_screen;
extern void* p_p2_email_screen;

#define SPACE_STRUCT_MOUSE_X                0x140 
#define SPACE_STRUCT_MOUSE_Y                0x142

#define SPACE_STRUCT_MOUSE_X_CENTRED        0x148 
#define SPACE_STRUCT_MOUSE_Y_CENTRED        0x14A 
#define SPACE_STRUCT_MOUSE_BUTTON01         0x14C 
#define SPACE_STRUCT_MOUSE_BUTTON02         0x14E
#define SPACE_STRUCT_MOUSE_BUTTON_PRESSED   0x152

#define SPACE_STRUCT_JOY_ON				0x27918 
#define SPACE_STRUCT_JOY_THROTTLE_ON	0x2791C 

#define SPACE_STRUCT_JOY_FLAGS			0x27920 
#define SPACE_STRUCT_JOY_DEAD_ZONE		0x27924 

#define SPACE_STRUCT_JOY_X_CEN			0x27930 
#define SPACE_STRUCT_JOY_Y_CEN			0x27934 

#define SPACE_STRUCT_JOY_X_MIN			0x27938 
#define SPACE_STRUCT_JOY_X_MAX			0x2793C 

#define SPACE_STRUCT_JOY_Y_MIN			0x27940 
#define SPACE_STRUCT_JOY_Y_MAX			0x27944 

#define SPACE_STRUCT_JOY_T_DEAD_ZONE	0x2796C 
#define SPACE_STRUCT_JOY_T_CEN			0x27974 
#define SPACE_STRUCT_JOY_T_MIN			0x27978
#define SPACE_STRUCT_JOY_T_MAX			0x2797C 



#define SPACE_OBJECT_STRUCT_SIZE  928 
#define SPACE_PC_SHIP_STRUCT_FT_OFFSET  0x1298 
#define SPACE_PC_SHIP_STRUCT_FX_OFFSET  0x12B0 
#define SPACE_PC_SHIP_STRUCT_FY_OFFSET  0x12A8 
#define SPACE_PC_SHIP_STRUCT_FR_OFFSET  0x12AC 
#define SPACE_PC_SHIP_STRUCT_AFTERBURNER_OFFSET  0x12D0

#define SPACE_PC_SHIP_STRUCT_SHIP_TYPE_VARS  0x11E6 
#define SPACE_PC_SHIP_STRUCT_CURRENT_TARGET_VARS  0x1216 

#define SPACE_OBJECT_STRUCT_CURRENT_SPEED  0x12B4 

extern void* p_p2_space_object_type_struct_list;
#define SPACE_OBJECT_TYPE_STRUCT_SIZE  0x48 

extern void* p_p2_space_struct;

extern DWORD* p_p2_space_struct_number_of_objects;//I think this this is the number of objects in space_struct.


extern float* p_p2_space_struct_joy_fx;
extern float* p_p2_space_struct_joy_fy;
extern float* p_p2_space_struct_joy_ft;

extern float* p_p2_space_struct_joy_f_dead_zone;

extern BYTE* p_p2_space_struct_joy_buttons;

extern WORD* p_p2_space_struct_exit_flag;

extern BYTE* p_p2_y_axis_orientation;

void p2_update_joystick_data(void* p_space_struct);
void p2_proccess_joystick_data(void* p_space_struct);
void p2_update_and_proccess_joystick_data();

extern void* p_p2_update_joystick_data;
extern void* p_p2_proccess_joystick_data;

extern void** pp_p2_wail32_sample_handle;
extern void** pp_p2_wail32_midi_sequence_handle;

bool p2_check_key_state(BYTE scan_code, BYTE flag_1, BYTE flag_2);

DWORD p2_music_start(DWORD flag_1, DWORD flag_2, DWORD flag_3);
extern DWORD(*p2_music_stop)();

extern void(*p2_error_message_box)(const char* msg);

extern void(*p2_wail32_sample_suspend)();
extern void(*p2_wail32_sample_resume)();

extern void(*p2_wail32_midi_sequence_suspend)();
extern void(*p2_wail32_midi_sequence_resume)();

extern void(*p2_movie_active)(LONG* p_active_flag);
extern BOOL(*p2_movie_play)(const char* path, BOOL clear_on_start, BOOL fade_out);

extern void(*p2_update_keyboard_state_ddinput)();

