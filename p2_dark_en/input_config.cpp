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

#include "resource.h"
#include "input_config.h"
#include "input.h"
#include "dark.h"
#include "modifications.h"
#include "configTools.h"

using namespace winrt;
using namespace Windows::Gaming::Input;

HWND hWin_SaveAsPreset = nullptr;
HWND hWin_Config_Joy = nullptr;
HWND hWin_Config_Joy_Off = nullptr;
HWND hWin_Config_Joy_Control = nullptr;
HWND hWin_Config_Control = nullptr;
HWND hWin_Config_Mouse = nullptr;
HWND hWin_Config_Mouse_Control = nullptr;

HWND hWin_Config_Keys1 = nullptr;
HWND hWin_Config_Keys2 = nullptr;


BOOL wait_joy_config = FALSE;

BOOL joyList_Updated = 0;

HWND hWin_AxisCalibrate = nullptr;

#define GEN_TEXT_BUFF_COUNT	64
wchar_t general_string_buff[GEN_TEXT_BUFF_COUNT]{ 0 };
wchar_t general_string_buff2[GEN_TEXT_BUFF_COUNT]{ 0 };

int current_JoySelected = -1;

int current_num_axes = 0;
double* current_axisArray = nullptr;

int current_num_buttons = 0;
bool* current_buttonArray = nullptr;

int current_num_switches = 0;
GameControllerSwitchPosition* current_switchArray;

int input_protype_tab_list[]{
	4,//GUI
	0,//Space
	1,//ReMap_1
	2,//ReMap_2
	3,//ReMap_3
};

P2_ACTIONS actions_gui[]{
		P2_ACTIONS::None,
		P2_ACTIONS::Left_Click_Key_GUI,
		P2_ACTIONS::Right_Click_Key_GUI,

		P2_ACTIONS::GUI_PAD,
		P2_ACTIONS::GUI_Exit_Game,
		P2_ACTIONS::GUI_Exit_No,
		P2_ACTIONS::GUI_Exit_Yes,

		P2_ACTIONS::Main_Load_Escape,
		P2_ACTIONS::Main_Load_Select,

		P2_ACTIONS::Navigation_Type_Nav_Point_Num,
		P2_ACTIONS::Navigation_Zoom_In,
		P2_ACTIONS::Navigation_Zoom_Out,
		P2_ACTIONS::Navigation_Reset_View,
		P2_ACTIONS::Navigation_Toggle_Roll_View,
		P2_ACTIONS::Navigation_Toggle_Planets,
		P2_ACTIONS::Navigation_Toggle_Nav_Points,
		P2_ACTIONS::Navigation_Toggle_Nav_point_Numbers,
		P2_ACTIONS::Navigation_Toggle_Bases,
};


P2_ACTIONS actions_gui_mouse[]{
		P2_ACTIONS::None,
		P2_ACTIONS::Left_Click,
		P2_ACTIONS::Right_Click,

		P2_ACTIONS::GUI_PAD,
		P2_ACTIONS::GUI_Exit_Game,
		P2_ACTIONS::GUI_Exit_No,
		P2_ACTIONS::GUI_Exit_Yes,

		P2_ACTIONS::Main_Load_Escape,
		P2_ACTIONS::Main_Load_Select,

		P2_ACTIONS::Navigation_Type_Nav_Point_Num,
		P2_ACTIONS::Navigation_Zoom_In,
		P2_ACTIONS::Navigation_Zoom_Out,
		P2_ACTIONS::Navigation_Reset_View,
		P2_ACTIONS::Navigation_Toggle_Roll_View,
		P2_ACTIONS::Navigation_Toggle_Planets,
		P2_ACTIONS::Navigation_Toggle_Nav_Points,
		P2_ACTIONS::Navigation_Toggle_Nav_point_Numbers,
		P2_ACTIONS::Navigation_Toggle_Bases,
};


P2_ACTIONS actions_space[]{
	P2_ACTIONS::None,

	P2_ACTIONS::Fire_Guns,
	P2_ACTIONS::Fire_Missile,
	P2_ACTIONS::Drop_Mine_Decoy,

	P2_ACTIONS::Jump,
	P2_ACTIONS::Speed_Increase,
	P2_ACTIONS::Speed_Decrease,
	P2_ACTIONS::Speed_Zero,
	P2_ACTIONS::Speed_Max,
	P2_ACTIONS::Afterburner,

	P2_ACTIONS::Target_Nearest_Hostile,
	P2_ACTIONS::Target_Nearest_Friendly,
	P2_ACTIONS::Cycle_Targets_Forward,
	P2_ACTIONS::Cycle_Targets_Back,
	P2_ACTIONS::Select_Target_In_Crosshairs,
	P2_ACTIONS::Match_Target_Speed,
	P2_ACTIONS::Targeting_Off,

	P2_ACTIONS::Tractor_Targeted_Object,
	P2_ACTIONS::Drop_Tractored_Object,

	P2_ACTIONS::VDW_Comms,
	P2_ACTIONS::VDW_Damage,
	P2_ACTIONS::VDW_Guns,
	P2_ACTIONS::VDW_Missiles,
	P2_ACTIONS::VDW_Mines_Decoys,

	P2_ACTIONS::Select_Deselect_Weapon_in_VDW,
	P2_ACTIONS::Cycle_Weapons_in_VDW,

	P2_ACTIONS::Cycle_Radar_Ranges,
	P2_ACTIONS::Toggle_Radar_Type,

	P2_ACTIONS::Nav_Map,
	P2_ACTIONS::Electronic_Diary,
	P2_ACTIONS::Game_Options,
	P2_ACTIONS::Hotkey_List,

	P2_ACTIONS::Toggle_Full_No_Guns,
	P2_ACTIONS::Toggle_SOS,
	P2_ACTIONS::Drop_Nuke_em,
	P2_ACTIONS::Activate_BSE,
	P2_ACTIONS::Warp_Shields,

	P2_ACTIONS::View_Front,
	P2_ACTIONS::View_Left,
	P2_ACTIONS::View_Back,
	P2_ACTIONS::View_Right,
	P2_ACTIONS::View_Ship,
	P2_ACTIONS::View_Target_Camera,
	P2_ACTIONS::View_External,
	P2_ACTIONS::View_Chase_Target,
	P2_ACTIONS::View_Fixed,
	P2_ACTIONS::View_Fly_By_Cinematic,

	//P2_ACTIONS::Exterior_Ship_View_Left,
	//P2_ACTIONS::Exterior_Ship_View_Right,
	//P2_ACTIONS::Exterior_Ship_View_Up,
	//P2_ACTIONS::Exterior_Ship_View_Down,

	P2_ACTIONS::Num_0,
	P2_ACTIONS::Num_1,
	P2_ACTIONS::Num_2,
	P2_ACTIONS::Num_3,
	P2_ACTIONS::Num_4,
	P2_ACTIONS::Num_5,
	P2_ACTIONS::Num_6,
	P2_ACTIONS::Num_7,
	P2_ACTIONS::Num_8,
	P2_ACTIONS::Num_9,

	P2_ACTIONS::Store_Target_0,
	P2_ACTIONS::Store_Target_1,
	P2_ACTIONS::Store_Target_2,
	P2_ACTIONS::Store_Target_3,
	P2_ACTIONS::Store_Target_4,
	P2_ACTIONS::Store_Target_5,
	P2_ACTIONS::Store_Target_6,
	P2_ACTIONS::Store_Target_7,
	P2_ACTIONS::Store_Target_8,
	P2_ACTIONS::Store_Target_9,

	P2_ACTIONS::Pitch_Down,
	P2_ACTIONS::Pitch_Up,
	P2_ACTIONS::Yaw_Left,
	P2_ACTIONS::Yaw_Right,

	P2_ACTIONS::Pitch_Up_Yaw_Left,
	P2_ACTIONS::Pitch_Up_Yaw_Right,
	P2_ACTIONS::Pitch_Down_Yaw_Left,
	P2_ACTIONS::Pitch_Down_Yaw_Right,

	P2_ACTIONS::Roll_Right,
	P2_ACTIONS::Roll_Left,

	P2_ACTIONS::Rotation_Speed_Key,
	P2_ACTIONS::Joystick_Roll_Modifier,
	P2_ACTIONS::Exit_Game,
	//P2_ACTIONS::Space_Exit_No,
	//P2_ACTIONS::Space_Exit_Yes,

	P2_ACTIONS::ReMap_1,
	P2_ACTIONS::ReMap_2,
	P2_ACTIONS::ReMap_3,
};


P2_ACTIONS actions_space_mouse[]{
	P2_ACTIONS::None,

	P2_ACTIONS::Left_Click,
	P2_ACTIONS::Right_Click,

	P2_ACTIONS::Fire_Missile,
	P2_ACTIONS::Drop_Mine_Decoy,

	P2_ACTIONS::Jump,

	P2_ACTIONS::Speed_Increase,
	P2_ACTIONS::Speed_Decrease,
	P2_ACTIONS::Speed_Zero,
	P2_ACTIONS::Speed_Max,
	P2_ACTIONS::Afterburner,

	P2_ACTIONS::Target_Nearest_Hostile,
	P2_ACTIONS::Target_Nearest_Friendly,
	P2_ACTIONS::Cycle_Targets_Forward,
	P2_ACTIONS::Cycle_Targets_Back,
	P2_ACTIONS::Select_Target_In_Crosshairs,
	P2_ACTIONS::Match_Target_Speed,
	P2_ACTIONS::Targeting_Off,

	P2_ACTIONS::Tractor_Targeted_Object,
	P2_ACTIONS::Drop_Tractored_Object,

	P2_ACTIONS::VDW_Comms,
	P2_ACTIONS::VDW_Damage,
	P2_ACTIONS::VDW_Guns,
	P2_ACTIONS::VDW_Missiles,
	P2_ACTIONS::VDW_Mines_Decoys,

	P2_ACTIONS::Select_Deselect_Weapon_in_VDW,
	P2_ACTIONS::Cycle_Weapons_in_VDW,

	P2_ACTIONS::Cycle_Radar_Ranges,
	P2_ACTIONS::Toggle_Radar_Type,

	P2_ACTIONS::Nav_Map,
	P2_ACTIONS::Electronic_Diary,
	P2_ACTIONS::Game_Options,
	P2_ACTIONS::Hotkey_List,

	P2_ACTIONS::Toggle_Full_No_Guns,
	P2_ACTIONS::Toggle_SOS,
	P2_ACTIONS::Drop_Nuke_em,
	P2_ACTIONS::Activate_BSE,
	P2_ACTIONS::Warp_Shields,

	P2_ACTIONS::View_Front,
	P2_ACTIONS::View_Left,
	P2_ACTIONS::View_Back,
	P2_ACTIONS::View_Right,
	P2_ACTIONS::View_Ship,
	P2_ACTIONS::View_Target_Camera,
	P2_ACTIONS::View_External,
	P2_ACTIONS::View_Chase_Target,
	P2_ACTIONS::View_Fixed,
	P2_ACTIONS::View_Fly_By_Cinematic,

	//P2_ACTIONS::Exterior_Ship_View_Left,
	//P2_ACTIONS::Exterior_Ship_View_Right,
	//P2_ACTIONS::Exterior_Ship_View_Up,
	//P2_ACTIONS::Exterior_Ship_View_Down,

	P2_ACTIONS::Num_0,
	P2_ACTIONS::Num_1,
	P2_ACTIONS::Num_2,
	P2_ACTIONS::Num_3,
	P2_ACTIONS::Num_4,
	P2_ACTIONS::Num_5,
	P2_ACTIONS::Num_6,
	P2_ACTIONS::Num_7,
	P2_ACTIONS::Num_8,
	P2_ACTIONS::Num_9,

	P2_ACTIONS::Store_Target_0,
	P2_ACTIONS::Store_Target_1,
	P2_ACTIONS::Store_Target_2,
	P2_ACTIONS::Store_Target_3,
	P2_ACTIONS::Store_Target_4,
	P2_ACTIONS::Store_Target_5,
	P2_ACTIONS::Store_Target_6,
	P2_ACTIONS::Store_Target_7,
	P2_ACTIONS::Store_Target_8,
	P2_ACTIONS::Store_Target_9,

	P2_ACTIONS::Pitch_Down,
	P2_ACTIONS::Pitch_Up,
	P2_ACTIONS::Yaw_Left,
	P2_ACTIONS::Yaw_Right,

	P2_ACTIONS::Pitch_Up_Yaw_Left,
	P2_ACTIONS::Pitch_Up_Yaw_Right,
	P2_ACTIONS::Pitch_Down_Yaw_Left,
	P2_ACTIONS::Pitch_Down_Yaw_Right,

	P2_ACTIONS::Roll_Right,
	P2_ACTIONS::Roll_Left,

	P2_ACTIONS::Rotation_Speed_Key,

	P2_ACTIONS::Exit_Game,
	//P2_ACTIONS::Space_Exit_No,
	//P2_ACTIONS::Space_Exit_Yes,

	P2_ACTIONS::ReMap_1,
	P2_ACTIONS::ReMap_2,
	P2_ACTIONS::ReMap_3,
};


AXIS_TYPE axes_Space[]{
	AXIS_TYPE::None,
	AXIS_TYPE::Yaw,
	AXIS_TYPE::Pitch,
	AXIS_TYPE::Roll,
	AXIS_TYPE::Throttle,
	AXIS_TYPE::AsOneButton,
	AXIS_TYPE::AsTwoButtons,
	AXIS_TYPE::Yaw_Left,
	AXIS_TYPE::Yaw_Right,
	AXIS_TYPE::Pitch_Up,
	AXIS_TYPE::Pitch_Down,
	AXIS_TYPE::Roll_Left,
	AXIS_TYPE::Roll_Right,
};


AXIS_TYPE axes_GUI[]{
	AXIS_TYPE::None,
	AXIS_TYPE::Pointer_X,
	AXIS_TYPE::Pointer_Y,
	AXIS_TYPE::AsOneButton,
	AXIS_TYPE::AsTwoButtons,
	AXIS_TYPE::Pointer_Left,
	AXIS_TYPE::Pointer_Right,
	AXIS_TYPE::Pointer_Up,
	AXIS_TYPE::Pointer_Down,
};


//_______________________________________________
static int Get_Axis_Type_Position(AXIS_TYPE axis) {
	AXIS_TYPE* axes = axes_Space;
	int axis_count = _countof(axes_Space);
	if (current_pro_type == PROFILE_TYPE::GUI) {
		axes = axes_GUI;
		axis_count = _countof(axes_GUI);
	}
	for (int i = 0; i < axis_count; i++) {
		if (axes[i] == axis)
			return i;
	}
	return -1;
}


//__________________________________________
static AXIS_TYPE Get_Axis_Type(int position) {
	AXIS_TYPE* axes = axes_Space;
	int axis_count = _countof(axes_Space);
	if (current_pro_type == PROFILE_TYPE::GUI) {
		axes = axes_GUI;
		axis_count = _countof(axes_GUI);
	}
	if (position < 0 || position >= axis_count)
		return AXIS_TYPE::None;
	return axes[position];
}


//_______________________________________________
static int Get_Action_Position(P2_ACTIONS action) {
	P2_ACTIONS* actions = actions_space;
	int actions_count = _countof(actions_space);
	if (current_pro_type == PROFILE_TYPE::GUI) {
		actions = actions_gui;
		actions_count = _countof(actions_gui);
	}
	for (int i = 0; i < actions_count; i++) {
		if (actions[i] == action)
			return i;
	}
	return -1;
}


//________________________________________
static P2_ACTIONS Get_Action(int position) {
	P2_ACTIONS* actions = actions_space;
	int actions_count = _countof(actions_space);
	if (current_pro_type == PROFILE_TYPE::GUI) {
		actions = actions_gui;
		actions_count = _countof(actions_gui);
	}
	if (position < 0 || position >= actions_count)
		return P2_ACTIONS::End;
	return actions[position];
}


//_____________________________________________________
static int Get_Action_Position_Mouse(P2_ACTIONS action) {
	P2_ACTIONS* actions = actions_space_mouse;
	int actions_count = _countof(actions_space_mouse);
	if (current_pro_type == PROFILE_TYPE::GUI) {
		actions = actions_gui_mouse;
		actions_count = _countof(actions_gui_mouse);
	}
	for (int i = 0; i < actions_count; i++) {
		if (actions[i] == action)
			return i;
	}
	return -1;
}


//______________________________________________
static P2_ACTIONS Get_Action_Mouse(int position) {
	P2_ACTIONS* actions = actions_space_mouse;
	int actions_count = _countof(actions_space_mouse);
	if (current_pro_type == PROFILE_TYPE::GUI) {
		actions = actions_gui_mouse;
		actions_count = _countof(actions_gui_mouse);
	}
	if (position < 0 || position >= actions_count)
		return P2_ACTIONS::End;
	return actions[position];
}



const UINT P2_ACTION_UID[]{
	IDS_NONE,
	IDS_ACTION001,
	IDS_ACTION002,
	IDS_ACTION003,
	IDS_ACTION004,
	IDS_ACTION005,
	IDS_ACTION006,
	IDS_ACTION007,
	IDS_ACTION008,
	IDS_ACTION009,
	IDS_ACTION010,
	IDS_ACTION011,
	IDS_ACTION012,
	IDS_ACTION013,
	IDS_ACTION014,
	IDS_ACTION015,
	IDS_ACTION016,
	IDS_ACTION017,
	IDS_ACTION018,
	IDS_ACTION019,
	IDS_ACTION020,
	IDS_ACTION021,
	IDS_ACTION022,
	IDS_ACTION023,
	IDS_ACTION024,
	IDS_ACTION025,
	IDS_ACTION026,
	IDS_ACTION027,
	IDS_ACTION028,
	IDS_ACTION029,
	IDS_ACTION030,
	IDS_ACTION031,
	IDS_ACTION032,
	IDS_ACTION033,
	IDS_ACTION034,
	IDS_ACTION035,
	IDS_ACTION036,
	IDS_ACTION037,
	IDS_ACTION038,
	IDS_ACTION039,
	IDS_ACTION040,
	IDS_ACTION041,
	IDS_ACTION042,
	IDS_ACTION043,
	IDS_ACTION044,
	IDS_ACTION045,
	IDS_ACTION046,
	IDS_ACTION047,
	IDS_ACTION048,
	IDS_ACTION049,
	IDS_ACTION050,
	IDS_ACTION051,
	IDS_ACTION052,
	IDS_ACTION053,
	IDS_ACTION054,
	IDS_ACTION055,
	IDS_ACTION056,
	IDS_ACTION057,
	IDS_ACTION058,
	IDS_ACTION059,
	IDS_ACTION060,
	IDS_ACTION061,
	IDS_ACTION062,
	IDS_ACTION063,
	IDS_ACTION064,
	IDS_ACTION065,
	IDS_ACTION066,
	IDS_ACTION067,
	IDS_ACTION068,
	IDS_ACTION069,
	IDS_ACTION070,
	IDS_ACTION071,
	IDS_ACTION072,
	IDS_ACTION073,
	IDS_ACTION074,
	IDS_ACTION075,
	IDS_ACTION076,
	IDS_ACTION077,
	IDS_ACTION078,
	IDS_ACTION079,
	IDS_ACTION080,
	IDS_ACTION081,
	IDS_ACTION082,
	IDS_ACTION083,
	IDS_ACTION084,
	IDS_ACTION085,
	IDS_ACTION086,
	IDS_ACTION087,
	IDS_ACTION088,
	IDS_ACTION089,
	IDS_ACTION090,
	IDS_ACTION091,
	IDS_ACTION092,
	IDS_ACTION093,
	IDS_ACTION094,
	IDS_ACTION095,
	IDS_ACTION096,
	IDS_ACTION097,
	IDS_ACTION098,
	IDS_ACTION099,
	IDS_ACTION100,
	IDS_ACTION101,
	IDS_ACTION102,
	IDS_ACTION103,
	IDS_ACTION104,
	IDS_ACTION105,
	IDS_ACTION106,
	IDS_ACTION107,
	IDS_ACTION108,
	IDS_ACTION109,
	IDS_ACTION110,
	IDS_ACTION111,
	IDS_ACTION112,
	IDS_ACTION113,
	IDS_ACTION114,
	IDS_ACTION115,
	IDS_ACTION116,
	IDS_ACTION117,
	IDS_ACTION118,
	IDS_ACTION119,
	IDS_ACTION120,
	IDS_ACTION121,
	IDS_ACTION122,
	IDS_ACTION123,
	IDS_ACTION124,
	IDS_ACTION125,
	IDS_ACTION126,
	IDS_ACTION127,
	IDS_ACTION128,
	IDS_ACTION129,
	IDS_ACTION130,
	IDS_ACTION131,
	IDS_ACTION132,
};


const UINT AXIS_TYPE_UID[]{
	IDS_NONE,
	IDS_AXIS_TYPE001,
	IDS_AXIS_TYPE002,
	IDS_AXIS_TYPE003,
	IDS_AXIS_TYPE004,
	IDS_AXIS_TYPE005,
	IDS_AXIS_TYPE006,
	IDS_AXIS_TYPE007,
	IDS_AXIS_TYPE008,
	IDS_AXIS_TYPE009,
	IDS_AXIS_TYPE010,
	IDS_AXIS_TYPE011,
	IDS_AXIS_TYPE012,
	IDS_AXIS_TYPE013,
	IDS_AXIS_TYPE014,
	IDS_AXIS_TYPE015,
	IDS_AXIS_TYPE016,
	IDS_AXIS_TYPE017,
	IDS_AXIS_TYPE018,
};


const UINT SWITCH_POS_UID[]{
	IDS_SWITCH_POS000,
	IDS_SWITCH_POS001,
	IDS_SWITCH_POS002,
	IDS_SWITCH_POS003,
	IDS_SWITCH_POS004,
	IDS_SWITCH_POS005,
	IDS_SWITCH_POS006,
	IDS_SWITCH_POS007,
	IDS_SWITCH_POS008,
};


//____________________________________________________________________
BOOL JoyConfig_Refresh_CurrentAction(P2_ACTIONS action, BOOL activate) {

	if (!hWin_Config_Joy)
		return FALSE;
	HWND hwnd_sub = GetDlgItem(hWin_Config_Joy, IDC_STATIC_CURRENT_ACTION);
	if (!hwnd_sub)
		return FALSE;
	Debug_Info("JoyConfig_Refresh_CurrentAction %d %d", static_cast<int>(action), current_pro_type);

	UINT UID = P2_ACTION_UID[static_cast<int>(P2_ACTIONS::None)];
	if (activate)
		UID = P2_ACTION_UID[static_cast<int>(action)];
	LoadString(phinstDLL, UID, general_string_buff, _countof(general_string_buff));
	SendMessage(hwnd_sub, (UINT)WM_SETTEXT, (WPARAM)0, (LPARAM)general_string_buff);

	switch (action) {
	case P2_ACTIONS::Pitch_Down:
		p2_joy_axes.y_neg = activate;
		break;
	case P2_ACTIONS::Pitch_Up:
		p2_joy_axes.y_pos = activate;
		break;
	case P2_ACTIONS::Yaw_Left:
		p2_joy_axes.x_neg = activate;
		break;
	case P2_ACTIONS::Yaw_Right:
		p2_joy_axes.x_pos = activate;
		break;

	case P2_ACTIONS::Pitch_Down_Yaw_Left:
		p2_joy_axes.y_neg = activate;
		p2_joy_axes.x_neg = activate;
		break;
	case P2_ACTIONS::Pitch_Down_Yaw_Right:
		p2_joy_axes.y_neg = activate;
		p2_joy_axes.x_pos = activate;
		break;
	case P2_ACTIONS::Pitch_Up_Yaw_Left:
		p2_joy_axes.y_pos = activate;
		p2_joy_axes.x_neg = activate;
		break;
	case P2_ACTIONS::Pitch_Up_Yaw_Right:
		p2_joy_axes.y_pos = activate;
		p2_joy_axes.x_pos = activate;
		break;

	case P2_ACTIONS::Roll_Left:
		p2_joy_axes.r_neg = activate;
		break;
	case  P2_ACTIONS::Roll_Right:
		p2_joy_axes.r_pos = activate;
		break;
	case P2_ACTIONS::Rotation_Speed_Key:
		p2_joy_axes.button_mod = FALSE;
		break;
	case P2_ACTIONS::Joystick_Roll_Modifier:
		p2_joy_axes.yaw_as_roll = FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}


//____________________________________________
static void JoyConfig_Refresh_Button_Display() {

	static bool pressed = false;
	static int button = 0;

	if(!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_button = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_BUTTONS);
	if (!hwnd_button)
		return;
	HWND hwnd_button_state = GetDlgItem(hWin_Config_Joy_Control, IDC_STATIC_BUTTON_STATE);
	if (!hwnd_button_state)
		return;
	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;
	int button_selected = (int)(SendMessage(hwnd_button, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));

	ACTION_KEY* p_action_button = p_joy_selected->Get_Action_Button(button_selected);
	if (!p_action_button)
		return;

	if (button == button_selected && pressed == p_action_button->Is_Pressed())
		return;
	button = button_selected;
	pressed = p_action_button->Is_Pressed();
	
	int id = IDS_UNPRESSED;
	if (pressed)
		id = IDS_PRESSED;
	LoadString(phinstDLL, id, general_string_buff, _countof(general_string_buff));
	SendMessage(hwnd_button_state, (UINT)WM_SETTEXT, (WPARAM)0, (LPARAM)general_string_buff);
}


//_____________________________________________________
static void JoyConfig_Refresh_Buttons(BOOL joy_changed) {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_button = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_BUTTONS);
	if (!hwnd_button)
		return;
	HWND hwnd_action = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_BUTTON_ACTION);
	if (!hwnd_action)
		return;
	HWND hwnd_button_state = GetDlgItem(hWin_Config_Joy_Control, IDC_STATIC_BUTTON_STATE);
	if (!hwnd_button_state)
		return;

	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;

	if (joy_changed) {// setup button list if joystick has changed
		LoadString(phinstDLL, IDS_UNPRESSED, general_string_buff, _countof(general_string_buff));
		SendMessage(hwnd_button_state, (UINT)WM_SETTEXT, (WPARAM)0, (LPARAM)general_string_buff);

		SendMessage(hwnd_button, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
		if (p_joy_selected->Buttons(nullptr) > 0) {
			EnableWindow(hwnd_button, TRUE);
			EnableWindow(hwnd_action, TRUE);
			wchar_t* msg = new wchar_t[12];
			LoadString(phinstDLL, IDS_BUTTON, general_string_buff, _countof(general_string_buff));
			for (int i = 0; i < p_joy_selected->Buttons(nullptr); i++) {
				swprintf_s(msg, 12, L"%s %d", general_string_buff, i);
				SendMessage(hwnd_button, CB_ADDSTRING, (WPARAM)0, (LPARAM)msg);
			}
			delete[] msg;
		}
		else {
			LoadString(phinstDLL, IDS_NONE, general_string_buff, _countof(general_string_buff));
			SendMessage(hwnd_button, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			EnableWindow(hwnd_button, FALSE);
			SendMessage(hwnd_action, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			EnableWindow(hwnd_action, FALSE);
		}
		SendMessage(hwnd_button, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	}

	int button_selected = (int)(SendMessage(hwnd_button, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	ACTION_KEY* p_action_button = p_joy_selected->Get_Action_Button(button_selected);
	if (p_action_button) {
		int action_pos = Get_Action_Position(p_action_button->GetAction());
		if (action_pos >= 0)
			SendMessage(hwnd_action, CB_SETCURSEL, (WPARAM)action_pos, (LPARAM)0);
		return;
	}
}


//______________________________________
static void JoyConfig_Button_SetButton() {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_selected_button = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_BUTTONS);
	if (!hwnd_selected_button)
		return;
	HWND hwnd_action = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_BUTTON_ACTION);
	if (!hwnd_action)
		return;

	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;

	int button_selected = (int)(SendMessage(hwnd_selected_button, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	ACTION_KEY* p_action_button = p_joy_selected->Get_Action_Button(button_selected);
	if (!p_action_button)
		return;

	P2_ACTIONS action = Get_Action((SendMessage(hwnd_action, CB_GETCURSEL, (WPARAM)0, (LPARAM)0)));
	if (action != P2_ACTIONS::End)
		p_action_button->Set_Action(action);
}


//____________________________________________
static void JoyConfig_Refresh_Switch_Display() {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	static int current_position = false;
	static int current_switch = 0;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_switch = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_SWITCHES);
	if (!hwnd_switch)
		return;
	HWND hwnd_button_state = GetDlgItem(hWin_Config_Joy_Control, IDC_STATIC_SWITCH_STATE);
	if (!hwnd_button_state)
		return;
	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;

	int switch_selected = (int)(SendMessage(hwnd_switch, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	ACTION_SWITCH* p_action_switch = p_joy_selected->Get_Action_Switch(switch_selected);
	if (!p_action_switch)
		return;

	if (current_switch == switch_selected && current_position == p_action_switch->Get_Current_Position())
		return;
	current_switch = switch_selected;
	current_position = p_action_switch->Get_Current_Position();

	
	LoadString(phinstDLL, SWITCH_POS_UID[current_position], general_string_buff, _countof(general_string_buff));
	SendMessage(hwnd_button_state, (UINT)WM_SETTEXT, (WPARAM)0, (LPARAM)general_string_buff);
}


//___________________________________________________________________________
static void JoyConfig_Refresh_Switches(BOOL joy_changed, BOOL switch_changed) {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_switch = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_SWITCHES);
	if (!hwnd_switch)
		return;
	HWND hwnd_pos = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_SWITCH_POS);
	if (!hwnd_pos)
		return;
	HWND hwnd_action = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_SWITCH_ACTION);
	if (!hwnd_action)
		return;
	HWND hwnd_button_state = GetDlgItem(hWin_Config_Joy_Control, IDC_STATIC_SWITCH_STATE);
	if (!hwnd_button_state)
		return;

	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;

	if (joy_changed) {
		LoadString(phinstDLL, SWITCH_POS_UID[0], general_string_buff, _countof(general_string_buff));
		SendMessage(hwnd_button_state, (UINT)WM_SETTEXT, (WPARAM)0, (LPARAM)general_string_buff);
		// setup switches list if joystick has changed
		SendMessage(hwnd_switch, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
		if (p_joy_selected->Switches(nullptr) > 0) {
			EnableWindow(hwnd_switch, TRUE);
			wchar_t* msg = new wchar_t[12];
			LoadString(phinstDLL, IDS_SWITCH, general_string_buff, _countof(general_string_buff));
			for (int i = 0; i < p_joy_selected->Switches(nullptr); i++) {
				swprintf_s(msg, 12, L"%s %d", general_string_buff, i);
				SendMessage(hwnd_switch, CB_ADDSTRING, (WPARAM)0, (LPARAM)msg);
			}
			delete[] msg;
		}
		else {
			LoadString(phinstDLL, IDS_NONE, general_string_buff, _countof(general_string_buff));
			SendMessage(hwnd_switch, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			EnableWindow(hwnd_switch, FALSE);
		}
		SendMessage(hwnd_switch, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	}
	if (joy_changed || switch_changed) {
		// setup switch positions list if joystick has changed
		SendMessage(hwnd_pos, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
		int switch_selected = (int)(SendMessage(hwnd_switch, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
		ACTION_SWITCH* p_action_switch = p_joy_selected->Get_Action_Switch(switch_selected);
		if (p_action_switch) {
			EnableWindow(hwnd_action, TRUE);
			EnableWindow(hwnd_pos, TRUE);
			int num_positions = p_action_switch->Get_Num_Positions();
			int step = 8 / (num_positions - 1);
			for (int i = 1; i < 9; i += step) {
				LoadString(phinstDLL, SWITCH_POS_UID[i], general_string_buff, _countof(general_string_buff));
				SendMessage(hwnd_pos, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			}
			SendMessage(hwnd_pos, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		}
		else {
			LoadString(phinstDLL, IDS_NONE, general_string_buff, _countof(general_string_buff));
			SendMessage(hwnd_pos, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_pos, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			EnableWindow(hwnd_pos, FALSE);
			SendMessage(hwnd_action, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			EnableWindow(hwnd_action, FALSE);
		}
	}

	int switch_selected = (int)(SendMessage(hwnd_switch, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	ACTION_SWITCH* p_action_switch = p_joy_selected->Get_Action_Switch(switch_selected);
	if (p_action_switch) {
		int switch_position = (int)(SendMessage(hwnd_pos, CB_GETCURSEL, (WPARAM)0, (LPARAM)0)) + 1;

		int action_pos = Get_Action_Position(p_action_switch->GetAction(switch_position));
		if (action_pos >= 0)
			SendMessage(hwnd_action, CB_SETCURSEL, (WPARAM)action_pos, (LPARAM)0);
		return;
	}
}


//______________________________________
static void JoyConfig_Switch_SetButton() {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_selected_switch = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_SWITCHES);
	if (!hwnd_selected_switch)
		return;
	HWND hwnd_pos = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_SWITCH_POS);
	if (!hwnd_pos)
		return;
	HWND hwnd_action = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_SWITCH_ACTION);
	if (!hwnd_action)
		return;

	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;

	int switch_selected = (int)(SendMessage(hwnd_selected_switch, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	int switch_position = (int)(SendMessage(hwnd_pos, CB_GETCURSEL, (WPARAM)0, (LPARAM)0)) + 1;
	ACTION_SWITCH* p_action_switch = p_joy_selected->Get_Action_Switch(switch_selected);
	if (!p_action_switch)
		return;

	P2_ACTIONS action = Get_Action((SendMessage(hwnd_action, CB_GETCURSEL, (WPARAM)0, (LPARAM)0)));
	if (action != P2_ACTIONS::End)
		p_action_switch->Set_Action(switch_position, action);
}


//__________________________________________
static void JoyConfig_Refresh_Axis_Display() {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_axis = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS);
	if (!hwnd_axis)
		return;

	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;
	int axis_selected = (int)(SendMessage(hwnd_axis, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	ACTION_AXIS* p_action_axis = p_joy_selected->Get_Action_Axis(axis_selected);
	if (!p_action_axis)
		return;
	double val = p_action_axis->Get_Current_Val();

	HWND hwnd_sub = GetDlgItem(hWin_Config_Joy_Control, IDC_STATIC_AXIS_BOX);
	RECT rc{};
	GetWindowRect(hwnd_sub, &rc); //get window rect of control relative to screen
	POINT pt = { rc.left, rc.top }; //new point object using rect x, y
	ScreenToClient(hWin_Config_Joy_Control, &pt); //convert screen co-ords to client based points

	int width = rc.right - rc.left - 2 - 3;

	int i_val = (int)(val * width);
	hwnd_sub = GetDlgItem(hWin_Config_Joy_Control, IDC_STATIC_AXIS_BAR);
	MoveWindow(hwnd_sub, pt.x + 1 + i_val, pt.y + 1, 3, 9, TRUE);
}


//_________________________________________________________________________
static void JoyConfig_Refresh_Axes(BOOL joy_changed, BOOL axis_type_change) {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_axis = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS);
	if (!hwnd_axis)
		return;
	HWND hwnd_type = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS_TYPE);
	if (!hwnd_type)
		return;
	HWND hwnd_sign = GetDlgItem(hWin_Config_Joy_Control, IDC_CHECK_SELECTED_AXIS_SIGN);
	if (!hwnd_sign)
		return;
	HWND hwnd_calibrate = GetDlgItem(hWin_Config_Joy_Control, IDC_BUTTON_CALIBRATE_AXIS);
	if (!hwnd_calibrate)
		return;
	HWND hwnd_centre = GetDlgItem(hWin_Config_Joy_Control, IDC_BUTTON_CENTRE_AXIS);
	if (!hwnd_centre)
		return;
	HWND hwnd_butt1 = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS_BUTTON1);
	if (!hwnd_butt1)
		return;
	HWND hwnd_butt2 = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS_BUTTON2);
	if (!hwnd_butt2)
		return;
	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;

	if (joy_changed) {// setup axes list if joystick has changed
		SendMessage(hwnd_axis, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
		if (p_joy_selected->Axes(nullptr) > 0) {
			EnableWindow(hwnd_axis, TRUE);
			wchar_t* msg = new wchar_t[12];
			LoadString(phinstDLL, IDS_AXIS, general_string_buff, _countof(general_string_buff));
			for (int i = 0; i < p_joy_selected->Axes(nullptr); i++) {
				swprintf_s(msg, 12, L"%s %d", general_string_buff, i);
				SendMessage(hwnd_axis, CB_ADDSTRING, (WPARAM)0, (LPARAM)msg);
			}
			delete[] msg;
		}
		else {
			LoadString(phinstDLL, IDS_NONE, general_string_buff, _countof(general_string_buff));
			SendMessage(hwnd_axis, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			EnableWindow(hwnd_axis, FALSE);
		}
		SendMessage(hwnd_axis, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	}
	if (joy_changed || axis_type_change) {
		int axis_selected = (int)(SendMessage(hwnd_axis, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
		ACTION_AXIS* p_action_axis = p_joy_selected->Get_Action_Axis(axis_selected);
		if (p_action_axis) {
			EnableWindow(hwnd_calibrate, TRUE);
			EnableWindow(hwnd_centre, TRUE);
			EnableWindow(hwnd_sign, TRUE);

			EnableWindow(hwnd_type, TRUE);
			AXIS_TYPE axis_type = p_action_axis->Get_Axis_As();
			int axis_type_pos = Get_Axis_Type_Position(axis_type);
			SendMessage(hwnd_type, CB_SETCURSEL, (WPARAM)axis_type_pos, (LPARAM)0);


			DWORD checked = BST_UNCHECKED;
			if (p_action_axis->Is_Axis_Reversed() == TRUE)
				checked = BST_CHECKED;
			SendMessage(hwnd_sign, BM_SETCHECK, (WPARAM)checked, (LPARAM)0);

			BOOL is_button1 = FALSE;
			BOOL is_button2 = FALSE;
			if (axis_type == AXIS_TYPE::AsOneButton)
				is_button2 = TRUE;
			else if (axis_type == AXIS_TYPE::AsTwoButtons) {
				is_button1 = TRUE;
				is_button2 = TRUE;
			}
			int action_pos = Get_Action_Position(p_action_axis->Get_Button_Action_Min());
			if (action_pos >= 0)
			SendMessage(hwnd_butt1, CB_SETCURSEL, (WPARAM)action_pos, (LPARAM)0);
			EnableWindow(hwnd_butt1, is_button1);
			action_pos = Get_Action_Position(p_action_axis->Get_Button_Action_Max());
			if (action_pos >= 0)
			SendMessage(hwnd_butt2, CB_SETCURSEL, (WPARAM)action_pos, (LPARAM)0);
			EnableWindow(hwnd_butt2, is_button2);

		}
		else {
			SendMessage(hwnd_type, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			EnableWindow(hwnd_type, FALSE);
			SendMessage(hwnd_sign, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			EnableWindow(hwnd_sign, FALSE);
			SendMessage(hwnd_butt1, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			EnableWindow(hwnd_butt1, FALSE);
			SendMessage(hwnd_butt2, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			EnableWindow(hwnd_butt2, FALSE);
			EnableWindow(hwnd_calibrate, FALSE);
			EnableWindow(hwnd_centre, FALSE);
		}
	}
}


//__________________________________
static void JoyConfig_Axis_SetType() {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_axis = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS);
	if (!hwnd_axis)
		return;
	HWND hwnd_type = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS_TYPE);
	if (!hwnd_type)
		return;

	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;

	int axis_selected = (int)(SendMessage(hwnd_axis, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	ACTION_AXIS* p_action_axis = p_joy_selected->Get_Action_Axis(axis_selected);
	if (!p_action_axis)
		return;
	AXIS_TYPE axis_type = Get_Axis_Type((int)(SendMessage(hwnd_type, CB_GETCURSEL, (WPARAM)0, (LPARAM)0)));
	p_action_axis->Set_Axis_As(axis_type);
	JoyConfig_Refresh_Axes(FALSE, TRUE);
}


//__________________________________
static void JoyConfig_Axis_SetSign() {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_axis = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS);
	if (!hwnd_axis)
		return;
	HWND hwnd_sign = GetDlgItem(hWin_Config_Joy_Control, IDC_CHECK_SELECTED_AXIS_SIGN);
	if (!hwnd_sign)
		return;

	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;

	int axis_selected = (int)(SendMessage(hwnd_axis, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	ACTION_AXIS* p_action_axis = p_joy_selected->Get_Action_Axis(axis_selected);
	if (!p_action_axis)
		return;

	DWORD button_state = (int)(SendMessage(hwnd_sign, BM_GETCHECK, (WPARAM)0, (LPARAM)0));
	BOOL is_rev_axis = FALSE;
	if (button_state & BST_CHECKED)
		is_rev_axis = TRUE;

	p_action_axis->Set_Axis_Reversed(is_rev_axis);
}


//____________________________________________________
static void JoyConfig_Axis_SetButton(DWORD IDC_BUTTON) {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_axis = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS);
	if (!hwnd_axis)
		return;
	HWND hwnd_button = GetDlgItem(hWin_Config_Joy_Control, IDC_BUTTON);
	if (!hwnd_button)
		return;

	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;

	int axis_selected = (int)(SendMessage(hwnd_axis, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	ACTION_AXIS* p_action_axis = p_joy_selected->Get_Action_Axis(axis_selected);
	if (!p_action_axis)
		return;

	P2_ACTIONS action = Get_Action((SendMessage(hwnd_button, CB_GETCURSEL, (WPARAM)0, (LPARAM)0)));
	if (action != P2_ACTIONS::End) {
		if (IDC_BUTTON == IDC_COMBO_SELECT_AXIS_BUTTON1)
			p_action_axis->Set_Button_Action_Min(action);
		else if (IDC_BUTTON == IDC_COMBO_SELECT_AXIS_BUTTON2)
			p_action_axis->Set_Button_Action_Max(action);
	}
}


//_____________________________________
static void JoyConfig_Refresh_Enabled() {

	if (!hWin_Config_Joy)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_enabled = GetDlgItem(hWin_Config_Joy, IDC_CHECK_JOY_ENABLE);
	if (!hwnd_enabled)
		return;

	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;
	DWORD checked = BST_UNCHECKED;
	if (p_joy_selected->IsEnabled())
		checked = BST_CHECKED;
	//Debug_Info("JoyConfig_Refresh_Enabled, checked:%d, is_checked:%d", checked);

	SendMessage(hwnd_enabled, BM_SETCHECK, (WPARAM)checked, (LPARAM)0);
}


//____________________________________
static void JoyConfig_Update_Enabled() {

	if (!hWin_Config_Joy)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_enabled = GetDlgItem(hWin_Config_Joy, IDC_CHECK_JOY_ENABLE);
	if (!hwnd_enabled)
		return;

	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;

	DWORD checked = (int)(SendMessage(hwnd_enabled, BM_GETCHECK, (WPARAM)0, (LPARAM)0));
	bool is_checked = false;
	if (checked & BST_CHECKED)
		is_checked = true;
	//Debug_Info("JoyConfig_Update_Enabled, checked:%d, is_checked:%d", checked, is_checked);
	p_joy_selected->Enable(is_checked);
}


//________________________________
static bool JoyConfig_Preset_Set() {

	if (!hWin_Config_Joy)
		return false;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return false;
	HWND hwnd_presets = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_PRESETS);
	if (!hwnd_presets)
		return false;
	int preset_selected = (int)(SendMessage(hwnd_presets, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	if (preset_selected == 0)
		return false;

	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return false;

	std::wstring file_path;
	if (!Get_Joystick_Config_Path(&file_path))
		return false;

	file_path.append(L"\\presets\\");

	wchar_t vid_pid_name[10]{ 0 };
	swprintf_s(vid_pid_name, L"%04x%04x_", p_joy_selected->Get_VID(), p_joy_selected->Get_PID());

	file_path.append(vid_pid_name);
	
	SendMessage(hwnd_presets, CB_GETLBTEXT, (WPARAM)preset_selected, (LPARAM)general_string_buff);
	file_path.append(general_string_buff);
	file_path.append(L".joy");

	if (p_joy_selected->Profile_Load(file_path.c_str()) == FALSE) {
		//if could not load preset from the AppData path, try loading from the local folder.
		if (!Get_Joystick_Config_Path_Local(&file_path))
			return false;
		
		file_path.append(L"\\presets\\");

		wchar_t vid_pid_name[10]{ 0 };
		swprintf_s(vid_pid_name, L"%04x%04x_", p_joy_selected->Get_VID(), p_joy_selected->Get_PID());

		file_path.append(vid_pid_name);

		SendMessage(hwnd_presets, CB_GETLBTEXT, (WPARAM)preset_selected, (LPARAM)general_string_buff);
		file_path.append(general_string_buff);
		file_path.append(L".joy");

		p_joy_selected->Profile_Load(file_path.c_str());
	}

	SendMessage(hwnd_presets, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	return true;
}


//____________________________________________________________________________________________________
static INT_PTR CALLBACK DialogProc_SaveAsPreset(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	static wchar_t* text_current = nullptr;
	static wchar_t* text_last_good = nullptr;
	static JOYSTICK* p_joy_selected = nullptr;

	switch (uMsg) {
	case WM_INITDIALOG: {
		////101 is the wcIII icon
		SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hinst_DARK, MAKEINTRESOURCE(101)));

		HWND hwndParent = GetParent(hwndDlg);

		//set position to centre of parent window.
		RECT rc_Win{ 0,0,0,0 };
		GetWindowRect(hwndDlg, &rc_Win);
		RECT rcParent{ 0,0,0,0 };
		GetWindowRect(hwndParent, &rcParent);
		SetWindowPos(hwndDlg, nullptr, rcParent.left + ((rcParent.right - rcParent.left) - (rc_Win.right - rc_Win.left)) / 2, rc_Win.top + ((rcParent.bottom - rcParent.top) - (rc_Win.bottom - rc_Win.top)) / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		HWND hwnd_edit = GetDlgItem(hwndDlg, IDC_EDIT_PRESET_NAME);
		SendMessage(hwnd_edit, EM_SETLIMITTEXT, (WPARAM)GEN_TEXT_BUFF_COUNT, (LPARAM)0);
		text_current = new wchar_t[GEN_TEXT_BUFF_COUNT] {0};
		text_last_good = new wchar_t[GEN_TEXT_BUFF_COUNT] {0};

		p_joy_selected = (JOYSTICK*)lParam;

		SetFocus(hwnd_edit);
		return FALSE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_EDIT_PRESET_NAME:
			switch (HIWORD(wParam)) {
			case EN_UPDATE: {
				wchar_t editExcludeChars[] = L"\\/:*?<>|";
				HWND hwnd_edit = GetDlgItem(hwndDlg, IDC_EDIT_PRESET_NAME);
				
				bool error = false;
				int text_length = SendMessage(hwnd_edit, WM_GETTEXT, (WPARAM)GEN_TEXT_BUFF_COUNT, (LPARAM)text_current);

				for (int indx = 0; indx < text_length; indx++) {
					wchar_t nChar = text_current[indx];
					for (int compare = 0; compare < _countof(editExcludeChars); compare++) {
						if (nChar == editExcludeChars[compare]) {
							error = true;
							continue;
						}
						if (error)
							continue;
					}
				}
				if (error) {
					DWORD start = 0 , end = 0;
					//get current char selection
					SendMessage(hwnd_edit, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
					//Restore the last good text that was entered 
					SendMessage(hwnd_edit, (UINT)WM_SETTEXT, (WPARAM)0, (LPARAM)text_last_good);
					//restore char selection
					SendMessage(hwnd_edit, EM_SETSEL, (WPARAM)start-1, (LPARAM)end-1);
					//beep
					MessageBeep(MB_OK);
				}
				else
					wcsncpy_s(text_last_good, GEN_TEXT_BUFF_COUNT, text_current, GEN_TEXT_BUFF_COUNT);

				return TRUE;
			}
			}
			break;
		case IDOK: {
			std::wstring file_path;
			if (!Get_Joystick_Config_Path(&file_path)) {
				DestroyWindow(hwndDlg);
				return TRUE;
			}

			file_path.append(L"\\presets");
			if (GetFileAttributes(file_path.c_str()) == INVALID_FILE_ATTRIBUTES) {
				if (!CreateDirectory(file_path.c_str(), nullptr)) {
					DestroyWindow(hwndDlg);
					return TRUE;
				}
			}
			file_path.append(L"\\");

			wchar_t vid_pid_name[10]{ 0 };
			swprintf_s(vid_pid_name, L"%04x%04x_", p_joy_selected->Get_VID(), p_joy_selected->Get_PID());

			file_path.append(vid_pid_name);
			file_path.append(text_last_good);
			file_path.append(L".joy");

			WIN32_FIND_DATA FindFileData{};
			HANDLE hFind = hFind = FindFirstFile(file_path.c_str(), &FindFileData);
			int save_ok = 0;
			if (hFind != INVALID_HANDLE_VALUE) {
				LoadString(phinstDLL, IDS_FILE_ALREADY_EXISTS, general_string_buff, _countof(general_string_buff));
				LoadString(phinstDLL, IDS_OVERWRITE, general_string_buff2, _countof(general_string_buff2));
				save_ok = MessageBox(hwndDlg, general_string_buff2, general_string_buff, MB_YESNO | MB_ICONQUESTION);
			}
			else {
				if (GetLastError() == ERROR_FILE_NOT_FOUND)
					save_ok = IDYES;
			}
			FindClose(hFind);

			if(save_ok == IDYES)
				p_joy_selected->Profile_Save(file_path.c_str());

			DestroyWindow(hwndDlg);
			return TRUE;
		}
		case IDCANCEL:
			DestroyWindow(hwndDlg);
			return TRUE;
		default:
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwndDlg);
		return FALSE;
	case WM_DESTROY:
		if (text_current)
			delete[] text_current;
		text_current = nullptr;
		if (text_last_good)
			delete[] text_last_good;
		text_last_good = nullptr;

		hWin_SaveAsPreset = nullptr;
		return FALSE;
	}
	return FALSE;
}


//_________________________________
static bool JoyConfig_Preset_Save() {

	if (!hWin_Config_Joy)
		return false;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return false;

	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return false;

	hWin_SaveAsPreset = CreateDialogParam(phinstDLL, MAKEINTRESOURCE(IDD_DIALOG_SAVE_AS_PRESET), hWin_Config_Joy, (DLGPROC)DialogProc_SaveAsPreset, (LPARAM)p_joy_selected);
	ShowWindow(hWin_SaveAsPreset, SW_SHOW);
	while (hWin_SaveAsPreset != nullptr) {
		Sleep(16);
		MSG message;
		while (PeekMessage(&message, 0, 0, 0, true)) {
			if (!hWin_SaveAsPreset || !IsDialogMessage(hWin_SaveAsPreset, &message)) {
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
	}

	return true;
}


//_____________________________________
static bool JoyConfig_Refresh_Presets() {

	if (!hWin_Config_Joy)
		return false;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return false;
	HWND hwnd_presets = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_PRESETS);
	if (!hwnd_presets)
		return false;
	SendMessage(hwnd_presets, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);

	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return false;

	std::wstring search_path_appdata;
	if (!Get_Joystick_Config_Path(&search_path_appdata))
		return false;

	search_path_appdata.append(L"\\presets");
	if (GetFileAttributes(search_path_appdata.c_str()) == INVALID_FILE_ATTRIBUTES) {
		if (!CreateDirectory(search_path_appdata.c_str(), nullptr)) {
			Debug_Info_Error("JoyConfig_Refresh_Presets: could not create presets path: %S", search_path_appdata.c_str());
			return false;
		}
	}
	search_path_appdata.append(L"\\");

	// create vendor/productID string
	wchar_t vid_pid_name[10]{ 0 };
	swprintf_s(vid_pid_name, L"%04x%04x_", p_joy_selected->Get_VID(), p_joy_selected->Get_PID());

	std::wstring file_path_appdata = search_path_appdata;

	file_path_appdata.append(vid_pid_name);
	file_path_appdata.append(L"*.joy");
	bool preset_found = false;

	WIN32_FIND_DATA FindFileData{};
	//search AppData path for presets.
	HANDLE hFind = FindFirstFile(file_path_appdata.c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		preset_found = true;
		EnableWindow(hwnd_presets, TRUE);
		LoadString(phinstDLL, IDS_SELECT_PRESET, general_string_buff, _countof(general_string_buff));
		SendMessage(hwnd_presets, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
		SendMessage(hwnd_presets, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

		std::wstring name;
		do {
			name = &FindFileData.cFileName[9];
			name.at(name.find_last_of(L'.')) = L'\0';
			SendMessage(hwnd_presets, CB_ADDSTRING, (WPARAM)0, (LPARAM)name.c_str());
			Debug_Info_Joy("JoyConfig_Refresh_Presets: add AppData preset: %S", name.c_str());

		} while (FindNextFile(hFind, &FindFileData));

		FindClose(hFind);
	}

	std::wstring search_path_local;
	if (!Get_Joystick_Config_Path_Local(&search_path_local))
		return false;

	search_path_local.append(L"\\presets");
	if (GetFileAttributes(search_path_local.c_str()) == INVALID_FILE_ATTRIBUTES)
		return true;

	search_path_local.append(L"\\");

	if (search_path_local.compare(search_path_appdata) == 0) {
		//Debug_Info_Joy("JoyConfig_Refresh_Presets: local path == Appdata path :%S"search_path_local.c_str());
		if (!preset_found) {
			//no presets found, add "None" string and disable combo.
			LoadString(phinstDLL, IDS_NONE, general_string_buff, _countof(general_string_buff));
			SendMessage(hwnd_presets, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_presets, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			EnableWindow(hwnd_presets, FALSE);
			return false;
		}
		return true;
	}
	std::wstring file_path_local = search_path_local;

	file_path_local.append(vid_pid_name);
	file_path_local.append(L"*.joy");

	WIN32_FIND_DATA FindFileData_Local{};
	//search local path for presets.
	HANDLE hFind_Local = FindFirstFile(file_path_local.c_str(), &FindFileData_Local);
	if (hFind_Local != INVALID_HANDLE_VALUE) {
		if (!preset_found) {
			EnableWindow(hwnd_presets, TRUE);
			LoadString(phinstDLL, IDS_SELECT_PRESET, general_string_buff, _countof(general_string_buff));
			SendMessage(hwnd_presets, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_presets, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			preset_found = true;
		}
		std::wstring name;
		do {
			name = &FindFileData_Local.cFileName[9];

			file_path_appdata = search_path_appdata;
			file_path_appdata.append(vid_pid_name);
			file_path_appdata.append(name);

			name.at(name.find_last_of(L'.')) = L'\0';

			//don't add the local preset if a preset with the same name exists in AppData presets.
			//Debug_Info_Joy("JoyConfig_Refresh_Presets: add local preset check appdata: L:%S, A:%S", name.c_str(), file_path_appdata.c_str());
			hFind = FindFirstFile(file_path_appdata.c_str(), &FindFileData);
			if (hFind == INVALID_HANDLE_VALUE) {
				SendMessage(hwnd_presets, CB_ADDSTRING, (WPARAM)0, (LPARAM)name.c_str());
				Debug_Info_Joy("JoyConfig_Refresh_Presets: add local preset: %S", name.c_str());
			}
			else
				FindClose(hFind);

		} while (FindNextFile(hFind_Local, &FindFileData_Local));

		FindClose(hFind_Local);
	}
	if (!preset_found) {
		//no presets found, add "None" string and disable combo.
		LoadString(phinstDLL, IDS_NONE, general_string_buff, _countof(general_string_buff));
		SendMessage(hwnd_presets, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
		SendMessage(hwnd_presets, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		EnableWindow(hwnd_presets, FALSE);
		return false;
	}
	return true;
}


//______________________________
void JoyConfig_Refresh_JoyList() {

	if (!hWin_Config_Joy)
		return;
	joyList_Updated++;
}


//_____________________________________
static void JoyConfig_JoyList_Refresh() {

	if (!hWin_Config_Joy)
		return;
	if (joyList_Updated <= 0)
		return;

	current_JoySelected = -1;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;

	SendMessage(hwnd_joy, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);

	size_t num_joysticks = Joysticks.GetNumJoysticks();

	if (num_joysticks < 1) {
		LoadString(phinstDLL, IDS_NO_CONTROLLERS_DETECTED, general_string_buff, _countof(general_string_buff));
		SendMessage(hwnd_joy, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
	}
	else {
		JOYSTICK* joy = nullptr;
		for (UINT i = 0; i < num_joysticks; i++) {
			joy = Joysticks.GetJoy(i);
			if (!joy)
				return;
			SendMessage(hwnd_joy, CB_ADDSTRING, (WPARAM)0, (LPARAM)joy->Get_DisplayName().c_str());
		}
	}
	SendMessage(hwnd_joy, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);


	JoyConfig_Refresh_Presets();
	JoyConfig_Refresh_Enabled();
	JoyConfig_Refresh_Axes(TRUE, TRUE);
	JoyConfig_Refresh_Buttons(TRUE);
	JoyConfig_Refresh_Switches(TRUE, TRUE);

	if (joyList_Updated > 0)
		joyList_Updated--;
}


//_________________________________________
//Brings a particular contoller state into focus, when it's corresponding button, axis or switch is manipulated.
static void Update_Controller_State_Focus() {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));

	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;
	bool* buttons = nullptr;
	double* axes = nullptr;
	GameControllerSwitchPosition* switches = nullptr;
	int axis_changed = -1;
	int butt_changed = -1;
	int switch_changed = -1;

	if (current_JoySelected != joy_selected) {
		//reset current controller states when joy changed.
		current_JoySelected = joy_selected;

		current_num_axes = p_joy_selected->Axes(&axes);
		
		if (current_axisArray)
			delete[] current_axisArray;
		current_axisArray = nullptr;
		if (current_num_axes > 0) {
			current_axisArray = new double[current_num_axes];
			for (int i = 0; i < current_num_axes; i++)
				current_axisArray[i] = axes[i];
		}

		current_num_buttons = p_joy_selected->Buttons(&buttons);

		if (current_buttonArray)
			delete[] current_buttonArray;
		current_buttonArray = nullptr;
		if (current_num_buttons > 0) {
			current_buttonArray = new bool[current_num_buttons];
			for (int i = 0; i < current_num_buttons; i++)
				current_buttonArray[i] = buttons[i];
		}

		current_num_switches = p_joy_selected->Switches(&switches);

		if (current_switchArray)
			delete[] current_switchArray;
		current_switchArray = nullptr;
		if (current_num_switches > 0) {
			current_switchArray = new GameControllerSwitchPosition[current_num_switches];
			for (int i = 0; i < current_num_switches; i++)
				current_switchArray[i] = switches[i];
		}
	}
	else {
		//compare current controller states against new input.
		int num_axes = p_joy_selected->Axes(&axes);
		if (num_axes > 0 && num_axes == current_num_axes) {
			for (int i = 0; i < current_num_axes; i++) {
				if (current_axisArray[i] > axes[i] + 0.30f || current_axisArray[i] < axes[i] - 0.30f) {
					axis_changed = i;
					current_axisArray[i] = axes[i];
				}
			}
		}
		if (axis_changed >= 0) {
			//Debug_Info("Check_For_Changes - axis changed:%d", axis_changed);
			HWND hwnd_axis = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS);
			SendMessage(hwnd_axis, CB_SETCURSEL, (WPARAM)axis_changed, (LPARAM)0);
			SetFocus(hwnd_axis);
			JoyConfig_Refresh_Axes(FALSE, TRUE);
		}
		
		int num_buttons = p_joy_selected->Buttons(&buttons);
		if (num_buttons > 0 && num_buttons == current_num_buttons) {
			for (int i = 0; i < current_num_buttons; i++) {
				if (current_buttonArray[i] != buttons[i])
					butt_changed = i;
				current_buttonArray[i] = buttons[i];
			}
		}

		if (butt_changed >= 0) {
			//Debug_Info("Check_For_Changes - butt changed:%d", butt_changed);
			HWND hwnd_selected_button = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_BUTTONS);
			SendMessage(hwnd_selected_button, CB_SETCURSEL, (WPARAM)butt_changed, (LPARAM)0);
			SetFocus(hwnd_selected_button);
			JoyConfig_Refresh_Buttons(FALSE);
		}

		int num_switches = p_joy_selected->Switches(&switches);
		if (num_switches > 0 && num_switches == current_num_switches) {
			for (int i = 0; i < current_num_switches; i++) {
				if (current_switchArray[i] != switches[i])
					switch_changed = i;
				current_switchArray[i] = switches[i];
			}
		}

		if (switch_changed >= 0) {
			//Debug_Info("Check_For_Changes - switch changed:%d", switch_changed);
			HWND hwnd_selected_switch = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_SWITCHES);
			SendMessage(hwnd_selected_switch, CB_SETCURSEL, (WPARAM)switch_changed, (LPARAM)0);
			SetFocus(hwnd_selected_switch);
			JoyConfig_Refresh_Switches(FALSE, TRUE);
		}
	}
}


//_________________________________________
static void JoyConfig_Refresh(HWND hwndDlg) {

	JoyConfig_JoyList_Refresh();

	HWND hwnd_sub = nullptr;

	RECT rc{};
	hwnd_sub = GetDlgItem(hwndDlg, IDC_STATIC_XY_BOX);
	GetWindowRect(hwnd_sub, &rc); //get window rect of control relative to screen
	POINT pt = { rc.left, rc.top }; //new point object using rect x, y
	ScreenToClient(hwndDlg, &pt); //convert screen co-ords to client based points
	
	int joy_move_x = (LONG)(16 * p2_joy_axes.x);
	int joy_move_y = (LONG)(16 * p2_joy_axes.y);
	int joy_move_r = (LONG)(16 * p2_joy_axes.r);
	int joy_move_t = (LONG)(100 * p2_joy_axes.t);

	int width = rc.right - rc.left - 2 - 9;
	int height = rc.bottom - rc.top - 2 - 9;
	int pos_x = (int)((float)(joy_move_x + 16) * ((float)width / 32.0f));
	int pos_y = (int)((float)(joy_move_y + 16) * ((float)height / 32.0f));

	hwnd_sub = GetDlgItem(hwndDlg, IDC_STATIC_XY_CROSS);
	MoveWindow(hwnd_sub, pt.x + 1 + pos_x, pt.y + 1 + pos_y, 9, 9, TRUE);

	hwnd_sub = GetDlgItem(hwndDlg, IDC_STATIC_ROLL_BOX);
	GetWindowRect(hwnd_sub, &rc); //get window rect of control relative to screen
	pt = { rc.left, rc.top }; //new point object using rect x, y
	ScreenToClient(hwndDlg, &pt); //convert screen co-ords to client based points

	width = rc.right - rc.left - 2 - 3;
	pos_x = (int)((float)(joy_move_r + 16) * ((float)width / 32.0f));
	hwnd_sub = GetDlgItem(hwndDlg, IDC_STATIC_ROLL_BAR);
	MoveWindow(hwnd_sub, pt.x + 1 + pos_x, pt.y + 1, 3, 9, TRUE);

	hwnd_sub = GetDlgItem(hwndDlg, IDC_STATIC_THROTTLE_BOX);
	GetWindowRect(hwnd_sub, &rc); //get window rect of control relative to screen
	pt = { rc.left, rc.top }; //new point object using rect x, y
	ScreenToClient(hwndDlg, &pt); //convert screen co-ords to client based points

	height = rc.bottom - rc.top - 2 - 3;
	pos_y = (int)((float)joy_move_t * ((float)height / 100.0f));
	hwnd_sub = GetDlgItem(hwndDlg, IDC_STATIC_THROTTLE_BAR);
	MoveWindow(hwnd_sub, pt.x + 1, pt.y + 1 + pos_y, 9, 3, TRUE);
	

	Update_Controller_State_Focus();

	JoyConfig_Refresh_Axis_Display();
	JoyConfig_Refresh_Button_Display();
	JoyConfig_Refresh_Switch_Display();
}


//_____________________________________________________________________________________
static void Axis_Calibration_Refresh_Axis_Display(HWND hwndDlg, int joystick, int axis) {

	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joystick);
	if (!p_joy_selected) 
		return;
	
	ACTION_AXIS* p_action_axis = p_joy_selected->Get_Action_Axis(axis);
	if (!p_action_axis) 
		return;
	
	double val = p_action_axis->Get_Current_Val();

	HWND hwnd_sub = GetDlgItem(hwndDlg, IDC_STATIC_AXIS_BOX);
	RECT rc{};
	GetWindowRect(hwnd_sub, &rc); //get window rect of control relative to screen
	POINT pt = { rc.left, rc.top }; //new point object using rect x, y
	ScreenToClient(hwndDlg, &pt); //convert screen co-ords to client based points

	int width = rc.right - rc.left - 2 - 3;

	int i_val = (int)(val * width);
	hwnd_sub = GetDlgItem(hwndDlg, IDC_STATIC_AXIS_BAR);
	MoveWindow(hwnd_sub, pt.x + 1 + i_val, pt.y + 1, 3, 9, TRUE);
}


//_______________________________________________________________________________________________________
static INT_PTR CALLBACK DialogProc_AxisCalibration(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_INITDIALOG: {
		////101 is the wcIII icon
		SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hinst_DARK, MAKEINTRESOURCE(101)));

		HWND hwndParent = GetParent(hwndDlg);

		//set position to centre of parent window.
		RECT rc_Win{ 0,0,0,0 };
		GetWindowRect(hwndDlg, &rc_Win);
		RECT rcParent{ 0,0,0,0 };
		GetWindowRect(hwndParent, &rcParent);
		SetWindowPos(hwndDlg, nullptr, rcParent.left + ((rcParent.right - rcParent.left) - (rc_Win.right - rc_Win.left)) / 2, rc_Win.top + ((rcParent.bottom - rcParent.top) - (rc_Win.bottom - rc_Win.top)) / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case IDOK:
		case IDCANCEL: 
			DestroyWindow(hwndDlg);
			return TRUE;
		default:
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwndDlg);
		return FALSE;
	case WM_DESTROY: 
		hWin_AxisCalibrate = nullptr;
		return FALSE;
	}
	
	return FALSE;
}


//____________________________________
static void JoyConfig_Axis_Calibrate() {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_axis = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS);
	if (!hwnd_axis)
		return;

	int calibrating_joy = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(calibrating_joy);
	if (!p_joy_selected) 
		return;
	int	calibrating_axis = (int)(SendMessage(hwnd_axis, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	ACTION_AXIS* p_action_axis = p_joy_selected->Get_Action_Axis(calibrating_axis);
	if (!p_action_axis)
		return;
	p_action_axis->Calibrate(TRUE);

	hWin_AxisCalibrate = CreateDialog(phinstDLL, MAKEINTRESOURCE(IDD_DIALOG_CALIBRATE), hWin_Config_Joy, (DLGPROC)DialogProc_AxisCalibration);
	ShowWindow(hWin_AxisCalibrate, SW_SHOW);
	while (hWin_AxisCalibrate != nullptr) {
		Sleep(16);
		MSG message;
		while (PeekMessage(&message, 0, 0, 0, true)) {
			if (!hWin_AxisCalibrate || !IsDialogMessage(hWin_AxisCalibrate, &message)) {
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
		Joysticks.Update();
		Axis_Calibration_Refresh_Axis_Display(hWin_AxisCalibrate, calibrating_joy, calibrating_axis);
	}

	//re-get p_joy_selected and p_action_axis incase joystick was lost during axis calibration.
	p_joy_selected = Joysticks.GetJoy(calibrating_joy);
	if (!p_joy_selected)
		return;
	p_action_axis = p_joy_selected->Get_Action_Axis(calibrating_axis);
	if (!p_action_axis)
		return;
	p_action_axis->Calibrate(FALSE);
}


//_________________________________
static void JoyConfig_Axis_Centre() {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_axis = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS);
	if (!hwnd_axis)
		return;


	int joy_selected = (int)(SendMessage(hwnd_joy, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	JOYSTICK* p_joy_selected = Joysticks.GetJoy(joy_selected);
	if (!p_joy_selected)
		return;

	int axis_selected = (int)(SendMessage(hwnd_axis, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	ACTION_AXIS* p_action_axis = p_joy_selected->Get_Action_Axis(axis_selected);
	if (!p_action_axis)
		return;

	p_action_axis->Centre();
}


//_________________________________________
static void JoyConfig_Update_Profile_Vars() {

	if (!hWin_Config_Joy)
		return;
	if (!hWin_Config_Joy_Control)
		return;

	HWND hwnd_joy = GetDlgItem(hWin_Config_Joy, IDC_COMBO_JOY_SELECT);
	if (!hwnd_joy)
		return;
	HWND hwnd_axis = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS);
	if (!hwnd_axis)
		return;

	HWND hwnd_sub = nullptr;
	RECT rc{};
	POINT pt{};

	//fill text for axis type combo
	hwnd_sub = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS_TYPE);

	//AXIS_TYPE_UID
	AXIS_TYPE* axis_types = axes_Space;
	int axis_type_count = _countof(axes_Space);
	P2_ACTIONS* actions = actions_space;
	int actions_count = _countof(actions_space);
	if (current_pro_type == PROFILE_TYPE::GUI) {
		axis_types = axes_GUI;
		axis_type_count = _countof(axes_GUI);
		actions = actions_gui;
		actions_count = _countof(actions_gui);
	}
	SendMessage(hwnd_sub, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	for (int i = 0; i < axis_type_count; i++) {
		LoadString(phinstDLL, AXIS_TYPE_UID[static_cast<int>(axis_types[i])], general_string_buff, _countof(general_string_buff));
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
	}
	SendMessage(hwnd_sub, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);


	HWND hwnd_axis_act1 = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS_BUTTON1);
	SendMessage(hwnd_axis_act1, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	HWND hwnd_axis_act2 = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_AXIS_BUTTON2);
	SendMessage(hwnd_axis_act2, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	HWND hwnd_butt_act = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_BUTTON_ACTION);
	SendMessage(hwnd_butt_act, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	HWND hwnd_switch_act = GetDlgItem(hWin_Config_Joy_Control, IDC_COMBO_SELECT_SWITCH_ACTION);
	SendMessage(hwnd_switch_act, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);

	//speed up combobox initialization by preventing drawing while adding strings.
	SetWindowRedraw(hwnd_axis_act1, FALSE);
	SetWindowRedraw(hwnd_axis_act2, FALSE);
	SetWindowRedraw(hwnd_butt_act, FALSE);
	SetWindowRedraw(hwnd_switch_act, FALSE);
	
	//speed up combobox initialization by setting the count and estimated memory usage first. 
	SendMessage(hwnd_axis_act1, CB_INITSTORAGE, actions_count, sizeof(general_string_buff) * actions_count);
	SendMessage(hwnd_axis_act2, CB_INITSTORAGE, actions_count, sizeof(general_string_buff) * actions_count);
	SendMessage(hwnd_butt_act, CB_INITSTORAGE, actions_count, sizeof(general_string_buff) * actions_count);
	SendMessage(hwnd_switch_act, CB_INITSTORAGE, actions_count, sizeof(general_string_buff) * actions_count);

	//fill text for action combo's
	for (int i = 0; i < actions_count; i++) {
		//
		LoadString(phinstDLL, P2_ACTION_UID[static_cast<int>(actions[i])], general_string_buff, _countof(general_string_buff));
		SendMessage(hwnd_axis_act1, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
		SendMessage(hwnd_axis_act2, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
		SendMessage(hwnd_butt_act, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
		SendMessage(hwnd_switch_act, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
	}

	SetWindowRedraw(hwnd_axis_act1, TRUE);
	SetWindowRedraw(hwnd_axis_act2, TRUE);
	SetWindowRedraw(hwnd_butt_act, TRUE);
	SetWindowRedraw(hwnd_switch_act, TRUE);

	//Debug_Info("_countof(P2_ACTION_UID) %d", _countof(P2_ACTION_UID));
	SendMessage(hwnd_axis_act1, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	SendMessage(hwnd_axis_act2, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
}


//__________________________________________________________________________________________________________
static INT_PTR CALLBACK DialogProc_Config_Joy_Control(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_INITDIALOG: {

		InitCommonControls();

		hWin_Config_Joy_Control = hwndDlg;

		JoyConfig_Refresh_Axes(TRUE, TRUE);
		JoyConfig_Refresh_Buttons(TRUE);
		JoyConfig_Refresh_Switches(TRUE, TRUE);

		JoyConfig_Update_Profile_Vars();
		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case IDC_COMBO_SELECT_AXIS:
			if (HIWORD(wParam) == CBN_SELCHANGE)
				JoyConfig_Refresh_Axes(FALSE, TRUE);

			return TRUE;
		case IDC_COMBO_SELECT_AXIS_TYPE:
			if (HIWORD(wParam) == CBN_SELCHANGE)
				JoyConfig_Axis_SetType();
			return TRUE;
		case IDC_CHECK_SELECTED_AXIS_SIGN:
			if (HIWORD(wParam) == BN_CLICKED)
				JoyConfig_Axis_SetSign();
			return TRUE;
		case IDC_COMBO_SELECT_AXIS_BUTTON1:
		case IDC_COMBO_SELECT_AXIS_BUTTON2:
			if (HIWORD(wParam) == CBN_SELCHANGE)
				JoyConfig_Axis_SetButton(LOWORD(wParam));
			return TRUE;
		case IDC_BUTTON_CALIBRATE_AXIS:
			JoyConfig_Axis_Calibrate();
			return TRUE;
		case IDC_BUTTON_CENTRE_AXIS: {
			JoyConfig_Axis_Centre();
			return TRUE;
		}

		case IDC_COMBO_SELECT_BUTTONS:
			if (HIWORD(wParam) == CBN_SELCHANGE)
				JoyConfig_Refresh_Buttons(FALSE);
			return TRUE;

		case IDC_COMBO_SELECT_BUTTON_ACTION:
			if (HIWORD(wParam) == CBN_SELCHANGE)
				JoyConfig_Button_SetButton();
			return TRUE;


		case IDC_COMBO_SELECT_SWITCHES:
			if (HIWORD(wParam) == CBN_SELCHANGE)
				JoyConfig_Refresh_Switches(FALSE, TRUE);
			return TRUE;
		case IDC_COMBO_SELECT_SWITCH_POS:
			if (HIWORD(wParam) == CBN_SELCHANGE)
				JoyConfig_Refresh_Switches(FALSE, FALSE);
			return TRUE;
		case IDC_COMBO_SELECT_SWITCH_ACTION:
			if (HIWORD(wParam) == CBN_SELCHANGE)
				JoyConfig_Switch_SetButton();
			return TRUE;

		default:
			break;
		}
		break;
	case WM_DESTROY: {
		hWin_Config_Joy_Control = nullptr;
		return FALSE;
	}
	default:
		return FALSE;
		break;
	}

	return TRUE;
}


//_________________________________________________________________________________________________
static INT_PTR CALLBACK DialogProc_JoyConfig(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_INITDIALOG: {

		InitCommonControls();

		hWin_Config_Joy = hwndDlg;

		JoyConfig_Refresh_Presets();
		JoyConfig_Refresh_Enabled();

		HWND hwnd_sub = nullptr;
		RECT rc{};
		POINT pt{};

		//set size of roll box
		hwnd_sub = GetDlgItem(hwndDlg, IDC_STATIC_ROLL_BOX);
		GetWindowRect(hwnd_sub, &rc);
		pt = { rc.left, rc.top };
		ScreenToClient(hwndDlg, &pt);
		MoveWindow(hwnd_sub, pt.x, pt.y, rc.right - rc.left, 9 + 2, TRUE);
		//set size of throttle box
		hwnd_sub = GetDlgItem(hwndDlg, IDC_STATIC_THROTTLE_BOX);
		GetWindowRect(hwnd_sub, &rc);
		pt = { rc.left, rc.top };
		ScreenToClient(hwndDlg, &pt);
		MoveWindow(hwnd_sub, pt.x, pt.y, 9 + 2, rc.bottom - rc.top, TRUE);



		INITCOMMONCONTROLSEX iccex{ 0 };
		//initialize common controls.
		iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		iccex.dwICC = ICC_TAB_CLASSES;
		InitCommonControlsEx(&iccex);

		TCITEM tie{ 0 };

		HWND hwndTab = GetDlgItem(hwndDlg, IDC_TAB_JOY_CONTROL);
		//add a tab for each of the child dialog boxes.
		tie.mask = TCIF_TEXT | TCIF_IMAGE;
		tie.iImage = -1;

		LoadString(phinstDLL, IDS_TAB_GUI, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, input_protype_tab_list[static_cast<int>(PROFILE_TYPE::GUI)], &tie);

		LoadString(phinstDLL, IDS_TAB_SPACE, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, input_protype_tab_list[static_cast<int>(PROFILE_TYPE::Space)], &tie);

		LoadString(phinstDLL, IDS_TAB_REMAP_1, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, input_protype_tab_list[static_cast<int>(PROFILE_TYPE::ReMap_1)], &tie);
		LoadString(phinstDLL, IDS_TAB_REMAP_2, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, input_protype_tab_list[static_cast<int>(PROFILE_TYPE::ReMap_2)], &tie);
		LoadString(phinstDLL, IDS_TAB_REMAP_3, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, input_protype_tab_list[static_cast<int>(PROFILE_TYPE::ReMap_3)], &tie);

		//create tab windows.
		hWin_Config_Joy_Control = CreateDialogParam(phinstDLL, MAKEINTRESOURCE(IDD_DIALOG_JOY_CONTROLS), hwndDlg, &DialogProc_Config_Joy_Control, 0);

		//set the position of tab windows, adjusting for the height of the tabs.
		RECT rcTab;
		GetWindowRect(hwndTab, &rcTab);
		MapWindowPoints(HWND_DESKTOP, hwndDlg, (LPPOINT)&rcTab, 2);
		TabCtrl_AdjustRect(hwndTab, FALSE, &rcTab);
		
		SetWindowPos(hWin_Config_Joy_Control, nullptr, rcTab.left, rcTab.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		TabCtrl_SetCurFocus(hwndTab, input_protype_tab_list[static_cast<int>(current_pro_type)]);
		ShowWindow(hWin_Config_Joy_Control, SW_SHOW);

		hwnd_sub = GetDlgItem(hwndDlg, IDC_COMBO_DEAD_ZONE);
		//wc axes have 16 degrees of movement from centre, mark deadzone levels as percentages for easier reading. 6.25% == 1/16 of axis from centre.
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"0%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"3.125%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"6.25%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"9.375%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"12.5%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"15.625%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"18.75%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"21.875%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"25%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"28.125%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"31.25%");

		SendMessage(hwnd_sub, CB_SETCURSEL, (WPARAM)Joysticks.Deadzone_Level(), (LPARAM)0);

		return TRUE;
	}
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case TCN_SELCHANGE: {
			HWND hwndTab = GetDlgItem(hwndDlg, IDC_TAB_JOY_CONTROL);
			int tabNum = TabCtrl_GetCurSel(hwndTab);
			if (tabNum == input_protype_tab_list[static_cast<int>(PROFILE_TYPE::GUI)])
				current_pro_type = PROFILE_TYPE::GUI;
			else if (tabNum == input_protype_tab_list[static_cast<int>(PROFILE_TYPE::Space)])
				current_pro_type = PROFILE_TYPE::Space;
			else if (tabNum == input_protype_tab_list[static_cast<int>(PROFILE_TYPE::ReMap_1)])
				current_pro_type = PROFILE_TYPE::ReMap_1;
			else if (tabNum == input_protype_tab_list[static_cast<int>(PROFILE_TYPE::ReMap_2)])
				current_pro_type = PROFILE_TYPE::ReMap_2;
			else if (tabNum == input_protype_tab_list[static_cast<int>(PROFILE_TYPE::ReMap_3)])
				current_pro_type = PROFILE_TYPE::ReMap_3;
			JoyConfig_Update_Profile_Vars();
			JoyConfig_Refresh_Axes(FALSE, TRUE);
			JoyConfig_Refresh_Buttons(FALSE);
			JoyConfig_Refresh_Switches(FALSE, TRUE);
			break;
		}
		default:
			break;
		}
		break;
	case WM_MOVE: {
		HWND hwndTab = GetDlgItem(hwndDlg, IDC_TAB_JOY_CONTROL);
		//move tab windows with the main window, adjusting for the height of the tabs.
		RECT rcTab;
		GetWindowRect(hwndTab, &rcTab);
		MapWindowPoints(HWND_DESKTOP, hwndDlg, (LPPOINT)&rcTab, 2);
		TabCtrl_AdjustRect(hwndTab, FALSE, &rcTab);

		SetWindowPos(hWin_Config_Joy_Control, nullptr, rcTab.left, rcTab.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_COMBO_JOY_SELECT:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				JoyConfig_Refresh_Presets();
				JoyConfig_Refresh_Enabled();
				JoyConfig_Refresh_Axes(TRUE, TRUE);
				JoyConfig_Refresh_Buttons(TRUE);
				JoyConfig_Refresh_Switches(TRUE, TRUE);
			}
			return TRUE;
		case IDC_COMBO_JOY_PRESETS:
			if (HIWORD(wParam) == CBN_SELCHANGE)
				if (JoyConfig_Preset_Set()) {
					JoyConfig_Refresh_Enabled();
					JoyConfig_Refresh_Axes(TRUE, TRUE);
					JoyConfig_Refresh_Buttons(TRUE);
					JoyConfig_Refresh_Switches(TRUE, TRUE);
				}
			return TRUE;
		case IDC_BUTTON_SAVE_PRESET:
			if (JoyConfig_Preset_Save())
				JoyConfig_Refresh_Presets();
			return TRUE;
		case IDC_CHECK_JOY_ENABLE:
			if (HIWORD(wParam) == BN_CLICKED)
				JoyConfig_Update_Enabled();
			return TRUE;

		case IDC_COMBO_DEAD_ZONE:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				int deadzone = (int)(SendMessage(GetDlgItem(hwndDlg, IDC_COMBO_DEAD_ZONE), CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
				Joysticks.Set_Deadzone_Level(deadzone);
			}
			return TRUE;
		case IDC_BUTTON_CENTRE_ALL:
			Joysticks.Centre_All();
			return TRUE;

		default:
			break;
		}
		break;
	case WM_DESTROY: {
		hWin_Config_Joy = nullptr;
		return FALSE;
	}
	default:
		return FALSE;
		break;
	}

	return TRUE;
}


//_____________________________________________________________________________________________________
static INT_PTR CALLBACK DialogProc_JoyConfig_Off(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_INITDIALOG: {

		InitCommonControls();

		hWin_Config_Joy_Off = hwndDlg;
		return TRUE;
	}
	case WM_DESTROY: {
		hWin_Config_Joy_Off = nullptr;
		return FALSE;
	}
	default:
		return FALSE;
		break;
	}

	return TRUE;
}


//__________________________________________________________________________
BOOL JoyConfig_Refresh_CurrentAction_Mouse(P2_ACTIONS action, BOOL activate) {

	if (!hWin_Config_Mouse)
		return FALSE;

	HWND hwnd_sub = GetDlgItem(hWin_Config_Mouse, IDC_STATIC_CURRENT_ACTION);
	if (!hwnd_sub)
		return FALSE;

	UINT UID = P2_ACTION_UID[static_cast<int>(P2_ACTIONS::None)];
	if (activate)
		UID = P2_ACTION_UID[static_cast<int>(action)];
	LoadString(phinstDLL, UID, general_string_buff, _countof(general_string_buff));
	SendMessage(hwnd_sub, (UINT)WM_SETTEXT, (WPARAM)0, (LPARAM)general_string_buff);
	
	return TRUE;
}


//____________________________________________________
static void JoyConfig_Refresh_Mouse_Display(HWND hwnd) {

	RedrawWindow(GetDlgItem(hwnd, IDC_STATIC_B1), nullptr, nullptr, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(hwnd, IDC_STATIC_B2), nullptr, nullptr, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(hwnd, IDC_STATIC_B3), nullptr, nullptr, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(hwnd, IDC_STATIC_B4), nullptr, nullptr, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(hwnd, IDC_STATIC_B5), nullptr, nullptr, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(hwnd, IDC_STATIC_SCROLL_UP), nullptr, nullptr, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(hwnd, IDC_STATIC_SCROLL_DN), nullptr, nullptr, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(hwnd, IDC_STATIC_SCROLL_LEFT), nullptr, nullptr, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(hwnd, IDC_STATIC_SCROLL_RIGHT), nullptr, nullptr, RDW_INVALIDATE);
}


//___________________________________________
static void MouseConfig_Update_Profile_Vars() {

	if (!hWin_Config_Mouse)
		return;
	if (!hWin_Config_Mouse_Control)
		return;

	//fill action selection lists.
	HWND hwnd_button_actions = GetDlgItem(hWin_Config_Mouse_Control, IDC_COMBO_SELECT_BUTTON_ACTION);
	SendMessage(hwnd_button_actions, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	HWND hwnd_wheel_up_actions = GetDlgItem(hWin_Config_Mouse_Control, IDC_COMBO_WHEEL_UP_ACTION);
	SendMessage(hwnd_wheel_up_actions, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	HWND hwnd_wheel_down_actions = GetDlgItem(hWin_Config_Mouse_Control, IDC_COMBO_WHEEL_DOWN_ACTION);
	SendMessage(hwnd_wheel_down_actions, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	HWND hwnd_wheel_left_actions = GetDlgItem(hWin_Config_Mouse_Control, IDC_COMBO_WHEEL_LEFT_ACTION);
	SendMessage(hwnd_wheel_left_actions, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	HWND hwnd_wheel_right_actions = GetDlgItem(hWin_Config_Mouse_Control, IDC_COMBO_WHEEL_RIGHT_ACTION);
	SendMessage(hwnd_wheel_right_actions, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);

	P2_ACTIONS* actions = actions_space_mouse;
	int actions_count = _countof(actions_space_mouse);
	if (current_pro_type == PROFILE_TYPE::GUI) {
		actions = actions_gui_mouse;
		actions_count = _countof(actions_gui_mouse);
	}

	//speed up combobox initialization by preventing drawing while adding strings.
	SetWindowRedraw(hwnd_button_actions, FALSE);
	SetWindowRedraw(hwnd_wheel_up_actions, FALSE);
	SetWindowRedraw(hwnd_wheel_down_actions, FALSE);
	SetWindowRedraw(hwnd_wheel_left_actions, FALSE);
	SetWindowRedraw(hwnd_wheel_right_actions, FALSE);

	//speed up combobox initialization by setting the count and estimated memory usage first. 
	SendMessage(hwnd_button_actions, CB_INITSTORAGE, actions_count, sizeof(general_string_buff) * actions_count);
	SendMessage(hwnd_wheel_up_actions, CB_INITSTORAGE, actions_count, sizeof(general_string_buff) * actions_count);
	SendMessage(hwnd_wheel_down_actions, CB_INITSTORAGE, actions_count, sizeof(general_string_buff) * actions_count);
	SendMessage(hwnd_wheel_left_actions, CB_INITSTORAGE, actions_count, sizeof(general_string_buff) * actions_count);
	SendMessage(hwnd_wheel_right_actions, CB_INITSTORAGE, actions_count, sizeof(general_string_buff) * actions_count);

	for (int i = 0; i < actions_count; i++) {
		LoadString(phinstDLL, P2_ACTION_UID[static_cast<int>(actions[i])], general_string_buff, _countof(general_string_buff));
		SendMessage(hwnd_button_actions, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
		SendMessage(hwnd_wheel_up_actions, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
		SendMessage(hwnd_wheel_down_actions, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
		SendMessage(hwnd_wheel_left_actions, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
		SendMessage(hwnd_wheel_right_actions, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
	}

	SetWindowRedraw(hwnd_button_actions, TRUE);
	SetWindowRedraw(hwnd_wheel_up_actions, TRUE);
	SetWindowRedraw(hwnd_wheel_down_actions, TRUE);
	SetWindowRedraw(hwnd_wheel_left_actions, TRUE);
	SetWindowRedraw(hwnd_wheel_right_actions, TRUE);

	HWND hwnd_button = GetDlgItem(hWin_Config_Mouse_Control, IDC_COMBO_SELECT_BUTTONS);
	int button_selected = (int)(SendMessage(hwnd_button, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
	int action_pos = Get_Action_Position_Mouse(Mouse.GetAction_Button(button_selected));
	if (action_pos >= 0)
		SendMessage(hwnd_button_actions, CB_SETCURSEL, (WPARAM)action_pos, (LPARAM)0);

	action_pos = Get_Action_Position_Mouse(Mouse.GetAction_Wheel_Up());
	if (action_pos >= 0)
		SendMessage(hwnd_wheel_up_actions, CB_SETCURSEL, (WPARAM)action_pos, (LPARAM)0);
	action_pos = Get_Action_Position_Mouse(Mouse.GetAction_Wheel_Down());
	if (action_pos >= 0)
		SendMessage(hwnd_wheel_down_actions, CB_SETCURSEL, (WPARAM)action_pos, (LPARAM)0);
	action_pos = Get_Action_Position_Mouse(Mouse.GetAction_Wheel_Left());
	if (action_pos >= 0)
		SendMessage(hwnd_wheel_left_actions, CB_SETCURSEL, (WPARAM)action_pos, (LPARAM)0);
	action_pos = Get_Action_Position_Mouse(Mouse.GetAction_Wheel_Right());
	if (action_pos >= 0)
		SendMessage(hwnd_wheel_right_actions, CB_SETCURSEL, (WPARAM)action_pos, (LPARAM)0);


}


//____________________________________________________________________________________________________________
static INT_PTR CALLBACK DialogProc_Config_Mouse_Control(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
	case WM_INITDIALOG: {

		InitCommonControls();

		hWin_Config_Mouse_Control = hwndDlg;

		HWND hwnd_sub = nullptr;

		//setup button selection combo.
		hwnd_sub = GetDlgItem(hWin_Config_Mouse_Control, IDC_COMBO_SELECT_BUTTONS);
		wchar_t* msg = new wchar_t[12];
		LoadString(phinstDLL, IDS_BUTTON, general_string_buff, _countof(general_string_buff));

		for (int i = 0; i < NUM_MOUSE_BUTTONS; i++) {
			swprintf_s(msg, 12, L"%s %d", general_string_buff, i + 1);
			SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)msg);
		}
		delete[] msg;
		SendMessage(hwnd_sub, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

		MouseConfig_Update_Profile_Vars();

		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_COMBO_SELECT_BUTTONS:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				HWND hwnd_button = GetDlgItem(hwndDlg, IDC_COMBO_SELECT_BUTTONS);
				int button_selected = (int)(SendMessage(hwnd_button, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
				HWND hwnd_actions = GetDlgItem(hwndDlg, IDC_COMBO_SELECT_BUTTON_ACTION);
				int action_pos = Get_Action_Position_Mouse(Mouse.GetAction_Button(button_selected));
				if (action_pos >= 0)
					SendMessage(hwnd_actions, CB_SETCURSEL, (WPARAM)action_pos, (LPARAM)0);
			}
			return TRUE;
		case IDC_COMBO_SELECT_BUTTON_ACTION:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				HWND hwnd_button = GetDlgItem(hwndDlg, IDC_COMBO_SELECT_BUTTONS);
				int button_selected = (int)(SendMessage(hwnd_button, CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
				HWND hwnd_action = GetDlgItem(hwndDlg, IDC_COMBO_SELECT_BUTTON_ACTION);
				P2_ACTIONS action_selected = Get_Action_Mouse((SendMessage(hwnd_action, CB_GETCURSEL, (WPARAM)0, (LPARAM)0)));
				if (action_selected != P2_ACTIONS::End)
					Mouse.SetAction_Button(button_selected, action_selected);
			}
			return TRUE;
		case IDC_COMBO_WHEEL_UP_ACTION:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				HWND hwnd_action = GetDlgItem(hwndDlg, IDC_COMBO_WHEEL_UP_ACTION);
				P2_ACTIONS action_selected = Get_Action_Mouse((SendMessage(hwnd_action, CB_GETCURSEL, (WPARAM)0, (LPARAM)0)));
				if (action_selected != P2_ACTIONS::End)
					Mouse.SetAction_Wheel_Up(action_selected);
			}
			return TRUE;
		case IDC_COMBO_WHEEL_DOWN_ACTION:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				HWND hwnd_action = GetDlgItem(hwndDlg, IDC_COMBO_WHEEL_DOWN_ACTION);
				P2_ACTIONS action_selected = Get_Action_Mouse((SendMessage(hwnd_action, CB_GETCURSEL, (WPARAM)0, (LPARAM)0)));
				if (action_selected != P2_ACTIONS::End)
					Mouse.SetAction_Wheel_Down(action_selected);
			}
			return TRUE;
		case IDC_COMBO_WHEEL_LEFT_ACTION:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				HWND hwnd_action = GetDlgItem(hwndDlg, IDC_COMBO_WHEEL_LEFT_ACTION);
				P2_ACTIONS action_selected = Get_Action_Mouse((SendMessage(hwnd_action, CB_GETCURSEL, (WPARAM)0, (LPARAM)0)));
				if (action_selected != P2_ACTIONS::End)
					Mouse.SetAction_Wheel_Left(action_selected);
			}
			return TRUE;
		case IDC_COMBO_WHEEL_RIGHT_ACTION:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				HWND hwnd_action = GetDlgItem(hwndDlg, IDC_COMBO_WHEEL_RIGHT_ACTION);
				P2_ACTIONS action_selected = Get_Action_Mouse((SendMessage(hwnd_action, CB_GETCURSEL, (WPARAM)0, (LPARAM)0)));
				if (action_selected != P2_ACTIONS::End)
					Mouse.SetAction_Wheel_Right(action_selected);
			}
			return TRUE;
		default:
			break;
		}
		break;
	case WM_DESTROY: {
		hWin_Config_Mouse_Control = nullptr;
		return FALSE;
	}
	default:
		return FALSE;
		break;
	}

	return TRUE;
}


//____________________________________________________________________________________________________
static INT_PTR CALLBACK DialogProc_Config_Mouse(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	static HBRUSH hbrush_colour_box = nullptr;
	static DWORD button_states = 0;

	switch (uMsg) {
	case WM_INITDIALOG: {
		hWin_Config_Mouse = hwndDlg;
		InitCommonControls();
		
		HWND hwnd_sub = nullptr;

		hwnd_sub = GetDlgItem(hwndDlg, IDC_COMBO_DEAD_ZONE);

		//wc axes have 16 degrees of movement from centre, mark deadzone levels as percentages for easier reading. 6.25% == 1/16 of axis from centre.
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"0%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"3.125%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"6.25%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"9.375%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"12.5%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"15.625%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"18.75%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"21.875%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"25%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"28.125%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"31.25%");

		SendMessage(hwnd_sub, CB_SETCURSEL, (WPARAM)Mouse.Deadzone_Level(), (LPARAM)0);
		
		hwnd_sub = GetDlgItem(hwndDlg, IDC_COMBO_AXIS_RANGE);

		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"10%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"20%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"30%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"40%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"50%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"60%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"70%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"80%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"90%");
		SendMessage(hwnd_sub, CB_ADDSTRING, (WPARAM)0, (LPARAM)L"100%");

		SendMessage(hwnd_sub, CB_SETCURSEL, (WPARAM)Mouse.Axis_Limit_Percentage() / 10 - 1, (LPARAM)0);

		hwnd_sub = GetDlgItem(hwndDlg, IDC_CHECK_INVERT_Y_AXIS);
		DWORD checked = BST_UNCHECKED;
		if (Mouse.Is_Y_Axis_Inverted())
			checked = BST_CHECKED;
		SendMessage(hwnd_sub, BM_SETCHECK, (WPARAM)checked, (LPARAM)0);

		INITCOMMONCONTROLSEX iccex{ 0 };
		//initialize common controls.
		iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		iccex.dwICC = ICC_TAB_CLASSES;
		InitCommonControlsEx(&iccex);

		TCITEM tie{ 0 };

		HWND hwndTab = GetDlgItem(hwndDlg, IDC_TAB_MOUSE_CONTROL);
		//add a tab for each of the child dialog boxes.
		tie.mask = TCIF_TEXT | TCIF_IMAGE;
		tie.iImage = -1;

		LoadString(phinstDLL, IDS_TAB_GUI, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, input_protype_tab_list[static_cast<int>(PROFILE_TYPE::GUI)], &tie);

		LoadString(phinstDLL, IDS_TAB_SPACE, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, input_protype_tab_list[static_cast<int>(PROFILE_TYPE::Space)], &tie);

		LoadString(phinstDLL, IDS_TAB_REMAP_1, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, input_protype_tab_list[static_cast<int>(PROFILE_TYPE::ReMap_1)], &tie);
		LoadString(phinstDLL, IDS_TAB_REMAP_2, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, input_protype_tab_list[static_cast<int>(PROFILE_TYPE::ReMap_2)], &tie);
		LoadString(phinstDLL, IDS_TAB_REMAP_3, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, input_protype_tab_list[static_cast<int>(PROFILE_TYPE::ReMap_3)], &tie);

		//create tab windows.
		hWin_Config_Mouse_Control = CreateDialogParam(phinstDLL, MAKEINTRESOURCE(IDD_DIALOG_MOUSE_CONTROLS), hwndDlg, &DialogProc_Config_Mouse_Control, 0);

		//set the position of tab windows, adjusting for the height of the tabs.
		RECT rcTab;
		GetWindowRect(hwndTab, &rcTab);
		MapWindowPoints(HWND_DESKTOP, hwndDlg, (LPPOINT)&rcTab, 2);
		TabCtrl_AdjustRect(hwndTab, FALSE, &rcTab);

		SetWindowPos(hWin_Config_Mouse_Control, nullptr, rcTab.left, rcTab.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		TabCtrl_SetCurFocus(hwndTab, input_protype_tab_list[static_cast<int>(current_pro_type)]);
		ShowWindow(hWin_Config_Mouse_Control, SW_SHOW);

		break;
	}
	case WM_CTLCOLORSTATIC: {
		//highlight pressed buttons.
		if (!hbrush_colour_box)
			hbrush_colour_box = CreateSolidBrush(RGB(128, 128, 128));

		if (((HWND)lParam == GetDlgItem(hwndDlg, IDC_STATIC_B1) && (button_states & (1 << 0))) ||
			((HWND)lParam == GetDlgItem(hwndDlg, IDC_STATIC_B2) && (button_states & (1 << 1))) ||
			((HWND)lParam == GetDlgItem(hwndDlg, IDC_STATIC_B3) && (button_states & (1 << 2))) ||
			((HWND)lParam == GetDlgItem(hwndDlg, IDC_STATIC_B4) && (button_states & (1 << 3))) ||
			((HWND)lParam == GetDlgItem(hwndDlg, IDC_STATIC_B5) && (button_states & (1 << 4))) ||
			((HWND)lParam == GetDlgItem(hwndDlg, IDC_STATIC_SCROLL_UP) && (button_states & (1 << 5))) ||
			((HWND)lParam == GetDlgItem(hwndDlg, IDC_STATIC_SCROLL_DN) && (button_states & (1 << 6))) ||
			((HWND)lParam == GetDlgItem(hwndDlg, IDC_STATIC_SCROLL_LEFT) && (button_states & (1 << 7))) ||
			((HWND)lParam == GetDlgItem(hwndDlg, IDC_STATIC_SCROLL_RIGHT) && (button_states & (1 << 8)))) {
			HDC hdcStatic = (HDC)wParam;
			SetTextColor(hdcStatic, RGB(255, 255, 255));
			SetBkColor(hdcStatic, RGB(128, 128, 128));
			return (INT_PTR)hbrush_colour_box;
		}
		return FALSE;
	}
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case TCN_SELCHANGE: {
			HWND hwndTab = GetDlgItem(hwndDlg, IDC_TAB_MOUSE_CONTROL);
			int tabNum = TabCtrl_GetCurSel(hwndTab);
			if (tabNum == input_protype_tab_list[static_cast<int>(PROFILE_TYPE::GUI)])
				current_pro_type = PROFILE_TYPE::GUI;
			else if (tabNum == input_protype_tab_list[static_cast<int>(PROFILE_TYPE::Space)])
				current_pro_type = PROFILE_TYPE::Space;
			else if (tabNum == input_protype_tab_list[static_cast<int>(PROFILE_TYPE::ReMap_1)])
				current_pro_type = PROFILE_TYPE::ReMap_1;
			else if (tabNum == input_protype_tab_list[static_cast<int>(PROFILE_TYPE::ReMap_2)])
				current_pro_type = PROFILE_TYPE::ReMap_2;
			else if (tabNum == input_protype_tab_list[static_cast<int>(PROFILE_TYPE::ReMap_3)])
				current_pro_type = PROFILE_TYPE::ReMap_3;
			MouseConfig_Update_Profile_Vars();
			break;
		}
		default:
			break;
		}
		break;
	case WM_MOVE: {
		HWND hwndTab = GetDlgItem(hwndDlg, IDC_TAB_MOUSE_CONTROL);
		//move tab windows with the main window, adjusting for the height of the tabs.
		RECT rcTab;
		GetWindowRect(hwndTab, &rcTab);
		MapWindowPoints(HWND_DESKTOP, hwndDlg, (LPPOINT)&rcTab, 2);
		TabCtrl_AdjustRect(hwndTab, FALSE, &rcTab);
		SetWindowPos(hWin_Config_Mouse_Control, nullptr, rcTab.left, rcTab.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_COMBO_DEAD_ZONE:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				int deadzone = (int)(SendMessage(GetDlgItem(hwndDlg, IDC_COMBO_DEAD_ZONE), CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
				Mouse.Set_Deadzone_Level(deadzone);
			}
			return TRUE;
		case IDC_COMBO_AXIS_RANGE:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				int percent = ((int)(SendMessage(GetDlgItem(hwndDlg, IDC_COMBO_AXIS_RANGE), CB_GETCURSEL, (WPARAM)0, (LPARAM)0)) + 1) * 10;
				Mouse.Set_Axis_Limit_Pecentage(percent);
			}
			return TRUE;
		case IDC_CHECK_INVERT_Y_AXIS:
			if (HIWORD(wParam) == BN_CLICKED) {
				DWORD button_state = (int)(SendMessage((HWND)lParam, BM_GETCHECK, (WPARAM)0, (LPARAM)0));
				bool is_y_axis_inverted = false;
				if (button_state & BST_CHECKED)
					is_y_axis_inverted = true;
				Mouse.Invert_Y_Axis(is_y_axis_inverted);
			}
			return TRUE;
		default:
			break;
		}
		break;
	case WM_DESTROY: {
		if (hbrush_colour_box)
			DeleteObject(hbrush_colour_box);
		hbrush_colour_box = nullptr;

		hWin_Config_Mouse = nullptr;
		return FALSE;
	}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP: {
		Mouse.Update_Buttons(wParam);

		int key_state = GET_KEYSTATE_WPARAM(wParam);
		if (key_state & MK_LBUTTON)
			button_states |= (1 << 0);
		else
			button_states &= ~(1 << 0);
		if (key_state & MK_RBUTTON)
			button_states |= (1 << 1);
		else
			button_states &= ~(1 << 1);
		if (key_state & MK_MBUTTON)
			button_states |= (1 << 2);
		else
			button_states &= ~(1 << 2);
		if (key_state & MK_XBUTTON1)
			button_states |= (1 << 3);
		else
			button_states &= ~(1 << 3);
		if (key_state & MK_XBUTTON2)
			button_states |= (1 << 4);
		else
			button_states &= ~(1 << 4);

		//clear scroll wheel states
		button_states &= ~(1 << 5);
		button_states &= ~(1 << 6);
		button_states &= ~(1 << 7);
		button_states &= ~(1 << 8);

		JoyConfig_Refresh_Mouse_Display(hwndDlg);
		break;
	}
	case WM_MOUSEWHEEL: {
		Mouse.Update_Wheel_Vertical(wParam);
		short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (zDelta > 0) {
			button_states |= (1 << 5);
			button_states &= ~(1 << 6);
		}
		else if (zDelta < 0) {
			button_states |= (1 << 6);
			button_states &= ~(1 << 5);
		}
		JoyConfig_Refresh_Mouse_Display(hwndDlg);
		break;
	}
	case WM_MOUSEHWHEEL: {
		Mouse.Update_Wheel_Horizontal(wParam);
		short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (zDelta > 0) {
			button_states |= (1 << 7);
			button_states &= ~(1 << 8);
		}
		else if (zDelta < 0) {
			button_states |= (1 << 8);
			button_states &= ~(1 << 7);
		}
		JoyConfig_Refresh_Mouse_Display(hwndDlg);
		break;
	}
	default:
		return FALSE;
		break;
	}
	return TRUE;
}


const UINT P2_KEYS_UID[]{
	IDS_NONE,
	IDS_KEYS001,
	IDS_KEYS002,
	IDS_KEYS003,
	IDS_KEYS004,
	IDS_KEYS005,
	IDS_KEYS006,
	IDS_KEYS007,
	IDS_KEYS008,
	IDS_KEYS009,
	IDS_KEYS010,
	IDS_KEYS011,
	IDS_KEYS012,
	IDS_KEYS013,
	IDS_KEYS014,
	IDS_KEYS015,
	IDS_KEYS016,
	IDS_KEYS017,
	IDS_KEYS018,
	IDS_KEYS019,
	IDS_KEYS020,
	IDS_KEYS021,
	IDS_KEYS022,
	IDS_KEYS023,
	IDS_KEYS024,
	IDS_KEYS025,
	IDS_KEYS026,
	IDS_KEYS027,
	IDS_KEYS028,
	IDS_KEYS029,
	IDS_KEYS030,
	IDS_KEYS031,
	IDS_KEYS032,
	IDS_KEYS033,
	IDS_KEYS034,
	IDS_KEYS035,
	IDS_KEYS036,
	IDS_KEYS037,
	IDS_KEYS038,
	IDS_KEYS039,
};

BYTE key_map[]{
	0,
	'A',
	'B',
	'C',
	'D',
	'E',
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'S',
	'T',
	'U',
	'V',
	'W',
	'X',
	'Y',
	'Z',
	VK_OEM_PLUS,// '+='
	VK_OEM_MINUS,// '-_'
	VK_OEM_1,// ':;'
	VK_OEM_2,// '?/'
	VK_OEM_3,// '~`'
	VK_OEM_4,// '['
	VK_OEM_5,// '|\'
	VK_OEM_6,// ']'
	VK_OEM_7,// '"''
	VK_TAB,
	VK_RETURN,
	VK_SPACE,
	VK_BACK,
};

P2_ACTIONS actions_keys_page01[]{
	P2_ACTIONS::None,
	P2_ACTIONS::Match_Target_Speed,
	P2_ACTIONS::Speed_Increase,
	P2_ACTIONS::Speed_Decrease,
	P2_ACTIONS::Speed_Zero,
	P2_ACTIONS::Speed_Max,
	P2_ACTIONS::Jump,
	P2_ACTIONS::Select_Target_In_Crosshairs,
	P2_ACTIONS::Cycle_Targets_Forward,
	P2_ACTIONS::Cycle_Targets_Back,
	P2_ACTIONS::Target_Nearest_Hostile,
	P2_ACTIONS::Target_Nearest_Friendly,
	P2_ACTIONS::Cycle_Radar_Ranges,
	P2_ACTIONS::Toggle_Radar_Type,
	P2_ACTIONS::Targeting_Off,
	P2_ACTIONS::Toggle_Full_No_Guns,
	P2_ACTIONS::Fire_Guns,
	P2_ACTIONS::Fire_Missile,
	P2_ACTIONS::Drop_Mine_Decoy,
	P2_ACTIONS::Afterburner,
	P2_ACTIONS::Rotation_Speed_Key,
};

P2_ACTIONS actions_keys_page02[]{
	P2_ACTIONS::None,
	P2_ACTIONS::Activate_BSE,
	P2_ACTIONS::Toggle_SOS,
	P2_ACTIONS::Warp_Shields,
	P2_ACTIONS::Drop_Nuke_em,
	P2_ACTIONS::Tractor_Targeted_Object,
	P2_ACTIONS::Drop_Tractored_Object,
	P2_ACTIONS::VDW_Comms,
	P2_ACTIONS::VDW_Damage,
	P2_ACTIONS::VDW_Missiles,
	P2_ACTIONS::VDW_Guns,
	P2_ACTIONS::VDW_Mines_Decoys,
	P2_ACTIONS::Cycle_Weapons_in_VDW,
	P2_ACTIONS::Select_Deselect_Weapon_in_VDW,
	P2_ACTIONS::Game_Options,
	P2_ACTIONS::Nav_Map,
	P2_ACTIONS::Electronic_Diary,
	P2_ACTIONS::Hotkey_List,
	P2_ACTIONS::Exit_Game,
};

const UINT P2_KEY_MODS_UID[]{
	IDS_NONE,
	IDS_MODS001,
	IDS_MODS002,
	IDS_MODS003,

};

BYTE key_mods[]{
	0,
	VK_SHIFT,
	VK_CONTROL,
	VK_MENU,
};


//___________________________________________
static int Find_Key_Value_Number(BYTE vk_key) {

	for (int i = 0; i < _countof(key_map); i++) {
		if (vk_key == key_map[i])
			return i;
	}
	return 0;
}


//_________________________________________________
static int Find_Mod_Key_Value_Number(BYTE mod_flag) {
	if (mod_flag & 1)
		return 1;
	else if (mod_flag & 2)
		return 2;
	else if (mod_flag & 4)
		return 3;
	return 0;
}


UINT KEY_COMBO_UID[]{
	0,
		IDC_COMBO_KEY_01,
		IDC_COMBO_KEY_02,
		IDC_COMBO_KEY_03,
		IDC_COMBO_KEY_04,
		IDC_COMBO_KEY_05,
		IDC_COMBO_KEY_06,
		IDC_COMBO_KEY_07,
		IDC_COMBO_KEY_08,
		IDC_COMBO_KEY_09,
		IDC_COMBO_KEY_10,
		IDC_COMBO_KEY_11,
		IDC_COMBO_KEY_12,
		IDC_COMBO_KEY_13,
		IDC_COMBO_KEY_14,
		IDC_COMBO_KEY_15,
		IDC_COMBO_KEY_16,
		IDC_COMBO_KEY_17,
		IDC_COMBO_KEY_18,
		IDC_COMBO_KEY_19,
};


//________________________________________
static int Find_Key_Combo_Number(UINT uid) {

	for (int i = 0; i < _countof(KEY_COMBO_UID); i++) {
		if (uid == KEY_COMBO_UID[i])
			return i;
	}
	return -1;
}


UINT MOD_KEY_COMBO_UID[]{
	0,
		IDC_COMBO_MOD_01,
		IDC_COMBO_MOD_02,
		IDC_COMBO_MOD_03,
		IDC_COMBO_MOD_04,
		IDC_COMBO_MOD_05,
		IDC_COMBO_MOD_06,
		IDC_COMBO_MOD_07,
		IDC_COMBO_MOD_08,
		IDC_COMBO_MOD_09,
		IDC_COMBO_MOD_10,
		IDC_COMBO_MOD_11,
		IDC_COMBO_MOD_12,
		IDC_COMBO_MOD_13,
		IDC_COMBO_MOD_14,
		IDC_COMBO_MOD_15,
		IDC_COMBO_MOD_16,
		IDC_COMBO_MOD_17,
		IDC_COMBO_MOD_18,
		0,
		IDC_COMBO_MOD_20,
};


//___________________________________________
static int Find_ModKey_Combo_Number(UINT uid) {

	for (int i = 0; i < _countof(MOD_KEY_COMBO_UID); i++) {
		if (uid == MOD_KEY_COMBO_UID[i])
			return i;
	}
	return -1;
}



//____________________________________________________________________________________________________
static INT_PTR CALLBACK DialogProc_Config_Keys1(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	static HBRUSH hbrush_colour_box = nullptr;
	static DWORD button_states = 0;

	switch (uMsg) {
	case WM_INITDIALOG: {
		hWin_Config_Keys1 = hwndDlg;
		InitCommonControls();

		HWND hwnd_sub = nullptr;

		//fill key lists.
		HWND hwnd_key01 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_01);
		HWND hwnd_key02 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_02);
		HWND hwnd_key03 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_03);
		HWND hwnd_key04 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_04);
		HWND hwnd_key05 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_05);
		HWND hwnd_key06 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_06);
		HWND hwnd_key07 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_07);
		HWND hwnd_key08 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_08);
		HWND hwnd_key09 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_09);
		HWND hwnd_key10 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_10);
		HWND hwnd_key11 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_11);
		HWND hwnd_key12 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_12);
		HWND hwnd_key13 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_13);
		HWND hwnd_key14 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_14);
		HWND hwnd_key15 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_15);
		HWND hwnd_key16 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_16);
		HWND hwnd_key17 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_17);
		HWND hwnd_key18 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_18);
		HWND hwnd_key19 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_19);

		//speed up combobox initialization by preventing drawing while adding strings.
		SetWindowRedraw(hwnd_key01, FALSE);
		SetWindowRedraw(hwnd_key02, FALSE);
		SetWindowRedraw(hwnd_key03, FALSE);
		SetWindowRedraw(hwnd_key04, FALSE);
		SetWindowRedraw(hwnd_key05, FALSE);
		SetWindowRedraw(hwnd_key06, FALSE);
		SetWindowRedraw(hwnd_key07, FALSE);
		SetWindowRedraw(hwnd_key08, FALSE);
		SetWindowRedraw(hwnd_key09, FALSE);
		SetWindowRedraw(hwnd_key10, FALSE);
		SetWindowRedraw(hwnd_key11, FALSE);
		SetWindowRedraw(hwnd_key12, FALSE);
		SetWindowRedraw(hwnd_key13, FALSE);
		SetWindowRedraw(hwnd_key14, FALSE);
		SetWindowRedraw(hwnd_key15, FALSE);
		SetWindowRedraw(hwnd_key16, FALSE);
		SetWindowRedraw(hwnd_key17, FALSE);
		SetWindowRedraw(hwnd_key18, FALSE);
		SetWindowRedraw(hwnd_key19, FALSE);

		int key_count = _countof(P2_KEYS_UID);
		// speed up combobox initialization by setting the count and estimated memory usage first.
		SendMessage(hwnd_key01, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key02, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key03, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key04, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key05, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key06, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key07, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key08, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key09, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key10, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key11, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key12, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key13, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key14, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key15, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key16, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key17, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key18, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key19, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		

		for (int i = 0; i < key_count; i++) {
			LoadString(phinstDLL, P2_KEYS_UID[i], general_string_buff, _countof(general_string_buff));
			SendMessage(hwnd_key01, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key02, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key03, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key04, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key05, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key06, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key07, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key08, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key09, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key10, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key11, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key12, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key13, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key14, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key15, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key16, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key17, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key18, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key19, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
		}

		SendMessage(hwnd_key01, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[1])][0]), (LPARAM)0);
		SendMessage(hwnd_key02, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[2])][0]), (LPARAM)0);
		SendMessage(hwnd_key03, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[3])][0]), (LPARAM)0);
		SendMessage(hwnd_key04, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[4])][0]), (LPARAM)0);
		SendMessage(hwnd_key05, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[5])][0]), (LPARAM)0);
		SendMessage(hwnd_key06, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[6])][0]), (LPARAM)0);
		SendMessage(hwnd_key07, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[7])][0]), (LPARAM)0);
		SendMessage(hwnd_key08, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[8])][0]), (LPARAM)0);
		SendMessage(hwnd_key09, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[9])][0]), (LPARAM)0);
		SendMessage(hwnd_key10, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[10])][0]), (LPARAM)0);
		SendMessage(hwnd_key11, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[11])][0]), (LPARAM)0);
		SendMessage(hwnd_key12, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[12])][0]), (LPARAM)0);
		SendMessage(hwnd_key13, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[13])][0]), (LPARAM)0);
		SendMessage(hwnd_key14, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[14])][0]), (LPARAM)0);
		SendMessage(hwnd_key15, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[15])][0]), (LPARAM)0);
		SendMessage(hwnd_key16, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[16])][0]), (LPARAM)0);
		SendMessage(hwnd_key17, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[17])][0]), (LPARAM)0);
		SendMessage(hwnd_key18, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[18])][0]), (LPARAM)0);
		SendMessage(hwnd_key19, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[19])][0]), (LPARAM)0);

		SetWindowRedraw(hwnd_key01, TRUE);
		SetWindowRedraw(hwnd_key02, TRUE);
		SetWindowRedraw(hwnd_key03, TRUE);
		SetWindowRedraw(hwnd_key04, TRUE);
		SetWindowRedraw(hwnd_key05, TRUE);
		SetWindowRedraw(hwnd_key06, TRUE);
		SetWindowRedraw(hwnd_key07, TRUE);
		SetWindowRedraw(hwnd_key08, TRUE);
		SetWindowRedraw(hwnd_key09, TRUE);
		SetWindowRedraw(hwnd_key10, TRUE);
		SetWindowRedraw(hwnd_key11, TRUE);
		SetWindowRedraw(hwnd_key12, TRUE);
		SetWindowRedraw(hwnd_key13, TRUE);
		SetWindowRedraw(hwnd_key14, TRUE);
		SetWindowRedraw(hwnd_key15, TRUE);
		SetWindowRedraw(hwnd_key16, TRUE);
		SetWindowRedraw(hwnd_key17, TRUE);
		SetWindowRedraw(hwnd_key18, TRUE);
		SetWindowRedraw(hwnd_key19, TRUE);

		HWND hwnd_mod01 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_01);
		HWND hwnd_mod02 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_02);
		HWND hwnd_mod03 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_03);
		HWND hwnd_mod04 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_04);
		HWND hwnd_mod05 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_05);
		HWND hwnd_mod06 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_06);
		HWND hwnd_mod07 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_07);
		HWND hwnd_mod08 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_08);
		HWND hwnd_mod09 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_09);
		HWND hwnd_mod10 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_10);
		HWND hwnd_mod11 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_11);
		HWND hwnd_mod12 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_12);
		HWND hwnd_mod13 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_13);
		HWND hwnd_mod14 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_14);
		HWND hwnd_mod15 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_15);
		HWND hwnd_mod20 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_20);

		//speed up combobox initialization by preventing drawing while adding strings.
		SetWindowRedraw(hwnd_mod01, FALSE);
		SetWindowRedraw(hwnd_mod02, FALSE);
		SetWindowRedraw(hwnd_mod03, FALSE);
		SetWindowRedraw(hwnd_mod04, FALSE);
		SetWindowRedraw(hwnd_mod05, FALSE);
		SetWindowRedraw(hwnd_mod06, FALSE);
		SetWindowRedraw(hwnd_mod07, FALSE);
		SetWindowRedraw(hwnd_mod08, FALSE);
		SetWindowRedraw(hwnd_mod09, FALSE);
		SetWindowRedraw(hwnd_mod10, FALSE);
		SetWindowRedraw(hwnd_mod11, FALSE);
		SetWindowRedraw(hwnd_mod12, FALSE);
		SetWindowRedraw(hwnd_mod13, FALSE);
		SetWindowRedraw(hwnd_mod14, FALSE);
		SetWindowRedraw(hwnd_mod15, FALSE);
		SetWindowRedraw(hwnd_mod20, FALSE);


		int mod_key_count = _countof(P2_KEY_MODS_UID);

		// speed up combobox initialization by setting the count and estimated memory usage first.
		SendMessage(hwnd_mod01, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod02, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod03, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod04, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod05, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod06, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod07, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod08, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod09, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod10, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod11, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod12, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod13, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod14, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod15, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);
		SendMessage(hwnd_mod20, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff) * mod_key_count);

		for (int i = 0; i < mod_key_count; i++) {
			LoadString(phinstDLL, P2_KEY_MODS_UID[i], general_string_buff, _countof(general_string_buff));
			SendMessage(hwnd_mod01, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod02, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod03, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod04, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod05, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod06, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod07, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod08, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod09, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod10, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod11, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod12, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod13, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod14, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod15, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod20, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
		}


		SendMessage(hwnd_mod01, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[1])][1]), (LPARAM)0);
		SendMessage(hwnd_mod02, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[2])][1]), (LPARAM)0);
		SendMessage(hwnd_mod03, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[3])][1]), (LPARAM)0);
		SendMessage(hwnd_mod04, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[4])][1]), (LPARAM)0);
		SendMessage(hwnd_mod05, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[5])][1]), (LPARAM)0);
		SendMessage(hwnd_mod06, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[6])][1]), (LPARAM)0);
		SendMessage(hwnd_mod07, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[7])][1]), (LPARAM)0);
		SendMessage(hwnd_mod08, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[8])][1]), (LPARAM)0);
		SendMessage(hwnd_mod09, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[9])][1]), (LPARAM)0);
		SendMessage(hwnd_mod10, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[10])][1]), (LPARAM)0);
		SendMessage(hwnd_mod11, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[11])][1]), (LPARAM)0);
		SendMessage(hwnd_mod12, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[12])][1]), (LPARAM)0);
		SendMessage(hwnd_mod13, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[13])][1]), (LPARAM)0);
		SendMessage(hwnd_mod14, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[14])][1]), (LPARAM)0);
		SendMessage(hwnd_mod15, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[15])][1]), (LPARAM)0);
		SendMessage(hwnd_mod20, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[20])][1]), (LPARAM)0);

		SetWindowRedraw(hwnd_mod01, TRUE);
		SetWindowRedraw(hwnd_mod02, TRUE);
		SetWindowRedraw(hwnd_mod03, TRUE);
		SetWindowRedraw(hwnd_mod04, TRUE);
		SetWindowRedraw(hwnd_mod05, TRUE);
		SetWindowRedraw(hwnd_mod06, TRUE);
		SetWindowRedraw(hwnd_mod07, TRUE);
		SetWindowRedraw(hwnd_mod08, TRUE);
		SetWindowRedraw(hwnd_mod09, TRUE);
		SetWindowRedraw(hwnd_mod10, TRUE);
		SetWindowRedraw(hwnd_mod11, TRUE);
		SetWindowRedraw(hwnd_mod12, TRUE);
		SetWindowRedraw(hwnd_mod13, TRUE);
		SetWindowRedraw(hwnd_mod14, TRUE);
		SetWindowRedraw(hwnd_mod15, TRUE);
		SetWindowRedraw(hwnd_mod20, TRUE);


		hwnd_sub = GetDlgItem(hwndDlg, IDC_CHECK_REV_SPEED_KEY_SPACE);
		DWORD checked = BST_UNCHECKED;
		if (Is_Rotation_Speed_Key_Reverse_Action())
			checked = BST_CHECKED;
		SendMessage(hwnd_sub, BM_SETCHECK, (WPARAM)checked, (LPARAM)0);

		break;
	}

	case WM_NOTIFY:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_COMBO_KEY_01:
		case IDC_COMBO_KEY_02:
		case IDC_COMBO_KEY_03:
		case IDC_COMBO_KEY_04:
		case IDC_COMBO_KEY_05:
		case IDC_COMBO_KEY_06:
		case IDC_COMBO_KEY_07:
		case IDC_COMBO_KEY_08:
		case IDC_COMBO_KEY_09:
		case IDC_COMBO_KEY_10:
		case IDC_COMBO_KEY_11:
		case IDC_COMBO_KEY_12:
		case IDC_COMBO_KEY_13:
		case IDC_COMBO_KEY_14:
		case IDC_COMBO_KEY_15:
		case IDC_COMBO_KEY_16:
		case IDC_COMBO_KEY_17:
		case IDC_COMBO_KEY_18:
		case IDC_COMBO_KEY_19:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				int pos = (int)(SendMessage(GetDlgItem(hwndDlg, LOWORD(wParam)), CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
				if (pos >= 0 && pos < _countof(key_map)) {
					UINT combo_num = Find_Key_Combo_Number(LOWORD(wParam));
					if(combo_num >= 0)
					P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[combo_num])][0] = key_map[pos];
				}
			}
			return TRUE;
		case IDC_COMBO_MOD_01:
		case IDC_COMBO_MOD_02:
		case IDC_COMBO_MOD_03:
		case IDC_COMBO_MOD_04:
		case IDC_COMBO_MOD_05:
		case IDC_COMBO_MOD_06:
		case IDC_COMBO_MOD_07:
		case IDC_COMBO_MOD_08:
		case IDC_COMBO_MOD_09:
		case IDC_COMBO_MOD_10:
		case IDC_COMBO_MOD_11:
		case IDC_COMBO_MOD_12:
		case IDC_COMBO_MOD_13:
		case IDC_COMBO_MOD_14:
		case IDC_COMBO_MOD_15:
		case IDC_COMBO_MOD_20:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				int pos = (int)(SendMessage(GetDlgItem(hwndDlg, LOWORD(wParam)), CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
				if (pos >= 0 && pos < _countof(key_mods)) {
					UINT combo_num = Find_ModKey_Combo_Number(LOWORD(wParam));
					
					if (pos == 1)
						P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[combo_num])][1] = 1;
					else if (pos == 2)
						P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[combo_num])][1] = 1 << 1;
					else if (pos == 3)
						P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[combo_num])][1] = 1 << 2;
					else 
						P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[combo_num])][1] = 1 << 3;

					if (LOWORD(wParam) == IDC_COMBO_MOD_20) {
						if (pos == 1)
							P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[combo_num])][0] = VK_SHIFT;
						else if (pos == 2)
							P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[combo_num])][0] = VK_CONTROL;
						else if (pos == 3)
							P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[combo_num])][0] = VK_MENU;
						else
							P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page01[combo_num])][0] = 0;
					}
				}
			}
			return TRUE;
		case IDC_CHECK_REV_SPEED_KEY_SPACE:
			if (HIWORD(wParam) == BN_CLICKED) {
				DWORD button_state = (int)(SendMessage((HWND)lParam, BM_GETCHECK, (WPARAM)0, (LPARAM)0));
				bool is_speed_key_rev = false;
				if (button_state & BST_CHECKED)
					is_speed_key_rev = true;
				Set_Rotation_Speed_Key_Reverse_Action(is_speed_key_rev);
			}
			return TRUE;
		default:
			break;
		}
		break;
	case WM_DESTROY: {

		hWin_Config_Keys1 = nullptr;
		return FALSE;
	}
	default:
		return FALSE;
		break;
	}
	return TRUE;
}


//____________________________________________________________________________________________________
static INT_PTR CALLBACK DialogProc_Config_Keys2(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	static HBRUSH hbrush_colour_box = nullptr;
	static DWORD button_states = 0;

	switch (uMsg) {
	case WM_INITDIALOG: {
		hWin_Config_Keys2 = hwndDlg;
		InitCommonControls();

		HWND hwnd_sub = nullptr;

		//fill key lists.
		HWND hwnd_key01 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_01);
		HWND hwnd_key02 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_02);
		HWND hwnd_key03 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_03);
		HWND hwnd_key04 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_04);
		HWND hwnd_key05 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_05);
		HWND hwnd_key06 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_06);
		HWND hwnd_key07 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_07);
		HWND hwnd_key08 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_08);
		HWND hwnd_key09 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_09);
		HWND hwnd_key10 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_10);
		HWND hwnd_key11 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_11);
		HWND hwnd_key12 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_12);
		HWND hwnd_key13 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_13);
		HWND hwnd_key14 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_14);
		HWND hwnd_key15 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_15);
		HWND hwnd_key16 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_16);
		HWND hwnd_key17 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_17);
		HWND hwnd_key18 = GetDlgItem(hwndDlg, IDC_COMBO_KEY_18);

		//speed up combobox initialization by preventing drawing while adding strings.
		SetWindowRedraw(hwnd_key01, FALSE);
		SetWindowRedraw(hwnd_key02, FALSE);
		SetWindowRedraw(hwnd_key03, FALSE);
		SetWindowRedraw(hwnd_key04, FALSE);
		SetWindowRedraw(hwnd_key05, FALSE);
		SetWindowRedraw(hwnd_key06, FALSE);
		SetWindowRedraw(hwnd_key07, FALSE);
		SetWindowRedraw(hwnd_key08, FALSE);
		SetWindowRedraw(hwnd_key09, FALSE);
		SetWindowRedraw(hwnd_key10, FALSE);
		SetWindowRedraw(hwnd_key11, FALSE);
		SetWindowRedraw(hwnd_key12, FALSE);
		SetWindowRedraw(hwnd_key13, FALSE);
		SetWindowRedraw(hwnd_key14, FALSE);
		SetWindowRedraw(hwnd_key15, FALSE);
		SetWindowRedraw(hwnd_key16, FALSE);
		SetWindowRedraw(hwnd_key17, FALSE);
		SetWindowRedraw(hwnd_key18, FALSE);

		int key_count = _countof(P2_KEYS_UID);
		// speed up combobox initialization by setting the count and estimated memory usage first.
		SendMessage(hwnd_key01, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key02, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key03, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key04, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key05, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key06, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key07, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key08, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key09, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key10, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key11, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key12, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key13, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key14, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key15, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key16, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key17, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);
		SendMessage(hwnd_key18, CB_INITSTORAGE, key_count, sizeof(general_string_buff) * key_count);

		for (int i = 0; i < key_count; i++) {
			LoadString(phinstDLL, P2_KEYS_UID[i], general_string_buff, _countof(general_string_buff));
			SendMessage(hwnd_key01, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key02, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key03, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key04, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key05, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key06, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key07, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key08, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key09, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key10, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key11, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key12, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key13, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key14, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key15, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key16, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key17, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_key18, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
		}

		SendMessage(hwnd_key01, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[1])][0]), (LPARAM)0);
		SendMessage(hwnd_key02, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[2])][0]), (LPARAM)0);
		SendMessage(hwnd_key03, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[3])][0]), (LPARAM)0);
		SendMessage(hwnd_key04, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[4])][0]), (LPARAM)0);
		SendMessage(hwnd_key05, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[5])][0]), (LPARAM)0);
		SendMessage(hwnd_key06, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[6])][0]), (LPARAM)0);
		SendMessage(hwnd_key07, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[7])][0]), (LPARAM)0);
		SendMessage(hwnd_key08, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[8])][0]), (LPARAM)0);
		SendMessage(hwnd_key09, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[9])][0]), (LPARAM)0);
		SendMessage(hwnd_key10, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[10])][0]), (LPARAM)0);
		SendMessage(hwnd_key11, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[11])][0]), (LPARAM)0);
		SendMessage(hwnd_key12, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[12])][0]), (LPARAM)0);
		SendMessage(hwnd_key13, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[13])][0]), (LPARAM)0);
		SendMessage(hwnd_key14, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[14])][0]), (LPARAM)0);
		SendMessage(hwnd_key15, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[15])][0]), (LPARAM)0);
		SendMessage(hwnd_key16, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[16])][0]), (LPARAM)0);
		SendMessage(hwnd_key17, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[17])][0]), (LPARAM)0);
		SendMessage(hwnd_key18, CB_SETCURSEL, (WPARAM)Find_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[18])][0]), (LPARAM)0);

		SetWindowRedraw(hwnd_key01, TRUE);
		SetWindowRedraw(hwnd_key02, TRUE);
		SetWindowRedraw(hwnd_key03, TRUE);
		SetWindowRedraw(hwnd_key04, TRUE);
		SetWindowRedraw(hwnd_key05, TRUE);
		SetWindowRedraw(hwnd_key06, TRUE);
		SetWindowRedraw(hwnd_key07, TRUE);
		SetWindowRedraw(hwnd_key08, TRUE);
		SetWindowRedraw(hwnd_key09, TRUE);
		SetWindowRedraw(hwnd_key10, TRUE);
		SetWindowRedraw(hwnd_key11, TRUE);
		SetWindowRedraw(hwnd_key12, TRUE);
		SetWindowRedraw(hwnd_key13, TRUE);
		SetWindowRedraw(hwnd_key14, TRUE);
		SetWindowRedraw(hwnd_key15, TRUE);
		SetWindowRedraw(hwnd_key16, TRUE);
		SetWindowRedraw(hwnd_key17, TRUE);
		SetWindowRedraw(hwnd_key18, TRUE);


		HWND hwnd_mod01 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_01);
		HWND hwnd_mod02 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_02);
		HWND hwnd_mod03 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_03);
		HWND hwnd_mod04 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_04);
		HWND hwnd_mod05 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_05);
		HWND hwnd_mod06 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_06);
		HWND hwnd_mod07 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_07);
		HWND hwnd_mod08 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_08);
		HWND hwnd_mod09 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_09);
		HWND hwnd_mod10 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_10);
		HWND hwnd_mod11 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_11);
		HWND hwnd_mod12 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_12);
		HWND hwnd_mod13 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_13);
		HWND hwnd_mod14 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_14);
		HWND hwnd_mod15 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_15);
		HWND hwnd_mod16 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_16);
		HWND hwnd_mod17 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_17);
		HWND hwnd_mod18 = GetDlgItem(hwndDlg, IDC_COMBO_MOD_18);

		//speed up combobox initialization by preventing drawing while adding strings.
		SetWindowRedraw(hwnd_mod01, FALSE);
		SetWindowRedraw(hwnd_mod02, FALSE);
		SetWindowRedraw(hwnd_mod03, FALSE);
		SetWindowRedraw(hwnd_mod04, FALSE);
		SetWindowRedraw(hwnd_mod05, FALSE);
		SetWindowRedraw(hwnd_mod06, FALSE);
		SetWindowRedraw(hwnd_mod07, FALSE);
		SetWindowRedraw(hwnd_mod08, FALSE);
		SetWindowRedraw(hwnd_mod09, FALSE);
		SetWindowRedraw(hwnd_mod10, FALSE);
		SetWindowRedraw(hwnd_mod11, FALSE);
		SetWindowRedraw(hwnd_mod12, FALSE);
		SetWindowRedraw(hwnd_mod13, FALSE);
		SetWindowRedraw(hwnd_mod14, FALSE);
		SetWindowRedraw(hwnd_mod15, FALSE);
		SetWindowRedraw(hwnd_mod16, FALSE);
		SetWindowRedraw(hwnd_mod17, FALSE);
		SetWindowRedraw(hwnd_mod18, FALSE);

		int mod_key_count = _countof(P2_KEY_MODS_UID);
		// speed up combobox initialization by setting the count and estimated memory usage first.
		SendMessage(hwnd_mod01, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod02, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod03, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod04, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod05, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod06, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod07, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod08, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod09, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod10, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod11, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod12, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod13, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod14, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod15, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod16, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod17, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);
		SendMessage(hwnd_mod18, CB_INITSTORAGE, mod_key_count, sizeof(general_string_buff)* mod_key_count);



		for (int i = 0; i < mod_key_count; i++) {
			LoadString(phinstDLL, P2_KEY_MODS_UID[i], general_string_buff, _countof(general_string_buff));
			SendMessage(hwnd_mod01, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod02, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod03, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod04, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod05, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod06, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod07, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod08, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod09, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod10, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod11, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod12, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod13, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod14, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod15, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod16, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod17, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
			SendMessage(hwnd_mod18, CB_ADDSTRING, (WPARAM)0, (LPARAM)general_string_buff);
		}
		
		SendMessage(hwnd_mod01, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[1])][1]), (LPARAM)0);
		SendMessage(hwnd_mod02, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[2])][1]), (LPARAM)0);
		SendMessage(hwnd_mod03, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[3])][1]), (LPARAM)0);
		SendMessage(hwnd_mod04, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[4])][1]), (LPARAM)0);
		SendMessage(hwnd_mod05, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[5])][1]), (LPARAM)0);
		SendMessage(hwnd_mod06, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[6])][1]), (LPARAM)0);
		SendMessage(hwnd_mod07, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[7])][1]), (LPARAM)0);
		SendMessage(hwnd_mod08, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[8])][1]), (LPARAM)0);
		SendMessage(hwnd_mod09, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[9])][1]), (LPARAM)0);
		SendMessage(hwnd_mod10, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[10])][1]), (LPARAM)0);
		SendMessage(hwnd_mod11, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[11])][1]), (LPARAM)0);
		SendMessage(hwnd_mod12, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[12])][1]), (LPARAM)0);
		SendMessage(hwnd_mod13, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[13])][1]), (LPARAM)0);
		SendMessage(hwnd_mod14, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[14])][1]), (LPARAM)0);
		SendMessage(hwnd_mod15, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[15])][1]), (LPARAM)0);
		SendMessage(hwnd_mod16, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[16])][1]), (LPARAM)0);
		SendMessage(hwnd_mod17, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[17])][1]), (LPARAM)0);
		SendMessage(hwnd_mod18, CB_SETCURSEL, (WPARAM)Find_Mod_Key_Value_Number(P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[18])][1]), (LPARAM)0);


		SetWindowRedraw(hwnd_mod01, TRUE);
		SetWindowRedraw(hwnd_mod02, TRUE);
		SetWindowRedraw(hwnd_mod03, TRUE);
		SetWindowRedraw(hwnd_mod04, TRUE);
		SetWindowRedraw(hwnd_mod05, TRUE);
		SetWindowRedraw(hwnd_mod06, TRUE);
		SetWindowRedraw(hwnd_mod07, TRUE);
		SetWindowRedraw(hwnd_mod08, TRUE);
		SetWindowRedraw(hwnd_mod09, TRUE);
		SetWindowRedraw(hwnd_mod10, TRUE);
		SetWindowRedraw(hwnd_mod11, TRUE);
		SetWindowRedraw(hwnd_mod12, TRUE);
		SetWindowRedraw(hwnd_mod13, TRUE);
		SetWindowRedraw(hwnd_mod14, TRUE);
		SetWindowRedraw(hwnd_mod15, TRUE);
		SetWindowRedraw(hwnd_mod16, TRUE);
		SetWindowRedraw(hwnd_mod17, TRUE);
		SetWindowRedraw(hwnd_mod18, TRUE);

		break;
	}

	case WM_NOTIFY:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_COMBO_KEY_01:
		case IDC_COMBO_KEY_02:
		case IDC_COMBO_KEY_03:
		case IDC_COMBO_KEY_04:
		case IDC_COMBO_KEY_05:
		case IDC_COMBO_KEY_06:
		case IDC_COMBO_KEY_07:
		case IDC_COMBO_KEY_08:
		case IDC_COMBO_KEY_09:
		case IDC_COMBO_KEY_10:
		case IDC_COMBO_KEY_11:
		case IDC_COMBO_KEY_12:
		case IDC_COMBO_KEY_13:
		case IDC_COMBO_KEY_14:
		case IDC_COMBO_KEY_15:
		case IDC_COMBO_KEY_16:
		case IDC_COMBO_KEY_17:
		case IDC_COMBO_KEY_18:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				int pos = (int)(SendMessage(GetDlgItem(hwndDlg, LOWORD(wParam)), CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
				if (pos >= 0 && pos < _countof(key_map)) {
					UINT combo_num = Find_Key_Combo_Number(LOWORD(wParam));
					if (combo_num >= 0)
						P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[combo_num])][0] = key_map[pos];
				}
			}
			return TRUE;
		case IDC_COMBO_MOD_01:
		case IDC_COMBO_MOD_02:
		case IDC_COMBO_MOD_03:
		case IDC_COMBO_MOD_04:
		case IDC_COMBO_MOD_05:
		case IDC_COMBO_MOD_06:
		case IDC_COMBO_MOD_07:
		case IDC_COMBO_MOD_08:
		case IDC_COMBO_MOD_09:
		case IDC_COMBO_MOD_10:
		case IDC_COMBO_MOD_11:
		case IDC_COMBO_MOD_12:
		case IDC_COMBO_MOD_13:
		case IDC_COMBO_MOD_14:
		case IDC_COMBO_MOD_15:
		case IDC_COMBO_MOD_16:
		case IDC_COMBO_MOD_17:
		case IDC_COMBO_MOD_18:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				int pos = (int)(SendMessage(GetDlgItem(hwndDlg, LOWORD(wParam)), CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
				if (pos >= 0 && pos < _countof(key_mods)) {
					UINT combo_num = Find_ModKey_Combo_Number(LOWORD(wParam));
					if (pos == 1)
						P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[combo_num])][1] = 1;
					else if (pos == 2)
						P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[combo_num])][1] = 1 << 1;
					else if (pos == 3)
						P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[combo_num])][1] = 1 << 2;
					else
						P2_ACTIONS_KEYS[static_cast<int>(actions_keys_page02[combo_num])][1] = 1 << 3;
				}
			}
			return TRUE;
		default:
			break;
		}
		break;
	case WM_DESTROY: {

		hWin_Config_Keys2 = nullptr;
		return FALSE;
	}
	default:
		return FALSE;
		break;
	}
	return TRUE;
}


//______________________________________________________________________________________________________
static INT_PTR CALLBACK DialogProc_Config_Control(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	static HWND hwndParent = nullptr;
	static bool was_Deactivated = false;
	static HWND hWin_Config_Joy_Active = nullptr;
	
	switch (uMsg) {
	case WM_INITDIALOG: {
		hWin_Config_Control = hwndDlg;
		
		////101 is the P2 icon
		SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hinst_DARK, MAKEINTRESOURCE(101)));
		InitCommonControls();

		hwndParent = GetParent(hwndDlg);

		INITCOMMONCONTROLSEX iccex{ 0 };
		//initialize common controls.
		iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		iccex.dwICC = ICC_TAB_CLASSES;
		InitCommonControlsEx(&iccex);

		TCITEM tie{ 0 };

		HWND hwndTab = GetDlgItem(hwndDlg, IDC_TAB1);
		//add a tab for each of the child dialog boxes.
		tie.mask = TCIF_TEXT | TCIF_IMAGE;
		tie.iImage = -1;

		LoadString(phinstDLL, IDS_TAB_JOYSTICK, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, 0, &tie);

		LoadString(phinstDLL, IDS_TAB_MOUSE, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, 1, &tie);

		LoadString(phinstDLL, IDS_TAB_KEYS_1, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, 2, &tie);

		LoadString(phinstDLL, IDS_TAB_KEYS_2, general_string_buff, _countof(general_string_buff));
		tie.pszText = general_string_buff;
		TabCtrl_InsertItem(hwndTab, 3, &tie);

		//set position to centre of parent window.
		RECT rc_Win{ 0,0,0,0 };
		GetWindowRect(hwndDlg, &rc_Win);
		RECT rcParent{ 0,0,0,0 };
		GetWindowRect(hwndParent, &rcParent);
		SetWindowPos(hwndDlg, nullptr, rcParent.left + ((rcParent.right - rcParent.left) - (rc_Win.right - rc_Win.left)) / 2, rc_Win.top + ((rcParent.bottom - rcParent.top) - (rc_Win.bottom - rc_Win.top)) / 2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		//disable main window while this dialog is running.
		EnableWindow(hwndParent, FALSE);
	
		//create tab windows.
		if (controller_enhancements_enabled) {
			hWin_Config_Joy = CreateDialogParam(phinstDLL, MAKEINTRESOURCE(IDD_DIALOG_CONFIG_JOY), hwndDlg, &DialogProc_JoyConfig, 0);
			hWin_Config_Joy_Active = hWin_Config_Joy;
		}
		else {
			hWin_Config_Joy_Off = CreateDialogParam(phinstDLL, MAKEINTRESOURCE(IDD_DIALOG_CONFIG_JOY_OFF), hwndDlg, &DialogProc_JoyConfig_Off, 0);
			hWin_Config_Joy_Active = hWin_Config_Joy_Off;
		}
		hWin_Config_Mouse = CreateDialogParam(phinstDLL, MAKEINTRESOURCE(IDD_DIALOG_CONFIG_MOUSE), hwndDlg, &DialogProc_Config_Mouse, 0);

		//hWin_Config_Keys1 = CreateDialogParam(phinstDLL, MAKEINTRESOURCE(IDD_DIALOG_CONFIG_KEYS_1), hwndDlg, &DialogProc_Config_Keys1, 0);
		//hWin_Config_Keys2 = CreateDialogParam(phinstDLL, MAKEINTRESOURCE(IDD_DIALOG_CONFIG_KEYS_2), hwndDlg, &DialogProc_Config_Keys2, 0);
		
		//set the position of tab windows, adjusting for the height of the tabs.
		RECT rcTab;
		GetClientRect(hwndDlg, &rcTab);
		TabCtrl_AdjustRect(hwndTab, FALSE, &rcTab);


		SetWindowPos(hWin_Config_Joy_Active, nullptr, rcTab.left, rcTab.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SetWindowPos(hWin_Config_Mouse, nullptr, rcTab.left, rcTab.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		//SetWindowPos(hWin_Config_Keys1, nullptr, rcTab.left, rcTab.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		//SetWindowPos(hWin_Config_Keys2, nullptr, rcTab.left, rcTab.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		//set initial focus tab.
		if (controller_enhancements_enabled && *p_p2_controller_flags) {
			TabCtrl_SetCurFocus(hwndTab, 0);
			ShowWindow(hWin_Config_Joy_Active, SW_SHOW);
			ShowWindow(hWin_Config_Mouse, SW_HIDE);
		}
		else {
			TabCtrl_SetCurFocus(hwndTab, 1);
			ShowWindow(hWin_Config_Joy_Active, SW_HIDE);
			ShowWindow(hWin_Config_Mouse, SW_SHOW);
		}


		HWND hwnd_sub = GetDlgItem(hwndDlg, IDC_CHECK_ALT_FLT_MODE);
		DWORD checked = BST_UNCHECKED;
		if (Is_Alt_Flight_Mode())
			checked = BST_CHECKED;
		SendMessage(hwnd_sub, BM_SETCHECK, (WPARAM)checked, (LPARAM)0);


		break;
	}
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case TCN_SELCHANGE: {
			HWND hwndTab = GetDlgItem(hwndDlg, IDC_TAB1);
			int tabNum = TabCtrl_GetCurSel(hwndTab);
			//if (!controller_enhancements_enabled && tabNum == 0) 
			//	TabCtrl_SetCurFocus(hwndTab, 1);
			if (tabNum == 0) {
				ShowWindow(hWin_Config_Joy_Active, SW_SHOW);
				ShowWindow(hWin_Config_Mouse, SW_HIDE);
				ShowWindow(hWin_Config_Keys1, SW_HIDE);
				ShowWindow(hWin_Config_Keys2, SW_HIDE);
			}
			else if (tabNum == 1) {
				ShowWindow(hWin_Config_Joy_Active, SW_HIDE);
				ShowWindow(hWin_Config_Mouse, SW_SHOW);
				ShowWindow(hWin_Config_Keys1, SW_HIDE);
				ShowWindow(hWin_Config_Keys2, SW_HIDE);
			}
			else if (tabNum == 2) {
				ShowWindow(hWin_Config_Joy_Active, SW_HIDE);
				ShowWindow(hWin_Config_Mouse, SW_HIDE);
				if (!hWin_Config_Keys1) {
					hWin_Config_Keys1 = CreateDialogParam(phinstDLL, MAKEINTRESOURCE(IDD_DIALOG_CONFIG_KEYS_1), hwndDlg, &DialogProc_Config_Keys1, 0);
					HWND hwndTab = GetDlgItem(hwndDlg, IDC_TAB1);
					RECT rcTab;
					GetClientRect(hwndDlg, &rcTab);
					TabCtrl_AdjustRect(hwndTab, FALSE, &rcTab);
					SetWindowPos(hWin_Config_Keys1, nullptr, rcTab.left, rcTab.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
				}
				ShowWindow(hWin_Config_Keys1, SW_SHOW);
				ShowWindow(hWin_Config_Keys2, SW_HIDE);
			}
			else if (tabNum == 3) {
				ShowWindow(hWin_Config_Joy_Active, SW_HIDE);
				ShowWindow(hWin_Config_Mouse, SW_HIDE);
				ShowWindow(hWin_Config_Keys1, SW_HIDE);
				if (!hWin_Config_Keys2) {
					hWin_Config_Keys2 = CreateDialogParam(phinstDLL, MAKEINTRESOURCE(IDD_DIALOG_CONFIG_KEYS_2), hwndDlg, &DialogProc_Config_Keys2, 0);
					HWND hwndTab = GetDlgItem(hwndDlg, IDC_TAB1);
					RECT rcTab;
					GetClientRect(hwndDlg, &rcTab);
					TabCtrl_AdjustRect(hwndTab, FALSE, &rcTab);
					SetWindowPos(hWin_Config_Keys2, nullptr, rcTab.left, rcTab.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
				}
				ShowWindow(hWin_Config_Keys2, SW_SHOW);
			}
			break;
		}
		default:
			break;
		}
		break;
	case WM_MOVE: {
		HWND hwndTab = GetDlgItem(hwndDlg, IDC_TAB1);
		//move tab windows with the main window, adjusting for the height of the tabs.
		RECT rcTab;
		GetClientRect(hwndDlg, &rcTab);
		TabCtrl_AdjustRect(hwndTab, FALSE, &rcTab);


		SetWindowPos(hWin_Config_Joy_Active, nullptr, rcTab.left, rcTab.top, 0,0, SWP_NOZORDER| SWP_NOSIZE);
		SetWindowPos(hWin_Config_Mouse, nullptr, rcTab.left, rcTab.top, 0,0, SWP_NOZORDER | SWP_NOSIZE);
		if (hWin_Config_Keys1)
			SetWindowPos(hWin_Config_Keys1, nullptr, rcTab.left, rcTab.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		if (hWin_Config_Keys2)
			SetWindowPos(hWin_Config_Keys2, nullptr, rcTab.left, rcTab.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			if (controller_enhancements_enabled)
				Joysticks.Save();
			Mouse.Save();
			Keys_Save();
			EnableWindow(hwndParent, TRUE);
			DestroyWindow(hwndDlg);
			return FALSE;
		}
		case IDCANCEL: {
			if (controller_enhancements_enabled)
				Joysticks.Load();
			Mouse.Load();
			Keys_Load();
			EnableWindow(hwndParent, TRUE);
			DestroyWindow(hwndDlg);
			return FALSE;
		}
		case IDC_CHECK_ALT_FLT_MODE:
			if (HIWORD(wParam) == BN_CLICKED) {
				DWORD button_state = (int)(SendMessage((HWND)lParam, BM_GETCHECK, (WPARAM)0, (LPARAM)0));
				bool is_alt_flt_mode = false;
				if (button_state & BST_CHECKED)
					is_alt_flt_mode = true;
				Set_Alt_Flight_Mode(is_alt_flt_mode);
			}
			return TRUE;
		default:
			break;
		}
		break;
	case WM_CLOSE:
		if (controller_enhancements_enabled)
			Joysticks.Load();
		Mouse.Load();
		Keys_Load();
		EnableWindow(hwndParent, TRUE);
		DestroyWindow(hwndDlg);
		return FALSE;
	case WM_DESTROY: {
		hWin_Config_Control = nullptr;
		hWin_Config_Joy_Active = nullptr;
		if (was_Deactivated)
			SetWindowActivation(TRUE);
		return FALSE;
	}
	case WM_ACTIVATEAPP:
		//WM_ACTIVATEAPP:wParam==FALSE wont be received with the parent window disabled so Set_WindowActive_State(TRUE) on exit to return re-enable fullscreen window.
		if (wParam == FALSE)
			was_Deactivated = true;
		else
			was_Deactivated = false;

		return FALSE;
	default:
		return FALSE;
		break;
	}

	return TRUE;
}


//__________________________________________________________
static HWND JoyConfig_Create(HWND hwnd, HINSTANCE hinstance) {

	if (hWin_Config_Control)
		return hWin_Config_Control;

	HWND hwndDlg = CreateDialog(hinstance, MAKEINTRESOURCE(IDD_DIALOG_CONFIG_MAIN), hwnd, (DLGPROC)DialogProc_Config_Control);
	if (!hwndDlg)
		return hwndDlg;
	JoyConfig_Refresh_JoyList();

	ShowWindow(hwndDlg, SW_SHOW);

	return hwndDlg;
}


//___________________
BOOL JoyConfig_Main() {

	if (!JoyConfig_Create(*p_p2_hWinMain, phinstDLL))
		return FALSE;

	wait_joy_config = TRUE;
	PROFILE_TYPE saved_pro_type = current_pro_type;

	P2_Active_App(FALSE);
	ShowCursor(TRUE);

	while (hWin_Config_Control != nullptr) {
		Sleep(16);
		MSG message;
		while (PeekMessage(&message, 0, 0, 0, true)) {
			if (!hWin_Config_Control || !IsDialogMessage(hWin_Config_Control, &message)) {
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
		Joysticks.Update();
		if (hWin_Config_Joy)
			JoyConfig_Refresh(hWin_Config_Joy);
	}

	current_num_axes = 0;
	if (current_axisArray)
		delete[] current_axisArray;
	current_axisArray = nullptr;

	current_num_buttons = 0;
	if (current_buttonArray)
		delete[] current_buttonArray;
	current_buttonArray = nullptr;

	current_num_switches = 0;
	if (current_switchArray)
		delete[] current_switchArray;
	current_switchArray = nullptr;

	ShowCursor(FALSE);
	P2_Active_App(TRUE);

	current_pro_type = saved_pro_type;
	wait_joy_config = FALSE;
	//clear keyboard state
	memset(p2_keyboard_state_main, 0, 256);

	return TRUE;
}
