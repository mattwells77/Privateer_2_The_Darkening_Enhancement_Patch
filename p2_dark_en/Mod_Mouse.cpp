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

#include "Display_DX11.h"
#include "modifications.h"
#include "memwrite.h"
#include "dark.h"
#include "input.h"

LONG mouse_client_x = 0;
LONG mouse_client_y = 0;

//___________________________________________
void Get_Mouse_Position(LONG* p_x, LONG* p_y) {
    if (!*p_p2_is_app_active)
        return;
    LONG x = 0;
    LONG y = 0;

    POINT p{ 0,0 };
    if (ClientToScreen(*p_p2_hWinMain, &p)) {
        POINT m{ 0,0 };
        GetCursorPos(&m);

        if (current_pro_type == PROFILE_TYPE::GUI) {
            //maintain GUI cursor speed for controller and keyboard control by multiplying movement by time past.
            static LARGE_INTEGER lastTime = { 0LL };
            LARGE_INTEGER time = { 0 };
            QueryPerformanceCounter(&time);
            if (p2_joy_axes.x || p2_joy_axes.y) {
                LARGE_INTEGER ElapsedMicroseconds = { 0 };
                ElapsedMicroseconds.QuadPart = time.QuadPart - lastTime.QuadPart;
                ElapsedMicroseconds.QuadPart *= 1000000LL;
                ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

                //maintain cursor speed across different screen resolutions by multiplying by the scaled surface unit height.
                float fscale_y = 0;
                surface_gui->GetScaledPixelDimensions(nullptr, &fscale_y);

                float fmul = (float)(ElapsedMicroseconds.QuadPart) / 160000;
                if (fmul < 0.01f)//set a minimum threshold, in order to still allow for movement in high frequency sampling situations eg. conversation choice.
                    fmul = 0.01f;
                else if (fmul > 0.5f)
                    fmul = 0.5f;

                //Debug_Info("Get_Mouse_Position: %f", fmul);
                LONG axis_x = (LONG)(p2_joy_axes.x * 128 * fmul * fscale_y);
                LONG axis_y = (LONG)(p2_joy_axes.y * 128 * fmul * fscale_y);
                m.x += axis_x;
                x += axis_x;
                m.y += axis_y;
                y += axis_y;
                SetCursorPos(m.x, m.y);
            }
            lastTime = time;
        }

        x = (m.x - p.x);
        y = (m.y - p.y);

        mouse_client_x = x;
        mouse_client_y = y;

        if (surface_gui) {
            float fx = 0;
            float fy = 0;
            surface_gui->GetPosition(&fx, &fy);
            x = (LONG)((x - fx) * GUI_WIDTH / surface_gui->GetScaledWidth());
            y = (LONG)((y - fy) * GUI_HEIGHT / surface_gui->GetScaledHeight());
        }
        else {
            x = x * GUI_WIDTH / clientWidth;
            y = y * GUI_HEIGHT / clientHeight;
        }
    }

    if (x < 0)
        x = 0;
    else if (x >= GUI_WIDTH)
        x = GUI_WIDTH - 1;
    if (y < 0)
        y = 0;
    else if (y >= GUI_HEIGHT)
        y = GUI_HEIGHT - 1;

    *p_x = x;
    *p_y = y;
    //Debug_Info("Get_Mouse_Pos x:%d, y:%d", *p_x, *p_y);
}


//__________________________________________________________
static void Get_Mouse_Position16(int16_t* p_x, int16_t* p_y) {
    LONG x = 0;
    LONG y = 0;
    Get_Mouse_Position(&x, &y);
    *p_x = (int16_t)x;
    *p_y = (int16_t)y;
}

//_______________________________________________
static void __declspec(naked) get_mouse_pos(void) {

    __asm {
        push ebx
        push ecx
        push esi
        push edi

        push edx
        push eax
        call Get_Mouse_Position16
        add esp, 0x8

        pop edi
        pop esi
        pop ecx
        pop ebx

        ret
    }
}


//__________________________________________________________________
static void Get_Mouse_Buttons(WORD* p_butt_left, WORD* p_butt_right) {

    *p_butt_left = p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Left_Click)][0]] >> 7;
    *p_butt_right = p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Right_Click)][0]] >> 7;

    if (current_pro_type == PROFILE_TYPE::GUI) {
        *p_butt_left |= p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Left_Click_Key_GUI)][0]] >> 7;
        *p_butt_right |= p2_keyboard_state_main[P2_ACTIONS_KEYS[static_cast<int>(P2_ACTIONS::Right_Click_Key_GUI)][0]] >> 7;
    }
}


//___________________________________________________
static void __declspec(naked) get_mouse_buttons(void) {

    __asm {
        push ebx
        push ecx
        push esi
        push edi

        push edx
        push eax
        call Get_Mouse_Buttons
        add esp, 0x8

        pop edi
        pop esi
        pop ecx
        pop ebx

        ret
    }
}


//______________________________________________________________________
static void Get_Mouse_Buttons_GUI(WORD* p_butt_left, WORD* p_butt_right) {

    static WORD butt_left_last = 0;
    static WORD butt_right_last = 0;

    Get_Mouse_Buttons(p_butt_left, p_butt_right);

    if (*p_butt_left) {
        if (butt_left_last)
            *p_butt_left = 0;
        else
            butt_left_last = 1;
    }
    else
        butt_left_last = 0;

    if (*p_butt_right) {
        if (butt_right_last)
            *p_butt_right = 0;
        else
            butt_right_last = 1;
    }
    else
        butt_right_last = 0;
}


//_______________________________________________________
static void __declspec(naked) get_mouse_buttons_gui(void) {

    __asm {
        push ebx
        push ecx
        push esi
        push edi
        push ebp

        push edx
        push eax
        call Get_Mouse_Buttons_GUI
        add esp, 0x8

        pop ebp
        pop edi
        pop esi
        pop ecx
        pop ebx

        ret
    }
}


//____________________________________________
static void Set_Mouse_Position(LONG x, LONG y) {

    POINT client{ 0,0 };
    if (ClientToScreen(*p_p2_hWinMain, &client)) {

        float fx = 0;
        float fy = 0;
        float fwidth = (float)clientWidth;
        float fheight = (float)clientHeight;
        if (surface_gui) {
            surface_gui->GetPosition(&fx, &fy);
            fwidth = surface_gui->GetScaledWidth();
            fheight = surface_gui->GetScaledHeight();
        }

        fx += x * fwidth / GUI_WIDTH;
        LONG ix = (LONG)fx;
        if ((float)ix != fx)
            ix++;
        mouse_client_x = ix;
        ix += client.x;

        fy += y * fheight / GUI_HEIGHT;
        LONG iy = (LONG)fy;
        if ((float)iy != fy)
            iy++;
        mouse_client_y = iy;
        iy += client.y;

        SetCursorPos(ix, iy);
    }

    if (x < 0)
        x = 0;
    else if (x >= GUI_WIDTH)
        x = GUI_WIDTH - 1;
    if (y < 0)
        y = 0;
    else if (y >= GUI_HEIGHT)
        y = GUI_HEIGHT - 1;
}


//_______________________________________________
static void __declspec(naked) set_mouse_pos(void) {

    __asm {
        push ebx
        push ecx
        push esi
        push edi
        push ebp

        push edx
        push eax
        call Set_Mouse_Position
        add esp, 0x8

        pop ebp
        pop edi
        pop esi
        pop ecx
        pop ebx

        ret
    }
}


//_______________________________________________________
static void __declspec(naked) check_pad_doubleclick(void) {

    __asm {
        cmp mouse_double_click_left, 0
        je endfunc

        mov word ptr ds : [eax + 0xEDA], 1//set clicked true
        mov word ptr ds : [eax + 0xEDC], 1//set double clicked true
        endfunc:
        ret
    }
}


//____________________________________________________________
static void Get_Space_Mouse_Movement(float* p_fx, float* p_fy) {

    LONG centre_x = clientWidth / 2;
    LONG centre_y = clientHeight / 2;

    LONG x = mouse_client_x - centre_x;
    LONG y = mouse_client_y - centre_y;

    LONG range = centre_y;
    if (centre_y > centre_x)
        range = centre_x;

    range = range * Mouse.Axis_Limit_Percentage() / 100;

    if (x > range)
        x = range;
    else if (x < -range)
        x = -range;

    if (y > range)
        y = range;
    else if (y < -range)
        y = -range;

    //convert mouse movement value to the ships axis range between -1 and 1.
    *p_fx = (float)x / range;
    *p_fy = (float)y / range;

    float f_deadzone = 1.0f / 32 * Mouse.Deadzone_Level();

    if (*p_fx <= f_deadzone && *p_fx >= -f_deadzone)
        *p_fx = 0;
    if (*p_fy <= f_deadzone && *p_fy >= -f_deadzone)
        *p_fy = 0;

    if (Mouse.Is_Y_Axis_Inverted())
        *p_fy = -*p_fy;
}


//________________________________________________
static void Update_Space_Mouse(BYTE* space_struct) {

    int16_t* p_mouse_x = (int16_t*)(space_struct + SPACE_STRUCT_MOUSE_X);
    int16_t* p_mouse_y = (int16_t*)(space_struct + SPACE_STRUCT_MOUSE_Y);
    Get_Mouse_Position16(p_mouse_x, p_mouse_y);

    uint16_t* p_mouse_button_1 = (uint16_t*)(space_struct + SPACE_STRUCT_MOUSE_BUTTON01);
    uint16_t* p_mouse_button_2 = (uint16_t*)(space_struct + SPACE_STRUCT_MOUSE_BUTTON02);
    Get_Mouse_Buttons(p_mouse_button_1, p_mouse_button_2);
    
    uint16_t* p_mouse_button_pressed = (uint16_t*)(space_struct + SPACE_STRUCT_MOUSE_BUTTON_PRESSED);
    if (p_mouse_button_1 || p_mouse_button_2)
        *p_mouse_button_pressed = 1;

    if (*p_p2_controller_flags != 0)
        return;

    int16_t* p_mouse_x_centred = (int16_t*)(space_struct + SPACE_STRUCT_MOUSE_X_CENTRED);
    int16_t* p_mouse_y_centred = (int16_t*)(space_struct + SPACE_STRUCT_MOUSE_Y_CENTRED);

    float f_mouse_x = 0;
    float f_mouse_y = 0;
    Get_Space_Mouse_Movement(&f_mouse_x, &f_mouse_y);

    *p_mouse_x_centred = (int16_t)(f_mouse_x * 240);
    *p_mouse_y_centred = (int16_t)(f_mouse_y * 240);

    void* p_pc_ship_struct = (void*)(space_struct + (*p_p2_space_struct_number_of_objects * SPACE_OBJECT_STRUCT_SIZE));

    *(float*)((BYTE*)p_pc_ship_struct + SPACE_PC_SHIP_STRUCT_FX_OFFSET) = -f_mouse_x;

    *(float*)((BYTE*)p_pc_ship_struct + SPACE_PC_SHIP_STRUCT_FR_OFFSET) = -(float)p2_joy_axes.r;
    //uint16_t* p_mouse_button_2 = (uint16_t*)(space_struct + 0x14E);
    if (*p_mouse_button_2) {
        *(float*)((BYTE*)p_pc_ship_struct + SPACE_PC_SHIP_STRUCT_FR_OFFSET) = -f_mouse_x;
        *(float*)((BYTE*)p_pc_ship_struct + SPACE_PC_SHIP_STRUCT_FX_OFFSET) = 0;
    }
    else if (Is_Alt_Flight_Mode()) {
        *(float*)((BYTE*)p_pc_ship_struct + SPACE_PC_SHIP_STRUCT_FR_OFFSET) += -f_mouse_x;
        if (*(float*)((BYTE*)p_pc_ship_struct + SPACE_PC_SHIP_STRUCT_FR_OFFSET) > 1.0f)
            *(float*)((BYTE*)p_pc_ship_struct + SPACE_PC_SHIP_STRUCT_FR_OFFSET) = 1.0f;
        else if (*(float*)((BYTE*)p_pc_ship_struct + SPACE_PC_SHIP_STRUCT_FR_OFFSET) < -1.0f)
            *(float*)((BYTE*)p_pc_ship_struct + SPACE_PC_SHIP_STRUCT_FR_OFFSET) = -1.0f;
    }

    *(float*)((BYTE*)p_pc_ship_struct + SPACE_PC_SHIP_STRUCT_FY_OFFSET) = -f_mouse_y;
}


//____________________________________________________
static void __declspec(naked) update_space_mouse(void) {

    __asm {
        push ebp

        push ebp
        call Update_Space_Mouse
        add esp, 0x4;

        pop ebp
        ret
    }
}


//________________________
void Modifications_Mouse() {

    //replace direct input mouse functions with winapi
    MemWrite8(0x4189D0, 0x53, 0xE9);
    FuncWrite32(0x4189D1, 0x83575651, (DWORD)&get_mouse_pos);

    MemWrite8(0x418A34, 0x53, 0xE9);
    FuncWrite32(0x418A35, 0x08EC8351, (DWORD)&get_mouse_buttons);

    MemWrite8(0x418A50, 0x53, 0xE9);
    FuncWrite32(0x418A51, 0x083575651, (DWORD)&get_mouse_buttons_gui);
    MemWrite16(0x418A55, 0x08EC, 0x9090);

    MemWrite8(0x4189C4, 0x0F, 0xE9);
    FuncWrite32(0x4189C5, 0xE998D2BF, (DWORD)&set_mouse_pos);
    MemWrite32(0x4189C9, 0x05199B, 0x90909090);

    //fix doubleclick on map location in P.A.D.
    MemWrite16(0x436B9D, 0x0C75, 0x9090);
    MemWrite8(0x436BA2, 0x66, 0x90);
    MemWrite16(0x436BA3, 0x80FF, 0xE890);
    FuncWrite32(0x436BA5, 0x0EDE, (DWORD)&check_pad_doubleclick);

    //update space mouse control
    FuncReplace32(0x44FDE8, 0xFFFF67D8, (DWORD)&update_space_mouse);
    //jump over mouse movement section.
    MemWrite16(0x4501FA, 0x850F, 0xE990); //JMP 00450369 to mouse button section.
    MemWrite32(0x4501FC, 0x01F6, 0x0169);
    //jump over button 2 check, done in Update_Space_Mouse function.
    MemWrite8(0x4503A7, 0x74, 0xEB);
}