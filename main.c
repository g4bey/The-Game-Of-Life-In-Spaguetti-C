#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Window *window; 
SDL_Renderer* renderer;

int init() {
	// init SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "Error SDL_Init : %s", SDL_GetError());
		return 0;
	}

	// creating window and renderer
	if(SDL_CreateWindowAndRenderer( 600 , 600 , SDL_WINDOW_RESIZABLE, &window, &renderer)<0)
	{
		printf("Error SDL_CreateWindowAndRenderer : %s",SDL_GetError());
		return 0;
	}

	return 1;
}

void quit() {
	SDL_Delay(3000); 
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit(); 
}

void set_color(SDL_Renderer *renderer, const SDL_Color color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

int main(int argc, char *argv[])
{
	if (init() != 1) return EXIT_FAILURE;

	const SDL_Color cell_color = {255, 255, 255, 255};
	const SDL_Color grid_color = {44, 44, 44, 255};

	SDL_SetWindowTitle(window, "Gabey's game of life.");

	set_color(renderer, cell_color);
	SDL_RenderDrawLine(renderer, 4,100,30,100);
	SDL_RenderPresent(renderer);  

	quit();
	return EXIT_SUCCESS;
}

