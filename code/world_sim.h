#ifndef WORLD_SIM
#define WORLD_SIM

// this is like way more than world simulation
// really should've put rendering into it's own thing
// same with the score stuff
// or just put it in main

#include <cmath>
#include <fstream>
#include <sstream>
#include <SDL.h>
#include <cstdlib> // for rand
#include <ctime>
#include "mikeys_algebra.h"
#include "sdl_util.h"

using namespace std;
using namespace mikeys_algebra;
using namespace sdl_util;

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 720;
const unsigned int TICKTIME = 15; // 15 = 66.6666 tps
const double DELTA = (double)TICKTIME/1000;
const double GRAVITY = 500;
const double THRUST = 1500;
const double FUEL_CONSUMPTION = 75;
const double DELTA_FUEL_USE = FUEL_CONSUMPTION * DELTA;

unsigned static int gSeed = time(NULL);

// I feel like I could have made things a lot easier if I bothered to learn 
// how extern works earlier
extern double gFuel;
extern unsigned int gScore, gHighScore;
bool gHighScoreChanged = false;

namespace world_sim {

	vec2d rand_pos() {
		// fix this
		vec2d rp;
		rp.x = (double)(rand() % (SCREEN_WIDTH - 128)) + 64;
		rp.y = (double)(rand() % (SCREEN_HEIGHT - 128)) + 64;
		return rp;
	}

	enum states {
		MAIN_MENU,
		GAME
	};

	enum actor_types {
		NONE,
		PLAYER,
		FUEL
	};

	// was originally gonna have more stuff
	// moving entities
	// but I had to scale back
	const unsigned int ACTOR_LIMIT = 2;

	class actor {
		private:
			static const unsigned int W = 48 , H = 48;
			static constexpr unsigned int halfW = W / 2, halfH = H / 2;
			unsigned int type = NONE;	
			SDL_Rect my_rect = { 0,0,W,H };
			texture_wrap my_texture;

		public:
			bool visible = true;
			vec2d position = {SCREEN_WIDTH/2,SCREEN_HEIGHT/2};
			vec2d prev_pos = position;
			vec2d velocity = { 0,0 };

			unsigned int my_type() {
				return type;
			}

			void become_player(SDL_Renderer* renderer) {
				// texture loading really shouldn't be done here but whatever
				// could have preloaded it in a big texture list
				// thought about that too late
				// something for next time
				printf("Creating player\n");
				type = PLAYER;
				my_texture.png_to_texture(renderer, "resources/vessel.png"); 
				position = { SCREEN_WIDTH / 2,SCREEN_HEIGHT / 2 };
				prev_pos = position;
				velocity = { 0,0 };
			}

			void become_fuel(SDL_Renderer* renderer) {
				printf("Creating fuel\n");
				type = FUEL;
				my_texture.png_to_texture(renderer, "resources/fuel_cell.png");
				position = rand_pos();
				prev_pos = position;
			}

			void dealloc() {
				type = NONE;
				my_texture.clear_tex();
			}

			void draw(SDL_Renderer* renderer, vec2d pos) {
				if (visible) {
					my_rect.x = round(pos.x) - halfW;
					my_rect.y = round(pos.y) - halfH;
					my_texture.rect_render(renderer, my_rect);
				}				
			}
	};

	class world_state {
		private:
		actor actors[ACTOR_LIMIT];

		void start(unsigned int &game_state, SDL_Renderer* renderer) {
			gHighScoreChanged = false;
			gScore = 0;
			gFuel = 100;
			game_state = GAME;
			printf("Game started!\n");
			actors[0].become_player(renderer);
			actors[1].become_fuel(renderer);
		}

		void end(unsigned int& game_state, SDL_Renderer *renderer) {

			game_state = MAIN_MENU;
			printf("Game ended!\n\nClearing actor memory\n");
			for (actor& a : actors) {
				if (a.my_type() == NONE) {
					printf("Nothing here\n");
				}
				else {
					printf("Actor here, deallocating\n");
					a.dealloc();
				}
			}

			ofstream hs;
			if (gHighScoreChanged) {
				hs.open("resources/high_score.txt", ofstream::out | ofstream::trunc);
				hs << gHighScore;
			}
			hs.close();

			printf("\n");
		}

		public:
			// using individual variables sucks but this
			// is a small enough game to not matter
			texture_wrap title;
			texture_wrap background;

			void update(const Uint8* key_states, unsigned int &game_state, SDL_Renderer *renderer, SDL_Window *window, bool &quit)
			{
				if (quit) {
					end(game_state, renderer);
					return;
				}
				
				if (game_state == MAIN_MENU) {

					if (key_states[SDL_SCANCODE_RETURN])
						start(game_state, renderer);
				}
				else
				{
					
					actors[0].prev_pos = actors[0].position;

					actors[0].velocity.y += GRAVITY * DELTA;
	
					vec2d wishdir = { 0,0 };

					if (key_states[SDL_SCANCODE_LEFT])
					{
						wishdir.x -= 1;
					}
					if (key_states[SDL_SCANCODE_RIGHT])
					{
						wishdir.x += 1;
					}
					if (key_states[SDL_SCANCODE_UP])
					{
						wishdir.y -= 1;
					}
					if (key_states[SDL_SCANCODE_DOWN])
					{
						wishdir.y += 1;
					}

					
					if (mikeys_algebra::hypot(wishdir.x, wishdir.y) != 0) {
						wishdir = normalize(wishdir);
						//printf("deltathrust(%f)\n", actors[0].thrust * DELTA);
					}
					else {
						// wishdir.y = -1;
					}
					
					// printf("wish(%f,%f)\n", wishdir.x, wishdir.y);

					// wasted so much time trying to figure out why left+up doesn't work
					// apparently this keyboard sucks and can't register those keys and space at the same time
					if (key_states[SDL_SCANCODE_Z] && gFuel > 0) {
						// only normalize when not zero
						// this is so the like 0.0001 fuel can't create too much thrust
						double usage = DELTA_FUEL_USE;
						if (usage > gFuel) {
							usage = -(gFuel - usage);
							gFuel = 0;
						}
						else {
							gFuel -= usage;
						}
						double dt_thrust = THRUST * (usage/DELTA_FUEL_USE) * DELTA;
						vec2d add_speed = { wishdir.x * dt_thrust, wishdir.y * dt_thrust };
						actors[0].velocity = add_vec(actors[0].velocity, add_speed);
					}
					
					/* testing teleporting */
					/*
					if (key_states[SDL_SCANCODE_RETURN])
					{
						actors[0].velocity = { 0,0 };
						actors[0].position = { SCREEN_WIDTH / 2,SCREEN_HEIGHT / 2 };
						actors[0].prev_pos = { SCREEN_WIDTH / 2,SCREEN_HEIGHT / 2 }; // to prevent interp
					}
					*/
					actors[0].position = add_vec(actors[0].position, scalar_mult(actors[0].velocity, DELTA));

					if (key_states[SDL_SCANCODE_ESCAPE]) // there was a semicolon here leaving the end out of scope whoops
						end(game_state, renderer);
					if (actors[0].position.y > SCREEN_HEIGHT + 96) {
						end(game_state, renderer);
					}

					for (unsigned int i = 1; i != ACTOR_LIMIT; i++) {
						if (actors[i].my_type() == FUEL) {
							vec2d &p_pos = actors[0].position;
							vec2d &my_pos = actors[i].position;
							vec2d dist_vec{ p_pos.x - my_pos.x,p_pos.y - my_pos.y };
							if (mikeys_algebra::hypot(dist_vec.x, dist_vec.y) < 48) {
								gScore += 50;
								my_pos = rand_pos();
								actors[i].prev_pos = my_pos;
								gFuel += 75;
								if (gFuel > 100) 
									gFuel = 100;
							}
						}
					}

					if (gScore > gHighScore) {
						gHighScore = gScore;
						gHighScoreChanged = true;
					}
				}
			}

			void render(unsigned int game_state, SDL_Renderer* renderer, double &lerp) {
				if (game_state == MAIN_MENU) {
					title.fill_render(renderer);
				}
				else 
				{
					background.fill_render(renderer);
					for (actor& a : actors) {
						if (a.my_type() != NONE) {
							vec2d travel = pts_to_vec(a.prev_pos, a.position);
							vec2d interp_reduction = scalar_mult(travel,lerp);
							a.draw(renderer, subtract_vec(a.position, interp_reduction));
							//a.draw(renderer, a.position);
						}
					}
				}
			}
	};
}

#endif
