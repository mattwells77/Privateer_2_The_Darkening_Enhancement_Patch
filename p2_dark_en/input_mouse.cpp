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


MOUSE Mouse;


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


//________________________________________
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

	PROFILE_TYPE saved_pro_type = current_pro_type;
	wchar_t profile_name[16];

	for (int i = 0; i < P2_PROFILE_MAX; i++) {
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

		action_key_button[0].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_01", CONFIG_MOUSE_BUTTON_01)));
		action_key_button[1].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_02", CONFIG_MOUSE_BUTTON_02)));
		action_key_button[2].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_03", CONFIG_MOUSE_BUTTON_03)));
		action_key_button[3].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_04", CONFIG_MOUSE_BUTTON_04)));
		action_key_button[4].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"BUTTON_05", CONFIG_MOUSE_BUTTON_05)));


		action_key_wheel_v[0].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"MOUSE_WHEEL_UP", CONFIG_MOUSE_WHEEL_UP)));
		action_key_wheel_v[1].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"MOUSE_WHEEL_DOWN", CONFIG_MOUSE_WHEEL_DOWN)));

		action_key_wheel_h[0].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"MOUSE_WHEEL_LEFT", CONFIG_MOUSE_WHEEL_LEFT)));
		action_key_wheel_h[1].SetAction(static_cast<P2_ACTIONS>(ConfigReadInt_InGame(profile_name, L"MOUSE_WHEEL_RIGHT", CONFIG_MOUSE_WHEEL_RIGHT)));
	}

	current_pro_type = saved_pro_type;
}


//________________
void MOUSE::Save() {

	ConfigWriteInt_InGame(L"MOUSE", L"DEAD_ZONE", Deadzone_Level());

	PROFILE_TYPE saved_pro_type = current_pro_type;
	wchar_t button_name[12];
	wchar_t profile_name[16];


	for (int i = 0; i < P2_PROFILE_MAX; i++) {
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
