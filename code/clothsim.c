#include <stdio.h>
#include <SDL.h>
#include <math.h>
#include <stdlib.h>

#define WIDTH 900.0
#define HEIGHT 600.0
#define PARTICLE_WIDTH 8
#define PARTICLE_HEIGHT 8
#define COLOR 0xffffffff

int isRunning = 1;
int down = 0;
double deltat = 0.25;

typedef struct _vector {
    double dx;
    double dy;
} vector;

typedef struct _particle {
    double x;
    double y;
    double prevx;
    double prevy;
    double mass;
} particle;

typedef struct _fibre {
    particle p1;
    particle p2;
    double length;
} fibre;

particle p_arr[4];
fibre f_arr[4];


double 
distance(particle p1, particle p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return sqrt((dx * dx) + (dy * dy));
}

void
keepitinside(particle* p) {
    vector vel = { p->x - p->prevx, p->y - p->prevy };

    if (p->x > WIDTH - PARTICLE_WIDTH) { p->x = WIDTH - PARTICLE_WIDTH; p->prevx = p->x + vel.dx; }
    if (p->x < 0.0) { p->x = 0.0; p->prevx = p->x + vel.dx; }
    if (p->y > HEIGHT - PARTICLE_HEIGHT) { p->y = HEIGHT - PARTICLE_HEIGHT; p->prevy = p->y + vel.dy; }
    if (p->y < 0.0) { p->y = 0.0; p->prevy = p->y + vel.dy; }
}

void 
render(SDL_Renderer* pRenderer) { 
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(pRenderer);


    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255); // White particles
    for (int i = 0; i < 4; i++) {
        SDL_Rect pixel = (SDL_Rect){ (int)p_arr[i].x, (int)p_arr[i].y, PARTICLE_WIDTH, PARTICLE_HEIGHT };
        SDL_RenderFillRect(pRenderer, &pixel);
    }

    SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255); // Green fibres
    for (int i = 0; i < 4; i++) {
        SDL_RenderDrawLine(pRenderer, (int)f_arr[i].p1.x, (int)f_arr[i].p1.y, (int)f_arr[i].p2.x, (int)f_arr[i].p2.y);
    }
}

void 
update() {
    for (int i = 0; i < 4; i++) {
        particle p = p_arr[i];

        vector force = { 0.25, 0.5 };
        vector acceleration = { force.dx / p.mass, force.dy / p.mass };

        vector temp = { p.x, p.y };

        p.x = p.x + (p.x - p.prevx) + acceleration.dx * (deltat * deltat);
        p.y = p.y + (p.y - p.prevy) + acceleration.dy * (deltat * deltat);

        p.prevx = temp.dx;
        p.prevy = temp.dy;

        keepitinside(&p);
        p_arr[i] = p;
    }
}

void
setup() {
    p_arr[0].x = 220;
    p_arr[0].y = 20;

    p_arr[1].x = 280;
    p_arr[1].y = 20;

    p_arr[2].x = 280;
    p_arr[2].y = 60;

    p_arr[3].x = 220;
    p_arr[3].y = 60;

    for (int i = 0; i < 4; i++) {
        p_arr[i].prevx = p_arr[i].x;
        p_arr[i].prevy = p_arr[i].y;
        p_arr[i].mass = 10000.0;
    }

    f_arr[0].p1 = p_arr[0];
    f_arr[0].p2 = p_arr[1];
    f_arr[0].length = distance(p_arr[0], p_arr[1]);

    f_arr[1].p1 = p_arr[1];
    f_arr[1].p2 = p_arr[2];
    f_arr[1].length = distance(p_arr[1], p_arr[2]);

    f_arr[2].p1 = p_arr[2];
    f_arr[2].p2 = p_arr[3];
    f_arr[2].length = distance(p_arr[2], p_arr[3]);

    f_arr[3].p1 = p_arr[3];
    f_arr[3].p2 = p_arr[0];
    f_arr[3].length = distance(p_arr[3], p_arr[0]);
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
            if (event.type == SDL_QUIT) isRunning = 0;
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) isRunning = 0;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) down = 1;
            else if (event.type == SDL_MOUSEBUTTONUP) down = 0;
            else if (event.type == SDL_MOUSEMOTION) {
                if (down) {
                    int mx, my;
                    SDL_GetMouseState(&mx, &my);
                }
            }
        }

        update();
        render(pRenderer);
        SDL_RenderPresent(pRenderer);
        // SDL_Delay(25);
    }

    return 0;
}
