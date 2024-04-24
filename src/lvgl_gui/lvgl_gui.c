/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints
#include "display2.h"
#include "radio.h"

#if LINUX_SUBSYSTEM && WITHLVGL

#include <linux/kd.h>

#include "linux_subsystem.h"
#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lvgl_gui/styles.h"

void linux_create_thread(pthread_t * tid, void * (* process)(void * args), int priority, int cpuid);

void wfl_init(void);
uint32_t * wfl_proccess(void);

#define DISP_BUF_SIZE	(128 * DIM_X)

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

void lvgl_init(void)
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
	linux_create_thread(& p_h, thread_lv_task_handler, 50, 0);

	lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
	styles_init();
}

void lvgl_deinit(void)
{
	int ttyd = open(LINUX_TTY_FILE, O_RDWR);
    ioctl(ttyd, KDSETMODE, KD_TEXT);
    close(ttyd);
    lv_deinit();
}

static lv_img_dsc_t wfl;
static lv_obj_t * page, * lbl, * img1, * btn1, * lbl_btn1, * win;

void split_freq(uint64_t freq, uint16_t * mhz, uint16_t * khz, uint16_t * hz)
{
    * mhz = freq / 1000000;
    * khz = (freq / 1000) % 1000;
    * hz = freq % 1000;
}

void lvgl_task1_cb(lv_timer_t * tmr)
{
	uint16_t mhz, khz, hz;
	split_freq(hamradio_get_freq_a(), & mhz, & khz, & hz);
	lv_label_set_text_fmt(lbl, "%i.%03i.%03i", mhz, khz, hz);

	wfl.data = (uint8_t *) wfl_proccess();
	lv_img_set_src(img1, & wfl);
}

static void event_handler_btn1(lv_event_t * event)
{
//	lv_obj_t * btn = lv_event_get_target(event);
//	lv_obj_t * label = lv_obj_get_child(btn, 0);
	hamradio_change_preamp();
}



void footer_buttons_init(void)
{
	static lv_obj_t * fbtn[9];
	uint16_t x = 1, y = DIM_Y - 45;

	for (int i = 0; i < 9; i ++)
	{
		fbtn[i] = lv_btn_create(page);
		lv_obj_add_style(fbtn[i], & style_footer_button, 0);
		lv_obj_set_pos(fbtn[i], x, y);
		x = x + 3 + 86;
	}
}

void lvgl_test(void)
{
	page = lv_obj_create(lv_scr_act());
	lv_obj_set_size(page, DIM_X, DIM_Y);
	lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(page, & style_mainscreen, 0);

	wfl_init();

	img1 = lv_img_create(page);
	lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);

	lbl = lv_label_create(page);
	lv_obj_add_style(lbl, & style_freq_main, 0);

	pipparams_t p;
	display2_getpipparams(& p);

	lv_obj_set_size(img1, p.w, p.h);
//	lv_img_set_antialias(img1, true);
//	lv_img_set_zoom(img1, 255);

//	btn1 = lv_btn_create(page);
//	lv_obj_set_pos(btn1, 0, 420);
//	lv_obj_add_event_cb(btn1, event_handler_btn1, LV_EVENT_CLICKED, NULL);
//	lbl_btn1 = lv_label_create(btn1);
//	lv_label_set_text(lbl_btn1, "Preamp");
//	lv_obj_add_style(btn1, &style_footer_button, 0);

	footer_buttons_init();

	wfl.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
	wfl.header.always_zero = 0;
	wfl.header.reserved = 0;
	wfl.header.w = p.w;
	wfl.header.h = p.h;
	wfl.data_size = wfl.header.w * wfl.header.h * 4;

	static lv_timer_t * lvgl_task1;
	lvgl_task1 = lv_timer_create(lvgl_task1_cb, 1, NULL);
	lv_timer_set_repeat_count(lvgl_task1, -1);
}

#endif /* LINUX_SUBSYSTEM && WITHLVGL */
