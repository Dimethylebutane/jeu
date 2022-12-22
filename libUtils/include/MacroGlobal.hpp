#pragma once

#include <iostream>
#include <fstream>

#include <exception>
#include <string_view>
#include <filesystem>

#include "SpinLock.hpp"
#include "Logger.hpp"

#define ENDL "\n"

void Init()
{
#ifdef NDEBUG //if release
    defaultLogger.change("./log.txt");
#endif // !NDEBUG
}

#pragma region LOGMACRO
//uses std::cout the std buffered output stream - thread safe

#define LOG(ARGS) defaultLogger << "[L] " << ARGS;
//uses std::clog the std buffered error stream - thread safe
#define LOGERR(ARGS) defaultLogger <<  "[E] " << ARGS;

//debug macro
#ifdef NDEBUG
#define DLOG(ARGS) ;
#define DERR(ARGS) ;
#else
//uses std::cout the std buffered output stream - thread safe
#define DLOG(ARGS) defaultLogger << "[L] " << ARGS;
//uses std::clog the std buffered error stream - thread safe
#define DERR(ARGS) defaultLogger <<  "[E] " << ARGS;
#endif

#pragma endregion


//pause (presss any key to cintinue)
#ifdef _WIN64
#define PAUSE std::system("pause");
#else
#define PAUSE std::system(R"(read -p "Press enter to continue " REPLY)");
#endif // _WIN64

#ifndef __unix__ //win
#define ia32pause _mm_pause(); //SpinLock PAUSE
#else   //unix
#define ia32pause __builtin_ia32_pause(); //SpinLock PAUSE
#endif //unix / windows

#define SpinLockPAUSE ia32pause //alias macro

#ifndef NDEBUG
template <typename T, typename E = std::exception>
inline void cpp_assert(T&& assertion, const E throwing = {}) {
    if (!assertion) {
        throw throwing;
    }
}
#else
template <typename T, typename E = std::exception>
inline void cpp_assert(T&& assertion, const E throwing = {}) {;}
#endif // !NDEBUG