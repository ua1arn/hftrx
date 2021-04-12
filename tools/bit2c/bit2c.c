// rbf2c.cpp : Defines the entry point for the console application.
//
// About format see
// PG374 (v1.0) June 3, 2020
// pg374-dfx-controller.pdf

#define BINOFFSET 0x0082	/* начало HEADER в исходном файле */
//
// Sample of output file header:
//
//	0xFFFFFFFF,
//	0xFFFFFFFF,
//	0x000000BB,
//	0x11220044,
//	0xFFFFFFFF,
//	0xFFFFFFFF,
//	0xAA995566,
//	0x20000000,
//	0x30022001,
//	0x00000000,

#include <stdio.h>

int main(int argc, char* argv[])
{
	FILE * fpi;

	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s inputfile outfile\n", argv [0]);
		return 1;
	}


	fpi = fopen(argv [1], "rb");
	if (fpi != NULL)
	{
		enum { ROWSOZE = 8 };
		int pos = 0;
		unsigned int words = 0;
		unsigned int bytes = 0;
		FILE * fpo = fopen(argv [2], "wt");

		if (fpo == NULL)
			return 1;

		fprintf(fpo, "/* $Id: $ */\n");
		fprintf(fpo, "/* GENERATED FILE - DO NOT EDIT */\n");
		fprintf(fpo, "/* Source file '%s' */\n", argv [1]);
		//fprintf(fpo, "static const FLASHMEMINIT uint16_t rbfimage [] =\n");
		//fprintf(fpo, "{\n");
		fseek(fpi, BINOFFSET, SEEK_SET);
		for (;;)
		{
			int c1, c2, c3, c4;
			unsigned long pv;

			c1 = fgetc(fpi);
			if (c1 == EOF)
				break;
			++ bytes;
			c2 = fgetc(fpi);
			if (c2 == EOF)
				c2 = 0xFF;
			else
				++ bytes;
			c3 = fgetc(fpi);
			if (c3 == EOF)
				c3 = 0xFF;
			else
				++ bytes;
			c4 = fgetc(fpi);
			if (c4 == EOF)
				c4 = 0xFF;
			else
				++ bytes;

			pv = (
				((unsigned long) c1 << 24) |
				((unsigned long) c2 << 16) |
				((unsigned long) c3 << 8) |
				((unsigned long) c4 << 0) |
				0);
			
			fprintf(fpo, " 0x%08lX,%s", pv, ((pos + 1) >= ROWSOZE) ? "\n" : "");
			pos = (pos + 1) % ROWSOZE;
			++ words;
		}
		if (pos != 0)
			fprintf(fpo, "\n");
		//fprintf(fpo, "};\n");
		fprintf(fpo, "/* %u bytes converted to %u words. */\n", bytes, words);
		fclose(fpo);
		//fprintf(stdout, "static const size_t rbflength = sizeof rbfimage / sizeof rbfimage [0]; /* %u 16-bit words (0x%08lx)*/\n", words, words);
		fprintf(stderr, "file '%s', %u bytes converted to %u words.\n", argv [1], bytes, words);
	}
	return 0;
}
