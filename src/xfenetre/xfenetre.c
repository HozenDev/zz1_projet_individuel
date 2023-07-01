#include "../log/log.h"
#include "xfenetre.h"
#include "../utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void xfenetre_getting_big_and_implosion_animation(int screen_width, int screen_height)
{
    int running = 1;
    int window_width = 200;
    int window_height = 200;

    float speed = 1.0;

    SDL_Window * window = NULL;
    SDL_Renderer * renderer = NULL;

    /* création de la fenetre principale */
    window = sdl_create_window("Xfenetre", window_width, window_height);
    if (!window) exit(-1);
    zlog(stdout, INFO, "OK '%s'", "xfenetre: Window is initialized.");

    /* création du renderer */
    renderer = sdl_create_renderer(window);
    if (!renderer) exit(-1);
    zlog(stdout, INFO, "OK '%s'", "xfenetre: Renderer is initialized.");

    sdl_set_renderer_color(renderer, (SDL_Color) {.r = rand()%255,.g = rand()%255,.b = rand()%255,.a = 255});
    
    /* Boucle de jeu */
    while (running)
    {
        speed *= 1.05;

        if (window_width < screen_width*0.4 && window_height < screen_height*0.6) {
            window_width += (int) speed;
            window_height += (int) speed;
        }
        else {
            running = 0;
        }

        SDL_SetWindowSize(window, window_width, window_height);
        SDL_SetWindowPosition(window, (screen_width - window_width)/2, (screen_height - window_height)/2);
        
        /* afficher à l'ecran */
        SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	/* delai minimal = 1 */
	SDL_Delay(10);
    }

    xfenetre_implosion_animation(window, renderer,
                                 screen_width, screen_height,
                                 window_width, window_height);

    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
}

void xfenetre_implosion_animation(SDL_Window * window, SDL_Renderer * renderer,
                                  int screen_width, int screen_height,
                                  int window_width, int window_height)
{
    int running = 1;
    float speed = 1.0;

    while (running) {
        speed *= 1.3;

        if (window_width > screen_width*0.15 && window_height > screen_height*0.25) {
            window_width -= (int) speed;
            window_height -= (int) speed;
        }
        else {
            running = 0;
        }

        /* mise à jour taille et position de la fenetre */
        SDL_SetWindowSize(window, window_width, window_height);
        SDL_SetWindowPosition(window, (screen_width - window_width)/2, (screen_height - window_height)/2);
        
        /* afficher à l'ecran */
        SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	/* delai minimal = 1 */
	SDL_Delay(10);
    }    
}

void xfenetre_particules_animation(int screen_width, int screen_height)
{
    int window_width = 50;
    int window_height = 50;

    SDL_Window * windows[N_WINDOWS];
    SDL_Renderer * renderers[N_WINDOWS];
    struct vector_s movements[N_WINDOWS];

    int i, wwi, whi;
    int max_iter = 200;
    
    for (i=0;i<N_WINDOWS;i++)
    {
        /* création des fenetres */    
        windows[i] = sdl_create_window("Xfenetre", window_width, window_height);
        if (!windows[i]) exit(-1);
        zlog(stdout, INFO, "OK '%s'", "xfenetre: Window is initialized.");

        /* création des renderers */
        renderers[i] = sdl_create_renderer(windows[i]);
        if (!renderers[i]) exit(-1);
        zlog(stdout, INFO, "OK '%s'", "xfenetre: Renderer is initialized.");
        sdl_set_renderer_color(renderers[i], (SDL_Color) {.r = rand()%255, .g = rand()%255, .b = rand()%255, .a = 255});

        /* initialisation vecteur de déplacement */
        movements[i].x = (((float)(rand()%2))-0.5)*2*((rand()%40)+1);
        movements[i].y = (((float)(rand()%2))-0.5)*2*((rand()%40)+1);
    }
    
    /* Boucle de jeu */
    while (max_iter--) {

        for (i = 0; i < N_WINDOWS; ++i) {

            /* mise à jour de la position des fenetres */
            SDL_GetWindowPosition(windows[i], &wwi, &whi);
            if (wwi < 0 || (wwi+window_width) > screen_width) movements[i].x *= -1;
            if (whi < 0 || (whi+window_height) > screen_height) movements[i].y *= -1;
            SDL_SetWindowPosition(windows[i], wwi+movements[i].x, whi+movements[i].y);

            /* ralentissements de la vitesses des fenetres */
            if (max_iter%5 == 0) 
            {
                if (movements[i].x > 0) movements[i].x -= 1;
                else if (movements[i].x < 0) movements[i].x += 1;
                if (movements[i].y > 0) movements[i].y -= 1;
                else if (movements[i].y < 0) movements[i].y += 1;
            }
        
            /* afficher à l'ecran */
            SDL_RenderClear(renderers[i]);
            SDL_RenderPresent(renderers[i]);
        }

	/* delai minimal = 1 */
	SDL_Delay(10);
    }

    for (i = 0; i < N_WINDOWS; ++i) {
         SDL_DestroyWindow(windows[i]);
         windows[i] = NULL;
         SDL_DestroyRenderer(renderers[i]);
         renderers[i] = NULL;   
    }
}

int xfenetre_main(void)
{
    int screen_width = 0;
    int screen_height = 0;
    
    /* SDL initialisation */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
	zlog(stderr, ERROR, "xfenetre: Erreur d'initialisation de la SDL : %s", SDL_GetError());
	return EXIT_FAILURE;
    }
    zlog(stdout, INFO, "OK '%s'", "SDL is initialized.");

    srand(time(NULL));

    utils_get_screen_resolution(&screen_width, &screen_height);
    
    xfenetre_getting_big_and_implosion_animation(screen_width, screen_height);
    xfenetre_particules_animation(screen_width, screen_height);

    SDL_Quit();
    
    return 0;
}
