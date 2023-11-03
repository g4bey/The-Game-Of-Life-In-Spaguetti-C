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
	if(SDL_CreateWindowAndRenderer( 600 , 600 , SDL_WINDOW_RESIZABLE | SDL_RENDERER_ACCELERATED, &window, &renderer)<0)
	{
		printf("Error SDL_CreateWindowAndRenderer : %s",SDL_GetError());
		return 0;
	}

	return 1;
}

void quit() {
	SDL_Delay(16000); 
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit(); 
}

void set_color(const SDL_Color color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

void draw_grid(int cell_size, int window_width, int window_height, SDL_Color color) {
	set_color(color);
	for (int y = 0; y < window_width; y += cell_size) {
            SDL_RenderDrawLine(renderer, 0, y, window_width, y);
			SDL_RenderDrawLine(renderer, y, 0, y, window_height);
    }
	SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    // + 1 so that the last grid lines fit in the screen.
    int window_width = 1280;
    int window_height = 720;
	int cell_size = 20;

	if (init() != 1) return EXIT_FAILURE;

	const SDL_Color cell_color = {255, 255, 255, 255};
	const SDL_Color grid_color = {44, 44, 44, 255};

	SDL_SetWindowTitle(window, "Gabey's game of life.");

	set_color(cell_color);
	SDL_RenderDrawLine(renderer, 4,100,30,100);
	SDL_RenderPresent(renderer);  

	draw_grid(cell_size, window_width, window_height, grid_color);

	SDL_RenderPresent(renderer);  

	quit();
	return EXIT_SUCCESS;
}

