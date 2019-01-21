/*
*	File:		StartWindow.hpp
*	
*
*/
#pragma once
#include <SDL.h>
#undef main
#include <iostream>
#include <mutex>

namespace game {

	extern std::mutex									closeStartWindow;
	extern const std::string							TITLE;

}

class StartWindow
{
public:
	bool					closeVar;
	void loop();
	StartWindow();
private:
	SDL_Window*				window;
	SDL_Surface*			windowSurface;
	SDL_Surface*			imageSurface;
	void destroy();
};

