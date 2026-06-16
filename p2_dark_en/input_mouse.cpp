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
#include "configTools.h"
#include "modifications.h"

WORD mouse_state_space[3]{ 0 };
WORD* p_mouse_button_space = &mouse_state_space[0];
WORD* p_mouse_x_space = &mouse_state_space[1];
WORD* p_mouse_y_space = &mouse_state_space[2];

bool mouse_double_click_left = false;
//bool mouse_double_click_right = false;

MOUSE Mouse;


//_____________________________
void Check_Mouse_Double_Click() {

	static LONGLONG doubleclick_time = (LONGLONG)GetDoubleClickTime() * Frequency.QuadPart / 1000LL;//ms to ticks
	static LONG doubleclick_width = GetSystemMetrics(SM_CXDOUBLECLK);
	static LONG doubleclick_height = GetSystemMetrics(SM_CYDOUBLECLK);

	static bool left_click_was_down = 0;
	static LARGE_INTEGER left_click_time = { 0 };
	static LONG left_click_left_x = 0;
	static LONG left_click_left_y = 0;

	BYTE click_state = p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Left_Click)][0]] >> 7;
	if (current_pro_type == PROFILE_TYPE::GUI)
		click_state |= p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Left_Click_Key_GUI)][0]] >> 7;

	if (click_state) {

		left_click_was_down = true;

		if (left_click_time.QuadPart > 0) {
			LARGE_INTEGER time = { 0 };
			QueryPerformanceCounter(&time);
			if (time.QuadPart <= left_click_time.QuadPart) {
				LONG x = 0;
				LONG y = 0;
				Get_Mouse_Position(&x, &y);

				if (abs(x - left_click_left_x) > doubleclick_width || abs(y - left_click_left_y) > doubleclick_height)
					left_click_time.QuadPart = 0;
				else
					mouse_double_click_left = true;
			}
			else
				left_click_time.QuadPart = 0;
		}
	}
	else {
		if (left_click_time.QuadPart > 0) {
			LARGE_INTEGER time = { 0 };
			QueryPerformanceCounter(&time);
			if (time.QuadPart > left_click_time.QuadPart)
				left_click_time.QuadPart = 0;
		}
		else if (left_click_was_down) {
			QueryPerformanceCounter(&left_click_time);
			left_click_time.QuadPart += doubleclick_time;
			Get_Mouse_Position(&left_click_left_x, &left_click_left_y);
		}
		mouse_double_click_left = false;
		left_click_was_down = false;
	}

}


//////////////////////////ACTION_KEY_MOUSE//////////////////////
 
//______________________________________________
bool ACTION_KEY_MOUSE::SetButton(bool new_state) {

	PROFILE_TYPE profile_type = current_pro_type;
	if (current_pro_type == PROFILE_TYPE::Space && current_pro_type_map != PROFILE_TYPE::Space)
		profile_type = current_pro_type_map;

	if (new_state == true && pressed == false) {
		active_profile = profile_type;//ensure this button is bound to the same profile untill it is released.
		Simulate_Key_Press(button[static_cast<int>(active_profile)]);
		pressed = true;
	}
	else if (new_state == false && pressed == true) {
		Simulate_Key_Release(button[static_cast<int>(active_profile)]);
		pressed = false;
	}
	return pressed;
};


//________________________________________________________
void ACTION_KEY_MOUSE::SetButton_Instant(LONG duration_ms) const {

	PROFILE_TYPE profile_type = current_pro_type;
	if (current_pro_type == PROFILE_TYPE::Space && current_pro_type_map != PROFILE_TYPE::Space)
		profile_type = current_pro_type_map;

	Simulate_Key_Pressed(button[static_cast<int>(profile_type)], duration_ms);
};


//////////////////////////MOUSE/////////////////////////////////

//_________________
void MOUSE::Setup() {
	if (setup)
		return;
	Load();
	setup = true;
}


//________________
void MOUSE::Load() {
	
	Set_Deadzone_Level(ConfigReadInt_InGame(L"MOUSE", L"DEAD_ZONE", CONFIG_MOUSE_DEAD_ZONE));
	Set_Axis_Limit_Pecentage(ConfigReadInt_InGame(L"MOUSE", L"AXIS_LIMIT_PERCENTAGE", CONFIG_MOUSE_AXIS_LIMIT_PERCENTAGE));
	bool is_y_axis_inverted = false;
	if (ConfigReadInt_InGame(L"MOUSE", L"INVERT_Y_AXIS", CONFIG_MOUSE_INVERT_Y_AXIS))
		is_y_axis_inverted = true;
	Invert_Y_Axis(is_y_axis_inverted);

	PROFILE_TYPE saved_pro_type = current_pro_type;
	wchar_t profile_name[16];

	for (int i = 0; i < NUM_JOY_PROFILES; i++) {
		switch (i) {
		case 0:
			current_pro_type = PROFILE_TYPE::GUI;
			swprintf(profile_name, _countof(profile_name), L"MOUSE_GUI");
			action_key_button[0].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_01", static_cast<int>(P2_ACTIONS::Left_Click))));
			action_key_button[1].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_02", static_cast<int>(P2_ACTIONS::Right_Click))));
			break;
		case 1:
			current_pro_type = PROFILE_TYPE::Space;
			swprintf(profile_name, _countof(profile_name), L"MOUSE_SPACE");
			action_key_button[0].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_01", static_cast<int>(P2_ACTIONS::Left_Click))));
			action_key_button[1].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_02", static_cast<int>(P2_ACTIONS::Right_Click))));
			break;
		default:
			current_pro_type = static_cast<PROFILE_TYPE>(i);
			swprintf(profile_name, _countof(profile_name), L"MOUSE_REMAP_%02d", i - 1);
			action_key_button[0].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_01", static_cast<int>(P2_ACTIONS::None))));
			action_key_button[1].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_02", static_cast<int>(P2_ACTIONS::None))));
			break;
		}

		//action_key_button[0].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_01", CONFIG_MOUSE_BUTTON_01)));
		//action_key_button[1].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_02", CONFIG_MOUSE_BUTTON_02)));
		action_key_button[2].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_03", static_cast<int>(P2_ACTIONS::None))));
		action_key_button[3].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_04", static_cast<int>(P2_ACTIONS::None))));
		action_key_button[4].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_05", static_cast<int>(P2_ACTIONS::None))));


		action_key_wheel_v[0].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"MOUSE_WHEEL_UP", static_cast<int>(P2_ACTIONS::None))));
		action_key_wheel_v[1].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"MOUSE_WHEEL_DOWN", static_cast<int>(P2_ACTIONS::None))));

		action_key_wheel_h[0].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"MOUSE_WHEEL_LEFT", static_cast<int>(P2_ACTIONS::None))));
		action_key_wheel_h[1].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"MOUSE_WHEEL_RIGHT", static_cast<int>(P2_ACTIONS::None))));
	}

	current_pro_type = saved_pro_type;
}


//________________
void MOUSE::Save() {

	ConfigWriteInt_InGame(L"MOUSE", L"DEAD_ZONE", Deadzone_Level());
	ConfigWriteInt_InGame(L"MOUSE", L"INVERT_Y_AXIS", Is_Y_Axis_Inverted());
	ConfigWriteInt_InGame(L"MOUSE", L"AXIS_LIMIT_PERCENTAGE", Axis_Limit_Percentage());

	PROFILE_TYPE saved_pro_type = current_pro_type;
	wchar_t button_name[12];
	wchar_t profile_name[16];


	for (int i = 0; i < NUM_JOY_PROFILES; i++) {
		switch (i) {
		case 0:
			current_pro_type = PROFILE_TYPE::GUI;
			swprintf(profile_name, _countof(profile_name), L"MOUSE_GUI");
			break;
		case 1:
			current_pro_type = PROFILE_TYPE::Space;
			swprintf(profile_name, _countof(profile_name), L"MOUSE_SPACE");
			break;
		default:
			current_pro_type = static_cast<PROFILE_TYPE>(i);
			swprintf(profile_name, _countof(profile_name), L"MOUSE_REMAP_%02d", i - 1);
			break;
		}

		for (int button = 0; button < NUM_MOUSE_BUTTONS; button++) {
			swprintf(button_name, _countof(button_name), L"BUTTON_%02d", button + 1);
			ConfigWriteInt_InGame(profile_name, button_name, static_cast<int>(action_key_button[button].GetAction()));
		}

		ConfigWriteInt_InGame(profile_name, L"MOUSE_WHEEL_UP", static_cast<int>(action_key_wheel_v[0].GetAction()));
		ConfigWriteInt_InGame(profile_name, L"MOUSE_WHEEL_DOWN", static_cast<int>(action_key_wheel_v[1].GetAction()));

		ConfigWriteInt_InGame(profile_name, L"MOUSE_WHEEL_LEFT", static_cast<int>(action_key_wheel_h[0].GetAction()));
		ConfigWriteInt_InGame(profile_name, L"MOUSE_WHEEL_RIGHT", static_cast<int>(action_key_wheel_h[1].GetAction()));

	}

	current_pro_type = saved_pro_type;
}


//_______________________________________
void MOUSE::Update_Buttons(WPARAM wParam) {

	Setup();

	int key_state = GET_KEYSTATE_WPARAM(wParam);
	if (key_state & MK_LBUTTON)
		action_key_button[0].SetButton(true);
	else
		action_key_button[0].SetButton(false);
	if (key_state & MK_RBUTTON)
		action_key_button[1].SetButton(true);
	else
		action_key_button[1].SetButton(false);
	if (key_state & MK_MBUTTON)
		action_key_button[2].SetButton(true);
	else
		action_key_button[2].SetButton(false);
	if (key_state & MK_XBUTTON1)
		action_key_button[3].SetButton(true);
	else
		action_key_button[3].SetButton(false);
	if (key_state & MK_XBUTTON2)
		action_key_button[4].SetButton(true);
	else
		action_key_button[4].SetButton(false);
}


//_______________________________________________
void MOUSE::Update_Button(int button, bool state) {

	Setup();

	if (button < 0 || button >= NUM_MOUSE_BUTTONS)
		return;

	action_key_button[button].SetButton(state);
}


//______________________________________________
void MOUSE::Update_Wheel_Vertical(WPARAM wParam) {

	Setup();

	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	Debug_Info_Joy("Update_Wheel_Vertical: %d", zDelta);

	float fzDelta = (float)zDelta / 120 * 60;

	if (zDelta > 0)
		action_key_wheel_v[0].SetButton_Instant((LONG)fzDelta);
	else if (zDelta < 0)
		action_key_wheel_v[1].SetButton_Instant(-(LONG)fzDelta);
}


//________________________________________________
void MOUSE::Update_Wheel_Horizontal(WPARAM wParam) {

	Setup();

	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	Debug_Info_Joy("Update_Wheel_Horizontal: %d", zDelta);

	float fzDelta = (float)zDelta / 120 * 60;

	if (zDelta > 0)
		action_key_wheel_h[0].SetButton_Instant((LONG)fzDelta);
	else if (zDelta < 0)
		action_key_wheel_h[1].SetButton_Instant(-(LONG)fzDelta);
}


//____________________________________________
P2_ACTIONS MOUSE::GetAction_Button(int button) {
	
	Setup();

	if (button < 0 || button > NUM_MOUSE_BUTTONS)
		return P2_ACTIONS::None;
	return action_key_button[button].GetAction();
}


//____________________________________
P2_ACTIONS MOUSE::GetAction_Wheel_Up() {
	
	Setup();

	return action_key_wheel_v[0].GetAction();
}


//______________________________________
P2_ACTIONS MOUSE::GetAction_Wheel_Down() {
	
	Setup();

	return action_key_wheel_v[1].GetAction();
}


//______________________________________
P2_ACTIONS MOUSE::GetAction_Wheel_Left() {
	
	Setup();

	return action_key_wheel_h[0].GetAction();
}


//_______________________________________
P2_ACTIONS MOUSE::GetAction_Wheel_Right() {
	
	Setup();

	return action_key_wheel_h[1].GetAction();
}


//_________________________________________________________
void MOUSE::SetAction_Button(int button, P2_ACTIONS action) {
	
	Setup();

	if (button < 0 || button > NUM_MOUSE_BUTTONS)
		return ;
	action_key_button[button].SetAction(action);
}


//_______________________________________________
void MOUSE::SetAction_Wheel_Up(P2_ACTIONS action) {
	
	Setup();

	action_key_wheel_v[0].SetAction(action);
}


//_________________________________________________
void MOUSE::SetAction_Wheel_Down(P2_ACTIONS action) {
	
	Setup();

	action_key_wheel_v[1].SetAction(action);
}


//_________________________________________________
void MOUSE::SetAction_Wheel_Left(P2_ACTIONS action) {
	
	Setup();

	action_key_wheel_h[0].SetAction(action);
}


//__________________________________________________
void MOUSE::SetAction_Wheel_Right(P2_ACTIONS action) {
	
	Setup();

	action_key_wheel_h[1].SetAction(action);
}
