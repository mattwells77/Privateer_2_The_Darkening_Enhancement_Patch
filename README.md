# Privateer 2: The Darkening Enhancement Patch
This patch was created for the Windows version (Deluxe Edition) of the PC game "Privateer 2: The Darkening".
This patch seems to work well with the [DOS to Windows Deluxe Edition Patch](https://originalthp.solsector.net/privateer2win95/ ""). Although it has not been fully tested at this time.

This patch uses DirectX 11 for rendering and will likely require Windows 10 or greater to use.

HD Movie playback is achieved using [libvlc](https://www.videolan.org/vlc/libvlc.html "libVLC is the core engine and the interface to the multimedia framework on which VLC media player is based.").

## Current list of enhancements:
- Support for better quality movies via [libvlc](https://www.videolan.org/vlc/libvlc.html "libVLC is the core engine and the interface to the multimedia framework on which VLC media player is based.") (no alternative movie packages exist at present). 
- Windows UAC compliance when the game is installed under "Program Files (x86)".
- Windowed mode, with window resizing options and mouse locking only when required in-game.

## Intended future enhancements:
- HD wide screen space.
- Joystick\Controller axis button mapping.

  ## Current issues (BUGS):
- Sound stuttering.
- Midi music not playing.
- Occasional crashes at the end of movie playback(due to sound issues).

  
## Installation:
- Pending.

## Compiling:
- To compile, this project requires the [VLC media player sdk vlc-3.0.21](https://download.videolan.org/pub/videolan/vlc/last/win32/) and the [libvlcpp C++ bindings](https://github.com/videolan/libvlcpp). I've set up relative paths for these, and they should be installed adjacent to the projects solution folder to avoid the need to modify any project settings.
- There is also an included batch file "post_build_copy_to_game_folder.cmd". That is set to copy the newly created DLL to your game folder after the build process completes. This should be edited to match your game installation path.

## Donations:
I don't need your money. If however, my work has inspired you with an overwhelming urge to part with some of your hard earned cash. Have at it! Cheers.
| Paypal |
| ------ |
[![](https://www.paypalobjects.com/en_AU/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/donate/?business=9MPA8DTFJQFKW&no_recurring=1&currency_code=AUD)
