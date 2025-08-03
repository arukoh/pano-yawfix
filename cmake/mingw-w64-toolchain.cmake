#!/bin/bash
# CMake toolchain file for Windows

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Set cross compiler
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# Set search path
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

# Use host directory to search for programs
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Use target directory to search for libraries and include files
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Windows-specific settings
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
set(CMAKE_SHARED_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

# OpenCL settings (for MinGW cross)
set(OpenCL_INCLUDE_DIR "/usr/x86_64-w64-mingw32/include")
set(OpenCL_LIBRARY "/usr/x86_64-w64-mingw32/lib/libOpenCL.dll.a")

# vcpkg settings
if(DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_TARGET_TRIPLET x64-mingw-static)
    set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
endif()
