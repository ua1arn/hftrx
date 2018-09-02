// rbf2c.cpp : Defines the entry point for the console application.
//

#include <stdio.h>

// Функция разворота младших восьми бит
static unsigned revbits8(unsigned v)
{
	unsigned b = v & 0xff;
	b = ((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16; 
	return b & 0xff;
}


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
		fprintf(fpo, "static const FLASHMEMINIT uint16_t rbfimage [] =\n");
		fprintf(fpo, "{\n");
		for (;;)
		{
			int c1, c2;
			unsigned pv;

			c1 = fgetc(fpi);
			if (c1 == EOF)
				break;
			++ bytes;
			c2 = fgetc(fpi);
			if (c2 == EOF)
				pv = revbits8((unsigned char) c1) * 256 | 0xff;
			else
			{
				++ bytes;
				pv = revbits8((unsigned char) c1) * 256 | revbits8((unsigned char) c2);
			}
			fprintf(fpo, " 0x%04X,%s", pv, ((pos + 1) >= ROWSOZE) ? "\n" : "");
			pos = (pos + 1) % ROWSOZE;
			++ words;
		}
		if (pos != 0)
			fprintf(fpo, "\n");
		fprintf(fpo, "};\n");
		fprintf(fpo, "/* %u bytes converted to %u words. */\n", bytes, words);
		fclose(fpo);
		//fprintf(stdout, "static const size_t rbflength = sizeof rbfimage / sizeof rbfimage [0]; /* %u 16-bit words (0x%08lx)*/\n", words, words);
		fprintf(stderr, "file '%s', %u bytes converted to %u words.\n", argv [1], bytes, words);
	}
	return 0;
}
