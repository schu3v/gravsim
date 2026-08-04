#include <math.h>
#include <SDL2/SDL.h>
#include "vp.h"

int vp_init(View_Port *vp, int width, int height, const char *title){
	Uint32 isfull = (width < 0 || height < 0) ? SDL_WINDOW_MAXIMIZED : 0;
	SDL_DisplayMode dm;

	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		return 0;
	}

	vp->screen = SDL_CreateWindow(
		title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | isfull
	);

	if (!vp->screen){
		SDL_Quit();

		return 0;
	}

	vp->ren = SDL_CreateRenderer(
		vp->screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!vp->ren || SDL_GetCurrentDisplayMode(0, &dm)){
		SDL_Quit();
		
		return 0;
	}

	vp->width  = isfull ? dm.w : width;
	vp->height = isfull ? dm.h : height;

	return 1;
}

void vp_cleanup(View_Port *vp){
	SDL_DestroyRenderer(vp->ren);
	SDL_DestroyWindow(vp->screen);
}

void render_filled_circle(View_Port *vp, double x0, double y0, double r){
	double y1 = y0, y2 = y0;
	double x1, x2;

	while (y1 >= y0 - r){
		x1 = x0 - floor(sqrt(r*r - (y1 - y0)*(y1 - y0)));
		x2 = x0 + floor(sqrt(r*r - (y1 - y0)*(y1 - y0)));

		SDL_RenderDrawLine(vp->ren, x1, y1, x2, y1);
		SDL_RenderDrawLine(vp->ren, x1, y2, x2, y2);
		y1--;
		y2++;
	}
}
