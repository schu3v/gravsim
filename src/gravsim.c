#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <SDL2/SDL.h>
#include "vp.h"
#include "args.h"
#include "body.h"
#include "bodyren.h"
#include "system.h"
#define SAVE_FILEPATH_SIZE 256
#define MAX_SCALE 4096.0
#define MIN_SCALE (1.0 / 4096.0)
#define DEFAULT_RADIUS 3.0
#define DEFAULT_R 0xFF
#define DEFAULT_G 0xFF
#define DEFAULT_B 0xFF

int main(int argc, char *argv[]){
	Args default_args = {
		0, 1, 1, 1, 1, -1, -1, 60, 2.0, 10.0,
		SIZE_MAX, 0, 0, NULL, "sys_"
	};
	Args *args = get_args(argc, argv, &default_args);
	double scale = 1.0;
	Uint32 start = 0, elapsed = 0, estimated = 0;
	size_t new_fix_i = args->fix_i;
	Vec2 click;
	Vec2 shift = {0, 0};
	Vec2 movement = {0, 0};
	View_Port vp;
	char save_filepath[SAVE_FILEPATH_SIZE];
	System *sys = NULL;
	SDL_Event event;

	if (!args)
		goto cleanup;

	if (!args->sys_filepath)
		goto cleanup;

	if (args->isdraw){
		if (!vp_init(&vp, args->width, args->height, "gravsim"))
			goto cleanup;
	}
	
	shift.x = (double)vp.width / 2.0;
	shift.y = (double)vp.height / 2.0;
	estimated = 1000 / args->fps;
	sys = read_system(args->sys_filepath, args->isuse_ren_info);

	if (!sys)
		goto cleanup;

	if (!sys->bodies || (!sys->bodies_ren && args->isuse_ren_info))
		goto cleanup;

	if (args->isdraw && !args->isuse_ren_info){
		sys->bodies_ren = malloc(sys->len * sizeof(Body_Ren));

		if (!sys->bodies_ren)
			goto cleanup;

		for (size_t i = 0; i < sys->len; i++){
			sys->bodies_ren[i].radius = DEFAULT_RADIUS;
			sys->bodies_ren[i].r = DEFAULT_R;
			sys->bodies_ren[i].g = DEFAULT_G;
			sys->bodies_ren[i].b = DEFAULT_B;
		}
	}

	for (;; sys->iter++){
		if (args->save_mult_iter != 0 && sys->iter != 0 && 
			sys->iter % args->save_mult_iter == 0){
			snprintf(
				save_filepath, SAVE_FILEPATH_SIZE, "%s%lu.conf",
				args->save_pref, sys->iter
			);
			write_system(sys, save_filepath, args->isuse_ren_info);
		}

		if (args->exit_iter != 0 && sys->iter == args->exit_iter)
			goto cleanup;

		if (args->isdraw){
			start = SDL_GetTicks();

			while (SDL_PollEvent(&event)){
				switch (event.type){
				case SDL_QUIT:
					goto cleanup;

				case SDL_WINDOWEVENT:
					SDL_GetWindowSize(vp.screen, &vp.width, &vp.height);
					shift.x = (double)vp.width / 2.0 + movement.x;
					shift.y = (double)vp.height / 2.0 + movement.y;
					break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym){
					case ' ':
						args->ispause ^= 1;
						break;

					case 'l':
						snprintf(
							save_filepath, SAVE_FILEPATH_SIZE, "%s%lu.conf",
							args->save_pref, sys->iter
						);
						write_system(sys, save_filepath, args->isuse_ren_info);
						break;

					case 'w':
						shift.y += args->move_step;
						movement.y += args->move_step;
						break;

					case 'a':
						shift.x += args->move_step;
						movement.x += args->move_step;
						break;

					case 's':
						shift.y -= args->move_step;
						movement.y -= args->move_step;
						break;

					case 'd':
						shift.x -= args->move_step;
						movement.x -= args->move_step;
						break;

					case SDLK_UP:
						scale *= args->scale_mult;
						scale = (scale < MAX_SCALE) ? scale : MAX_SCALE;
						break;

					case SDLK_DOWN:
						scale /= args->scale_mult;
						scale = (scale > MIN_SCALE) ? scale : MIN_SCALE;
						break;
					}

					break;
				
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1){
						click.x = (double)event.button.x - shift.x;
						click.y = (double)event.button.y - shift.y;

						new_fix_i = get_index_chosen_body(sys->bodies, sys->bodies_ren, 
							&click, scale, sys->len
						);

						if (new_fix_i != sys->len){
							args->fix_i = new_fix_i;
							movement.x = 0;
							movement.y = 0;
						}
					}

					else if (event.button.button == 3){
						args->fix_i = sys->len;
						movement.x = 0;
						movement.y = 0;
						shift.x = (double)vp.width / 2.0;
						shift.y = (double)vp.height / 2.0;
					}
				}
			}

			if (args->fix_i < sys->len){
				shift.x = (double)vp.width / 2.0 - 
					scale * sys->bodies[args->fix_i].x + movement.x;
				shift.y = (double)vp.height / 2.0 - 
					scale * sys->bodies[args->fix_i].y + movement.y;
			}

			SDL_SetRenderDrawColor(vp.ren, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(vp.ren);
			SDL_GetRendererOutputSize(vp.ren, &vp.width, &vp.height);
			render_bodies(&vp, sys->bodies, sys->bodies_ren, &shift, scale, sys->len);
			SDL_RenderPresent(vp.ren);

			if (args->ispause){
				continue;
			}
		}

		if (args->islogi || args->islogb)
			clrscr();

		if (args->islogi)
			printf("Iter: %lu\n", sys->iter);

		if (args->islogb)
			log_bodies(sys->bodies, sys->len);

		update_coords(sys->bodies, sys->len, sys->G);

		if (args->isdraw){
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
	
	if (args && args->isdraw){
		vp_cleanup(&vp);
		free(args);
	}

	return 0;
}