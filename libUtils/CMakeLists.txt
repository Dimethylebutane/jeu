cmake_minimum_required (VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(libUtils)
set(LIBNAME libUtils)
string(TOLOWER ${LIBNAME} LIBNAME_LOWERCASE)

set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_library(${LIBNAME}
            STATIC
            "src/Logger.cpp")

target_include_directories(${LIBNAME} PUBLIC ${CMAKE_SOURCE_DIR}/${LIBNAME}/include)