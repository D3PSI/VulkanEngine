/*
*	File:			Logger.cpp
*
*
*/
#include "Logger.hpp"

/*
*	Function:		Logger()
*	Purpose:		Default constructor
*
*/
Logger::Logger(std::string directory_) {

	directory = directory_;
	eventLogStreamFileName		= "event.log";
	errorLogStreamFileName		= "error.log";
	startStopStreamFileName		= "startup.log";

}

/*
*	Function:		void log(LogNr logNr, std::string text)
*	Purpose:		Writes to selected Log-File
*
*/
void Logger::log(LogNr logNr, std::string text) {

	static int countEvent = 0;
	static int countError = 0;
	std::ofstream stream;

	time_t current_time;
	struct tm local_time;

	time(&current_time);
	localtime_s(&local_time, &current_time);

	int Year = local_time.tm_year + 1900;
	int Month = local_time.tm_mon + 1;
	int Day = local_time.tm_mday;

	int Hour = local_time.tm_hour;
	int Min = local_time.tm_min;
	int Sec = local_time.tm_sec;

	switch (logNr) {
	case ERROR_LOG:
		if (countError == 0) {

			stream.open(directory + errorLogStreamFileName, std::ios::trunc);
			countError++;

			stream << Day << ":"
				<< Month << ":"
				<< Year << "   "
				<< Hour << ":"
				<< Min << ":"
				<< Sec << "		===		"
				<< text << std::endl;

			stream.close();

			std::cerr << Day << ":"
				<< Month << ":"
				<< Year << "   "
				<< Hour << ":"
				<< Min << ":"
				<< Sec << "		===		"
				<< text << std::endl;


		}
		else {

			stream.open(directory + errorLogStreamFileName, std::ios::app);

			stream << Day << ":"
				<< Month << ":"
				<< Year << "   "
				<< Hour << ":"
				<< Min << ":"
				<< Sec << "		===		"
				<< text << std::endl;

			std::cerr << Day << ":"
				<< Month << ":"
				<< Year << "   "
				<< Hour << ":"
				<< Min << ":"
				<< Sec << "		===		"
				<< text << std::endl;

			stream.close();

		}
	case START_STOP_LOG:
		stream.open(directory + startStopStreamFileName, std::ios::app);

		stream << Day << ":"
			<< Month << ":"
			<< Year << "   "
			<< Hour << ":"
			<< Min << ":"
			<< Sec << "		===		"
			<< text << std::endl;

		stream.close();
	case EVENT_LOG:
		if (countEvent == 0) {

			stream.open(directory + eventLogStreamFileName, std::ios::trunc);
			countEvent++;

			stream << Day << ":"
				<< Month << ":"
				<< Year << "   "
				<< Hour << ":"
				<< Min << ":"
				<< Sec << "		===		"
				<< text << std::endl;
			
			stream.close();

			std::cout << Day << ":"
				<< Month << ":"
				<< Year << "   "
				<< Hour << ":"
				<< Min << ":"
				<< Sec << "		===		"
				<< text << std::endl;

		}
		else {

			stream.open(directory + eventLogStreamFileName, std::ios::app);

			stream << Day << ":"
				<< Month << ":"
				<< Year << "   "
				<< Hour << ":"
				<< Min << ":"
				<< Sec << "		===		"
				<< text << std::endl;

			stream.close();

			std::cout << Day << ":"
				<< Month << ":"
				<< Year << "   "
				<< Hour << ":"
				<< Min << ":"
				<< Sec << "		===		"
				<< text << std::endl;

		}
		break;
	default:
		break;

	}

	if (logNr == ERROR_LOG) {
	
		throw std::runtime_error(text);
	
	}

}

/*
*	Function:		~Logger()
*	Purpose:		Default destructor
*
*/
Logger::~Logger() {



}
