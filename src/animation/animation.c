#include "animation.h"
#include "../log/log.h"

#include <dirent.h>
#include <stdio.h>

void animation_free_background(struct background_s * b)
{
    SDL_DestroyTexture(b->t);
}

void animation_free_sprite(struct sprite_s * s)
{
    int i;
    
    for (i = 0; i < s->n_state; ++i)
    {
        SDL_DestroyTexture(s->states[i]);
    }
}

void animation_render_sprite(SDL_Renderer * renderer,
                             struct sprite_s * s)
{
    sdl_render_image(renderer, s->states[(int) s->current_animation], s->r);
}

void animation_update_sprite(struct sprite_s * s, float speed)
{
    s->current_animation += speed;
    if ((int) s->current_animation >= s->n_state) s->current_animation = 0.0;
}

struct sprite_s * animation_sprite_from_file(SDL_Renderer * renderer, char * rname)
{
    struct sprite_s * s = NULL;
    
    struct dirent *pDirent = NULL;
    DIR *pDir = NULL;

    char buf[MAX_PATH]; /* realpath of files */
    int i = 0;
    
    pDir = opendir (rname);
    if (pDir == NULL) {
        zlog(stderr, ERROR, "Cannot open directory '%s'\n", rname);
        exit(-1);
    }

    s = (struct sprite_s *) malloc(sizeof(struct sprite_s));
    s->r.x = 0;
    s->r.y = 0;
    s->current_animation = 0.0;
    
    while ((pDirent = readdir(pDir)) != NULL) {
        if (strcmp(pDirent->d_name, ".") != 0 && strcmp(pDirent->d_name, "..") != 0)
        {
            strncpy(buf, rname, MAX_PATH);
            strncat(buf, pDirent->d_name, MAX_PATH-1);
            zlog(stdout, DEBUG, "'%s'", buf);

            s->states[i] = sdl_load_image(renderer, buf);
            i++;
        }
    }

    s->n_state = i;
    if (i > 0)
    {
        SDL_QueryTexture(s->states[0], NULL, NULL, &s->r.w, &s->r.h);
    }

    closedir (pDir);
    return s;
}

struct background_s * animation_background_from_file(SDL_Renderer * renderer,
                                                     char * fname)
{
    struct background_s * b = NULL;

    b = (struct background_s *) malloc(sizeof(struct background_s));
    b->name = (char *) malloc(sizeof(char)*strlen(fname));
    strcpy(b->name, fname);

    b->t = sdl_load_image(renderer, fname);

    b->r.x = 0;
    b->r.y = 0;

    SDL_QueryTexture(b->t, NULL, NULL, &b->r.w, &b->r.h);
    
    return b;
}

void animation_render_background(SDL_Renderer * renderer,
                                 struct background_s * b,
                                 int screen_w, int screen_h)
{
    SDL_Rect r_screen;
    SDL_Rect destRect;
    int n = ((int) screen_w/b->r.w) + 2;

    (void) screen_h;
    zlog(stdout, DEBUG, "number of print bakcground '%s': %d", b->name, n);

    r_screen.x = b->r.x;
    r_screen.y = b->r.y;
    r_screen.w = b->r.w;
    r_screen.h = b->r.h;

    while (r_screen.x < screen_w)
    {
        destRect.x = r_screen.x;
        destRect.y = 0;
        destRect.w = b->r.w;
        destRect.h = b->r.h;
        SDL_RenderCopy(renderer, b->t, NULL, &destRect);
        r_screen.x += b->r.w;
    }

}

int animation_main(void)
{
    int width = 1280;
    int height = 720;

    struct sprite_s * s = NULL;

    struct background_s * back = NULL;
    struct background_s * buildings = NULL;
    struct background_s * highway = NULL;
    struct background_s * palms = NULL;

    SDL_Window * window = NULL;
    SDL_Renderer * renderer = NULL;
    SDL_Event event;
    
    int running = 1;

    float speed = 1.0;

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
    sdl_set_renderer_color(renderer, (SDL_Color) {.r = 0, .g = 0, .b = 0, .a = 255});
    
    sdl_init_image();
    
    s = animation_sprite_from_file(renderer, "../data/sprite/");

    zlog(stdout, DEBUG, "%d, %d", s->r.w, s->r.h);

    back = animation_background_from_file(renderer, "../data/parallax/back.png");
    buildings = animation_background_from_file(renderer, "../data/parallax/buildings.png");
    highway = animation_background_from_file(renderer, "../data/parallax/highway.png");
    palms = animation_background_from_file(renderer, "../data/parallax/palms.png");

    sdl_scale_rect_image(&back->r, back->t, height);
    sdl_scale_rect_image(&buildings->r, buildings->t, height);
    sdl_scale_rect_image(&highway->r, highway->t, height);
    sdl_scale_rect_image(&palms->r, palms->t, height);

    for (int i=0; i<s->n_state; i++)
    {
        sdl_scale_rect_image(&s->r, s->states[i], 0.25*height);    
    }
    s->r.x = 20;
    s->r.y = 0.7*height;
    
    while (running)
    {
        SDL_RenderClear(renderer);

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
		}
		break;
	    case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_RIGHT)
		{
                    /* update sprite placement */
                    if (s->r.x < 0.6*width - (s->r.w)/2) s->r.x += speed;

                    /* update background offset */
                    back->r.x -= speed*0.01;
                    buildings->r.x -= speed*0.03;
                    palms->r.x -= speed*0.2;
                    highway->r.x -= speed*0.3;

                    /* update speed */
                    if (speed < 0.8*height) speed *= 1.1;

                    /* update sprite animation */
                    if (speed < 0.3*width) animation_update_sprite(s, speed*0.01);
                    else animation_update_sprite(s, 1);
                }
                else if (event.key.keysym.sym == SDLK_LEFT)
                {
                    if (s->r.x > 0.2*width) s->r.x -= speed;
                    speed *= 1.05;
                    animation_update_sprite(s, 0.1);
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = 0;
                }
		break;
            case SDL_KEYUP:
                speed = 1.0;
                s->current_animation = 0.0;
                break;
	    case SDL_MOUSEBUTTONDOWN:
		zlog(stdout, INFO, "appui: x:%d y:%d", event.button.x, event.button.y);
		break;
	    case SDL_QUIT:
		zlog(stdout, INFO, "event.type: SDL_QUIT", NULL);
		running = 0;
                break;
	    }
	}

        animation_render_background(renderer, back, width, height);
        animation_render_background(renderer, buildings, width, height);
        animation_render_background(renderer, palms, width, height);
        animation_render_background(renderer, highway, width, height);

        animation_render_sprite(renderer, s);
        SDL_RenderPresent(renderer);

        /* delai minimal = 1 */
	SDL_Delay(10);
    }

    SDL_DestroyWindow(window);
    window = NULL;
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    animation_free_sprite(s);
    animation_free_background(back);
    animation_free_background(buildings);
    animation_free_background(palms);
    animation_free_background(highway);
    
    return 0;
}
