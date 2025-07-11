// makelvglfonts.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

typedef unsigned char uint8_t;
#define FLASHMEM 
#define ARRAY_SIZE(a) ((sizeof (a) / sizeof (a)[0]))

static void writestrings(FILE * fp, const char * const * table, unsigned n)
{
	while (n --)
		fprintf(fp, "%s\n", * table ++);
}

static void createfileheader(FILE * fp)
{
	static const char * const strings [] =
	{
		"#ifdef LV_LVGL_H_INCLUDE_SIMPLE",
		"#include \"lvgl.h\"",
		"#else",
		"#include \"lvgl/lvgl.h\"",
		"#endif",
	};
	writestrings(fp, strings, ARRAY_SIZE(strings));
}

static void createbitmapbegin(FILE * fp)
{
	static const char * const strings [] =
	{
		"/*-----------------",
		" *    BITMAPS",
		" *----------------*/",
		"",
		"/*Store the image of the glyphs*/",
		"static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {",
	};
	writestrings(fp, strings, ARRAY_SIZE(strings));
}

static void createbitmapend(FILE * fp)
{
	static const char * const strings [] =
	{
		"};",
		"",
	};
	writestrings(fp, strings, ARRAY_SIZE(strings));
}

const unsigned char revbittable [256] =
{
	0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
	0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
	0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
	0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
	0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
	0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
	0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
	0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
	0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
	0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
	0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
	0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
	0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
	0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
	0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
	0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF,
};

static unsigned getbit(const uint8_t * data, unsigned bitpos, unsigned w, unsigned h)
{
	if (bitpos >= (w * h))
		return 0;
	return (data [bitpos / 8] >> (7 - bitpos % 8)) & 0x01;
}

static unsigned getrasterbyte(const uint8_t * data, unsigned bytepos, unsigned w, unsigned h)
{
	unsigned r = 0;
	//unsigned row = bytepos / hbytes;
	//unsigned rowbypepos = bytepos % hbytes;
	r |= (1u << 0) * getbit(data, bytepos * 8 + 0, w, h);
	r |= (1u << 1) * getbit(data, bytepos * 8 + 1, w, h);
	r |= (1u << 2) * getbit(data, bytepos * 8 + 2, w, h);
	r |= (1u << 3) * getbit(data, bytepos * 8 + 3, w, h);
	r |= (1u << 4) * getbit(data, bytepos * 8 + 4, w, h);
	r |= (1u << 5) * getbit(data, bytepos * 8 + 5, w, h);
	r |= (1u << 6) * getbit(data, bytepos * 8 + 6, w, h);
	r |= (1u << 7) * getbit(data, bytepos * 8 + 7, w, h);
	return r;
}

static void createrasterchunk(FILE * fp, int ch, unsigned offset, const uint8_t * raster, int w, int h)
{
	unsigned hbytes = (w + 7) / 8;
	unsigned i;
	unsigned fullsize = hbytes * h;
	unsigned charsinrow = 16;
	unsigned dumppos = 0;

	fprintf(fp, "\t" "/* ch offset = %u, code = 0x%02X */\n", offset, (unsigned) ch);
	for (i = 0; i < fullsize; ++ i)
	{
		if (dumppos == 0)
			fprintf(fp, "\t");
		int endofline = (i + 1) >= fullsize || (dumppos + 1) >= charsinrow;

		fprintf(fp, "0x%02X,%s", getrasterbyte(raster, i, w, h), endofline ? "\n" : " ");
		if (++ dumppos >= charsinrow)
		{
			dumppos = 0;
		}
		
	}	
}

static void createglyphdescbegin(FILE * fp)
{
	static const char * const strings [] =
	{
		"/*---------------------",
		" *  GLYPH DESCRIPTION",
		" *--------------------*/",
		"",
		"static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {",
	};
	writestrings(fp, strings, ARRAY_SIZE(strings));
}

static void createglyphdescend(FILE * fp)
{
	static const char * const strings [] =
	{
		"};",
		"",
	};
	writestrings(fp, strings, ARRAY_SIZE(strings));
}

static void createglyphdesc(FILE * fp, int id, unsigned offset, int w, int h)
{
    //{.bitmap_index = 0, .adv_w = 29, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    fprintf(fp, "\t" "{.bitmap_index = %u, .adv_w = %u, .box_w = %d, .box_h = %u, .ofs_x = 0, .ofs_y = 0}," "\n",
		offset, w * 16, w, h);
}

static void createcharmappingbegin(FILE * fp)
{
	static const char * const strings [] =
	{
		"/*---------------------",
		" *  CHARACTER MAPPING",
		" *--------------------*/",
		"",
		"/*Collect the unicode lists and glyph_id offsets*/",
		"static const lv_font_fmt_txt_cmap_t cmaps[] =",
		"{",
		"\t" "{",
	};
	writestrings(fp, strings, ARRAY_SIZE(strings));
}
static void createcharmappingend(FILE * fp)
{
	static const char * const strings [] =
	{
		"\t" "},",
		"};",
		"",
	};
	writestrings(fp, strings, ARRAY_SIZE(strings));
}

static void createcharmapping(FILE * fp, int rangestart, int rangelength, const char * symbols, int nsymbols)
{
	if (symbols == NULL)
	{
		fprintf(fp, "\t" "\t" ".range_start = %d, .range_length = %d, .glyph_id_start = 1," "\n", rangestart, rangelength);
		fprintf(fp, "\t" "\t" ".unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY" "\n");
	}
	else
	{
		fprintf(fp, "\t" "\t" ".range_start = %d, .range_length = %d, .glyph_id_start = 1," "\n", rangestart, rangelength);
		fprintf(fp, "\t" "\t" ".unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY" "\n");
	}
}


static void createcformatdesc(FILE * fp)
{
	static const char * const strings [] =
	{
		"/*--------------------",
		" *  ALL CUSTOM DATA",
		" *--------------------*/",
		"",
		"#if LVGL_VERSION_MAJOR == 8",
		"/*Store all the custom data of the font*/",
		"static  lv_font_fmt_txt_glyph_cache_t cache;",
		"#endif",
		"",
		"#if LVGL_VERSION_MAJOR >= 8",
		"static const lv_font_fmt_txt_dsc_t font_dsc = {",
		"#else",
		"static lv_font_fmt_txt_dsc_t font_dsc = {",
		"#endif",
		"    .glyph_bitmap = glyph_bitmap,",
		"    .glyph_dsc = glyph_dsc,",
		"    .cmaps = cmaps,",
		"    .kern_dsc = NULL,",
		"    .kern_scale = 0,",
		"    .cmap_num = 1,",
		"    .bpp = 1,",
		"    .kern_classes = 0,",
		"    .bitmap_format = 0,",
		"#if LVGL_VERSION_MAJOR == 8",
		"    .cache = &cache",
		"#endif",
		"};",
	};
	writestrings(fp, strings, ARRAY_SIZE(strings));
}

static void createcfonrdescbegin(FILE * fp)
{
}

static void createcfonrdescend(FILE * fp)
{
}

void makefont(const char * fontname, const unsigned char * rasterarray, int nchars, const char * outfilename, 
	  int w, int h, 
	  int startchar,
	  const char * symbols, int nsymbols
	  )
{
	if (symbols != NULL && nsymbols != nchars)
	{
		fprintf(stderr, "Wrong number of charsacters (bitmap %u, set %u)\n", nchars, nsymbols);
		return;
	}

	FILE * fp = fopen(outfilename, "wt");
	if (fp == NULL)
	{
		fprintf(stderr, "Can not open output file '%s'\n", outfilename);
		return;
	}
	createfileheader(fp);
	createbitmapbegin(fp);
	unsigned * offsets = (unsigned *) calloc(sizeof (unsigned), nchars);
	int i;
	for (i = 0; i < nchars; ++ i)
	{
		int hbytes = (w + 7) / 8;
		int rows = h;
		unsigned offset = i * hbytes * rows;
		offsets [i] = offset;
		if (symbols != NULL)
		{
			createrasterchunk(fp, symbols [i], offset, rasterarray + offset, w, h);
		}
		else
		{
			createrasterchunk(fp, startchar + i, offset, rasterarray + offset, w, h);
		}
	}
	createbitmapend(fp);

	/* GLYPH DESCRIPTION */
	createglyphdescbegin(fp);
	createglyphdesc(fp, 0, 0, w, h);
	for (i = 0; i < nchars; ++ i)
	{
		createglyphdesc(fp, i + 1, offsets [i], w, h);
	}
	createglyphdescend(fp);

	/* Collect the unicode lists and glyph_id offsets */
	createcharmappingbegin(fp);
	createcharmapping(fp, startchar, nchars, symbols, nsymbols);
	createcharmappingend(fp);

	createcformatdesc(fp);

	createcfonrdescbegin(fp);
	//createcfonrdesc(fp, startchar, nchars);
	createcfonrdescend(fp);

#if 1

	fprintf(fp, "/*-----------------" "\n");
	fprintf(fp, " *  PUBLIC FONT" "\n");
	fprintf(fp, " *----------------*/" "\n");
	fprintf(fp, "" "\n");
	fprintf(fp, "/*Initialize a public general font descriptor*/" "\n");
	fprintf(fp, "#if LVGL_VERSION_MAJOR >= 8" "\n");
	fprintf(fp, "const lv_font_t %s = {" "\n", fontname);
	fprintf(fp, "#else" "\n");
	fprintf(fp, "lv_font_t %s = {" "\n", fontname);
	fprintf(fp, "#endif" "\n");
	fprintf(fp, "    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/" "\n");
	fprintf(fp, "    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/" "\n");
	fprintf(fp, "    .line_height = %d,          /*The maximum line height required by the font*/" "\n", h);
	fprintf(fp, "    .base_line = 0,             /*Baseline measured from the bottom of the line*/" "\n");
	fprintf(fp, "#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)" "\n");
	fprintf(fp, "    .subpx = LV_FONT_SUBPX_NONE," "\n");
	fprintf(fp, "#endif" "\n");
	fprintf(fp, "#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8" "\n");
	fprintf(fp, "    .underline_position = 0," "\n");
	fprintf(fp, "    .underline_thickness = 0," "\n");
	fprintf(fp, "#endif" "\n");
	fprintf(fp, "    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */" "\n");
	fprintf(fp, "#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9" "\n");
	fprintf(fp, "    .fallback = NULL," "\n");
	fprintf(fp, "#endif" "\n");
	fprintf(fp, "    .user_data = NULL," "\n");
	fprintf(fp, "};" "\n");
	fprintf(fp, "" "\n");
#endif
	fclose(fp);
	free(offsets);
}

//LV_FONT_DECLARE(ltdc_CenturyGothic_big)			// width=36, height=56
//LV_FONT_DECLARE(ltdc_CenturyGothic_half)		// width=28, height=56
//LV_FONT_DECLARE(Epson_LTDC_big)			// width=36, height=56
//LV_FONT_DECLARE(Epson_LTDC_half)		// width=28, height=56
//LV_FONT_DECLARE(Epson_LTDC_small_RU)	// width=16, height=15
//LV_FONT_DECLARE(Epson_LTDC_small)		// width=16, height=15
//LV_FONT_DECLARE(Epson_LTDC_small2)		// width=10, height=16
//LV_FONT_DECLARE(Epson_LTDC_small3)		// width=8, height=8

#include "ltdc_CenturyGothic_big.h"
#include "ltdc_CenturyGothic_half.h"
#include "S1D13781_font_big_LTDC.h"
#include "S1D13781_font_half_LTDC.h"
#include "S1D13781_font_small_LTDC.h"
#include "S1D13781_font_small2_LTDC.h"
#include "S1D13781_font_small3_LTDC.h"

#define PREFIX "../../src/display/fonts/"

static const char symbols [] = "0123456789_ .";
int main(int argc, char* argv[])
{
	makefont("ltdc_CenturyGothic_big", ltdc_CenturyGothic_big [0][0],  ARRAY_SIZE(ltdc_CenturyGothic_big), PREFIX "ltdc_CenturyGothic_big.c", 36, 56, 0, symbols, strlen(symbols));
	makefont("ltdc_CenturyGothic_half", ltdc_CenturyGothic_half [0][0], ARRAY_SIZE(ltdc_CenturyGothic_half), PREFIX "ltdc_CenturyGothic_half.c", 28, 56, 0, symbols, strlen(symbols));
	makefont("Epson_LTDC_big", S1D13781_bigfont_LTDC [0][0],  ARRAY_SIZE(S1D13781_bigfont_LTDC), PREFIX "Epson_LTDC_big.c", 36, 56, 0, symbols, strlen(symbols));
	makefont("Epson_LTDC_half", ltdc_CenturyGothic_half [0][0], ARRAY_SIZE(ltdc_CenturyGothic_half), PREFIX "Epson_LTDC_half.c", 28, 56, 0, symbols, strlen(symbols));
	makefont("Epson_LTDC_small", S1D13781_smallfont_LTDC [0][0], ARRAY_SIZE(S1D13781_smallfont_LTDC), PREFIX "Epson_LTDC_small.c", 16, 15, 0x20, NULL, 0);
	makefont("Epson_LTDC_small2", S1D13781_smallfont2_LTDC [0][0], ARRAY_SIZE(S1D13781_smallfont2_LTDC), PREFIX "Epson_LTDC_small2.c", 10, 16, 0x20, NULL, 0);
	makefont("Epson_LTDC_small3", S1D13781_smallfont3_LTDC [0], ARRAY_SIZE(S1D13781_smallfont3_LTDC), PREFIX "Epson_LTDC_small3.c", 8, 8, 0x20, NULL, 0);
	return 0;
}
