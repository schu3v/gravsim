#include <math.h>
#include <stddef.h>
#include "body.h"

void update_vel(Body *bodies, size_t len, double G){
	double diffx_sqr, diffy_sqr;
	double r_sqr, F;
	Vec2 F_vec;

	for (size_t i = 0; i < len; i++){
		for (size_t j = i + 1; j < len; j++){
			diffx_sqr = pow(bodies[j].x - bodies[i].x, 2.0);
			diffy_sqr = pow(bodies[j].y - bodies[i].y, 2.0);
			r_sqr = diffx_sqr + diffy_sqr;
			F = G * bodies[i].m * bodies[j].m / r_sqr;

			F_vec.x = F * sqrt(diffx_sqr / r_sqr);
			F_vec.y = F * sqrt(diffy_sqr / r_sqr);

			if (bodies[i].x > bodies[j].x){
				F_vec.x *= -1.0;
			}

			if (bodies[i].y > bodies[j].y){
				F_vec.y *= -1.0;
			}

			bodies[i].vx += F_vec.x / bodies[i].m;
			bodies[i].vy += F_vec.y / bodies[i].m;
			bodies[j].vx -= F_vec.x / bodies[j].m;
			bodies[j].vy -= F_vec.y / bodies[j].m;
		}
	}
}

void update_coords(Body *bodies, size_t len, double G){
	update_vel(bodies, len, G);

	for (size_t i = 0; i < len; i++){
		bodies[i].x += bodies[i].vx;
		bodies[i].y += bodies[i].vy;
	}
}