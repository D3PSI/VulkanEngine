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

typedef enum LogNr {

	ERROR_LOG = 0,
	START_STOP_LOG = 1,
	EVENT_LOG = 2

} LogNr;

class Logger
{
public:
	Logger(std::string directory = "logs/");
	void log(LogNr logNr, std::string text);
	~Logger();
private:
	std::string directory;
	std::string eventLogStreamFileName;
	std::string errorLogStreamFileName;
	std::string startStopStreamFileName;
};

