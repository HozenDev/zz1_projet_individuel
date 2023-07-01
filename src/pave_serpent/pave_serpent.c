#include "pave_serpent.h"
#include "../sdl/sdl.h"
#include "../log/log.h"

const struct colors_s colors_available = 
{
    .BLACK    = (SDL_Color) {30, 30, 30, 255},
    .WHITE    = (SDL_Color) {220, 220, 220, 255},
    .RED = (SDL_Color) {255, 100, 100, 255},
    .GREEN  = (SDL_Color) {100, 255, 100, 255},
    .BLUE = (SDL_Color) {100, 100, 255, 255},
    .YELLOW = (SDL_Color) {255, 255, 100, 255}
};

void pave_serpent_draw_circle_vibration(SDL_Renderer * renderer, int cx, int cy, int r, int px, int py)
{
    sdl_draw_circle(renderer, cx+px, cy+py, r);
}

void pave_serpent_draw_figure_vibration(SDL_Renderer * renderer, struct figure_s * figure,
                                        int power_x, int power_y)
{
    struct figure_s * curr = figure;
    
    while (curr != NULL)
    {
        sdl_draw_segment(renderer, curr->s.sx, curr->s.sy, curr->s.ex+power_x, curr->s.ey+power_y);
        zlog(stdout, DEBUG, "Draw segment between (%d, %d) and (%d, %d)",
             curr->s.sx, curr->s.sy, curr->s.ex+power_x, curr->s.ey+power_y);
        curr = curr->next;
    }
}

void pave_serpent_draw_figure(SDL_Renderer * renderer, struct figure_s * figure)
{
    struct figure_s * curr = figure;
    
    while (curr != NULL)
    {
        sdl_draw_segment(renderer, curr->s.sx, curr->s.sy, curr->s.ex, curr->s.ey);
        zlog(stdout, DEBUG, "Draw segment between (%d, %d) and (%d, %d)",
             curr->s.sx, curr->s.sy, curr->s.ex, curr->s.ey);
        curr = curr->next;
    }
}

void pave_serpent_free_figure(struct figure_s ** f0)
{
    struct figure_s * curr = *f0; // maillon courant
 
    while (curr != NULL) // parcours de la liste
    {
	struct figure_s * tmp = curr;
        curr = curr->next;
        (*tmp).s.sx = 0; 
        (*tmp).s.sy = 0;
        (*tmp).s.ex = 0; 
        (*tmp).s.ey = 0;
        (*tmp).next = NULL; 
    
        free(tmp);               // liberation du maillon
    }
    (*f0) = NULL; // mets a0 a NULL car la liste est vide
}

struct figure_s ** pave_serpent_get_figure_from_file(struct figure_s ** f0, char * fname)
{
    FILE * fp = fopen(fname, "r");
    int start_x, start_y, end_x, end_y;

    struct figure_s * figure = NULL;
    struct segment_s segment;

    if (!fp)
    {
        zlog(stderr, ERROR, "Error in opening file: '%s'", fname);
        exit(-1);
    }

    *f0 = NULL;

    while (fscanf(fp, "(%d,%d)-(%d,%d)%*c", &start_x, &start_y, &end_x, &end_y) == 4)
    {
        segment.sx = start_x; segment.sy = start_y;
        segment.ex = end_x; segment.ey = end_y;

        zlog(stdout, DEBUG, "Load segment between (%d, %d) and (%d, %d)",
             segment.sx, segment.sy, segment.ex, segment.ey);
        
        figure = (struct figure_s *) malloc(sizeof(struct figure_s));
        figure->s = segment;
        figure->next = *f0;
        *f0 = figure;
    }

    fclose(fp);

    return f0;
}

int pave_serpent_loop(void)
{
    int running = 1;
    int width = 800;
    int height = 800;
    float px, py = 0.0;

    SDL_Event event;

    SDL_Window * window = NULL;
    SDL_Renderer * renderer = NULL;

    struct figure_s * flower_stem = NULL;
    struct figure_s * flower_stem1 = NULL;
    /* struct figure_s * flower_leaf1 = NULL; */
    /* struct figure_s * flower_leaf2 = NULL; */
    /* struct figure_s * flower_leaf3 = NULL; */

    /* SDL initialisation */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
	zlog(stderr, ERROR, "Erreur d'initialisation de la SDL : %s", SDL_GetError());
	return EXIT_FAILURE;
    }
    zlog(stdout, INFO, "OK '%s'", "SDL is initialized.");

    /* création de la fenetre principale */
    window = sdl_create_window("test", width, height);
    if (!window) exit(-1);
    zlog(stdout, INFO, "OK '%s'", "game_loop: Window is initialized.");

    /* création du renderer */
    renderer = sdl_create_renderer(window);
    if (!renderer) exit(-1);
    zlog(stdout, INFO, "OK '%s'", "game_loop: Renderer is initialized.");

    pave_serpent_get_figure_from_file(&flower_stem, "../data/flower_stem.txt");
    pave_serpent_get_figure_from_file(&flower_stem1, "../data/flower_stem1.txt");
    /* pave_serpent_get_figure_from_file(&flower_leaf1, "../data/flower_leaf1.txt"); */
    /* pave_serpent_get_figure_from_file(&flower_leaf2, "../data/flower_leaf2.txt"); */
    /* pave_serpent_get_figure_from_file(&flower_leaf3, "../data/flower_leaf3.txt"); */

    /* Boucle de jeu */
    while (running) {

	/* Boucle d'évènements */
	while (SDL_PollEvent(&event))
	{
	    switch(event.type)
	    {
	    case SDL_WINDOWEVENT:
		switch (event.window.event)
		{
		case SDL_WINDOWEVENT_CLOSE:
		    zlog(stdout, INFO, "sdl event window close", NULL);
		    break;
		default:
		    break;
		}
		break;
	    case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_SPACE)
		{
                    zlog(stdout, INFO, "SPACE tapped", NULL);
                }
		break;
	    case SDL_MOUSEBUTTONDOWN:
		zlog(stdout, INFO, "appui: x:%d y:%d", event.button.x, event.button.y);
		break;
            case SDL_MOUSEMOTION:
                px = event.motion.xrel;
                py = event.motion.yrel;
                break;
	    case SDL_QUIT:
		zlog(stdout, INFO, "event.type: SDL_QUIT", NULL);
		running = 0;
                break;
	    }
	}

        if (px*px < 1) px = ((rand()%10)+5) * (((float)(rand()%2))-0.5)*2;
        else {
            if (px < 0) px += 0.5;
            else px -= 0.5;
        }
        if (py*py < 1) py = ((rand()%10)+5) * (((float)(rand()%2))-0.5)*2;
        else {
            if (py < 0) py += 0.5;
            else py -= 0.5;
        }

        sdl_set_renderer_color(renderer, (SDL_Color) {.r = 0, .g = 0, .b = 0, .a = 255});
        SDL_RenderClear(renderer);
        
        sdl_set_renderer_color(renderer, colors_available.GREEN);
        pave_serpent_draw_figure_vibration(renderer, flower_stem, px*0.5, py*0.1);
        pave_serpent_draw_figure_vibration(renderer, flower_stem1, px*0.05, py*0);

        sdl_set_renderer_color(renderer, colors_available.RED);
        pave_serpent_draw_circle_vibration(renderer, 500, 100, 30, px*0.5, py*0.1);
        
        sdl_set_renderer_color(renderer, colors_available.BLUE);
        pave_serpent_draw_circle_vibration(renderer, 500, 450, 45, px*0, py*0.4);
        /* pave_serpent_draw_figure_vibration(renderer, flower_leaf1, px*0.3, py*0.3); */

        sdl_set_renderer_color(renderer, colors_available.YELLOW);
        pave_serpent_draw_circle_vibration(renderer, 300, 450, 60, px*0.2, py*0.2);
        /* pave_serpent_draw_figure_vibration(renderer, flower_leaf2, px*0.3, py*0.5); */

        sdl_set_renderer_color(renderer, colors_available.WHITE);
        pave_serpent_draw_circle_vibration(renderer, 250, 200, 20, px*0.5, py*0);
        /* pave_serpent_draw_figure_vibration(renderer, flower_leaf3, px*0.1, py*0); */
        
        SDL_RenderPresent(renderer);
        
	/* delai minimal = 1 */
	SDL_Delay(30);
    }

    pave_serpent_free_figure(&flower_stem);
    pave_serpent_free_figure(&flower_stem1);
    /* pave_serpent_free_figure(&flower_leaf1); */
    /* pave_serpent_free_figure(&flower_leaf2); */
    /* pave_serpent_free_figure(&flower_leaf3); */

    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    return 0;
}

int pave_serpent_main(void)
{
    pave_serpent_loop();
    return 0;
}
