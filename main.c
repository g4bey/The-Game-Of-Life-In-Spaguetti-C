#include <SDL2/SDL.h>
#include <stdio.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>

SDL_Window *window; 
SDL_Renderer* renderer;
SDL_Texture * grid;

const int cell_size = 15;
const int window_width = 1280 - 1280 % cell_size;
const int window_height = 720 - 720 % cell_size;
const int cols = window_width / cell_size;
const int rows = window_height / cell_size;

SDL_Color red = {255, 0, 0, 255};
SDL_Color gridColor = {34, 34,34, 255};
SDL_Color white = { 255, 255, 255};
SDL_Color alive = { 200, 200, 200};
SDL_Color alpha = {0, 0, 0, 0};

typedef struct Cell {
  int state;
  SDL_Rect rect;
  // ... colors in the future ? :3
} Cell;

SDL_Rect getRect(int blockX, int blockY) {
	SDL_Rect rect;
	rect.x = blockX*cell_size;
	rect.y = blockY*cell_size;
	rect.w = cell_size;
	rect.h = cell_size;
	return rect;
}

void init_game_array(Cell arr[rows][cols]) {
	for (int row = 0; row < rows; row++) {
		for (int col = 0;col < cols; col++) {
			arr[row][col].rect = getRect(col, row);
			arr[row][col].state = 0;
		}
	}
}

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

struct {
	int generation; // how many generation since starting.
	int edition_mode;
	int running;
} game_state = {0, 1, 0};

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
	// disable buffering for stdout
	setvbuf (stdout, NULL, _IONBF, 0);

	Cell game[rows][cols];
	init_game_array(game);

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

			// toggle <=> untoggle blocks.
			if(event.type == SDL_MOUSEBUTTONDOWN) {
				if (game_state.edition_mode == 1) {
					int col = mouseX / cell_size;
					int row= mouseY / cell_size;
					if (game[row][col].state == 0) {
						game[row][col].state = 1;
					} else {
						game[row][col].state = 0;
					}
				} else {
					printf("Edition mode is disabled.\n");
				}
			}
			
			if(event.type == SDL_KEYDOWN) {
				if(event.key.keysym.sym==SDLK_l) {
					if (game_state.running == 0) {
						if (game_state.edition_mode == 0) {
							game_state.edition_mode = 1;
						} else {
							game_state.edition_mode = 0;
						}
					} else {
        				printf("Game is already running.\n");
					}
				}
			}

		}
		// FIX SEG FAULT LOL
		SDL_GetMouseState(&mouseX, &mouseY);
		char str[100];
		int blockX = mouseX / cell_size;
		int blockY= mouseY / cell_size;
		sprintf(str, "x: %d,y: %d, chunk (x-y): %d-%d", mouseX, mouseY, blockX, blockY);

		for (int row = 0; row < rows; row++) {
			for (int col = 0;col < cols; col++) {
				if (game[row][col].state == 1) {
					set_color(alive);
					SDL_RenderFillRect( renderer, &game[row][col].rect );
				}
			}
		}	

		// showing current block while hovevering 
		set_color(white);
		SDL_Rect cell = getRect(blockX, blockY);
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

