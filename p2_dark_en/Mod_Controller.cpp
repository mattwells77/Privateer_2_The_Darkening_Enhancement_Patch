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

#include "input.h"
#include "input_config.h"
#include "dark.h"
#include "memwrite.h"
#include "modifications.h"
#include "configTools.h"


ACTION_KEY action_key_original[4]{};


//_____________________________________
static void Action_Key_Original_Setup() {
	static bool run_once = 0;

	if (run_once)
		return;

	PROFILE_TYPE saved_pro_type = current_pro_type;

	current_pro_type = PROFILE_TYPE::GUI;
	action_key_original[0].Set_Action(P2_ACTIONS::Left_Click);
	action_key_original[1].Set_Action(P2_ACTIONS::Right_Click);
	action_key_original[2].Set_Action(P2_ACTIONS::GUI_PAD);
	action_key_original[3].Set_Action(P2_ACTIONS::Main_Load_Escape);
	current_pro_type = PROFILE_TYPE::Space;
	action_key_original[0].Set_Action(P2_ACTIONS::Fire_Guns);
	action_key_original[1].Set_Action(P2_ACTIONS::Joystick_Roll_Modifier);/////////////////////////////////
	action_key_original[2].Set_Action(P2_ACTIONS::Fire_Missile);
	action_key_original[3].Set_Action(P2_ACTIONS::Afterburner);
	current_pro_type = saved_pro_type;
	run_once = true;
}


//____________________________________
static void Original_Joystick_Update() {

	Action_Key_Original_Setup();

	p2_update_and_proccess_joystick_data();
	//Debug_Info("Original_Joystick_Update x %f, y %f, t %f, dz %f", *p_p2_space_struct_joy_fx, *p_p2_space_struct_joy_fy, *p_p2_space_struct_joy_ft, *p_p2_space_struct_joy_f_dead_zone);

	BYTE* pbuttons = p_p2_space_struct_joy_buttons;
	action_key_original[0].SetButton(*pbuttons & 0x1);
	action_key_original[1].SetButton((*pbuttons & 0x2) >> 1);
	action_key_original[2].SetButton((*pbuttons & 0x4) >> 2);
	action_key_original[3].SetButton((*pbuttons & 0x8) >> 3);
	*pbuttons = 0;

	p2_joy_axes.y = *p_p2_space_struct_joy_fy;

	if (*p_p2_controller_flags & 0x2)
		p2_joy_axes.t = *p_p2_space_struct_joy_ft;
	else
		p2_joy_axes.t = 0;

	if (p2_joy_axes.yaw_as_roll) {
		p2_joy_axes.r = (*p_p2_space_struct_joy_fx);
		p2_joy_axes.x = 0;
	}
	else {
		p2_joy_axes.x = (*p_p2_space_struct_joy_fx);
		p2_joy_axes.r = 0;
	}

	Update_Axis_Keys();
	Maintain_Axis_Limits();
}



//______________________
static void Joy_Update() {
	Check_Simulated_Key_For_Release();

	if (controller_enhancements_enabled)
		Joysticks.Update();
	else
		Original_Joystick_Update();

	Check_Mouse_Double_Click();
}


//_________________________________________________
static void __declspec(naked) joy_update_main(void) {

	__asm {
		push esi

		call Joy_Update

		pop esi

		call p2_update_keyboard_state_ddinput
		ret
	}
}


//_________________________________________________
static void Joy_GetCurrentState(BYTE* space_struct) {
	
	//if (controller_enhancements_enabled)
	//	Joysticks.Update();
	//else
	//	Original_Joystick_Update();
	
	//*p_p2_space_struct_joy_ft = (float)p2_joy_axes.t;

	void* p_pc_ship_struct = (void*)(space_struct + (*p_p2_space_struct_number_of_objects * SPACE_OBJECT_STRUCT_SIZE));

	*(float*)((BYTE*)p_pc_ship_struct + SPACE_PC_SHIP_STRUCT_FX_OFFSET) = -(float)p2_joy_axes.x;

	*(float*)((BYTE*)p_pc_ship_struct + SPACE_PC_SHIP_STRUCT_FR_OFFSET) = -(float)p2_joy_axes.r;

	*(float*)((BYTE*)p_pc_ship_struct + SPACE_PC_SHIP_STRUCT_FY_OFFSET) = (float)p2_joy_axes.y;
}


//_______________________________________________________
static void __declspec(naked) joy_get_current_state(void) {

	__asm {
		push ebp

		push eax
		call Joy_GetCurrentState
		add esp, 0x4

		pop ebp
		ret
	}
}


//___________________________________
static void GetCurrentThrottleState() {

	*p_p2_space_struct_joy_ft = (float)p2_joy_axes.t;
}


//____________________________________________________________
static void __declspec(naked) get_current_throttle_state(void) {

	__asm {
		push ebp

		call GetCurrentThrottleState

		pop ebp
		ret
	}
}


//______________________________________________________________________
static void __declspec(naked) options_screen_skip_joy_button_draws(void) {
	//don't draw buttons 12 to 15
	__asm {
		cmp ecx, 11
		jl exit_func
		cmp ecx, 16
		jge exit_func
		add ecx, 4
		add ebp, 32

		exit_func:

		add edi, 20
		add ebp, 8
		add ebx, 20
		inc ecx
		ret
	}
}


//_______________________________________________________________________
static void __declspec(naked) options_screen_skip_joy_button_checks(void) {
	//don't check buttons 12 to 15
	__asm {
		cmp edi, 11
		jl exit_func
		cmp edi, 16
		jge exit_func

		add edi, 4

		exit_func:
		inc edi
		add edx, 20
		add ebx, 20
		ret
	}
}


//_______________________________
static void Call_JoyConfig_Main() {
	//The controller configuration utility needs to be run in the main thread. Evoke it by sending "ALT+J".
	INPUT inputs[4] = {};
	ZeroMemory(inputs, sizeof(inputs));

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_MENU;
	inputs[0].ki.dwFlags = 0;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = 'J';
	inputs[1].ki.dwFlags = 0;

	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = 'J';
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.wVk = VK_MENU;
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;

	UINT uSent = SendInput(4, inputs, sizeof(INPUT));
	if (uSent != 4)
		Debug_Info_Error("Call_JoyConfig_Main - SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
}


//_____________________________________________________________________
static void __declspec(naked) options_screen_calibrate_controller(void) {

	__asm {
		pushad
		call Call_JoyConfig_Main
		popad
		ret
	}
}



//___________________________________
static BOOL Check_Update_Input_Time() {

	static LONGLONG duration_HZ = Frequency.QuadPart / 60LL;
	static LONGLONG last_time = 0;
	LONGLONG elapsed_ticks = 0;
	LARGE_INTEGER time = { 0 };

	QueryPerformanceCounter(&time);

	elapsed_ticks = time.QuadPart - last_time;
	if (elapsed_ticks < 0 || elapsed_ticks > duration_HZ) {
		last_time = time.QuadPart;
		return TRUE;
	}
	return FALSE;
}


//_________________________________________________________
static void __declspec(naked) check_update_input_time(void) {

	__asm {
		call Check_Update_Input_Time
		test eax, eax
		ret
	}
}


//__________________________________________
void Modifications_Controller_Enhancements() {

	controller_enhancements_enabled = true;

	//skip the drawing of redundant joystick buttons when controller enhancements are enabled.
	MemWrite8(0x45D2C4, 0x83, 0xE8);
	FuncWrite32(0x45D2C5, 0xC58314C7, (DWORD)&options_screen_skip_joy_button_draws);
	MemWrite32(0x45D2C9, 0x14C38308, 0x90909090);
	MemWrite8(0x45D2CD, 0x41, 0x90);
	
	//just draw the "calibrate joystick" button
	//jump over the drawing of the "calibrate throttle" button and text
	MemWrite16(0x45D3CD, 0x3D75, 0x3BEB); //JMP SHORT 0045D40A                   
	MemWrite8(0x45D40B, 0x34, 0x49); //JMP SHORT 0045D455


	//skip the checking of redundant joystick buttons when controller enhancements are enabled.
	MemWrite8(0x45D797, 0x47, 0xE8);
	FuncWrite32(0x45D798, 0x8314C283, (DWORD)&options_screen_skip_joy_button_checks);
	MemWrite16(0x45D79C, 0x14C3, 0x9090);

	//adjust the y position check for the "calibrate joystick" button.
	//BF 10270000                  MOV EDI, 332-50
	MemWrite16(0x45D7D3, 0x3D6B, 0xBF90);
	MemWrite32(0x45D7D5, 0x4BAC9C, 332-50);
	MemWrite8(0x45D7D9, 0x14, 0x90);

	//call config joy window replacing the old "calibrate joystick" routine flag.
	MemWrite16(0x45D81D, 0x84C7, 0x9090);
	MemWrite8(0x45D81F, 0x24, 0xE8);
	FuncWrite32(0x45D820, 0x023C, (DWORD)&options_screen_calibrate_controller);
	MemWrite32(0x45D824, 0x02, 0x90909090);

	//jump the "calibrate throttle" button check.
	MemWrite16(0x45D828, 0x868B, 0x55EB);//JMP SHORT 0045D87F
	MemWrite32(0x45D82A, 0x0140, 0x90909090);
}


//___________________________
void Modifications_Joystick() {

	// Replaced Sleep(20) function delay with a timer check, when updating controller/keyboard and exit game flag checking.
	// This was bottlenecking the message loop, causing lag when processing mouse and keyboard messages.
	MemWrite8(0x46B0C7, 0x6A, 0xE8);
	FuncWrite32(0x46B0C8, 0x15FF2E14, (DWORD)&check_update_input_time);
	MemWrite16(0x46B0CC, 0x022C, 0xC085);//TEST EAX, EAX
	MemWrite16(0x46B0CE, 0x0057, 0xAC74);//JE SHORT 0046B07C


	// Update controller state when updating keyboard state for GUI interactions.
	FuncReplace32(0x46B0D1, 0xFFFFF003, (DWORD)&joy_update_main);

	// Replace the original joy data update fuction.
	FuncReplace32(0x44FE0E, 0xFFFFB906, (DWORD)&joy_get_current_state);

	// Get the throttle state whether controllers enabled or not to allow throttle +- mouse and keyboard input.
	//jump to "get_current_throttle_state" if controllers disabled.
	MemWrite8(0x44FE0A, 0x0E, 0x07); //JNE SHORT 0044FE12
	// Replace the original joy data proccessing function.
	FuncReplace32(0x44FE15, 0xFFFFB94B, (DWORD)&get_current_throttle_state);

	//jump over zeroing x,y and r axes.
	MemWrite16(0x44FE19, 0x958B, 0x64EB);//JMP SHORT 0044FE7F
	MemWrite32(0x44FE1B, 0x02785C, 0x90909090);

	//jump over roll key checks, '<' and '>'.
	MemWrite8(0x44FF43, 0xBA, 0xE9);//JMP 00450006
	MemWrite32(0x44FF44, 0x08, 0xBE);

	//jump over joy x axis to pc ship roll axis when button 2 held
	MemWrite16(0x450008, 0x840F, 0xE990);
	//jump over joy x and y to pc ship yaw, roll and pitch. 
	MemWrite8(0x45009A, 0x75, 0xEB);

	//jump over "increase speed" key checks, this is now done in function "Update_Axis_Keys".
	MemWrite16(0x441782, 0xDB31, 0x26EB);//JMP SHORT 004417AA
	MemWrite16(0x4417AA, 0x840F, 0xE990);//JMP 00441883

	//jump over "decrease speed" key checks, this is now done in function "Update_Axis_Keys".
	MemWrite16(0x441883, 0xDB31, 0x26EB);//JMP SHORT 004418AB
	MemWrite16(0x4418AB, 0x840F, 0xE990);//JMP 00441984

	//jump over "set speed to zero" key check, this is now done in function "Update_Axis_Keys".
	MemWrite16(0x442F4F, 0xDB31, 0x6AEB);//JMP SHORT 00442FBB

	//jump over "set speed to max" key check, this is now done in function "Update_Axis_Keys".
	MemWrite16(0x442FBB, 0xDB31, 0x62EB);//JMP SHORT 0044301F

	//allow throttle value reading whether throttle control flag set or not. Throttle value for axes and keys are now set in "Update_Axis_Keys".
	MemWrite16(0x44FE88, 0x6A75, 0x9090);
}
