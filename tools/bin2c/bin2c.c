
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
		//fprintf(fpo, "static const uint16_t rbfimage [] =\n");
		//fprintf(fpo, "{\n");
		for (;;)
		{
			int c1;
			unsigned pv;

			c1 = fgetc(fpi);
			if (c1 == EOF)
				break;
			fprintf(fpo, " 0x%02X,%s", c1, ((pos + 1) >= ROWSOZE) ? "\n" : "");
			pos = (pos + 1) % ROWSOZE;
			++ bytes;
		}
		if (pos != 0)
			fprintf(fpo, "\n");
		//fprintf(fpo, "};\n");
		fprintf(fpo, "/* %u bytes converted */\n", bytes);
		fclose(fpo);
		//fprintf(stdout, "static const size_t rbflength = sizeof rbfimage / sizeof rbfimage [0]; /* %u 16-bit words (0x%08lx)*/\n", words, words);
		fprintf(stderr, "file '%s', %u bytes converted.\n", argv [1], bytes);
	}
	return 0;
}
