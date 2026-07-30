#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include "body.h"
#include "bodyren.h"

typedef struct System {
	uint64_t iter;
	double G;
	size_t len;
	Body *bodies;
	Body_Ren *bodies_ren;
} System;

System *read_system(char *filepath, char isren);
int write_system(System *sys, char *filepath, char isren);
void log_bodies(Body *bodies, size_t len);
int hex_char_to_int(char ch);

#endif