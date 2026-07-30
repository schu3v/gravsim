#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "args.h"

Args *get_args(int argc, char **argv, Args *default_args){
	Args *args = malloc(sizeof(Args));

	if (!args)
		return NULL;

	memcpy(args, default_args, sizeof(Args));

	for (int i = 0; i < argc; i++){
		if (!strcmp(argv[i], "-fnolog")){
			args->islogi = 0;
			args->islogb = 0;
		}

		if (!strcmp(argv[i], "-fnologi"))
			args->islogi = 0;

		if (!strcmp(argv[i], "-fnologb"))
			args->islogb = 0;

		if (!strcmp(argv[i], "-fnodraw"))
			args->isdraw = 0;

		if (!strcmp(argv[i], "-fnoreninfo"))
			args->isuse_ren_info = 0;

		if (!strcmp(argv[i], "-fpause"))
			args->ispause = 1;

		if (i + 1 >= argc)
			continue;

		if (!strcmp(argv[i], "-w"))
			args->width = atoi(argv[i + 1]);

		if (!strcmp(argv[i], "-h"))
			args->height = atoi(argv[i + 1]);

		if (!strcmp(argv[i], "-fps"))
			args->fps = atoi(argv[i + 1]);

		if (!strcmp(argv[i], "-scale"))
			args->scale_mult = atof(argv[i + 1]);

		if (!strcmp(argv[i], "-move"))
			args->move_step = atof(argv[i + 1]);

		if (!strcmp(argv[i], "-fixi"))
			args->fix_i = strtoull(argv[i + 1], NULL, 0);

		if (!strcmp(argv[i], "-siter"))
			args->save_mult_iter = strtoull(argv[i + 1], NULL, 0);

		if (!strcmp(argv[i], "-eiter"))
			args->exit_iter = strtoull(argv[i + 1], NULL, 0);

		if (!strcmp(argv[i], "-sys"))
			args->sys_filepath = argv[i + 1];

		if (!strcmp(argv[i], "-sdir"))
			args->save_dir = argv[i + 1];

		if (!strcmp(argv[i], "-spref"))
			args->save_pref = argv[i + 1];
	}

	return args;
}
