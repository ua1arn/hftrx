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

#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"

void linux_create_thread(pthread_t * tid, void * (* process)(void * args), int priority, int cpuid);

#endif /* LINUX_SUBSYSTEM */

#include "lvgl/lvgl.h"
#include "src/lvgl_gui/styles.h"



#define DISP_BUF_SIZE	(128 * DIM_X)

#if LINUX_SUBSYSTEM

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

void evdev_read_cb(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
	evdev_read(drv, data);
}


void lvgl_dev_init(void)
{
	lv_deinit();

	int ttyd = open("/dev/tty0", O_RDWR);
	if (ttyd)
		ioctl(ttyd, KDSETMODE, KD_GRAPHICS);

	close(ttyd);

	const char * argv [3] = { "/sbin/modprobe", "gt911.ko", NULL, };
	linux_run_shell_cmd(argv);
	usleep(500000);

	/*LVGL init*/
	lv_init();

	/*Linux frame buffer device init*/
	fbdev_init();

	/*A small buffer for LittlevGL to draw the screen's content*/
	static lv_color_t buf1[DISP_BUF_SIZE];
	static lv_color_t buf2[DISP_BUF_SIZE];

	/*Initialize a descriptor for the buffer*/
	static lv_disp_draw_buf_t disp_buf;
	lv_disp_draw_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);

	/*Initialize and register a display driver*/
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(& disp_drv);
	disp_drv.draw_buf   = & disp_buf;
	disp_drv.flush_cb   = fbdev_flush;
	disp_drv.hor_res    = DIM_X;
	disp_drv.ver_res    = DIM_Y;
	lv_disp_drv_register(& disp_drv);

	/* Linux input device init */
	evdev_init();

	/* Set up touchpad input device interface */
	static lv_indev_drv_t touch_drv;
	lv_indev_drv_init(& touch_drv);
	touch_drv.type = LV_INDEV_TYPE_POINTER;
	touch_drv.read_cb = evdev_read_cb;
	lv_indev_drv_register(& touch_drv);

	linux_create_thread(& p_inc, thread_lv_tick_inc, 50, 0);
	linux_create_thread(& p_h, thread_lv_task_handler, 50, 1);
}

void lvgl_deinit(void)
{
	int ttyd = open(LINUX_TTY_FILE, O_RDWR);
    ioctl(ttyd, KDSETMODE, KD_TEXT);
    close(ttyd);
    lv_deinit();
}

#endif

static lv_obj_t * main_page;

typedef struct {
	char name[30];
	int32_t payload;
} el_data_t;

lv_obj_t * button_create(lv_obj_t * parent, lv_coord_t x, lv_coord_t y, const char * name, const char * text, lv_style_t * style, void (cb)(lv_event_t *))
{
	lv_obj_t * btn = lv_btn_create(parent);
	lv_obj_set_pos(btn, x, y);
	lv_obj_add_style(btn, style, 0);
	lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, NULL);

	el_data_t * us = (el_data_t *) calloc(1, sizeof(el_data_t));
	strcpy(us->name, name);
	lv_obj_set_user_data(btn, us);

	lv_obj_t * lbl_freq = lv_label_create(btn);
	lv_label_set_text(lbl_freq, text);
	lv_obj_add_style(lbl_freq, & style_label_btn, 0);

	return btn;
}

// Приложение
void lvgl_init(void)
{
	lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
	styles_init();

	main_page = lv_obj_create(lv_scr_act());
	lv_obj_set_size(main_page, DIM_X, DIM_Y);
	lv_obj_clear_flag(main_page, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(main_page, & style_mainscreen, 0);
}

static lv_obj_t * lbl_freq, * img1_wfl, * btn1, * lbl_btn1, * win;

void split_freq(uint64_t freq, unsigned * mhz, unsigned * khz, unsigned * hz)
{
    * mhz = freq / 1000000;
    * khz = (freq / 1000) % 1000;
    * hz = freq % 1000;
}


static void event_handler_btn1(lv_event_t * event)
{
	lv_obj_t * btn = lv_event_get_target(event);
	el_data_t * us = (el_data_t *) lv_obj_get_user_data(btn);

	PRINTF("%s\n", us->name);


//	lv_obj_t * label = lv_obj_get_child(btn, 0);
//	hamradio_change_preamp();
//	TP();
//	lv_obj_del(win);
}

lv_obj_t * window_create(lv_obj_t * parent, lv_coord_t w, lv_coord_t h)
{
	lv_obj_t * const win = lv_obj_create(parent);
	lv_obj_add_style(win, & style_window, 0);
	lv_obj_set_size(win, w, h);

	return win;
}

static lv_obj_t * fbtn [9];

static void footer_buttons_init(void)
{
	uint16_t x = 1, y = DIM_Y - 45;

	for (int i = 0; i < 9; i ++)
	{
		char b[30];
		snprintf(b, ARRAY_SIZE(b), "btn_footer%d", i);
		fbtn[i] = button_create(main_page, x, y, b, b, & style_footer_button, event_handler_btn1);
		x = x + 3 + 86;
	}
}

void lvgl_task1_cb(lv_timer_t * tmr)
{
	if (lbl_freq)
	{
		unsigned mhz, khz, hz;
		split_freq(hamradio_get_freq_a(), & mhz, & khz, & hz);
		lv_label_set_text_fmt(lbl_freq, "%u.%03u.%03u", mhz, khz, hz);
	}
	if (img1_wfl)
	{
		wfl_proccess();
		lv_obj_invalidate(img1_wfl);
	}
}

// application
void lvgl_test(void)
{
	wfl_init();


	lbl_freq = lv_label_create(main_page);
	lv_obj_add_style(lbl_freq, & style_freq_main, 0);

	img1_wfl = lv_img_create(main_page);
	//lv_obj_align(img1_wfl, LV_ALIGN_CENTER, 0, 0);

	pipparams_t p;
	display2_getpipparams(& p);

	lv_obj_set_size(img1_wfl, p.w, p.h);
	lv_obj_set_pos(img1_wfl, p.x, p.y);
//	lv_img_set_antialias(img1_wfl, true);
//	lv_img_set_zoom(img1_wfl, 255);
	lv_img_set_src(img1_wfl, wfl_proccess());	// src_type=LV_IMAGE_SRC_VARIABLE

	footer_buttons_init();

//	win = window_create(main_page, 300, 200);
//	lv_obj_t * btn = button_create(win, 0, 0, "Preamp", & style_footer_button, event_handler_btn1);

	static lv_timer_t * lvgl_task1;
	lvgl_task1 = lv_timer_create(lvgl_task1_cb, 1, NULL);
	lv_timer_set_repeat_count(lvgl_task1, -1);
}

#endif /* WITHLVGL */
