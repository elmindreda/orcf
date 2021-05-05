// SPDX-License-Identifier: MIT

#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#define INITGUID
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define NTDDI_VERSION 0x05010000
#define DIRECTDRAW_VERSION 0x700

#define PROJECT_NAME L"OFDR Resolution Cap Fix"

#include <windows.h>
#include <ddraw.h>

#include <stdlib.h>
#include <malloc.h>
#include <wchar.h>

typedef HRESULT WINAPI ApiCreateFunction(GUID*,void**,REFIID,IUnknown*);

typedef struct ApiWrapper
{
    IDirectDraw7Vtbl const* vtable;
    LONG volatile count;
    IDirectDraw7* api;
} ApiWrapper;

static HRESULT STDMETHODCALLTYPE api_query_interface(IDirectDraw7* self, REFIID iid, void** object)
{
    ApiWrapper* const wrapper = (ApiWrapper*) self;

    if (!object)
        return E_POINTER;

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IDirectDraw) ||
        IsEqualIID(iid, &IID_IDirectDraw2) ||
        IsEqualIID(iid, &IID_IDirectDraw4) ||
        IsEqualIID(iid, &IID_IDirectDraw7))
    {
        *object = wrapper;
        InterlockedIncrement(&wrapper->count);
        return S_OK;
    }

    *object = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE api_add_ref(IDirectDraw7* self)
{
    ApiWrapper* const wrapper = (ApiWrapper*) self;
    return InterlockedIncrement(&wrapper->count);
}

static ULONG STDMETHODCALLTYPE api_release(IDirectDraw7* self)
{
    ApiWrapper* const wrapper = (ApiWrapper*) self;

    ULONG const count = InterlockedDecrement(&wrapper->count);
    if (!count)
    {
        IDirectDraw7_Release(wrapper->api);
        free(wrapper);
    }

    return count;
}

static HRESULT STDMETHODCALLTYPE api_compact(IDirectDraw7* self)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_Compact(wrapper->api);
}

static HRESULT STDMETHODCALLTYPE api_create_clipper(IDirectDraw7* self, DWORD flags, IDirectDrawClipper** clipper, IUnknown* outer)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_CreateClipper(wrapper->api, flags, clipper, outer);
}

static HRESULT STDMETHODCALLTYPE api_create_palette(IDirectDraw7* self, DWORD flags, PALETTEENTRY* entries, IDirectDrawPalette** palette, IUnknown* outer)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_CreatePalette(wrapper->api, flags, entries, palette, outer);
}

static HRESULT STDMETHODCALLTYPE api_create_surface(IDirectDraw7* self, DDSURFACEDESC2* surface_desc, IDirectDrawSurface7** surface, IUnknown* outer)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_CreateSurface(wrapper->api, surface_desc, surface, outer);
}

static HRESULT STDMETHODCALLTYPE api_duplicate_surface(IDirectDraw7* self, IDirectDrawSurface7* surface, IDirectDrawSurface7** duplicate)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_DuplicateSurface(wrapper->api, surface, duplicate);
}

static HRESULT STDMETHODCALLTYPE api_enum_display_modes(IDirectDraw7* self, DWORD flags, DDSURFACEDESC2* surface_desc, void* user, LPDDENUMMODESCALLBACK2 callback)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_EnumDisplayModes(wrapper->api, flags, surface_desc, user, callback);
}

static HRESULT STDMETHODCALLTYPE api_enum_surfaces(IDirectDraw7* self, DWORD flags, DDSURFACEDESC2* surface_desc, void* user, LPDDENUMSURFACESCALLBACK7 callback)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_EnumSurfaces(wrapper->api, flags, surface_desc, user, callback);
}

static HRESULT STDMETHODCALLTYPE api_flip_to_gdi_surface(IDirectDraw7* self)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_FlipToGDISurface(wrapper->api);
}

static HRESULT STDMETHODCALLTYPE api_get_caps(IDirectDraw7* self, DDCAPS* driver_caps, DDCAPS* hel_caps)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_GetCaps(wrapper->api, driver_caps, hel_caps);
}

static HRESULT STDMETHODCALLTYPE api_get_display_mode(IDirectDraw7* self, DDSURFACEDESC2* surface_desc)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_GetDisplayMode(wrapper->api, surface_desc);
}

static HRESULT STDMETHODCALLTYPE api_get_four_cc_codes(IDirectDraw7* self, DWORD* num_entries, DWORD* entries)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_GetFourCCCodes(wrapper->api, num_entries, entries);
}

static HRESULT STDMETHODCALLTYPE api_get_gdi_surface(IDirectDraw7* self, IDirectDrawSurface7** surface)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_GetGDISurface(wrapper->api, surface);
}

static HRESULT STDMETHODCALLTYPE api_get_monitor_frequency(IDirectDraw7* self, DWORD* frequency)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_GetMonitorFrequency(wrapper->api, frequency);
}

static HRESULT STDMETHODCALLTYPE api_get_scan_line(IDirectDraw7* self, DWORD* scan_line)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_GetScanLine(wrapper->api, scan_line);
}

static HRESULT STDMETHODCALLTYPE api_get_vertical_blank_status(IDirectDraw7* self, BOOL* status)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_GetVerticalBlankStatus(wrapper->api, status);
}

static HRESULT STDMETHODCALLTYPE api_initialize(IDirectDraw7* self, GUID* guid)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_Initialize(wrapper->api, guid);
}

static HRESULT STDMETHODCALLTYPE api_restore_display_mode(IDirectDraw7* self)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_RestoreDisplayMode(wrapper->api);
}

static HRESULT STDMETHODCALLTYPE api_set_cooperative_level(IDirectDraw7* self, HWND window, DWORD flags)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_SetCooperativeLevel(wrapper->api, window, flags);
}

static HRESULT STDMETHODCALLTYPE api_set_display_mode(IDirectDraw7* self, DWORD width, DWORD height, DWORD bpp, DWORD frequency, DWORD flags)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_SetDisplayMode(wrapper->api, width, height, bpp, frequency, flags);
}

static HRESULT STDMETHODCALLTYPE api_wait_for_vertical_blank(IDirectDraw7* self, DWORD flags, HANDLE event)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_WaitForVerticalBlank(wrapper->api, flags, event);
}

static HRESULT STDMETHODCALLTYPE api_get_available_vid_mem(IDirectDraw7* self, DDSCAPS2* surface_caps, DWORD* total_memory, DWORD* free_memory)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    HRESULT const result = IDirectDraw7_GetAvailableVidMem(wrapper->api, surface_caps, total_memory, free_memory);

    if (SUCCEEDED(result))
    {
        // Returning an acceptable value here is what allows higher resolutions
        // The rest of this file is just a means to do this safely
        *total_memory = 0x7fffffff;
    }

    return result;
}

static HRESULT STDMETHODCALLTYPE api_get_surface_from_dc(IDirectDraw7* self, HDC dc, IDirectDrawSurface7** surface)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_GetSurfaceFromDC(wrapper->api, dc, surface);
}

static HRESULT STDMETHODCALLTYPE api_restore_all_surfaces(IDirectDraw7* self)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_RestoreAllSurfaces(wrapper->api);
}

static HRESULT STDMETHODCALLTYPE api_test_cooperative_level(IDirectDraw7* self)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_TestCooperativeLevel(wrapper->api);
}

static HRESULT STDMETHODCALLTYPE api_get_device_identifier(IDirectDraw7* self, DDDEVICEIDENTIFIER2* identifier, DWORD flags)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_GetDeviceIdentifier(wrapper->api, identifier, flags);
}

static HRESULT STDMETHODCALLTYPE api_start_mode_test(IDirectDraw7* self, SIZE* sizes, DWORD num_sizes, DWORD flags)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_StartModeTest(wrapper->api, sizes, num_sizes, flags);
}

static HRESULT STDMETHODCALLTYPE api_evaluate_mode(IDirectDraw7* self, DWORD flags, DWORD* remaining)
{
    ApiWrapper const* const wrapper = (ApiWrapper*) self;
    return IDirectDraw7_EvaluateMode(wrapper->api, flags, remaining);
}

static IDirectDraw7Vtbl const api_wrapper_vtable =
{
    .QueryInterface         = api_query_interface,
    .AddRef                 = api_add_ref,
    .Release                = api_release,
    .Compact                = api_compact,
    .CreateClipper          = api_create_clipper,
    .CreatePalette          = api_create_palette,
    .CreateSurface          = api_create_surface,
    .DuplicateSurface       = api_duplicate_surface,
    .EnumDisplayModes       = api_enum_display_modes,
    .EnumSurfaces           = api_enum_surfaces,
    .FlipToGDISurface       = api_flip_to_gdi_surface,
    .GetCaps                = api_get_caps,
    .GetDisplayMode         = api_get_display_mode,
    .GetFourCCCodes         = api_get_four_cc_codes,
    .GetGDISurface          = api_get_gdi_surface,
    .GetMonitorFrequency    = api_get_monitor_frequency,
    .GetScanLine            = api_get_scan_line,
    .GetVerticalBlankStatus = api_get_vertical_blank_status,
    .Initialize             = api_initialize,
    .RestoreDisplayMode     = api_restore_display_mode,
    .SetCooperativeLevel    = api_set_cooperative_level,
    .SetDisplayMode         = api_set_display_mode,
    .WaitForVerticalBlank   = api_wait_for_vertical_blank,
    .GetAvailableVidMem     = api_get_available_vid_mem,
    .GetSurfaceFromDC       = api_get_surface_from_dc,
    .RestoreAllSurfaces     = api_restore_all_surfaces,
    .TestCooperativeLevel   = api_test_cooperative_level,
    .GetDeviceIdentifier    = api_get_device_identifier,
    .StartModeTest          = api_start_mode_test,
    .EvaluateMode           = api_evaluate_mode
};

HRESULT WINAPI DirectDrawCreateEx(GUID* guid, void** api, REFIID iid, IUnknown* outer)
{
    WCHAR const module_name[] = L"ddraw.dll";
    UINT const system_directory_length = GetSystemDirectoryW(NULL, 0);
    size_t const module_path_size = system_directory_length + wcslen(module_name) + 2;
    WCHAR* const module_path = calloc(module_path_size, sizeof(WCHAR));
    if (!module_path)
    {
        MessageBoxW(NULL, L"Failed to allocate memory", PROJECT_NAME, MB_OK | MB_ICONERROR);
        return DDERR_OUTOFMEMORY;
    }

    GetSystemDirectoryW(module_path, module_path_size);
    wcscat_s(module_path, module_path_size, L"\\");
    wcscat_s(module_path, module_path_size, module_name);

    HMODULE const module = LoadLibraryW(module_path);
    free(module_path);

    if (!module)
    {
        MessageBoxW(NULL, L"Failed to load system ddraw.dll", PROJECT_NAME, MB_OK | MB_ICONERROR);
        return DDERR_GENERIC;
    }

    ApiCreateFunction* const actual_create_api = (ApiCreateFunction*) GetProcAddress(module, "DirectDrawCreateEx");
    if (!actual_create_api)
    {
        MessageBoxW(NULL, L"Failed to load DirectDrawCreateEx from system ddraw.dll", PROJECT_NAME, MB_OK | MB_ICONERROR);

        FreeLibrary(module);
        return DDERR_GENERIC;
    }

    ApiWrapper* const wrapper = calloc(1, sizeof(ApiWrapper));
    if (!wrapper)
    {
        MessageBoxW(NULL, L"Failed to allocate memory", PROJECT_NAME, MB_OK | MB_ICONERROR);

        FreeLibrary(module);
        return DDERR_OUTOFMEMORY;
    }

    wrapper->vtable = &api_wrapper_vtable;
    wrapper->count = 1;

    HRESULT const result = actual_create_api(guid, (void**) &wrapper->api, iid, outer);
    if (FAILED(result))
    {
        FreeLibrary(module);
        free(wrapper);
        return result;
    }

    *api = wrapper;
    return result;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, void* reserved)
{
    return TRUE;
}

