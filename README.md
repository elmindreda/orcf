# OFDR Resolution Cap Fix

[![Build and Test](https://github.com/elmindreda/orcf/actions/workflows/build.yml/badge.svg)](https://github.com/elmindreda/orcf/actions/workflows/build.yml)

## What?

This is a fix for the faulty video memory size check in the game _Operation
Flashpoint: Dragon Rising_.  When this fault causes the game to think there
isn't enough video memory, it refuses to set any resolution larger than
1280x1024.

Put the `ddraw.dll` produced by this project in the same directory as `OFDR.exe`
and the game should allow larger resolutions to be set.

This fix Works on My Machine™, but may not work for everyone.


## Why?

I wanted to play _Operation Flashpoint: Dragon Rising_ but couldn't see very far
at the resolutions the game would allow.


## How?

The game calls `IDirectDraw7::GetAvailableVidMem` early on to check if there is
enough video memory for it to allow resolutions higher than 1280x1024.  On my
machine with a 2GB dedicated GPU and 16GB for the integrated GPU to borrow from,
DirectDraw returns 128MB.

This project produces a shim `ddraw.dll` that returns one byte less than 2GB,
the highest value the game appears to accept.

The DLL produced by this project is not a full replacement of the system DLL but
does everything the game needs.  It is placed in the same directory as
`OFDR.exe` .  It intercepts the `IDirectDraw7` interface and the call to
`IDirectDraw7::GetAvailableVidMem` and gives the game a value that satisfies it.
Resolutions up to 1920x1080 have been successfully tested.

This fix is written in C11 and has been verified to build with Visual Studio,
Clang-CL, Clang, MinGW-w64 and MinGW.  You will need CMake to generate build
files for your development environment.

MinGW has no DirectDraw headers so the MinGW-w64 ones are bundled in the `deps`
directory.  They are added to the project by CMake when needed.


## Credits

- People discussing this bug in the Steam forum, whose observations of the bug
  got me started in the right direction
- archive.org, who had saved a copy of the DirectX 7 SDK documentation
- The previous fix, a replacement `OFDR.exe` with an unexplained 10MB added,
  which made me so suspicious I wrote this fix instead

