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



JOYCAPS joy_caps_original{ 0 };
ACTION_KEY action_key_original[4]{};
ACTION_SWITCH action_pov_original{};


//______________________
static void Joy_Update() {
	Initiate_Target_In_Crosshairs_Wait_Time();
	
	Check_Simulated_Key_For_Release();

	if (controller_enhancements_enabled)
		Joysticks.Update();
	else
		Legacy_Joystick.Update();
		//p2_update_joystick_data(p_p2_space_struct);

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
	//don't draw joy option buttons 12 to 15.
	//don't draw flight orientation button, 16. 
	__asm {
		cmp ecx, 16
		jne check_joy_buttons
		add ecx, 1
		add ebp, 8
		jmp exit_func

		check_joy_buttons:
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
	//don't check joy option buttons 12 to 15.
	//don't check flight orientation button, 16. 
	__asm {
		cmp edi, 16
		jne check_joy_buttons
		add edi, 1
		jmp exit_func

		check_joy_buttons:
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


//______________________________________________________________________________________________________________________________________________________________________________________________________________________
static void __stdcall Set_Legacy_Joy_Caps(float deadzone, DWORD x_min, DWORD y_min, DWORD x_centre, DWORD y_centre, DWORD x_max, DWORD y_max, DWORD t_min, DWORD t_centre, DWORD t_max, DWORD joy_on, DWORD throttle_on) {

	*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_ON) = joy_on;
	*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_THROTTLE_ON) = throttle_on;
	
	if (joyGetNumDevs())
		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_FLAGS) = 5;
	else
		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_FLAGS) = 0;
	
	*(float*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_DEAD_ZONE) = deadzone;

	*(float*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_T_DEAD_ZONE) = 0.1f; //= t_deadzone

	Legacy_Joystick.Load();
	JOYCAPS* caps = Legacy_Joystick.Get_Caps();

	if (!caps) {
		Debug_Info_Error("Set_Legacy_Joy_Caps Failed! ");
		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_X_CEN) = x_centre;
		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_Y_CEN) = y_centre;
		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_T_CEN) = t_centre;

		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_X_MIN) = x_min;
		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_X_MAX) = x_max;

		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_Y_MIN) = y_min;
		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_Y_MAX) = y_max;

		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_T_MIN) = t_min;
		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_T_MAX) = t_max;
	}
	else {
		Legacy_Joystick.Centre_Axes();

		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_X_MIN) = caps->wXmin;//x max
		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_X_MAX) = caps->wXmax;//x min
		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_Y_MIN) = caps->wYmin;//y max
		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_Y_MAX) = caps->wYmax;//y min

		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_T_MIN) = caps->wZmin;//z max
		*(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_T_MAX) = caps->wZmax;//z min
	}

	Debug_Info_Joy("joy on: %u", *(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_ON));//x 
	Debug_Info_Joy("throttle on: %u", *(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_THROTTLE_ON));//x centre

	Debug_Info_Joy("x_centre: %u", *(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_X_CEN));//x centre
	Debug_Info_Joy("y_centre: %u", *(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_Y_CEN));//y centre
	Debug_Info_Joy("z_centre: %u", *(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_T_CEN));//z centre

	Debug_Info_Joy("x_min: %u", *(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_X_MIN));//x max
	Debug_Info_Joy("x_max: %u", *(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_X_MAX));//x min

	Debug_Info_Joy("y_min: %u", *(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_Y_MIN));//y max
	Debug_Info_Joy("y_max: %u", *(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_Y_MAX));//y min

	//Debug_Info("t_deadzone: %f", *(float*)((BYTE*)p_p2_space_struct + 0x2796C));

	Debug_Info_Joy("t_min: %u", *(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_T_MIN));//z max
	Debug_Info_Joy("t_max: %u", *(DWORD*)((BYTE*)p_p2_space_struct + SPACE_STRUCT_JOY_T_MAX));//z min
}


//__________________________________________
void Modifications_Controller_Enhancements() {

	controller_enhancements_enabled = true;
}


//___________________________
void Modifications_Joystick() {

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
	MemWrite32(0x45D7D5, 0x4BAC9C, 332 - 50 - 20);
	MemWrite8(0x45D7D9, 0x14, 0x90);

	//call config joy window replacing the old "calibrate joystick" routine flag.
	MemWrite16(0x45D81D, 0x84C7, 0x9090);
	MemWrite8(0x45D81F, 0x24, 0xE8);
	FuncWrite32(0x45D820, 0x023C, (DWORD)&options_screen_calibrate_controller);
	MemWrite32(0x45D824, 0x02, 0x90909090);

	//jump the "calibrate throttle" button check.
	MemWrite16(0x45D828, 0x868B, 0x55EB);//JMP SHORT 0045D87F
	MemWrite32(0x45D82A, 0x0140, 0x90909090);


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

	//jump over "match speed of target" key check, this is now done in function "Update_Axis_Keys".
	MemWrite8(0x4425B6, 0xBB, 0xE9);//JMP 004426B6
	MemWrite32(0x4425B7, 0x10, 0xFB);

	//allow throttle value reading whether throttle control flag set or not. Throttle value for axes and keys are now set in "Update_Axis_Keys".
	MemWrite16(0x44FE88, 0x6A75, 0x9090);

	//skip y axis orientation section.
	//This is still done in the Original_Joystick_Update function but no longer has an effect on the mouse y axis. Mice have their own setting for inverting the y axis.
	MemWrite8(0x4503FD, 0x75, 0xEB);

	//00421BD3 | .E8 E4980200                      CALL Set_Joy_Caps(float deadzone, x_max, y_max, x_entre, y_centre, x_; \DARK.Set_Joy_Caps(float deadzone, x_max, y_max, x_entre, y_centre, x_min, y_min, t_max, t_centre, t_min, joy_on, throttle_on)
	FuncReplace32(0x421BD4, 0x0298E4, (DWORD)&Set_Legacy_Joy_Caps);

	//skip start space option screen on space entry when joystick not setup.
	MemWrite8(0x4397F9, 0x74, 0xEB);
	//in space option menu skip joy setup on entry when joystick not setup.
	MemWrite8(0x45CFD1, 0x75, 0xEB);
	MemWrite32(0x45D007, 0x02, 0x01);

	//0046A3AD | .  2E:FF15 98015700                 CALL DWORD PTR CS : [<&WINMM.joyGetPosEx>]
	//MemWrite32(0x46A3B0, 0x570198, (DWORD)&p_Original_joyGetPosEx);


	//004503FD | . / 75 28                            JNE SHORT 00450427

}
