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

#include "Display_DX11.h"
#include "libvlc_common.h"


struct MOVIE_STATE {
    bool isPlaying;
    bool hasPlayed;
    bool isError;
};


class LibVlc_Movie {
public:
    LibVlc_Movie(const char* path);

    ~LibVlc_Movie() {
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
        is_stop_set = true;
        mediaPlayer.stopAsync();
#else
        mediaPlayer.stop();
#endif
        while (is_vlc_playing)//ensure vlc is done with surface. 
            Sleep(0);
        if (surface)
            delete surface;
        surface = nullptr;
        Debug_Info_Movie("LibVlc_Movie: destroy: %s", path.c_str());
    };

    bool Play();
    void Pause(bool pause) {
        if (isPlaying) {
            Debug_Info_Movie("LibVlc_Movie: Pause: %d", pause);
            paused = pause;
            //using this function primarily when window is resizing or loosing focus.
            //need to use stop here instead of pause as this was causing crashes.
            if (pause) {
                position = mediaPlayer.position();
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
                is_stop_set = true;
                mediaPlayer.stopAsync();
                while (is_vlc_playing)//ensure vlc is done with surface. 
                    Sleep(0);
#else
                mediaPlayer.stop();
#endif
            }
            else {
                mediaPlayer.play();
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
                mediaPlayer.setPosition(position, false);
#else
                mediaPlayer.setPosition(position);
#endif
                //subtitles must be re-initialised whenever media playback is stopped. subtitles can not be setup until media is actually playing - on_play() function called.
                while (!is_vlc_playing && !isError)
                    Sleep(0);
                Initialise_Subtitles();
                Initialise_Audio();
            }
        }
    };
    void SetMedia(std::string path);

    void Stop() {

#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
        is_stop_set = true;
        mediaPlayer.stopAsync();
        while (is_vlc_playing)//ensure vlc is done with surface. 
            Sleep(0);
#else
        mediaPlayer.stop();
#endif
        isPlaying = false;

        Debug_Info_Movie("LibVlc_Movie: Stop: %s", path.c_str());
    };
    bool IsPlaying() const {
        return isPlaying;
    }
    bool IsPlaying(MOVIE_STATE* p_movie_state) const {

        if (p_movie_state) {
            //p_movie_state->branch = branch;
            //p_movie_state->list_num = list_num;
            p_movie_state->isPlaying = isPlaying;
            p_movie_state->hasPlayed = hasPlayed;
            p_movie_state->isError = isError;
        }

        return isPlaying;
    }
    bool HasPlayed() const {
        return hasPlayed;
    }
    bool IsError() const {
        return isError;
    };
    void SetScale() {
        if (surface) {
            surface->ScaleTo((float)clientWidth, (float)clientHeight, SCALE_TYPE::fit);
            Debug_Info_Movie("LibVlc_Movie: SetScale: %s", path.c_str());
        }
    }
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    RenderTarget* Set_Surface(unsigned int width, unsigned int height) {
        if (!surface || width != surface->GetWidth() || height < surface->GetHeight()) {
            if (surface)
                delete surface;
            surface = new RenderTarget(0, 0, width, height, 32, 0);
        }
        return surface;
    }
    RenderTarget* Get_Surface() {
        return surface;
    }
    bool IsReadyToDisplay() const {
        if (!initialised_for_play || !isPlaying)
            return false;
        return true;
    }
    void SetReadyForDisplay() {
        if (!initialised_for_play) {
            Debug_Info_Movie("LibVlc_Movie: initialised_for_play SetReadyForDisplay %s", path.c_str());
            InitialiseForPlay_End();
        }
    }
    void Destroy_Surface() {
        cleanup();
    };
#else
    DrawSurface* Get_Surface() {
        return surface;
    }
#endif
protected:
private:
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    VLC::MediaPlayer mediaPlayer = VLC::MediaPlayer(vlc_instance);
#else
    VLC::MediaPlayer mediaPlayer = VLC::MediaPlayer(vlc_instance);
#endif
    std::string path;
    bool isPlaying;
    bool hasPlayed;
    bool isError;
    bool paused;
    float position;
    bool is_vlc_playing;
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    bool is_stop_set;//when stopping after being deliberatly stopped rather than reaching the end of the movie. 
    RenderTarget* surface;
#else
    DrawSurface* surface;
#endif

    void Initialise_Subtitles();
    void Initialise_Audio();

    void on_play() {
        is_vlc_playing = true;
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
        is_stop_set = false;
#endif
        Debug_Info_Movie("LibVlc_Movie: on_play Play started: %s", path.c_str());

    };
    void on_stopped() {
        is_vlc_playing = false;
        if (!paused) {
            isPlaying = false;
            Debug_Info_Movie("LibVlc_Movie: on_stopped Play stopped: %s", path.c_str());
        }
        //if(surface)
        //surface->Clear_Texture(0xFF0000FF);
    };
    void on_encountered_error() {
        isError = true;
        Debug_Info_Error("LibVlc_Movie: Error Encountered !!!: %s", path.c_str());
    };
    void on_end_reached() {
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
        //libvlc v4 has replace onEndReached with onStopping.
        //have to check if it was stopped(!isPlaying) or ending.
        if (is_stop_set || isError)
            return;
#endif
        Debug_Info_Movie("LibVlc_Movie: end reached: %s", path.c_str());
        isPlaying = false;
        hasPlayed = true;

    };

#if LIBVLC_VERSION_INT < LIBVLC_VERSION(4, 0, 0, 0)
    void* lock(void** planes) {
        //Debug_Info("lock: %s", path.c_str());
        BYTE* pSurface = nullptr;
        LONG pitch = 0;

        if (surface->Lock((VOID**)&pSurface, &pitch) != S_OK) {
            Debug_Info_Error("LibVlc_Movie: LOCK FAILED %s", path.c_str());
            return nullptr;
        }
        *planes = (VOID**)pSurface;
        return nullptr;

    };
    void unlock(void* picture, void* const* planes) {
        //Debug_Info("unlock: %s", path.c_str());
        surface->Unlock();

    };
    void display(void* picture) const {
        Display_Dx_Present(PRESENT_TYPE::movie);
    };
    uint32_t format(char* chroma, uint32_t* width, uint32_t* height, uint32_t* p_pitch, uint32_t* lines) {
        memcpy(chroma, "BGRA", 4);
        Debug_Info_Movie("LibVlc_Movie: setVideoFormatCallbacks w:%d, h:%d", *width, *height);
        if (!surface || *width != surface->GetWidth() || *height < surface->GetHeight() || surface->GetPixelWidth() != 4) {
            if (surface)
                delete surface;
            surface = new DrawSurface(0, 0, *width, *height, 32, 0);
            surface->ScaleTo((float)clientWidth, (float)clientHeight, SCALE_TYPE::fit);
            Debug_Info_Movie("LibVlc_Movie: surface created: %s", path.c_str());
        }
        BYTE* pSurface = nullptr;
        LONG pitch = 0;
        if (surface->Lock((VOID**)&pSurface, &pitch) != S_OK)
            return 0;
        surface->Unlock();
        *width = surface->GetWidth();
        *height = surface->GetHeight();
        *p_pitch = pitch;
        *lines = *height;
        return 1;
    };
#endif
    void cleanup() {
        Debug_Info_Movie("LibVlc_Movie: cleanup: %s", path.c_str());
        //if (surface)
        //    delete surface;
        //surface = nullptr;
    };

};


extern LibVlc_Movie* pMovie_vlc;

BOOL Get_Movie_Path(const char* tgv_path, std::string* p_retPath);