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
#include "modifications.h"
#include "memwrite.h"
#include "configTools.h"
#include "dark.h"
#include "Display_DX11.h"
#include "libvlc_Movies.h"


///ddraw dummy structures, for the basic requirements here.-----------------------------------
typedef struct _DUMMY_DDSURFACEDESC {
    DWORD   dwSize;                 // size of the structure
    DWORD   dwFlags;                // unused here.
    DWORD   dwHeight;               // height of surface to be created
    DWORD   dwWidth;                // width of input surface
    LONG    lPitch;                 // distance to start of next line (return value only)
    DWORD   dwBackBufferCount;      // unused here.
    DWORD   dwMipMapCount;          // unused here.
    DWORD   dwAlphaBitDepth;        // unused here.
    DWORD   dwReserved;             // reserved
    LPVOID  lpSurface;              // pointer to the associated surface memory
    /*DWORD   colorKeySec1a;          // color key section filler
    DWORD   colorKeySec1b;          // color key section filler
    DWORD   colorKeySec2a;          // color key section filler
    DWORD   colorKeySec2b;          // color key section filler
    DWORD   colorKeySec3a;          // color key section filler
    DWORD   colorKeySec3b;          // color key section filler
    DWORD   colorKeySec4a;          // color key section filler
    DWORD   colorKeySec4b;          // color key section filler
    DWORD   dwSize;                 // size of the structure
    DWORD   dwFlags;                // unused here.
    DWORD   dwFourCC;               // unused here.
    DWORD   dwRGBBitCount;          // how many bits per pixel
    DWORD   dwRBitMask;             // unused here.
    DWORD   dwGBitMask;             // unused here.
    DWORD   dwBBitMask;             // unused here.
    DWORD   dwRGBAlphaBitMask;      // unused here.
    DWORD   dwCaps;                 // dwCaps section filler*/
} DDSURFACEDESC;
//------------------------------------------------------------------------------------------------
typedef _DUMMY_DDSURFACEDESC   FAR* LPDDSURFACEDESC;


DrawSurface8* movie_tgv_1 = nullptr;
DrawSurface8* movie_tgv_2 = nullptr;


//____________________________________________________________________________________________________________________
static HRESULT __stdcall Movie_Create_Surface_1(LPDDSURFACEDESC lp_desc, DrawSurface8** pp_surface8_RT, IUnknown FAR*) {

    Debug_Info_Movie("Movie Create Surface 1");
    if (!lp_desc)
        return S_FALSE;
    if (!pp_surface8_RT)
        return S_FALSE;

    static BOOL interlaced_movies = ConfigReadInt(L"MOVIES", L"SHOW_ORIGINAL_MOVIES_INTERLACED", CONFIG_MOVIES_SHOW_ORIGINAL_INTERLACED);
    SCALE_TYPE scale_type = SCALE_TYPE::fit;

    DWORD projected_width = lp_desc->dwWidth;
    DWORD projected_height = lp_desc->dwHeight;
    if (interlaced_movies) {
        projected_width += projected_width;
        projected_height += projected_height;
    }

    if (!surface_movieTGV || surface_movieTGV->GetWidth() != projected_width || surface_movieTGV->GetHeight() != projected_height) {
        if (surface_movieTGV)
            delete surface_movieTGV;
        surface_movieTGV = new RenderTarget(0, 0, projected_width, projected_height, 32, 0x00000000);

        if (interlaced_movies) {
            scale_type = SCALE_TYPE::fit_best;
            surface_movieTGV->Set_Default_SamplerState(pd3dPS_SamplerState_Point);
        }
        surface_movieTGV->ScaleTo((float)clientWidth, (float)clientHeight, scale_type);
        Debug_Info("Movie Surface Created");
    }

    if (!movie_tgv_1) {
        movie_tgv_1 = new DrawSurface8(lp_desc->dwWidth, lp_desc->dwHeight, false, 0);
        Debug_Info_Movie("Movie Create Surface 1 movie_tgv_1 width %d, height %d", lp_desc->dwWidth, lp_desc->dwHeight);
        Set_Scanline_Tex_Height((float)lp_desc->dwHeight);
        movie_tgv_1->ScaleTo((float)projected_width, (float)projected_height, SCALE_TYPE::fill);
        if (interlaced_movies)
            movie_tgv_1->Set_Default_Pixel_Shader(pd3d_PS_Basic_Tex_8_scan_lines);
    }
    *pp_surface8_RT = movie_tgv_1;
    return S_OK;
}


//____________________________________________________________________________________________________________________
static HRESULT __stdcall Movie_Create_Surface_2(LPDDSURFACEDESC lp_desc, DrawSurface8** pp_surface8_RT, IUnknown FAR*) {

    Debug_Info_Movie("Movie Create Surface 2");
    if (!lp_desc)
        return S_FALSE;
    if (!pp_surface8_RT)
        return S_FALSE;

    if (!movie_tgv_2)
        movie_tgv_2 = new DrawSurface8(lp_desc->dwWidth, lp_desc->dwHeight, false, 0);

    *pp_surface8_RT = movie_tgv_2;
    return S_OK;
}


//_________________________________________________________________________________________________
static HRESULT __stdcall Movie_Lock_Surface_1(LPRECT pRect, LPDDSURFACEDESC lp_desc, DWORD, HANDLE) {
    //Debug_Info_Movie("Movie Lock Surface 1");
    if (!lp_desc)
        return S_FALSE;
    if (!movie_tgv_1)
        return S_FALSE;

    BYTE* pSurface = nullptr;
    LONG pitch = 0;

    if (movie_tgv_1->Lock((VOID**)&pSurface, &pitch) != S_OK)
        return S_FALSE;

    lp_desc->lPitch = pitch;
    lp_desc->lpSurface = pSurface;

    return S_OK;
}


//_________________________________________________________________________________________________
static HRESULT __stdcall Movie_Lock_Surface_2(LPRECT pRect, LPDDSURFACEDESC lp_desc, DWORD, HANDLE) {
    //Debug_Info_Movie("Movie Lock Surface 2");
    if (!lp_desc)
        return S_FALSE;
    if (!movie_tgv_2)
        return S_FALSE;

    BYTE* pSurface = nullptr;
    LONG pitch = 0;

    if (movie_tgv_2->Lock((VOID**)&pSurface, &pitch) != S_OK)
        return S_FALSE;

    lp_desc->lPitch = pitch;
    lp_desc->lpSurface = pSurface;

    return S_OK;
}


//_____________________________________________________
static HRESULT __stdcall Movie_Unlock_Surface_2(LPVOID) {
    //Debug_Info_Movie("Movie Unlock Surface 2");
    if (!movie_tgv_2)
        return S_FALSE;

    if (movie_tgv_2)
        movie_tgv_2->Unlock();

    return S_OK;
}


//_____________________________________________________
static HRESULT __stdcall Movie_Unlock_Surface_1(LPVOID) {
    //Debug_Info_Movie("Movie Unlock Surface 1");
    if (!movie_tgv_1)
        return S_FALSE;


    movie_tgv_1->Unlock();

    ID3D11DepthStencilView* depthStencilView = GetDepthStencilView();
    surface_movieTGV->ClearRenderTarget(depthStencilView);
    surface_movieTGV->SetRenderTarget(depthStencilView);

    movie_tgv_1->Display();


    Display_Dx_Present(PRESENT_TYPE::movie);
    return S_OK;
}


//_____________________________________________
static HRESULT  Movie_Destroy_Surface_1(LPVOID) {
    Debug_Info_Movie("Movie Destroy Surface 1");
    if (movie_tgv_1)
        delete movie_tgv_1;
    movie_tgv_1 = nullptr;
    return S_OK;
}


//_____________________________________________
static HRESULT  Movie_Destroy_Surface_2(LPVOID) {
    Debug_Info_Movie("Movie Destroy Surface 2");
    if (movie_tgv_2)
        delete movie_tgv_2;
    movie_tgv_2 = nullptr;
    return S_OK;
}


//___________________________________________________________
static void __declspec(naked) movie_set_palette_entries(void) {

    __asm {
        mov eax, dword ptr ds : [eax]

        pushad
        push 0
        lea eax, dword ptr ds : [eax + 0xAC]
        push eax
        push 256
        push 0
        call Palette_Set_Entries3
        add esp, 0x10
        popad

        test byte ptr ds : [eax + 0x1] , 0x8
        ret

    }
}


//________________________________________________________________
static void __declspec(naked) movie_get_surface_pixel_format(void) {

    __asm {
        mov dword ptr ds : [ebx + 0x1C] , 8
        ret
    }
}


//______________________________________________________________
static void __declspec(naked) movie_get_surface_dimensions(void) {

    __asm {
        mov dword ptr ds:[eax + 0x4], 480
        mov dword ptr ds:[eax + 0x8], 640
        ret
    }
}


//______________________________________________________________________________
static BOOL Play_Movie(const char* tgv_path, BOOL clear_on_start, BOOL fade_out) {
    
    Debug_Info_Movie("Play_Movie_Sequence: main_path: %s, fade_in: %d, fade_out: %d", tgv_path, clear_on_start, fade_out);

    //make sure fade level is a back to full brightness for movie display. 
    Fade(FALSE, TRUE);

    //get the hd movie path, skip movie playback if this movie has been flagged in movies.ini
    std::string hd_movie_path;
    if (Get_Movie_Path(tgv_path, &hd_movie_path) == -1) {
        if (clear_on_start)
            surface_gui->Clear_Texture(0x00000000);
        if (fade_out)
            Fade(TRUE, TRUE);
        return FALSE;
    }

    p2_music_stop();

    if (pMovie_vlc)
        delete pMovie_vlc;

    pMovie_vlc = new LibVlc_Movie(hd_movie_path.c_str());

    BOOL exit_flag = FALSE;
    BOOL escape_flag = FALSE;
    BOOL play_successfull = FALSE;
    MOVIE_STATE movie_state{ 0 };

    if (pMovie_vlc->Play()) {
        if (clear_on_start)
            surface_gui->Clear_Texture(0x00000000);
        
        play_successfull = TRUE;
        while (!exit_flag) {

            escape_flag = p2_check_key_state(0x1, 0x8, 0x10);
            if(escape_flag)
                exit_flag = TRUE;

            if (!pMovie_vlc->IsPlaying(&movie_state)) {
                if (!movie_state.hasPlayed) {
                    Debug_Info_Error("HD Movie Playing: Error Encounted!");
                    play_successfull = FALSE;
                }
                else
                    Debug_Info_Movie("HD Movie Played OK");

                exit_flag = TRUE;
            }
            //improves p2_check_key_state key press detection.
            Sleep(50);
        }
        pMovie_vlc->Stop();
    }
    //if alternate movie failed to play, continue movie using original player.
    if (!play_successfull) {
        delete pMovie_vlc;
        pMovie_vlc = nullptr;
        Debug_Info_Movie("Play Original Movie");
        return p2_movie_play(tgv_path, clear_on_start, fade_out);
    }


    if (fade_out)
        Fade(TRUE, FALSE);

    Debug_Info_Movie("HD Movie: Done");

    p2_music_start(1, 8, 0);
    //return escape flag to skip other movies in sequence.
    return escape_flag;
}


//_________________________
void Modifications_Movies() {

    //set movie palette entries, this was previously done in another function.
    MemWrite16(0x4894FD, 0x008B, 0xE890);
    FuncWrite32(0x4894FF, 0x080140F6, (DWORD)&movie_set_palette_entries);

    //movie surface 1
    //replace IDirectDraw_CreateSurface(p, a, b, c)
    MemWrite16(0x4894D8, 0x1A8B, 0xE890);
    FuncWrite32(0x4894DA, 0x1853FF52, (DWORD)&Movie_Create_Surface_1);

    //movie surface 2
    //replace IDirectDraw_CreateSurface(p, a, b, c)
    MemWrite16(0x4894F2, 0x1A8B, 0xE890);
    FuncWrite32(0x4894F4, 0x1853FF52, (DWORD)&Movie_Create_Surface_2);


    //movie surface 1
    //prevent call to IDirectDrawSurface_IsLost(p) set ret eax to 0.
    MemWrite16(0x489527, 0x8B50, 0xB890);
    MemWrite32(0x489529, 0x6052FF10, 0x0);

    //movie surface 1
    //replace IDirectDrawSurface_Lock(p, a, b, c, d)
    MemWrite16(0x489554, 0x108B, 0xE890);
    FuncWrite32(0x489556, 0x6452FF50, (DWORD)&Movie_Lock_Surface_1);


    //movie surface 2
    //prevent call to IDirectDrawSurface_IsLost(p) set ret eax to 0.
    MemWrite16(0x489576, 0x8B50, 0xB890);
    MemWrite32(0x489578, 0x6052FF10, 0x0);

    //movie surface 2
    //replace IDirectDrawSurface_Lock(p, a, b, c, d)
    MemWrite16(0x4895A3, 0x188B, 0xE890);
    FuncWrite32(0x4895A5, 0x6453FF50, (DWORD)&Movie_Lock_Surface_2);


    //movie surface 2
    //replace IDirectDrawSurface_Unlock(p, b)
    MemWrite8(0x48970F, 0x50, 0x90);
    MemWrite16(0x489710, 0x108B, 0x9090);
    MemWrite16(0x489712, 0x92FF, 0xE890);
    FuncWrite32(0x489714, 0x80, (DWORD)&Movie_Unlock_Surface_2);

    //movie surface 1
    //replace IDirectDrawSurface_Unlock(p, b)
    MemWrite8(0x489722, 0x50, 0x90);
    MemWrite16(0x489723, 0x108B, 0x9090);
    MemWrite16(0x489725, 0x92FF, 0xE890);
    FuncWrite32(0x489727, 0x80, (DWORD)&Movie_Unlock_Surface_1);


    //movie surface 1
    //replace  IDirectDrawSurface_Release(p) 
    MemWrite16(0x4892D4, 0x8B53, 0xE890);
    FuncWrite32(0x4892D6, 0x0852FF13, (DWORD)&Movie_Destroy_Surface_1);

    //movie surface 2
    //replace  IDirectDrawSurface_Release(p) 
    MemWrite16(0x4892EC, 0x8B51, 0xE890);
    FuncWrite32(0x4892EE, 0x0852FF11, (DWORD)&Movie_Destroy_Surface_2);


    //skip un-needed DirectDraw blit, flip and set palette function.
    MemWrite8(0x47494F, 0xE8, 0x90);
    MemWrite32(0x474950, 0x013544, 0x90909090);

    //replace function calling IDirectDrawSurface_GetPixelFormat(p,a)
    MemWrite8(0x487D68, 0x51, 0xE9);
    FuncWrite32(0x487D69, 0xE5895556, (DWORD)&movie_get_surface_pixel_format);

    //replace function calling IDirectDrawSurface_GetSurfaceDesc(p,a)
    //getting with and height of surface.
    MemWrite16(0x487E44, 0x5153, 0xE990);
    FuncWrite32(0x487E46, 0xE5895552, (DWORD)&movie_get_surface_dimensions);

    //skip IDirectDraw_CreatePalette and IDirectDrawSurface_SetPalette
    MemWrite8(0x48921F, 0x75, 0xEB);


    //jump over un-needed end of movie draw and palette stuff.
    MemWrite8(0x46937E, 0xE8, 0xE9);//JMP 004694E5
    MemWrite32(0x46937F, 0x2B6D, 0x0162);


    //replace play movie function calls to play alternate movies using libvlc
    FuncReplace32(0x41B254, 0x04DF3C, (DWORD)&Play_Movie);
    FuncReplace32(0x41C1B0, 0x04CFE0, (DWORD)&Play_Movie);
    FuncReplace32(0x41D1ED, 0x04BFA3, (DWORD)&Play_Movie);
    FuncReplace32(0x41D27F, 0x04BF11, (DWORD)&Play_Movie);
    FuncReplace32(0x41D296, 0x04BEFA, (DWORD)&Play_Movie);
    FuncReplace32(0x41D2B6, 0x04BEDA, (DWORD)&Play_Movie);
    FuncReplace32(0x41D2D1, 0x04BEBF, (DWORD)&Play_Movie);
    FuncReplace32(0x41D2E8, 0x04BEA8, (DWORD)&Play_Movie);

    //------------------------------------------------
    //jump over interlaced check 0x40 ?
    //prevent dx surface being widened to width x 2.
    //forces the use of dx surface pitch when drawing.

    //when creating dx surfaces.
    MemWrite8(0x4894BC, 0x74, 0xEB);
    //not 8 bit?
    MemWrite8(0x4895F9, 0x74, 0xEB);
    //3
    MemWrite8(0x48961D, 0x74, 0xEB);
    //4
    MemWrite16(0x489674, 0x0675, 0x9090);
    //in 1 first func
    MemWrite8(0x4934A1, 0x74, 0xEB);
    //in 1 draw func
    MemWrite8(0x49350F, 0x74, 0xEB);
    //in 1 draw func
    MemWrite16(0x493590, 0x840F, 0xE990);
    //------------------------------------------------
}
