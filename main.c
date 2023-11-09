#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Window *window; 
SDL_Renderer* renderer;
SDL_Texture * grid;

const int cell_size = 15;
const int window_width = 1280 - 1280 % 25;
const int window_height = 720 - 720 % 25;

SDL_Color red = {255, 0, 0, 255};
SDL_Color alpha = {0, 0, 0, 0};

int init_window() {
	// init SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "Error SDL_Init : %s", SDL_GetError());
		return 0;
	}

	// creating window and renderer
	if(SDL_CreateWindowAndRenderer( window_width , window_height , SDL_WINDOW_RESIZABLE, &window, &renderer)<0)
	{
		printf("Error SDL_CreateWindowAndRenderer : %s",SDL_GetError());
		return 0;
	}
	
	if(SDL_CreateWindowAndRenderer( window_width , window_height , SDL_WINDOW_RESIZABLE, &window, &renderer)<0)
	{
		printf("Error SDL_CreateWindowAndRenderer : %s",SDL_GetError());
		return 0;
	}


	return 1;
}

void set_color(SDL_Color * color) {
	if (color == NULL) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	} else {
		SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
	}
}

int  init_grid_texture(SDL_Color * color) {
		grid = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET , window_width, window_height);
	
	if (grid == NULL) {
		printf("Error SDL_CreateTexture : %s",SDL_GetError());
		return 0;
	}
	
	SDL_SetRenderTarget( renderer, grid );
	set_color(color);
	for (int x = 0; x < window_width; x += cell_size) {
		SDL_RenderDrawLine(renderer, x, 0,  x,  window_height);
	}
	
	for (int y = 0; y < window_height; y += cell_size) {
		SDL_RenderDrawLine(renderer, 0,  y,  window_width,  y);
	}
	
	SDL_SetRenderTarget( renderer, NULL );
	
	return 1;
}

// TODO... 
// https://stackoverflow.com/questions/62379457/how-to-draw-a-grid-of-pixels-fast-with-sdl2
// Try the solution tomorrow.
// https://fr.wikibooks.org/wiki/Programmation_avec_la_SDL/Les_textures
// https://github.com/IsakBrendestam/GridTemplate/blob/main/main.c
// https://github.com/catsocks/sdl-grid
// https://www.youtube.com/watch?v=75yEVx8iXYQ < from suirface
// https://www.youtube.com/watch?v=75yEVx8iXYQ
// https://fr.wikibooks.org/wiki/Programmation_avec_la_SDL/Les_textures
// https://www.youtube.com/watch?v=XfZ6WrV5Z7Y&t=2700s
// fr https://www.youtube.com/watch?v=IF99v5_uHEY
// https://zestedesavoir.com/tutoriels/1014/utiliser-la-sdl-en-langage-c/modification-pixels-par-pixels/
// https://benedicthenshaw.com/soft_render_sdl2.html
int main(int argc, char *argv[])
{
	
	if (init_window() != 1) return EXIT_FAILURE;	
	if (init_grid_texture(&red) != 1) return EXIT_FAILURE;
	SDL_SetWindowTitle(window, "Gabey's game of life.");
	
	SDL_RenderCopy(renderer, grid, NULL, NULL);
	SDL_RenderPresent(renderer);

	SDL_Event event;
	while (1)
	{
		SDL_WaitEvent(&event);
		if (event.type == SDL_QUIT) {
			break;
		}
		SDL_Delay(10);
	}

	SDL_DestroyTexture(grid);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit(); 
	return EXIT_SUCCESS;
}

