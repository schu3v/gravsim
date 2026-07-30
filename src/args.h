#ifndef ARGS_H
#define ARGS_H

#include "body.h"

#ifdef _WIN32
#include <conio.h>
#else
#define clrscr() printf("\e[1;1H\e[2J")
#endif

typedef struct Args {
	char ispause;
	char isdraw;
	char isuse_ren_info;
	char islog;
	char islogi;
	char islogb;
	int width;
	int height;
	int fps;
	double scale_step;
	double move_step;
	size_t fix_i;
	size_t save_mult_iter;
	size_t exit_iter;
	char *sys_filepath;
	char *save_pref;
} Args;

Args *get_args(int argc, char **argv, Args *default_args);

#endif