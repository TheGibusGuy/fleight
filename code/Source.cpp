#include <SDL.h>
#include <SDL_ttf.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include "sdl_util.h"
#include "signature.h"
#include "world_sim.h"

using namespace std;
using namespace sdl_util;
using namespace world_sim;
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
const SDL_Color gColor{ 255,127,39 };

TTF_Font* gFont = NULL;

// game vars
unsigned int gHighScore, gScore = 0;
double gFuel = 0;

void render_text(string text) {

	SDL_Surface* temp_surf = TTF_RenderText_Solid_Wrapped(gFont, text.c_str(), gColor, SCREEN_WIDTH);
	SDL_Rect target_rect = { 0,0,temp_surf->w,temp_surf->h };
	SDL_Texture* temp_ture = SDL_CreateTextureFromSurface(gRenderer, temp_surf);
	SDL_FreeSurface(temp_surf);
	SDL_RenderCopy(gRenderer, temp_ture, NULL, &target_rect);
	SDL_DestroyTexture(temp_ture);
}

void game_loop() {
	printf("Game loop started!\n");

	SDL_Event e;

	bool quit = false;
	unsigned long long current_tick = SDL_GetTicks64();
	unsigned long long next_tick = current_tick;
	unsigned long step_no = 0;
	double lerp = 0;

	world_state w_state;
	// ah whatever it works
	w_state.title.png_to_texture(gRenderer, "resources/title.png");
	w_state.background.png_to_texture(gRenderer, "resources/background.png");

	unsigned int game_state = MAIN_MENU;

	while (not quit) {

		if ((current_tick >= next_tick)) {
			// man I never used this thing
			step_no++;

			while (SDL_PollEvent(&e))
				if (e.type == SDL_QUIT) quit = true;

			const Uint8* key_states = SDL_GetKeyboardState(NULL);

			while (current_tick >= next_tick) 
				next_tick += TICKTIME;
			
			w_state.update(key_states, game_state, gRenderer, gWindow, quit);
		}

		lerp = ((double)(next_tick - current_tick)) / TICKTIME;
	
		// code like this is why there's like half an hour vids
		// of dudes complaing about oop
		w_state.render(game_state,gRenderer,lerp);

		stringstream hud_text;
		hud_text << "High Score: " << gHighScore << "\n";

		if (game_state == GAME)
			hud_text << "Score: " << gScore << "\nFuel: " << gFuel << "\n";

		render_text(hud_text.str());

		SDL_RenderPresent(gRenderer);
			
		current_tick = SDL_GetTicks64();
	}
	w_state.title.clear_tex();
	w_state.background.clear_tex();
	printf("Game loop ended!\n");
}

int main(int argc, char* args[])
{
	srand(time(NULL));
	print_signature();
	printf("\nI could remove the command line if I wanted to,\n");
	printf("and I know how, but I think it's cool so I'm keeping it in.\n\n");

	bool success = true;

	if (!sys_init()) {
		printf("Sytems init failed!\n");
		success = false;
	}
	else {
		gWindow = SDL_CreateWindow("Fleight", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			printf("Window created.\n");
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL) {
				printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				printf("Renderer created.\n");
			}
		}
	}

	// should be an an init function, but I don't have remaining brainpower to do that,
	// along with the rest of the game
	gFont = TTF_OpenFont("resources/courbd.ttf", 28);
	printf("%s\n", SDL_GetError());

	string file_contents;
	ifstream hs("resources/high_score.txt");
	getline(hs, file_contents);
	gHighScore = stoi(file_contents);

	// Close the file
	hs.close();

	if (success) {
		printf("SDL is functional!\n");
		game_loop();
	}
	else {
		printf("SDL isn't working!");
	}

	// I have to make the sure the program reaches this point to deallocate resources
	// So "never nesting" early returns won't fly here.
	// deallocate resources
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Quit SDL subsystems
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();

	return 0;
}
