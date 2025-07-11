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
		"",
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

static unsigned getbit(const uint8_t * data, unsigned bitpos, unsigned w, unsigned h)
{
	const unsigned origrow = bitpos / w;	// vertical point
	const unsigned origbit = bitpos % w;	// horisontal point
	const unsigned origbytesinrow = (w + 7) / 8;

	if (bitpos >= (w * h))
		return 0;
	return (data [origrow * origbytesinrow + origbit / 8] >> (origbit % 8)) & 0x01;
}

// Collect byte
static unsigned getrasterbyte(const uint8_t * data, unsigned bytepos, unsigned w, unsigned h)
{
	unsigned r = 0;
	//unsigned row = bytepos / hbytes;
	//unsigned rowbypepos = bytepos % hbytes;
	r |= (1u << 7) * !! getbit(data, bytepos * 8 + 0, w, h);
	r |= (1u << 6) * !! getbit(data, bytepos * 8 + 1, w, h);
	r |= (1u << 5) * !! getbit(data, bytepos * 8 + 2, w, h);
	r |= (1u << 4) * !! getbit(data, bytepos * 8 + 3, w, h);
	r |= (1u << 3) * !! getbit(data, bytepos * 8 + 4, w, h);
	r |= (1u << 2) * !! getbit(data, bytepos * 8 + 5, w, h);
	r |= (1u << 1) * !! getbit(data, bytepos * 8 + 6, w, h);
	r |= (1u << 0) * !! getbit(data, bytepos * 8 + 7, w, h);
	return r;
}

static void createrasterchunk(FILE * fp, int ch, unsigned offset, const uint8_t * raster, int w, int h)
{
	const unsigned hbytes = (w + 7) / 8;
	const unsigned fullsize = hbytes * h;
	const unsigned charsinrow = 16;
	unsigned dumppos = 0;
	unsigned i;

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

void makefont(const char * keysymbol, const char * fontname, const unsigned char * rasterarray, int nchars, const char * outfilename, 
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

	{
		fprintf(fp, "#ifndef %s" "\n", keysymbol);
		fprintf(fp, "#define %s 1" "\n", keysymbol);
		fprintf(fp, "#endif" "\n");
		fprintf(fp, "" "\n");
		fprintf(fp, "#if %s" "\n", keysymbol);
		fprintf(fp, "" "\n");
	}

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

	{
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
	}

	{
		fprintf(fp, "#endif /* %s */" "\n", keysymbol);
		fprintf(fp, "" "\n");
	}

	fclose(fp);
	free(offsets);
}

//LV_FONT_DECLARE(ltdc_CenturyGothic_big)			// width=36, height=56
//LV_FONT_DECLARE(ltdc_CenturyGothic_half)		// width=28, height=56
//LV_FONT_DECLARE(Epson_LTDC_big)			// width=36, height=56
//LV_FONT_DECLARE(Epson_LTDC_half)		// width=28, height=56
//LV_FONT_DECLARE(Epson_LTDC_small_RU)	// width=10, height=15
//LV_FONT_DECLARE(Epson_LTDC_small)		// width=16, height=15
//LV_FONT_DECLARE(Epson_LTDC_small2)		// width=10, height=16
//LV_FONT_DECLARE(Epson_LTDC_small3)		// width=8, height=8

#include "ltdc_CenturyGothic_big.h"
#include "ltdc_CenturyGothic_half.h"
#include "S1D13781_font_big_LTDC.h"
#include "S1D13781_font_half_LTDC.h"
#include "S1D13781_font_small_LTDC.h"
#include "S1D13781_font_small_RU_LTDC.h"
#include "S1D13781_font_small2_LTDC.h"
#include "S1D13781_font_small3_LTDC.h"

#define PREFIX "../../src/display/fonts/LVGL/"

static const char symbols [] = "0123456789_ .";
int main(int argc, char* argv[])
{
	makefont("LV_FONT_LTDC_CENTURYGOTHIC_BIG", "ltdc_CenturyGothic_big", ltdc_CenturyGothic_big [0][0],  ARRAY_SIZE(ltdc_CenturyGothic_big), PREFIX "ltdc_CenturyGothic_big.c", 36, 56, 0, symbols, strlen(symbols));
	makefont("LV_FONT_LTDC_CENTURYGOTHIC_HALF", "ltdc_CenturyGothic_half", ltdc_CenturyGothic_half [0][0], ARRAY_SIZE(ltdc_CenturyGothic_half), PREFIX "ltdc_CenturyGothic_half.c", 28, 56, 0, symbols, strlen(symbols));
	makefont("LV_FONT_EPSON_LTDC_BIG", "Epson_LTDC_big", S1D13781_bigfont_LTDC [0][0],  ARRAY_SIZE(S1D13781_bigfont_LTDC), PREFIX "Epson_LTDC_big.c", 36, 56, 0, symbols, strlen(symbols));
	makefont("LV_FONT_EPSON_LTDC_HALF", "Epson_LTDC_half", ltdc_CenturyGothic_half [0][0], ARRAY_SIZE(ltdc_CenturyGothic_half), PREFIX "Epson_LTDC_half.c", 28, 56, 0, symbols, strlen(symbols));
	makefont("LV_FONT_EPSON_LTDC_SMALL", "Epson_LTDC_small", S1D13781_smallfont_LTDC [0][0], ARRAY_SIZE(S1D13781_smallfont_LTDC), PREFIX "Epson_LTDC_small.c", 16, 15, 0x20, NULL, 0);
	makefont("LV_FONT_EPSON_LTDC_SMALL_RU", "Epson_LTDC_small_RU", S1D13781_smallfont_RU_LTDC [0][0], ARRAY_SIZE(S1D13781_smallfont_RU_LTDC), PREFIX "Epson_LTDC_small_RU.c", 10, 15, 0x20, NULL, 0);
	makefont("LV_FONT_EPSON_LTDC_SMALL2", "Epson_LTDC_small2", S1D13781_smallfont2_LTDC [0][0], ARRAY_SIZE(S1D13781_smallfont2_LTDC), PREFIX "Epson_LTDC_small2.c", 10, 16, 0x20, NULL, 0);
	makefont("LV_FONT_EPSON_LTDC_SMALL3", "Epson_LTDC_small3", S1D13781_smallfont3_LTDC [0], ARRAY_SIZE(S1D13781_smallfont3_LTDC), PREFIX "Epson_LTDC_small3.c", 8, 8, 0x20, NULL, 0);
	return 0;
}
