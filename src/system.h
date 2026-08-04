#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include "body.h"
#include "bodyren.h"

#define clrscr() printf("\e[1;1H\e[2J")

typedef struct System {
	uint64_t iter;
	double G;
	size_t len;
	Body *bodies;
	Body_Ren *bodies_ren;
} System;

int read_system(System *sys, char *filepath, char isren);
int write_system(System *sys, char *filepath, char isren);
void log_bodies(Body *bodies, size_t len);
int hex_char_to_int(char ch);

#endif
