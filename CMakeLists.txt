
cmake_minimum_required(VERSION 3.15...3.20)

project(orcf VERSION 0.1 DESCRIPTION "OFDR Resolution Cap Fix" LANGUAGES C)

if (NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "This fix must be compiled as 32-bit to be loaded by OFDR.exe")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(orcf_ddraw SHARED)
target_sources(orcf_ddraw PRIVATE orcf_ddraw.c orcf_ddraw.def orcf_ddraw.rc)
target_compile_features(orcf_ddraw PRIVATE c_std_11)

# Ensure our output file is named ddraw.dll
set_target_properties(orcf_ddraw PROPERTIES OUTPUT_NAME ddraw PREFIX "")

# Ensure the test uses the SDK import library and not ours
set_target_properties(orcf_ddraw PROPERTIES IMPORT_PREFIX "orcf_")

# Force the output file to use statically linked UCRT for easier distribution
set_target_properties(orcf_ddraw PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if (MINGW)
    # Use our replacements if MinGW lacks the required headers
    include(CheckIncludeFile)
    check_include_file(ddraw.h has_ddraw_h)
    if (NOT has_ddraw_h)
        target_include_directories(orcf_ddraw PRIVATE deps)
    endif()

    # Avoid a warning about renaming DirectDrawCreateEx from its stdcall name
    target_link_options(orcf_ddraw PRIVATE -Wl,--enable-stdcall-fixup)
endif()

