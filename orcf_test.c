// SPDX-License-Identifier: MIT

#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#define INITGUID
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define NTDDI_VERSION 0x05010000
#define DIRECTDRAW_VERSION 0x700

#include <windows.h>
#include <ddraw.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    IDirectDraw7* api;

    if (FAILED(DirectDrawCreateEx(NULL, (void**) &api, &IID_IDirectDraw7, NULL)))
    {
        puts("Failed to create DirectDraw");
        exit(EXIT_FAILURE);
    }

    DDSCAPS2 surface_caps = { .dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM };
    DWORD total_memory, free_memory;

    if (FAILED(IDirectDraw7_GetAvailableVidMem(api, &surface_caps, &total_memory, &free_memory)))
    {
        IDirectDraw7_Release(api);
        puts("Failed to retrieve available video memory");
        exit(EXIT_FAILURE);
    }

    IDirectDraw7_Release(api);

    if (total_memory != 0x7fffffff)
    {
        puts("Available video memory does not match what ORCF returns");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

