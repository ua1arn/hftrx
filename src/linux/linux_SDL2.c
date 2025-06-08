/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"

#if LINUX_SUBSYSTEM && WITHSDL2VIDEO

#include "linux_subsystem.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GLES3/gl32.h>

#if WITHLVGL

#include "lvgl.h"

void lvglhw_initialize(void)
{
	lv_display_t * disp = lv_sdl_window_create(DIM_X, DIM_Y);
#if 0
	SDL_Renderer * renderer = lv_sdl_window_get_renderer(disp);
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, & display_mode);
    if ((display_mode.w > DIM_X) && (display_mode.h > DIM_Y))
    {
    	float d_x = (float) display_mode.w / DIM_X;
    	float d_y = (float) display_mode.h / DIM_Y;
    	SDL_RenderSetScale(renderer, d_x, d_y); 			// масштабирование до размеров экрана
    	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");	// Antialiasing для масштабированных объектов
    }
#endif
}

#else

void get_cursor_pos(uint16_t * x, uint16_t * y);
uint8_t check_is_mouse_present(void);

SDL_Renderer * renderer;
SDL_Window * window;
SDL_Texture * texture;
SDL_Texture * mouse_cursor;
int cursor_width, cursor_height;

int sdl2_render_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return 0;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, & display_mode);

    // Установить атрибуты для настройки аппаратного ускорения графики
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DIM_X, DIM_Y, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

    if ((display_mode.w > DIM_X) && (display_mode.h > DIM_Y))
    {
    	float d_x = (float) display_mode.w / DIM_X;
    	float d_y = (float) display_mode.h / DIM_Y;
    	SDL_RenderSetScale(renderer, d_x, d_y); 			// масштабирование до размеров экрана
    	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");	// Antialiasing для масштабированных объектов
    }

    // Текстура для отрисовки фреймбуфера
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, DIM_X, DIM_Y);
    if (! texture) return 0;

#if MOUSE_EVDEV
	int imgFlags = IMG_INIT_PNG;
	if (! (IMG_Init(imgFlags) & imgFlags)) {
		printf("Failed to initialize SDL_image: %s\n", IMG_GetError());
		SDL_Quit();
		return 0;
	}

	SDL_Surface * surface = IMG_Load(MOUSE_CURSOR_PATH);
	if (! surface) {
		printf("Failed to load image: %s\n", IMG_GetError());
		return 0;
	}

	mouse_cursor = SDL_CreateTextureFromSurface(renderer, surface);
	if (! mouse_cursor) {
		printf("Failed to create texture: %s\n", SDL_GetError());
	}

	SDL_QueryTexture(mouse_cursor, NULL, NULL, & cursor_width, & cursor_height);

	SDL_FreeSurface(surface);
#endif /* MOUSE_EVDEV */

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

#if MOUSE_EVDEV
    if (mouse_cursor && check_is_mouse_present()) {
    	uint16_t mouse_x, mouse_y;
    	get_cursor_pos(& mouse_x, & mouse_y);
        SDL_Rect png_destRect = { mouse_x, mouse_y, cursor_width, cursor_height };
        SDL_RenderCopy(renderer, mouse_cursor, NULL, & png_destRect);
    }
#endif /* MOUSE_EVDEV */

	SDL_RenderPresent(renderer);
}

#endif /* WITHLVGL */

#endif /* LINUX_SUBSYSTEM && WITHSDL2VIDEO */
