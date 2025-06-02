/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if WITHLVGL

#include "formats.h"	// for debug prints
#include "display2.h"
#include "radio.h"
#include <string.h>

#if LINUX_SUBSYSTEM

#include <linux/kd.h>

#include "../linux/linux_subsystem.h"

//#include "lv_drivers/indev/evdev.h"

void linux_create_thread(pthread_t * tid, void * (* process)(void * args), int priority, int cpuid);

#include "lvgl/lvgl.h"
#include "src/lvgl_gui/styles.h"


#if WITHSDL2VIDEO

pthread_t p_inc, p_h;

void * thread_lv_tick_inc(void * p)
{
	while(1) {
		lv_tick_inc(1);
		usleep(1000);
	}

	return NULL;
}

void * thread_lv_task_handler(void * p)
{
	while(1) {
		lv_task_handler();
		usleep(10000);
	}

	return NULL;
}

//void evdev_read_cb(lv_indev_drv_t * drv, lv_indev_data_t * data)
//{
//	evdev_read(drv, data);
//}


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

void lvgl_deinit(void)
{
	int ttyd = open(LINUX_TTY_FILE, O_RDWR);
    ioctl(ttyd, KDSETMODE, KD_TEXT);
    close(ttyd);
    lv_deinit();
}

#endif /* WITHSDL2VIDEO */

#endif /* LINUX_SUBSYSTEM */

#endif /* WITHLVGL */
