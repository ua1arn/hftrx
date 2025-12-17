/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Доработки для LS020 Василий Линывый, livas60@mail.ru
//

#include "hardware.h"
#include "board.h"
#include "display.h"
#include "formats.h"
#include "spi.h"	// hardware_spi_master_send_frame
#include "display2.h"
#include <string.h>


const char * savestring = "no data";
const char * savewhere = "no func";

#if LCDMODE_LTDC

#include "fontmaps.h"

/* рисование линии на основном экране произвольным цветом
*/
void
display_line(const gxdrawb_t * db,
	int x1, int y1,
	int x2, int y2,
	COLORPIP_T color
	)
{

	colpip_line(db, x1, y1, x2, y2, color, 0);
}


/* индивидуальные функции драйвера дисплея - реализованы в соответствующем из файлов */
// Заполниить цветом фона
void display_clear(const gxdrawb_t * db)
{
	const COLORPIP_T bg = display2_getbgcolor();

	colpip_fillrect(db, 0, 0, DIM_X, DIM_Y, bg);
}

void display_bar(
	const gxdrawb_t * db,
	uint_fast16_t x,
	uint_fast16_t y,
	uint_fast8_t width,	/* количество знакомест, занимаемых индикатором */
	uint_fast8_t rowspan,	/* количество ячеек по вертикали, занимаемых индикатором */
	int_fast16_t value,		/* значение, которое надо отобразить */
	int_fast16_t tracevalue,		/* значение маркера, которое надо отобразить */
	int_fast16_t topvalue,	/* значение, соответствующее полностью заполненному индикатору */
	uint_fast8_t vpattern,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL */
	uint_fast8_t patternmax,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL - для отображения запомненного значения */
	uint_fast8_t emptyp,			/* паттерн для заполнения между штрихами */
	const gxstyle_t * dbstyle		/* foreground and background colors, text alignment */
	)
{
	ASSERT(value <= topvalue);
	ASSERT(tracevalue <= topvalue);
	const uint_fast16_t wfull = GRID2X(width);
	const uint_fast16_t h = GRID2Y(rowspan);
	const uint_fast16_t wpart = (int_fast32_t) wfull * value / topvalue;
	const uint_fast16_t wmark = (int_fast32_t) wfull * tracevalue / topvalue;
	const uint_fast8_t hpattern = 0x33;

	colpip_fillrect(db, 	x, y, 			wpart, h, 			dbstyle->textcolor);
	colpip_fillrect(db, 	x + wpart, y, 	wfull - wpart, h, 	dbstyle->bgcolor);
	if (wmark < wfull && wmark >= wpart)
		colpip_fillrect(db, x + wmark, y, 	1, h, 				dbstyle->textcolor);
}

#if 1//! WITHLVGL


// Выдать один цветной пиксель (фон/символ)
static void
ltdc_pixel(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	uint_fast8_t v,			// 0 - цвет background, иначе - foreground
	const gxstyle_t * dbstyle	/* foreground and background colors, text alignment */
	)
{
	PACKEDCOLORPIP_T * const tgr = colpip_mem_at(db, x, y);
	* tgr = v ? dbstyle->textcolor : dbstyle->bgcolor;
}


#endif /* ! WITHLVGL */


/* аппаратный сброс дисплея - перед инициализаций */
void
display_reset(void)
{
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(1); // Delay 1ms
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(10); // Delay 10ms
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(50); // Delay 50 ms
}

void display_initialize(void)
{
#if WITHLVGL
	display_lvgl_initialize();
#endif /* WITHLVGL */
}

/* Разряжаем конденсаторы питания */
void display_uninitialize(void)
{
}


#if WITHLVGL //&& ! LINUX_SUBSYSTEM

#include "lvgl.h"
//#include "../demos/lv_demos.h"
//#include "../demos/vector_graphic/lv_demo_vector_graphic.h"
//#include "src/lvgl_gui/styles.h"

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/

#if defined (RTMIXIDLCD)

static void maindisplay_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
	if (lv_display_is_double_buffered(disp) && lv_display_flush_is_last(disp))
	{
    	dcache_clean(
    		(uintptr_t) px_map,
    		lv_color_format_get_size(lv_display_get_color_format(disp)) * GXSIZE(lv_display_get_horizontal_resolution(disp), lv_display_get_vertical_resolution(disp))
    		);
    	hardware_ltdc_main_set(RTMIXIDLCD, (uintptr_t) px_map);	/* set visible buffer start. Wait VSYNC. */
#if WITHHDMITVHW && defined (RTMIXIDTV) && 1
    	// Дубль жкрана
    	hardware_ltdc_main_set(RTMIXIDTV, (uintptr_t) px_map);	/* set visible buffer start. Wait VSYNC. */
#endif
#if WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC
    	{
    		gxdrawb_t dbv;
    		//gxdrawb_initlvgl(& dbv, lv_display_get_layer_top(disp));
    		gxdrawb_initialize(& dbv, (PACKEDCOLORPIP_T *) px_map, lv_display_get_horizontal_resolution(disp), lv_display_get_vertical_resolution(disp));
    		display_snapshot(& dbv);	/* запись видимого изображения */
    	}
#endif /* WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC */
    }
	/*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_display_flush_ready(disp);
}

#endif /* defined (RTMIXIDLCD) */

#if defined (RTMIXIDTV)

static void subdisplay_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
	if (lv_display_is_double_buffered(disp) && lv_display_flush_is_last(disp))
	{
    	dcache_clean(
    		(uintptr_t) px_map,
    		lv_color_format_get_size(lv_display_get_color_format(disp)) * GXSIZE(lv_display_get_horizontal_resolution(disp), lv_display_get_vertical_resolution(disp))
    		);
    	hardware_ltdc_main_set(RTMIXIDTV, (uintptr_t) px_map);	/* set visible buffer start. Wait VSYNC. */
    }
	/*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_display_flush_ready(disp);
}

#endif /* defined (RTMIXIDTV) */

static uint32_t myhardgeticks(void)
{
	return sys_now();
}

struct driverdata
{
	int rtmixid;
};

// Инициадизация нужного количества дисплеев
void display_lvgl_initialize(void)
{
	lv_init();	// LVGL library initialize

#if defined (RTMIXIDLCD)
	if (1)
	{
		static struct driverdata maindisplay =
		{
				.rtmixid = RTMIXIDLCD
		};
		// main display
	    lv_display_t * disp = lv_display_create(DIM_X, DIM_Y);
	    lv_display_set_driver_data(disp, & maindisplay);
	    lv_display_set_flush_cb(disp, maindisplay_flush);

	    static LV_ATTRIBUTE_MEM_ALIGN RAMFRAMEBUFF uint8_t fb1 [GXSIZE(DIM_X, DIM_Y) * LCDMODE_PIXELSIZE];
	    static LV_ATTRIBUTE_MEM_ALIGN RAMFRAMEBUFF uint8_t fb2 [GXSIZE(DIM_X, DIM_Y) * LCDMODE_PIXELSIZE];

	    //LV_DRAW_BUF_DEFINE_STATIC(dbuf_3_3, DIM_X, DIM_Y, LV_COLOR_FORMAT_ARGB8888);

	    lv_display_set_buffers_with_stride(
	    		disp,
				fb1, fb2, sizeof(fb1),
	    		GXADJ(DIM_X) * LCDMODE_PIXELSIZE,
				LV_DISPLAY_RENDER_MODE_DIRECT);
	    //lv_display_set_3rd_draw_buffer(disp, & dbuf_3_3);
	    lv_display_set_color_format(disp, (lv_color_format_t) display_get_lvformat());
	    lv_display_set_antialiasing(disp, false);

	    lv_obj_t * const obj = lv_label_create(lv_display_get_screen_active(disp));
	    lv_label_set_text_static(obj, "LCD/LVDS display");

	    lv_display_set_default(disp);
	}
#endif /* defined (RTMIXIDTV) */

#if WITHHDMITVHW && defined (RTMIXIDTV) && 0
	if (1)
	{
		static struct driverdata subdisplay =
		{
				.rtmixid = RTMIXIDTV
		};
		// tv display
	    lv_display_t * disp = lv_display_create(DIM_X, DIM_Y);
	    lv_display_set_driver_data(disp, & subdisplay);
	    lv_display_set_flush_cb(disp, subdisplay_flush);

	    static LV_ATTRIBUTE_MEM_ALIGN RAMFRAMEBUFF uint8_t fb1 [GXSIZE(TVD_WIDTH, TVD_HEIGHT) * TVMODE_PIXELSIZE];
	    static LV_ATTRIBUTE_MEM_ALIGN RAMFRAMEBUFF uint8_t fb2 [GXSIZE(TVD_WIDTH, TVD_HEIGHT) * TVMODE_PIXELSIZE];

	    //LV_DRAW_BUF_DEFINE_STATIC(dbuf_3_3, DIM_X, DIM_Y, LV_COLOR_FORMAT_ARGB8888);

	    lv_display_set_buffers_with_stride(
	    		disp,
				fb1, fb2, sizeof(fb1),
    		GXADJ(DIM_X) * TVMODE_PIXELSIZE,
				LV_DISPLAY_RENDER_MODE_DIRECT);
	    //lv_display_set_3rd_draw_buffer(disp, & dbuf_3_3);
	    lv_display_set_color_format(disp, (lv_color_format_t) display_get_lvformat());
	    lv_display_set_antialiasing(disp, false);

	    lv_obj_t * const obj = lv_label_create(lv_display_get_screen_active(disp));
	    lv_label_set_text_static(obj, "HDMI display");

	    //lv_display_set_default(disp);
	}
#endif /* WITHHDMITVHW && defined (RTMIXIDTV) */

	// Add custom draw unit
	lvglhw_initialize();

	// lvgl будет получать тики
	lv_tick_set_cb(myhardgeticks);
}

void gxdrawb_initlvgl(gxdrawb_t * db, void * layerv)
{
	lv_layer_t * layer = (lv_layer_t *) layerv;
	gxdrawb_initialize(db,
			(PACKEDCOLORPIP_T *) lv_draw_buf_goto_xy(layer->draw_buf, 0, 0),
			layer->draw_buf->header.w, layer->draw_buf->header.h);
	db->layerv = layerv;
	db->cachebase = (uintptr_t) layer->draw_buf->data;
	db->cachesize = layer->draw_buf->data_size;
	db->stride = layer->draw_buf->header.stride;
}


#endif /* WITHLVGL //&& ! LINUX_SUBSYSTEM */

// Используется при выводе на графический индикатор с кординатами и размерами в пикселях
// Многострочное отображение
void pix_display_texts(const gxdrawb_t * db, uint_fast16_t xpixB, uint_fast16_t ypix, uint_fast16_t w, uint_fast16_t h, const gxstyle_t * dbstylep, const char * const * slines, unsigned nlines)
{
	size_t len;
	const unifont_t * const font = dbstylep->font;

	savewhere = __func__;
#if ! WITHLVGL
	if (font == NULL)
		return;
	if (dbstylep->bgradius)
	{
		w -= dbstylep->bgbackoffw;
		h -= dbstylep->bgbackoffh;
	}
#endif
	ASSERT3(w >= (dbstylep->bgradius * 2), __FILE__, __LINE__, slines [0]);
	ASSERT3(h >= (dbstylep->bgradius * 2), __FILE__, __LINE__, slines [0]);
	const uint_fast16_t avlw = w - (dbstylep->bgradius * 2);
	const uint_fast16_t avlh = h - (dbstylep->bgradius * 2);
	colmain_rounded_rect(db, xpixB, ypix, xpixB + w - 1, ypix + h - 1, dbstylep->bgradius, dbstylep->bgcolor, dbstylep->bgfilled);
	ypix += dbstylep->bgradius;
	const uint_fast16_t vstep = avlh / nlines;
	for (; nlines --; ypix += vstep)
	{
		uint_fast16_t xpix = xpixB + dbstylep->bgradius;
		const char * s = * slines ++;
#if WITHLVGL
		lv_layer_t * const layer = (lv_layer_t *) db->layerv;
		if (layer)
		{
			//PRINTF("x/y=%d/%d '%s'\n", xpix, ypix, s);
			lv_draw_rect_dsc_t d;
			lv_draw_label_dsc_t l;
			lv_area_t coords;
			lv_draw_label_dsc_init(& l);
			lv_draw_rect_dsc_init(& d);
			lv_area_set(& coords, xpix, ypix, xpix + w - 1, ypix + h - 1);
			d.bg_color = display_lvlcolor(dbstylep->bgcolor);
			l.color = display_lvlcolor(dbstylep->textcolor);
			l.align = LV_TEXT_ALIGN_RIGHT;
			l.flag = 0*LV_TEXT_FLAG_EXPAND | LV_TEXT_FLAG_FIT;
			l.text = s;
			l.font = & Epson_LTDC_small;
			//PRINTF("display_string: x/y=%d/%d '%s'\n", (int) xpix, (int) xpix, s);
			lv_draw_rect(layer, & d, & coords);
			lv_draw_label(layer, & l, & coords);

			continue;
		}
#endif

//		ASSERT(dbstylep->font_height);
//		ASSERT(dbstylep->font_draw_char);
//		ASSERT(dbstylep->font_width);
		char c;
		savestring = s;
		uint_fast16_t stringheight;
		const uint_fast16_t stringwidth = colpip_string_widthheight(font, s, & stringheight);
		ASSERT(font);
		switch (dbstylep->textvalign)
		{
		default:
		case GXSTYLE_VALIGN_CENTER:
			if (vstep > stringheight)
				ypix += (vstep - stringheight) / 2;
			break;
		case GXSTYLE_VALIGN_TOP:
			break;

		case GXSTYLE_VALIGN_BOTTOM:
			if (vstep > stringheight)
				ypix += (vstep - stringheight);
			break;
		}

		const uint_fast16_t textw = ulmin16(avlw, stringwidth);
		const uint_fast16_t xpix0 = xpix;
		//ASSERT3(avlw >= textw, __FILE__, __LINE__, s);
		switch (dbstylep->texthalign)
		{
		default:
		case GXSTYLE_HALIGN_RIGHT:
			xpix = textw < w ? xpix + avlw - textw : xpix;
			while ((c = * s ++) != '\0' && xpix - xpix0 + font->font_charwidth(font, c) <= avlw)
				xpix = font->font_draw(db, xpix, ypix, font, c, dbstylep->textcolor);
			break;
		case GXSTYLE_HALIGN_LEFT:
			while ((c = * s ++) != '\0' && xpix - xpix0 + font->font_charwidth(font, c) <= avlw)
				xpix = font->font_draw(db, xpix, ypix, font, c, dbstylep->textcolor);
			break;
		case GXSTYLE_HALIGN_CENTER:
			// todo: to be implemented
			break;
		}
	}

}

static const FLASHMEM int32_t vals10 [] =
{
	INT32_C(1000000000),
	INT32_C(100000000),
	INT32_C(10000000),
	INT32_C(1000000),
	INT32_C(100000),
	INT32_C(10000),
	INT32_C(1000),
	INT32_C(100),
	INT32_C(10),
	INT32_C(1),
};


// Отображение цифр в поле "больших цифр" - индикатор основной частоты настройки аппарата.
void
pix_display_value_big(
	const gxdrawb_t * db,
	uint_fast16_t xpix,	// x координата начала вывода значения
	uint_fast16_t ypix,	// y координата начала вывода значения
	uint_fast16_t w,
	uint_fast16_t h,
	int_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t comma2,	// = comma + 3;		// comma position (from right, inside width)
	uint_fast8_t rj,	// = 1;		// right truncated
	uint_fast8_t blinkpos,		// позиция, где символ заменён пробелом
	uint_fast8_t blinkstate,	// 0 - пробел, 1 - курсор
	uint_fast8_t withhalf,		// 0 - только большие цифры
	const gxstyle_t * dbstyle	/* foreground and background colors, text alignment */
	)
{
	//	if (width > ARRAY_SIZE(vals10))
	//		width = ARRAY_SIZE(vals10);
		//const uint_fast8_t comma2 = comma + 3;		// comma position (from right, inside width)
	const uint_fast8_t j = ARRAY_SIZE(vals10) - rj;
	uint_fast8_t i = (j - width);
	uint_fast8_t z = blinkpos == 255 ? 1 : 0;	// only zeroes
	uint_fast8_t half = 0;	// отображаем после второй запатой - маленьким шрифтом
	for (; i < j; ++ i)
	{
		const ldiv_t res = ldiv(freq, vals10 [i]);
		const uint_fast8_t g = (j - i);		// десятичная степень текущего разряда на отображении

		// разделитель десятков мегагерц
		if (comma2 == g)
		{
			xpix = display_put_char_big(db, xpix, ypix, (z == 0) ? '.' : '#', dbstyle);	// '#' - узкий пробел. Точка всегда узкая
		}
		else if (comma == g)
		{
			z = 0;
			half = withhalf;
			xpix = display_put_char_big(db, xpix, ypix, '.', dbstyle);
		}

		if (blinkpos == g)
		{
			const uint_fast8_t bc = blinkstate ? '_' : ' ';
			// эта позиция редактирования частоты. Справа от неё включаем все нули
			z = 0;
			if (half)
				xpix = display_put_char_half(db, xpix, ypix, bc, dbstyle);
			else
				xpix = display_put_char_big(db, xpix, ypix, bc, dbstyle);
		}
		else if (z == 1 && (i + 1) < j && res.quot == 0)
			xpix = display_put_char_big(db, xpix, ypix, ' ', dbstyle);	// supress zero
		else
		{
			z = 0;
			if (half)
				xpix = display_put_char_half(db, xpix, ypix, '0' + res.quot, dbstyle);
			else
				xpix = display_put_char_big(db, xpix, ypix, '0' + res.quot, dbstyle);
		}
		freq = res.rem;
	}
}

// Отображение цифр в поле "больших цифр" - индикатор основной частоты настройки аппарата.
void
display_freq(
	const gxdrawb_t * db,
	uint_fast8_t xcell,	// x координата начала вывода значения
	uint_fast8_t ycell,	// y координата начала вывода значения
	uint_fast8_t xspan,
	uint_fast8_t yspan,
	int_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t comma2,	// = comma + 3;		// comma position (from right, inside width)
	uint_fast8_t rj,	// = 1;		// right truncated
	uint_fast8_t blinkpos,		// позиция, где символ заменён пробелом
	uint_fast8_t blinkstate,	// 0 - пробел, 1 - курсор
	uint_fast8_t withhalf,		// 0 - только большие цифры
	const gxstyle_t * dbstylep	/* foreground and background colors, text alignment */
	)
{

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdata_begin(xcell, ycell, & ypix);
	pix_display_value_big(db, xpix, ypix, GRID2X(xspan), GRID2Y(yspan), freq, width, comma, comma2, rj, blinkpos, blinkstate, withhalf, dbstylep);
}

void
NOINLINEAT
pix_display_value_small(
	const gxdrawb_t * db,
	uint_fast16_t xpix,	// x координата начала вывода значения
	uint_fast16_t ypix,	// y координата начала вывода значения
	uint_fast16_t w,
	uint_fast16_t h,
	int_fast32_t freq,
	uint_fast8_t width,	// full width (if >= 128 - display with sign)
	uint_fast8_t comma,		// comma position (from right, inside width)
	uint_fast8_t comma2,
	uint_fast8_t rj,		// right truncated
	const gxstyle_t * dbstylep	/* foreground and background colors, text alignment */
	)
{
	const unifont_t * const font = dbstylep->font;
//	if (width > ARRAY_SIZE(vals10))
//		width = ARRAY_SIZE(vals10);
	const uint_fast8_t wsign = (width & WSIGNFLAG) != 0;
	const uint_fast8_t wminus = (width & WMINUSFLAG) != 0;
	const uint_fast8_t j = ARRAY_SIZE(vals10) - rj;
	uint_fast8_t i = j - (width & WWIDTHFLAG);	// Номер цифры по порядку
	uint_fast8_t z = 1;	// если в позиции встретился '0' - не отоображать

	if (dbstylep->bgradius)
	{
		w -= dbstylep->bgbackoffw;
		h -= dbstylep->bgbackoffh;
	}

	const uint_fast16_t avlw = w - (dbstylep->bgradius * 2);
	const uint_fast16_t avlh = h - (dbstylep->bgradius * 2);
	colmain_rounded_rect(db, xpix, ypix, xpix + w - 1, ypix + h - 1, dbstylep->bgradius, dbstylep->bgcolor, dbstylep->bgfilled);
	xpix += dbstylep->bgradius;
	ypix += dbstylep->bgradius;
	const uint_fast16_t stringheight = font->font_charheight(font);

	if (avlh > stringheight)
	{
		ypix += (avlh - stringheight) / 2;
	}
	if (wsign || wminus)
	{
		// отображение со знаком.
		z = 0;
		if (freq < 0)
		{
			xpix = display_put_char(db, xpix, ypix, '-', dbstylep);
			freq = - freq;
		}
		else if (wsign)
			xpix = display_put_char(db, xpix, ypix, '+', dbstylep);
		else
			xpix = display_put_char(db, xpix, ypix, ' ', dbstylep);
	}
	for (; i < j; ++ i)
	{
		const ldiv_t res = ldiv(freq, vals10 [i]);
		const uint_fast8_t g = (j - i);
		// разделитель десятков мегагерц
		if (comma2 == g)
		{
			xpix = display_put_char(db, xpix, ypix, (z == 0) ? '.' : ' ', dbstylep);
		}
		else if (comma == g)
		{
			z = 0;
			xpix = display_put_char(db, xpix, ypix, '.', dbstylep);
		}

		if (z == 1 && (i + 1) < j && res.quot == 0)
			xpix = display_put_char(db, xpix, ypix, ' ', dbstylep);	// supress zero
		else
		{
			z = 0;
			xpix = display_put_char(db, xpix, ypix, '0' + res.quot, dbstylep);
		}
		freq = res.rem;
	}
}

#endif /* LCDMODE_LTDC */

#if LCDMODE_LTDC

const int sin90 [91] =
{
	0, 175, 349, 523, 698, 872,1045,1219,1392,   /*  0..8        */
	1564,1736,1908,2079,2250,2419,2588,2756,2924,   /*  9..17       */
	3090,3256,3420,3584,3746,3907,4067,4226,4384,   /* 18..26       */
	4540,4695,4848,5000,5150,5299,5446,5592,5736,
	5878,6018,6157,6293,6428,6561,6691,6820,6947,
	7071,7193,7314,7431,7547,7660,7771,7880,7986,
	8090,8192,8290,8387,8480,8572,8660,8746,8829,
	8910,8988,9063,9135,9205,9272,9336,9397,9455,
	9511,9563,9613,9659,9703,9744,9781,9816,9848,
	9877,9903,9925,9945,9962,9976,9986,9994,9998,   /* 81..89       */
	10000                                           /* 90           */
};

static int muldiv(int a, int b, unsigned c)
{
	return  (unsigned) ((a * (long) b + (c / 2)) / c);
}

static int isin(unsigned alpha, unsigned r)
{
	while (alpha >= 360)
		alpha -= 360;

	if (alpha < 90)         /* 0..3 hours   */
		return muldiv(sin90 [ alpha ], r, 10000);
	if (alpha < 180)        /* 9..0 hours   */
		return muldiv(sin90 [ 180 - alpha ], r, 10000);
	if (alpha < 270)        /* 6..9 hours   */
		return - muldiv(sin90 [ alpha - 180], r, 10000);
				/* 3..6 hours   */
	return - muldiv(sin90 [ 360 - alpha ], r, 10000);
}

static  int icos(unsigned alpha, unsigned r)
{
	return isin(alpha + 90, r);
}

// Рисование радиусов
void
colpip_radius(
	const gxdrawb_t * db,
	int xc, int yc,
	unsigned gs,
	unsigned r1, unsigned r2,
	COLORPIP_T color,
	int antialiasing,
	int style)			// 1 - растягивание по горизонтали
{
	int     x, y;
	int     x2, y2;

	x = xc + icos(gs, style ? r1 << 1 : r1);
	y = yc + isin(gs, r1);
	x2 = xc + icos(gs, style ? r2 << 1 : r2);
	y2 = yc + isin(gs, r2);

	colpip_line(db, x, y, x2, y2, color, antialiasing);
}

void
polar_to_dek(
		uint_fast16_t xc,
		uint_fast16_t yc,
		uint_fast16_t gs,
		uint_fast16_t r,
		uint_fast16_t * x,
		uint_fast16_t * y,
		uint_fast8_t style)
{
	* x = xc + icos(gs, style ? r << 1 : r);
	* y = yc + isin(gs, r);
}

// круговой интерполятор
// нач.-x, нач.-y, градус начала, градус конуа, радиус, шаг приращения угла
void
colpip_segm(
	const gxdrawb_t * db,
	int xc, int yc,
	unsigned gs, unsigned ge,
	unsigned r, int step,
	COLORPIP_T color,
	int antialiasing,
	int style)			// не-0: x2 растягивание по горизонтали
{
	int     x, y;
	int     xo, yo;
	char     first;
	int     vcos, vsin;

	if (gs == ge) return;

	// Даёт артефакты с использованием flood fill
	if (r < 20) step = 15;

	first = 1;
	while (gs != ge)
	{
		vsin = isin(gs, r);
		vcos = icos(gs, style ? r * 2 : r);
		x = xc + vcos;
		y = yc + vsin;

		if (first != 0) // 1-я точка
		{
			// переместить к началу рисования
			xo = x, yo = y;
			first = 0;
		}
		else
		{  // рисовать элемент окружности
			colpip_line(db, xo, yo, x, y, color, antialiasing);
			xo = x, yo = y;
		}
		gs = ((gs + 360) + step) % 360;
		if (ge == 360)
			ge = 0;
	}

	if (first == 0)
	{
		// завершение окружности
		vsin = isin(ge, r);
		vcos = icos(ge, style ? r * 2 : r);
		x = xc + vcos;
		y = yc + vsin;

		colpip_line(db, xo, yo, x, y, color, antialiasing); // рисовать линию
	}
}

/* Нарисовать прямоугольник со скругленными углами */
void colmain_rounded_rect(
	const gxdrawb_t * db,
	uint_fast16_t x1,
	uint_fast16_t y1,
	uint_fast16_t x2,
	uint_fast16_t y2,
	uint_fast8_t r,		// радиус закругления углов
	COLORPIP_T color,
	uint_fast8_t fill
	)
{
	if (r == 0)
	{
		colpip_rect(db, x1, y1, x2, y2, color, fill);
		return;
	}

	ASSERT((r * 2) < (x2 - x1));
	ASSERT((r * 2) < (y2 - y1));

#if CPUSTYLE_ALLWINNER && ! LINUX_SUBSYSTEM

	const uint_fast16_t wpartial = (x2 - x1 + 1 + 1) / 2;
	const uint_fast16_t hpartial = (y2 - y1 + 1 + 1) / 2;

	if (wpartial == 0 || hpartial == 0)
		return;
	// Использование аппаартного копирования при построении
	// Рисуем левую верхнюю левую четверть
	colpip_segm(db, x1 + r, y1 + r, 180, 270, r, 1, color, 1, 0); // up left
	colpip_set_hline(db, x1 + r, y1, wpartial - r, color); // top
	colpip_set_vline(db, x1, y1 + r, hpartial - r, color); // left
	colpip_set_hline(db, x1, y1 + hpartial, wpartial, color); // horisontal center
	colpip_set_vline(db, x1 + wpartial, y1, hpartial, color); // vertical center
	if (fill)
	{
		PACKEDCOLORPIP_T * const oldColor = colpip_mem_at(db, x1 + r, y1 + r);
		display_floodfill(db, x1 + r, y1 + r, color, * oldColor);
	}
	// Копируем левый верхний угол в левый нижний
	colpip_bitblt(
			db->cachebase, db->cachesize,
			db,
			x1, y2 - hpartial,
			db->cachebase, db->cachesize,
			db,
			x1, y1, wpartial, hpartial,
			BITBLT_FLAG_YMIRROR, 0);
	// Копируем левую половину в правую
	colpip_bitblt(
			db->cachebase, 0*db->cachesize,
			db,
			x2 - wpartial, y1,
			db->cachebase, 0*db->cachesize,
			db,
			x1, y1, wpartial, y2 - y1 + 1,	// часть по ширине, всю по высоте
			BITBLT_FLAG_XMIRROR, 0);

#else
	colpip_segm(db, x1 + r, y1 + r, 180, 270, r, 1, color, 1, 0); // up left
	colpip_segm(db, x2 - r, y1 + r, 270, 360, r, 1, color, 1, 0); // up right
	colpip_segm(db, x2 - r, y2 - r,   0,  90, r, 1, color, 1, 0); // down right
	colpip_segm(db, x1 + r, y2 - r,  90, 180, r, 1, color, 1, 0); // down left

	colpip_line(db, x1 + r, y1, x2 - r, y1, color, 0); // up
	colpip_line(db, x1, y1 + r, x1, y2 - r, color, 0); // left
	colpip_line(db, x1 + r, y2, x2 - r, y2, color, 0); // down
	colpip_line(db, x2, y1 + r, x2, y2 - r, color, 0); // right

	if (fill)
	{
		PACKEDCOLORPIP_T * const oldColor = colpip_mem_at(db, x1 + r, y1 + r);
		display_floodfill(db, x1 + (x2 - x1) / 2, y1 + r, color, * oldColor);
	}
#endif
}


void gxdrawb_initialize(gxdrawb_t * db, PACKEDCOLORPIP_T * buffer, uint_fast16_t dx, uint_fast16_t dy)
{
	ASSERT(buffer);
	db->buffer = buffer;
	db->dx = dx;
	db->dy = dy;
	db->cachebase = (uintptr_t) buffer;
	db->cachesize = GXSIZE(dx, dy) * sizeof (PACKEDCOLORPIP_T);
	db->stride = GXADJ(dx) * sizeof (PACKEDCOLORPIP_T);
	db->layerv = NULL;
}

#endif /* LCDMODE_LTDC */

#if WITHLTDCHW || 1

#if LCDMODE_LQ043T3DX02K
	// Sony PSP-1000 display panel
	// LQ043T3DX02K panel (272*480)
	// RK043FN48H-CT672B  panel (272*480) - плата STM32F746G-DISCO
	/**
	  * @brief  RK043FN48H Size
	  */
static const videomode_t vdmode0 =
{
	.width = 480,				/* LCD PIXEL WIDTH            */
	.height = 272,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  RK043FN48H Timing
	  */
	.hsync = 41,				/* Horizontal synchronization */
	.hbp = 2,				/* Horizontal back porch      */
	.hfp = 2,				/* Horizontal front porch     */

	.vsync = 10,				/* Vertical synchronization   */
	.vbp = 2,					/* Vertical back porch        */
	.vfp = 4,					/* Vertical front porch       */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 1,		/* требуется формирование сигнала RESET для панели по этому выводу после начала формирования синхронизации */
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};
	/* SONY PSP-1000 display (4.3") required. */
	/* Используется при BOARD_DEMODE = 0 */
	//#define BOARD_DERESET 1		/* требуется формирование сигнала RESET для панели по этому выводу после начала формирования синхронизации */

#elif LCDMODE_AT070TN90

/* AT070TN90 panel (800*480) - 7" display HV mode */
static const videomode_t vdmode0 =
{
	.width = 800,			/* LCD PIXEL WIDTH            */
	.height = 480,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 40,				/* Horizontal synchronization 1..40 */
	.hbp = 6,				/* Horizontal back porch      */
	.hfp = 210,				/* Horizontal front porch  16..354   */

	.vsync = 20,				/* Vertical synchronization 1..20  */
	.vbp = 3,				/* Vertical back porch      */
	.vfp = 22,				/* Vertical front porch  7..147     */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif 1 && LCDMODE_AT070TNA2

/* AT070TNA2 panel (1024*600) - 7" display HV mode */
// HX8282-A01.pdf, page 38
static const videomode_t vdmode0 =
{
	.width = 1024,			/* LCD PIXEL WIDTH            */
	.height = 600,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	/* from r6dan: thb+thpw=160 is fixed */
	/* EK79001.PDF: */
	.hsync = 20,			/* Horizontal synchronization 1..140 */
	.hbp = 140,				/* Horizontal back porch */
	.hfp = 160,				/* Horizontal front porch  16..216  (r6dan: 140-160-180)  */

	/* from r6dan: tvb+tvpw=23 is fixed */
	.vsync = 3,				/* Vertical synchronization 1..20 */
	.vbp = 20,				/* Vertical back porch */
	.vfp = 12,				/* Vertical front porch  1..127  (r6dan: 2-12-22) */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_LQ123K3LG01

/* LQ123K3LG01 panel (1280*480) - 12.3" display LVDS mode */
static const videomode_t vdmode0 =
{
	.width = 1280,			/* LCD PIXEL WIDTH            */
	.height = 480,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  LQ123K3LG01 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 6,			/* Horizontal synchronization 1..40 */
	.hbp = 39,				/* Horizontal back porch      */
	.hfp = 368,				/* Horizontal front porch  16..354   */

	.vsync = 1,				/* Vertical synchronization 1..20  */
	.vbp = 24,			/* Vertical back porch      */
	.vfp = 15,				/* Vertical front porch  7..147     */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_TCG104XGLPAPNN

/* TCG104XGLPAPNN-AN30 panel (1024*768) - 10.4" display */
// TCG104XGLPAPNN-AN30-1384899.pdf
// horizontal period 1114 / 1344 / 1400
// vertical period 778 / 806 / 845
// Synchronization method should be DE mode
static const videomode_t vdmode0 =
{
	.width = 1024,			/* LCD PIXEL WIDTH            */
	.height = 768,			/* LCD PIXEL HEIGHT           */

	.hsync = 120,			/* Horizontal synchronization 1..140 */
	.hbp = 100,				/* Horizontal back porch  xxx   */
	.hfp = 100,				/* Horizontal front porch  16..216   */

	.vsync = 20,				/* Vertical synchronization 1..20  */
	.vbp = 9,				/* Vertical back porch  xxx   */
	.vfp = 9,				/* Vertical front porch  1..127     */

	// Synchronization method should be DE mode
	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	.fps = 60,	/* frames per second 50 60 70 */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_ILI8961
	// HHT270C-8961-6A6 (320*240)
static const videomode_t vdmode0 =
{
	.width = 320 * 3,				/* LCD PIXEL WIDTH            */
	.height = 240,			/* LCD PIXEL HEIGHT           */

	/**
	  * @brief  RK043FN48H Timing
	  */
	.hsync = 1,				/* Horizontal synchronization */
	.hbp = 2,				/* Horizontal back porch      */
	.hfp = 2,				/* Horizontal front porch     */

	.vsync = 1,				/* Vertical synchronization   */
	.vbp = 2,					/* Vertical back porch        */
	.vfp = 2,					/* Vertical front porch       */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_ILI9341
	// SF-TC240T-9370-T (320*240)
static static const const videomode_t vdmode0 =
{

	.width = 240,				/* LCD PIXEL WIDTH            */
	.height = 320,			/* LCD PIXEL HEIGHT           */

	/**
	  * @brief  ILI9341 Timing
	  */
	.hsync = 10,				/* Horizontal synchronization */
	.hbp = 20,				/* Horizontal back porch      */
	.hfp = 10,				/* Horizontal front porch     */

	.vsync = 2,				/* Vertical synchronization   */
	.vbp = 2,					/* Vertical back porch        */
	.vfp = 4,					/* Vertical front porch       */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_H497TLB01P4
	/* 720xRGBx1280 - 5" AMOELD Panel H497TLB01.4 */
	// See also:
	// https://github.com/bbelos/rk3188-kernel/blob/master/drivers/video/rockchip/transmitter/tc358768.c
	// https://github.com/tanish2k09/venom_kernel_aio_otfp/blob/master/drivers/input/touchscreen/mediatek/S3202/synaptics_dsx_i2c.c
	// https://stash.phytec.com/projects/TIRTOS/repos/vps-phytec/raw/src/boards/src/bsp_boardPriv.h?at=e8b92520f41e6523301d120dae15db975ad6d0da
	//https://code.ihub.org.cn/projects/825/repositories/874/file_edit_page?file_name=am57xx-idk-common.dtsi&path=arch%2Farm%2Fboot%2Fdts%2Fam57xx-idk-common.dtsi&rev=master
static const videomode_t vdmode0 =
{
	.width = 720,			/* LCD PIXEL WIDTH            */
	.height = 1280,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 5,				/* Horizontal synchronization 1..40 */
	.hbp = 11,				/* Horizontal back porch      */
	.hfp = 16,				/* Horizontal front porch  16..354   */

	.vsync = 5,				/* Vertical synchronization 1..20  */
	.vbp = 11,					/* Vertical back porch        */
	.vfp = 16,				/* Vertical front porch  7..147     */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_TV101WXM
	/* 720xRGBx1280 - 5" AMOELD Panel H497TLB01.4 */
	// See also:
	// https://github.com/bbelos/rk3188-kernel/blob/master/drivers/video/rockchip/transmitter/tc358768.c
	// https://github.com/tanish2k09/venom_kernel_aio_otfp/blob/master/drivers/input/touchscreen/mediatek/S3202/synaptics_dsx_i2c.c
	// https://stash.phytec.com/projects/TIRTOS/repos/vps-phytec/raw/src/boards/src/bsp_boardPriv.h?at=e8b92520f41e6523301d120dae15db975ad6d0da
	//https://code.ihub.org.cn/projects/825/repositories/874/file_edit_page?file_name=am57xx-idk-common.dtsi&path=arch%2Farm%2Fboot%2Fdts%2Fam57xx-idk-common.dtsi&rev=master
static const videomode_t vdmode0 =
{
	.width = 800,			/* LCD PIXEL WIDTH            */
	.height = 1280,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 5,				/* Horizontal synchronization 1..40 */
	.hbp = 11,				/* Horizontal back porch      */
	.hfp = 16,				/* Horizontal front porch  16..354   */

	.vsync = 5,				/* Vertical synchronization 1..20  */
	.vbp = 11,					/* Vertical back porch        */
	.vfp = 16,				/* Vertical front porch  7..147     */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

#elif LCDMODE_HSD100IF3
/* 1024 x 600 display LVDS mode */
static const videomode_t vdmode0 =
{
	.width = 1024,      /* LCD PIXEL WIDTH            */
	.height = 600,      /* LCD PIXEL HEIGHT           */
	/**
	* @brief  LQ123K1LG03 Timing
	* MODE=0 (DE)
	* When selected DE mode, VSYNC & HSYNC must pulled HIGH
	* MODE=1 (SYNC)
	* When selected sync mode, de must be grounded.
	*/
	.hsync = 6,//40,      /* Horizontal synchronization 1..40 */
	.hbp = 39,//6,        /* Horizontal back porch      */
	.hfp = 368,///210,        /* Horizontal front porch  16..354   */

	.vsync = 1,//20,        /* Vertical synchronization 1..20  */
	.vbp = 24,//73,        /* Vertical back porch      */
	.vfp = 15,///22,        /* Vertical front porch  7..147     */


	/* Accumulated parameters for this display */
	//LEFTMARGIN = 46,    /* horizontal blanking EXACTLY */
	//TOPMARGIN = 23,      /* vertical blanking EXACTLY */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,      /* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,      /* Negative polarity required for HSYNC signal */
	.deneg = 0,        /* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,  // LQ043T3DX02K require DE reset
	.fps = 60,  /* frames per second */
	.ntsc = 0,
	.interlaced = 0
};
#else
	//#error Unsupported LCDMODE_xxx

#endif

/* AT070TN90 panel (800*480) - 7" display HV mode */
static const videomode_t vdmode_800x480 =
{
	.width = 800,			/* LCD PIXEL WIDTH            */
	.height = 480,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 40,				/* Horizontal synchronization 1..40 */
	.hbp = 6,				/* Horizontal back porch      */
	.hfp = 210,				/* Horizontal front porch  16..354   */

	.vsync = 20,				/* Vertical synchronization 1..20  */
	.vbp = 3,				/* Vertical back porch      */
	.vfp = 22,				/* Vertical front porch  7..147     */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

/* NTSC TV out parameters */
/* Aspect ratio 1.5 */
static const videomode_t vdmode_NTSC0 =
{
	.width = 720,			/* LCD PIXEL WIDTH            */
	.height = 480,			/* LCD PIXEL HEIGHT           */

	.hsync = 63,				/* Horizontal synchronization */
	.hbp = 60,				/* Horizontal back porch      */
	.hfp = 15,				/* Horizontal front porch */

	.vsync = 7,			/* Vertical synchronization */
	.vbp = 30,				/* Vertical back porch      */
	.vfp = 8,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 60,	/* frames per second */
	.ntsc = 1,
	.interlaced = 1
};

/* PAL TV out parameters */
/* Aspect ratio 1.25 */
static const videomode_t vdmode_PAL0 =
{
	.width = 720,			/* LCD PIXEL WIDTH            */
	.height = 576,			/* LCD PIXEL HEIGHT           */

	.hsync = 65,			/* Horizontal synchronization */
	.hbp = 68,				/* Horizontal back porch      */
	.hfp = 11,				/* Horizontal front porch  */

	.vsync = 6,				/* Vertical synchronization */
	.vbp = 39,				/* Vertical back porch      */
	.vfp = 4,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 1
};

//	Horizontal Timings
//		Active Pixels       1920
//		Front Porch           88
//		Sync Width            44
//		Back Porch           148
//		Blanking Total       280
//		Total Pixels        2200
//
//	Vertical Timings
//		Active Lines        1080
//		Front Porch            4
//		Sync Width             5
//		Back Porch            36
//		Blanking Total        45
//		Total Lines         1125

// https://github.com/akatrevorjay/edid-generator/tree/master
/* HDMI TV out parameters HD 1920x1080 60 Hz*/
/* Aspect ratio 16:9 (1.7(7)), dot clock = 148.5 MHz */
// https://edid.tv/edid/2253/
// ! TESTED
//	1920x1080 (0x47)  138.5MHz -HSync -VSync *current +preferred
//	 h: width  1920 start 1968 end 2000 total 2080 skew    0 clock   66.6KHz
//	 v: height 1080 start 1088 end 1102 total 1110           clock   60.0Hz
static const videomode_t vdmode_HDMI_1920x1080at60 =
{
	.width = 1920,			/* LCD PIXEL WIDTH            */
	.height = 1080,			/* LCD PIXEL HEIGHT           */

	// Horizontal Blanking XBLANK = hsync + hbp + hfp = 280
	.hsync = 44,			/* Horizontal synchronization XPULSE  */
	.hbp = 148,				/* Horizontal back porch  XBLANK - XOFFSET - XPULSE    */
	.hfp = 88,				/* Horizontal front porch  XOFFSET */

	// Vertical Blanking YBLANK  = vsync + vbp + vfp = 45
	.vsync = 5,				/* Vertical synchronization */
	.vbp = 36,				/* Vertical back porch      */
	.vfp = 4,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

// https://projectf.io/posts/video-timings-vga-720p-1080p/
/* Aspect ratio 16:9 (1.7(7)), dot clock = 80.291 MHz */
// ! TESTED, параметры не подтверждены, работает и на 50 и 60 герц
// 1360×765
// https://billauer.co.il/blog/2015/07/vesa-edid-parameters/
static const videomode_t vdmode_HDMI_1366x768at60 =
{
	.width = 1366,			/* LCD PIXEL WIDTH            */
	.height = 768,			/* LCD PIXEL HEIGHT           */

	// Horizontal Blanking XBLANK = hsync + hbp + hfp = 280
	.hsync = 44,			/* Horizontal synchronization XPULSE  */
	.hbp = 148,				/* Horizontal back porch  XBLANK - XOFFSET - XPULSE    */
	.hfp = 88,				/* Horizontal front porch  XOFFSET */

	// Vertical Blanking YBLANK  = vsync + vbp + vfp = 45
	.vsync = 5,				/* Vertical synchronization */
	.vbp = 36,				/* Vertical back porch      */
	.vfp = 4,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

/* Aspect ratio 16:9 (1.7(7)), dot clock = xxx MHz */
// ! TESTED
// https://tomverbeure.github.io/video_timings_calculator
// CVT-RB Modeline "1024x768_59.87" 56 1024 1072 1104 1184 768 771 775 790 +HSync -VSync
static const videomode_t vdmode_HDMI_1024x768at60 =
{
	.width = 1024,			/* LCD PIXEL WIDTH            */
	.height = 768,			/* LCD PIXEL HEIGHT           */

	// Horizontal Blanking XBLANK = hsync + hbp + hfp = 160
	.hsync = 32,			/* Horizontal synchronization XPULSE  */
	.hbp = 80,				/* Horizontal back porch  XBLANK - XOFFSET - XPULSE    */
	.hfp = 48,				/* Horizontal front porch  XOFFSET */

	// Vertical Blanking YBLANK  = vsync + vbp + vfp = 22
	.vsync = 4,				/* Vertical synchronization */
	.vbp = 15,				/* Vertical back porch      */
	.vfp = 3,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 60,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

//	1440x900 (0xb5)  106.5MHz -HSync +VSync
//	h: width  1440 start 1520 end 1672 total 1904 skew    0 clock   55.9KHz
//	v: height  900 start  903 end  909 total  934           clock   59.9Hz
// ! TESTED, параметры не подтверждены, работает на 50 герц
// 74.720 MHz
// https://tomverbeure.github.io/video_timings_calculator
// CVT-RB
static const videomode_t vdmode_HDMI_1440x900at50 =
{
	.width = 1440,			/* LCD PIXEL WIDTH            */
	.height = 900,			/* LCD PIXEL HEIGHT           */

	// Horizontal Blanking XBLANK = hsync + hbp + hfp = 160
    .hsync = 32,            /* Horizontal synchronization XPULSE  */
    .hbp = 80,    			/* Horizontal back porch  XBLANK - XOFFSET - XPULSE    */
    .hfp = 48,              /* Horizontal front porch  XOFFSET */

	// Vblank от DMT - иначе не работает
	// Vertical Blanking YBLANK  = vsync + vbp + vfp = 34
	.vsync = 6,				/* Vertical synchronization */
 	.vbp = 25,                /* Vertical back porch      */
	.vfp = 3,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 0,			/* Positive polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 50,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

// Pixel Clock: 74.25MHz
/* Aspect ratio 16:9 (1.7(7)), dot clock = 74.250 MHz */
// ! TESTED
static const videomode_t vdmode_HDMI_1280x720at50 =
{
	.width = 1280,			/* LCD PIXEL WIDTH            */
	.height = 720,			/* LCD PIXEL HEIGHT           */

	// Horizontal Blanking XBLANK = hsync + hbp + hfp = 700
	.hsync = 40,			/* Horizontal synchronization XPULSE  */
	.hbp = 700 - 40 - 440,	/* Horizontal back porch  XBLANK - XOFFSET - XPULSE    */
	.hfp = 440,				/* Horizontal front porch  XOFFSET */

	// Vertical Blanking YBLANK  = vsync + vbp + vfp = 30
	.vsync = 5,				/* Vertical synchronization */
	.vbp = 30 - 5 - 5,				/* Vertical back porch      */
	.vfp = 5,				/* Vertical front porch */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */

	.fps = 50,	/* frames per second */
	.ntsc = 0,
	.interlaced = 0
};

static uint_fast8_t glob_tvoutformat;

const videomode_t * hdmiformats [HDMIFORMATS_count] =
{
	& vdmode_HDMI_1366x768at60,	// б. тел. не занимается масштабированием этого формата ! TESTED, параметры не подтверждены, работает и на 50 и 60 герц
	& vdmode_HDMI_1024x768at60,	// большой телевизор не занимается масштабированием этого формата ! TESTED
	& vdmode_HDMI_1280x720at50,	// б. тел. масштабирует ! TESTED
	& vdmode_HDMI_1440x900at50,	// б. тел. не занимается масштабированием этого формата ! TESTED, параметры не подтверждены, работает на 50 герц
	& vdmode_HDMI_1920x1080at60,	// б. тел. масштабирует ! TESTED
};

const videomode_t * get_videomode_TVOUT(void)
{
#if WITHHDMITVHW
	return hdmiformats [glob_tvoutformat];
#else /* WITHHDMITVHW */
	return & vdmode_PAL0;
	//return & vdmode_NTSC0;
#endif /* WITHHDMITVHW */
}

void
board_set_tvoutformat(uint_fast8_t v)
{
	const uint_fast8_t n = v;

	if (glob_tvoutformat != n)
	{
		glob_tvoutformat = n;
		hardware_tvout_set_format();
	}
}

/* Название видеорежима для отображения в меню */
size_t getvaltexthdmiformat(char * buff, size_t count, int_fast32_t value)
{
	const videomode_t * const vdmode = hdmiformats [value];
	/* Название видеорежима для отображения в меню */
	return local_snprintf_P(buff, count, "%u x %u @%u", vdmode->width, vdmode->height, vdmode->fps);
}

#endif /* WITHLTDCHW */

#if WITHLTDCHW

/* для эесперементов с масштабиованием в DE - когда DIM_X & DIM_Y не соответствуют подключённому дисплею */
const videomode_t * get_videomode_LCD(void)
{
	//return & vdmode_800x480;
	return & vdmode0;
}

const videomode_t * get_videomode_DESIGN(void)
{
	return & vdmode0;
}

#endif /* WITHLTDCHW */

/*
 * настройка портов для последующей работы с дополнительными (кроме последовательного канала)
 * сигналами дисплея.
 */

void display_hardware_initialize(void)
{
	//PRINTF(PSTR("display_hardware_initialize start\n"));

#if WITHLTDCHW
	hardware_ltdc_initialize();
	hardware_ltdc_L8_palette();
#endif /* WITHLTDCHW */

#if LCDMODETX_TC358778XBG
	const videomode_t * const vdmode = get_videomode_LCD();
	tc358768_initialize(vdmode);
	panel_initialize(vdmode);
#endif /* LCDMODETX_TC358778XBG */
#if LCDMODEX_SII9022A
	/* siiI9022A Lattice Semiconductor Corp HDMI Transmitter */
	sii9022x_initialize(vdmode);
#endif /* LCDMODEX_SII9022A */

	//PRINTF(PSTR("display_hardware_initialize done\n"));
}

void display_wakeup(void)
{
#if WITHLTDCHW
	hardware_ltdc_initialize();
	hardware_ltdc_L8_palette();
#endif /* WITHLTDCHW */
#if LCDMODETX_TC358778XBG
	const videomode_t * const vdmode = get_videomode_LCD();
  tc358768_wakeup(vdmode);
    panel_wakeup();
#endif /* LCDMODETX_TC358778XBG */
#if LCDMODEX_SII9022A
    // siiI9022A Lattice Semiconductor Corp HDMI Transmitter
    sii9022x_wakeup(vdmode);
#endif /* LCDMODEX_SII9022A */
}


// https://habr.com/ru/post/166317/

//	Hue - тон, цикличная угловая координата.
//	Value, Brightness - яркость, воспринимается как альфа-канал, при v=0 пиксель не светится,
//	при v=17 - светится максимально ярко, в зависимости от H и S.
//	Saturation. С отсутствием фона, значения  дадут не серый цвет, а белый разной яркости,
//	поэтому параметр W=Smax-S можно называть Whiteness - он отражает степень "белизны" цвета.
//	При W=0, S=Smax=15 цвет полностью определяется Hue, при S=0, W=Wmax=15 цвет пикселя
//	будет белым.

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_t;

typedef struct {
    uint8_t h;
    uint8_t s;
    uint8_t v;
} HSV_t;

const uint8_t max_whiteness = 15;
const uint8_t max_value = 17;

enum
{
	sixth_hue = 16,
	third_hue = sixth_hue * 2,
	half_hue = sixth_hue * 3,
	two_thirds_hue = sixth_hue * 4,
	five_sixths_hue = sixth_hue * 5,
	full_hue = sixth_hue * 6
};

RGB_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return (RGB_t) { r, g, b };
}

HSV_t hsv(uint8_t h, uint8_t s, uint8_t v)
{
    return (HSV_t) { h, s, v };
}

RGB_t hsv2rgb(HSV_t hsv)
{
	const RGB_t black = { 0, 0, 0 };

    if (hsv.v == 0) return black;

    uint8_t high = hsv.v * max_whiteness;//channel with max value
    if (hsv.s == 0) return rgb(high, high, high);

    uint8_t W = max_whiteness - hsv.s;
    uint8_t low = hsv.v * W;//channel with min value
    uint8_t rising = low;
    uint8_t falling = high;

    const uint8_t h_after_sixth = hsv.h % sixth_hue;
    if (h_after_sixth > 0)
    {
    	//not at primary color? ok, h_after_sixth = 1..sixth_hue - 1
        const uint8_t z = hsv.s * (uint8_t) (hsv.v * h_after_sixth) / sixth_hue;
        rising += z;
        falling -= z + 1;//it's never 255, so ok
    }

    uint8_t H = hsv.h;
    while (H >= full_hue)
    	H -= full_hue;

    if (H < sixth_hue) return rgb(high, rising, low);
    if (H < third_hue) return rgb(falling, high, low);
    if (H < half_hue) return rgb(low, high, rising);
    if (H < two_thirds_hue) return rgb(low, falling, high);
    if (H < five_sixths_hue) return rgb(rising, low, high);
    return rgb(high, low, falling);
}

#if WITHRLEDECOMPRESS

COLORPIP_T convert_565_to_a888(uint16_t color)
{
	uint8_t b5 = RGB565_B(color);
	uint8_t g6 = RGB565_G(color);
	uint8_t r5 = RGB565_R(color);

	return TFTRGB(r5, g6, b5);
}

void graw_picture_RLE(const gxdrawb_t * db, uint16_t x, uint16_t y, const picRLE_t * picture, PACKEDCOLORPIP_T bg_color)
{
	uint_fast32_t i = 0;
	uint_fast16_t x1 = x, y1 = y;
	uint_fast16_t transparent_color = 0, count = 0;

	while (y1 < y + picture->height)
	{
		if ((int16_t)picture->data [i] < 0) // no repeats
		{
			count = (-(int16_t)picture->data [i]);
			i ++;
			for (uint_fast16_t p = 0; p < count; p ++)
			{
				const COLORPIP_T point = convert_565_to_a888(picture->data [i]);
				colpip_point(db, x1, y1, picture->data [i] == 0 ? bg_color : point);

				x1 ++;
				if (x1 >= x + picture->width)
				{
					x1 = x;
					y1 ++;
				}
				i ++;
			}
		}
		else // repeats
		{
			count = ((int16_t)picture->data [i]);
			i++;

			const COLORPIP_T point = convert_565_to_a888(picture->data [i]);
			for (uint_fast16_t p = 0; p < count; p ++)
			{
				colpip_point(db, x1, y1, picture->data [i] == 0 ? bg_color : point);

				x1 ++;
				if (x1 >= x + picture->width)
				{
					x1 = x;
					y1 ++;
				}
			}
			i ++;
		}
	}
}

void graw_picture_RLE_buf(const gxdrawb_t * db, uint16_t x, uint16_t y, const picRLE_t * picture, COLORPIP_T bg_color)
{
	uint_fast32_t i = 0;
	uint_fast16_t x1 = x, y1 = y;
	uint_fast16_t transparent_color = 0, count = 0;

	while (y1 < y + picture->height)
	{
		if ((int16_t)picture->data [i] < 0) // no repeats
		{
			count = (-(int16_t)picture->data [i]);
			i ++;
			for (uint_fast16_t p = 0; p < count; p++)
			{
				COLORPIP_T point = convert_565_to_a888(picture->data [i]);
				colpip_point(db, x1, y1, picture->data [i] == transparent_color ? bg_color : point);

				x1 ++;
				if (x1 >= x + picture->width)
				{
					x1 = x;
					y1 ++;
				}
				i ++;
			}
		}
		else // repeats
		{
			count = ((int16_t)picture->data [i]);
			i ++;

			PACKEDCOLORPIP_T point = convert_565_to_a888(picture->data [i]);
			for (uint_fast16_t p = 0; p < count; p++)
			{
				colpip_point(db, x1, y1, picture->data[i] == transparent_color ? bg_color : point);

				x1 ++;
				if (x1 >= x + picture->width)
				{
					x1 = x;
					y1 ++;
				}
			}
			i ++;
		}
	}
}

#endif /* WITHRLEDECOMPRESS */

#if LCDMODE_LTDC

void display_do_AA(
	const gxdrawb_t * db,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз)
	uint_fast16_t width,
	uint_fast16_t height
	)
{
	uint_fast16_t x;
	for (x = col; x < (col + width - 1); x ++)
	{
		uint_fast16_t y;
		for (y = row; y < (row + height - 1); y ++)
		{
			const COLORPIP_T p1 = * colpip_mem_at(db, x, y);
			const COLORPIP_T p2 = * colpip_mem_at(db, x + 1, y);
			const COLORPIP_T p3 = * colpip_mem_at(db, x, y + 1);
			const COLORPIP_T p4 = * colpip_mem_at(db, x + 1, y + 1);

			unsigned p1_r, p1_g, p1_b, p2_r, p2_g, p2_b, p3_r, p3_g, p3_b, p4_r, p4_g, p4_b;
			unsigned p_r, p_b, p_g;

			 if ((p1 == p2) && (p1 == p3) && (p1 == p4))	// если смежные пиксели одинакового цвета, пропустить расчёт
				continue;

			p1_r = COLORPIP_R(p1);
			p1_g = COLORPIP_G(p1);
			p1_b = COLORPIP_B(p1);

			p2_r = COLORPIP_R(p2);
			p2_g = COLORPIP_G(p2);
			p2_b = COLORPIP_B(p2);

			p3_r = COLORPIP_R(p3);
			p3_g = COLORPIP_G(p3);
			p3_b = COLORPIP_B(p3);

			p4_r = COLORPIP_R(p4);
			p4_g = COLORPIP_G(p4);
			p4_b = COLORPIP_B(p4);

			p_r = ((uint_fast32_t) p1_r + p2_r + p3_r + p4_r) / 4;
			p_g = ((uint_fast32_t) p1_g + p2_g + p3_g + p4_g) / 4;
			p_b = ((uint_fast32_t) p1_b + p2_b + p3_b + p4_b) / 4;

			colpip_point(db, x, y, TFTRGB(p_r, p_g, p_b));
		}
	}
}

#endif /* LCDMODE_LTDC */


/* Получить желаемую частоту pixel clock для данного видеорежима. */
uint_fast32_t display_getdotclock(const videomode_t * vdmode)
{
	/* Accumulated parameters for this display */
	const unsigned HEIGHT = vdmode->height;	/* height */
	const unsigned WIDTH = vdmode->width;	/* width */
	const unsigned HSYNC = vdmode->hsync;	/*  */
	const unsigned VSYNC = vdmode->vsync;	/*  */
	const unsigned LEFTMARGIN = HSYNC + vdmode->hbp;	/* horizontal delay before DE start */
	const unsigned TOPMARGIN = VSYNC + vdmode->vbp;	/* vertical delay before DE start */
	const unsigned HTOTAL = LEFTMARGIN + WIDTH + vdmode->hfp;	/* horizontal full period */
	const unsigned VTOTAL = TOPMARGIN + HEIGHT + vdmode->vfp;	/* vertical full period */

	return (uint_fast32_t) vdmode->fps * HTOTAL * VTOTAL;
	//return (uint_fast32_t) vdmode->fps * HTOTAL * VTOTAL / (vdmode->interlaced + 1);
}

// Используется при выводе на графический индикатор с кординатами и размерами в пикселях
void
pix_display_text(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, uint_fast16_t w, uint_fast16_t h, const gxstyle_t * dbstyle, const char * s)
{
	pix_display_texts(db, xpix, ypix, w, h, dbstyle, & s, 1);	// одга строка для multi-line
}

// Используется при выводе на графический индикатор с кординатами и размерами по сетке
void
display_text(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, const char * s, uint_fast8_t xspan, uint_fast8_t yspan, const gxstyle_t * dbstylep)
{
	pix_display_texts(db, GRID2X(xcell), GRID2Y(ycell), GRID2X(xspan), GRID2Y(yspan), dbstylep, & s, 1);
}

void gxstyle_setsmallfont(gxstyle_t * dbstyle, const unifont_t * font)
{
	dbstyle->font = font;
}

// уменьшение размера плашки
void gxstyle_setbgbackoff(gxstyle_t * dbstyle, unsigned x, unsigned y)
{
	dbstyle->bgbackoffw = x;	// уменьшение размера плашки по горизонтали
	dbstyle->bgbackoffh = y;	// уменьшение размера плашки по вертикали
}

void gxstyle_setbgradius(gxstyle_t * dbstyle, unsigned r)
{
	dbstyle->bgradius = r;
}

void gxstyle_setbgrfilled(gxstyle_t * dbstyle, unsigned f)
{
	dbstyle->bgfilled = f;
}

void gxstyle_setsbigandhalffont(gxstyle_t * dbstyle)
{
#if ! LCDMODE_LTDC
#else /* WITHALTERNATIVEFONTS */
#endif /* WITHALTERNATIVEFONTS */
}

#define GXSTYLE_BACKOFF 2	// на столько пикселей уменьшается высота и ширина при наличии радиуса в стиле

void gxstyle_initialize(gxstyle_t * dbstyle)
{
	memset(dbstyle, 0, sizeof * dbstyle);
	gxstyle_textcolor(dbstyle, COLORPIP_WHITE, COLORPIP_BLACK);
	gxstyle_setsmallfont(dbstyle, & unifont_small);
	gxstyle_texthalign(dbstyle, GXSTYLE_HALIGN_RIGHT);
	gxstyle_textvalign(dbstyle, GXSTYLE_VALIGN_CENTER);
	gxstyle_setbgradius(dbstyle, display2_gettileradius());
	gxstyle_setbgrfilled(dbstyle, 1);
	gxstyle_setbgbackoff(dbstyle, GXSTYLE_BACKOFF, GXSTYLE_BACKOFF); // уменьшение размера плашки
}


void gxstyle_textcolor(gxstyle_t * dbstyle, COLORPIP_T fg, COLORPIP_T bg)
{
#if ! LCDMODE_LTDC_L24
	dbstyle->textcolor = fg;
	dbstyle->bgcolor = bg;
#else /* ! LCDMODE_LTDC_L24 */
	dbstyle->textcolor.r = COLORPIP_R(fg);
	dbstyle->textcolor.g = COLORPIP_G(fg);
	dbstyle->textcolor.b = COLORPIP_B(fg);
	dbstyle->bgcolor.r = COLORPIP_R(bg);
	dbstyle->bgcolor.g = COLORPIP_G(bg);
	dbstyle->bgcolor.b = COLORPIP_B(bg);

#endif /* ! LCDMODE_LTDC_L24 */

}

void gxstyle_texthalign(gxstyle_t * dbstyle, enum gxstyle_texthalign a)
{
	dbstyle->texthalign = a;
}

void gxstyle_textvalign(gxstyle_t * dbstyle, enum gxstyle_textvalign a)
{
	dbstyle->textvalign = a;
}
