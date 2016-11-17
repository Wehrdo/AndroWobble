# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.4.1)


include_directories(src/main/jni/private/)

file(GLOB SOURCES src/main/jni/private/*.cpp)
file(GLOB HEADERS src/main/jni/private/*.cpp)

# Specifies a library name, specifies whether the library is STATIC or
# SHARED, and provides relative paths to the source code. You can
# define multiple libraries by adding multiple add.library() commands,
# and CMake builds them for you. When you build your app, Gradle
# automatically packages shared libraries with your APK.

add_library( # Specifies the name of the library.
             howie

             # Sets the library as a shared library.
             STATIC

             # Provides a relative path to your source file(s).
             ${SOURCES}
             ${HEADERS}
             )

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log)

find_library(OpenSLES-lib,
             OpenSLES)

#include_directories(${ANDROID_NDK}/platforms/${ANDROID_PLATFORM}/${ANDROID_ABI}/usr/include/SLES)

target_link_libraries( # Specifies the target library.
                       howie

                       # Links the target library to the log library
                       # included in the NDK.
                       ${OpenSLES-lib}
                       ${log-lib})