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
	extern float										loadingProgress;

}

class StartWindow
{
public:
	bool					closeVar;
	void loop();
	StartWindow();
private:
	SDL_Window*				window;
	SDL_Surface*			imageSurface;
	SDL_Texture*			background;
	SDL_Renderer*			renderer;

	void destroy();
};

