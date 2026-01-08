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

#pragma once


#ifdef WAIL32_EXPORTS
#define WAIL32_REROUTE __declspec(dllexport) 
#else
#define WAIL32_REROUTE __declspec(dllimport) 
#endif

typedef DWORD(__stdcall* P_FUNC_STD_00)(void);
typedef DWORD(__stdcall* P_FUNC_STD_01)(DWORD);
typedef DWORD(__stdcall* P_FUNC_STD_02)(DWORD, DWORD);
typedef DWORD(__stdcall* P_FUNC_STD_03)(DWORD, DWORD, DWORD);
typedef DWORD(__stdcall* P_FUNC_STD_04)(DWORD, DWORD, DWORD, DWORD);
typedef DWORD(__stdcall* P_FUNC_STD_05)(DWORD, DWORD, DWORD, DWORD, DWORD);


extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_active_sequence_count(DWORD); //_AIL_active_sequence_count@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_allocate_sample_handle(DWORD); //_AIL_allocate_sample_handle@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_allocate_sequence_handle(DWORD); //_AIL_allocate_sequence_handle@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_controller_value(DWORD, DWORD, DWORD); //_AIL_controller_value@12
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_end_sample(DWORD); //_AIL_end_sample@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_init_sample(DWORD); //_AIL_init_sample@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_init_sequence(DWORD, DWORD, DWORD); //_AIL_init_sequence@12
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_load_sample_buffer(DWORD, DWORD, DWORD, DWORD); //_AIL_load_sample_buffer@16
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_midiOutClose(DWORD); //_AIL_midiOutClose@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_midiOutOpen(DWORD, DWORD, DWORD); //_AIL_midiOutOpen@12
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_minimum_sample_buffer_size(DWORD, DWORD, DWORD); //_AIL_minimum_sample_buffer_size@12
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_register_EOB_callback(DWORD, DWORD); //_AIL_register_EOB_callback@8
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_register_EOS_callback(DWORD, DWORD); //_AIL_register_EOS_callback@8
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_register_timer(DWORD); //_AIL_register_timer@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_release_timer_handle(DWORD); //_AIL_release_timer_handle@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_resume_sample(DWORD); //_AIL_resume_sample@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_resume_sequence(DWORD); //_AIL_resume_sequence@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_sample_buffer_ready(DWORD); //_AIL_sample_buffer_ready@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_sample_status(DWORD); //_AIL_sample_status@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_sample_volume(DWORD); //_AIL_sample_volume@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_send_channel_voice_message(DWORD, DWORD, DWORD, DWORD, DWORD); //_AIL_send_channel_voice_message@20
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_set_preference(DWORD, DWORD); //_AIL_set_preference@8
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_set_sample_address(DWORD, DWORD, DWORD); //_AIL_set_sample_address@12
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_set_sample_loop_count(DWORD, DWORD); //_AIL_set_sample_loop_count@8
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_set_sample_pan(DWORD, DWORD); //_AIL_set_sample_pan@8
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_set_sample_playback_rate(DWORD, DWORD); //_AIL_set_sample_playback_rate@8
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_set_sample_type(DWORD, DWORD, DWORD); //_AIL_set_sample_type@12
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_set_sample_volume(DWORD, DWORD); //_AIL_set_sample_volume@8
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_set_sequence_loop_count(DWORD, DWORD); //_AIL_set_sequence_loop_count@8
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_set_sequence_volume(DWORD, DWORD, DWORD); //_AIL_set_sequence_volume@12
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_set_timer_frequency(DWORD, DWORD); //_AIL_set_timer_frequency@8
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_shutdown(void); //_AIL_shutdown@0
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_start_sample(DWORD); //_AIL_start_sample@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_start_sequence(DWORD); //_AIL_start_sequence@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_start_timer(DWORD); //_AIL_start_timer@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_startup(void); //_AIL_startup@0
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_stop_sample(DWORD); //_AIL_stop_sample@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_stop_sequence(DWORD); //_AIL_stop_sequence@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_stop_timer(DWORD); //_AIL_stop_timer@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_waveOutClose(DWORD); //_AIL_waveOutClose@4
extern "C" WAIL32_REROUTE  DWORD __stdcall AIL_waveOutOpen(DWORD, DWORD, DWORD, DWORD); //_AIL_waveOutOpen@16
extern "C" WAIL32_REROUTE  DWORD __stdcall FILE_read(DWORD, DWORD); //_FILE_read@8
extern "C" WAIL32_REROUTE  DWORD __stdcall FILE_size(DWORD); //_FILE_size@4
extern "C" WAIL32_REROUTE  DWORD __stdcall FILE_write(DWORD, DWORD, DWORD); //_FILE_write@12

