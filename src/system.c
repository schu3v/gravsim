#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#define KEY_LEN 6
#define VAL_LEN 64
#define FMT_LEN 32
#define STR_LEN 1024
#define INIT_SYS_LEN 1024
#define ADD_SYS_LEN 1024

#define CLEANUP_SYS_AND_RET(sys, file, old_bodies, old_bodies_ren) do {	\
	free((sys)->bodies_ren ? (sys)->bodies_ren : old_bodies_ren);		\
	free((sys)->bodies ? (sys)->bodies : old_bodies);					\
	free(sys); 															\
	fclose(file); 														\
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
	static char str[STR_LEN], key[KEY_LEN], val[VAL_LEN], fmt[FMT_LEN];
	int n_scanned = -1;
	ssize_t body_i = -1;
	FILE *file = fopen(filepath, "r");
	System *sys = calloc(1, sizeof(System));
	Body *old_bodies = NULL;
	Body_Ren *old_bodies_ren = NULL;

	if (!file || !sys){
		free(sys);
		fclose(file);

		return NULL;
	}

	snprintf(fmt, FMT_LEN, "%%%ds %%%ds", KEY_LEN - 1, VAL_LEN - 1);

	sys->len = INIT_SYS_LEN;
	REALLOC_SYS(sys, file, sys->bodies, sys->bodies_ren, isren);

	while (fgets(str, STR_LEN, file) != NULL){
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
			sys->iter = strtoul(val, NULL, 0);
			continue;
		}
			
		else if (!strcmp(key, "@G")){
			sys->G = strtof(val, NULL);
			continue;
		}

		if (body_i < 0)
			CLEANUP_SYS_AND_RET(sys, file, sys->bodies, sys->bodies_ren);

		else if (!strcmp(key, "x"))
			sys->bodies[body_i].x = (float)atof(val);

		else if (!strcmp(key, "y"))
			sys->bodies[body_i].y = (float)atof(val);

		else if (!strcmp(key, "vx"))
			sys->bodies[body_i].vx = (float)atof(val);

		else if (!strcmp(key, "vy"))
			sys->bodies[body_i].vy = (float)atof(val);

		else if (!strcmp(key, "m"))
			sys->bodies[body_i].m = (float)atof(val);

		else if (!strcmp(key, "r") && isren)
			sys->bodies_ren[body_i].radius = (float)atof(val);

		else if (!strcmp(key, "rgb") && isren){
			sys->bodies_ren[body_i].r = hex_char_to_int(val[0]) * 16 +
				hex_char_to_int(val[1]);
			sys->bodies_ren[body_i].g = hex_char_to_int(val[2]) * 16 +
				hex_char_to_int(val[3]);
			sys->bodies_ren[body_i].b = hex_char_to_int(val[4]) * 16 +
				hex_char_to_int(val[5]);
		}
	}

	sys->len = body_i + 1;
	REALLOC_SYS(sys, file, old_bodies, old_bodies_ren, isren);

	fclose(file);

	return sys;
}

int write_system(System *sys, char *filepath){
	FILE *file = fopen(filepath, "w");

	if (!file){
		fclose(file);

		return 0;
	}

	fprintf(file, "@iter %u\n@G %.9f\n\n", sys->iter, sys->G);

	for (size_t i = 0; i < sys->len; i++){
		fprintf(
			file, 
			"%%\nx %.9f\ny %.9f\nvx %.9f\nvy %.9f\nm %.9f\n",
			sys->bodies[i].x, sys->bodies[i].y, 
			sys->bodies[i].vx, sys->bodies[i].vy, sys->bodies[i].m
		);

		if (sys->bodies_ren){
			fprintf(
				file, 
				"r %.9f\nrgb %02X%02X%02X\n",
				sys->bodies_ren[i].radius, sys->bodies_ren[i].r,
				sys->bodies_ren[i].g, sys->bodies_ren[i].b
			);
		}
	}

	fclose(file);

	return 1;
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