
#include "hardware.h"
#include "formats.h"

#if 0

#include "litehtml.h"
#include <litehtml/encodings.h>

using namespace litehtml;

class hftrxcontainer: public litehtml::document_container
{
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
	litehtml::string resolve_color(const litehtml::string& /*color*/) const
	{
		return litehtml::string();
	}
	void split_text(const char *text, const std::function<void(const char*)> &on_word, const std::function<void(const char*)> &on_space);

	// see doc/document_createFromString.txt
//	static litehtml::document::ptr  createFromString(
//		const estring&       str,
//		document_container*  container,
//		const string&        master_styles = litehtml::master_css,
//		const string&        user_styles = "");
};

//void hftrxcontainer::pt_to_px(int pt) const
//{
//
//}

litehtml::uint_ptr hftrxcontainer::create_font(const char *faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics *fm)
{
	return 0;
}
void hftrxcontainer::delete_font(litehtml::uint_ptr hFont)
{
}
int hftrxcontainer::text_width(const char *text, litehtml::uint_ptr hFont)
{
	return 1;
}
void hftrxcontainer::draw_text(litehtml::uint_ptr hdc, const char *text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position &pos)
{
}
int hftrxcontainer::pt_to_px(int pt) const
{
	return 1;
}
int hftrxcontainer::get_default_font_size() const
{
	return 1;
}
const char* hftrxcontainer::get_default_font_name() const
{
	return "";
}
void hftrxcontainer::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker &marker)
{
}
void hftrxcontainer::load_image(const char *src, const char *baseurl, bool redraw_on_ready)
{
}
void hftrxcontainer::get_image_size(const char *src, const char *baseurl, litehtml::size &sz)
{
}
void hftrxcontainer::draw_image(litehtml::uint_ptr hdc, const background_layer &layer, const std::string &url, const std::string &base_url)
{
}
void hftrxcontainer::draw_solid_fill(litehtml::uint_ptr hdc, const background_layer &layer, const web_color &color)
{
}
void hftrxcontainer::draw_linear_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::linear_gradient &gradient)
{
}
void hftrxcontainer::draw_radial_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::radial_gradient &gradient)
{
}
void hftrxcontainer::draw_conic_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::conic_gradient &gradient)
{
}
void hftrxcontainer::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders &borders, const litehtml::position &draw_pos, bool root)
{
}

void hftrxcontainer::set_caption(const char *caption)
{
}
void hftrxcontainer::set_base_url(const char *base_url)
{
}
void link(const std::shared_ptr<litehtml::document> &doc, const litehtml::element::ptr &el)
{
}
void hftrxcontainer::on_anchor_click(const char *url, const litehtml::element::ptr &el)
{
}
void hftrxcontainer::on_mouse_event(const litehtml::element::ptr &el, litehtml::mouse_event event)
{
}
void hftrxcontainer::set_cursor(const char *cursor)
{
}
void hftrxcontainer::transform_text(litehtml::string &text, litehtml::text_transform tt)
{
}
void hftrxcontainer::import_css(litehtml::string &text, const litehtml::string &url, litehtml::string &baseurl)
{
}
void hftrxcontainer::set_clip(const litehtml::position &pos, const litehtml::border_radiuses &bdr_radius)
{
}
void hftrxcontainer::del_clip()
{
}
void hftrxcontainer::get_client_rect(litehtml::position &client) const
{

}
litehtml::element::ptr hftrxcontainer::create_element(const char *tag_name, const litehtml::string_map &attributes, const std::shared_ptr<litehtml::document> &doc)
{
	return nullptr;
}

void hftrxcontainer::get_media_features(litehtml::media_features &media) const
{

}
void hftrxcontainer::get_language(litehtml::string &language, litehtml::string &culture) const
{

}
void hftrxcontainer::split_text(const char *text, const std::function<void(const char*)> &on_word, const std::function<void(const char*)> &on_space)
{
}

void hftrxcontainer::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el)
{

}


static char htmlString [] = "HTML";

void cpptest(void)
{
	uint_ptr hdc = 0;
	position wndclip(0, 0, DIM_X, DIM_Y);
	//hftrxcontainer cont("", this);
	hftrxcontainer cont;
	// see doc/document_createFromString.txt
	auto doc = document::createFromString(htmlString, & cont);
	doc->draw(hdc, 0, 0, & wndclip);
}
#endif
