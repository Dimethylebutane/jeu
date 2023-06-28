#pragma once

#include <iostream>
#include <fstream>

#include <exception>
#include <string_view>
#include <filesystem>
#include <string>

#include "SpinLock.hpp"
#include "Logger.hpp"

#define ENDL "\n"

#pragma region LOGMACRO

#define LOG(ARGS) defaultLogger << "[L] " << ARGS;
#define LOGERR(ARGS) defaultLogger << "[E] " << ARGS;
#define LOGWARN(ARGS) defaultLogger << "[W] " << ARGS;

//debug macro
#ifdef NDEBUG
#define DLOG(ARGS) ;
#define DERR(ARGS) ;
#define DWARN(ARGS) ;
#else
#define DLOG(ARGS) defaultLogger << "[L] " << ARGS;
#define DERR(ARGS) defaultLogger <<  "[E] " << ARGS;
#define DWARN(ARGS) defaultLogger << "[W] " << ARGS;
#endif
#pragma endregion

inline void throwRuntimeError(std::string msg, const int line=0, const char* func="?", const char* file="?")
{
	LOGERR("Error in file " << file << " line " << line << " in function " << func << "\n	" << msg);
	throw std::runtime_error(msg + "\nError in file " + file + " line " + std::to_string(line) + " in function " + func );
}

#define THROW_RUNTIME_ERROR(MSG) throwRuntimeError(MSG, __LINE__, __FUNCTION__, __FILE_NAME__)

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
