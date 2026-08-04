#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <SDL2/SDL.h>
#include "vp.h"
#include "args.h"
#include "body.h"
#include "bodyren.h"
#include "system.h"
#define SAVE_DIR_MAX_LEN 255
#define SAVE_PREF_MAX_LEN 231
#define SYS_FILEPATH_MAX_LEN 256
#define SAVE_FILE_PATH_SIZE 512
#define MAX_SCALE 4096.0
#define MIN_SCALE (1.0 / 4096.0)
#define DEFAULT_RADIUS 3.0
#define DEFAULT_R 0xFF
#define DEFAULT_G 0xFF
#define DEFAULT_B 0xFF

int main(int argc, char *argv[]){
	static char save_file_path[SAVE_FILE_PATH_SIZE];
	static char sys_filepath_buf[SYS_FILEPATH_MAX_LEN + 1] = "\0";
	static char save_dir_buf[SAVE_DIR_MAX_LEN + 1] = "./";
	static char save_pref_buf[SAVE_PREF_MAX_LEN + 1] = "sys_";
	Uint32 start = 0, elapsed = 0, estimated = 0;
	SDL_Event event;
	int save_dir_len = -1;
	double scale = 1.0;
	size_t new_fix_i;
	Vec2 click;
	Vec2 shift = {0, 0};
	Vec2 movement = {0, 0};
	View_Port vp = {-1, -1, NULL, NULL};
	System sys = {0, 0.0, 0, NULL, NULL};
	Args args = {
		0, 1, 1, 1, 1, -1, -1, 60, 2.0, 10.0,
		SIZE_MAX, 0, 0, SYS_FILEPATH_MAX_LEN,
		SAVE_DIR_MAX_LEN, SAVE_PREF_MAX_LEN,
		sys_filepath_buf, save_dir_buf, save_pref_buf
	};
	
	get_args(&args, argc, argv);
	new_fix_i = args.fix_i;

	if (args.sys_filepath[0] == '\0')
		goto cleanup;

	if (args.isdraw){
		if (!vp_init(&vp, args.width, args.height, "gravsim"))
			goto cleanup;
	}
	
	shift.x = (double)vp.width / 2.0;
	shift.y = (double)vp.height / 2.0;
	estimated = 1000 / args.fps;
	save_dir_len = strlen(args.save_dir);
	
	if (!read_system(&sys, args.sys_filepath, args.isuse_ren_info))
		goto cleanup;

	if (save_dir_len > SAVE_DIR_MAX_LEN || 
		strlen(args.save_pref) > SAVE_PREF_MAX_LEN)
	{
		goto cleanup;
	}

	if (!sys.bodies || (!sys.bodies_ren && args.isuse_ren_info))
		goto cleanup;

	if (args.isdraw && !args.isuse_ren_info){
		sys.bodies_ren = malloc(sys.len * sizeof(Body_Ren));

		if (!sys.bodies_ren)
			goto cleanup;

		for (size_t i = 0; i < sys.len; i++){
			sys.bodies_ren[i].radius = DEFAULT_RADIUS;
			sys.bodies_ren[i].r = DEFAULT_R;
			sys.bodies_ren[i].g = DEFAULT_G;
			sys.bodies_ren[i].b = DEFAULT_B;
		}
	}

	if (args.save_dir[save_dir_len - 1] != '/'){
		if (save_dir_len >= SAVE_DIR_MAX_LEN)
			goto cleanup;

		args.save_dir[save_dir_len] = '/';
		args.save_dir[save_dir_len + 1] = '\0';
		save_dir_len++;
	}

	for (;; sys.iter++){
		if (args.save_mult_iter != 0 && sys.iter != 0 && 
			sys.iter % args.save_mult_iter == 0){
			snprintf(
				save_file_path, SAVE_FILE_PATH_SIZE, "%s%s%llu.conf",
				args.save_dir, args.save_pref, (unsigned long long)sys.iter
			);
			write_system(&sys, save_file_path, args.isuse_ren_info);
		}

		if (args.exit_iter != 0 && sys.iter == args.exit_iter)
			goto cleanup;

		if (args.isdraw){
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
						args.ispause ^= 1;
						break;

					case 'l':
						snprintf(
							save_file_path, SAVE_FILE_PATH_SIZE, 
							"%s%s%llu.conf", args.save_dir, 
							args.save_pref, (unsigned long long)sys.iter
						);
						write_system(
							&sys, save_file_path, args.isuse_ren_info
						);
						break;

					case 'w':
						shift.y += args.move_step;
						movement.y += args.move_step;
						break;

					case 'a':
						shift.x += args.move_step;
						movement.x += args.move_step;
						break;

					case 's':
						shift.y -= args.move_step;
						movement.y -= args.move_step;
						break;

					case 'd':
						shift.x -= args.move_step;
						movement.x -= args.move_step;
						break;

					case SDLK_UP:
						scale *= args.scale_mult;
						scale = (scale < MAX_SCALE) ? scale : MAX_SCALE;
						break;

					case SDLK_DOWN:
						scale /= args.scale_mult;
						scale = (scale > MIN_SCALE) ? scale : MIN_SCALE;
						break;
					}

					break;
				
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 1){
						click.x = (double)event.button.x - shift.x;
						click.y = (double)event.button.y - shift.y;

						new_fix_i = get_index_chosen_body(
							sys.bodies, sys.bodies_ren, 
							&click, scale, sys.len
						);

						if (new_fix_i != sys.len){
							args.fix_i = new_fix_i;
							movement.x = 0;
							movement.y = 0;
						}
					}

					else if (event.button.button == 3){
						args.fix_i = sys.len;
						movement.x = 0;
						movement.y = 0;
						shift.x = (double)vp.width / 2.0;
						shift.y = (double)vp.height / 2.0;
					}
				}
			}

			if (args.fix_i < sys.len){
				shift.x = (double)vp.width / 2.0 - 
					scale * sys.bodies[args.fix_i].x + movement.x;
				shift.y = (double)vp.height / 2.0 - 
					scale * sys.bodies[args.fix_i].y + movement.y;
			}

			SDL_SetRenderDrawColor(vp.ren, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderClear(vp.ren);
			SDL_GetRendererOutputSize(vp.ren, &vp.width, &vp.height);
			render_bodies(&vp, sys.bodies, sys.bodies_ren, 
				&shift, scale, sys.len
			);
			SDL_RenderPresent(vp.ren);

			if (args.ispause){
				continue;
			}
		}

		if (args.islogi || args.islogb)
			clrscr();

		if (args.islogi)
			printf("Iter: %llu\n", (unsigned long long)sys.iter);

		if (args.islogb)
			log_bodies(sys.bodies, sys.len);

		update_coords(sys.bodies, sys.len, sys.G);

		if (args.isdraw){
			elapsed = SDL_GetTicks() - start;

			if (elapsed < estimated)
				SDL_Delay(estimated - elapsed);
		}
	}

cleanup:
	free(sys.bodies_ren);
	free(sys.bodies);
	
	/* vp.width == -1 if vp_init failed or didn't called */
	if (vp.width != -1)
		vp_cleanup(&vp);

	return 0;
}
