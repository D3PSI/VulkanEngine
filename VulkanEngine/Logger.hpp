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
#include <conio.h>

enum LogNr {

	ERROR_LOG			= 0,
	START_STOP_LOG		= 1,
	EVENT_LOG			= 2

};

class Logger
{
public:
	Logger(std::string directory_ = "logs/");
	void log(LogNr logNr_, std::string text_);
	~Logger();
private:
	std::string directory;
	std::string eventLogStreamFileName;
	std::string errorLogStreamFileName;
	std::string startStopStreamFileName;
};

