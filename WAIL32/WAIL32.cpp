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

#include "WAIL32.h"

HMODULE hModule_mss32 = LoadLibraryA("MSS32.DLL");


#ifdef _DEBUG
static void func_check() {
	DWORD(__stdcall * pAIL_active_sequence_count)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_allocate_sample_handle)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_allocate_sequence_handle)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_controller_value)(DWORD, DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_end_sample)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_init_sample)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_init_sequence)(DWORD, DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_load_sample_buffer)(DWORD, DWORD, DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_midiOutClose)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_midiOutOpen)(DWORD, DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_minimum_sample_buffer_size)(DWORD, DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_register_EOB_callback)(DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_register_EOS_callback)(DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_register_timer)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_release_timer_handle)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_resume_sample)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_resume_sequence)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_sample_buffer_ready)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_sample_status)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_sample_volume)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_send_channel_voice_message)(DWORD, DWORD, DWORD, DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_set_preference)(DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_set_sample_address)(DWORD, DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_set_sample_loop_count)(DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_set_sample_pan)(DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_set_sample_playback_rate)(DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_set_sample_type)(DWORD, DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_set_sample_volume)(DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_set_sequence_loop_count)(DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_set_sequence_volume)(DWORD, DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_set_timer_frequency)(DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pAIL_shutdown)(void) = nullptr;
	DWORD(__stdcall * pAIL_start_sample)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_start_sequence)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_start_timer)(DWORD) = nullptr;
	//DWORD(__stdcall* pAIL_startup)(void) = nullptr;
	DWORD(__stdcall * pAIL_stop_sample)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_stop_sequence)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_stop_timer)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_waveOutClose)(DWORD) = nullptr;
	DWORD(__stdcall * pAIL_waveOutOpen)(DWORD, DWORD, DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pFILE_read)(DWORD, DWORD) = nullptr;
	DWORD(__stdcall * pFILE_size)(DWORD) = nullptr;
	DWORD(__stdcall * pFILE_write)(DWORD, DWORD, DWORD) = nullptr;



	pAIL_active_sequence_count = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_active_sequence_count@4");
	if (!pAIL_active_sequence_count)
		MessageBox(nullptr, L"pAIL_active_sequence_count", L"Failed", 0);
	pAIL_allocate_sample_handle = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_allocate_sample_handle@4");
	if (!pAIL_allocate_sample_handle)
		MessageBox(nullptr, L"pAIL_allocate_sample_handle", L"Failed", 0);
	pAIL_allocate_sequence_handle = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_allocate_sequence_handle@4");
	if (!pAIL_allocate_sequence_handle)
		MessageBox(nullptr, L"pAIL_allocate_sequence_handle", L"Failed", 0);
	pAIL_controller_value = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_controller_value@12");
	if (!pAIL_controller_value)
		MessageBox(nullptr, L"pAIL_controller_value", L"Failed", 0);
	pAIL_end_sample = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_end_sample@4");
	if (!pAIL_end_sample)
		MessageBox(nullptr, L"pAIL_end_sample", L"Failed", 0);
	pAIL_init_sample = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_init_sample@4");
	if (!pAIL_init_sample)
		MessageBox(nullptr, L"pAIL_init_sample", L"Failed", 0);
	pAIL_init_sequence = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_init_sequence@12");
	if (!pAIL_init_sequence)
		MessageBox(nullptr, L"pAIL_init_sequence", L"Failed", 0);
	pAIL_load_sample_buffer = (P_FUNC_STD_04)GetProcAddress(hModule_mss32, "_AIL_load_sample_buffer@16");
	if (!pAIL_load_sample_buffer)
		MessageBox(nullptr, L"pAIL_load_sample_buffer", L"Failed", 0);
	pAIL_midiOutClose = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_midiOutClose@4");
	if (!pAIL_midiOutClose)
		MessageBox(nullptr, L"pAIL_midiOutClose", L"Failed", 0);
	pAIL_midiOutOpen = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_midiOutOpen@12");
	if (!pAIL_midiOutOpen)
		MessageBox(nullptr, L"pAIL_midiOutOpen", L"Failed", 0);
	pAIL_minimum_sample_buffer_size = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_minimum_sample_buffer_size@12");
	if (!pAIL_minimum_sample_buffer_size)
		MessageBox(nullptr, L"pAIL_minimum_sample_buffer_size", L"Failed", 0);
	pAIL_register_EOB_callback = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_register_EOB_callback@8");
	if (!pAIL_register_EOB_callback)
		MessageBox(nullptr, L"pAIL_register_EOB_callback", L"Failed", 0);
	pAIL_register_EOS_callback = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_register_EOS_callback@8");
	if (!pAIL_register_EOS_callback)
		MessageBox(nullptr, L"pAIL_register_EOS_callback", L"Failed", 0);
	pAIL_register_timer = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_register_timer@4");
	if (!pAIL_register_timer)
		MessageBox(nullptr, L"pAIL_register_timer", L"Failed", 0);
	pAIL_release_timer_handle = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_release_timer_handle@4");
	if (!pAIL_release_timer_handle)
		MessageBox(nullptr, L"pAIL_release_timer_handle", L"Failed", 0);
	pAIL_resume_sample = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_resume_sample@4");
	if (!pAIL_resume_sample)
		MessageBox(nullptr, L"pAIL_resume_sample", L"Failed", 0);
	pAIL_resume_sequence = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_resume_sequence@4");
	if (!pAIL_resume_sequence)
		MessageBox(nullptr, L"pAIL_resume_sequence", L"Failed", 0);
	pAIL_sample_buffer_ready = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_sample_buffer_ready@4");
	if (!pAIL_sample_buffer_ready)
		MessageBox(nullptr, L"pAIL_sample_buffer_ready", L"Failed", 0);
	pAIL_sample_status = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_sample_status@4");
	if (!pAIL_sample_status)
		MessageBox(nullptr, L"pAIL_sample_status", L"Failed", 0);
	pAIL_sample_volume = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_sample_volume@4");
	if (!pAIL_sample_volume)
		MessageBox(nullptr, L"pAIL_sample_volume", L"Failed", 0);
	pAIL_send_channel_voice_message = (P_FUNC_STD_05)GetProcAddress(hModule_mss32, "_AIL_send_channel_voice_message@20");
	if (!pAIL_send_channel_voice_message)
		MessageBox(nullptr, L"pAIL_send_channel_voice_message", L"Failed", 0);
	pAIL_set_preference = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_preference@8");
	if (!pAIL_set_preference)
		MessageBox(nullptr, L"pAIL_set_preference", L"Failed", 0);
	pAIL_set_sample_address = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_set_sample_address@12");
	if (!pAIL_set_sample_address)
		MessageBox(nullptr, L"pAIL_set_sample_address", L"Failed", 0);
	pAIL_set_sample_loop_count = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_sample_loop_count@8");
	if (!pAIL_set_sample_loop_count)
		MessageBox(nullptr, L"pAIL_set_sample_loop_count", L"Failed", 0);
	pAIL_set_sample_pan = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_sample_pan@8");
	if (!pAIL_set_sample_pan)
		MessageBox(nullptr, L"pAIL_set_sample_pan", L"Failed", 0);
	pAIL_set_sample_playback_rate = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_sample_playback_rate@8");
	if (!pAIL_set_sample_playback_rate)
		MessageBox(nullptr, L"pAIL_set_sample_playback_rate", L"Failed", 0);
	pAIL_set_sample_type = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_set_sample_type@12");
	if (!pAIL_set_sample_type)
		MessageBox(nullptr, L"pAIL_set_sample_type", L"Failed", 0);
	pAIL_set_sample_volume = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_sample_volume@8");
	if (!pAIL_set_sample_volume)
		MessageBox(nullptr, L"pAIL_set_sample_volume", L"Failed", 0);
	pAIL_set_sequence_loop_count = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_sequence_loop_count@8");
	if (!pAIL_set_sequence_loop_count)
		MessageBox(nullptr, L"pAIL_set_sequence_loop_count", L"Failed", 0);
	pAIL_set_sequence_volume = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_set_sequence_volume@12");
	if (!pAIL_set_sequence_volume)
		MessageBox(nullptr, L"pAIL_set_sequence_volume", L"Failed", 0);
	pAIL_set_timer_frequency = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_timer_frequency@8");
	if (!pAIL_set_timer_frequency)
		MessageBox(nullptr, L"pAIL_set_timer_frequency", L"Failed", 0);
	pAIL_shutdown = (P_FUNC_STD_00)GetProcAddress(hModule_mss32, "_AIL_shutdown@0");
	if (!pAIL_shutdown)
		MessageBox(nullptr, L"pAIL_shutdown", L"Failed", 0);
	pAIL_start_sample = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_start_sample@4");
	if (!pAIL_start_sample)
		MessageBox(nullptr, L"pAIL_start_sample", L"Failed", 0);
	pAIL_start_sequence = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_start_sequence@4");
	if (!pAIL_start_sequence)
		MessageBox(nullptr, L"pAIL_start_sequence", L"Failed", 0);
	pAIL_start_timer = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_start_timer@4");
	if (!pAIL_start_timer)
		MessageBox(nullptr, L"pAIL_start_timer", L"Failed", 0);
	//DWORD(__stdcall* pAIL_startup)(void) = nullptr;
	pAIL_stop_sample = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_stop_sample@4");
	if (!pAIL_stop_sample)
		MessageBox(nullptr, L"pAIL_stop_sample", L"Failed", 0);
	pAIL_stop_sequence = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_stop_sequence@4");
	if (!pAIL_stop_sequence)
		MessageBox(nullptr, L"pAIL_stop_sequence", L"Failed", 0);
	pAIL_stop_timer = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_stop_timer@4");
	if (!pAIL_stop_timer)
		MessageBox(nullptr, L"pAIL_stop_timer", L"Failed", 0);
	pAIL_waveOutClose = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_waveOutClose@4");
	if (!pAIL_waveOutClose)
		MessageBox(nullptr, L"pAIL_waveOutClose", L"Failed", 0);
	pAIL_waveOutOpen = (P_FUNC_STD_04)GetProcAddress(hModule_mss32, "_AIL_waveOutOpen@16");
	if (!pAIL_waveOutOpen)
		MessageBox(nullptr, L"pAIL_waveOutOpen", L"Failed", 0);
	pFILE_read = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_file_read@8");
	if (!pFILE_read)
		MessageBox(nullptr, L"pFILE_read", L"Failed", 0);
	pFILE_size = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_file_size@4");
	if (!pFILE_size)
		MessageBox(nullptr, L"pFILE_size", L"Failed", 0);
	pFILE_write = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_file_write@12");
	if (!pFILE_write)
		MessageBox(nullptr, L"pFILE_write", L"Failed", 0);
}
#endif // DEBUG

//____________________________________________________
DWORD __stdcall AIL_active_sequence_count(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_active_sequence_count@4");
	if (pfunc)
		return pfunc(Arg_1);

	return -1;
}


//_____________________________________________________
DWORD __stdcall AIL_allocate_sample_handle(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_allocate_sample_handle@4");
	if (pfunc)
		return pfunc(Arg_1);

	return -1;
}


//_______________________________________________________
DWORD __stdcall AIL_allocate_sequence_handle(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_allocate_sequence_handle@4");
	if (pfunc)
		return pfunc(Arg_1);

	return -1;
}


//_________________________________________________________________________
DWORD __stdcall AIL_controller_value(DWORD Arg_1, DWORD Arg_2, DWORD Arg_3) {

	static P_FUNC_STD_03 pfunc = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_controller_value@12");
	if (pfunc)
		return pfunc(Arg_1, Arg_2, Arg_3);
	
	return -1;
}


//_________________________________________
DWORD __stdcall AIL_end_sample(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_end_sample@4");
	if (pfunc)
		return pfunc(Arg_1);

	return -1;
}


//__________________________________________
DWORD __stdcall AIL_init_sample(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_init_sample@4");
	if (pfunc)
		return pfunc(Arg_1);
	
	return -1;
}


//______________________________________________________________________
DWORD __stdcall AIL_init_sequence(DWORD Arg_1, DWORD Arg_2, DWORD Arg_3) {

	static P_FUNC_STD_03 pfunc = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_init_sequence@12");
	if (pfunc)
		return pfunc(Arg_1, Arg_2, Arg_3);
	
	return -1;
}


//________________________________________________________________________________________
DWORD __stdcall AIL_load_sample_buffer(DWORD Arg_1, DWORD Arg_2, DWORD Arg_3, DWORD Arg_4) {

	static P_FUNC_STD_04 pfunc = (P_FUNC_STD_04)GetProcAddress(hModule_mss32, "_AIL_load_sample_buffer@16");
	if (pfunc)
		return pfunc(Arg_1, Arg_2, Arg_3, Arg_4);

	return -1;
}


//___________________________________________
DWORD __stdcall AIL_midiOutClose(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_midiOutClose@4");
	if (pfunc)
		return pfunc(Arg_1);
	
	return -1;
}


//____________________________________________________________________
DWORD __stdcall AIL_midiOutOpen(DWORD Arg_1, DWORD Arg_2, DWORD Arg_3) {

	static P_FUNC_STD_03 pfunc = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_midiOutOpen@12");
	if (pfunc)
		return pfunc(Arg_1, Arg_2, Arg_3);
	
	return -1;
}


//___________________________________________________________________________________
DWORD __stdcall AIL_minimum_sample_buffer_size(DWORD Arg_1, DWORD Arg_2, DWORD Arg_3) {

	static P_FUNC_STD_03 pfunc = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_minimum_sample_buffer_size@12");
	if (pfunc)
		return pfunc(Arg_1, Arg_2, Arg_3);
	
	return -1;
}


//_________________________________________________________________
DWORD __stdcall AIL_register_EOB_callback(DWORD Arg_1, DWORD Arg_2) {

	static P_FUNC_STD_02 pfunc = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_register_EOB_callback@8");
	if (pfunc)
		return pfunc(Arg_1, Arg_2);

	return -1;
}


//_________________________________________________________________
DWORD __stdcall AIL_register_EOS_callback(DWORD Arg_1, DWORD Arg_2) {

	static P_FUNC_STD_02 pfunc = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_register_EOS_callback@8");
	if (pfunc)
		return pfunc(Arg_1, Arg_2);

	return -1;
}


//_____________________________________________
DWORD __stdcall AIL_register_timer(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_register_timer@4");
	if (pfunc)
		return pfunc(Arg_1);

	return -1;
}


//___________________________________________________
DWORD __stdcall AIL_release_timer_handle(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_release_timer_handle@4");
	if (pfunc)
		return pfunc(Arg_1);

	return -1;
}


//____________________________________________
DWORD __stdcall AIL_resume_sample(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_resume_sample@4");
	if (pfunc)
		return pfunc(Arg_1);
	
	return -1;
}


//______________________________________________
DWORD __stdcall AIL_resume_sequence(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_resume_sequence@4");
	if (pfunc)
		return pfunc(Arg_1);

	return -1;
}


//__________________________________________________
DWORD __stdcall AIL_sample_buffer_ready(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_sample_buffer_ready@4");
	if (pfunc)
		return pfunc(Arg_1);
	
	return -1;
}


//____________________________________________
DWORD __stdcall AIL_sample_status(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_sample_status@4");
	if (pfunc)
		return pfunc(Arg_1);

	return -1;
}


//____________________________________________
DWORD __stdcall AIL_sample_volume(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_sample_volume@4");
	if (pfunc)
		return pfunc(Arg_1);

	return -1;
}


//_____________________________________________________________________________________________________________
DWORD __stdcall AIL_send_channel_voice_message(DWORD Arg_1, DWORD Arg_2, DWORD Arg_3, DWORD Arg_4, DWORD Arg_5) {

	static P_FUNC_STD_05 pfunc = (P_FUNC_STD_05)GetProcAddress(hModule_mss32, "_AIL_send_channel_voice_message@20");
	if (pfunc)
		return pfunc(Arg_1, Arg_2, Arg_3, Arg_4, Arg_5);

	return -1;
}


//__________________________________________________________
DWORD __stdcall AIL_set_preference(DWORD Arg_1, DWORD Arg_2) {

	static P_FUNC_STD_02 pfunc = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_preference@8");
	if (pfunc)
		return pfunc(Arg_1, Arg_2);

	return -1;
}


//___________________________________________________________________________
DWORD __stdcall AIL_set_sample_address(DWORD Arg_1, DWORD Arg_2, DWORD Arg_3) {

	static P_FUNC_STD_03 pfunc = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_set_sample_address@12");
	if (pfunc)
		return pfunc(Arg_1, Arg_2, Arg_3);

	return -1;
}


//_________________________________________________________________
DWORD __stdcall AIL_set_sample_loop_count(DWORD Arg_1, DWORD Arg_2) {

	static P_FUNC_STD_02 pfunc = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_sample_loop_count@8");
	if (pfunc)
		return pfunc(Arg_1, Arg_2);

	return -1;
}


//__________________________________________________________
DWORD __stdcall AIL_set_sample_pan(DWORD Arg_1, DWORD Arg_2) {

	static P_FUNC_STD_02 pfunc = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_sample_pan@8");
	if (pfunc)
		return pfunc(Arg_1, Arg_2);

	return -1;
}


//____________________________________________________________________
DWORD __stdcall AIL_set_sample_playback_rate(DWORD Arg_1, DWORD Arg_2) {

	static P_FUNC_STD_02 pfunc = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_sample_playback_rate@8");
	if (pfunc)
		return pfunc(Arg_1, Arg_2);

	return -1;
}


//________________________________________________________________________
DWORD __stdcall AIL_set_sample_type(DWORD Arg_1, DWORD Arg_2, DWORD Arg_3) {

	static P_FUNC_STD_03 pfunc = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_set_sample_type@12");
	if (pfunc)
		return pfunc(Arg_1, Arg_2, Arg_3);

	return -1;
}


//_____________________________________________________________
DWORD __stdcall AIL_set_sample_volume(DWORD Arg_1, DWORD Arg_2) {

	static P_FUNC_STD_02 pfunc = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_sample_volume@8");
	if (pfunc)
		return pfunc(Arg_1, Arg_2);

	return -1;
}


//___________________________________________________________________
DWORD __stdcall AIL_set_sequence_loop_count(DWORD Arg_1, DWORD Arg_2) {

	static P_FUNC_STD_02 pfunc = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_sequence_loop_count@8");
	if (pfunc)
		return pfunc(Arg_1, Arg_2);

	return -1;
}


//____________________________________________________________________________
DWORD __stdcall AIL_set_sequence_volume(DWORD Arg_1, DWORD Arg_2, DWORD Arg_3) {

	static P_FUNC_STD_03 pfunc = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_set_sequence_volume@12");
	if (pfunc)
		return pfunc(Arg_1, Arg_2, Arg_3);

	return -1;
}


//_______________________________________________________________
DWORD __stdcall AIL_set_timer_frequency(DWORD Arg_1, DWORD Arg_2) {

	static P_FUNC_STD_02 pfunc = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_set_timer_frequency@8");
	if (pfunc)
		return pfunc(Arg_1, Arg_2);

	return -1;
}


//________________________________
DWORD __stdcall AIL_shutdown(void) {

	static P_FUNC_STD_00 pfunc = (P_FUNC_STD_00)GetProcAddress(hModule_mss32, "_AIL_shutdown@0");
	if (pfunc)
		return pfunc();

	return -1;
}


//___________________________________________
DWORD __stdcall AIL_start_sample(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_start_sample@4");
	if (pfunc)
		return pfunc(Arg_1);

	return -1;
}


//_____________________________________________
DWORD __stdcall AIL_start_sequence(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_start_sequence@4");
	if (pfunc)
		return pfunc(Arg_1);

	return -1;
}


//__________________________________________
DWORD __stdcall AIL_start_timer(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_start_timer@4");
	if (pfunc)
		return  pfunc(Arg_1);

	return -1;
}


//_______________________________
DWORD __stdcall AIL_startup(void) {
	
	static P_FUNC_STD_00 pfunc = (P_FUNC_STD_00)GetProcAddress(hModule_mss32, "_AIL_startup@0");
	if (pfunc) {
#ifdef _DEBUG
		func_check();
		MessageBox(nullptr, L"AIL_startup", L"Success", 0);
#endif
		return pfunc();
	}
	return -1;
}


//__________________________________________
DWORD __stdcall AIL_stop_sample(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_stop_sample@4");
	if (pfunc)
		return pfunc(Arg_1);

	return -1;
}


//____________________________________________
DWORD __stdcall AIL_stop_sequence(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_stop_sequence@4");
	if (pfunc)
		return pfunc(Arg_1);

	MessageBox(nullptr, L"AIL_stop_sequence", L"Failed", 0);
	return -1;
}


//_________________________________________
DWORD __stdcall AIL_stop_timer(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_stop_timer@4");
	if (pfunc)
		return pfunc(Arg_1);

	MessageBox(nullptr, L"AIL_stop_timer", L"Failed", 0);
	return -1;
}


//___________________________________________
DWORD __stdcall AIL_waveOutClose(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_waveOutClose@4");
	if (pfunc)
		return pfunc(Arg_1);

	MessageBox(nullptr, L"AIL_waveOutClose", L"Failed", 0);
	return -1;
}


//_________________________________________________________________________________
DWORD __stdcall AIL_waveOutOpen(DWORD Arg_1, DWORD Arg_2, DWORD Arg_3, DWORD Arg_4) {

	static P_FUNC_STD_04 pfunc = (P_FUNC_STD_04)GetProcAddress(hModule_mss32, "_AIL_waveOutOpen@16");
	if (pfunc)
		return pfunc(Arg_1, Arg_2, Arg_3, Arg_4);

	MessageBox(nullptr, L"_AIL_waveOutOpen@16", L"Failed", 0);
	return -1;
}


//_________________________________________________
DWORD __stdcall FILE_read(DWORD Arg_1, DWORD Arg_2) {

	static P_FUNC_STD_02 pfunc = (P_FUNC_STD_02)GetProcAddress(hModule_mss32, "_AIL_file_read@8");
	if (pfunc)
		return pfunc(Arg_1, Arg_2);

	MessageBox(nullptr, L"FILE_read", L"Failed", 0);
	return -1;
}


//____________________________________
DWORD __stdcall FILE_size(DWORD Arg_1) {

	static P_FUNC_STD_01 pfunc = (P_FUNC_STD_01)GetProcAddress(hModule_mss32, "_AIL_file_size@4");
	if (pfunc)
		return pfunc(Arg_1);

	MessageBox(nullptr, L"FILE_size", L"Failed", 0);
	return -1;
}


//_______________________________________________________________
DWORD __stdcall FILE_write(DWORD Arg_1, DWORD Arg_2, DWORD Arg_3) {

	static P_FUNC_STD_03 pfunc = (P_FUNC_STD_03)GetProcAddress(hModule_mss32, "_AIL_file_write@12");
	if (pfunc)
		return pfunc(Arg_1, Arg_2, Arg_3);

	MessageBox(nullptr, L"FILE_write", L"Failed", 0);
	return -1;
}
