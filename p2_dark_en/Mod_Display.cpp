/*
The MIT License (MIT)
Copyright © 2025-2026 Matt Wells

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
#include "configTools.h"
#include "libvlc_Movies.h"
#include "dark.h"
#include "input.h"
#include "input_config.h"

#define WIN_MODE_STYLE  WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX

bool is_windowed = false;

BOOL clip_cursor = FALSE;
bool is_cursor_clipped = false;

UINT clientWidth = GUI_WIDTH;
UINT clientHeight = GUI_HEIGHT;


LARGE_INTEGER Frequency = { 0LL };

float f_cycle_Hz_GUI = 30.0f;
float f_cycle_Hz_Space = 30.0f;

HMODULE hinst_DARK = nullptr;

//flag: clear buffer after drawing in "Draw_Image_Buffer_Rect_Space_Main" function.
BOOL clear_after_space_draw = FALSE;


//___________________________________
static void Multimedia_Wait(float Hz) {
  
    LARGE_INTEGER waitTime = { 0LL };

    waitTime.QuadPart = (LONGLONG)(1000000.0f / Hz);
    waitTime.QuadPart *= Frequency.QuadPart;
    waitTime.QuadPart /= 1000000LL;

    static LARGE_INTEGER time = { 0 };

    waitTime.QuadPart += time.QuadPart;

    while (waitTime.QuadPart > time.QuadPart) {
        QueryPerformanceCounter(&time);
    }
}


//___________________________
static BOOL IsMouseInClient() {
    //check if mouse within client rect.
    RECT rcClient;
    POINT p{ 0,0 }, m{ 0,0 };

    GetCursorPos(&m);

    ClientToScreen(*p_p2_hWinMain, &p);
    GetClientRect(*p_p2_hWinMain, &rcClient);

    rcClient.left += p.x;
    rcClient.top += p.y;
    rcClient.right += p.x;
    rcClient.bottom += p.y;


    if (m.x < rcClient.left || m.x > rcClient.right)
        return FALSE;
    if (m.y < rcClient.top || m.y > rcClient.bottom)
        return FALSE;
    return TRUE;
}


//___________________________
static BOOL ClipMouseCursor() {

    POINT p{ 0,0 };
    if (!ClientToScreen(*p_p2_hWinMain, &p))
        return FALSE;
    RECT rcClient;
    if (!GetClientRect(*p_p2_hWinMain, &rcClient))
        return FALSE;
    rcClient.left += p.x;
    rcClient.top += p.y;
    rcClient.right += p.x;
    rcClient.bottom += p.y;

    return ClipCursor(&rcClient);
}


//________________________________________________________________________________
static void SetWindowTitle(HWND hwnd, const wchar_t* msg, UINT width, UINT height) {

    wchar_t winText[64];
    swprintf_s(winText, 64, L"%S  @%ix%i   %s", p_p2_szAppName, width, height, msg);
    SendMessage(hwnd, WM_SETTEXT, (WPARAM)0, (LPARAM)winText);

}


//_______________________________________________________
static void SetWindowTitle(HWND hwnd, const wchar_t* msg) {

    SetWindowTitle(hwnd, msg, clientWidth, clientHeight);
}


//___________________________________________________________________________________________
static bool Check_Window_GUI_Scaling_Limits(HWND hwnd, RECT* p_rc_win, bool set_window_title) {

    if (!p_rc_win)
        return false;
    bool resized = false;
    DWORD dwStyle = 0;
    DWORD dwExStyle = 0;
    dwStyle = GetWindowLong(hwnd, GWL_STYLE);
    dwExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

    //get the dimensions of the window frame style.
    RECT rc_style{ 0,0,0,0 };
    AdjustWindowRectEx(&rc_style, dwStyle, false, dwExStyle);
    RECT rc_client;
    CopyRect(&rc_client, p_rc_win);
    //subtract the window style rectangle leaving the client rectangle.
    rc_client.left -= rc_style.left;
    rc_client.top -= rc_style.top;
    rc_client.right -= rc_style.right;
    rc_client.bottom -= rc_style.bottom;

    LONG client_width = rc_client.right - rc_client.left;
    LONG client_height = rc_client.bottom - rc_client.top;

    //prevent window dimensions going beyond what is supported by your graphics card.
    if (client_width > (LONG)max_texDim || client_height > (LONG)max_texDim) {
        if (client_width > (LONG)max_texDim)
            client_width = (LONG)max_texDim;
        if (client_height > (LONG)max_texDim)
            client_height = (LONG)max_texDim;
        rc_client.right = rc_client.left + client_width;
        rc_client.bottom = rc_client.top + client_height;
        //add the client and style rects to get the window rect.
        p_rc_win->left = rc_client.left + rc_style.left;
        p_rc_win->top = rc_client.top + rc_style.top;
        p_rc_win->right = rc_client.right + rc_style.right;
        p_rc_win->bottom = rc_client.bottom + rc_style.bottom;
        resized = true;
    }

    //prevent window dimensions going under the minumum values of 640x480.
    if (client_width < GUI_WIDTH || client_height < GUI_HEIGHT) {
        if (client_width < GUI_WIDTH)
            client_width = GUI_WIDTH;
        if (client_height < GUI_HEIGHT)
            client_height = GUI_HEIGHT;

        rc_client.right = rc_client.left + client_width;
        rc_client.bottom = rc_client.top + client_height;
        //add the client and style rects to get the window rect.
        p_rc_win->left = rc_client.left + rc_style.left;
        p_rc_win->top = rc_client.top + rc_style.top;
        p_rc_win->right = rc_client.right + rc_style.right;
        p_rc_win->bottom = rc_client.bottom + rc_style.bottom;
        resized = true;
    }
    if (set_window_title)
        SetWindowTitle(hwnd, L"", client_width, client_height);
    //Debug_Info("Check_Window_GUI_Scaling_Limits w:%d, h:%d", client_width, client_height);
    return resized;
}


//________________________
static bool Display_Exit() {

    Display_Dx_Destroy();
    return 0;
}


//_________________________________
static BOOL Window_Setup(HWND hwnd) {

    Check_Command_Line_Overrides();

    hinst_DARK = GetModuleHandleW(nullptr);

    QueryPerformanceFrequency(&Frequency);
    
    if (ConfigReadInt_InGame(L"MAIN", L"WINDOWED", CONFIG_MAIN_WINDOWED))
        is_windowed = true;
    
    if (is_windowed) {
        Debug_Info("Window Setup: Windowed");
        WINDOWPLACEMENT winPlace{ 0 };
        winPlace.length = sizeof(WINDOWPLACEMENT);
        
        SetWindowLongPtr(hwnd, GWL_STYLE, WIN_MODE_STYLE);
        //Debug_Info("is_windowed set style");

        if (ConfigReadWinData(L"MAIN", L"WIN_DATA", &winPlace)) {
            if (winPlace.showCmd != SW_MAXIMIZE)
                winPlace.showCmd = SW_SHOWNORMAL;
        }
        else {
            GetWindowPlacement(hwnd, &winPlace);
            winPlace.showCmd = SW_SHOWNORMAL;
            Debug_Info("is_windowed GetWindowPlacement");
        }
        if (winPlace.showCmd == SW_SHOWNORMAL) //if the window isn't maximized
            Check_Window_GUI_Scaling_Limits(hwnd, &winPlace.rcNormalPosition, false);
        
        SetWindowPlacement(hwnd, &winPlace);
    }
    else {
        Debug_Info("Window Setup: Fullscreen");
        SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP);
        SetWindowPos(hwnd, 0, 0, 0, 0, 0, 0);
        ShowWindow(hwnd, SW_MAXIMIZE);
    }

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    //Get the window client width and height.
    clientWidth = clientRect.right - clientRect.left;
    clientHeight = clientRect.bottom - clientRect.top;

    if (!Display_Dx_Setup(hwnd, clientWidth, clientHeight))
        p2_error_message_box("DirectX Failed to Start");

    Debug_Info("Window Setup: Done");

    Check_Optional_Enhancements();
    Keys_Load();

    f_cycle_Hz_GUI = (float)ConfigReadInt(L"MAIN", L"CYCLE_HZ_GUI", CONFIG_MAIN_CYCLE_HZ_GUI);
    f_cycle_Hz_Space = (float)ConfigReadInt(L"SPACE", L"CYCLE_HZ_SPACE", CONFIG_SPACE_CYCLE_HZ_SPACE);

    return 1;
}


//______________________________________________
static void __declspec(naked) setup_window(void) {

    __asm {
        push ebx
        push ecx
        push edx
        push edi
        push esi
        push ebp

        mov eax, p_p2_hWinMain
        mov eax, dword ptr ds : [eax]
        push eax
        call Window_Setup
        add esp, 0x4


        //0046A7A5 | > \BA 00D80400   MOV EDX, 4D800; UNICODE "efaultdiscovery-l1-1-0"
        //0046A7AA | .B8 01000000   MOV EAX, 1
        //0046A7AF | .E8 20160000   CALL 0046BDD4; [DARK.0046BDD4
        //0046A7B4 | .A3 18004F00   MOV DWORD PTR DS : [4F0018] , EAX


        //create a buffer to draw to 640 x 496 ?
        mov edx, 0x4D800
        mov eax, 0x1
        mov edi, 0x046BDD4
        call edi
        mov edi, 0x4F0018
        mov dword ptr ds:[edi], eax

        pop ebp
        pop esi
        pop edi
        pop edx
        pop ecx
        pop ebx

        ret

    }
}


//_____________________________________
void SetWindowActivation(BOOL isActive) {

    //When game window loses focus, fullscreen mode needs to temporarily be put into windowed mode in order to appear on the taskbar and alt-tab display.
    if (!is_windowed) {
        if (isActive == FALSE) {//Convert to windowed mode when app loses focus.
            SetWindowLongPtr(*p_p2_hWinMain, GWL_EXSTYLE, 0);
            SetWindowLongPtr(*p_p2_hWinMain, GWL_STYLE, WIN_MODE_STYLE | WS_VISIBLE);
            SetWindowPos(*p_p2_hWinMain, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
            ShowWindow(*p_p2_hWinMain, SW_RESTORE);
            //Debug_Info("SetWindowActivation full to win");
        }
        else if (isActive) {//Return to fullscreen mode when app regains focus.
            SetWindowLongPtr(*p_p2_hWinMain, GWL_EXSTYLE, WS_EX_TOPMOST);
            SetWindowLongPtr(*p_p2_hWinMain, GWL_STYLE, WS_POPUP | WS_VISIBLE);
            SetWindowPos(*p_p2_hWinMain, HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
            ShowWindow(*p_p2_hWinMain, SW_MAXIMIZE);
            //Debug_Info("SetWindowActivation win to full");
        }
    }
}


//__________________________
static void Window_Resized() {

    RECT clientRect;
    GetClientRect(*p_p2_hWinMain, &clientRect);

    //Get the window client width and height.
    clientWidth = clientRect.right - clientRect.left;
    clientHeight = clientRect.bottom - clientRect.top;

    Display_Dx_Resize(clientWidth, clientHeight);

    if (is_cursor_clipped) {
        //Debug_Info("Window_Resized - is_cursor_clipped");
        if (ClipMouseCursor()) {
            //Debug_Info("Window_Resized - Mouse Cursor Clipped");
        }
    }
    if (is_windowed) {
        WINDOWPLACEMENT winPlace{ 0 };
        winPlace.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(*p_p2_hWinMain, &winPlace);
        ConfigWriteWinData(L"MAIN", L"WIN_DATA", &winPlace);
    }
}


//______________________________________
static void Toggle_WindowMode(HWND hwnd) {

    is_windowed = 1 - is_windowed;
    ConfigWriteInt_InGame(L"MAIN", L"WINDOWED", is_windowed);

    if (is_windowed) {
        Debug_Info("Toggle_WindowMode: Windowed");
        WINDOWPLACEMENT winPlace{ 0 };
        winPlace.length = sizeof(WINDOWPLACEMENT);

        SetWindowLongPtr(hwnd, GWL_STYLE, WIN_MODE_STYLE);

        if (ConfigReadWinData(L"MAIN", L"WIN_DATA", &winPlace)) {
            if (winPlace.showCmd != SW_MAXIMIZE)
                winPlace.showCmd = SW_SHOWNORMAL;
        }
        else {
            GetWindowPlacement(hwnd, &winPlace);
            winPlace.showCmd = SW_SHOWNORMAL;
            Debug_Info("is_windowed GetWindowPlacement");
        }
        if (winPlace.showCmd == SW_SHOWNORMAL) //if the window isn't maximized
            Check_Window_GUI_Scaling_Limits(hwnd, &winPlace.rcNormalPosition, false);

        SetWindowPlacement(hwnd, &winPlace);

    }
    else {//Return to fullscreen mode when app regains focus.
        Debug_Info("Toggle_WindowMode: Fullscreen");
        SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP);
        //SetWindowPos(hwnd, 0, 0, 0, 0, 0, 0);
        ShowWindow(hwnd, SW_MAXIMIZE);
    }
    Display_Dx_Present();
}


//_______________________________
void P2_Active_App(BOOL activate) {
    if (activate == FALSE) {
        Debug_Info("P2_Active_App Deactivate");
        *p_p2_is_app_active = FALSE;

        if (*p_p2_hThread_main != nullptr)
            SuspendThread(*p_p2_hThread_main);

        if (*pp_p2_wail32_sample_handle)
            p2_wail32_sample_suspend();
        if (*pp_p2_wail32_midi_sequence_handle)
            p2_wail32_midi_sequence_suspend();

        if (*p_p2_movie_flag_unk != 2)
            return;

        if (!*p_p2_is_app_movie_suspended)
            return;
        p2_movie_active(*pp_p2_movie_active_flag);
        *p_p2_is_app_movie_suspended = TRUE;

        if (is_cursor_clipped) {
            ClipCursor(nullptr);
            is_cursor_clipped = false;
            //Debug_Info("WM_ACTIVATEAPP false, Mouse Cursor Un-Clipped");
        }

        if (pMovie_vlc)
            pMovie_vlc->Pause(true);
    }
    else {
        Debug_Info("P2_Active_App Activate");
        *p_p2_is_app_active = TRUE;

        if (*pp_p2_wail32_sample_handle)
            p2_wail32_sample_resume();
        if (*pp_p2_wail32_midi_sequence_handle)
            p2_wail32_midi_sequence_resume();

        if (*p_p2_hThread_main != nullptr)
            ResumeThread(*p_p2_hThread_main);

        if (*p_p2_is_app_movie_suspended) {
            if (*pp_p2_movie_active_flag) {
                p2_movie_active(*pp_p2_movie_active_flag);

                *p_p2_is_app_movie_suspended = FALSE;
            }
        }
        if (*p_p2_cursor)
            SetCursor(*p_p2_cursor);
        if (is_cursor_clipped) {
            if (ClipMouseCursor()) {
                //Debug_Info("WM_ACTIVATEAPP Mouse Cursor Clipped");
            }
        }
        if (pMovie_vlc)
            pMovie_vlc->Pause(false);
    }
}


//______________________________________________________________________________________
static LRESULT CALLBACK WinProc_Main(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    static bool is_in_sizemove = false;
    static bool is_cursor_hidden = true;

    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
        Mouse.Update_Buttons(wParam);
        return 0;
    case WM_MOUSEWHEEL:
        Mouse.Update_Wheel_Vertical(wParam);
        return 0;
    case WM_MOUSEHWHEEL:
        Mouse.Update_Wheel_Horizontal(wParam);
        return 0;
    //case WM_LBUTTONDBLCLK:
    //case WM_RBUTTONDBLCLK: {
    //    movie_mouse_double_click = true;
    //    return 0;
    //}
    case WM_ERASEBKGND:
        return 1;
    case WM_KEYDOWN:
        if (!(lParam & 0x40000000)) { //the previous key state.
            //BYTE scan_code = (BYTE)((lParam & 0x00FF0000) >> 16);
            //Debug_Info("WM_KEYDOWN scan:%X vk:%X", scan_code, wParam);
            p2_keyboard_state_main[wParam] |= 0x80;
            //avoid issues when the Shift key is down changing the output of number lock keys. Mark both potential key outputs if one is set.
            switch (wParam) {
            case VK_DECIMAL:
                p2_keyboard_state_main[VK_DELETE] |= 0x80;
                break;
            case VK_DELETE:
                p2_keyboard_state_main[VK_DECIMAL] |= 0x80;
                break;
            case VK_NUMPAD0:
                p2_keyboard_state_main[VK_INSERT] |= 0x80;
                break;
            case VK_INSERT:
                p2_keyboard_state_main[VK_NUMPAD0] |= 0x80;
                break;
            case VK_NUMPAD1:
                p2_keyboard_state_main[VK_END] |= 0x80;
                break;
            case VK_END:
                p2_keyboard_state_main[VK_NUMPAD1] |= 0x80;
                break;
            case VK_NUMPAD2:
                p2_keyboard_state_main[VK_DOWN] |= 0x80;
                break;
            case VK_DOWN:
                p2_keyboard_state_main[VK_NUMPAD2] |= 0x80;
                break;
            case VK_NUMPAD3:
                p2_keyboard_state_main[VK_NEXT] |= 0x80;
                break;
            case VK_NEXT:
                p2_keyboard_state_main[VK_NUMPAD3] |= 0x80;
                break;
            case VK_NUMPAD4:
                p2_keyboard_state_main[VK_LEFT] |= 0x80;
                break;
            case VK_LEFT:
                p2_keyboard_state_main[VK_NUMPAD4] |= 0x80;
                break;
            case VK_NUMPAD6:
                p2_keyboard_state_main[VK_RIGHT] |= 0x80;
                break;
            case VK_RIGHT:
                p2_keyboard_state_main[VK_NUMPAD6] |= 0x80;
                break;
            case VK_NUMPAD7:
                p2_keyboard_state_main[VK_HOME] |= 0x80;
                break;
            case VK_HOME:
                p2_keyboard_state_main[VK_NUMPAD7] |= 0x80;
                break;
            case VK_NUMPAD8:
                p2_keyboard_state_main[VK_UP] |= 0x80;
                break;
            case VK_UP:
                p2_keyboard_state_main[VK_NUMPAD8] |= 0x80;
                break;
            case VK_NUMPAD9:
                p2_keyboard_state_main[VK_PRIOR] |= 0x80;
                break;
            case VK_PRIOR:
                p2_keyboard_state_main[VK_NUMPAD9] |= 0x80;
                break;
            default:
                break;
            }
        }
        return 0;
    case WM_KEYUP: {
        //BYTE scan_code = (BYTE)((lParam & 0x00FF0000) >> 16);
        //Debug_Info("WM_KEYU scan:%X vk:%X", scan_code, wParam);
        p2_keyboard_state_main[wParam] = 0x0;
        //avoid issues when the Shift key is down changing the output of number lock keys. Mark both potential key outputs if one is set.
        switch (wParam) {
        case VK_DECIMAL:
            p2_keyboard_state_main[VK_DELETE] = 0x0;
            break;
        case VK_DELETE:
            p2_keyboard_state_main[VK_DECIMAL] = 0x0;
            break;
        case VK_NUMPAD0:
            p2_keyboard_state_main[VK_INSERT] = 0x0;
            break;
        case VK_INSERT:
            p2_keyboard_state_main[VK_NUMPAD0] = 0x0;
            break;
        case VK_NUMPAD1:
            p2_keyboard_state_main[VK_END] = 0x0;
            break;
        case VK_END:
            p2_keyboard_state_main[VK_NUMPAD1] = 0x0;
            break;
        case VK_NUMPAD2:
            p2_keyboard_state_main[VK_DOWN] = 0x0;
            break;
        case VK_DOWN:
            p2_keyboard_state_main[VK_NUMPAD2] = 0x0;
            break;
        case VK_NUMPAD3:
            p2_keyboard_state_main[VK_NEXT] = 0x0;
            break;
        case VK_NEXT:
            p2_keyboard_state_main[VK_NUMPAD3] = 0x0;
            break;
        case VK_NUMPAD4:
            p2_keyboard_state_main[VK_LEFT] = 0x0;
            break;
        case VK_LEFT:
            p2_keyboard_state_main[VK_NUMPAD4] = 0x0;
            break;
        case VK_NUMPAD6:
            p2_keyboard_state_main[VK_RIGHT] = 0x0;
            break;
        case VK_RIGHT:
            p2_keyboard_state_main[VK_NUMPAD6] = 0x0;
            break;
        case VK_NUMPAD7:
            p2_keyboard_state_main[VK_HOME] = 0x0;
            break;
        case VK_HOME:
            p2_keyboard_state_main[VK_NUMPAD7] = 0x0;
            break;
        case VK_NUMPAD8:
            p2_keyboard_state_main[VK_UP] = 0x0;
            break;
        case VK_UP:
            p2_keyboard_state_main[VK_NUMPAD8] = 0x0;
            break;
        case VK_NUMPAD9:
            p2_keyboard_state_main[VK_PRIOR] = 0x0;
            break;
        case VK_PRIOR:
            p2_keyboard_state_main[VK_NUMPAD9] = 0x0;
            break;
        default:
            break;
        }
        return 0;
    }
    case WM_SYSKEYDOWN: 
        if (!(lParam & 0x40000000)) { //the previous key state.
            //BYTE scan_code = (BYTE)((lParam & 0x00FF0000) >> 16);
            //Debug_Info("WM_SYSKEYDOWN scan:%X vk:%X", scan_code, wParam);
            p2_keyboard_state_main[wParam] |= 0x80;

            if ((lParam & (1 << 29)) != 0) { //if ALT key is down.

                if (wParam == VK_RETURN) {
                    if (pMovie_vlc)
                        pMovie_vlc->Pause(true);

                    Toggle_WindowMode(hwnd);
                    return 0;
                }
                else if (wParam == 'J') {
                    if (is_cursor_clipped)
                        ClipCursor(nullptr);
                    JoyConfig_Main();
                    if (is_cursor_clipped)
                        ClipMouseCursor();
                    return 0;
                }
            }
        }
        return 0;
    case WM_SYSKEYUP: {
        //BYTE scan_code = (BYTE)((lParam & 0x00FF0000) >> 16);
        //Debug_Info("WM_SYSKEYUP scan:%X vk:%X", scan_code, wParam);
        p2_keyboard_state_main[wParam] = 0x0;
        //if ((lParam & (1 << 29)) != 0) { //if ALT key is down.
        //    return 0;
        //}
        return 0;
    }
    case WM_KILLFOCUS:
        Debug_Info("WM_KILLFOCUS");
        return 0;
    case WM_INITMENU:
        Debug_Info("WM_INITMENU");
        return 0;
    case WM_MENUCHAR:
        Debug_Info("WM_MENUCHAR %X", MNC_CLOSE<<16);
        return MNC_CLOSE << 16;

    //case WM_MOUSEMOVE:
    case WM_SYSCOMMAND:
        switch ((wParam & 0xFFF0)) {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
            return 0;
            break;
        case SC_MAXIMIZE:
        case SC_RESTORE:
            if (pMovie_vlc)
                pMovie_vlc->Pause(true);
            break;
        default:
            break;
        }
        break;
    case WM_SETCURSOR: {
        //if (*p_p2_cursor)
        //    SetCursor(*p_p2_cursor);
        //return 1;
        DWORD currentWinStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
        if (GetForegroundWindow() == hwnd && (currentWinStyle & WS_POPUP) || (clip_cursor == TRUE)) {
            if (!is_cursor_clipped) {
                if (ClipMouseCursor()) {
                    is_cursor_clipped = true;
                    //Debug_Info("WM_SETCURSOR Mouse Cursor Clipped");
                }
                //else
                //    Debug_Info("WM_SETCURSOR ClipMouseCursor failed");
            }
            break;
        }
        if (is_cursor_clipped) {
            ClipCursor(nullptr);
            is_cursor_clipped = false;
            //Debug_Info("WM_SETCURSOR Mouse Cursor Un-Clipped");
        }
        if (hWin_Config_Control)
            break;//dont alter the cursor visibility when joy config window open.
        WORD ht = LOWORD(lParam);
        if (HTCLIENT == ht) {

            SetCursor(*p_p2_cursor);

            if (IsMouseInClient()) {
                if (!is_cursor_hidden) {
                    is_cursor_hidden = true;
                    ShowCursor(false);
                }
            }
            else {
                if (is_cursor_hidden) {
                    is_cursor_hidden = false;
                    ShowCursor(true);
                }
            }
        }
        else {
            if (is_cursor_hidden) {
                is_cursor_hidden = false;
                ShowCursor(true);
            }
        }
        break;
    }
    case WM_WINDOWPOSCHANGING: {
        WINDOWPOS* winpos = (WINDOWPOS*)lParam;
        //Debug_Info("WM_WINDOWPOSCHANGING size adjusting");
        RECT rcWindow = { winpos->x, winpos->y, winpos->x + winpos->cx, winpos->y + winpos->cy };
        Check_Window_GUI_Scaling_Limits(hwnd, &rcWindow, true);
        winpos->x = rcWindow.left;
        winpos->y = rcWindow.top;
        winpos->cx = rcWindow.right - rcWindow.left;
        winpos->cy = rcWindow.bottom - rcWindow.top;
        return 0;
    }
    case WM_WINDOWPOSCHANGED: {
        //Debug_Info("WM_WINDOWPOSCHANGED");
        if (IsIconic(hwnd))
            break;
        WINDOWPOS* winpos = (WINDOWPOS*)lParam;

        if (!is_in_sizemove) {
            if (!(winpos->flags & (SWP_NOSIZE))) {
                //Debug_Info("WM_WINDOWPOSCHANGED is_in_sizemove");
                Window_Resized();
                if (pMovie_vlc) {
                    pMovie_vlc->Pause(false);
                    pMovie_vlc->SetScale();
                }

            }
            SetWindowTitle(hwnd, L"");
        }
        //SetWindowTitle(hwnd, L"");
        return 0;
    }
    case WM_ENTERSIZEMOVE:
        //Debug_Info("WM_ENTERSIZEMOVE");
        is_in_sizemove = true;
        if (pMovie_vlc)
            pMovie_vlc->Pause(true);

        return 0;

    case WM_EXITSIZEMOVE:
        //Debug_Info("WM_EXITSIZEMOVE");
        is_in_sizemove = false;
        Window_Resized();
        if (pMovie_vlc) {
            pMovie_vlc->Pause(false);
            pMovie_vlc->SetScale();
        }
 
        SetWindowTitle(hwnd, L"");
        return 0;

    case WM_CLOSE:
        Debug_Info("WM_CLOSE");
       *p_p2_game_exit_flag = 1;
        //window_close_state = TRUE;
        //break;
        return 0;
    case WM_DESTROY:
        // Clean up window-specific data objects. 
        Display_Exit();
        PostQuitMessage(0);
        break;
    case WM_ACTIVATE:
        break;
    case WM_ACTIVATEAPP:
        SetWindowActivation(wParam);
        if (wParam == FALSE) {
            Debug_Info("WM_ACTIVATEAPP false");
            *p_p2_is_app_active = FALSE;

            if (*p_p2_hThread_main != nullptr)
                SuspendThread(*p_p2_hThread_main);

            if (*pp_p2_wail32_sample_handle)
                p2_wail32_sample_suspend();
            if (*pp_p2_wail32_midi_sequence_handle)
                p2_wail32_midi_sequence_suspend();

            if (*p_p2_movie_flag_unk != 2)
                break;

            if (!*p_p2_is_app_movie_suspended)
                break;
            p2_movie_active(*pp_p2_movie_active_flag);
            *p_p2_is_app_movie_suspended = TRUE;

            if (is_cursor_clipped) {
                ClipCursor(nullptr);
                is_cursor_clipped = false;
                //Debug_Info("WM_ACTIVATEAPP false, Mouse Cursor Un-Clipped");
            }

            if (pMovie_vlc)
                pMovie_vlc->Pause(true);
        }
        else {
            Debug_Info("WM_ACTIVATEAPP true");
            *p_p2_is_app_active = TRUE;

            if (*pp_p2_wail32_sample_handle)
                p2_wail32_sample_resume();
            if (*pp_p2_wail32_midi_sequence_handle)
                p2_wail32_midi_sequence_resume();

            if (*p_p2_hThread_main != nullptr)
                ResumeThread(*p_p2_hThread_main);

            if (*p_p2_is_app_movie_suspended) {
                if (*pp_p2_movie_active_flag) {
                    p2_movie_active(*pp_p2_movie_active_flag);

                    *p_p2_is_app_movie_suspended = FALSE;
                }
            }
            if (*p_p2_cursor)
                SetCursor(*p_p2_cursor);
            if (is_cursor_clipped) {
                if (ClipMouseCursor()) {
                    //Debug_Info("WM_ACTIVATEAPP Mouse Cursor Clipped");
                }
            }
            if (pMovie_vlc)
                pMovie_vlc->Pause(false);
            
            //clear keyboard on re-activation.
            memset(p2_keyboard_state_main, 0, 256);
        }
        break;
    default:
        break;
    }

    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}


//______________________________________
static void Wait_Vblank_Cycles(LONG num) {

        Multimedia_Wait(f_cycle_Hz_GUI/num);
}


//____________________________________________________
static void __declspec(naked) wait_vblank_cycles(void) {

    __asm {
        push ebx
        push ecx
        push edx
        push edi
        push esi
        push ebp

        push eax
        call Wait_Vblank_Cycles
        add esp, 0x4

        pop ebp
        pop esi
        pop edi
        pop edx
        pop ecx
        pop ebx

        ret
    }
}


//___________________________________________
static void Clear_Surface(void** ppddsurface) {

    //DDBLT_PRESENTATION | DDBLT_COLORFILL
    Debug_Info("Clear_Surface: %X", ppddsurface);
    surface_gui->Clear_Texture(0x00000000);
}


//_______________________________________________
static void __declspec(naked) clear_surface(void) {

    __asm {
        push ebx
        push ecx
        push edx
        push edi
        push esi
        push ebp

        push eax
        call Clear_Surface
        add esp, 0x4

        pop ebp
        pop esi
        pop edi
        pop edx
        pop ecx
        pop ebx

        ret
    }
}


//______________________________________________
static void FastBlit_Surface(void** ppddsurface) {

    Debug_Info("FastBlit_Surface");
}


//__________________________________________________
static void __declspec(naked) fastblit_surface(void) {

    __asm {
        push ebx
        push ecx
        push edx
        push edi
        push esi
        push ebp

        push eax
        call FastBlit_Surface
        add esp, 0x4

        pop ebp
        pop esi
        pop edi
        pop edx
        pop ecx
        pop ebx

        ret
    }
}


//______________________________
static void Clear_Main_Surface() {

    Debug_Info("Clear_Main_Surface");
    surface_gui->Clear_Texture(0x00000000);
}


//____________________________________________________
static void __declspec(naked) clear_main_surface(void) {

    __asm {
        push ebx
        push ecx
        push edx
        push edi
        push esi
        push ebp

        call Clear_Main_Surface

        pop ebp
        pop esi
        pop edi
        pop edx
        pop ecx
        pop ebx

        ret
    }
}


//_______________________
static void Wait_Vblank() {

    Debug_Info("Wait_Vblank");
}


//_____________________________________________
static void __declspec(naked) wait_vblank(void) {

    __asm {
        push ebx
        push ecx
        push edx
        push edi
        push esi
        push ebp

        call Wait_Vblank

        pop ebp
        pop esi
        pop edi
        pop edx
        pop ecx
        pop ebx

        ret
    }
}


//____________________________________________________________________________________________________________________________________________________________________________________
static void Draw_Image_Buffer(IMAGE_BUFFER* from_img_buff, LONG sub_left, LONG sub_top, LONG sub_right, LONG sub_bottom, DrawSurface8_RT* p_to_surface8_rt, LONG to_left, LONG to_top) {

    if (!p_to_surface8_rt)
        return;
    LONG to_right = (LONG)p_to_surface8_rt->GetWidth() - 1;
    LONG to_bottom = (LONG)p_to_surface8_rt->GetHeight() - 1;

    if (to_left < 0) {
        sub_left -= to_left;
        to_left = 0;
    }
    if (to_top < 0) {
        sub_top -= to_top;
        to_top = 0;
    }
    if (sub_left < 0)
        sub_left = 0;
    if (sub_top < 0)
        sub_top = 0;

    if (sub_right > from_img_buff->right)
        sub_right = from_img_buff->right;
    if (sub_bottom > from_img_buff->bottom)
        sub_bottom = from_img_buff->bottom;
    if (sub_right > to_right)
        sub_right = to_right;
    if (sub_bottom > to_bottom)
        sub_bottom = to_bottom;

    if (sub_left > sub_right)
        return;
    if (sub_top > sub_bottom)
        return;

    DWORD sub_width = sub_right - sub_left + 1;
    DWORD sub_height = sub_bottom - sub_top + 1;

    DWORD from_width = from_img_buff->right - from_img_buff->left + 1;

    BYTE* from_buff = from_img_buff->buff;

    //Debug_Info("Draw_Image_Buffer %X, sub_left:%d, sub_top:%d, width:%d, height:%d", from_buff, sub_left, sub_top, sub_width, sub_height);
    BYTE* pSurface = nullptr;
    LONG pitch = 0;
    if (p_to_surface8_rt->Lock((VOID**)&pSurface, &pitch) != S_OK)
        return;

    from_buff += sub_top * from_width + sub_left;
    pSurface += to_top * pitch + to_left;
    for (DWORD y = 0; y < sub_height; y++) {
        for (DWORD x = 0; x < sub_width; x++)
            pSurface[x] = from_buff[x];
        pSurface += pitch;
        from_buff += from_width;
    }

    p_to_surface8_rt->Unlock();
}

//__________________________________________________________________________________________________________________________
static void Draw_Image_Buffer_GUI(IMAGE_BUFFER* from_img_buff, LONG sub_left, LONG sub_top, LONG sub_right, LONG sub_bottom) {

    Draw_Image_Buffer(from_img_buff, sub_left, sub_top, sub_right, sub_bottom, surface_gui, sub_left, sub_top);
    Display_Dx_Present(PRESENT_TYPE::gui);
}


//____________________________________________________________________________________________________________________________
static void Draw_Image_Buffer_Rect_GUI(IMAGE_BUFFER_RECT* p_img, LONG sub_left, LONG sub_top, LONG sub_right, LONG sub_bottom) {

    Draw_Image_Buffer(p_img->img_buff, sub_left, sub_top, sub_right, sub_bottom, surface_gui, sub_left, sub_top);
}


//_____________________________________________________________________________________________________________________________
static void Draw_Image_Buffer_Rect_Movie_Text_Top(IMAGE_BUFFER_RECT* from_struct, LONG left, LONG top, LONG right, LONG bottom) {
    
    surface_gui->Clear_Texture(0x00000000);

    LONG movie_height = 0;
    if (pMovie_vlc) {
        DrawSurface* surface = pMovie_vlc->Get_Surface();
        movie_height = (LONG)surface->GetScaledHeight();
    }
    else if (surface_movieTGV) 
        movie_height = (LONG)surface_movieTGV->GetScaledHeight();

    LONG text_y = 0;
    LONG text_height = bottom - top;
    //LONG black_bar_height = (clientHeight - movie_height) / 2;
    LONG black_bar_height = (LONG)(((float)surface_gui->GetHeight() / clientHeight) * ((clientHeight - movie_height) / 2));

    //draw text in the black area above the movie if there is room.
    if (black_bar_height >= text_height) {
        text_y = (clientHeight - movie_height) / 4;
        text_y = (480 * text_y) / clientHeight;
        text_y -= text_height / 2;
    }
    else //otherwise draw text over the movie at the top rather than overlapping the black bar.
        text_y = black_bar_height;
    
    if (text_y < 0)
        text_y = 0;

    //sub 1 from bottom, to prevent drawing a line of junk at the bottom of buffer.
    Draw_Image_Buffer(from_struct->img_buff, left, top, right, bottom - 1, surface_gui, left, text_y);
}


//________________________________________________________________________________________________________________________________
static void Draw_Image_Buffer_Rect_Movie_Text_Bottom(IMAGE_BUFFER_RECT* from_struct, LONG left, LONG top, LONG right, LONG bottom) {
    
    LONG movie_height = 0;
    if (pMovie_vlc) {
        DrawSurface* surface = pMovie_vlc->Get_Surface();
        movie_height = (LONG)surface->GetScaledHeight();
    }
    else if (surface_movieTGV)
        movie_height = (LONG)surface_movieTGV->GetScaledHeight();


    LONG text_y = 0;
    LONG text_height = bottom - top;
    //LONG black_bar_height = (clientHeight - movie_height) / 2;
    LONG black_bar_height = (LONG)(((float)surface_gui->GetHeight() / clientHeight) * ((clientHeight - movie_height) / 2));

    //draw text in the black area under the movie if there is room.
    if (black_bar_height >= text_height) {
        text_y = (clientHeight - movie_height) / 4;
        text_y = (480 * text_y) / clientHeight;
        text_y = 480 - text_y - text_height / 2;
    }
    else //otherwise draw text over the movie at the bottom rather than overlapping the black bar.
        text_y = 480 - black_bar_height - text_height;

    if (text_y > 480 - text_height)
        text_y = 480 - text_height;

    //sub 1 from bottom, to prevent drawing a line of junk at the bottom of buffer.
    Draw_Image_Buffer(from_struct->img_buff, left, top, right, bottom - 1, surface_gui, left, text_y);

    Display_Dx_Present(PRESENT_TYPE::movie);
}


//_________________________________________
static void Wait_Vblank_Draw_Main_Surface() {

    Debug_Info("Wait_Vblank_Draw_Main_Surface");
    //called after a palette update, no need to redraw the main gui image buffer.
    if (surface_gui)
        surface_gui->Redraw_Staging();
    Display_Dx_Present(PRESENT_TYPE::gui);
}


//_______________________________________________________________
static void __declspec(naked) wait_vblank_draw_main_surface(void) {

    __asm {
        push ebx
        push ecx
        push edx
        push edi
        push esi
        push ebp

        call Wait_Vblank_Draw_Main_Surface

        pop ebp
        pop esi
        pop edi
        pop edx
        pop ecx
        pop ebx

        ret
    }
}


//no longer called, replaced by fade functions or Get_Palette_Entries3.
/*//_________________________________________________________
static void Get_Palette_Entry(DWORD offset, DWORD* p_entry) {

    Palette_Get_Entries(offset, 1, p_entry, 2);
}*/


//___________________________________________________________
static void Get_Palette_Entries(DWORD offset, BYTE* p_entry) {
    //Debug_Info("Get_Palette_Entries3 offset:%d, r:%d, g:%d, b:%d", offset, p_entry[0], p_entry[1], p_entry[2]);
    Palette_Get_Entries3(offset, 256, p_entry, 2);
}


//__________________________________________________________
static void Set_Palette_Entries(DWORD offset, BYTE* p_entry) {
    //Debug_Info("Set_Palette_Entries offset:%d, r:%d, g:%d, b:%d", offset, entry[0], entry[1], entry[2]);
    Palette_Set_Entries3( offset, 256, p_entry, 2);
}


//___________________________________________________________
static void Clear_Palette_Colour(DWORD offset, BYTE* p_entry) {
    //Debug_Info("Set_Palette_Entries offset:%d, r:%d, g:%d, b:%d", offset, entry[0], entry[1], entry[2]);
    Palette_Clear_Colour3(p_entry, 2);
}


//________________________________________________________
static void Set_Palette_Entry(DWORD offset, BYTE* p_entry) {
    //Debug_Info("Set_Palette_Entry offset:%d, r:%d, g:%d, b:%d", offset, entry[0], entry[1], entry[2]);
    Palette_Set_Entries3( offset, 1, p_entry, 2);
}


//_______________________________
void Fade(BOOL out, BOOL instant) {

    static BOOL last_fade = -1;
    if (out == last_fade)
        return;
    last_fade = out;

    if (out)
        Debug_Info("Fade OUT");
    else
        Debug_Info("Fade IN");

    float val = 0.0f;
    if (instant) {
        if (out)
            val = 0.0f;
        else
            val = 1.0f;
        Set_Brigtness_Level(val);
        //Display_Dx_Present();
        Debug_Info("Fade INSTANT");
        return;
    }

    LARGE_INTEGER thisTime = { 0LL };
    LARGE_INTEGER nextTime = { 0LL };
    LARGE_INTEGER update_offset{ 0LL };
    update_offset.QuadPart = Frequency.QuadPart / 100;
    QueryPerformanceCounter(&thisTime);
    nextTime.QuadPart = thisTime.QuadPart + update_offset.QuadPart;

    float level = 0.0f;

    while (level < 1.0f) {
        QueryPerformanceCounter(&thisTime);
        if (thisTime.QuadPart >= nextTime.QuadPart) {
            nextTime.QuadPart = thisTime.QuadPart + update_offset.QuadPart;

            level += 0.05f;
            if (out)
                val = 1.0f - level;
            else
                val = level;

            Set_Brigtness_Level(val);
            Display_Dx_Present();
        }
    }
}


//__________________________________________
static void __declspec(naked) fade_out(void) {

    __asm {
        push ebx
        push ecx
        push edx
        push esi
        push edi
        push ebp

        push FALSE
        push TRUE
        call Fade
        add esp, 0x8

        pop ebp
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx

        ret
    }
}


//________________________________
static void Fade_In_Pal(BYTE* pal) {
    Debug_Info("Fade_In_Pal pal: %X", pal);

    Palette_Set_Entries3(0, 256, pal, 2);
    if (surface_gui)
        surface_gui->Redraw_Staging();

    Fade(FALSE, FALSE);
}


//_____________________________________________
static void __declspec(naked) fade_in_pal(void) {

    __asm {
        push ebx
        push ecx
        push edx
        push esi
        push edi
        push ebp

  
        push eax
        call Fade_In_Pal
        add esp, 0x4

        pop ebp
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx

        ret
    }
}


//________________________________________________________
static void __declspec(naked) movie_reset_brightness(void) {

    __asm {
        push ebx
        push ecx
        push edx
        push esi
        push edi
        push ebp

        push TRUE
        push FALSE
        call Fade
        add esp, 0x8

        pop ebp
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx

        ret
    }
}


//___________________________________________________________________
static void Fade_In_Buffer_Pal(IMAGE_BUFFER* image_buffer, BYTE* pal) {
    Debug_Info("Fade_In_Buffer_Pal image_buffer: %X, pal: %X", image_buffer, pal);

    Palette_Set_Entries3(0, 256, pal, 2);
    if (surface_gui)
        surface_gui->Redraw_Staging();

    Fade(FALSE, FALSE);

}


//________________________________________________________
static void __declspec(naked) fade_in_buffer_get_pal(void) {

    __asm {
        push ebx
        push ecx
        push edx
        push esi
        push edi
        push ebp

        mov ebp, edx
        lea edx, [ebx * 8]
        add edx, 0x8
        mov edx, dword ptr ds : [ebp + edx]
        add edx, ebp

        push edx
        push eax
        call Fade_In_Buffer_Pal
        add esp, 0x8

        pop ebp
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx

        ret
    }
}


//____________________________________________________
static void __declspec(naked) fade_in_buffer_pal(void) {

    __asm {
        push ebx
        push ecx
        push edx
        push esi
        push edi
        push ebp

        push edx
        push eax
        call Fade_In_Buffer_Pal
        add esp, 0x8

        pop ebp
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx

        ret
    }
}


//_____________________________________________________
static void Fade_Out_Buffer(IMAGE_BUFFER* image_buffer) {

    Debug_Info("Fade_Out_Buffer image_buffer: %X", image_buffer);

    Fade(TRUE, FALSE);

    static BYTE black_pal[768]{ 0 };
    Palette_Clear_Colour3(black_pal, 2);
}


//_________________________________________________
static void __declspec(naked) fade_out_buffer(void) {

    __asm {
        push ebx
        push ecx
        push edx
        push esi
        push edi
        push ebp


        push eax
        call Fade_Out_Buffer
        add esp, 0x4

        pop ebp
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx

        ret
    }
}


/*
//main space draw display vars
BYTE* p_space_buff = nullptr;
LONG space_pitch = 0;
LONG space_right = 0;
LONG space_bottom = 0;

int space_count = 0;
//________________________________
static void Main_Space_Draw_Lock() {

    if (!surface_space2D || surface_space2D->Lock((VOID**)&p_space_buff, &space_pitch) != S_OK) {
        p_space_buff = nullptr;
        space_pitch = 0;
        space_right = 0;
        space_bottom = 0;
        return;
    }
    space_right = (LONG)surface_space2D->GetWidth() - 1;
    space_bottom = (LONG)surface_space2D->GetHeight() - 1;
    space_count = 0;
}


//_________________________________________________________
static void __declspec(naked) main_space_surface_lock(void) {

    __asm {
        pushad
        call Main_Space_Draw_Lock
        popad
        //re-insert original code
        mov esi, 0x8
        ret
    }
}


//______________________________________________________________________________________________________________________________________
static void Draw_Image_Buffer_Rect_Space(IMAGE_BUFFER_RECT* from_img_buff, LONG sub_left, LONG sub_top, LONG sub_right, LONG sub_bottom) {
    //Debug_Info_Movie("Draw_Image_Buffer_Rect_Space: %X, %d, %d, %d, %d, %d", from_img_buff->img_buff->buff, space_count, sub_left, sub_top, sub_right- sub_left, sub_bottom - sub_top);
    space_count++;
    if (!p_space_buff)
        return;

    if (sub_left < 0)
        sub_left = 0;
    if (sub_top < 0)
        sub_top = 0;
    if (sub_right > from_img_buff->right)
        sub_right = from_img_buff->right;
    if (sub_bottom > from_img_buff->bottom)
        sub_bottom = from_img_buff->bottom;
    if (sub_right > space_right)
        sub_right = space_right;
    if (sub_bottom > space_bottom)
        sub_bottom = space_bottom;


    DWORD sub_width = sub_right - sub_left + 1;
    DWORD sub_height = sub_bottom - sub_top + 1;

    IMAGE_BUFFER* img_buff = from_img_buff->img_buff;
    DWORD from_width = img_buff->right - img_buff->left + 1;
    //DWORD  from_height = img_buff->bottom - img_buff->top + 1;

    BYTE* from_buff = img_buff->buff;
    BYTE* to_buff = p_space_buff;

    from_buff += sub_top * from_width + sub_left;
    to_buff += sub_top * space_pitch + sub_left;
    for (DWORD y = 0; y < sub_height; y++) {
        for (DWORD x = 0; x < sub_width; x++)
            to_buff[x] = from_buff[x];

        to_buff += space_pitch;
        from_buff += from_width;
    }
}


//_____________________________________________
static void Main_Space_Surface_Unlock_Display() {

    surface_space2D->Unlock();

    while (wait_joy_config)
        Sleep(0);

    Multimedia_Wait(f_cycle_Hz_Space);
    Display_Dx_Present(PRESENT_TYPE::space);
}


//___________________________________________________________________
static void __declspec(naked) main_space_surface_unlock_display(void) {

    __asm {

        push edi
        push ebp

        call Main_Space_Surface_Unlock_Display

        pop ebp
        pop edi

        pop esi
        pop edx
        pop ecx
        pop ebx

        ret
    }
}
*/



//_____________________________________________
static void Draw_Image_Buffer_Rect_Space_Main() {

    if (!surface_space2D)
        return;

    BYTE* p_space_buff = nullptr;
    LONG space_pitch = 0;

    if (surface_space2D->Lock((VOID**)&p_space_buff, &space_pitch) != S_OK)
        return;

    LONG space_right = (LONG)surface_space2D->GetWidth() - 1;
    LONG space_bottom = (LONG)surface_space2D->GetHeight() - 1;

    LONG sub_left = 0;
    LONG sub_top = 0;
    DWORD sub_width = 640;
    DWORD sub_height = 480;

    IMAGE_BUFFER* img_buff = p_p2_main_image_buffer;
    DWORD from_width = img_buff->right - img_buff->left + 1;
    //DWORD  from_height = img_buff->bottom - img_buff->top + 1;

    BYTE* from_buff = img_buff->buff;
    BYTE* to_buff = p_space_buff;

    from_buff += sub_top * from_width + sub_left;
    to_buff += sub_top * space_pitch + sub_left;
    for (DWORD y = 0; y < sub_height; y++) {
        for (DWORD x = 0; x < sub_width; x++)
            to_buff[x] = from_buff[x];

        to_buff += space_pitch;
        from_buff += from_width;
    }

    surface_space2D->Unlock();

    if (current_pro_type != PROFILE_TYPE::GUI || clear_after_space_draw)
        memset(img_buff->buff, 0, 640 * 480);

    while (wait_joy_config)
        Sleep(0);
    Multimedia_Wait(f_cycle_Hz_Space);
    Display_Dx_Present(PRESENT_TYPE::space);
}



//____________________________________________________________________
static void __declspec(naked) clear_screen_options_joy_calibrate(void) {

    __asm {
        mov clear_after_space_draw, TRUE
        call Draw_Image_Buffer_Rect_Space_Main
        mov clear_after_space_draw, FALSE
        ret
    }
}

//_________________________________________________________________
static void __declspec(naked) cursor_clip_conversation_choice(void) {

    __asm {
        mov clip_cursor, TRUE
        //call wait_vblank_draw_main_surface // unnecessary, causes a flicker between movie and choice text screen.
        ret
    }
}


//___________________________________________________________________
static void __declspec(naked) cursor_unclip_conversation_choice(void) {

    __asm {
        mov clip_cursor, FALSE
        call clear_surface
        ret
    }
}


//_______________________________________________________
static void Change_Profile_Type(PROFILE_TYPE new_profile) {

    static PROFILE_TYPE last_profile_type = current_pro_type;

    current_pro_type = new_profile;

    //clear keyboard on profile change incase button is down during transition.
    if (last_profile_type != current_pro_type) {
        //if (last_profile_type == PROFILE_TYPE::GUI || current_pro_type == PROFILE_TYPE::GUI) {
            memset(p2_keyboard_state_main, 0, 256);
            last_profile_type = current_pro_type;
        //}
    }


}


//_______________________________________________________
static void __declspec(naked) cursor_clipper_space(void) {

    __asm {

        pushad
        call Reset_Key_Throttle //reset key controlled throttle value to max when entering space.
        push PROFILE_SPACE
        call Change_Profile_Type
        add esp, 0x4
        popad

        //mov current_pro_type, PROFILE_SPACE
        mov clip_cursor, TRUE
        call p_p2_space_main
        mov clip_cursor, FALSE
        //mov current_pro_type, PROFILE_GUI

        pushad
        push PROFILE_GUI
        call Change_Profile_Type
        add esp, 0x4
        popad


        ret
    }
}


//_______________________________________________________________
static void __declspec(naked) cursor_clipper_options_screen(void) {

    __asm {
        pushad
        push PROFILE_GUI
        call Change_Profile_Type
        add esp, 0x4
        popad
        //mov current_pro_type, PROFILE_GUI
        mov clip_cursor, FALSE
        call p_p2_options_screen
        mov clip_cursor, TRUE
        //mov current_pro_type, PROFILE_SPACE
        pushad
        push PROFILE_SPACE
        call Change_Profile_Type
        add esp, 0x4
        popad
        ret
    }
}


//__________________________________________________________________
static void __declspec(naked) cursor_clipper_navigation_screen(void) {

    __asm {
        pushad
        push PROFILE_GUI
        call Change_Profile_Type
        add esp, 0x4
        popad
        //mov current_pro_type, PROFILE_GUI
        mov clear_after_space_draw, TRUE
        mov clip_cursor, FALSE
        call p_p2_navigation_screen
        mov clip_cursor, TRUE
        mov clear_after_space_draw, FALSE
        //mov current_pro_type, PROFILE_SPACE
        pushad
        push PROFILE_SPACE
        call Change_Profile_Type
        add esp, 0x4
        popad
        ret
    }
}


//_______________________________________________________________
static void __declspec(naked) cursor_clipper_hotkeys_screen(void) {

    __asm {
        pushad
        push PROFILE_GUI
        call Change_Profile_Type
        add esp, 0x4
        popad
        //mov current_pro_type, PROFILE_GUI
        mov clip_cursor, FALSE
        call p_p2_hotkeys_screen
        mov clip_cursor, TRUE
        //mov current_pro_type, PROFILE_SPACE
        pushad
        push PROFILE_SPACE
        call Change_Profile_Type
        add esp, 0x4
        popad
        ret
    }
}


//_____________________________________________________________
static void __declspec(naked) cursor_clipper_diary_screen(void) {

    __asm {
        pushad
        push PROFILE_GUI
        call Change_Profile_Type
        add esp, 0x4
        popad
        //mov current_pro_type, PROFILE_GUI
        mov clip_cursor, FALSE
        call p_p2_diary_screen
        mov clip_cursor, TRUE
        //mov current_pro_type, PROFILE_SPACE
        pushad
        push PROFILE_SPACE
        call Change_Profile_Type
        add esp, 0x4
        popad
        ret
    }
}


//_____________________________________________________________
static void __declspec(naked) cursor_clipper_email_screen(void) {

    __asm {
        pushad
        push PROFILE_GUI
        call Change_Profile_Type
        add esp, 0x4
        popad
        //mov current_pro_type, PROFILE_GUI
        mov clip_cursor, FALSE
        call p_p2_email_screen
        mov clip_cursor, TRUE
        //mov current_pro_type, PROFILE_SPACE
        pushad
        push PROFILE_SPACE
        call Change_Profile_Type
        add esp, 0x4
        popad
        ret
    }
}


//__________________________________________________________
static void __declspec(naked) alt_x_window_space_start(void) {

    __asm {
        mov eax, p_p2_space_struct_exit_flag
        cmp word ptr ds : [eax] , 0
        je exit_func
        mov clip_cursor, FALSE
        //mov current_pro_type, PROFILE_GUI
        pushad
        push PROFILE_GUI
        call Change_Profile_Type
        add esp, 0x4
        popad
        exit_func :
        ret
    }
}


//_________________________________________________________
static void __declspec(naked) alt_x_window_space_end(void) {

    __asm {
        mov eax, p_p2_space_struct_exit_flag
        mov word ptr ds : [eax] , 0
        //mov current_pro_type, PROFILE_SPACE
        mov clip_cursor, TRUE
        pushad
        push PROFILE_SPACE
        call Change_Profile_Type
        add esp, 0x4
        popad
        ret
    }
}


//_____________________________________________________________
static void __declspec(naked) alt_x_window_sample_suspend(void) {

    __asm {
        //pause sample_handler while alt-x window is opened.
        mov eax, pp_p2_wail32_sample_handle
        cmp dword ptr ds:[eax], 0
        je exit_func
        call p2_wail32_sample_suspend

        exit_func:
        //restore original code
        mov dword ptr ds:[EBP - 0x18], 0
        ret
    }
}


//____________________________________________________________
static void __declspec(naked) alt_x_window_sample_resume(void) {

    __asm {
        //resume sample_handler after alt-x window is closed.
        mov eax, pp_p2_wail32_sample_handle
        cmp dword ptr ds : [eax] , 0
        je exit_func
        call p2_wail32_sample_resume

        exit_func:
        //restore original code
        movsx eax, dword ptr ds : [EBP - 0x18]
        cmp eax, 2
        ret
    }
}


//___________________________
void Modifications_Display() {

    //disable close[X] button when registering window class.
    MemWrite32(0x46ABD2, CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_NOCLOSE);
    MemWrite32(0x46AD5A, CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_NOCLOSE);

    //replace WinProc function
    MemWrite8(0x46A8E8, 0x53, 0xE9);
    FuncWrite32(0x46A8E9, 0x81555756, (DWORD)&WinProc_Main);

    MemWrite8(0x46A554, 0x53, 0xE9);
    FuncWrite32(0x46A555, 0x57565251, (DWORD)&setup_window);
 
    //in main setup
    //jump over clear dd surfaces //JMP SHORT 0046AEE1
    MemWrite16(0x46AE60, 0xE089, 0x7FEB);

    //on main loop exit
    MemWrite8(0x46B0E6, 0xBB, 0xE8);
    FuncWrite32(0x46B0E7, 0x64, (DWORD)&Display_Exit);
    //jump over clear dd surfaces and release. JMP 0046B194
    MemWrite8(0x46B0EB, 0x8D, 0xE9);
    MemWrite32(0x46B0EC, 0x00C82484, 0xA4);
    MemWrite16(0x46B0F0, 0x0000, 0x9090);

    MemWrite8(0x46A7E8, 0x53, 0xE9);
    FuncWrite32(0x46A7E9, 0x158B5251, (DWORD)&Display_Exit);

    //main gui draw speed control.
    MemWrite16(0x418B24, 0x5153, 0xE990);
    FuncWrite32(0x418B26, 0xC6895652, (DWORD)&wait_vblank_cycles);

    //clear gui draw surface.
    MemWrite16(0x46A828, 0x5153, 0xE990);
    FuncWrite32(0x46A82A, 0x64EC8352, (DWORD)&clear_surface);

    //don't think this is used.
    MemWrite8(0x46A864, 0x53, 0xE9);
    FuncWrite32(0x46A865, 0x83575651, (DWORD)&fastblit_surface);
    MemWrite16(0x46A869, 0x10EC, 0x9090);

    //clear gui draw surface.
    MemWrite16(0x46A8A4, 0x5153, 0xE990);
    FuncWrite32(0x46A8A6, 0x64EC8352, (DWORD)&clear_main_surface);

    //don't think this is used.
    MemWrite8(0x46B1E8, 0x6A, 0xE9);
    FuncWrite32(0x46B1E9, 0xCEACA100, (DWORD)&wait_vblank);
    MemWrite16(0x46B1ED, 0x0055, 0x9090);
    

    MemWrite8(0x46B1F8, 0x53, 0xE9);
    FuncWrite32(0x46B1F9, 0x83555756, (DWORD)&Draw_Image_Buffer_GUI);
    MemWrite16(0x46B1FD, 0x6CEC, 0x9090);

    MemWrite8(0x46B2DC, 0x53, 0xE9);
    FuncWrite32(0x46B2DD, 0x83555756, (DWORD)&Draw_Image_Buffer_Rect_GUI);
    MemWrite16(0x46B2E1, 0x78EC, 0x9090);

    FuncReplace32(0x41B817, 0x04FAC1, (DWORD)&Draw_Image_Buffer_Rect_Movie_Text_Top);
    FuncReplace32(0x41B838, 0x04FAA0, (DWORD)&Draw_Image_Buffer_Rect_Movie_Text_Bottom);
 
    /*
    // draw space rects 
    //lock space surface once for all rects
    MemWrite8(0x459F5F, 0xBE, 0xE8);
    FuncWrite32(0x459F60, 0x08, (DWORD)&main_space_surface_lock);
    //draw all rects
    FuncReplace32(0x459FF5, 0x0112E3, (DWORD)&Draw_Image_Buffer_Rect_Space);
    //unlock space surface once for all rects
    MemWrite8(0x45A01C, 0x5E, 0xE9);
    FuncWrite32(0x45A01D, 0xC35B595A, (DWORD)&main_space_surface_unlock_display);
    */
    //Replace main space draw function
    MemWrite8(0x459F20, 0x53, 0xE9);
    FuncWrite32(0x459F21, 0x57565251, (DWORD)&Draw_Image_Buffer_Rect_Space_Main);

    //called after a palette update
    MemWrite8(0x46B528, 0x6A, 0xE9);
    FuncWrite32(0x46B529, 0xCEACA100, (DWORD)&wait_vblank_draw_main_surface);
    MemWrite16(0x46B52D, 0x0055, 0x9090);

    //no longer called, replaced by fade functions or Get_Palette_Entries3.
    //MemWrite8(0x46B420, 0x53, 0xE9);
    //FuncWrite32(0x46B421, 0x0C245C8B, (DWORD)&Get_Palette_Entry);

    //replace one at a time single entry retrieval with get whole palette at once for greater efficiency. 
    FuncReplace32(0x43324B, 0x0381D1, (DWORD)&Get_Palette_Entries);
    //jump rest of loop //JMP SHORT 00433228
    MemWrite16(0x433252, 0xD4EB, 0x9090);

    FuncReplace32(0x43342F, 0x037FED, (DWORD)&Get_Palette_Entries);
    //jump rest of loop //JMP SHORT 0043340C
    MemWrite16(0x433436, 0xD4EB, 0x9090);


    MemWrite16(0x46B45C, 0x5653, 0xE990);
    FuncWrite32(0x46B45E, 0x04EC8357, (DWORD)&Set_Palette_Entry);

    //replace one at a time single entry insertion with set whole palette at once for greater efficiency. 
    FuncReplace32(0x4106FC, 0x05AD5C, (DWORD)&Set_Palette_Entries);
    //jump rest of loop //JLE SHORT 004106F9
    MemWrite16(0x41070D, 0xEA7E, 0x9090);

    FuncReplace32(0x4182DD, 0x05317B, (DWORD)&Set_Palette_Entries);
    //jump rest of loop //JLE SHORT 004182DA
    MemWrite16(0x4182EE, 0xEA7E, 0x9090);
 
    FuncReplace32(0x418351, 0x053107, (DWORD)&Set_Palette_Entries);
    //jump rest of loop //JL SHORT 0041834E
    MemWrite16(0x418362, 0xEA7C, 0x9090);
    

    //replace one at a time single entry insertion with clear whole palette colour at once for greater efficiency. 
    FuncReplace32(0x41D13B, 0x04E31D, (DWORD)&Clear_Palette_Colour);//0xFF
    //jump rest of loop //JMP SHORT 0041D11E
    MemWrite16(0x41D142, 0xDAEB, 0x9090);

    FuncReplace32(0x41D3D0, 0x04E088, (DWORD)&Clear_Palette_Colour);//0xFF
    //jump rest of loop //JMP SHORT 0041D3B3
    MemWrite16(0x41D3D7, 0xDAEB, 0x9090);

    FuncReplace32(0x41F38B, 0x04C0CD, (DWORD)&Clear_Palette_Colour);//0x100
    //jump rest of loop //JMP SHORT 0041F371
    MemWrite16(0x41F392, 0xDDEB, 0x9090);

    FuncReplace32(0x421FF9, 0x04945F, (DWORD)&Clear_Palette_Colour);//0xFF
    //jump rest of loop //JMP SHORT 00421FDC
    MemWrite16(0x422000, 0xDAEB, 0x9090);


    //Fade in and out functions.
    MemWrite8(0x469584, 0x53, 0xE9);
    FuncWrite32(0x469585, 0x55565251, (DWORD)&fade_out);

    MemWrite8(0x46966C, 0x53, 0xE9);
    FuncWrite32(0x46966D, 0x57565251, (DWORD)&fade_in_pal);

    MemWrite8(0x418374, 0x53, 0xE9);
    FuncWrite32(0x418375, 0x57565251, (DWORD)&fade_out);

    MemWrite8(0x418220, 0x51, 0xE9);
    FuncWrite32(0x418221, 0x83555756, (DWORD)&fade_in_buffer_get_pal);
    MemWrite16(0x418225, 0x04EC, 0x9090);

    MemWrite8(0x418278, 0x53, 0xE9);
    FuncWrite32(0x418279, 0x55575651, (DWORD)&fade_in_buffer_pal);

    MemWrite8(0x418300, 0x53, 0xE9);
    FuncWrite32(0x418301, 0x81565251, (DWORD)&fade_out_buffer);
    MemWrite8(0x418305, 0xEC, 0x90);
    MemWrite32(0x418306, 0x0300, 0x90909090);

    //reset fade brightness on movie start
    FuncReplace32(0x4691F9, 0x162B, (DWORD)&movie_reset_brightness);

    //--switch mouse clipping on/off depending on current screen in windowed mode--
    FuncReplace32(0x41B639, 0x04FEEB, (DWORD)&cursor_clip_conversation_choice);
    FuncReplace32(0x41BA7A, 0x04EDAA, (DWORD)&cursor_unclip_conversation_choice);

    FuncReplace32(0x41C0E3, 0x01C050, (DWORD)&cursor_clipper_space);
    FuncReplace32(0x4227BC, 0x015977, (DWORD)&cursor_clipper_space);

    FuncReplace32(0x4397FF, 0x023659, (DWORD)&cursor_clipper_options_screen);
    FuncReplace32(0x440FEE, 0x01BE6A, (DWORD)&cursor_clipper_options_screen);
    FuncReplace32(0x442797, 0x01A6C1, (DWORD)&cursor_clipper_options_screen);


    FuncReplace32(0x44211F, 0x01867D, (DWORD)&cursor_clipper_navigation_screen);
    //disable set mouse pos to allow mouse to move freely in windowed mode.
    MemWrite8(0x45B446, 0xE8, 0x90);
    MemWrite32(0x45B447, 0xFFFBD579, 0x90909090);

    //disable set mouse pos for P.A.D. to allow mouse to move freely in windowed mode.
    MemWrite8(0x436B34, 0xE8, 0x90);
    MemWrite32(0x436B35, 0xFFFE1E8B, 0x90909090);


    //0043C14E | .E8 E1FC0100 | CALL EMAIL() ? ?
    FuncReplace32(0x43C14F, 0x01FCE1, (DWORD)&cursor_clipper_email_screen);
    //00442593 | .E8 BC9E0100   CALL DIARY() ? ?
    FuncReplace32(0x442594, 0x019EBC, (DWORD)&cursor_clipper_diary_screen);
    //00442726 | .E8 49CC0100   CALL ALT_H_MENU() ? ?
    FuncReplace32(0x442727, 0x01CC49, (DWORD)&cursor_clipper_hotkeys_screen);

    //clear screen buffer after drawing on old joystick calibration screen.  
    FuncReplace32(0x45E1B8, 0xFFFFBD64, (DWORD)&clear_screen_options_joy_calibrate);
    //clear screen buffer after drawing on old joystick throttle calibration screen.
    FuncReplace32(0x45E9D0, 0xFFFFB54C, (DWORD)&clear_screen_options_joy_calibrate);

    //put space alt x window in GUI mode while it is up.
    MemWrite32(0x4398FF, 0xAEB88366, 0xE8909090);
    FuncWrite32(0x439903, 0x00000279, (DWORD)&alt_x_window_space_start);

    MemWrite32(0x439921, 0xAE80C766, 0xE8909090);
    FuncWrite32(0x439925, 0x00000279, (DWORD)&alt_x_window_space_end);
    MemWrite8(0x439929, 0x00, 0x90);

    MemWrite32(0x439944, 0xAE80C766, 0xE8909090);
    FuncWrite32(0x439948, 0x00000279, (DWORD)&alt_x_window_space_end);
    MemWrite8(0x43994C, 0x00, 0x90);


    //-----------------------------------------------------------------------------

        //in void ERROR_EXIT_MESSAGE_BOX(const char* msg) //should be all right
    //0046B5F2 | .FF53 08       CALL DWORD PTR DS : [EBX + 8] ; IDirectDrawSurface4_Release(p)
    //0046B616 | .FF53 08       CALL DWORD PTR DS : [EBX + 8] ; IDirectDrawSurface4_Release(p)

    //---pause sample handler while ALT-X window is opened to prevent harsh static sounds---
    MemWrite16(0x41D89D, 0x45C7, 0x9090);
    //MemWrite8(0x41D89F, 0xE8, 0xE8);
    FuncWrite32(0x41D8A0, 0x00, (DWORD)&alt_x_window_sample_suspend);

    MemWrite8(0x41D934, 0x0F, 0xE8);
    FuncWrite32(0x41D935, 0x83E845BF, (DWORD)&alt_x_window_sample_resume);
    MemWrite16(0x41D939, 0x02F8, 0x9090);
    //--------------------------------------------------------------------------------------
}
