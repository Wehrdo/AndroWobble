# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_VERBOSE_MAKEFILE on)

include_directories(src/main/jni/private/)

file(GLOB SOURCES src/main/jni/private/*.cpp)
file(GLOB HEADERS src/main/jni/private/*.cpp)

# Specifies a library name, specifies whether the library is STATIC or
# SHARED, and provides relative paths to the source code. You can
# define multiple libraries by adding multiple add.library() commands,
# and CMake builds them for you. When you build your app, Gradle
# automatically packages shared libraries with your APK.

add_library( # Specifies the name of the library.
             howie-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${SOURCES}
             ${HEADERS}
             )




find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log)

find_library(OpenSLES-lib
             OpenSLES)

target_link_libraries( # Specifies the target library.
                       howie-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${OpenSLES-lib}
                       ${log-lib})


# copy out the lib binary and remove generated files
set(distribution_DIR ${CMAKE_CURRENT_SOURCE_DIR}../../distribution)

set_target_properties(howie-lib
                      PROPERTIES
                      LIBRARY_OUTPUT_DIRECTORY
                      "${distribution_DIR}/howie-lib/lib/${ANDROID_ABI}")
#add_custom_command(TARGET howie-lib POST_BUILD
#                   COMMAND "${CMAKE_COMMAND}" -E
#                   copy "${CMAKE_CURRENT_SOURCE_DIR}/src/jni/howie.h"
#                   "${distribution_DIR}/howie-lib/include/howie.h.h"
#                   COMMENT "Copying howie-lib to output directory")