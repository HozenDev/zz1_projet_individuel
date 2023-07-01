#ifndef pave_serpent_h
#define pave_serpent_h

#include "../sdl/sdl.h"

struct colors_s {
    SDL_Color BLACK;
    SDL_Color WHITE;
    SDL_Color RED;
    SDL_Color GREEN;
    SDL_Color BLUE;
    SDL_Color YELLOW;
};

struct segment_s {
    int sx, sy; /* start point coordinates */
    int ex, ey; /* end point coordinates */
};

struct figure_s {
    struct segment_s s;
    struct figure_s * next;
};

int pave_serpent_loop(void);

int pave_serpent_main(void);

#endif
