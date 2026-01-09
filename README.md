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

## Acknowledgements:
- Most of the sound issues were fixed by rerouting function calls within the Miles Sound System library "WAIL32.DLL" to a newer version "MSS32.DLL". Credit for the method used should go to [MildewMan1](https://github.com/MildewMan1 ""), Who used this in his [DeadlockFixes](https://github.com/MildewMan1/DeadlockFixes "") project.
- I'd also like to thank the creators of [radgametools](https://github.com/vogonsorg/radgametools ""), a compilation of Bink, Smacker, miles, etc binaries. From which, after a lot of trial and error I found a version of "MSS32.DLL" with the least amount of sound stutter. This file was originally listed in their project as "6.1d-mss32.DLL".
  
## Installation:
- Click on [Releases](https://github.com/mattwells77/Privateer_2_The_Darkening_Enhancement_Patch/releases) and download the latest "p2_dark_en_x.x.x.zip" where "x" is the version number and [libvlc_min_pack_3.0.21.zip](https://github.com/mattwells77/Wing_Commander_III_Enhancement_Patch/releases/tag/libvlc_min_pack_3.0.21) files.
- You may wish to rename or back-up the original "WAIL32.DLL", as this file will be replaced by this patch.
- Extract the contents of these files to your Privateer 2 Install directory.
- Run the "DARK.EXE" to play.

#### CD version notes:
- The patch is expecting a complete hard drive install. If you intend to play the game from CD's set "NO_CD=0" in the "p2_dark_en.ini".

#### GOG DOS version notes:
- You must first upgrade to the Windows version to play. If installing [DOS to Windows Deluxe Edition Patch](https://originalthp.solsector.net/privateer2win95/ ""), you won't need to install the "Privateer 2 Enhancement patch by HCl" as his and my patches do similar things. You also shouldn't use the "LAUNCHER.EXE" that comes with the patch but instead run "DARK.EXE" to play the game.

## Compiling:
- To compile, this project requires the [VLC media player sdk vlc-3.0.21](https://download.videolan.org/pub/videolan/vlc/last/win32/) and the [libvlcpp C++ bindings](https://github.com/videolan/libvlcpp). I've set up relative paths for these, and they should be installed adjacent to the projects solution folder to avoid the need to modify any project settings.
- There is also an included batch file "post_build_copy_to_game_folder.cmd". That is set to copy the newly created DLL to your game folder after the build process completes. This should be edited to match your game installation path.

## Donations:
I don't need your money. If however, my work has inspired you with an overwhelming urge to part with some of your hard earned cash. Then have at it! Cheers.
| Paypal |
| ------ |
[![](https://www.paypalobjects.com/en_AU/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/donate/?business=9MPA8DTFJQFKW&no_recurring=1&currency_code=AUD)
