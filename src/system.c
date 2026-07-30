#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#define KEY_SIZE 8
#define VAL_SIZE 984
#define FMT_SIZE 32
#define STR_SIZE 1024
#define INIT_SYS_LEN 1024
#define ADD_SYS_LEN 1024

#define CLEANUP_SYS_AND_RET(sys, file, old_bodies, old_bodies_ren) do {	\
	if (sys){															\
		free((sys)->bodies_ren ? (sys)->bodies_ren : old_bodies_ren);	\
		free((sys)->bodies ? (sys)->bodies : old_bodies);				\
		free(sys); 														\
	}																	\
																		\
	if (file)															\
		fclose(file); 													\
																		\
	return NULL;														\
} while(0)

#define REALLOC_SYS(sys, file, old_bodies, old_bodies_ren, isren) do {	\
	(old_bodies) = (sys)->bodies;										\
	(old_bodies_ren) = (sys)->bodies_ren;								\
	(sys)->bodies = realloc((sys)->bodies, (sys)->len * sizeof(Body));	\
																		\
	if (isren){															\
		(sys)->bodies_ren = realloc(									\
			(sys)->bodies_ren, (sys)->len * sizeof(Body_Ren)			\
		);																\
	}																	\
																		\
	if (!(sys)->bodies || (!(sys)->bodies_ren && (isren)))				\
		CLEANUP_SYS_AND_RET(sys, file, old_bodies, old_bodies_ren);		\
} while (0)

System *read_system(char *filepath, char isren){
	static char str[STR_SIZE], key[KEY_SIZE], val[VAL_SIZE], fmt[FMT_SIZE];
	int n_scanned = -1;
	ssize_t body_i = -1;
	FILE *file = fopen(filepath, "r");
	System *sys = calloc(1, sizeof(System));
	Body *old_bodies = NULL;
	Body_Ren *old_bodies_ren = NULL;

	if (!file || !sys)
		CLEANUP_SYS_AND_RET(sys, file, sys->bodies, sys->bodies_ren);

	snprintf(fmt, FMT_SIZE, "%%%ds %%%ds", KEY_SIZE - 1, VAL_SIZE - 1);

	sys->len = INIT_SYS_LEN;
	REALLOC_SYS(sys, file, old_bodies, old_bodies_ren, isren);

	while (fgets(str, STR_SIZE, file) != NULL){
		n_scanned = sscanf(str, fmt, key, val);

		/* Comment or empty string */
		if (key[0] == '#' || n_scanned == 0 || n_scanned == EOF)
			continue;

		/* New body */
		if (key[0] == '%' && n_scanned == 1){
			body_i++;

			if (body_i >= sys->len){
				sys->len += ADD_SYS_LEN;
				REALLOC_SYS(sys, file, old_bodies, old_bodies_ren, isren);
			}

			continue;
		}

		if (n_scanned < 2)
			CLEANUP_SYS_AND_RET(sys, file, sys->bodies, sys->bodies_ren);

		if (!strcmp(key, "@iter")){
			sys->iter = strtoull(val, NULL, 0);
			continue;
		}
			
		else if (!strcmp(key, "@G")){
			sys->G = atof(val);
			continue;
		}

		if (body_i < 0)
			CLEANUP_SYS_AND_RET(sys, file, sys->bodies, sys->bodies_ren);

		else if (!strcmp(key, "x"))
			sys->bodies[body_i].x = atof(val);

		else if (!strcmp(key, "y"))
			sys->bodies[body_i].y = atof(val);

		else if (!strcmp(key, "vx"))
			sys->bodies[body_i].vx = atof(val);

		else if (!strcmp(key, "vy"))
			sys->bodies[body_i].vy = atof(val);

		else if (!strcmp(key, "m"))
			sys->bodies[body_i].m = atof(val);

		else if (!strcmp(key, "r") && isren)
			sys->bodies_ren[body_i].radius = atof(val);

		else if (!strcmp(key, "rgb") && isren){
			sys->bodies_ren[body_i].r = hex_char_to_int(val[0]) * 16 +
				hex_char_to_int(val[1]);
			sys->bodies_ren[body_i].g = hex_char_to_int(val[2]) * 16 +
				hex_char_to_int(val[3]);
			sys->bodies_ren[body_i].b = hex_char_to_int(val[4]) * 16 +
				hex_char_to_int(val[5]);
		}
	}

	if (body_i < 0)
		CLEANUP_SYS_AND_RET(sys, file, old_bodies, old_bodies_ren);

	sys->len = body_i + 1;
	REALLOC_SYS(sys, file, old_bodies, old_bodies_ren, isren);

	fclose(file);

	return sys;
}

int write_system(System *sys, char *filepath, char isren){
	FILE *file = fopen(filepath, "w");

	if (!file){
		fclose(file);

		return 0;
	}

	fprintf(file, "@iter %lu\n@G %.9lf\n\n", sys->iter, sys->G);

	for (size_t i = 0; i < sys->len; i++){
		fprintf(
			file, 
			"%%\nx %.9lf\ny %.9lf\nvx %.9lf\nvy %.9lf\nm %.9lf\n",
			sys->bodies[i].x, sys->bodies[i].y, 
			sys->bodies[i].vx, sys->bodies[i].vy, sys->bodies[i].m
		);

		if (isren){
			fprintf(
				file, 
				"r %.9lf\nrgb %02X%02X%02X\n",
				sys->bodies_ren[i].radius, sys->bodies_ren[i].r,
				sys->bodies_ren[i].g, sys->bodies_ren[i].b
			);
		}
	}

	fclose(file);

	return 1;
}

void log_bodies(Body *bodies, size_t len){
	for (size_t i = 0; i < len; i++){
		printf("BODY %lu\n", i);
		printf("\tx: %lf\ty: %lf\n\tvx: %lf\tvy: %lf\n\tm: %lf\n", 
			bodies[i].x, bodies[i].y, bodies[i].vx, bodies[i].vy, bodies[i].m
		);
	}
}

int hex_char_to_int(char ch){
	if (ch >= '0' && ch <= '9')
		return ch - '0';

	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;

	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;

	return -1;
}