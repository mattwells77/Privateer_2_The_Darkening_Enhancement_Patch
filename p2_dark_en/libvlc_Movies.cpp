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

#include "libvlc_Movies.h"
#include "configTools.h"
#include "dark.h"

std::string movie_dir;
std::string movie_ext;
std::string movie_config_path;

BOOL are_movie_path_setting_set = FALSE;
int branch_offset_ms = 0;


LibVlc_Movie* pMovie_vlc = nullptr;

std::string language_audio;
std::string language_subs;
BOOL subtitles_enabled = FALSE;

//__________________________________________
static bool isPathRelative(const char* path) {

    if (isalpha(path[0])) {
        if (path[1] == ':' && path[2] == '\\')
            return false;
    }
    else if (path[0] == '\\')
        return false;
    return true;
}


//Set the absolute path for movie config file.
//_________________________________
static void Set_Movie_Config_Path() {
    movie_config_path.clear();
    if (isPathRelative(movie_dir.c_str())) {
        char* pMoviePath = new char[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, pMoviePath);
        movie_config_path.assign(pMoviePath);
        movie_config_path.append("\\");
        delete[] pMoviePath;
    }
    movie_config_path.append(movie_dir);
    movie_config_path.append("movies.ini");
}


//______________________________
static BOOL Set_Movie_Settings() {
    if (are_movie_path_setting_set)
        return TRUE;

    char* char_buff = new char[MAX_PATH] {0};
    char* char_ptr = char_buff;
    
    wchar_t* wchar_buff = new wchar_t[MAX_PATH] {0};
    size_t num_bytes = 0;

    //set subtitles enabled flag.
    subtitles_enabled = ConfigReadInt(L"MAIN", L"SUBS_ENABLED", CONFIG_MAIN_SUBS_ENABLED);
    //set subs language.
    ConfigReadString(L"MAIN", L"SUBS_LANG", CONFIG_MAIN_SUBS_LANG, wchar_buff, MAX_PATH);
    //convert wstring to string
    wcstombs_s(&num_bytes, char_buff, MAX_PATH, wchar_buff, _TRUNCATE);
    language_subs.assign("[");
    language_subs.append(char_buff);
    language_subs.append("]");

    //set subs language.
    ConfigReadString(L"MAIN", L"AUDIO_LANG", CONFIG_MAIN_AUDIO_LANG, wchar_buff, MAX_PATH);
    //convert wstring to string
    wcstombs_s(&num_bytes, char_buff, MAX_PATH, wchar_buff, _TRUNCATE);
    language_audio.assign("[");
    language_audio.append(char_buff);
    language_audio.append("]");

    //set the hd movie directory.
    ConfigReadString(L"MOVIES", L"PATH", CONFIG_MOVIES_PATH, wchar_buff, MAX_PATH);
    //convert wstring to string
    wcstombs_s(&num_bytes, char_buff, MAX_PATH, wchar_buff, _TRUNCATE);

    //skip over any space chars
    while (*char_ptr == ' ')
        char_ptr++;
    
    movie_dir = char_ptr;
    //make sure path ends with a back-slash.
    if (movie_dir.at(movie_dir.length() - 1) != '\\')
        movie_dir.append("\\");
    Debug_Info_Movie("movie directory: %s", movie_dir.c_str());

    Set_Movie_Config_Path();
    Debug_Info_Movie("movie_config_path: %s", movie_config_path.c_str());

    //get the hd movie extension.
    ConfigReadString(L"MOVIES", L"EXT", CONFIG_MOVIES_EXT, wchar_buff, MAX_PATH);
    //convert wstring to string
    wcstombs_s(&num_bytes, char_buff, MAX_PATH, wchar_buff, _TRUNCATE);

    //skip over any space chars
    char_ptr = char_buff;
    while (*char_ptr == ' ')
        char_ptr++;
    
    movie_ext.clear();
    ////make sure extension begins with a dot.
    if (char_ptr[0] != '.')
        movie_ext.append(".");
    movie_ext.append(char_ptr);
    Debug_Info_Movie("movie extension: %s", movie_ext.c_str());


    delete[] char_buff;
    delete[] wchar_buff;
    return are_movie_path_setting_set = TRUE;
}


//_______________________________________________________________
BOOL Get_Movie_Path(const char* tgv_path, std::string* p_retPath) {
    Set_Movie_Settings();

    if (!p_retPath)
        return FALSE;
    p_retPath->clear();

    char movie_name[9]{ 0 };

    const char* movie_name_start = strrchr(tgv_path, '\\');

    if (movie_name_start) {
        movie_name_start += 1;
        strncpy_s(movie_name, _countof(movie_name), movie_name_start, _countof(movie_name) - 1);
        char* ext = strrchr(movie_name, '.');
        if (ext) {
            *ext = '\0';
        }
        char* c = movie_name;
        while (*c) {
            *c = tolower(*c);
            c++;
        }

        if (GetPrivateProfileIntA(movie_name, "skip", FALSE, movie_config_path.c_str())) {
            Debug_Info_Movie("Skipping Movie: %s", movie_name);
            return -1;
        }
        p_retPath->assign(movie_dir);
        p_retPath->append(movie_name);
        p_retPath->append(movie_ext);
    }
    Debug_Info_Movie("Get_Movie_Name_From_Path: %s", p_retPath->c_str());

    return TRUE;
}


#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
//_________________________________________________________________________________________________________________________________
static bool video_output_setup_cb(void** opaque, const libvlc_video_setup_device_cfg_t* cfg, libvlc_video_setup_device_info_t* out) {
    Debug_Info_Movie("LibVlc_Movie: video_output_setup_cb");
    //LibVlc_Movie* libvlc_movie = static_cast<LibVlc_Movie*>(*opaque);
    out->d3d11.device_context = GetD3dDeviceContext();
    out->d3d11.context_mutex = nullptr;
    return true;
}

//_______________________________________________
static void video_output_cleanup_cb(void* opaque) {
    Debug_Info_Movie("LibVlc_Movie: video_output_cleanup_cb");
    LibVlc_Movie* libvlc_movie = static_cast<LibVlc_Movie*>(opaque);
    libvlc_movie->Destroy_Surface();

    Reset_DX11_Shader_Defaults();
}

//____________________________________________________________________________________________________________________
static bool video_update_output_cb(void* opaque, const libvlc_video_render_cfg_t* cfg, libvlc_video_output_cfg_t* out) {

    LibVlc_Movie* libvlc_movie = static_cast<LibVlc_Movie*>(opaque);
    unsigned width = cfg->width;
    if (width == 0)
        width = 8;
    unsigned height = cfg->height;
    if (height == 0)
        height = 8;
    Debug_Info_Movie("LibVlc_Movie: video_update_output_cb %d, %d", width, height);

    libvlc_movie->Set_Surface(width, height);
    RenderTarget* surface = libvlc_movie->Get_Surface();
    surface->ScaleTo((float)clientWidth, (float)clientHeight, SCALE_TYPE::fit);

    out->dxgi_format = DXGI_FORMAT_B8G8R8A8_UNORM;
    out->full_range = true;
    out->colorspace = libvlc_video_colorspace_BT709;
    out->primaries = libvlc_video_primaries_BT709;
    out->transfer = libvlc_video_transfer_func_SRGB;
    out->orientation = libvlc_video_orient_top_left;

    return true;
}

//_____________________________________
static void video_swap_cb(void* opaque) {
    //Debug_Info_Movie("LibVlc_Movie: video_swap_cb");
    LibVlc_Movie* libvlc_movie = static_cast<LibVlc_Movie*>(opaque);
    if (!libvlc_movie->IsReadyToDisplay()) 
        return;
    Display_Dx_Present(PRESENT_TYPE::movie);
}

//________________________________________________________
static bool video_makeCurrent_cb(void* opaque, bool enter) {

    LibVlc_Movie* libvlc_movie = static_cast<LibVlc_Movie*>(opaque);
    libvlc_movie->SetReadyForDisplay();
    if (!libvlc_movie->IsReadyToDisplay())
        return false;
    RenderTarget* surface = libvlc_movie->Get_Surface();
    if (!surface)
        return false;
    if (enter) {
        //Debug_Info_Movie("LibVlc_Movie: video_makeCurrent_cb enter");
        surface->ClearRenderTarget(GetDepthStencilView());
    }
    else {
        //Debug_Info_Movie("LibVlc_Movie: video_makeCurrent_cb exit");
        Reset_DX11_Shader_Defaults();
    }
    return true;
}

//_____________________________________________________________________________
static bool video_output_select_plane_cb(void* opaque, size_t plane, void* out) {
    //Debug_Info_Movie("LibVlc_Movie: video_output_select_plane_cb");
    ID3D11RenderTargetView** output = static_cast<ID3D11RenderTargetView**>(out);
    LibVlc_Movie* libvlc_movie = static_cast<LibVlc_Movie*>(opaque);
    RenderTarget* surface = libvlc_movie->Get_Surface();
    if (!surface)
        return false;
    if (plane != 0) //DXGI_FORMAT_R8G8B8A8_UNORM should only have the one plane.
        return false;
    //return ID3D11RenderTargetView.
    *output = surface->GetRenderTargetView();
    return true;
}
#endif


//_____________________________________________
LibVlc_Movie::LibVlc_Movie(const char* in_path) {

    Set_Movie_Settings();

    //Get_Movie_Path(in_path, &path);
    path.assign(in_path);

    isPlaying = false;
    hasPlayed = false;
    isError = false;

    surface = nullptr;
    
    position = 0;
    paused = false;
    is_vlc_playing = false;
   
    using namespace std::placeholders; // for `_1`

    mediaPlayer.eventManager().onPlaying(std::bind(&LibVlc_Movie::on_play, this));
    mediaPlayer.eventManager().onStopped(std::bind(&LibVlc_Movie::on_stopped, this));
    mediaPlayer.eventManager().onEncounteredError(std::bind(&LibVlc_Movie::on_encountered_error, this));

#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    is_stop_set = false;
    mediaPlayer.eventManager().onStopping(std::bind(&LibVlc_Movie::on_end_reached, this));
#else
    mediaPlayer.eventManager().onEndReached(std::bind(&LibVlc_Movie::on_end_reached, this));
#endif
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    libvlc_video_set_output_callbacks(mediaPlayer, libvlc_video_engine_d3d11,
        video_output_setup_cb, video_output_cleanup_cb, nullptr,
        video_update_output_cb, video_swap_cb, video_makeCurrent_cb,
        nullptr, nullptr, video_output_select_plane_cb, this);
#else
    mediaPlayer.setVideoCallbacks(std::bind(&LibVlc_Movie::lock, this, _1), std::bind(&LibVlc_Movie::unlock, this, _1, _2), std::bind(&LibVlc_Movie::display, this, _1));
    mediaPlayer.setVideoFormatCallbacks(std::bind(&LibVlc_Movie::format, this, _1, _2, _3, _4, _5), std::bind(&LibVlc_Movie::cleanup, this));
#endif

   Debug_Info_Movie("LibVlc_Movie: Create Done: %s", path.c_str());
}


//_______________________
bool LibVlc_Movie::Play() {

    if (isError)
        isPlaying = false;
    else {
        Debug_Info_Movie("LibVlc_Movie: InitialiseForPlay_Start initialising...: %s", path.c_str());
        SetMedia(path);
        isPlaying = mediaPlayer.play();
        if (!isPlaying)
            isError = true;
        if (isError)
            return false;
        //subtitles can not be setup until media is actually playing - on_play() function called.
        while (!is_vlc_playing && !isError) {
            Sleep(0);
        }
        if (isError)
            return false;
        Initialise_Subtitles();
        Initialise_Audio();
    }
    return isPlaying;
}


//___________________________________________
void LibVlc_Movie::SetMedia(std::string path) {

    VLC::Media media;

#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    media = VLC::Media(path, VLC::Media::FromPath);
    //////////////libvlc subtitle autodetection path (bug) work-around////////////////////////////////////////////
    //To-Do - Check for this issue in later versions. Current version = vlc-4.0.0-dev-win32-3db080cb
    //The current libvlc 4 nightly doesn't seem to scan the ".\subtitles" folders as the stable v3 does.
    //Needed to add the full path here, to get things working.
    std::string subs_path = "sub-autodetect-path=";
    subs_path.append(movie_dir);
    subs_path.append("subtitles\\");
    libvlc_media_add_option(media, subs_path.c_str());
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else
    Debug_Info_Movie("LibVlc_Movie: SetMedia: %s", path.c_str());
    media = VLC::Media(vlc_instance, path, VLC::Media::FromPath);

#endif
    mediaPlayer.setMedia(media);
}


//_______________________________________
void LibVlc_Movie::Initialise_Subtitles() {

#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    VLC::MediaPtr media_p = mediaPlayer.media();
    //libvlc_media_parse_request(vlc_instance, *media_p, libvlc_media_parse_local, -1);
    libvlc_media_tracklist_t* p_sub_tracklist = libvlc_media_player_get_tracklist(mediaPlayer, libvlc_track_type_t::libvlc_track_text, false);
    size_t subCount = libvlc_media_tracklist_count(p_sub_tracklist);
#else
    size_t subCount = mediaPlayer.spuCount();
#endif
    Debug_Info_Movie("LibVlc_Movie: Subtitle track count: %d", subCount);

    if (subtitles_enabled == FALSE)
#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
        libvlc_media_player_unselect_track_type(mediaPlayer, libvlc_track_type_t::libvlc_track_text);
#else
        mediaPlayer.setSpu(-1);
#endif
    else {
        Debug_Info_Movie("LibVlc_Movie: Search for subtitles: %s", language_subs.c_str());

#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
        for (size_t i = 0; i < subCount; i++) {
            libvlc_media_track_t* p_sub = libvlc_media_tracklist_at(p_sub_tracklist, i);
            std::string name = p_sub->psz_name;

            if (name.find(language) != std::string::npos) {
                Debug_Info_Movie("LibVlc_Movie: Subtitle track selected ID: %d, name: %s", p_sub->i_id, name.c_str());
                libvlc_media_player_select_track(mediaPlayer, p_sub);
            }
        }
        libvlc_media_tracklist_delete(p_sub_tracklist);
#else
        std::vector <VLC::TrackDescription> trackDescriptions = mediaPlayer.spuDescription();
        for (size_t i = 0; i < trackDescriptions.size(); i++) {
            VLC::TrackDescription description = trackDescriptions[i];
            //find a subtitle track description containing matching language text and set it for display.
            if (description.name().find(language_subs) != std::string::npos) {
                Debug_Info_Movie("LibVlc_Movie: Subtitle track selected ID: %d, name: %s", description.id(), description.name().c_str());
                mediaPlayer.setSpu(description.id());
            }
        }
#endif
    }
}


//___________________________________
void LibVlc_Movie::Initialise_Audio() {

#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    VLC::MediaPtr media_p = mediaPlayer.media();
    //libvlc_media_parse_request(vlc_instance, *media_p, libvlc_media_parse_local, -1);
    libvlc_media_tracklist_t* p_audio_tracklist = libvlc_media_player_get_tracklist(mediaPlayer, libvlc_track_type_t::libvlc_track_audio, false);
    size_t audioCount = libvlc_media_tracklist_count(p_audio_tracklist);
#else
    size_t audioCount = mediaPlayer.audioTrackCount();
#endif
    Debug_Info_Movie("LibVlc_Movie: Audio track count: %d", audioCount);
    Debug_Info_Movie("LibVlc_Movie: Search for audio: %s", language_audio.c_str());

#if LIBVLC_VERSION_INT >= LIBVLC_VERSION(4, 0, 0, 0)
    for (size_t i = 0; i < audioCount; i++) {
        libvlc_media_track_t* p_track = libvlc_media_tracklist_at(p_audio_tracklist, i);
        std::string name = p_track->psz_name;

        if (name.find(language) != std::string::npos) {
            Debug_Info_Movie("LibVlc_Movie: Audio track selected ID: %d, name: %s", p_track->i_id, name.c_str());
            libvlc_media_player_select_track(mediaPlayer, p_track);
        }

    }
    libvlc_media_tracklist_delete(p_audio_tracklist);
#else
    std::vector <VLC::TrackDescription> trackDescriptions = mediaPlayer.audioTrackDescription();
    for (size_t i = 0; i < trackDescriptions.size(); i++) {
        VLC::TrackDescription description = trackDescriptions[i];
        //find a audio track description containing matching language text.
        if (description.name().find(language_audio) != std::string::npos) {
            Debug_Info_Movie("LibVlc_Movie: Audio track selected ID: %d, name: %s", description.id(), description.name().c_str());
            mediaPlayer.setAudioTrack(description.id());
        }
    }
#endif
}
