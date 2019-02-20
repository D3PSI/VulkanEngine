/*
*	File:			VERSION.cpp
*	Purpose:		Set options for game at compile time (comment out = default settings)
*
*/
#pragma once
//#define GAME_RELEASE						// for releasing software
//#define GAME_DEVELOPMENT					// for development in IDE's or testing on different systems (debugging)

//#define GAME_WINDOW_MODE_WINDOWED			// display the game in windowed mode
//#define GAME_WINDOW_MODE_BORDERLESS		// display the game in borderless window mode
//#define GAME_WINDOW_MODE_FULLSCREEN		// display the game in fullscreen mode

//#define GAME_USE_FRAMERATE_CAP_60				// use a framerate cap
//#define GAME_NO_FRAMERATE_CAP					// dont use a framerate cap to prevent screen tearing in borderless window and fullscreen mode

#define GAME_USE_TINY_OBJ					// sets the importer library to be tiny_obj_loader instead of ASSIMP