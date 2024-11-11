
#include "hardware.h"
#include "formats.h"

#if 0

#include <string.h>
#include "display/display.h"
#include "display/fontmaps.h"
#include "display2.h"

#include "litehtml.h"
#include <litehtml/encodings.h>

using namespace litehtml;

class hftrxcontainer: public litehtml::document_container
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
	hftrxcontainer() = default;
	virtual ~hftrxcontainer() = default;

};

static COLORPIP_T getCOLPIP(const litehtml::web_color& color)
{
	return TFTALPHA(color.alpha, TFTRGB(color.red, color.green, color.blue));
}

litehtml::uint_ptr hftrxcontainer::create_font(const char *faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics *fm)
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
void hftrxcontainer::delete_font(litehtml::uint_ptr hFont)
{
	(void) hFont;
}

int hftrxcontainer::text_width(const char *text, litehtml::uint_ptr hFont)
{
	(void) hFont;
	return 5 * strlen(text);
}

void hftrxcontainer::draw_text(litehtml::uint_ptr hdc, const char *text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position &pos)
{
	//PRINTF("draw_text: text='%s'\n", text);
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;

	//colpip_fillrect(buffer, dx, dy, pos.left(), pos.top(), pos.width, pos.height, getCOLPIP(color));
	colpip_string_tbg(buffer, dx, dy, pos.left(), pos.top(), text, getCOLPIP(color));
}

int hftrxcontainer::pt_to_px(int pt) const
{
	return pt;
}
int hftrxcontainer::get_default_font_size() const
{
	return 12;
}
const char* hftrxcontainer::get_default_font_name() const
{
	return "Times New Roman";
}
void hftrxcontainer::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker &marker)
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

void hftrxcontainer::load_image(const char *src, const char *baseurl, bool redraw_on_ready)
{
	TP();
}
void hftrxcontainer::get_image_size(const char *src, const char *baseurl, litehtml::size &sz)
{
	TP();
}
void hftrxcontainer::draw_image(litehtml::uint_ptr hdc, const background_layer &layer, const std::string &url, const std::string &base_url)
{
	TP();
}
void hftrxcontainer::draw_solid_fill(litehtml::uint_ptr hdc, const background_layer &layer, const web_color &color)
{
	TP();
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;

	colpip_fillrect(buffer, dx, dy, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, getCOLPIP(color));
}

void hftrxcontainer::draw_linear_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::linear_gradient &gradient)
{
	TP();
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	COLORPIP_T color = COLORPIP_RED;

	colpip_fillrect(buffer, dx, dy, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, color);

}
void hftrxcontainer::draw_radial_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::radial_gradient &gradient)
{
	TP();
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	COLORPIP_T color = COLORPIP_RED;

	colpip_fillrect(buffer, dx, dy, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, color);

}

void hftrxcontainer::draw_conic_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::conic_gradient &gradient)
{
	TP();
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	COLORPIP_T color = COLORPIP_RED;

	colpip_fillrect(buffer, dx, dy, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, color);

}

void hftrxcontainer::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders &borders, const litehtml::position &draw_pos, bool root)
{
	TP();
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	COLORPIP_T color = COLORPIP_RED;

	colpip_fillrect(buffer, dx, dy, draw_pos.x, draw_pos.y, draw_pos.width, draw_pos.height, color);
}

void hftrxcontainer::set_caption(const char *caption)
{
	PRINTF("set_caption: caption='%s'\n", caption);
}
void hftrxcontainer::set_base_url(const char *base_url)
{
	PRINTF("set_base_url: base_url='%s'\n", base_url);
}
void link(const std::shared_ptr<litehtml::document> &doc, const litehtml::element::ptr &el)
{
	TP();
}
void hftrxcontainer::on_anchor_click(const char *url, const litehtml::element::ptr &el)
{
	TP();
}
void hftrxcontainer::on_mouse_event(const litehtml::element::ptr &el, litehtml::mouse_event event)
{
	TP();
}
void hftrxcontainer::set_cursor(const char *cursor)
{
	TP();
}
void hftrxcontainer::transform_text(litehtml::string &text, litehtml::text_transform tt)
{
	TP();
}
void hftrxcontainer::import_css(litehtml::string &text, const litehtml::string &url, litehtml::string &baseurl)
{
	//TP();
}

void hftrxcontainer::set_clip(const litehtml::position &pos, const litehtml::border_radiuses &bdr_radius)
{
	TP();
}
void hftrxcontainer::del_clip()
{
	TP();
}
void hftrxcontainer::get_client_rect(litehtml::position &client) const
{
	client = litehtml::position(0, 0, DIM_X, DIM_Y);
}

litehtml::element::ptr hftrxcontainer::create_element(const char *tag_name, const litehtml::string_map &attributes, const std::shared_ptr<litehtml::document> &doc)
{
	//PRINTF("create_element: tag_name='%s'\n", tag_name);
	return 0;
}

void hftrxcontainer::get_media_features(litehtml::media_features &media) const
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

void hftrxcontainer::get_language(litehtml::string &language, litehtml::string &culture) const
{
	language = "en";
	culture = "";
}

//void hftrxcontainer::split_text(const char *text, const std::function<void(const char*)> &on_word, const std::function<void(const char*)> &on_space)
//{
//	//PRINTF("split_text: text='%s'\n", text);
//	//on_word("zzzzzz");
//	on_word(text);
//}

void hftrxcontainer::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el)
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
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	COLORPIP_T color = COLORPIP_WHITE;

	uint_ptr hdc = 0;
	const position wndclip(0, 0, DIM_X, DIM_Y);
	//hftrxcontainer cont("", this);
	hftrxcontainer cont;
	// see doc/document_createFromString.txt
	auto doc = document::createFromString(htmlString, & cont);

	TP();
	colpip_fillrect(buffer, dx, dy, wndclip.left(), wndclip.top(), wndclip.width, wndclip.height, color);
	TP();
	doc->render(DIM_X, render_all);
	doc->draw(hdc, 0, 0, & wndclip);
	colmain_nextfb();
	TP();
	for (;;)
		;
	local_delay_ms(2500);
	TP();
}
#endif
