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


//__________________________________________
void Modifications_Controller_Enhancements() {

	controller_enhancements_enabled = true;

	//skip the drawing of redundant joystick buttons when controller enhancements are enabled.
	MemWrite8(0x45D2C4, 0x83, 0xE8);
	FuncWrite32(0x45D2C5, 0xC58314C7, (DWORD)&options_screen_skip_joy_button_draws);
	MemWrite32(0x45D2C9, 0x14C38308, 0x90909090);
	MemWrite8(0x45D2CD, 0x41, 0x90);
	//jump joystick and throttle button drawing
	MemWrite8(0x45D36B, 0x50, 0xE9); //JMP 0045D455
	MemWrite32(0x45D36C, 0x2C249C8B, 0xE5);
	MemWrite16(0x45D370, 0x0002, 0x9090);
	MemWrite8(0x45D372, 0x00, 0x90);

	//skip the checking of redundant joystick buttons when controller enhancements are enabled.
	MemWrite8(0x45D797, 0x47, 0xE8);
	FuncWrite32(0x45D798, 0x8314C283, (DWORD)&options_screen_skip_joy_button_checks);
	MemWrite16(0x45D79C, 0x14C3, 0x9090);
	//jump joystick and throttle calibration checks
	MemWrite16(0x45D7C2, 0x8E0F, 0xE990);
}


//___________________________
void Modifications_Joystick() {

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
