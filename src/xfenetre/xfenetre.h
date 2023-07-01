#ifndef xfenetre_h
#define xfenetre_h

#include "../sdl/sdl.h"

#define N_WINDOWS 30

struct vector_s {
    int x;
    int y;
};

void xfenetre_particules_animation(int, int);
void xfenetre_implosion_animation(SDL_Window *, SDL_Renderer *,
                                  int, int,
                                  int, int);
void xfenetre_getting_big_and_implosion_animation(int, int);

int xfenetre_main(void);

#endif
