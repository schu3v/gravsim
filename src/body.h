#ifndef BODY_H
#define BODY_H

typedef struct Vec2 {
	double x, y;
} Vec2;

typedef struct Body {
	double x, y;
	double vx, vy;
	double m;
} Body;

void update_vel(Body *bodies, size_t len, double G);
void update_coords(Body *bodies, size_t len, double G);

#endif