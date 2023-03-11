#ifndef SDL_UTIL
#define SDL_UTIL
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>

namespace sdl_util {
	bool sys_init() {
		bool success = true;

		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			printf("Video initialization failed: %s", SDL_GetError());
			success = false;
		}

		int imgFlags = IMG_INIT_PNG;	// each enumerator for img_init is 1 bit
										// so imagine you have system a (1000)
										// and system b (0100)
										// you would binary or them together into imgFlags (1100)
										// and pass that to the function
										// the function returns an number with the sucessful bits
										// this code uses binary and to check
										// if the png bit is flipped
										// a nonzero answer will evauluate to true
										// meaning that there was a scucess

		if (!(IMG_Init(imgFlags) & imgFlags)) {
			printf("Png loading failed: %s\n", SDL_GetError());
			success = false;
		}

		if (TTF_Init() == -1)
		{
			printf("TTF loading failed: %s\n", TTF_GetError());
			success = false;
		}

		if (success) {
			printf("Systems initialized.\n");
		}
		else {
			printf("Systems have failed to initialize.\n");
		}

		return success;
	}

	class texture_wrap {
		private:
			SDL_Texture* real_texture = NULL;
			unsigned int width = 0, height = 0;
		public:
			bool png_to_texture(SDL_Renderer* target_renderer, std::string path) {

				SDL_Surface* temp_surf = IMG_Load(path.c_str());
				if (temp_surf == NULL)
					return false;
				width = temp_surf->w;
				height = temp_surf->h;
				SDL_SetColorKey(temp_surf, SDL_TRUE, SDL_MapRGB(temp_surf->format, 0xFF, 0xFF, 0xFF));
				SDL_Texture* temp_texture = SDL_CreateTextureFromSurface(target_renderer, temp_surf);
				if (temp_texture == NULL)
					return false;
				SDL_FreeSurface(temp_surf);
				real_texture = temp_texture;
				return true;
			}

			void fill_render(SDL_Renderer* target_renderer) {
				SDL_RenderCopy(target_renderer, real_texture, NULL, NULL);
			}

			void rect_render(SDL_Renderer* target_renderer, const SDL_Rect& target_rect) {
				SDL_RenderCopy(target_renderer, real_texture, NULL, &target_rect);
			}

			void clear_tex() {
				if (real_texture != NULL) {
					SDL_DestroyTexture(real_texture);
					real_texture = NULL;
				}
			}
	};


}
#endif