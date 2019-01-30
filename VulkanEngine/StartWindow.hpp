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

