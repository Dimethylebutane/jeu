#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "SpinLock.hpp"

/// <summary>
/// Thread safe logger with simple context
/// </summary>
class LoggerStream
{
private:
	Spinlock_MT lock;
	std::ofstream fileStream;
	std::ostream& logStream;

public:
	LoggerStream(const std::string& filename);
	LoggerStream(const char* filename);
	~LoggerStream()
	{
		close();
	}

	LoggerStream& operator=(const LoggerStream&) = delete;
	LoggerStream& operator=(LoggerStream&&) = delete;

	void change(const std::string& filename);

	template <typename T>
	void log(T message)
	{
		const SpinLockMTGuard locker(lock);

		logStream << "log :   " << message << "\n";
	}

	template <>
	void log(std::stringstream& message)
	{
		const SpinLockMTGuard locker(lock);

		logStream << "log :   " << message.str() << "\n";

		logStream.flush();
	}

	void close();
};

struct LoggerBuffer
{
	std::stringstream ss;
	LoggerStream& loggerStream;

	//constructor
	LoggerBuffer(LoggerStream& LoggerStream) : loggerStream(LoggerStream) {}

	//no weird constructor
	LoggerBuffer(const LoggerBuffer&) = delete;

	//nor weird buffer duplication
	LoggerBuffer& operator=(const LoggerBuffer&) = delete;
	LoggerBuffer& operator=(LoggerBuffer&&) = delete;

	//l/rvalue stuff ?for pipe?
	LoggerBuffer(LoggerBuffer&& buf) noexcept : ss(std::move(buf.ss)), loggerStream(buf.loggerStream) {}

	template <typename T>
	LoggerBuffer& operator<<(T&& message)
	{
		ss << std::forward<T>(message);
		return *this;
	}

	~LoggerBuffer(){
		//in logger buffer operator << -> a loggerBuffer is created to be returned but is deleted after being returned -> empty line appear
		//check if string stream is empty
		//if(ss.tellp() != std::streampos(0))
		loggerStream.log<std::stringstream&>(ss);
	}
};

template <typename T>
LoggerBuffer operator<<(LoggerStream& simpleLogger, T&& message)
{
	LoggerBuffer buf(simpleLogger);
	buf.ss << std::forward<T>(message);
	return buf;
}

extern LoggerStream defaultLogger;