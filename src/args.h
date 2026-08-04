#ifndef ARGS_H
#define ARGS_H

#include "body.h"

typedef struct Args {
	char ispause;
	char isdraw;
	char isuse_ren_info;
	char islogi;
	char islogb;
	int width;
	int height;
	int fps;
	double scale_mult;
	double move_step;
	size_t fix_i;
	size_t save_mult_iter;
	size_t exit_iter;
	size_t sys_filepath_max_len; /* no option, specifies inside program */
	size_t save_dir_max_len; /* no option */
	size_t save_pref_max_len; /* no option */
	char *sys_filepath;
	char *save_dir;
	char *save_pref;
} Args;

int get_args(Args *args, int argc, char **argv);

#endif
