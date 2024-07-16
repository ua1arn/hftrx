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
		unsigned int bytes = 0;
		FILE * fpo = fopen(argv [2], "wt");

		if (fpo == NULL)
			return 1;

		fprintf(fpo, "/* $Id: $ */\n");
		fprintf(fpo, "/* GENERATED FILE - DO NOT EDIT */\n");
		fprintf(fpo, "/* Source file '%s' */\n", argv [1]);
		//fprintf(fpo, "static const uint16_t rbfimage [] =\n");
		//fprintf(fpo, "{\n");
		for (;;)
		{
			int c1;
			unsigned pv;

			c1 = fgetc(fpi);
			if (c1 == EOF)
				break;
			++ bytes;
			pv = revbits8((unsigned char) c1);
			fprintf(fpo, " 0x%04X,%s", pv, ((pos + 1) >= ROWSOZE) ? "\n" : "");
			pos = (pos + 1) % ROWSOZE;
			++ bytes;
		}
		if (pos != 0)
			fprintf(fpo, "\n");
		//fprintf(fpo, "};\n");
		fprintf(fpo, "/* %u bytes converted. */\n", bytes);
		fclose(fpo);
		//fprintf(stdout, "static const size_t rbflength = sizeof rbfimage / sizeof rbfimage [0]; /* %u 16-bit bytes (0x%08lx)*/\n", bytes, bytes);
		fprintf(stderr, "file '%s', %u bytes converted.\n", argv [1], bytes);
	}
	return 0;
}
