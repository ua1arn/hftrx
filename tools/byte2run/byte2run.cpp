// byte2run.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define ROWLENGTH	8

static const char * const typenames [2] = 
{
    "PACKEDCOLORMAIN_T",
    "PACKEDCOLORPIP_T",
};

static const char * const byte2runname [2] = 
{
    "byte2runmain",
    "byte2runpip",
};


static const char * const prefixes [2] = 
{
    "COLORMAIN",
    "COLORPIP",
};



static void
onebyte(int tft565, unsigned v, const char * fg, const char * bg)
{
    const char * const pfx = prefixes [tft565];
	printf("%s_%s, ", pfx, v ? fg : bg);
}

static void
onerow(int tft565, unsigned v, const char * fg, const char * bg)
{
	unsigned pos;

	printf(" " "{ ");
	for (pos = 0; pos < ROWLENGTH; ++ pos)
	{
		onebyte(tft565, v & (1uL << pos), fg, bg);
	}
	printf("}," "\t" "/* 0x%02X */\n", v);
}

static void
onetable(int tft565, const char * fg, const char * bg)
{
    const char * const name565 = byte2runname [tft565];
    const char * const pfx = prefixes [tft565];
	unsigned taglesize = (1uL << ROWLENGTH);
	unsigned i;
	printf("/* Foreground %s_%s, background %s_%s */" "\n", pfx, fg, pfx, bg);
	printf("//extern const FLASHMEM %s %s_%s_%s_%s_%s [%u][%d]; // fg=%s_%s, bg=%s_%s\n", typenames [tft565], name565, pfx, fg, pfx, bg, taglesize, ROWLENGTH, pfx, fg, pfx, bg);
	printf("const FLASHMEM %s %s_%s_%s_%s_%s [%u][%u] =\n", typenames [tft565], name565, pfx, fg, pfx, bg, taglesize, ROWLENGTH);
	printf("{\n");
	for (i = 0; i < taglesize; ++ i)
	{
		onerow(tft565, i, fg, bg);
	}
	printf("};\n");
}

static void
oneselector(int tft565, const char * fg, const char * bg, int first)
{
    const char * const name565 = byte2runname [tft565];
    const char * const pfx = prefixes [tft565];

	printf(" " "%sif (fg == %s_%s && bg == %s_%s)" " \\" "\n", (first ? "else " : ""), prefixes [0], fg, prefixes [0], bg);
	printf(" " " " "%s = & %s_%s_%s_%s_%s;" " \\" "\n", "tgt", name565, pfx, fg, pfx, bg);
}

static void
defoneselector(int tft565, const char * fg, const char * bg, int first)
{
    const char * const name565 = byte2runname [tft565];
    const char * const pfx = prefixes [tft565];

	printf(" " "else" " \\" "\n");
	printf(" " " " "%s = & %s_%s_%s_%s_%s;" " \\" "\n", "tgt", name565, pfx, fg, pfx, bg);
}

static void svnheader(void)
{
	printf("/* $Id: $ */" "\n");
}


static const char * const array [][2] =
{
	{	"WHITE",	"BLACK"		},     // zero index used as default case
	{	"WHITE",	"GREEN"	    },
	{	"WHITE",	"DARKGREEN"	},
	{	"BLACK",	"DARKGREEN"	},
	{	"GREEN",	"BLACK"		},	// s-meter
	{	"WHITE",	"DARKRED"	},
	{	"BLACK",	"DARKRED"	},
	{	"RED",	  "BLACK"		},	// s-meter
	{	"YELLOW",   "BLACK"		},
	{	"BLACK",	"GREEN"		},
	{	"BLACK",	"RED"		},
	{	"SPECTRUMBG2",	"BLACK"		},     // цвет частоты дополнительного приемника
};


static void oneset(int tft565)
{
	int i;

	for (i = 0; i < (sizeof array / sizeof array [0]); ++ i)
	{
		onetable(tft565, array [i][0], array [i][1]);
	}
	printf("/* Use in display_setcolors function.*/" "\n");
    printf("#define %s_SELECTOR(tgt) do { \\" "\n", prefixes [tft565]);
	for (i = 1; i < (sizeof array / sizeof array [0]); ++ i)
	{
		oneselector(tft565, array [i][0], array [i][1], i > 1);
	}
	defoneselector(tft565, array [0][0], array [0][1], 0);
	printf("} while (0)" "\n");
}

int main(int argc, char* argv[])
{
 svnheader();
 //printf("#if ! LCDMODE_LTDC_PIPL8" "\n");
 oneset(0);
 //printf("#endif /* ! LCDMODE_LTDC_PIPL8 */" "\n");
 //oneset(1);

	return 0;
}


