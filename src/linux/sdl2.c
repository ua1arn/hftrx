/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"

#if LINUX_SUBSYSTEM

#include "linux_subsystem.h"
#include <SDL2/SDL.h>
#include <GLES3/gl32.h>

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

    // Установить атрибуты для настройки аппаратного ускорения графики
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DIM_X, DIM_Y, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        sdl2_render_close();
        return 0;
    }

    // Get OpenGL version information
    const char * glVersion = (const char *) glGetString(GL_VERSION);
    const char * glRenderer = (const char *) glGetString(GL_RENDERER);
    printf("OpenGL Version: %s\n", glVersion);
    printf("OpenGL Renderer: %s\n", glRenderer);

    SDL_RenderSetScale(renderer, 1.28f, 1.25f); 				// 800x480 -> 1024x600
    ASSERT(SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"));	// Antialiasing для масштабированных объектов

    // Текстура для отрисовки фреймбуфера
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
	// Натянуть фреймбуфер на текстуру
	ASSERT(! SDL_UpdateTexture(texture, NULL, (PACKEDCOLORPIP_T *) frame, DIM_X * sizeof(PACKEDCOLORPIP_T)));
	SDL_Rect destRect = { 0, 0, DIM_X, DIM_Y };
	SDL_RenderCopy(renderer, texture, NULL, & destRect);
	SDL_RenderPresent(renderer);
}

#endif /* WITHSDL2VIDEO */

#endif /* LINUX_SUBSYSTEM */
