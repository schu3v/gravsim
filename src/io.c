#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"

Flags *get_flags_from_args(int argc, char **argv, Flags *default_flags){
	Flags *flags = (Flags *)malloc(sizeof(Flags));

	memcpy(flags, default_flags, sizeof(Flags));

	for (int i = 0; i < argc; i++){
		if (!strcmp(argv[i], "-fpause"))
			flags->ispause = 1;

		if (!strcmp(argv[i], "-fnodraw"))
			flags->isdraw = 0;

		if (!strcmp(argv[i], "-fnouseren"))
			flags->isuse_ren_info = 0;

		if (!strcmp(argv[i], "-fnolog"))
			flags->islog = 0;

		if (!strcmp(argv[i], "-fnologi"))
			flags->islogi = 0;

		if (!strcmp(argv[i], "-fnologb"))
			flags->islogb = 0;

		if (i + 1 >= argc)
			continue;

		if (!strcmp(argv[i], "-w"))
			flags->width = atoi(argv[i + 1]);

		if (!strcmp(argv[i], "-h"))
			flags->height = atoi(argv[i + 1]);

		if (!strcmp(argv[i], "-fps"))
			flags->fps = atoi(argv[i + 1]);

		if (!strcmp(argv[i], "-ss"))
			flags->scale_step = strtof(argv[i + 1], NULL);

		if (!strcmp(argv[i], "-ms"))
			flags->move_step = strtof(argv[i + 1], NULL);

		if (!strcmp(argv[i], "-fixi"))
			flags->fix_i = strtoull(argv[i + 1], NULL, 0);

		if (!strcmp(argv[i], "-siter"))
			flags->save_mult_iter = strtoull(argv[i + 1], NULL, 0);

		if (!strcmp(argv[i], "-eiter"))
			flags->exit_iter = strtoull(argv[i + 1], NULL, 0);

		if (!strcmp(argv[i], "-sys"))
			flags->sys_filepath = argv[i + 1];

		if (!strcmp(argv[i], "-sdir"))
			flags->save_dir = argv[i + 1];

		if (!strcmp(argv[i], "-spref"))
			flags->save_pref = argv[i + 1];
	}

	return flags;
}

void log_bodies(Body *bodies, size_t len){
	for (size_t i = 0; i < len; i++){
		printf("BODY %lu\n", i);
		printf("\tx: %f\ty: %f\n\tvx: %f\tvy: %f\n\tm: %f\n", 
			bodies[i].x, bodies[i].y, bodies[i].vx, bodies[i].vy, bodies[i].m
		);
	}
}