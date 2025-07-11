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

static unsigned getbit(const uint8_t * data, unsigned bitpos, unsigned w, unsigned vieww, unsigned h)
{
	const unsigned origrow = bitpos / vieww;	// vertical point
	const unsigned origbit = bitpos % vieww;	// horisontal point
	const unsigned origbytesinrow = (w + 7) / 8;

	if (bitpos >= (w * h))
		return 0;
	return (data [origrow * origbytesinrow + origbit / 8] >> (origbit % 8)) & 0x01;
}

// return number of packed bytes
unsigned createrasterchunk(FILE * fp, int vch, unsigned voffset, const uint8_t * raster, int w, int vieww, int h)
{
	const unsigned fullbitsize = vieww * h;	// output raster capacity
	const unsigned charsinrow = 16;
	unsigned dumppos = 0;
	unsigned i;
	unsigned bytes = 0;

	unsigned acc = 0;
	unsigned accbits = 0;
	fprintf(fp, "\t" "/* ch offset = %u, code = 0x%02X */\n", voffset, vch);
	for (i = 0; i < fullbitsize; ++ i)
	{
		acc = (acc << 1) | getbit(raster, i, w, vieww, h);	// peek original raster bit
		if (++ accbits >= 8)
		{
			if (dumppos == 0)
				fprintf(fp, "\t");
			int endofline = (dumppos + 1) >= charsinrow;
			fprintf(fp, "0x%02X,%s", acc, endofline ? "\n" : " ");
			++ bytes;
			if (++ dumppos >= charsinrow)
			{
				dumppos = 0;
			}
			acc = 0;
			accbits = 0;
		}
	}	
	if (accbits != 0)
	{
		accbits <<= 8 - accbits;
		if (dumppos == 0)
			fprintf(fp, "\t");
		int endofline = 1;
		fprintf(fp, "0x%02X,%s", acc, endofline ? "\n" : " ");
		++ bytes;
		if (++ dumppos >= charsinrow)
		{
			dumppos = 0;
		}
	}
	else if (dumppos != 0)
	{
		fprintf(fp, "\n");
	}
	return bytes;
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

static void createglyphdesc(FILE * fp, int id, unsigned offset, int w, int h, int ch)
{
    //{.bitmap_index = 0, .adv_w = 29, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
	if (ch == EOF)
	{
		fprintf(fp, "\t" "{.bitmap_index = %u, .adv_w = %u, .box_w = %d, .box_h = %u, .ofs_x = 0, .ofs_y = 0}," "\n",
			offset, w * 16, w, h);
	}
	else if (isprint(ch))
	{
		fprintf(fp, "\t" "{.bitmap_index = %u, .adv_w = %u, .box_w = %d, .box_h = %u, .ofs_x = 0, .ofs_y = 0}, /* ch='%c' */" "\n",
			offset, w * 16, w, h, ch);
	}
	else
	{
		fprintf(fp, "\t" "{.bitmap_index = %u, .adv_w = %u, .box_w = %d, .box_h = %u, .ofs_x = 0, .ofs_y = 0}, /* ch=0x%02X */" "\n",
			offset, w * 16, w, h, ch);
	}
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

struct sortpair
{
	unsigned char c;
	unsigned char ix;
};

static int compchar(const void * left, const void * right)
{
	unsigned l = ((const struct sortpair *) left)->c;
	unsigned r = ((const struct sortpair *) right)->c;

	if (l > r)
		return 1;
	if (l < r)
		return - 1;
	return 0;
}

void makefont(const char * keysymbol, const char * fontname, const unsigned char * rasterarray, unsigned nchars, const char * outfilename, 
	  unsigned w, unsigned h, 
	  const char * nssymbols,
	  unsigned (* wxlate)(char cc)
	  )
{
	struct sortpair * symbols = NULL;
	unsigned startchar = 0x20;
	if (nssymbols != NULL && strlen(nssymbols) != nchars)
	{
		fprintf(stderr, "Wrong number of charsacters (bitmap %u, set %u)\n", nchars, strlen(nssymbols));
		return;
	}

	FILE * fp = fopen(outfilename, "wt");
	if (fp == NULL)
	{
		fprintf(stderr, "Can not open output file '%s'\n", outfilename);
		return;
	}

	if (nssymbols != NULL)
	{
		symbols = (struct sortpair * ) calloc(nchars, sizeof (struct sortpair));
		unsigned i;
		for (i = 0; i < nchars; ++ i)
		{
			symbols [i].c = nssymbols [i];
			symbols [i].ix = i;
		}
		qsort(symbols, nchars, sizeof (struct sortpair), compchar); 
		startchar = symbols [0].c;
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
	unsigned i;
	unsigned dstoffset = 0;
	
	for (i = 0; i < nchars; ++ i)
	{
		const int srcrowbytes = (w + 7) / 8;
		const int srcrows = h;
		const unsigned c = symbols ? symbols [i].c : startchar + i;
		const unsigned vieww = wxlate ? (* wxlate)(c) : w;
		const unsigned srcindex = symbols ? symbols [i].ix : i;
		const unsigned srcoffset = srcindex * srcrowbytes * srcrows;
		offsets [i] = dstoffset;
		dstoffset += createrasterchunk(fp, c, dstoffset, rasterarray + srcoffset, w, vieww, h);
	}
	createbitmapend(fp);

	/* GLYPH DESCRIPTION */
	createglyphdescbegin(fp);
	createglyphdesc(fp, 0, 0, 0, 0, EOF);
	for (i = 0; i < nchars; ++ i)
	{
		const unsigned c = symbols ? symbols [i].c : startchar + i;
		const unsigned vieww = wxlate ? (* wxlate)(c) : w;
		createglyphdesc(fp, i + 1, offsets [i], vieww, h, c);
	}
	createglyphdescend(fp);

	if (symbols != NULL)
	{
		/* sparce text */
		fprintf(fp, "/*---------------------" "\n");
 		fprintf(fp, "*  CHARACTER MAPPING" "\n");
 		fprintf(fp, "*--------------------*/" "\n");
		fprintf(fp, "" "\n");
		fprintf(fp, "static const uint16_t unicode_list_0[] = {" "\n");

		unsigned i;
		for (i = 0; i < nchars; ++ i)
		{
			const unsigned c = symbols [i].c;
			fprintf(fp, "\t" "0x%02X," "\n", c - startchar);
		}

		fprintf(fp, "};" "\n");
		fprintf(fp, "" "\n");
	}

	/* Collect the unicode lists and glyph_id offsets */
	createcharmappingbegin(fp);
	if (symbols == NULL)
	{
		fprintf(fp, "\t" "\t" ".range_start = %d, .range_length = %d, .glyph_id_start = 1," "\n", startchar, nchars);
		fprintf(fp, "\t" "\t" ".unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY" "\n");
	}
	else
	{
		fprintf(fp, "\t" "\t" ".range_start = 0x%02X, .range_length = %d, .glyph_id_start = 1," "\n", startchar, symbols [nchars - 1].c - startchar + 1);
		fprintf(fp, "\t" "\t" ".unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = %u, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY" "\n", nchars);
	}
	createcharmappingend(fp);

	createcformatdesc(fp);

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
	free(symbols);
	free(offsets);
	fprintf(stderr, "%s\n", fontname);
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


#if defined (BIGCHARW_NARROW) && defined (BIGCHARW)
unsigned bigfont_width(char cc)
{
	return (cc == '.' || cc == '#') ? BIGCHARW_NARROW  : BIGCHARW;	// полная ширина символа в пикселях
}
#endif /* defined (BIGCHARW_NARROW) && defined (BIGCHARW) */

#if defined (HALFCHARW)
unsigned halffont_width(char cc)
{
	(void) cc;
	return HALFCHARW;	// полная ширина символа в пикселях
}
#endif /* defined (HALFCHARW) */


int main(int argc, char* argv[])
{
	makefont("LV_FONT_LTDC_CENTURYGOTHIC_BIG", "ltdc_CenturyGothic_big", ltdc_CenturyGothic_big [0][0],  ARRAY_SIZE(ltdc_CenturyGothic_big), PREFIX "ltdc_CenturyGothic_big.c", 36, 56, symbols, bigfont_width);
	makefont("LV_FONT_LTDC_CENTURYGOTHIC_HALF", "ltdc_CenturyGothic_half", ltdc_CenturyGothic_half [0][0], ARRAY_SIZE(ltdc_CenturyGothic_half), PREFIX "ltdc_CenturyGothic_half.c", 28, 56, symbols, halffont_width);
	makefont("LV_FONT_EPSON_LTDC_BIG", "Epson_LTDC_big", S1D13781_bigfont_LTDC [0][0],  ARRAY_SIZE(S1D13781_bigfont_LTDC), PREFIX "Epson_LTDC_big.c", 36, 56, symbols, bigfont_width);
	makefont("LV_FONT_EPSON_LTDC_HALF", "Epson_LTDC_half", ltdc_CenturyGothic_half [0][0], ARRAY_SIZE(ltdc_CenturyGothic_half), PREFIX "Epson_LTDC_half.c", 28, 56, symbols, halffont_width);
	makefont("LV_FONT_EPSON_LTDC_SMALL", "Epson_LTDC_small", S1D13781_smallfont_LTDC [0][0], ARRAY_SIZE(S1D13781_smallfont_LTDC), PREFIX "Epson_LTDC_small.c", 16, 15, NULL, NULL);
	makefont("LV_FONT_EPSON_LTDC_SMALL_RU", "Epson_LTDC_small_RU", S1D13781_smallfont_RU_LTDC [0][0], ARRAY_SIZE(S1D13781_smallfont_RU_LTDC), PREFIX "Epson_LTDC_small_RU.c", 10, 15, NULL, NULL);
	makefont("LV_FONT_EPSON_LTDC_SMALL2", "Epson_LTDC_small2", S1D13781_smallfont2_LTDC [0][0], ARRAY_SIZE(S1D13781_smallfont2_LTDC), PREFIX "Epson_LTDC_small2.c", 10, 16, NULL, NULL);
	makefont("LV_FONT_EPSON_LTDC_SMALL3", "Epson_LTDC_small3", S1D13781_smallfont3_LTDC [0], ARRAY_SIZE(S1D13781_smallfont3_LTDC), PREFIX "Epson_LTDC_small3.c", 8, 8, NULL, NULL);
	return 0;
}
