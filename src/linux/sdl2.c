/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"

#if LINUX_SUBSYSTEM

#include "linux_subsystem.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#if WITHSDL2VIDEO

SDL_Renderer * renderer;
SDL_Window * window;
SDL_Texture * texture;

int sdl2_render_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DIM_X, DIM_Y, SDL_WINDOW_RESIZABLE);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        sdl2_render_close();
        return 0;
    }

    SDL_RenderSetScale(renderer, 1.28f, 1.25f); // 800x480 -> 1024x600
    ASSERT(SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"));

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, DIM_X, DIM_Y);
    if (! texture) return 0;

    return 1;
}

void sdl2_render_close(void)
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void sdl2_render_update(uintptr_t frame)
{
	ASSERT(! SDL_UpdateTexture(texture, NULL, (PACKEDCOLORPIP_T *) frame, DIM_X * sizeof(PACKEDCOLORPIP_T)));
	ASSERT(SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"));
	SDL_Rect destRect = { 0, 0, DIM_X, DIM_Y };
	SDL_RenderCopy(renderer, texture, NULL, & destRect);
	SDL_RenderPresent(renderer);
}

#endif /* WITHSDL2VIDEO */

#endif /* LINUX_SUBSYSTEM */
