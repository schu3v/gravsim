#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <SDL2/SDL.h>
#include "vp.h"
#include "io.h"
#include "body.h"
#include "bodyren.h"
#include "system.h"
#define SAVE_FILEPATH_SIZE 128

int main(int argc, char *argv[]){
	Flags default_flags = {
		0, 1, 1, 1, 1, 1, -1, -1, 60, 2.0, 10.0,
		SIZE_MAX, 0, 0, NULL, ".", "sys"
	};
	Flags *flags = get_flags_from_args(argc, argv, &default_flags);
	float scale = 1.0;
	float max_scale = 1024.0;
	Uint32 start = 0, elapsed = 0, estimated = 0;
	size_t new_fix_i = flags->fix_i;
	Vec2 click;
	Vec2 shift = {0, 0};
	Vec2 movement = {0, 0};
	View_Port vp;
	char save_filepath[SAVE_FILEPATH_SIZE];
	System *sys = NULL;
	SDL_Event event;

	if (!flags)
		goto cleanup;

	if (!flags->sys_filepath)
		goto cleanup;

	if (flags->isdraw){
		if (!vp_init(&vp, flags->width, flags->height, "gravsim"))
			goto cleanup;
	}
	
	shift.x = (float)vp.width / 2.0;
	shift.y = (float)vp.height / 2.0;
	estimated = 1000 / flags->fps;
	sys = read_system(flags->sys_filepath, flags->isuse_ren_info);

	if (!sys)
		goto cleanup;

	if (!sys->bodies || (!sys->bodies_ren && flags->isdraw))
		goto cleanup;

	for (;; sys->iter++){
		if (flags->save_mult_iter != 0 && sys->iter != 0 && 
			sys->iter % flags->save_mult_iter == 0){
			snprintf(
				save_filepath, SAVE_FILEPATH_SIZE, "%s/%s_%u.conf",
				flags->save_dir, flags->save_pref, sys->iter
			);
			write_system(sys, save_filepath);
		}

		if (flags->exit_iter != 0 && sys->iter == flags->exit_iter)
			goto cleanup;

		if (flags->isdraw){
			start = SDL_GetTicks();

			while (SDL_PollEvent(&event)){
				switch (event.type){
				case SDL_QUIT:
					goto cleanup;

				case SDL_WINDOWEVENT:
					SDL_GetWindowSize(vp.screen, &vp.width, &vp.height);
					shift.x = vp.width / 2.0 + movement.x;
					shift.y = vp.height / 2.0 + movement.y;
					break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym){
					case 32:
						flags->ispause ^= 1;
						break;

					case 'w':
						shift.y += flags->move_step;
						movement.y += flags->move_step;
						break;

					case 'a':
						shift.x += flags->move_step;
						movement.x += flags->move_step;
						break;

					case 's':
						shift.y -= flags->move_step;
						movement.y -= flags->move_step;
						break;

					case 'd':
						shift.x -= flags->move_step;
						movement.x -= flags->move_step;
						break;

					case 'l':
						snprintf(
							save_filepath, SAVE_FILEPATH_SIZE, "%s/%s_%u.conf",
							flags->save_dir, flags->save_pref, sys->iter
						);
						write_system(sys, save_filepath);
						break;

					case SDLK_UP:
						scale *= flags->scale_step;
						scale = (scale < max_scale) ? scale : max_scale;
						break;

					case SDLK_DOWN:
						scale /= flags->scale_step;
						break;
					}

					break;
				
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1){
						click.x = (float)event.button.x - shift.x;
						click.y = (float)event.button.y - shift.y;

						new_fix_i = get_index_chosen_body(sys->bodies, sys->bodies_ren, 
							click, scale, sys->len
						);

						if (new_fix_i != sys->len){
							flags->fix_i = new_fix_i;
							movement.x = 0;
							movement.y = 0;
						}
					}

					else if (event.button.button == 3){
						flags->fix_i = sys->len;
						movement.x = 0;
						movement.y = 0;
						shift.x = (float)vp.width / 2.0;
						shift.y = (float)vp.height / 2.0;
					}
				}
			}

			if (flags->fix_i < sys->len){
				shift.x = (float)vp.width / 2.0 - 
					scale * sys->bodies[flags->fix_i].x + movement.x;
				shift.y = (float)vp.height / 2.0 - 
					scale * sys->bodies[flags->fix_i].y + movement.y;
			}

			SDL_SetRenderDrawColor(vp.ren, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(vp.ren);
			SDL_GetRendererOutputSize(vp.ren, &vp.width, &vp.height);
			render_bodies(&vp, sys->bodies, sys->bodies_ren, shift, scale, sys->len);
			SDL_RenderPresent(vp.ren);

			if (flags->ispause){
				continue;
			}
		}

		if (flags->islog){
			if (flags->islogi || flags->islogb)
				clrscr();

			if (flags->islogi)
				printf("Iter: %u\n", sys->iter);

			if (flags->islogb)
				log_bodies(sys->bodies, sys->len);
		}	

		update_coords(sys->bodies, sys->len, sys->G);

		if (flags->isdraw){
			elapsed = SDL_GetTicks() - start;

			if (elapsed < estimated)
				SDL_Delay(estimated - elapsed);
		}
	}

cleanup:
	if (sys){
		free(sys->bodies_ren);
		free(sys->bodies);
		free(sys);
	}
	
	if (flags && flags->isdraw){
		vp_cleanup(&vp);
		free(flags);
	}

	return 0;
}