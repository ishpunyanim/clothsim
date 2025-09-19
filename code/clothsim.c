#include <stdio.h>
#include <SDL.h>
#include <math.h>
#include <stdlib.h>

#define WIDTH 900.0
#define HEIGHT 600.0
#define PARTICLE_WIDTH 1
#define PARTICLE_HEIGHT 1
#define COLOR 0xffffffff
#define PARTICLE_COUNT 75

int isRunning = 1;
int down = 0;
double deltat = 0.25;

typedef struct _vector {
	double doublex;
	double doubley;
} vector;

typedef struct _particle {
	double x;
	double y;
	double prevx;
	double prevy;
	double mass;
	int pinned;
} particle;

typedef struct _fibre {
	particle* p1;
	particle* p2;
	int broken;
	double length;
} fibre;

particle p_arr[PARTICLE_COUNT][PARTICLE_COUNT];
fibre f_arr_h[PARTICLE_COUNT * (PARTICLE_COUNT - 1)];
fibre f_arr_v[PARTICLE_COUNT * (PARTICLE_COUNT - 1)];

int mx, my, prev_mx, prev_my;
double cursor_size = 25.0;
Uint32 button;

int
selected(particle p) {

	vector cursor_to_point_direction = { p.x - (double)mx, p.y - (double)my };
	double cursor_to_position_distance = cursor_to_point_direction.doublex * cursor_to_point_direction.doublex +
		cursor_to_point_direction.doubley * cursor_to_point_direction.doubley;
	return (cursor_to_position_distance < cursor_size* cursor_size);
}

double
distance(particle p1, particle p2) {
	double dx = p1.x - p2.x;
	double dy = p1.y - p2.y;
	return sqrt((dx * dx) + (dy * dy));
}

void
keepitinside(particle* p) {
	vector vel = { p->x - p->prevx, p->y - p->prevy };

	if (p->x > WIDTH - PARTICLE_WIDTH) { p->x = WIDTH - PARTICLE_WIDTH; p->prevx = p->x; }
	if (p->x < 0.0) { p->x = 0.0; p->prevx = p->x; }
	if (p->y > HEIGHT - PARTICLE_HEIGHT) { p->y = HEIGHT - PARTICLE_HEIGHT; p->prevy = p->y; }
	if (p->y < 0.0) { p->y = 0.0; p->prevy = p->y; }
}

void
render(SDL_Renderer* pRenderer) {
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255); // black background
	SDL_RenderClear(pRenderer);


	SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255); // white particles (not existing anymore)
	for (int i = 0; i < PARTICLE_COUNT; i++) {
		for (int j = 0; j < PARTICLE_COUNT; j++) {
			// SDL_Rect pixel = (SDL_Rect){ (int)p_arr[i][j].x, (int)p_arr[i][j].y, PARTICLE_WIDTH, PARTICLE_HEIGHT };
			// SDL_RenderFillRect(pRenderer, &pixel);
			SDL_RenderDrawPoint(pRenderer, (int)p_arr[i][j].x, (int)p_arr[i][j].y);
		}
	}

	SDL_SetRenderDrawColor(pRenderer, 0, 140, 100, 255); // colored fibres
	for (int i = 0; i < PARTICLE_COUNT * (PARTICLE_COUNT - 1); i++) {
		if (!f_arr_h[i].broken) SDL_RenderDrawLine(pRenderer, (int)f_arr_h[i].p1->x, (int)f_arr_h[i].p1->y, (int)f_arr_h[i].p2->x, (int)f_arr_h[i].p2->y);
	}

	for (int i = 0; i < PARTICLE_COUNT * (PARTICLE_COUNT - 1); i++) {
		if (!f_arr_v[i].broken) SDL_RenderDrawLine(pRenderer, (int)f_arr_v[i].p1->x, (int)f_arr_v[i].p1->y, (int)f_arr_v[i].p2->x, (int)f_arr_v[i].p2->y);
	}
}

void
update() {
	for (int i = 0; i < PARTICLE_COUNT; i++) {
		for (int j = 0; j < PARTICLE_COUNT; j++) {
			particle p = p_arr[i][j];

			if (p.pinned) continue;

			vector force = { 4.9, 4.9 };
			vector acceleration = { force.doublex / p.mass, force.doubley / p.mass };

			// Dragging mechanism
			// Verlet step: new position = current position + implicit velocity + acceleration
			// Because we modified prevx/prevy during dragging, the velocity term (x - prevx)
			// now contains the mouse motion, so the particle is carried along with the cursor.
			if (down && selected(p)) {
				double elasticity = 10.0;
				double stiffness = 0.5;
				vector difference = { (double)(mx - prev_mx), (double)(my - prev_my) };
				if (difference.doublex > elasticity) difference.doublex = elasticity;
				if (difference.doubley > elasticity) difference.doubley = elasticity;
				if (difference.doublex < -elasticity) difference.doublex = -elasticity;
				if (difference.doubley < -elasticity) difference.doubley = -elasticity;

				p.prevx = p.x - difference.doublex * stiffness;
				p.prevy = p.y - difference.doubley * stiffness;
			}

			vector temp = { p.x, p.y };

			p.x = p.x + (p.x - p.prevx) + acceleration.doublex * (deltat * deltat);
			p.y = p.y + (p.y - p.prevy) + acceleration.doubley * (deltat * deltat);

			p.prevx = temp.doublex;
			p.prevy = temp.doubley;

			keepitinside(&p);
			p_arr[i][j] = p;
		}
	}

	// fibres must store *pointers* to particles, not copies 
	// otherwise constraint solver moves fake copies instead of real ones
	// for (int k = 0; k < 5; k++) { // repeat solver iterations
	for (int i = 0; i < PARTICLE_COUNT * (PARTICLE_COUNT - 1); i++) {
		if (f_arr_h[i].broken) continue; // skip already broken fibre

		if (down && button == SDL_BUTTON_RIGHT && (selected(*f_arr_h[i].p1) || selected(*f_arr_h[i].p2))) {
			f_arr_h[i].broken = 1; 
			continue; 
		} 

		double dx = f_arr_h[i].p2->x - f_arr_h[i].p1->x;
		double dy = f_arr_h[i].p2->y - f_arr_h[i].p1->y;

		double dist = sqrt(dx * dx + dy * dy);
		double diff = f_arr_h[i].length - dist;
		double percent = (diff / dist) / 2.0;

		double offsetx = dx * percent;
		double offsety = dy * percent;

		if (!f_arr_h[i].p1->pinned && !f_arr_h[i].broken) {
			f_arr_h[i].p1->x -= offsetx;
			f_arr_h[i].p1->y -= offsety;
		}

		if (!f_arr_h[i].p2->pinned && !f_arr_h[i].broken) {
			f_arr_h[i].p2->x += offsetx;
			f_arr_h[i].p2->y += offsety;
		}
	}

	for (int i = 0; i < PARTICLE_COUNT * (PARTICLE_COUNT - 1); i++) {
		if (f_arr_v[i].broken) continue; // skip already broken fibre

		if (down && button == SDL_BUTTON_RIGHT && (selected(*f_arr_v[i].p1) || selected(*f_arr_v[i].p2))) {
			f_arr_v[i].broken = 1;
			continue;
		}

		double dx = f_arr_v[i].p2->x - f_arr_v[i].p1->x;
		double dy = f_arr_v[i].p2->y - f_arr_v[i].p1->y;

		double dist = sqrt(dx * dx + dy * dy);
		double diff = f_arr_v[i].length - dist;
		double percent = (diff / dist) / 2.0;

		double offsetx = dx * percent;
		double offsety = dy * percent;

		if (!f_arr_v[i].p1->pinned && !f_arr_v[i].broken) {
			f_arr_v[i].p1->x -= offsetx;
			f_arr_v[i].p1->y -= offsety;
		}

		if (!f_arr_v[i].p2->pinned && !f_arr_v[i].broken) {
			f_arr_v[i].p2->x += offsetx;
			f_arr_v[i].p2->y += offsety;
		}
	}
	// }
}

void
setup() {
	for (int i = 0; i < PARTICLE_COUNT; i++) {
		for (int j = 0; j < PARTICLE_COUNT; j++) {
			particle p = p_arr[i][j];
			p.x = WIDTH / 4.0 + (j * WIDTH / (2 * PARTICLE_COUNT));
			p.y = 50 + i * 5;

			p.prevx = p.x;
			p.prevy = p.y;
			p.mass = 19.6;

			// if (i == 0) p.pinned = 1;
			// else p.pinned = 0;

			p_arr[i][j] = p;
		}
	}
	p_arr[0][0].pinned = 1;
	p_arr[0][PARTICLE_COUNT / 4].pinned = 1;
	p_arr[0][PARTICLE_COUNT / 2].pinned = 1;
	p_arr[0][3 * PARTICLE_COUNT / 4].pinned = 1;
	p_arr[0][PARTICLE_COUNT - 1].pinned = 1;

	for (int i = 0; i < PARTICLE_COUNT; i++) {
		for (int j = 0; j < PARTICLE_COUNT - 1; j++) {
			int l = i * (PARTICLE_COUNT - 1) + j;
			f_arr_h[l].p1 = &p_arr[i][j];
			f_arr_h[l].p2 = &p_arr[i][j + 1];
			f_arr_h[l].length = distance(p_arr[i][j], p_arr[i][j + 1]);
		}
	}
	for (int i = 0; i < PARTICLE_COUNT - 1; i++) {
		for (int j = 0; j < PARTICLE_COUNT; j++) {
			int l = i * PARTICLE_COUNT + j;
			f_arr_v[l].p1 = &p_arr[i][j];
			f_arr_v[l].p2 = &p_arr[i + 1][j];
			f_arr_v[l].length = distance(p_arr[i][j], p_arr[i + 1][j]);
		}
	}
}

int
main() {
	printf("Hello Humans!");

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* pWindow = SDL_CreateWindow("Cloth Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

	// Create a SDL renderer
	SDL_Renderer* pRenderer = SDL_CreateRenderer(pWindow, -1, 0);

	setup();

	while (isRunning) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {  // poll until all events are handled in a single frame before moving to next frame
			prev_mx = mx;
			prev_my = my;
			SDL_GetMouseState(&mx, &my);

			if (event.type == SDL_QUIT) isRunning = 0;
			else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_ESCAPE) isRunning = 0;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				down = 1;
				button = event.button.button;
			}
			else if (event.type == SDL_MOUSEBUTTONUP) down = 0;
		}

		update();
		render(pRenderer);
		SDL_RenderPresent(pRenderer);
		// SDL_Delay(25);
	}

	return 0;
}
