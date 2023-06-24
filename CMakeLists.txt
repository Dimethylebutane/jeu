# CMakeList.txt : CMake project for jeu, include source and define
# project specific logic here.

cmake_minimum_required (VERSION 3.19)

SET(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_C_COMPILER /usr/bin/clang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

project ("JEU" VERSION 0.0.1 DESCRIPTION "ksdjhksdjh")
message("Started CMake for ${PROJECT_NAME} v${PROJECT_VERSION}...")

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

    #define stuff
#set where to look for lib
set(CMAKE_LIBRARY_PATH "/usr/lib")
set(CMAKE_INCLUDE_PATH "/usr/include")
#resources path
set(RESOURCESPATH "${PROJECT_BINARY_DIR}/Resources")
#macro for program
add_compile_definitions(RESOURCES_PATH="${RESOURCESPATH}")

add_subdirectory ("libUtils")
add_subdirectory ("Engine")
add_subdirectory ("ContentLoader")
