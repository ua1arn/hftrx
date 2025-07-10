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

static void createrasterchunk(FILE * fp, int ch, unsigned offset, const uint8_t * raster, int w, int h)
{
	int hbytes = (w + 7) / 8;
	int rows = h;
	int dumppos = 0;
	int i;
	int fullsize = hbytes * rows;
	int charsinrow = 16;

	fprintf(fp, "\t" "/* ch offset = %u, code = 0x%02X */\n", offset, (unsigned) ch);
	for (i = 0; i < fullsize; ++ i)
	{
		if (dumppos == 0)
			fprintf(fp, "\t");
		int endofline = (i + 1) >= fullsize || (dumppos + 1) >= charsinrow;

		fprintf(fp, "0x%02X,%s", raster [i], endofline ? "\n" : " ");
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

	};
	writestrings(fp, strings, ARRAY_SIZE(strings));
}

static void createcharmapping(FILE * fp, int rangestart, int rangelength)
{
	fprintf(fp, "\t" "\t" ".range_start = %d, .range_length = %d, .glyph_id_start = 1," "\n", rangestart, rangelength);
	fprintf(fp, "\t" "\t" ".unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY" "\n");
}


void makefont(const char * name, const unsigned char * rasterarray, int nchars, const char * outfilename, int w, int h, int startchar)
{
	FILE * fp = fopen(outfilename, "wt");
	if (fp == NULL)
	{
		fprintf(stderr, "Can not open output file '%s'\n", outfilename);


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
		createrasterchunk(fp, startchar + i, offset, rasterarray + offset, w, h);
	}
	createbitmapend(fp);

	createglyphdescbegin(fp);
	createglyphdesc(fp, 0, 0, w, h);
	for (i = 0; i < nchars; ++ i)
	{
		createglyphdesc(fp, i + 1, offsets [i], w, h);
	}

	createglyphdescend(fp);

	createcharmappingbegin(fp);
	createcharmapping(fp, startchar, nchars);
	createcharmappingend(fp);
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
int main(int argc, char* argv[])
{
	makefont("ltdc_CenturyGothic_big", ltdc_CenturyGothic_big [0][0],  ARRAY_SIZE(ltdc_CenturyGothic_big), PREFIX "ltdc_CenturyGothic_big.c", 36, 56, 0);
	makefont("ltdc_CenturyGothic_half", ltdc_CenturyGothic_half [0][0], ARRAY_SIZE(ltdc_CenturyGothic_half), PREFIX "ltdc_CenturyGothic_half.c", 28, 56, 0);
	makefont("Epson_LTDC_big", S1D13781_bigfont_LTDC [0][0],  ARRAY_SIZE(S1D13781_bigfont_LTDC), PREFIX "Epson_LTDC_big.c", 36, 56, 0);
	makefont("Epson_LTDC_half", ltdc_CenturyGothic_half [0][0], ARRAY_SIZE(ltdc_CenturyGothic_half), PREFIX "Epson_LTDC_half.c", 28, 56, 0);
	makefont("Epson_LTDC_small", S1D13781_smallfont_LTDC [0][0], ARRAY_SIZE(S1D13781_smallfont_LTDC), PREFIX "Epson_LTDC_small.c", 16, 15, 0x20);
	makefont("Epson_LTDC_small2", S1D13781_smallfont2_LTDC [0][0], ARRAY_SIZE(S1D13781_smallfont2_LTDC), PREFIX "Epson_LTDC_small2.c", 10, 16, 0x20);
	makefont("Epson_LTDC_small3", S1D13781_smallfont3_LTDC [0], ARRAY_SIZE(S1D13781_smallfont3_LTDC), PREFIX "Epson_LTDC_small3.c", 8, 8, 0x20);
	return 0;
}
