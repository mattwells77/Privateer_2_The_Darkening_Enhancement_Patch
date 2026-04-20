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
#include <ppl.h>
#include <winrt/Windows.Gaming.Input.h>
#include <winrt/Windows.Foundation.h>

#define KEYS_DAT_CODE			'K' + ('E' << 8) + ('Y' << 16) + ('S' << 24)
#define GAME_CODE	'P' + ('2' << 8) + ('D' << 16) + ('K' << 24)

#define JOYSTICK_PROFILE_VERSION	3
#define JOYSTICK_CONFIG_PATH			L"controllers"

#define NUM_MOUSE_BUTTONS	5


extern BYTE P2_ACTIONS_KEYS[][4];

#define NUM_JOY_PROFILES	5

enum class PROFILE_TYPE {
	GUI,
	Space,
	ReMap_1,
	ReMap_2,
	ReMap_3,
	End,
};

extern PROFILE_TYPE current_pro_type;

extern PROFILE_TYPE current_pro_type_map;

#define PROFILE_SPACE	1; 
#define PROFILE_GUI		0; 

extern int profile_space;
extern int profile_gui;

extern int P2_PROFILE_MAX;

enum class P2_ACTIONS {
	None,
	Exit_Game,
	Drop_Mine_Decoy,
	Select_Target_In_Crosshairs,
	Cycle_Targets_Forward,
	Cycle_Targets_Back,
	VDW_Comms,
	VDW_Damage,
	Game_Options,
	Select_Deselect_Weapon_in_VDW,
	Cycle_Weapons_in_VDW,
	VDW_Guns,
	VDW_Missiles,
	Activate_BSE,
	VDW_Mines_Decoys,
	Fire_Missile,
	Cycle_Radar_Ranges,
	Afterburner,
	Toggle_Radar_Type,
	Speed_Increase,
	Speed_Increase_2,
	Speed_Decrease,
	Speed_Decrease_2,
	Jump,
	Toggle_SOS,
	Toggle_Full_No_Guns,
	Targeting_Off,
	Tractor_Targeted_Object,
	Drop_Tractored_Object,
	Nav_Map,
	Drop_Nuke_em,
	Electronic_Diary,
	Match_Target_Speed,
	Hotkey_List,
	Game_Options_2,
	Warp_Shields,
	Target_Nearest_Hostile,
	Target_Nearest_Friendly,
	Exterior_Ship_View_Left,
	Exterior_Ship_View_Right,
	Exterior_Ship_View_Up,
	Exterior_Ship_View_Down,
	Speed_Zero,
	Speed_Max,

	Roll_Right,//(Joystick)
	Roll_Left,// (Joystick)
	Fire_Guns,

	View_Front,
	View_Left,
	View_Back,
	View_Right,
	View_Ship,
	View_Target_Camera,
	View_External,
	View_Chase_Target,
	View_Fixed,
	View_Fly_By_Cinematic,

	Num_1,
	Num_2,
	Num_3,
	Num_4,
	Num_5,
	Num_6,
	Num_7,
	Num_8,
	Num_9,


	//movie choice
	Movie_Choice_Up,
	Movie_Choice_Down,
	Movie_Choice_Select,


	//gen gui//
	GUI_PAD,
	GUI_Exit_Game,
	GUI_Exit_No,
	GUI_Exit_Yes,

	//main load scrn
	Main_Load_Escape,
	Main_Load_Select,

	//unk01
	Unk01_Select,

	//space_exit
	Space_Exit_No,
	Space_Exit_Yes,

	//nav
	Navigation_Type_Nav_Point_Num,
	Navigation_Zoom_In,
	Navigation_Zoom_In_2,
	Navigation_Zoom_Out,
	Navigation_Zoom_Out_2,
	Navigation_Reset_View,
	Navigation_Toggle_Roll_View,
	Navigation_Toggle_Planets,
	Navigation_Toggle_Nav_Points,
	Navigation_Toggle_Nav_point_Numbers,
	Navigation_Toggle_Bases,
	Navigation_Toggle_Escape,

	//email?
	Email_Escape,
	//diary
	Diary_Escape,
	//options
	Options_Escape,
	Options_Escape_2,
	//hot keys
	HotKeys_Escape,
	HotKeys_Escape_2,

	//movie playback
	Movie_Escape,
	Movie_Increase_Gamma,
	Movie_Decrease_Gamma,

	Pitch_Down,
	Pitch_Up,
	Yaw_Left,
	Yaw_Right,

	Pitch_Up_Yaw_Left,
	Pitch_Up_NumPad,
	Pitch_Up_Yaw_Right,
	Yaw_Left_NumPad,
	Yaw_Right_NumPad,
	Pitch_Down_Yaw_Left,
	Pitch_Down_NumPad,
	Pitch_Down_Yaw_Right,

	Roll_Left_NumPad,
	Roll_Right_NumPad,

	Store_Target_0,

	Left_Click_Key_GUI,
	Right_Click_Key_GUI,

	ReMap_1,
	ReMap_2,
	ReMap_3,

	Left_Click,
	Right_Click,

	Rotation_Speed_Key,
	Joystick_Roll_Modifier,

	Store_Target_1,
	Store_Target_2,
	Store_Target_3,
	Store_Target_4,
	Store_Target_5,
	Store_Target_6,
	Store_Target_7,
	Store_Target_8,
	Store_Target_9,
	Num_0,
	End,
};


enum class AXIS_TYPE {
	None,
	Yaw,
	Pitch,
	Roll,
	Throttle,
	AsOneButton,
	AsTwoButtons,
	Yaw_Left,
	Yaw_Right,
	Pitch_Up,
	Pitch_Down,
	Roll_Left,
	Roll_Right,
	//Throttle_Up,
	//Throttle_Down,
	Pointer_X,
	Pointer_Y,
	Pointer_Left,
	Pointer_Right,
	Pointer_Up,
	Pointer_Down,
};


enum class SWITCH_POS {
	Centre,
	Up,
	Up_Right,
	Right,
	Down_Right,
	Down,
	Down_Left,
	Left,
	Up_Left,
};


//____________________
class ACTION_KEY_MOUSE {
public:
	ACTION_KEY_MOUSE() {
		for (int i = 0; i < NUM_JOY_PROFILES; i++)
			button[i] = P2_ACTIONS::None;
		pressed = false;
		active_profile = PROFILE_TYPE::GUI;
		active_profile_num = 0;
	};
	bool Is_Pressed() const { return pressed; };
	void SetAction(P2_ACTIONS in_action) {
		button[static_cast<int>(current_pro_type)] = in_action;
	};
	bool SetButton(bool new_state);
	void SetButton_Instant(LONG duration_ms) const;
	P2_ACTIONS GetAction() const { return button[static_cast<int>(current_pro_type)]; };
protected:
private:
	P2_ACTIONS button[NUM_JOY_PROFILES];
	PROFILE_TYPE active_profile;
	int active_profile_num;
	bool pressed;
};


//______________
class ACTION_KEY {
public:
	ACTION_KEY() {
		for (int i = 0; i < NUM_JOY_PROFILES; i++)
			button[i] = P2_ACTIONS::None;
		pressed = false;
		active_profile = PROFILE_TYPE::GUI;
		active_profile_num = 0;
	};
	bool Is_Pressed() const { return pressed; };
	void Set_Action(P2_ACTIONS in_action) {
		button[static_cast<int>(current_pro_type)] = in_action;
	};
	bool SetButton(bool new_state);
	P2_ACTIONS GetAction() const { return button[static_cast<int>(current_pro_type)]; };
	PROFILE_TYPE GetActiveProfile() const { return active_profile; };
protected:
private:
	P2_ACTIONS button[NUM_JOY_PROFILES];
	PROFILE_TYPE active_profile;
	int active_profile_num;
	bool pressed;
};


//_______________
class AXIS_LIMITS {
public:
	AXIS_LIMITS() {
		min = 0.0f;
		max = 1.0f;
		centre = 0.5f;
		span = 1.0f;
		centre_min = 0.0f;
		centre_max = 1.0f;
		centre_span = 1.0f;

	}
	double min;
	double max;
	double centre;
	double span;
	double centre_min;
	double centre_max;
	double centre_span;
};


//_______________
class ACTION_AXIS {
public:
	ACTION_AXIS() {
		current_val = 0;
		current_centred_val = 0;
		for (int i = 0; i < NUM_JOY_PROFILES; i++) {
			assiged_to[i] = AXIS_TYPE::None;
			rev_axis[i] = FALSE;
			is_centred[i] = FALSE;
		}
		calibrating = FALSE;
	};
	AXIS_LIMITS* Get_Axis_Limits() { return &limits; };
	AXIS_TYPE Get_Axis_As() const { return assiged_to[static_cast<int>(current_pro_type)]; };
	BOOL Is_Axis_Reversed() const { return rev_axis[static_cast<int>(current_pro_type)]; };
	P2_ACTIONS Get_Button_Action_Min() const { return button_min.GetAction(); };
	P2_ACTIONS Get_Button_Action_Max() const { return button_max.GetAction(); };
	double Get_Current_Val() const { return current_val; };
	void Set_Axis_As(AXIS_TYPE axis_type) {
		assiged_to[static_cast<int>(current_pro_type)] = axis_type;
		int pro_int = static_cast<int>(current_pro_type);
		if (assiged_to[pro_int] == AXIS_TYPE::Pointer_X || assiged_to[pro_int] == AXIS_TYPE::Pointer_Y || assiged_to[pro_int] == AXIS_TYPE::Yaw || assiged_to[pro_int] == AXIS_TYPE::Pitch || assiged_to[pro_int] == AXIS_TYPE::Roll || assiged_to[pro_int] == AXIS_TYPE::AsTwoButtons)
			is_centred[static_cast<int>(current_pro_type)] = TRUE;
		else
			is_centred[static_cast<int>(current_pro_type)] = FALSE;
	};
	void Set_Axis_Reversed(BOOL is_rev_axis) { rev_axis[static_cast<int>(current_pro_type)] = is_rev_axis; };
	void Set_Button_Action_Min(P2_ACTIONS in_action_min) { button_min.Set_Action(in_action_min); };
	void Set_Button_Action_Max(P2_ACTIONS in_action_max) { button_max.Set_Action(in_action_max); };
	void Set_State(double axis_val, bool ignore_space_remap_profiles);
	void Calibrate(BOOL state);
	void Centre() {
		if (calibrating)
			return;
		limits.centre = current_val;// current_centred_val;
	}
	//BOOL IsCentred() const { return is_centred[static_cast<int>(current_pro_type)]; };
protected:
private:
	double current_val;
	double current_centred_val;
	AXIS_LIMITS limits;
	AXIS_TYPE assiged_to[NUM_JOY_PROFILES];
	BOOL rev_axis[NUM_JOY_PROFILES];
	BOOL is_centred[NUM_JOY_PROFILES];
	BOOL calibrating;
	ACTION_KEY button_min;
	ACTION_KEY button_max;
};


//_________________
class ACTION_SWITCH {
public:
	ACTION_SWITCH() {
		num_positions = 0;
		for (int i = 0; i < NUM_JOY_PROFILES; i++)
			action[i] = nullptr;
		current_position = 0;
		active_profile = PROFILE_TYPE::GUI;
	};
	~ACTION_SWITCH() {
		for (int i = 0; i < NUM_JOY_PROFILES; i++) {
			if (action[i])
				delete action[i];
			action[i] = nullptr;
		}
	}
	void Set_Num_Positions(int in_num_positions) {
		for (int i = 0; i < NUM_JOY_PROFILES; i++) {
			if (action[i])
				delete action[i];
		}
		num_positions = in_num_positions + 1;
		for (int i = 0; i < NUM_JOY_PROFILES; i++)
			action[i] = new P2_ACTIONS[num_positions]{ P2_ACTIONS::None };

		current_position = 0;
	}
	int Switch_Position(int new_pos);
	bool Set_Action(int pos, P2_ACTIONS in_action) {
		if (!action[static_cast<int>(current_pro_type)] || pos < 0 || pos >= num_positions)
			return false;
		action[static_cast<int>(current_pro_type)][pos] = in_action;
		return true;
	}
	P2_ACTIONS GetAction(int pos) const {
		if (pos < 0 || pos >= num_positions)
			return P2_ACTIONS::None;
		return action[static_cast<int>(current_pro_type)][pos];
	}
	int Get_Num_Positions() const { return num_positions; };
	int Get_Current_Position() const { return current_position; };
protected:
private:
	P2_ACTIONS* action[NUM_JOY_PROFILES];
	int current_position;
	int num_positions;
	PROFILE_TYPE active_profile;
};


//_____________
class JOYSTICK {
public:
	JOYSTICK(winrt::Windows::Gaming::Input::RawGameController const& in_rawGameController);
	~JOYSTICK() {
		if (axisArray)
			delete[] axisArray;
		axisArray = nullptr;
		if (action_axis)
			delete[] action_axis;
		action_axis = nullptr;

		if (buttonArray)
			delete[] buttonArray;
		buttonArray = nullptr;

		if (switchArray)
			delete[] switchArray;
		switchArray = nullptr;


			if (action_button)
				delete[] action_button;
			action_button = nullptr;

			if (action_switch)
				delete[] action_switch;
			action_switch = nullptr;
		
	};
	bool IsConnected() const { return connected; };
	bool Connect(winrt::Windows::Gaming::Input::RawGameController const& in_rawGameController);
	bool DisConnect(winrt::Windows::Gaming::Input::RawGameController const& in_rawGameController);
	bool IsEnabled() const { return enabled; };
	bool Enable(bool enable) { enabled = enable; return enabled; };
	void Update();
	int Axes(double** pp_axisArray) {
		if (pp_axisArray)
			*pp_axisArray = axisArray;
		return num_axes;
	};
	int Buttons(bool** pp_buttonArray) {
		if (pp_buttonArray)
			*pp_buttonArray = buttonArray;
		return num_buttons;
	};
	int Switches(winrt::Windows::Gaming::Input::GameControllerSwitchPosition** pp_switchArray) {
		if (pp_switchArray)
			*pp_switchArray = switchArray;
		return num_switches;
	};
	winrt::hstring Get_NonRoamableId() const { return NonRoamableId; };
	winrt::hstring Get_DisplayName() const { return DisplayName; };
	BOOL Profile_Load();
	BOOL Profile_Load(const wchar_t* file_path);
	BOOL Profile_Save();
	BOOL Profile_Save(const wchar_t* file_path);
	ACTION_AXIS* Get_Action_Axis(int axis_num) {
		if (axis_num < 0 || axis_num >= num_axes)
			return nullptr;
		return &action_axis[axis_num];
	}
	ACTION_KEY* Get_Action_Button(int button_num) {
		if (button_num < 0 || button_num >= num_buttons)
			return nullptr;
		return &action_button[button_num];
	};
	ACTION_SWITCH* Get_Action_Switch(int switch_num) {
		if (switch_num < 0 || switch_num >= num_switches)
			return nullptr;
		return &action_switch[switch_num];
	};
	USHORT Get_PID() const { return pid; };
	USHORT Get_VID() const { return vid; };
protected:
private:
	bool connected;
	bool enabled;
	winrt::Windows::Gaming::Input::RawGameController rawGameController;

	USHORT pid;
	USHORT vid;
	winrt::hstring NonRoamableId;
	winrt::hstring DisplayName;

	int num_axes;
	double* axisArray;
	ACTION_AXIS* action_axis;

	int num_buttons;
	bool* buttonArray;
	ACTION_KEY* action_button;

	int num_switches;
	winrt::Windows::Gaming::Input::GameControllerSwitchPosition* switchArray;
	ACTION_SWITCH* action_switch;
};


//_____________
class JOYSTICKS {
public:
	JOYSTICKS() {
		joysticks = {};
		setup = false;
		deadzone = 0;
		concurrency::critical_section::scoped_lock s0{ controllerListLock };
		//Setup();
	}
	~JOYSTICKS() {
		concurrency::critical_section::scoped_lock s8{ controllerListLock };
		for (auto& joysticks : joysticks)
			delete joysticks;
	};
	JOYSTICK* Get_Joy(winrt::hstring NonRoamableId) {
		for (auto& joystick : joysticks) {
			if (NonRoamableId == joystick->Get_NonRoamableId())
				return joystick;
		}
	};
	void Setup();
	void Update();
	JOYSTICK* GetJoy(UINT num) {
		concurrency::critical_section::scoped_lock s4{ controllerListLock };
		if (num < joysticks.size())
			return joysticks.at(num);
		return nullptr;
	}
	winrt::hstring GetJoyName(UINT num) {
		concurrency::critical_section::scoped_lock s5{ controllerListLock };
		if (num < joysticks.size())
			return joysticks.at(num)->Get_DisplayName();
		return L"";
	}
	size_t GetNumJoysticks() {
		concurrency::critical_section::scoped_lock s6{ controllerListLock };
		return joysticks.size();
	};
	//Centre all axes assiged to Yaw, Pitch and Roll on enabled joysticks.
	void Centre_All();
	BOOL Save();
	BOOL Load();
	int Deadzone_Level() const { return deadzone; };
	void Set_Deadzone_Level(int in_deadzone) {
		if (in_deadzone < 0)
			in_deadzone = 0;
		if (in_deadzone > 10)
			in_deadzone = 10;
		deadzone = in_deadzone; 
	};
protected:
private:
	std::vector<JOYSTICK*> joysticks;
	concurrency::critical_section controllerListLock;
	//std::mutex mut1;

	bool setup;
	int deadzone;
};


//_________
class MOUSE {
public:
	MOUSE() {
		setup = false;
		deadzone_level = 0;
		deadzone = 0;

	};
	~MOUSE() {

	}
	void Setup();
	void Load();
	void Save();
	void Update_Buttons(WPARAM wParam);
	void Update_Button(int button, bool state);
	void Update_Wheel_Vertical(WPARAM wParam);
	void Update_Wheel_Horizontal(WPARAM wParam);
	void Set_Deadzone_Level(int in_deadzone_level) {
		if (in_deadzone_level < 0)
			in_deadzone_level = 0;
		if (in_deadzone_level > 10)
			in_deadzone_level = 10;
		deadzone_level = in_deadzone_level;
		deadzone = deadzone_level * 10;
	};
	int Deadzone_Level() const { return deadzone_level; };
	int Deadzone() const { return deadzone; };

	P2_ACTIONS GetAction_Button(int button);
	P2_ACTIONS GetAction_Wheel_Up();
	P2_ACTIONS GetAction_Wheel_Down();
	P2_ACTIONS GetAction_Wheel_Left();
	P2_ACTIONS GetAction_Wheel_Right();

	void SetAction_Button(int button, P2_ACTIONS action);
	void SetAction_Wheel_Up(P2_ACTIONS action);
	void SetAction_Wheel_Down(P2_ACTIONS action);
	void SetAction_Wheel_Left(P2_ACTIONS action);
	void SetAction_Wheel_Right(P2_ACTIONS action);
	//bool IsButton_Pressed(int button) { return action_key_button[button].Is_Pressed(); };
protected:
private:
	ACTION_KEY_MOUSE action_key_button[NUM_MOUSE_BUTTONS];
	ACTION_KEY_MOUSE action_key_wheel_v[2];
	ACTION_KEY_MOUSE action_key_wheel_h[2];

	bool setup;
	int deadzone_level;
	int deadzone;
};


struct P2_JOY_AXES {
	double x;
	double y;
	double r;
	double t;


	BOOL yaw_as_roll;
	BOOL button_mod;

	BOOL x_neg;
	BOOL x_pos;
	BOOL y_neg;
	BOOL y_pos;

	BOOL y_pos_x_neg;
	BOOL y_pos_x_pos;
	BOOL y_neg_x_neg;
	BOOL y_neg_x_pos;

	BOOL r_neg;
	BOOL r_pos;
};


extern P2_JOY_AXES p2_joy_axes;

extern JOYSTICKS Joysticks;

bool Get_Joystick_Config_Path(std::wstring* p_ret_string);

void Simulate_Key_Press(P2_ACTIONS action);
void Simulate_Key_Release(P2_ACTIONS action);
void Simulate_Key_Pressed(P2_ACTIONS action, LONG duration_ms);
void Check_Simulated_Key_For_Release();
void Update_Axis_Keys();
void Reset_Key_Throttle();
void Maintain_Axis_Limits();

//keys
bool Get_Key_State(BYTE key, BYTE mod_key_flags, BYTE run_once);
BOOL Keys_Save();
BOOL Keys_Load();

//mouse
void Get_Mouse_Position(LONG* p_x, LONG* p_y);
//check double click for mouse, controller and keyboard.
void Check_Mouse_Double_Click();

extern MOUSE Mouse;
extern WORD mouse_state_space[3];
extern bool controller_enhancements_enabled;

extern bool mouse_double_click_left;