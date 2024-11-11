
#include "hardware.h"
#include "formats.h"

#if 1

#include <string.h>
#include "display/display.h"
#include "display/fontmaps.h"
#include "display2.h"

#include "litehtml.h"
#include <litehtml/encodings.h>

using namespace litehtml;

class hftrxgd: public litehtml::document_container
{
	// call back interface to draw text, images and other elements
public:
	litehtml::uint_ptr create_font(const char *faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics *fm);
	void delete_font(litehtml::uint_ptr hFont);
	int text_width(const char *text, litehtml::uint_ptr hFont);
	void draw_text(litehtml::uint_ptr hdc, const char *text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position &pos);
	int pt_to_px(int pt) const;
	int get_default_font_size() const;
	const char* get_default_font_name() const;
	void draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker &marker);
	void load_image(const char *src, const char *baseurl, bool redraw_on_ready);
	void get_image_size(const char *src, const char *baseurl, litehtml::size &sz);
	void draw_image(litehtml::uint_ptr hdc, const background_layer &layer, const std::string &url, const std::string &base_url);
	void draw_solid_fill(litehtml::uint_ptr hdc, const background_layer &layer, const web_color &color);
	void draw_linear_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::linear_gradient &gradient);
	void draw_radial_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::radial_gradient &gradient);
	void draw_conic_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::conic_gradient &gradient);
	void draw_borders(litehtml::uint_ptr hdc, const litehtml::borders &borders, const litehtml::position &draw_pos, bool root);

	void set_caption(const char *caption);
	void set_base_url(const char *base_url);
	void link(const std::shared_ptr<litehtml::document> &doc, const litehtml::element::ptr &el);
	void on_anchor_click(const char *url, const litehtml::element::ptr &el);
	void on_mouse_event(const litehtml::element::ptr &el, litehtml::mouse_event event);
	void set_cursor(const char *cursor);
	void transform_text(litehtml::string &text, litehtml::text_transform tt);
	void import_css(litehtml::string &text, const litehtml::string &url, litehtml::string &baseurl);
	void set_clip(const litehtml::position &pos, const litehtml::border_radiuses &bdr_radius);
	void del_clip();
	void get_client_rect(litehtml::position &client) const;
	litehtml::element::ptr create_element(const char *tag_name, const litehtml::string_map &attributes, const std::shared_ptr<litehtml::document> &doc);

	void get_media_features(litehtml::media_features &media) const;
	void get_language(litehtml::string &language, litehtml::string &culture) const;
//	litehtml::string resolve_color(const litehtml::string& /*color*/) const
//	{
//		return litehtml::string();
//	}
//	void split_text(const char *text, const std::function<void(const char*)> &on_word, const std::function<void(const char*)> &on_space);
	int m_dx;
	int m_dy;
	hftrxgd() = default;
	virtual ~hftrxgd() = default;

};

static COLORPIP_T getCOLPIP(const litehtml::web_color& color)
{
	return TFTALPHA(color.alpha, TFTRGB(color.red, color.green, color.blue));
}

litehtml::uint_ptr hftrxgd::create_font(const char *faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics *fm)
{
	PRINTF("create_font: faceName='%s', size=%d\n", faceName, size);
	if (fm)
	{
		fm->font_size = size;
		fm->ascent = 0;//PANGO_PIXELS((double)pango_font_metrics_get_ascent(metrics));
		fm->descent = 0;//PANGO_PIXELS((double)pango_font_metrics_get_descent(metrics));
		fm->height = SMALLCHARH; //PANGO_PIXELS((double)pango_font_metrics_get_height(metrics));
		fm->x_height = fm->height;
	}
	return 1;
}
void hftrxgd::delete_font(litehtml::uint_ptr hFont)
{
	(void) hFont;
}

int hftrxgd::text_width(const char *text, litehtml::uint_ptr hFont)
{
	(void) hFont;
	return SMALLCHARW * strlen(text);
}

void hftrxgd::draw_text(litehtml::uint_ptr hdc, const char *text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position &pos)
{
	//PRINTF("draw_text: text='%s'\n", text);
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;

	//colpip_fillrect(buffer, dx, dy, pos.left(), pos.top(), pos.width, pos.height, getCOLPIP(color));
	colpip_string_tbg(buffer, dx, dy, pos.left(), pos.top(), text, getCOLPIP(color));
}

int hftrxgd::pt_to_px(int pt) const
{
	return pt;
}
int hftrxgd::get_default_font_size() const
{
	return 12;
}
const char* hftrxgd::get_default_font_name() const
{
	return "Times New Roman";
}
void hftrxgd::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker &marker)
{
	TP();
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;

    colpip_fillrect(buffer, dx, dy, marker.pos.left(), marker.pos.top(), marker.pos.width, marker.pos.height, getCOLPIP(marker.color));

//	int top_margin = marker.pos.height / 3;
//	if (top_margin < 4)
//		top_margin = 0;
//
//	int draw_x = marker.pos.left();
//	int draw_y = marker.pos.top() + top_margin;
//	int draw_width = marker.pos.height - top_margin * 2;
//	int draw_height = marker.pos.height - top_margin * 2;
//
//	colpip_fillrect(buffer, dx, dy, draw_x, draw_y, draw_width, draw_height, getCOLPIP(marker.color));
}

void hftrxgd::load_image(const char *src, const char *baseurl, bool redraw_on_ready)
{
	TP();
}
void hftrxgd::get_image_size(const char *src, const char *baseurl, litehtml::size &sz)
{
	TP();
}
void hftrxgd::draw_image(litehtml::uint_ptr hdc, const background_layer &layer, const std::string &url, const std::string &base_url)
{
	TP();
}
void hftrxgd::draw_solid_fill(litehtml::uint_ptr hdc, const background_layer &layer, const web_color &color)
{
	//PRINTF("draw_solid_fill: bottom_left_x=%d\n", layer.border_radius.bottom_left_x);
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;

	colpip_fillrect(buffer, dx, dy, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, getCOLPIP(color));
}

void hftrxgd::draw_linear_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::linear_gradient &gradient)
{
	//TP();
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	COLORPIP_T color = COLORPIP_RED;

	colpip_fillrect(buffer, dx, dy, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, color);

}
void hftrxgd::draw_radial_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::radial_gradient &gradient)
{
	//TP();
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	COLORPIP_T color = COLORPIP_GREEN;

	colpip_fillrect(buffer, dx, dy, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, color);

}

void hftrxgd::draw_conic_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::conic_gradient &gradient)
{
	//TP();
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	COLORPIP_T color = COLORPIP_BLUE;

	colpip_fillrect(buffer, dx, dy, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, color);

}

void hftrxgd::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders &borders, const litehtml::position &draw_pos, bool root)
{
	//PRINTF("draw_borders: bottom_left_x=%d\n", borders.radius.bottom_left_x);
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	COLORPIP_T top_color = getCOLPIP(borders.top.color);
	COLORPIP_T botom_color = getCOLPIP(borders.bottom.color);
	COLORPIP_T left_color = getCOLPIP(borders.left.color);
	COLORPIP_T right_color = getCOLPIP(borders.right.color);
	colpip_rect(buffer, dx, dy, draw_pos.left(), draw_pos.top(), draw_pos.right() - 1, draw_pos.bottom() - 1, top_color, 0);
}

void hftrxgd::set_caption(const char *caption)
{
	//PRINTF("set_caption: caption='%s'\n", caption);
}
void hftrxgd::set_base_url(const char *base_url)
{
	//PRINTF("set_base_url: base_url='%s'\n", base_url);
}
void link(const std::shared_ptr<litehtml::document> &doc, const litehtml::element::ptr &el)
{
	//TP();
}
void hftrxgd::on_anchor_click(const char *url, const litehtml::element::ptr &el)
{
	TP();
}
void hftrxgd::on_mouse_event(const litehtml::element::ptr &el, litehtml::mouse_event event)
{
	TP();
}
void hftrxgd::set_cursor(const char *cursor)
{
	TP();
}
void hftrxgd::transform_text(litehtml::string &text, litehtml::text_transform tt)
{
	TP();
}
void hftrxgd::import_css(litehtml::string &text, const litehtml::string &url, litehtml::string &baseurl)
{
	//TP();
}

void hftrxgd::set_clip(const litehtml::position &pos, const litehtml::border_radiuses &bdr_radius)
{
	TP();
}
void hftrxgd::del_clip()
{
	TP();
}
void hftrxgd::get_client_rect(litehtml::position &client) const
{
	client = litehtml::position(0, 0, DIM_X, DIM_Y);
}

litehtml::element::ptr hftrxgd::create_element(const char *tag_name, const litehtml::string_map &attributes, const std::shared_ptr<litehtml::document> &doc)
{
	//PRINTF("create_element: tag_name='%s'\n", tag_name);
	return 0;
}

void hftrxgd::get_media_features(litehtml::media_features &media) const
{
	position client;
	get_client_rect(client);

	media.type = media_type_screen;
	media.width = client.width;
	media.height = client.height;
	media.color = 8;
	media.monochrome = 0;
	media.color_index = 256;
	media.resolution = 96;//GetDeviceCaps(m_tmp_hdc, LOGPIXELSX);
	media.device_width = client.width; //1000; //GetDeviceCaps(m_tmp_hdc, HORZRES);
	media.device_height = client.height; //1000; //GetDeviceCaps(m_tmp_hdc, VERTRES);
}

void hftrxgd::get_language(litehtml::string &language, litehtml::string &culture) const
{
	language = "en";
	culture = "";
}

//void hftrxgd::split_text(const char *text, const std::function<void(const char*)> &on_word, const std::function<void(const char*)> &on_space)
//{
//	//PRINTF("split_text: text='%s'\n", text);
//	//on_word("zzzzzz");
//	on_word(text);
//}

void hftrxgd::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el)
{
	//TP();
}

static const char htmlString [] =
{
	#include "testdata/nut.html.h"
	0,
};

void litehtmltest(void)
{
	uint_ptr hdc = 0;
	const position wndclip(0, 0, DIM_X, DIM_Y);
	hftrxgd cont;
	// see doc/document_createFromString.txt
	auto doc = document::createFromString(htmlString, & cont);

	doc->render(wndclip.width, render_all);
	doc->draw(hdc, 0, 0, & wndclip);
	colmain_nextfb();
	TP();
	local_delay_ms(2500);
	for (;;)
		;
}
#endif
