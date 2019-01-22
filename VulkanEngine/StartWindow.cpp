#include "StartWindow.hpp"

/*
*	Function:		StartWindow()
*	Purpose:		Default constructor
*
*/
StartWindow::StartWindow() {

	game::closeStartWindow.lock();
	closeVar = false;
	game::closeStartWindow.unlock();

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

	renderer = SDL_CreateRenderer(
		
		window,
		-1,
		SDL_RENDERER_ACCELERATED
	
	);

	if (window == nullptr) {

		std::cout << "Could not create window: " << SDL_GetError() << std::endl;

	}

	imageSurface = SDL_LoadBMP("res/textures/startwindow/infinity.bmp");
	if (imageSurface == NULL) {

		std::cout << "SDL could not load image! SDL Error: " << SDL_GetError() << std::endl;

	}

	background = SDL_CreateTextureFromSurface(renderer, imageSurface);

	SDL_SetWindowIcon(window, imageSurface);

}

/*
*	Function:		~StartWindow()
*	Purpose:		Handles destruction of window
*
*/
void StartWindow::destroy() {

	SDL_FreeSurface(imageSurface);
	SDL_DestroyTexture(background);
	SDL_DestroyRenderer(renderer);

	imageSurface = nullptr;

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
	SDL_Rect rect;
	rect.x = 100;
	rect.y = 500;
	rect.w = 400;
	rect.h = 20;

	while (!closeVar) {

		while (SDL_PollEvent(&e)) {

			if (e.type == SDL_QUIT) {

				game::closeStartWindow.lock();
				closeVar = true;
				game::closeStartWindow.unlock();

			}

		}

		SDL_Rect rectProgress;
		rectProgress.x = 100;
		rectProgress.y = 500;
		rectProgress.w = static_cast< int >(game::loadingProgress * 400);
		rectProgress.h = 20;

		SDL_RenderCopy(
			
			renderer, 
			background,
			NULL,
			NULL
		
		); 
		SDL_SetRenderDrawColor(

			renderer,
			255,
			255,
			255,
			SDL_ALPHA_OPAQUE

		);
		SDL_RenderFillRect(renderer, &rect);
		SDL_SetRenderDrawColor(
		
			renderer,
			150,
			82,
			104,
			SDL_ALPHA_OPAQUE

		);
		SDL_RenderDrawRect(renderer, &rect);
		SDL_RenderFillRect(renderer, &rectProgress);
		SDL_RenderPresent(renderer);
	
	}

	destroy();

}
