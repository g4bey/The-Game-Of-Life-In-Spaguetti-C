#include <SDL2/SDL.h>
#include <stdio.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>

SDL_Window *window; 
SDL_Renderer* renderer;
SDL_Texture * grid;

const int cell_size = 15;
const int window_width = 1280 - 1280 % 25;
const int window_height = 720 - 720 % 25;

SDL_Color red = {255, 0, 0, 255};
SDL_Color gridColor = {34, 34,34, 255};
SDL_Color white = { 255, 255, 255 };
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

	return 1;
}

void set_color(SDL_Color color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

int  init_grid_texture(SDL_Color color) {
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

void renderText(TTF_Font * font, char * str, SDL_Rect dest) {
	SDL_Color fg = { 255, 255, 255 };
	SDL_Surface* surf = TTF_RenderText_Solid(font, str, fg);

	dest.w = surf->w;
	dest.h = surf->h;

	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);

	SDL_RenderCopy(renderer, tex, NULL, &dest);
	SDL_DestroyTexture(tex);
	SDL_FreeSurface(surf);
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
	if (init_grid_texture(gridColor) != 1) return EXIT_FAILURE;
	SDL_SetWindowTitle(window, "Gabey's game of life.");
	
	TTF_Init();
	TTF_Font * font = TTF_OpenFont("arial.ttf", 25);
	int mouseX, mouseY;

	SDL_Surface * fontSurface;
	SDL_Texture * fontTexture;
	
	int texW = 0;
	int texH = 0;
	SDL_Rect textZone = { 0, 0, texW, texH };
	SDL_QueryTexture(fontTexture, NULL, NULL, &texW, &texH);

	int continueRunning = 1;
	while(continueRunning){
		SDL_Event event;

		set_color(white);
		SDL_RenderClear( renderer );
		SDL_RenderCopy(renderer, grid, NULL, NULL);

		while (SDL_PollEvent( &event ))
		{
			if (event.type == SDL_QUIT) {
				continueRunning = 0;
				break;
			}
		}

		SDL_GetMouseState(&mouseX, &mouseY);
		char str[100];
		int blockX = mouseX / cell_size;
		int blockY= mouseY / cell_size;
		sprintf(str, "x: %d,y: %d, chunk (x-y): %d-%d", mouseX, mouseY, blockX, blockY);

		set_color(white);
		 SDL_Rect cell;
		cell.x = blockX*cell_size;
		cell.y = blockY*cell_size;
		cell.w = cell_size;
		cell.h = cell_size;
		SDL_RenderFillRect( renderer, &cell );

		renderText(font, str, textZone);
		fontSurface = TTF_RenderText_Solid(font,str, white);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(fontTexture);
	SDL_FreeSurface(fontSurface);
	TTF_CloseFont(font);
	TTF_Quit();

	SDL_DestroyTexture(grid);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit(); 
	return EXIT_SUCCESS;
}

