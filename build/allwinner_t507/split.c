#include <stdio.h>
#include <string.h>

void copyfile(FILE * dst, FILE * src, int uptolength)
{
	int c;

//	rewind(src);
	for (;uptolength > 0;)
	{
		enum { CHUNK = 1024 };
		size_t n;
		unsigned char b [CHUNK];
		n = fread(b, 1, CHUNK, src);
		if (n == 0)
			break;
		fwrite(b, 1, n, dst);
		uptolength -= n;
	}
}


int main(int argc, char * * argv)
{

	unsigned startoffset = 0x1004000;
	//unsigned startoffset = 0x2000;
	unsigned cutlength = 256 * 1024;
	FILE * fi;
	FILE * fo;
	const char * srcfilename = "Y:\\ua1arn\\ubuntu-t507.img";
	const char * dstfilename = "sunxihead.bin";
	fi = fopen(srcfilename, "rb");
	if (fi == NULL)
	{
		fprintf(stderr, "No source file '%s'\n", srcfilename);
		return 1;
	}
	fo = fopen(dstfilename, "wb");
	if (fo == NULL)
	{
		fprintf(stderr, "No destination file '%s'\n", dstfilename);
		return 1;
	}
	if (fi == NULL || fo == NULL)
		return 1;

	fseek(fi, startoffset, SEEK_SET);
	copyfile(fo, fi, cutlength);

	fclose(fi);
	fclose(fo);

	return 0;
}
