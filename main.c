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

SDL_Color gridColor = {34, 34,34, 255};
SDL_Color white = { 255, 255, 255};
SDL_Color alive = { 200, 200, 200};

typedef struct Cell {
  int state;
  int blockX;
  int blockY;
  int liveNeighbors;
  SDL_Rect rect;
} Cell;

struct {
	int generation; // how many generation since starting.
	int edition_mode;
	int running;
} game_state = {0, 1, 0};

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
			arr[row][col].blockX = col;
			arr[row][col].blockY = row;
			arr[row][col].liveNeighbors = 0;
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

// % is not modulo but remainder.
int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

int countLiveNeighbors(Cell cell, Cell game[rows][cols]) {
	int left = mod(cell.blockX - 1, cols);
	int right = mod(cell.blockX + 1, cols);
	int down = mod(cell.blockY + 1, rows);
	int up = mod(cell.blockY - 1, rows);

	int upperLeft = game[up][left].state;
	int upper = game[up][cell.blockX].state;
	int upperRight = game[up][right].state;
	int lefter = game[cell.blockY][left].state;
	int righter = game[cell.blockY][right].state;
	int downerLeft = game[down][left].state;
	int downer = game[down][cell.blockX].state;
	int downerRight = game[down][right].state;
	
	return upperLeft + upper + upperRight 
			+ lefter + righter + downerLeft 
			+ downer + downerRight;
}

void countLiveNeighborsForAllCells(Cell game[rows][cols]) {
	for (int row = 0; row < rows; row++) {
		for (int col = 0;col < cols; col++) {
			int neighbors = countLiveNeighbors(game[row][col], game);
			game[row][col].liveNeighbors = neighbors;
		}
	} 
}

// Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
// Any live cell with two or three live neighbours lives on to the next generation.
// Any live cell with more than three live neighbours dies, as if by overpopulation.
// Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
void updateCellState(Cell game[rows][cols]) {
	for (int row = 0; row < rows; row++) {
		for (int col = 0;col < cols; col++) {
			if (game[row][col].state == 1) {
				if (game[row][col].liveNeighbors < 2) {
					game[row][col].state = 0;
				} else if (game[row][col].liveNeighbors > 3) {
					game[row][col].state = 0;
				}

			} else {
				if (game[row][col].liveNeighbors == 3) {
					game[row][col].state = 1;
				}
			}
		}
	} 
}

void renderCells(Cell game[rows][cols]) {
	for (int row = 0; row < rows; row++) {
		for (int col = 0;col < cols; col++) {
			if (game[row][col].state == 1) {
				SDL_RenderFillRect( renderer, &game[row][col].rect);
			}
		}
	}	
}

int ticks = 0;
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
			else if(event.type == SDL_MOUSEBUTTONDOWN) {
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
			
			// l: lock / unlock editing mode.
			// r: start the game if it's not running.
			// c: clear board and reset the game.
			else if(event.type == SDL_KEYDOWN) {

				switch (event.key.keysym.sym) {
				case SDLK_l:
					if (game_state.running == 0) {
						if (game_state.edition_mode == 0) {
							game_state.edition_mode = 1;
						} else {
							game_state.edition_mode = 0;
						}
					}
					break;
				case SDLK_r:
					if (game_state.running == 0) {
						game_state.running = 1;
					} else {
						game_state.running = 0;
        				printf("Game was already running. Pausing game.\n");
					}
					break;
				case SDLK_c:
					game_state.running = 0;
					game_state.generation = 0;
					init_game_array(game);
					break;
				default:
					break;
				}
			}

		}

		// computing chunk
		int blockX = mouseX / cell_size;
		int blockY= mouseY / cell_size;
		
		// updating textual informations om the top left corner.
		SDL_GetMouseState(&mouseX, &mouseY);
		char str[100];
		sprintf(str, "x: %d,y: %d, chunk (x-y): %d-%d, generation: %d, Running: %d",
		mouseX, mouseY, blockX, blockY, game_state.generation,game_state.running);
		renderText(font, str, textZone);

		// rendering the cells
		set_color(alive);
		renderCells(game);

		if (game_state.running == 1 && ticks > 100) {
			countLiveNeighborsForAllCells(game);
			updateCellState(game);
			game_state.generation++;
			ticks = 0;
		}

		// showing current block while hovevering 
		set_color(white);
		SDL_Rect rect = getRect(blockX, blockY);
		SDL_RenderFillRect( renderer, &rect );
		
		SDL_RenderPresent(renderer);

		SDL_Delay(10);
		ticks += 10;
	}

	SDL_DestroyTexture(fontTexture);
	TTF_CloseFont(font);
	TTF_Quit();

	SDL_DestroyTexture(grid);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit(); 
	return EXIT_SUCCESS;
}

