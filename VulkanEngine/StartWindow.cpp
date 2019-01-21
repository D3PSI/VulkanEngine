#include "StartWindow.hpp"

/*
*	Function:		StartWindow()
*	Purpose:		Default constructor
*
*/
StartWindow::StartWindow() {

	closeVar = false;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {

		std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;

	}

	window = SDL_CreateWindow(

		game::TITLE.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		600,
		600,
		SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS

	);
	windowSurface = SDL_GetWindowSurface(window);

	if (window == nullptr) {

		std::cout << "Could not create window: " << SDL_GetError() << std::endl;

	}

	imageSurface = SDL_LoadBMP("res/textures/startwindow/infinity.bmp");
	if (imageSurface == NULL) {

		std::cout << "SDL could not load image! SDL Error: " << SDL_GetError() << std::endl;

	}

}

/*
*	Function:		~StartWindow()
*	Purpose:		Handles destruction of window
*
*/
void StartWindow::destroy() {

	SDL_FreeSurface(imageSurface);
	SDL_FreeSurface(windowSurface);

	imageSurface = nullptr;
	windowSurface = nullptr;

	SDL_DestroyWindow(window);
	SDL_Quit();

}

/*
*	Function:		void loop()
*	Purpose:		Contains loop for startup window
*
*/
void StartWindow::loop() {

	SDL_Event e;

	while (!closeVar) {

		while (SDL_PollEvent(&e)) {

			if (e.type == SDL_QUIT) {

				game::closeStartWindow.lock();
				closeVar = true;
				game::closeStartWindow.unlock();

			}

		}

		SDL_BlitSurface(
			
			imageSurface,
			NULL,
			windowSurface, 
			NULL
		
		);

		SDL_UpdateWindowSurface(window);
	
	}

	destroy();

}
