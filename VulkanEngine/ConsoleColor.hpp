#pragma once
#include "VERSION.cpp"
#include <iostream>
#include <Windows.h>

inline std::ostream& blue(std::ostream& s_) {

	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE
		| FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	return s_;

}

inline std::ostream& red(std::ostream& s_) {

	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
		FOREGROUND_RED | FOREGROUND_INTENSITY);
	return s_;

}

inline std::ostream& green(std::ostream& s_) {

	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
		FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	return s_;

}

inline std::ostream& yellow(std::ostream& s_) {

	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
		FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	return s_;

}

inline std::ostream& white(std::ostream& s_) {

	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return s_;

}

struct color {

	color(WORD attribute_) :m_color(attribute_) {};
	WORD m_color;

};

template <class _Elem, class _Traits>
std::basic_ostream<_Elem, _Traits>&
operator<<(std::basic_ostream<_Elem, _Traits>& i_, color& c_) {

	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, c_.m_color);
	return i_;

}
