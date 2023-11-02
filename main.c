#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	SDL_Window *window; 
	SDL_Renderer* renderer;
	
	// init SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "Error SDL_Init : %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	// creating window and renderer
	if(SDL_CreateWindowAndRenderer( 600 , 600 , SDL_WINDOW_RESIZABLE,&window,&renderer)<0)
	{
		printf("Error SDL_CreateWindowAndRenderer : %s",SDL_GetError());
		return EXIT_FAILURE;
	}
	
	SDL_Delay(3000); 
	SDL_DestroyWindow(window);
	SDL_Quit();   
	return EXIT_SUCCESS;
}

