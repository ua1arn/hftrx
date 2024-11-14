
#include "hardware.h"
#include "formats.h"

#if LCDMODE_LTDC && 0

#include <string.h>
#include <stdexcept>

#include "display/display.h"
#include "display/fontmaps.h"
#include "display2.h"

#include "litehtml.h"
#include <litehtml/encodings.h>
#include <litehtml/el_td.h>
#include <litehtml/render_item.h>

namespace litehtml
{

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
	hftrxgd(int dx, int dy) :
			m_dx(dx), m_dy(dy)
	{
	}
	virtual ~hftrxgd() = default;

};

static COLORPIP_T getCOLPIP(const litehtml::web_color &color)
{
	return TFTALPHA(color.alpha, TFTRGB(color.red, color.green, color.blue));
}

litehtml::uint_ptr hftrxgd::create_font(const char *faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics *fm)
{
	PRINTF("create_font: faceName='%s', size=%d\n", faceName, size);
	if (fm)
	{
		fm->font_size = size;
		fm->ascent = 0; //PANGO_PIXELS((double)pango_font_metrics_get_ascent(metrics));
		fm->descent = 0; //PANGO_PIXELS((double)pango_font_metrics_get_descent(metrics));
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
	PACKEDCOLORPIP_T *const buffer = colmain_fb_draw();

	//colpip_fillrect(buffer, m_dx, m_dy, pos.left(), pos.top(), pos.width, pos.height, getCOLPIP(color));
	colpip_string_tbg(buffer, m_dx, m_dy, pos.left(), pos.top(), text, getCOLPIP(color));
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
	PACKEDCOLORPIP_T *const buffer = colmain_fb_draw();

	colpip_fillrect(buffer, m_dx, m_dy, marker.pos.left(), marker.pos.top(), marker.pos.width, marker.pos.height, getCOLPIP(marker.color));

//	int top_margin = marker.pos.height / 3;
//	if (top_margin < 4)
//		top_margin = 0;
//
//	int draw_x = marker.pos.left();
//	int draw_y = marker.pos.top() + top_margin;
//	int draw_width = marker.pos.height - top_margin * 2;
//	int draw_height = marker.pos.height - top_margin * 2;
//
//	colpip_fillrect(buffer, m_dx, m_dy, draw_x, draw_y, draw_width, draw_height, getCOLPIP(marker.color));
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
	PACKEDCOLORPIP_T *const buffer = colmain_fb_draw();
	const uint_fast16_t m_dx = DIM_X;
	const uint_fast16_t m_dy = DIM_Y;

	colpip_fillrect(buffer, m_dx, m_dy, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, getCOLPIP(color));
}

void hftrxgd::draw_linear_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::linear_gradient &gradient)
{
	//TP();
	PACKEDCOLORPIP_T *const buffer = colmain_fb_draw();
	COLORPIP_T color = COLORPIP_RED;

	colpip_fillrect(buffer, m_dx, m_dy, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, color);

}
void hftrxgd::draw_radial_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::radial_gradient &gradient)
{
	//TP();
	PACKEDCOLORPIP_T *const buffer = colmain_fb_draw();
	COLORPIP_T color = COLORPIP_GREEN;

	colpip_fillrect(buffer, m_dx, m_dy, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, color);

}

void hftrxgd::draw_conic_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::conic_gradient &gradient)
{
	//TP();
	PACKEDCOLORPIP_T *const buffer = colmain_fb_draw();
	COLORPIP_T color = COLORPIP_BLUE;

	colpip_fillrect(buffer, m_dx, m_dy, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, color);

}

void hftrxgd::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders &borders, const litehtml::position &draw_pos, bool root)
{
	//PRINTF("draw_borders: bottom_left_x=%d\n", borders.radius.bottom_left_x);
	PACKEDCOLORPIP_T *const buffer = colmain_fb_draw();
	COLORPIP_T top_color = getCOLPIP(borders.top.color);
	COLORPIP_T botom_color = getCOLPIP(borders.bottom.color);
	COLORPIP_T left_color = getCOLPIP(borders.left.color);
	COLORPIP_T right_color = getCOLPIP(borders.right.color);
	colpip_rect(buffer, m_dx, m_dy, draw_pos.left(), draw_pos.top(), draw_pos.right() - 1, draw_pos.bottom() - 1, top_color, 0);
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
	PRINTF("link: doc=%pid='%s'\n", (const void*) & doc, (const char*) el->id());

	//TP();
}
void hftrxgd::on_anchor_click(const char *url, const litehtml::element::ptr &el)
{
	TP();
}

// вызывается если попадаем в элемент
void hftrxgd::on_mouse_event(const litehtml::element::ptr &el, litehtml::mouse_event event)
{
	//TP();
	PRINTF("on_mouse_event: id='%d'\n", el->tag());
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

class freqel: public litehtml::el_td
{
	virtual void draw(uint_ptr hdc, int x, int y, const position *clip, const std::shared_ptr<render_item> &ri);
	virtual void draw_background(uint_ptr hdc, int x, int y, const position *clip, const std::shared_ptr<render_item> &ri);
	virtual void get_text(string &text);
	virtual void get_content_size(size &sz, int max_width);

	virtual bool on_mouse_over() override;
	virtual bool on_mouse_leave() override;
	virtual bool on_lbutton_down() override;
	virtual bool on_lbutton_up() override;
	virtual void on_click() override;

	int m_rxid;
public:
	freqel(const std::shared_ptr<document> &doc, int rxid) :
			litehtml::el_td(doc), m_rxid(rxid)
	{
		set_data("Hello set data");
	}
	virtual ~ freqel() = default;
};

void freqel::draw(uint_ptr hdc, int x, int y, const position *clip, const std::shared_ptr<render_item> &ri)
{
	PACKEDCOLORPIP_T *const buffer = colmain_fb_draw();
	const int dx = DIM_X;
	const int dy = DIM_Y;
	COLORPIP_T color = COLORPIP_BLUE;

	const position &pos = ri->pos();
	string text;
	get_text(text);
	PRINTF("draw: x=%d, y=%d, text='%s', x=%d, y=%d, w=%d, h=%d, U=%d\n", x, y, text.c_str(), pos.x, pos.y, pos.width, pos.height, ri->margin_top());
	//colpip_fillrect(buffer, dx, dy, x + pos.x, y + pos.y, pos.width, pos.height, color);
	litehtml::el_td::draw(hdc, x, y, clip, ri);
}

void freqel::draw_background(uint_ptr hdc, int x, int y, const position *clip, const std::shared_ptr<render_item> &ri)
{
	PACKEDCOLORPIP_T *const buffer = colmain_fb_draw();
	const int dx = DIM_X;
	const int dy = DIM_Y;
	COLORPIP_T color = COLORPIP_BLUE;

	const position &pos = ri->pos();
	string text;
	get_text(text);
	PRINTF("draw: x=%d, y=%d, text='%s', x=%d, y=%d, w=%d, h=%d\n", x, y, text.c_str(), pos.x, pos.y, pos.width, pos.height);
	//colpip_fillrect(buffer, dx, dy, x + pos.x, y + pos.y, pos.width, pos.height, color);
	litehtml::el_td::draw_background(hdc, x, y, clip, ri);
}

void freqel::get_content_size(size &sz, int max_width)
{
	TP();
	sz = size(200, 50);
}

void freqel::get_text(string &text)
{
	TP();
	text = "14030000";
}

bool freqel::on_mouse_over()
{
	TP();
	return false;
}
bool freqel::on_mouse_leave()
{
	TP();
	return false;
}
bool freqel::on_lbutton_down()
{
	TP();
	return false;
}
bool freqel::on_lbutton_up()
{
	TP();
	return false;
}
void freqel::on_click()
{
	TP();
}

litehtml::element::ptr hftrxgd::create_element(const char *tag_name, const litehtml::string_map &attributes, const std::shared_ptr<litehtml::document> &doc)
{
	try
	{
		std::string id = attributes.at("id");
		std::string classname = attributes.at("class");
		PRINTF("create_element: tag_name='%s', id='%s', class='%s'\n", tag_name, id.c_str(), classname.c_str());

		if (id == "FREQ_A")
		{
			auto newTag = std::make_shared<freqel>(doc, 0);
			return newTag;
		}
		if (id == "FREQ_B")
		{
			auto newTag = std::make_shared<freqel>(doc, 1);
			return newTag;
		}

	} catch (const std::out_of_range&)
	{
		//PRINTF("create_element: tag_name='%s', id not found\n", tag_name);
	}
	return nullptr;
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
	media.resolution = 96; //GetDeviceCaps(m_tmp_hdc, LOGPIXELSX);
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

void hftrxgd::link(const std::shared_ptr<litehtml::document> &doc, const litehtml::element::ptr &el)
{
	//TP();
}

}	// namespace

static const char hftrx_css [] =
		R"##(
html {
	display: block;
}
head {
	display: none
}
meta {
	display: none
}
title {
	display: none
}
link {
	display: none
}
style {
	display: none
}
script {
	display: none
}
body {
	display:block; 
	margin:0px; 
}
p {
	display:block; 
	margin-top:1em; 
	margin-bottom:1em;
}
b, strong {
	display:inline; 
	font-weight:bold;
}
i, em, cite {
	display:inline; 
	font-style:italic;
}
ins, u {
	text-decoration:underline
}
del, s, strike {
	text-decoration:line-through
}
center 
{
	text-align:center;
	display:block;
}
a:link
{
	text-decoration: underline;
	color: #00f;
	cursor: pointer;
}
h1, h2, h3, h4, h5, h6, div {
	display:block;
}
h1 {
	font-weight:bold; 
	margin-top:0.67em; 
	margin-bottom:0.67em; 
	font-size: 2em;
}
h2 {
	font-weight:bold; 
	margin-top:0.83em; 
	margin-bottom:0.83em; 
	font-size: 1.5em;
}
h3 {
	font-weight:bold; 
	margin-top:1em; 
	margin-bottom:1em; 
	font-size:1.17em;
}
h4 {
	font-weight:bold; 
	margin-top:1.33em; 
	margin-bottom:1.33em
}
h5 {
	font-weight:bold; 
	margin-top:1.67em; 
	margin-bottom:1.67em;
	font-size:.83em;
}
h6 {
	font-weight:bold; 
	margin-top:2.33em; 
	margin-bottom:2.33em;
	font-size:.67em;
} 
br {
	display:inline-block;
}
br[clear="all"]
{
	clear:both;
}
br[clear="left"]
{
	clear:left;
}
br[clear="right"]
{
	clear:right;
}
span {
	display:inline
}
img {
	display: inline-block;
}

img[align="right"]
{
	float: right;
}
img[align="left"]
{
	float: left;
}
hr {
	display: block;
	margin-top: 0px;
	margin-bottom: 0px;
	margin-left: 0px;
	margin-right: 0px;
	border-style: inset;
	border-width: 0px
}


/***************** TABLES ********************/

table {
	display: table;
	border-collapse: separate;
	border-spacing: 0px;
	border-top-color:gray;
	border-left-color:gray;
	border-bottom-color:black;
	border-right-color:black;
	font-size: medium;
	font-weight: normal;
	font-style: normal;
}

tbody, tfoot, thead {
	display:table-row-group;
	vertical-align:middle;
}
tr {
	display: table-row;
	vertical-align: inherit;
	border-color: inherit;
}
td, th {
	display: table-cell;
	vertical-align: inherit;
	border-width:0px;
	padding:0px;
}
th {
	font-weight: bold;
}
table[border] {
	border-style:solid;
}
table[border^="0"] {
	border-style:none;
}
table[border] td, table[border] th {
	border-style:solid;
	border-top-color:black;
	border-left-color:black;
	border-bottom-color:gray;
	border-right-color:gray;
}
table[border^="0"] td, table[border^="0"] th {
	border-style:none;
}

table[align=left] {
   float: left;
}

table[align=right] {
   float: right;
}

table[align=center] {
   margin-left: auto;
   margin-right: auto;
}

caption {
	display: table-caption;
}

td[nowrap], th[nowrap] {
	white-space:nowrap;
}

tt, code, kbd, samp {
	font-family: monospace
}

pre, xmp, plaintext, listing {
	display: block;
	font-family: monospace;
	white-space: pre;
	margin: 1em 0
}

/***************** LISTS ********************/

ul, menu, dir {
	display: block;
	list-style-type: disc;
	margin-top: 1em;
	margin-bottom: 1em;
	margin-left: 0;
	margin-right: 0;
	padding-left: 40px
}

ol {
	display: block;
	list-style-type: decimal;
	margin-top: 1em;
	margin-bottom: 1em;
	margin-left: 0;
	margin-right: 0;
	padding-left: 40px
}

li {
	display: list-item;
}

ul ul, ol ul {
	list-style-type: circle;
}

ol ol ul, ol ul ul, ul ol ul, ul ul ul {
	list-style-type: square;
}

dd {
	display: block;
	margin-left: 40px;
}

dl {
	display: block;
	margin-top: 0;
	margin-bottom: 1em;
	margin-left: 0;
	margin-right: 0;
}

dt {
	display: block;
}

ol ul, ul ol, ul ul, ol ol {
	margin-top: 0;
	margin-bottom: 0
}

blockquote {
	display: block;
	margin-top: 1em;
	margin-bottom: 1em;
	margin-left: 40px;
	margin-right: 40px;
}

/*********** FORM ELEMENTS ************/

form {
	display: block;
	margin-top: 0em;
}

option {
	display: none;
}

input, textarea, keygen, select, button, isindex {
	margin: 0em;
	color: initial;
	line-height: normal;
	text-transform: none;
	text-indent: 0;
	text-shadow: none;
	display: inline-block;
}
input[type="hidden"] {
	display: none;
}


article, aside, footer, header, hgroup, nav, section 
{
	display: block;
}

sub {
	vertical-align: sub;
	font-size: smaller;
}

sup {
	vertical-align: super;
	font-size: smaller;
}

figure {
	display: block;
	margin-top: 1em;
	margin-bottom: 1em;
	margin-left: 40px;
	margin-right: 40px;
}

figcaption {
	display: block;
}

)##";

static const char hftrx_layout_800x480 [] =
R"##(
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
	<head>
		<style>
			td {
			}
		</style>
		
	</head> 
	<body style="background-color:black; color:green;">
		<!-- 800 x 480 - 50 columns by 16 pixels, 32 rows by 15 pixels  -->
		<table style="width:100%" border="0px" cellspacing="0px" cellpadding="0px" height="15px">
			<!-- row 0 -->
			<tr>
				<td colspan="2">1</td>
				<td colspan="12" rowspan="2">2</td>
				<td>3</td>
			</tr>
			<!-- row 1 -->
			<tr>
				<td>4</td>
				<td class="BIGFREQ" id="FREQ_A">5</td>
				<td>6</td>
			</tr>
			<!-- row 2 -->
			<tr>
				<td>7</td>
				<td>8</td>
				<td>9</td>
			</tr>
			<!-- row 3 -->
			<tr>
				<td> </td>
				<td> </td>
				<td> </td>
				<td> </td>
				<td> </td>
				<td> </td>
			</tr>
			<!-- row 4 -->
			<tr>
				<td></td>
			</tr>
			<!-- row 5 -->
			<tr>
				<td></td>
			</tr>
			<!-- row 6 -->
			<tr>
				<td></td>
			</tr>
			<!-- row 7 -->
			<tr>
			<td>X</td>
				<td colspan="3">X</td>
				<td>X</td>
				<td>X</td>
				<td>X</td>
				<td>X</td>
				<td>X</td>
				<td>X</td>
			</tr>
		</table>
		
	</body>
</html>

)##";

void litehtmltest(void)
{
	using namespace litehtml;
	static hftrxgd cont(DIM_X, DIM_Y);
	static const litehtml::position wndclip(0, 0, DIM_X, DIM_Y);

	//static hftrxgd cont2(DIM_X * 2, DIM_Y * 2);
	//static const position wndclip2(0, 0, DIM_X * 2, DIM_Y *2 );
	uint_ptr hdc = 0;
	// see doc/document_createFromString.txt
	auto doc = litehtml::document::createFromString(hftrx_layout_800x480, & cont, hftrx_css);
	//auto doc2 = document::createFromString(htmlString, & cont2);
	//element el_freq = doc->
	doc->render(wndclip.width, litehtml::render_all);
	//doc2->render(wndclip2.width, render_all);

	doc->draw(hdc, 0, 0, & wndclip);
	colmain_nextfb();
	TP();

	position::vector redraw_boxes;
	doc->on_lbutton_down(110, 110, 0, 0, redraw_boxes);
	local_delay_ms(2500);
	for (;;)
		;
}
#endif /* LCDMODE_LTDC */
