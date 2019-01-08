/*
*	File:			Logger.cpp
*
*
*/
#include <Windows.h>
#include "Logger.hpp"
#include "ConsoleColor.hpp"

/*
*	Function:		Logger()
*	Purpose:		Default constructor
*
*/
Logger::Logger(std::string directory_) {

	directory					= directory_;
	eventLogStreamFileName		= "event.log";
	errorLogStreamFileName		= "error.log";
	startStopStreamFileName		= "startup.log";

}

/*
*	Function:		void log(LogNr logNr_, std::string text_)
*	Purpose:		Writes to selected Log-File
*
*/
void Logger::log(LogNr logNr_, std::string text_) {

	static int countEvent = 0;
	static int countError = 0;
	std::ofstream stream;

	time_t current_time;
	struct tm local_time;

	time(&current_time);
	localtime_s(&local_time, &current_time);

	int Year		= local_time.tm_year + 1900;
	int Month		= local_time.tm_mon + 1;
	int Day			= local_time.tm_mday;

	int Hour		= local_time.tm_hour;
	int Min			= local_time.tm_min;
	int Sec			= local_time.tm_sec;

	switch (logNr_) {
	case ERROR_LOG:
		if (countError == 0) {

			stream.open(directory + errorLogStreamFileName, std::ios::trunc);
			countError++;

			stream << green << Day << white << ":"
				<< green << Month << white << ":"
				<< green << Year << white << "   "
				<< green << Hour << white << ":"
				<< green << Min << white << ":"
				<< green << Sec << white << "		===		"
				<< red << "CRITICAL: "
				<< blue << text_ << white << std::endl;

			stream.close();

			std::cerr << green << Day << white << ":"
				<< green << Month << white << ":"
				<< green << Year << white << "   "
				<< green << Hour << white << ":"
				<< green << Min << white << ":"
				<< green << Sec << white << "		===		"
				<< red << "CRITICAL: "
				<< blue << text_ << white << std::endl;


		}
		else {

			stream.open(directory + errorLogStreamFileName, std::ios::app);

			stream<< green << Day << white << ":"
				<< green << Month << white << ":"
				<< green << Year << white << "   "
				<< green << Hour << white << ":"
				<< green << Min << white << ":"
				<< green << Sec << white << "		===		"
				<< red << "CRITICAL: "
				<< blue << text_ << white << std::endl;

			std::cerr << green << Day << white << ":"
				<< green << Month << white << ":"
				<< green << Year << white << "   "
				<< green << Hour << white << ":"
				<< green << Min << white << ":"
				<< green << Sec << white << "		===		"
				<< red << "CRITICAL: "
				<< blue << text_ << white << std::endl;

			stream.close();

		}
	case START_STOP_LOG:
		stream.open(directory + startStopStreamFileName, std::ios::app);

		stream << green << Day << white << ":"
			<< green << Month << white << ":"
			<< green << Year << white << "   "
			<< green << Hour << white << ":"
			<< green << Min << white << ":"
			<< green << Sec << white << "		===		"
			<< blue << text_ << white << std::endl;

		stream.close();
	case EVENT_LOG:
		if (countEvent == 0) {

			stream.open(directory + eventLogStreamFileName, std::ios::trunc);
			countEvent++;

			stream << green << Day << white << ":"
				<< green << Month << white << ":"
				<< green << Year << white << "   "
				<< green << Hour << white << ":"
				<< green << Min << white << ":"
				<< green << Sec << white << "		===		"
				<< blue << text_ << white << std::endl;
			
			stream.close();

			std::cout << green << Day << white << ":"
				<< green << Month << white << ":"
				<< green << Year << white << "   "
				<< green << Hour << white << ":"
				<< green << Min << white << ":"
				<< green << Sec << white << "		===		"
				<< blue << text_ << white << std::endl;

		}
		else {

			stream.open(directory + eventLogStreamFileName, std::ios::app);

			stream << green << Day << white << ":"
				<< green << Month << white << ":"
				<< green << Year << white << "   "
				<< green << Hour << white << ":"
				<< green << Min << white << ":"
				<< green << Sec << white << "		===		"
				<< blue << text_ << white << std::endl;

			stream.close();

			std::cout << green << Day << white << ":"
				<< green << Month << white << ":"
				<< green << Year << white << "   "
				<< green << Hour << white << ":"
				<< green << Min << white << ":"
				<< green << Sec << white << "		===		"
				<< blue << text_ << white << std::endl;

		}
		break;
	default:
		break;

	}

	if (logNr_ == ERROR_LOG) {
#ifdef GAME_DEVELOPMENT
		showConsoleWindow();
		std::cout << "\n\nPress any key to exit...";
		_getch();
#endif

		throw std::runtime_error(text_);
	
	}

}

/*
*	Function:		void showConsoleWindow()
*	Purpose:		Shows the console window
*
*/
void Logger::showConsoleWindow(void) {

	::ShowWindow(::GetConsoleWindow(), SW_SHOW);

}

/*
*	Function:		void hideConsoleWindow();
*	Purpose:		Hides the console window
*
*/
void Logger::hideConsoleWindow(void) {

	::ShowWindow(::GetConsoleWindow(), SW_HIDE);

}

/*
*	Function:		bool isConsoleWindowVisible()
*	Purpose:		Checks whether the console window is visible
*
*/
bool Logger::isConsoleWindowVisible(void) {

	return (::IsWindowVisible(::GetConsoleWindow()) != false);

}

/*
*	Function:		~Logger()
*	Purpose:		Default destructor
*
*/
Logger::~Logger() {



}
