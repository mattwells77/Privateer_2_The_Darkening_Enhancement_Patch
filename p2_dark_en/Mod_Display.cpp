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

#include "pch.h"

#include "Display_DX11.h"
#include "modifications.h"
#include "memwrite.h"
#include "configTools.h"
#include "libvlc_Movies.h"
#include "dark.h"
//#include "joystick_config.h"

#define WIN_MODE_STYLE  WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX

bool is_windowed = false;

BOOL clip_cursor = FALSE;
bool is_cursor_clipped = false;

UINT clientWidth = GUI_WIDTH;
UINT clientHeight = GUI_HEIGHT;


LARGE_INTEGER Frequency = { 0LL };

float f_cycle_Hz_GUI = 30.0f;
float f_cycle_Hz_Space = 30.0f;


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

        //disable close button for now.
        HMENU SysMenu = GetSystemMenu(hwnd, FALSE);
        RemoveMenu(SysMenu, SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
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


//______________________________________________________________________________________
static LRESULT CALLBACK WinProc_Main(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    static bool is_in_sizemove = false;
    static bool is_cursor_hidden = true;

    switch (uMsg)
    {
    case WM_ERASEBKGND:
        return 1;
    case WM_KEYDOWN:
        if (!(lParam & 0x40000000)) { //The previous key state. The value is 1 if the key is down before the message is sent, or it is zero if the key is up.
            if (wParam == VK_F11) { //Use F11 key to toggle windowed mode.
                if (pMovie_vlc)
                    pMovie_vlc->Pause(true);

                Toggle_WindowMode(hwnd);
                return 0;
            }

        }
        break;
    case WM_KILLFOCUS:
        Debug_Info("WM_KILLFOCUS");
        return 0;
    case WM_INITMENU:
        Debug_Info("WM_INITMENU");
        return 0;
    case WM_MENUCHAR:

        Debug_Info("WM_MENUCHAR %X", MNC_CLOSE<<16);
        return MNC_CLOSE << 16;
        
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
        return 1;
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
        //if (hWin_JoyConfig)
        //    break;//dont alter the cursor visibility when joy config window open.
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
        //Set_WindowActive_State(wParam);
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
    LONG black_bar_height = (clientHeight - movie_height) / 2;

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
    LONG black_bar_height = (clientHeight - movie_height) / 2;

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
        Display_Dx_Present();
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


//__________________________________________________
static void Get_Mouse_Position(LONG* p_x, LONG* p_y) {

    LONG x = 0;
    LONG y = 0;

    POINT p{ 0,0 };
    if (ClientToScreen(*p_p2_hWinMain, &p)) {
        POINT m{ 0,0 };
        GetCursorPos(&m);

        x = (m.x - p.x);
        y = (m.y - p.y);

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


//_______________________________________________
static void __declspec(naked) get_mouse_pos(void) {

    __asm {
        push ebx
        push ecx
        push esi
        push edi

        push edx
        push eax
        call Get_Mouse_Position
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

    *p_butt_left = GetAsyncKeyState(VK_LBUTTON);
    *p_butt_right = GetAsyncKeyState(VK_RBUTTON);
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


    *p_butt_left = GetAsyncKeyState(VK_LBUTTON);
    *p_butt_right = GetAsyncKeyState(VK_RBUTTON);

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
        ix += client.x;

        fy += y * fheight / GUI_HEIGHT;
        LONG iy = (LONG)fy;
        if ((float)iy != fy)
            iy++;
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


//main space draw display vars
BYTE* p_space_buff = nullptr;
LONG space_pitch = 0;
LONG space_right = 0;
LONG space_bottom = 0;


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
static void __declspec(naked) cursor_clipper_space(void) {

    __asm {
        mov clip_cursor, TRUE
        call p_p2_space_main
        mov clip_cursor, FALSE
        ret
    }
}


//_______________________________________________________________
static void __declspec(naked) cursor_clipper_options_screen(void) {

    __asm {
        mov clip_cursor, FALSE
        call p_p2_options_screen
        mov clip_cursor, TRUE
        ret
    }
}


//__________________________________________________________
static void __declspec(naked) cursor_unclip_space_exit(void) {

    __asm {
        mov clip_cursor, FALSE
        cmp word ptr ds:[eax + 0x279AE], 0x0
        ret
    }
}


//________________________________________________________
static void __declspec(naked) cursor_clip_space_exit(void) {

    __asm {
        mov clip_cursor, TRUE
        cmp dword ptr ds:[eax + 0x1154], 0x2
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
 
    // draw space rects 
    //lock space surface once for all rects
    MemWrite8(0x459F5F, 0xBE, 0xE8);
    FuncWrite32(0x459F60, 0x08, (DWORD)&main_space_surface_lock);
    //draw all rects
    FuncReplace32(0x459FF5, 0x0112E3, (DWORD)&Draw_Image_Buffer_Rect_Space);
    //unlock space surface once for all rects
    MemWrite8(0x45A01C, 0x5E, 0xE9);
    FuncWrite32(0x45A01D, 0xC35B595A, (DWORD)&main_space_surface_unlock_display);


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

    MemWrite16(0x4398FF, 0x8366, 0xE890);
    FuncWrite32(0x439901, 0x0279AEB8, (DWORD)&cursor_unclip_space_exit);
    MemWrite16(0x439905, 0x0000, 0x9090);

    MemWrite16(0x43995C, 0xB883, 0xE890);
    FuncWrite32(0x43995E, 0x00001154, (DWORD)&cursor_clip_space_exit);
    MemWrite8(0x439962, 0x02, 0x90);
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
