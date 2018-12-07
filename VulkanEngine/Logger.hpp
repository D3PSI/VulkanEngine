/*
*	File:			Logger.hpp
*	
*
*/
#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>

#define ERROR_LOG 0
#define START_STOP_LOG 1
#define EVENT_LOG 2

class Logger
{
public:
	Logger(std::string directory = "logs/");
	void log(int logNr, std::string text);
	~Logger();
private:
	std::string directory;
	std::string eventLogStreamFileName;
	std::string errorLogStreamFileName;
	std::string startStopStreamFileName;
};

