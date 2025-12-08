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

const char* appPath = nullptr;


//________________________________
void Check_Optional_Enhancements() {
    
    Modifications_Run_Game_From_Any_DIR();

    if (ConfigReadInt(L"MAIN", L"NO_CD", CONFIG_MAIN_NO_CD)) {
        Modifications_No_CD();
        Debug_Info("Optional_Enhancement: NO-CD mode ENABLED");
    }
}


//______________________________________________
static void __declspec(naked) No_CD_set_cd(void) {
    //set the currently loaded cd number to the number called for on the EAX register.
    __asm {
        push edx

        mov edx, p_p2_current_CD_loaded
        mov word ptr ds : [edx], ax

        pop edx

        ret
    }
}


//________________________________________________
static void __declspec(naked) No_CD_check_cd(void) {
    //skip opening cd.dat to get the currently loaded CD, if the currently loaded CD num is 0(no cd) than set cd to 1.
    __asm {
        mov eax, p_p2_current_CD_loaded
        cmp word ptr ds : [eax], 0
        jne exitfunc
        
        mov word ptr ds : [eax], 1

        exitfunc:

        ret
    }
}


//________________________
void Modifications_No_CD() {
    
    //replace load cd function
    //set the currently loaded cd number to the number called for.
    MemWrite8(0x41CBC0, 0x53, 0xE9);
    FuncWrite32(0x41CBC1, 0x57565251, (DWORD)&No_CD_set_cd);

    //skip opening cd.dat to get the currently loaded CD, if the currently loaded CD num is 0(no cd) than set cd to 1.
    FuncReplace32(0x41D166, 0xFFFFBA12, (DWORD)&No_CD_check_cd);
    MemWrite16(0x41D16A, 0xC085, 0x53EB);//JMP SHORT 0041D1BF

    //prevent game from altering the current directory.
    MemWrite8(0x418B7C, 0x51, 0xC3);

    //in load movie function, set movies to load from a relative path.
    //set CD path to point to a NULL character.
    MemWrite32(0x41B191, 0x52D318, (DWORD)0x4A0223);
    //shift the movie path offset by one to remove the first backslash character, so that "\movies\" becomes "movies\".
    MemWrite32(0x41B1CE, 0x4A021B, 0x4A021C);
}


//________________________________________
// Allows the game to be run from any DIR, rather than "C:\DARK".
void Modifications_Run_Game_From_Any_DIR() {

    //get the current directory from which the game executable was run from. 
    appPath = GetAppPathA();
    Debug_Info("Running game from: %s", appPath);

    //These offsets are used to switch the current directory from the CD to install path.
    //They and are redundant when using the No_CD mod.
    //reroute the original install path pointer to that of the current directory.
    MemWrite32(0x41BB4F, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41BC05, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41BC89, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41BD0D, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41BD6A, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41BDA2, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41BDFF, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41BEBE, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41BF8F, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41C0AD, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41C0D2, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41C234, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41CAE7, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41CBCF, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41CC9E, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41CF4B, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41CF55, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41CF87, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41D0E1, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41D0F3, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41D10C, 0x52D418, (DWORD)appPath);
    MemWrite32(0x41F289, 0x52D418, (DWORD)appPath);
    MemWrite32(0x4210E3, 0x52D418, (DWORD)appPath);
    MemWrite32(0x421565, 0x52D418, (DWORD)appPath);
    //ignore when reading install path from dark.cfg
    //00421861  MOV EAX, OFFSET INSTALL_PATH ? ? ? ? ?
    //00421870  MOV EAX, OFFSET INSTALL_PATH ? ? ? ? ?
    MemWrite32(0x421EB6, 0x52D418, (DWORD)appPath);
    MemWrite32(0x4227A3, 0x52D418, (DWORD)appPath);
    MemWrite32(0x422A3B, 0x52D418, (DWORD)appPath);
    MemWrite32(0x422CE4, 0x52D418, (DWORD)appPath);
    MemWrite32(0x422D5E, 0x52D418, (DWORD)appPath);
    MemWrite32(0x436FED, 0x52D418, (DWORD)appPath);
    MemWrite32(0x4380C5, 0x52D418, (DWORD)appPath);
    MemWrite32(0x458CE9, 0x52D418, (DWORD)appPath);
    MemWrite32(0x45920D, 0x52D418, (DWORD)appPath);
    MemWrite32(0x4592BC, 0x52D418, (DWORD)appPath);
    MemWrite32(0x459398, 0x52D418, (DWORD)appPath);
    MemWrite32(0x45C4FB, 0x52D418, (DWORD)appPath);
    MemWrite32(0x45ED2C, 0x52D418, (DWORD)appPath);
    //ignore when writing install path to dark.cfg
    //0045ED5D  PUSH OFFSET INSTALL_PATH ? ?
    MemWrite32(0x46950E, 0x52D418, (DWORD)appPath);
    MemWrite32(0x4697A0, 0x52D418, (DWORD)appPath);
    MemWrite32(0x46994A, 0x52D418, (DWORD)appPath);
}


//____________________________________________________________________________________________________________________________________________________________________________________________________________________________
static HANDLE __stdcall CreateFile_UAC(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {

        //Debug_Info("CreateFile_UAC: %s, acc: %X, Disposition: %X", lpFileName, dwDesiredAccess, dwCreationDisposition);
        std::wstring path = GetAppDataPath();
        if (!path.empty()) {
            path.append(L"\\");
            HANDLE handle = INVALID_HANDLE_VALUE;
            
            size_t num_bytes = 0;
            wchar_t* wchar_buff = new wchar_t[13] {0};
            if (mbstowcs_s(&num_bytes, wchar_buff, 13, lpFileName, 13) == 0) {
                path.append(wchar_buff);
                Debug_Info("Load file from UAC folder path: %S", path);
                //if the file is being opened with write access and does not exist in the UAC data folder than copy the original to the UAC data folder.
                //dwCreationDisposition is generally set to OPEN_EXISTING or TRUNCATE_EXISTING, requiring these files to already exist.
                if (dwDesiredAccess & GENERIC_WRITE) {
                    DWORD attributes = GetFileAttributes(path.c_str());
                    if (attributes == INVALID_FILE_ATTRIBUTES) {
                        if (CopyFile(wchar_buff, path.c_str(), TRUE))
                            attributes = GetFileAttributes(path.c_str());
                    }
                }
                handle = CreateFile(path.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
            }
            delete[] wchar_buff;
            if (handle != INVALID_HANDLE_VALUE)
                return handle;
        }
        Debug_Info("Load file from UAC folder failed, opening local lpFileName: %s instead.", lpFileName);
    return CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}
void* p_create_file_uac = &CreateFile_UAC;


//_____________________________________________________
static BOOL __stdcall DeleteFile_UAC(LPCSTR lpFileName) {

    Debug_Info("DeleteFile_UAC: %s", lpFileName);
    std::wstring path = GetAppDataPath();
    if (!path.empty()) {
        path.append(L"\\");
        BOOL retVal = FALSE;
        size_t num_bytes = 0;
        wchar_t* wchar_buff = new wchar_t[13] {0};
        if (mbstowcs_s(&num_bytes, wchar_buff, 13, lpFileName, 13) == 0) {
            path.append(wchar_buff);
            retVal = DeleteFile(path.c_str());
        }
        delete[] wchar_buff;
        if (retVal)
            return retVal;
    }
    return DeleteFileA(lpFileName);
}
void* p_delete_file_uac = &DeleteFile_UAC;


//__________________________
void Modifications_General() {

    //-----------------------UAC-Patch---------------------------
    MemWrite32(0x497824, 0x57031C, (DWORD)&p_create_file_uac);
    MemWrite32(0x49777C, 0x57032C, (DWORD)&p_delete_file_uac);
    //__________________________________________________________
}
