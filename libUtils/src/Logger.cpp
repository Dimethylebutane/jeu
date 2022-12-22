#include "../include/Logger.hpp"
#include <iostream>

LoggerStream defaultLogger("");


LoggerStream::LoggerStream(const char* filename) : logStream(fileStream)
{
	//wait for other thread to end their work
	const SpinLockMTGuard locker(lock);

	if (! (*filename == 0)) //null terminated string
		fileStream = std::ofstream(filename);

	//set output buffer as std::cout or filestream
	logStream.rdbuf(fileStream.is_open() ? fileStream.rdbuf() : std::cout.rdbuf());
}

LoggerStream::LoggerStream(const std::string& filename) : logStream(fileStream)
{
	//wait for other thread to end their work
	const SpinLockMTGuard locker(lock);

	if (!filename.empty())
		fileStream = std::ofstream(filename);

	//set output buffer as std::cout or filestream
	logStream.rdbuf(fileStream.is_open() ? fileStream.rdbuf() : std::cout.rdbuf());
}


void LoggerStream::change(const std::string& filename)
{
	//wait for other thread to end their work
	const SpinLockMTGuard locker(lock);

	*this << "Chaging stream direction for: " << filename;

	close();

	if (!filename.empty())
		fileStream = std::ofstream(filename);

	logStream.rdbuf(filename.empty() ? std::cout.rdbuf() : fileStream.rdbuf());
}

void LoggerStream::close()
{
	if (fileStream.is_open())
		fileStream.close();
}