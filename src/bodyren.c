#include <SDL2/SDL.h>
#include <math.h>
#include "bodyren.h"

void render_body(View_Port *vp, Body *body, Body_Ren *body_ren, 
	Vec2 *shift, double scale)
{
	double x = body->x * scale + shift->x;
	double y = body->y * scale + shift->y;
	double r = body_ren->radius * scale;

	if (
		x + r < 0.0 || y + r < 0.0 || 
		x - r > (double)vp->width || y - r > (double)vp->height
	){
		return;
	}

	SDL_SetRenderDrawColor(vp->ren, body_ren->r, 
		body_ren->g, body_ren->b, 0xFF
	);
	render_filled_circle(vp, x, y, r);
}

void render_bodies(View_Port *vp, Body *bodies, Body_Ren *bodies_ren, 
	Vec2 *shift, double scale, size_t len)
{
	for (size_t i = 0; i < len; i++){
		render_body(vp, bodies + i, bodies_ren + i, shift, scale);
	}
}

size_t get_index_chosen_body(Body *bodies, Body_Ren *bodies_ren, 
	Vec2 *click, double scale, size_t len)
{
	size_t i = 0;

	while (i < len){
		if (pow(bodies[i].x * scale - click->x, 2) + 
			pow(bodies[i].y * scale - click->y, 2) <= 
			pow(bodies_ren[i].radius * scale, 2)){
			return i;
		}

		i++;
	}

	return len;
}