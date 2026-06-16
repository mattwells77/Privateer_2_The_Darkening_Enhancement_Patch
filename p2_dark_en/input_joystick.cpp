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


using namespace std;
using namespace winrt;
using namespace Windows::Gaming::Input;

bool alt_flt_mode = true;

bool controller_enhancements_enabled = false;
double key_throttle = 1.0f;//stored throttle value controlled by keys.

JOYSTICKS Joysticks;
P2_JOY_AXES p2_joy_axes{};

PROFILE_TYPE current_pro_type = PROFILE_TYPE::GUI;
PROFILE_TYPE current_pro_type_map = PROFILE_TYPE::Space;

#define P2_ACTIONS_MAX	(static_cast<int>(P2_ACTIONS::End)-1)

vector<PROFILE_TYPE> profile_map_list;

int ReMap_1_list_pos = 0;
int ReMap_2_list_pos = 0;
int ReMap_3_list_pos = 0;

struct SIMULATED_KEY_ACTION {
	P2_ACTIONS action;
	LARGE_INTEGER endTime;
};

vector<SIMULATED_KEY_ACTION> simulated_keys;


//_______________________
bool Is_Alt_Flight_Mode() {
	static bool run_once = false;
	if (!run_once) {
		alt_flt_mode = ConfigReadInt_InGame(L"SPACE", L"ALT_FLIGHT_MODE", CONFIG_SPACE_ALT_FLIGHT_MODE);
		run_once = true;
	}
	return alt_flt_mode;
}


//____________________________________
void Set_Alt_Flight_Mode(bool is_true) {
	alt_flt_mode = is_true;
	ConfigWriteInt_InGame(L"SPACE", L"ALT_FLIGHT_MODE", alt_flt_mode);
}


//__________________________________________________
bool Get_Joystick_Config_Path(wstring *p_ret_string) {
	if (!p_ret_string)
		return false;

	p_ret_string->assign(GetAppDataPath());
	if (!p_ret_string->empty())
		p_ret_string->append(L"\\");
	p_ret_string->append(JOYSTICK_CONFIG_PATH);
	
	Debug_Info_Joy("Get_Joystick_Config_Path: %S", p_ret_string->c_str());

	if (GetFileAttributes(p_ret_string->c_str()) == INVALID_FILE_ATTRIBUTES) {
		if (!CreateDirectory(p_ret_string->c_str(), nullptr)) {
			p_ret_string->clear();
			return false;
		}
	}
	return true;
}


//________________________________________
void Simulate_Key_Press(P2_ACTIONS action) {

	BOOL is_config = JoyConfig_Refresh_CurrentAction(action, TRUE);
	is_config |= JoyConfig_Refresh_CurrentAction_Mouse(action, TRUE);
	if (is_config)
		return;

	switch (action) {
	case P2_ACTIONS::None:
		return;
	case P2_ACTIONS::ReMap_1:
		if (!ReMap_1_list_pos) {
			ReMap_1_list_pos = profile_map_list.size();
			current_pro_type_map = PROFILE_TYPE::ReMap_1;
			profile_map_list.push_back(current_pro_type_map);
		}
		return;
	case P2_ACTIONS::ReMap_2:
		if (!ReMap_2_list_pos) {
			ReMap_2_list_pos = profile_map_list.size();
			current_pro_type_map = PROFILE_TYPE::ReMap_2;
			profile_map_list.push_back(current_pro_type_map);
		}
		return;
	case P2_ACTIONS::ReMap_3:
		if (!ReMap_3_list_pos) {
			ReMap_3_list_pos = profile_map_list.size();
			current_pro_type_map = PROFILE_TYPE::ReMap_3;
			profile_map_list.push_back(current_pro_type_map);
		}
		return;


	case P2_ACTIONS::Pitch_Down:
		p2_joy_axes.y_neg = TRUE;
		return;
	case P2_ACTIONS::Pitch_Up:
		p2_joy_axes.y_pos = TRUE;
		return;
	case P2_ACTIONS::Yaw_Left:
		p2_joy_axes.x_neg = TRUE;
		return;
	case P2_ACTIONS::Yaw_Right:
		p2_joy_axes.x_pos = TRUE;
		return;

	case P2_ACTIONS::Pitch_Down_Yaw_Left:
		p2_joy_axes.y_neg = TRUE;
		p2_joy_axes.x_neg = TRUE;
		return;
	case P2_ACTIONS::Pitch_Down_Yaw_Right:
		p2_joy_axes.y_neg = TRUE;
		p2_joy_axes.x_pos = TRUE;
		return;
	case P2_ACTIONS::Pitch_Up_Yaw_Left:
		p2_joy_axes.y_pos = TRUE;
		p2_joy_axes.x_neg = TRUE;
		return;
	case P2_ACTIONS::Pitch_Up_Yaw_Right:
		p2_joy_axes.y_pos = TRUE;
		p2_joy_axes.x_pos = TRUE;
		return;

	case P2_ACTIONS::Roll_Left:
		p2_joy_axes.r_neg = TRUE;
		return;
	case  P2_ACTIONS::Roll_Right:
		p2_joy_axes.r_pos = TRUE;
		return;

	case P2_ACTIONS::Rotation_Speed_Key:
		p2_joy_axes.button_mod = TRUE;
		return;
	case P2_ACTIONS::Joystick_Roll_Modifier:
		p2_joy_axes.yaw_as_roll = TRUE;
		p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Select_Target_In_Crosshairs)][0]] |= 0x80;
		return;
	default:
		p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(action)][0]] |= 0x80;

		if (P2_ACTIONS_KEYS[static_cast<int>(action)][1] & 0x1)
			p2_keyboard_state_main[VK_SHIFT] |= 0x80;//Shift
		if (P2_ACTIONS_KEYS[static_cast<int>(action)][1] & 0x2)
			p2_keyboard_state_main[VK_CONTROL] |= 0x80;//Ctrl
		if (P2_ACTIONS_KEYS[static_cast<int>(action)][1] & 0x4)
			p2_keyboard_state_main[VK_MENU] |= 0x80;//Alt
		break;
	}
}


//__________________________________________
void Simulate_Key_Release(P2_ACTIONS action) {

	BOOL is_config = JoyConfig_Refresh_CurrentAction(action, FALSE);
	is_config |= JoyConfig_Refresh_CurrentAction_Mouse(action, FALSE);
	if (is_config)
		return;

	switch (action) {
	case P2_ACTIONS::None:
		return;
	case P2_ACTIONS::ReMap_1:
		profile_map_list.erase(profile_map_list.begin() + ReMap_1_list_pos);
		current_pro_type_map = profile_map_list.back();
		if (ReMap_2_list_pos > ReMap_1_list_pos)
			ReMap_2_list_pos--;
		if (ReMap_3_list_pos > ReMap_1_list_pos)
			ReMap_3_list_pos--;
		ReMap_1_list_pos = 0;
		return;
	case P2_ACTIONS::ReMap_2:
		profile_map_list.erase(profile_map_list.begin() + ReMap_2_list_pos);
		current_pro_type_map = profile_map_list.back();
		if (ReMap_1_list_pos > ReMap_2_list_pos)
			ReMap_1_list_pos--;
		if (ReMap_3_list_pos > ReMap_2_list_pos)
			ReMap_3_list_pos--;
		ReMap_2_list_pos = 0;
		return;
	case P2_ACTIONS::ReMap_3:
		profile_map_list.erase(profile_map_list.begin() + ReMap_3_list_pos);
		current_pro_type_map = profile_map_list.back();
		if (ReMap_1_list_pos > ReMap_3_list_pos)
			ReMap_1_list_pos--;
		if (ReMap_2_list_pos > ReMap_3_list_pos)
			ReMap_2_list_pos--;
		ReMap_3_list_pos = 0;
		return;

	case P2_ACTIONS::Pitch_Down:
		p2_joy_axes.y_neg = FALSE;
		return;
	case P2_ACTIONS::Pitch_Up:
		p2_joy_axes.y_pos = FALSE;
		return;
	case P2_ACTIONS::Yaw_Left:
		p2_joy_axes.x_neg = FALSE;
		return;
	case P2_ACTIONS::Yaw_Right:
		p2_joy_axes.x_pos = FALSE;
		return;

	case P2_ACTIONS::Pitch_Down_Yaw_Left:
		p2_joy_axes.y_neg = FALSE;
		p2_joy_axes.x_neg = FALSE;
		return;
	case P2_ACTIONS::Pitch_Down_Yaw_Right:
		p2_joy_axes.y_neg = FALSE;
		p2_joy_axes.x_pos = FALSE;
		return;
	case P2_ACTIONS::Pitch_Up_Yaw_Left:
		p2_joy_axes.y_pos = FALSE;
		p2_joy_axes.x_neg = FALSE;
		return;
	case P2_ACTIONS::Pitch_Up_Yaw_Right:
		p2_joy_axes.y_pos = FALSE;
		p2_joy_axes.x_pos = FALSE;
		return;

	case P2_ACTIONS::Roll_Left:
		p2_joy_axes.r_neg = FALSE;
		return;
	case  P2_ACTIONS::Roll_Right:
		p2_joy_axes.r_pos = FALSE;
		return;

	case P2_ACTIONS::Rotation_Speed_Key:
		p2_joy_axes.button_mod = FALSE;
		return;
	case P2_ACTIONS::Joystick_Roll_Modifier:
		p2_joy_axes.yaw_as_roll = FALSE;
		p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Select_Target_In_Crosshairs)][0]] = 0x0;
		return;
	default:
		p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(action)][0]] = 0x0;

		if (P2_ACTIONS_KEYS[static_cast<int>(action)][1] & 0x1)
			p2_keyboard_state_main[VK_SHIFT] = 0x0;//Shift
		if (P2_ACTIONS_KEYS[static_cast<int>(action)][1] & 0x2)
			p2_keyboard_state_main[VK_CONTROL] = 0x0;//Ctrl
		if (P2_ACTIONS_KEYS[static_cast<int>(action)][1] & 0x4)
			p2_keyboard_state_main[VK_MENU] = 0x0;//Alt
		break;
	}
}


//____________________________________
void Check_Simulated_Key_For_Release() {

	if (simulated_keys.empty())
		return;

	static LARGE_INTEGER time = { 0 };
	QueryPerformanceCounter(&time);

	for (size_t i = 0; i < simulated_keys.size(); i++) {
		if (simulated_keys[i].endTime.QuadPart < time.QuadPart) {
			if (simulated_keys[i].action != P2_ACTIONS::None)
				Simulate_Key_Release(simulated_keys[i].action);
			simulated_keys[i].action = P2_ACTIONS::None;
		}
	}

	while (!simulated_keys.empty() && simulated_keys.back().action == P2_ACTIONS::None)
		simulated_keys.pop_back();
}


//____________________________________________________________
void Simulate_Key_Pressed(P2_ACTIONS action, LONG duration_ms) {

	BOOL is_config = JoyConfig_Refresh_CurrentAction(action, TRUE);
	is_config |= JoyConfig_Refresh_CurrentAction_Mouse(action, TRUE);
	if (is_config)
		return;

	LONGLONG duration = (LONGLONG)duration_ms * Frequency.QuadPart / 1000LL;//ms to ticks

	//check if key already pressed and extend the time held if so.
	for (size_t i = 0; i < simulated_keys.size(); i++) {
		if (simulated_keys[i].action == action) {
			simulated_keys[i].endTime.QuadPart += duration;
			return;
		}
	}

	Simulate_Key_Press(action);
	SIMULATED_KEY_ACTION key{};
	key.action = action;

	QueryPerformanceCounter(&key.endTime);
	key.endTime.QuadPart += duration;
	simulated_keys.push_back(key);
}


//_______________________
void Reset_Key_Throttle() {

	key_throttle = 1.0f;
}


//_______________________________
static float Match_Target_Speed() {

	DWORD num_objects = *p_p2_space_struct_number_of_objects;
	BYTE* player_ship = (BYTE*)p_p2_space_struct + (num_objects * SPACE_OBJECT_STRUCT_SIZE);

	LONG target_obj_num = *(LONG*)(player_ship + SPACE_PC_SHIP_STRUCT_CURRENT_TARGET_VARS);
	target_obj_num >>= 0x10;
	BYTE* target_ship = (BYTE*)p_p2_space_struct + (target_obj_num * SPACE_OBJECT_STRUCT_SIZE);

	DWORD player_ship_type = *(DWORD*)(player_ship + SPACE_PC_SHIP_STRUCT_SHIP_TYPE_VARS);
	player_ship_type >>= 0x10;

	BYTE* ship_type_list = (BYTE*)p_p2_space_object_type_struct_list;
	float player_ship_max_speed = *(float*)(ship_type_list + (player_ship_type * SPACE_OBJECT_TYPE_STRUCT_SIZE));

	float target_speed = *(float*)(target_ship + SPACE_OBJECT_STRUCT_CURRENT_SPEED);

	//float* p_player_ship_current_speed = (float*)(player_ship + SPACE_PC_SHIP_STRUCT_FT_OFFSET);
	//Debug_Info("Match_Target_Speed: tagS: %f, plmaxS %f, plcurrS %f", target_speed, player_ship_max_speed, p_player_ship_current_speed);
	if (target_speed > player_ship_max_speed)
		return 1.0f;

	return target_speed / player_ship_max_speed;
}


//_____________________
void Update_Axis_Keys() {

	static double last_joy_throttle = p2_joy_axes.t;

	static BYTE* keys[11]{
	&p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Roll_Left_NumPad)][0]],
	&p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Pitch_Up_Yaw_Left)][0]],
	&p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Pitch_Up)][0]],
	&p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Pitch_Up_Yaw_Right)][0]],
	&p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Yaw_Left)][0]],
	0,//key num 5 does nothing
	&p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Yaw_Right)][0]],
	&p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Pitch_Down_Yaw_Left)][0]],
	&p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Pitch_Down)][0]],
	&p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Pitch_Down_Yaw_Right)][0]],
	&p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Roll_Right_NumPad)][0]] };


	double button_val = 0.4f;
	if (current_pro_type == PROFILE_TYPE::GUI) 
		button_val = 0.2;
	if (p2_joy_axes.button_mod || p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Rotation_Speed_Key)][0]])
		button_val = 1.0f;
	if (p2_joy_axes.x_neg || (*keys[1] || *keys[4] || *keys[7]))
		p2_joy_axes.x -= button_val;

	if (p2_joy_axes.x_pos || (*keys[3] || *keys[6] || *keys[9]))
		p2_joy_axes.x += button_val;

	if (p2_joy_axes.y_neg || (*keys[7] || *keys[8] || *keys[9]))
		p2_joy_axes.y -= button_val;

	if (p2_joy_axes.y_pos || (*keys[1] || *keys[2] || *keys[3]))
		p2_joy_axes.y += button_val;

	if (p2_joy_axes.r_neg || *keys[0] || p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Roll_Left)][0]])
		p2_joy_axes.r -= button_val;
	if (p2_joy_axes.r_pos || *keys[10] || p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Roll_Right)][0]])
		p2_joy_axes.r += button_val;

	if (current_pro_type == PROFILE_TYPE::Space) {
		static BYTE* p_speed_plus = P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Speed_Increase)];
		static BYTE* p_speed_plus2 = P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Speed_Increase_2)];
		static BYTE* p_speed_minus = P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Speed_Decrease)];
		static BYTE* p_speed_minus2 = P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Speed_Decrease_2)];
		static BYTE* p_speed_zero = P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Speed_Zero)];
		static BYTE* p_speed_max = P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Speed_Max)];

		static BYTE* p_match_target_speed = P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Match_Target_Speed)];

		if (Get_Key_State(p_speed_plus[0],p_speed_plus[1] ,	p_speed_plus[2]) || Get_Key_State(p_speed_plus2[0], p_speed_plus2[1], p_speed_plus2[2]))
			key_throttle += 0.02f;
		
		if (Get_Key_State(p_speed_minus[0], p_speed_minus[1], p_speed_minus[2]) || Get_Key_State(p_speed_minus2[0], p_speed_minus2[1], p_speed_minus2[2]))
			key_throttle -= 0.02f;

		if (key_throttle < 0)
			key_throttle = 0;
		else if (key_throttle > 1.0f)
			key_throttle = 1.0f;


		if (Get_Key_State(p_speed_zero[0], p_speed_zero[1], p_speed_zero[2]))
			key_throttle = 0;

		if (Get_Key_State(p_speed_max[0], p_speed_max[1], p_speed_max[2]))
			key_throttle = 1.0f;

		if (Get_Key_State(p_match_target_speed[0], p_match_target_speed[1], p_match_target_speed[2]))
			key_throttle = Match_Target_Speed();

		if (p2_joy_axes.t != last_joy_throttle) {
			last_joy_throttle = p2_joy_axes.t;
			if(*p_p2_controller_flags != 0)
			key_throttle = p2_joy_axes.t;
		}
		else {
			p2_joy_axes.t = key_throttle;
		}
	}
}


//_________________________
void Maintain_Axis_Limits() {

	if (p2_joy_axes.x < -1.0f)
		p2_joy_axes.x = -1.0f;
	else if (p2_joy_axes.x > 1.0f)
		p2_joy_axes.x = 1.0f;

	if (p2_joy_axes.y < -1.0f)
		p2_joy_axes.y = -1.0;
	else if (p2_joy_axes.y > 1.0f)
		p2_joy_axes.y = 1.0f;

	if (p2_joy_axes.r < -1.0f)
		p2_joy_axes.r = -1.0f;
	else if (p2_joy_axes.r > 1.0f)
		p2_joy_axes.r = 1.0f;

	if (p2_joy_axes.t < 0)
		p2_joy_axes.t = 0;
	else if (p2_joy_axes.t > 1.0f)
		p2_joy_axes.t = 1.0f;

}


//////////////////////////ACTION_KEY/////////////////////////////////

//________________________________________
bool ACTION_KEY::SetButton(bool new_state) {

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
		active_profile = PROFILE_TYPE::End;
	}

	return pressed;
};


//////////////////////////ACTION_AXIS/////////////////////////////////

//____________________________________________________________________________
void ACTION_AXIS::Set_State(double axis_val, bool ignore_space_remap_profiles) {

	PROFILE_TYPE profile_type = current_pro_type;
	//check the space remaps if set, when not deliberately checking the main space profile.
	if (!ignore_space_remap_profiles) {
		if (current_pro_type == PROFILE_TYPE::Space && current_pro_type_map != PROFILE_TYPE::Space)
			profile_type = current_pro_type_map;
	}

	if (calibrating) {
		if (axis_val < limits.min)
			limits.min = axis_val;
		if (axis_val > limits.max)
			limits.max = axis_val;
		if (rev_axis[static_cast<int>(profile_type)])
			axis_val = 1.0f - axis_val;

		current_val = axis_val;
		return;
	}
	if (rev_axis[static_cast<int>(profile_type)])
		axis_val = 1.0f - axis_val;

	if (axis_val > limits.max)
		current_val = limits.max;
	else if (axis_val < limits.min)
		current_val = limits.min;
	current_val = (axis_val - limits.min) / limits.span;

	double half_val = current_val / 2;

	if (is_centred[static_cast<int>(profile_type)]) {
		if (axis_val > limits.centre_max)
			current_centred_val = limits.centre_max;
		else if (axis_val < limits.centre_min)
			current_centred_val = limits.centre_min;

		current_centred_val = (axis_val - limits.centre_min) / limits.centre_span;

		if (current_centred_val > limits.centre) {
			current_centred_val -= limits.centre;
			current_centred_val /= (limits.centre_max - limits.centre) * 2.0f;
		}
		else if (current_centred_val < limits.centre) {
			current_centred_val -= limits.centre;
			current_centred_val /= (limits.centre - limits.centre_min) * 2.0f;
		}
		else
			current_centred_val -= limits.centre;

	}

	//check "axis as buttons" to ensure buttons are released if profile changes while pressed.
	// no need to do this if "ignore_space_remap_profiles" is set, when checking space defaults.
	if (!ignore_space_remap_profiles) {
		PROFILE_TYPE button_profile_type = button_max.GetActiveProfile();
		if (button_profile_type != profile_type && button_max.Is_Pressed()) {
			if (assiged_to[static_cast<int>(button_profile_type)] == AXIS_TYPE::AsOneButton && (current_val < 0.1))
				button_max.SetButton(false);
			else if (assiged_to[static_cast<int>(button_profile_type)] == AXIS_TYPE::AsTwoButtons && (current_val <= 0.1))
				button_max.SetButton(false);
		}
		button_profile_type = button_min.GetActiveProfile();
		if (button_profile_type != profile_type && button_min.Is_Pressed()) {
			if (assiged_to[static_cast<int>(button_profile_type)] == AXIS_TYPE::AsTwoButtons && (current_val >= -0.1))
				button_min.SetButton(false);
		}
	}

	bool new_state = false;

	switch (assiged_to[static_cast<int>(profile_type)]) {
	case AXIS_TYPE::Pointer_X:// check if axis is assigned to Pointer X GUI.
		p2_joy_axes.x += current_centred_val;
		break;
	case AXIS_TYPE::Pointer_Y:// check if axis is assigned to Pointer Y GUI.
		p2_joy_axes.y += current_centred_val;
		break;
	case AXIS_TYPE::Yaw:// check if axis is assigned to Yaw.
		p2_joy_axes.x += current_centred_val;
		break;
	case AXIS_TYPE::Pitch:// check if axis is assigned to Pitch
		p2_joy_axes.y += current_centred_val;
		break;
	case AXIS_TYPE::Roll:// check if axis is assigned to Roll.
		p2_joy_axes.r += current_centred_val;
		break;
		
	case AXIS_TYPE::Throttle:// check if axis is assigned to Throttle.
		p2_joy_axes.t = current_val;
		break;
	case AXIS_TYPE::AsOneButton:// check if axis is assigned to imitate a single button press when pushed toward max.
		new_state = false;
		if (current_val >= 0.1)
			new_state = true;
		button_max.SetButton(new_state);
		break;
	case AXIS_TYPE::AsTwoButtons:// check if axis is assigned to imitate button presses at either end of the axis.
		new_state = false;
		if (current_centred_val > 0.1)
			new_state = true;
		button_max.SetButton(new_state);

		new_state = false;
		if (current_centred_val < -0.1)
			new_state = true;
		button_min.SetButton(new_state);
		break;

	case AXIS_TYPE::Pointer_Left:// check if axis is assigned to Pointer left GUI.
		p2_joy_axes.x -= half_val;
		break;
	case AXIS_TYPE::Pointer_Right:// check if axis is assigned to Pointer right GUI.
		p2_joy_axes.x += half_val;
		break;
	case AXIS_TYPE::Pointer_Up:// check if axis is assigned to Pointer up GUI.
		p2_joy_axes.y += half_val;
		break;
	case AXIS_TYPE::Pointer_Down:// check if axis is assigned to Pointer down GUI.
		p2_joy_axes.y -= half_val;
		break;
	case AXIS_TYPE::Yaw_Left:// check if axis is assigned to Yaw left.
		p2_joy_axes.x -= half_val;
		break;
	case AXIS_TYPE::Yaw_Right:// check if axis is assigned to Yaw right.
		p2_joy_axes.x += half_val;
		break;
	case AXIS_TYPE::Pitch_Up:// check if axis is assigned to Pitch up.
		p2_joy_axes.y += half_val;
		break;
	case AXIS_TYPE::Pitch_Down:// check if axis is assigned to Pitch down.
		p2_joy_axes.y -= half_val;
		break;
	case AXIS_TYPE::Roll_Left:// check if axis is assigned to Roll left.
		p2_joy_axes.r += -half_val;
		break;
	case AXIS_TYPE::Roll_Right:// check if axis is assigned to Roll right.
		p2_joy_axes.r += half_val;
		break;
	default:
		//if axis has not been assigned, check main space profile assignment.
		//this is done so that yaw, pitch, roll and throttle axes set in the main space profile continue to function in remaps. 
		if (current_pro_type == PROFILE_TYPE::Space && current_pro_type != profile_type)
			Set_State(axis_val, true);
		break;
	}
}


//_____________________________________
void ACTION_AXIS::Calibrate(BOOL state) {

	if (state) {
		limits.min = 1.0f;
		limits.max = 0.0f;
		limits.centre = 0.5f;
		calibrating = TRUE;
	}
	else {
		limits.span = limits.max - limits.min;

		//The original joystick function crops 1/64 from max and min values to account for hardware imprecision. Original range (-256 to 256) (-248 to 248). 
		limits.centre_max = limits.max - 0.015625;// 0.03125f;
		limits.centre_min = limits.min + 0.015625;// 0.03125f;

		limits.centre_span = limits.centre_max - limits.centre_min;
		calibrating = FALSE;
	}
};


//////////////////////////ACTION_SWITCH/////////////////////////////////

//_____________________________________________
int ACTION_SWITCH::Switch_Position(int new_pos) {

	PROFILE_TYPE profile_type = current_pro_type;
	if (current_pro_type == PROFILE_TYPE::Space && current_pro_type_map != PROFILE_TYPE::Space)
		profile_type = current_pro_type_map;

	if (!action[static_cast<int>(profile_type)] || new_pos == current_position || new_pos >= num_positions)
		return current_position;
	//release current button
	if (action[static_cast<int>(active_profile)][current_position] != P2_ACTIONS::None && current_position != 0) {
		Simulate_Key_Release(action[static_cast<int>(active_profile)][current_position]);
	}
	//press new button
	if (action[static_cast<int>(profile_type)][new_pos] != P2_ACTIONS::None && new_pos != 0) {
		active_profile = profile_type;
		Simulate_Key_Press(action[static_cast<int>(active_profile)][new_pos]);
	}
	current_position = new_pos;
	return current_position;
};


//////////////////////////JOYSTICK/////////////////////////////////

//_______________________________________________________________________________________________________________________________________
JOYSTICK::JOYSTICK(winrt::Windows::Gaming::Input::RawGameController const& in_rawGameController) :rawGameController(in_rawGameController) {

	pid = rawGameController.HardwareProductId();
	vid = rawGameController.HardwareVendorId();
	NonRoamableId = rawGameController.NonRoamableId();

	DisplayName = rawGameController.DisplayName();
	//Debug_Info("NonRoamableId: %S", NonRoamableId.c_str());

	connected = true;
	enabled = true;

	num_buttons = rawGameController.ButtonCount();
	buttonArray = new bool[num_buttons];


	num_switches = rawGameController.SwitchCount();
	switchArray = new GameControllerSwitchPosition[num_switches];

	num_axes = rawGameController.AxisCount();
	axisArray = new double[num_axes];

	action_button = new ACTION_KEY[num_buttons]{};
	action_axis = new ACTION_AXIS[num_axes]{};

	GameControllerSwitchKind switchKind;
	int num_switch_positions = 0;
	action_switch = new ACTION_SWITCH[num_switches];
	for (int i = 0; i < num_switches; i++) {
		switchKind = rawGameController.GetSwitchKind(i);
		//switchKind enum: 0=2, 1=4, 2=8. shift 2 left by the enum val to get number of positions.
		num_switch_positions = 2 << static_cast<int>(switchKind);
		action_switch[i].Set_Num_Positions(num_switch_positions);
	}

	if (!Profile_Load()) {
		Profile_Load_First_Preset();
		Profile_Save();
	}
};


//_____________________
void JOYSTICK::Update() {

	if (!connected)
		return;
	if (!enabled)
		return;

	rawGameController.GetCurrentReading(array_view<bool>(buttonArray, num_buttons), array_view<GameControllerSwitchPosition>(switchArray, num_switches), array_view<double>(axisArray, num_axes));

	for (int i = 0; i < num_buttons; i++)
		action_button[i].SetButton(buttonArray[i]);
	for (int i = 0; i < num_switches; i++)
		action_switch[i].Switch_Position(static_cast<int>(switchArray[i]));


	for (int i = 0; i < num_axes; i++)
		action_axis[i].Set_State(axisArray[i], false);
}


//__________________________________________________________________________________________________
bool JOYSTICK::Connect(winrt::Windows::Gaming::Input::RawGameController const& in_rawGameController) {

	if (!connected) {
		if (pid == in_rawGameController.HardwareProductId() && vid == in_rawGameController.HardwareVendorId() && NonRoamableId == rawGameController.NonRoamableId()) {
			rawGameController = in_rawGameController;
			Debug_Info_Joy("%S reconnected", rawGameController.DisplayName().c_str());
			//Debug_Info("%S prev NonRoamableId", NonRoamableId.c_str());
			//NonRoamableId = rawGameController.NonRoamableId();
			//Debug_Info("%S new NonRoamableId", NonRoamableId.c_str());
			connected = true;

			return true;
		}
	}
	return false;
};


//_____________________________________________________________________________________________________
bool JOYSTICK::DisConnect(winrt::Windows::Gaming::Input::RawGameController const& in_rawGameController) {

	if (rawGameController == in_rawGameController) {
		connected = false;
		Debug_Info_Joy("%S disconnected", rawGameController.DisplayName().c_str());
		return true;
	}
	return false;
};


//___________________________________________________
BOOL JOYSTICK::Profile_Load(const wchar_t* file_path) {

	FILE* fileCache = nullptr;
	DWORD version = 0;
	int i_data = 0;
	DWORD d_data = 0;

	if (_wfopen_s(&fileCache, file_path, L"rb") == 0) {
		
		fread(&version, sizeof(DWORD), 1, fileCache);
		if (version > JOYSTICK_PROFILE_VERSION) {
			fclose(fileCache);
			Debug_Info_Error("JOYSTICK::Profile_Load(), version mismatch, version:%d", version);
			return FALSE;
		}
		if (version >= 3) {
			fread(&d_data, sizeof(DWORD), 1, fileCache);
			if (d_data != GAME_CODE) {
				fclose(fileCache);
				DWORD type = GAME_CODE;
				Debug_Info_Error("JOYSTICK::Profile_Load(), type mismatch, found: %c%c%c%c, expected: %c%c%c%c", ((BYTE*)&d_data)[0], ((BYTE*)&d_data)[1], ((BYTE*)&d_data)[2], ((BYTE*)&d_data)[3], ((BYTE*)&type)[0], ((BYTE*)&type)[1], ((BYTE*)&type)[2], ((BYTE*)&type)[3]);
				return FALSE;
			}
		}
		fread(&d_data, sizeof(DWORD), 1, fileCache);
		
		enabled = false;
		if (d_data & 1) 
			enabled = true;

		fread(&i_data, sizeof(i_data), 1, fileCache);
		if (i_data != num_axes) {
			fclose(fileCache);
			Debug_Info_Error("JOYSTICK::Profile_Load(), num_axes don't match:%d", version);
			return FALSE;
		}

		PROFILE_TYPE saved_pro_type = current_pro_type;
		ACTION_AXIS* p_axis = nullptr;
		AXIS_LIMITS* p_limits = nullptr;
		ACTION_KEY* p_key = nullptr;
		ACTION_SWITCH* p_switch = nullptr;
		for (int i = 0; i < num_axes; i++) {
			p_axis = Get_Action_Axis(i);
			if (!p_axis) {
				Debug_Info_Error("JOYSTICK::Profile_Load() FAILED!: p_axis %d = null", i);
				fclose(fileCache);
				return FALSE;
			}
			p_limits = p_axis->Get_Axis_Limits();
			fread(&p_limits->min, sizeof(p_limits->min), 1, fileCache);
			fread(&p_limits->max, sizeof(p_limits->max), 1, fileCache);
			fread(&p_limits->span, sizeof(p_limits->span), 1, fileCache);
			fread(&p_limits->centre, sizeof(p_limits->centre), 1, fileCache);

			fread(&p_limits->centre_min, sizeof(p_limits->centre_min), 1, fileCache);
			fread(&p_limits->centre_max, sizeof(p_limits->centre_max), 1, fileCache);
			fread(&p_limits->centre_span, sizeof(p_limits->centre_span), 1, fileCache);

			if (version >= 3)
				current_pro_type = PROFILE_TYPE::GUI;
			else
				current_pro_type = PROFILE_TYPE::Space;
			fread(&i_data, sizeof(i_data), 1, fileCache);
			p_axis->Set_Axis_As(static_cast<AXIS_TYPE>(i_data));

			fread(&i_data, sizeof(i_data), 1, fileCache);
			p_axis->Set_Axis_Reversed(i_data);

			fread(&i_data, sizeof(i_data), 1, fileCache);
			if (i_data < 0 || i_data > P2_ACTIONS_MAX)//ensure i_data is within P2_ACTIONS boundaries, set to "None" is not.
				i_data = 0;
			p_axis->Set_Button_Action_Min(static_cast<P2_ACTIONS>(i_data));

			fread(&i_data, sizeof(i_data), 1, fileCache);
			if (i_data < 0 || i_data > P2_ACTIONS_MAX)//ensure i_data is within P2_ACTIONS boundaries, set to "None" is not.
				i_data = 0;
			p_axis->Set_Button_Action_Max(static_cast<P2_ACTIONS>(i_data));
			if (version >= 3) {
				for (int i = 1; i < NUM_JOY_PROFILES; i++) {
					current_pro_type = static_cast<PROFILE_TYPE>(i);
					fread(&i_data, sizeof(i_data), 1, fileCache);
					p_axis->Set_Axis_As(static_cast<AXIS_TYPE>(i_data));

					fread(&i_data, sizeof(i_data), 1, fileCache);
					p_axis->Set_Axis_Reversed(i_data);

					fread(&i_data, sizeof(i_data), 1, fileCache);
					if (i_data < 0 || i_data > P2_ACTIONS_MAX)//ensure i_data is within P2_ACTIONS boundaries, set to "None" is not.
						i_data = 0;
					p_axis->Set_Button_Action_Min(static_cast<P2_ACTIONS>(i_data));

					fread(&i_data, sizeof(i_data), 1, fileCache);
					if (i_data < 0 || i_data > P2_ACTIONS_MAX)//ensure i_data is within P2_ACTIONS boundaries, set to "None" is not.
						i_data = 0;
					p_axis->Set_Button_Action_Max(static_cast<P2_ACTIONS>(i_data));
				}
			}
		}

		fread(&i_data, sizeof(i_data), 1, fileCache);
		if (i_data != num_buttons) {
			fclose(fileCache);
			Debug_Info_Error("JOYSTICK::Profile_Load(), num_buttons don't match:%d", num_buttons);
			return FALSE;
		}
		if (version >= 3)
			current_pro_type = PROFILE_TYPE::GUI;
		else
			current_pro_type = PROFILE_TYPE::Space;
		for (int i = 0; i < num_buttons; i++) {
			p_key = Get_Action_Button(i);
			if (!p_key) {
				Debug_Info_Error("JOYSTICK::Profile_Load() FAILED!: p_key %d = null", i);
				fclose(fileCache);
				current_pro_type = saved_pro_type;
				return FALSE;
			}
			fread(&i_data, sizeof(i_data), 1, fileCache);
			if (i_data < 0 || i_data > P2_ACTIONS_MAX)//ensure i_data is within P2_ACTIONS boundaries, set to "None" is not.
				i_data = 0;
			p_key->Set_Action(static_cast<P2_ACTIONS>(i_data));
		}
		if (version >= 3) {
			for (int i = 1; i < NUM_JOY_PROFILES; i++) {
				current_pro_type = static_cast<PROFILE_TYPE>(i);
				//current_pro_type = PROFILE_TYPE::Space;
				for (int i = 0; i < num_buttons; i++) {
					p_key = Get_Action_Button(i);
					if (!p_key) {
						Debug_Info_Error("JOYSTICK::Profile_Load() FAILED!: p_key %d = null", i);
						fclose(fileCache);
						current_pro_type = saved_pro_type;
						return FALSE;
					}
					fread(&i_data, sizeof(i_data), 1, fileCache);
					if (i_data < 0 || i_data > P2_ACTIONS_MAX)//ensure i_data is within P2_ACTIONS boundaries, set to "None" is not.
						i_data = 0;
					p_key->Set_Action(static_cast<P2_ACTIONS>(i_data));
				}
			}
		}

		fread(&i_data, sizeof(i_data), 1, fileCache);
		if (i_data != num_switches) {
			fclose(fileCache);
			Debug_Info_Error("JOYSTICK::Profile_Load(), num_switches don't match:%d", num_buttons);
			current_pro_type = saved_pro_type;
			return FALSE;
		}
		for (int i = 0; i < num_switches; i++) {
			p_switch = Get_Action_Switch(i);
			if (!p_switch) {
				Debug_Info_Error("JOYSTICK::Profile_Load() FAILED!: p_switch %d = null", i);
				fclose(fileCache);
				current_pro_type = saved_pro_type;
				return FALSE;
			}

			int num_positions = 0;
			fread(&num_positions, sizeof(num_positions), 1, fileCache);
			
			if (version >= 3)
				current_pro_type = PROFILE_TYPE::GUI;
			else
				current_pro_type = PROFILE_TYPE::Space;

			for (int i = 0; i < num_positions; i++) {
				fread(&i_data, sizeof(i_data), 1, fileCache);
				if (i_data < 0 || i_data > P2_ACTIONS_MAX)//ensure i_data is within P2_ACTIONS boundaries, set to "None" is not.
					i_data = 0;
				p_switch->Set_Action(i, static_cast<P2_ACTIONS>(i_data));
			}
			if (version >= 3) {
				for (int i = 1; i < NUM_JOY_PROFILES; i++) {
					current_pro_type = static_cast<PROFILE_TYPE>(i);
					//current_pro_type = PROFILE_TYPE::Space;
					for (int i = 0; i < num_positions; i++) {
						fread(&i_data, sizeof(i_data), 1, fileCache);
						if (i_data < 0 || i_data > P2_ACTIONS_MAX)//ensure i_data is within P2_ACTIONS boundaries, set to "None" is not.
							i_data = 0;
						p_switch->Set_Action(i, static_cast<P2_ACTIONS>(i_data));
					}
				}
			}
		}

		fclose(fileCache);
		current_pro_type = saved_pro_type;
	}
	else {
		Debug_Info_Error("JOYSTICK::Profile_Load(), _wfopen_s failed: %S", file_path);
		return FALSE;
	}
	return TRUE;
};


//___________________________
BOOL JOYSTICK::Profile_Load() {

	wstring path;
	if (!Get_Joystick_Config_Path(&path))
		return FALSE;

	wchar_t file_name[10 + 33 + 5]{};
	swprintf_s(file_name, 11, L"\\%04x%04x_", vid, pid);

	BYTE bHash[16];//convert the game path to hash data
	HashData((BYTE*)NonRoamableId.c_str(), NonRoamableId.size(), bHash, 16);
	wchar_t* bHashString = file_name + 10;

	for (int i = 0; i < 16; ++i)//convert the hash data to a string, this will be a unique folder name to store the config data in.
		swprintf_s(&bHashString[i * 2], 33 - i * 2, L"%02x", bHash[i]);

	swprintf_s(file_name + 10 + 32, 5, L".joy");

	path.append(file_name);
	Debug_Info_Joy("Profile_Load path: %S", path.c_str());
	return Profile_Load(path.c_str());
};


//________________________________________
BOOL JOYSTICK::Profile_Load_First_Preset() {

	wstring search_path;
	if (!Get_Joystick_Config_Path(&search_path))
		return FALSE;

	search_path.append(L"\\presets");
	if (GetFileAttributes(search_path.c_str()) == INVALID_FILE_ATTRIBUTES)
		return FALSE;

	search_path.append(L"\\");

	wchar_t vid_pid_name[10]{ 0 };
	swprintf_s(vid_pid_name, L"%04x%04x_", Get_VID(), Get_PID());

	//wstring file_path = search_path;

	search_path.append(vid_pid_name);
	search_path.append(L"*.joy");


	WIN32_FIND_DATA FindFileData{};
	HANDLE hFind = hFind = FindFirstFile(search_path.c_str(), &FindFileData);
	FindClose(hFind);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;

	wstring file_path;
	if (!Get_Joystick_Config_Path(&file_path))
		return FALSE;

	file_path.append(L"\\presets\\");
	file_path.append(FindFileData.cFileName);

	if (Profile_Load(file_path.c_str())) {
		Debug_Info_Joy("Profile_Load_First_Preset: %S", file_path.c_str());
		return TRUE;
	}
	return FALSE;
}


//___________________________________________________
BOOL JOYSTICK::Profile_Save(const wchar_t* file_path) {

	FILE* fileCache = nullptr;
	DWORD profile_type = GAME_CODE;
	DWORD version = JOYSTICK_PROFILE_VERSION;
	int i_data = 10;
	DWORD dw_data = 0;

	if (_wfopen_s(&fileCache, file_path, L"wb") == 0) {
		PROFILE_TYPE saved_pro_type = current_pro_type;
		ACTION_AXIS* p_axis = nullptr;
		AXIS_LIMITS* p_limits = nullptr;
		ACTION_KEY* p_key = nullptr;
		ACTION_SWITCH* p_switch = nullptr;

		fwrite(&version, sizeof(DWORD), 1, fileCache);
		fwrite(&profile_type, sizeof(DWORD), 1, fileCache);
		//data = NonRoamableId.size();
		//fwrite(&data, sizeof(DWORD), 1, fileCache);
		//fwrite(NonRoamableId.c_str(), sizeof(wchar_t), NonRoamableId.size(), fileCache);
		dw_data = 0;
		if (enabled)
			dw_data |= 1;
		fwrite(&dw_data, sizeof(DWORD), 1, fileCache);

		fwrite(&num_axes, sizeof(num_axes), 1, fileCache);

		for (int i = 0; i < num_axes; i++) {
			p_axis = Get_Action_Axis(i);
			if (!p_axis) {
				Debug_Info_Error("JOYSTICK::Profile_Save() FAILED!: p_axis %d = null", i);
				fclose(fileCache);
				current_pro_type = saved_pro_type;
				return FALSE;
			}
			p_limits = p_axis->Get_Axis_Limits();
			fwrite(&p_limits->min, sizeof(p_limits->min), 1, fileCache);
			fwrite(&p_limits->max, sizeof(p_limits->max), 1, fileCache);
			fwrite(&p_limits->span, sizeof(p_limits->span), 1, fileCache);
			fwrite(&p_limits->centre, sizeof(p_limits->centre), 1, fileCache);

			fwrite(&p_limits->centre_min, sizeof(p_limits->centre_min), 1, fileCache);
			fwrite(&p_limits->centre_max, sizeof(p_limits->centre_max), 1, fileCache);
			fwrite(&p_limits->centre_span, sizeof(p_limits->centre_span), 1, fileCache);

			for (int i = 0; i < NUM_JOY_PROFILES; i++) {
				current_pro_type = static_cast<PROFILE_TYPE>(i);
				i_data = static_cast<int>(p_axis->Get_Axis_As());
				fwrite(&i_data, sizeof(i_data), 1, fileCache);

				i_data = p_axis->Is_Axis_Reversed();
				fwrite(&i_data, sizeof(i_data), 1, fileCache);

				i_data = static_cast<int>(p_axis->Get_Button_Action_Min());
				fwrite(&i_data, sizeof(i_data), 1, fileCache);

				i_data = static_cast<int>(p_axis->Get_Button_Action_Max());
				fwrite(&i_data, sizeof(i_data), 1, fileCache);
			}
		}

		fwrite(&num_buttons, sizeof(num_buttons), 1, fileCache);
		
		for (int i = 0; i < NUM_JOY_PROFILES; i++) {
			current_pro_type = static_cast<PROFILE_TYPE>(i);
			for (int i = 0; i < num_buttons; i++) {
				p_key = Get_Action_Button(i);
				i_data = static_cast<int>(p_key->GetAction());
				fwrite(&i_data, sizeof(i_data), 1, fileCache);
			}
		}
		fwrite(&num_switches, sizeof(num_switches), 1, fileCache);
		for (int i = 0; i < num_switches; i++) {
			p_switch = Get_Action_Switch(i);
			if (!p_switch) {
				Debug_Info_Error("JOYSTICK::Profile_Save() FAILED!: p_switch %d = null", i);
				fclose(fileCache);
				current_pro_type = saved_pro_type;
				return FALSE;
			}
			int num_positions = p_switch->Get_Num_Positions();
			fwrite(&num_positions, sizeof(num_positions), 1, fileCache);
			
			for (int i = 0; i < NUM_JOY_PROFILES; i++) {
				current_pro_type = static_cast<PROFILE_TYPE>(i);
				for (int i = 0; i < num_positions; i++) {
					i_data = static_cast<int>(p_switch->GetAction(i));
					fwrite(&i_data, sizeof(i_data), 1, fileCache);
				}
			}
		}
		fclose(fileCache);
		current_pro_type = saved_pro_type;
	}
	else {
		Debug_Info_Error("JOYSTICK::Profile_Save(), _wfopen_s failed: %S", file_path);
		return FALSE;
	}
	return TRUE;
};


//___________________________
BOOL JOYSTICK::Profile_Save() {

	wstring path;
	if (!Get_Joystick_Config_Path(&path))
		return FALSE;

	wchar_t file_name[10 + 33 + 5]{};
	swprintf_s(file_name, 11, L"\\%04x%04x_", vid, pid);

	BYTE bHash[16];//convert the game path to hash data
	HashData((BYTE*)NonRoamableId.c_str(), NonRoamableId.size(), bHash, 16);
	wchar_t* bHashString = file_name + 10;

	for (int i = 0; i < 16; ++i)//convert the hash data to a string, this will be a unique folder name to store the config data in.
		swprintf_s(&bHashString[i * 2], 33 - i * 2, L"%02x", bHash[i]);

	swprintf_s(file_name + 10 + 32, 5, L".joy");

	path.append(file_name);

	Debug_Info_Joy("Profile_Save path: %S", path.c_str());
	return Profile_Save(path.c_str());
};


//////////////////////////JOYSTICKS/////////////////////////////////

//_____________________
void JOYSTICKS::Setup() {
	if (setup)
		return;
	profile_map_list.push_back(PROFILE_TYPE::Space);
	Set_Deadzone_Level(ConfigReadInt_InGame(L"MAIN", L"DEAD_ZONE", CONFIG_MAIN_DEAD_ZONE));

	winrt::init_apartment();
	//Debug_Info("JOYSTICKS setup");

	RawGameController::RawGameControllerAdded([this](Windows::Foundation::IInspectable const& /* sender */, RawGameController const& addedController) {
	
			concurrency::critical_section::scoped_lock s1{ controllerListLock };
			for (auto& joystick : joysticks) {
				if (joystick->Connect(addedController)) {
					Debug_Info_Joy("RawGameControllerAdded: controller reconnected");
					JoyConfig_Refresh_JoyList();
					return;
				}
			}
			JOYSTICK* joy = new JOYSTICK(addedController);
			joysticks.push_back(joy);
			Debug_Info_Joy("RawGameControllerAdded: done");
			JoyConfig_Refresh_JoyList();
		});

	RawGameController::RawGameControllerRemoved([this](winrt::Windows::Foundation::IInspectable const&, RawGameController const& removedController) {

			concurrency::critical_section::scoped_lock s2{ controllerListLock };
			for (auto& joystick : joysticks) {
				if (joystick->DisConnect(removedController)) {
					Debug_Info_Joy("RawGameControllerRemoved: Disconnected");
					JoyConfig_Refresh_JoyList();
					return;
				}
			}
			Debug_Info_Error("RawGameControllerRemoved: controller was not found in list!!!");
		});

	setup = true;
};


//______________________
void JOYSTICKS::Update() {

	Setup();

	concurrency::critical_section::scoped_lock s3{ controllerListLock };
	/*
	static PROFILE_TYPE last_profile_type = current_pro_type;
	//clear keyboard on profile change incase button is down during transition.
	if (last_profile_type != current_pro_type) {
		if (last_profile_type == PROFILE_TYPE::GUI || current_pro_type == PROFILE_TYPE::GUI) {
			memset(p2_keyboard_state_main, 0, 256);
			last_profile_type = current_pro_type;
		}
	}*/

	p2_joy_axes.x = 0.0f;
	p2_joy_axes.y = 0.0f;
	p2_joy_axes.r = 0.0f;
	p2_joy_axes.t = 0.0f;

	for (auto& joysticks : joysticks)
		joysticks->Update();

	float f_deadzone = 1.0f / 16 * deadzone;

	if (p2_joy_axes.x <= f_deadzone && p2_joy_axes.x >= -f_deadzone)
		p2_joy_axes.x = 0;
	if (p2_joy_axes.y <= f_deadzone && p2_joy_axes.y >= -f_deadzone)
		p2_joy_axes.y = 0;
	if (p2_joy_axes.r <= f_deadzone && p2_joy_axes.r >= -f_deadzone)
		p2_joy_axes.r = 0;

	p2_joy_axes.x *= 2;
	p2_joy_axes.y *= 2;
	p2_joy_axes.r *= 2;

	if (p2_joy_axes.yaw_as_roll) {
		p2_joy_axes.r = p2_joy_axes.x;
		p2_joy_axes.x = 0;
	}
	else if (Is_Alt_Flight_Mode()) {
		p2_joy_axes.r += p2_joy_axes.x;
	}
	
	Update_Axis_Keys();
	Maintain_Axis_Limits();
}


//____________________
BOOL JOYSTICKS::Save() {

	concurrency::critical_section::scoped_lock s7{ controllerListLock };
	BOOL all_good = TRUE;
	for (auto& joystick : joysticks) {
		if (!joystick->Profile_Save())
			all_good = FALSE;
	}

	ConfigWriteInt_InGame(L"MAIN", L"DEAD_ZONE", deadzone);

	return all_good;
}


//____________________
BOOL JOYSTICKS::Load() {

	concurrency::critical_section::scoped_lock s7{ controllerListLock };
	BOOL all_good = TRUE;
	for (auto& joystick : joysticks) {
		if (!joystick->Profile_Load())
			all_good = FALSE;
	}

	Set_Deadzone_Level(ConfigReadInt_InGame(L"MAIN", L"DEAD_ZONE", CONFIG_MAIN_DEAD_ZONE));
	return all_good;
}


//__________________________
void JOYSTICKS::Centre_All() {

	size_t num_joysticks = GetNumJoysticks();
	if (num_joysticks < 1)
		return;

	//int count = 0;
	JOYSTICK* joy = nullptr;
	int num_axes = 0;
	ACTION_AXIS* axis = nullptr;
	AXIS_TYPE type = AXIS_TYPE::None;

	for (UINT i = 0; i < num_joysticks; i++) {
		joy = GetJoy(i);
		if (joy && joy->IsEnabled()) {
			num_axes = joy->Axes(nullptr);
			for (int axis_num = 0; axis_num < num_axes; axis_num++) {
				axis = joy->Get_Action_Axis(axis_num);
				if (axis) {
					type = axis->Get_Axis_As();
					if (type == AXIS_TYPE::Pointer_Y || type == AXIS_TYPE::Pointer_X ||type == AXIS_TYPE::Pitch || type == AXIS_TYPE::Yaw || type == AXIS_TYPE::Roll) {
						axis->Centre();
						//count++;
					}
				}
			}
		}
	}
	//Debug_Info("Centre_All_Assiged_Axes: %d axes centred", count);

}

