// bin2ihex.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "getopt_win.h"

static void
print2hex(unsigned v)
{
    printf("%02X", v & 0xFF);
}

static void 
hexdata(unsigned long address, unsigned char type, const unsigned char * data, unsigned length)
{
    unsigned cks = 0;
    unsigned i;
    cks += (length >> 0) & 0xFF;
    cks += (address >> 8) & 0xFF;
    cks += (address >> 0) & 0xFF;
    cks += (type >> 0) & 0xFF;
    for (i = 0; i < length; ++ i)
    {
        cks += data [i];
    }
    printf(":");
    print2hex(length);
    print2hex(address >> 8);
    print2hex(address >> 0);
    print2hex(type);
    for (i = 0; i < length; ++ i)
    {
        print2hex(data [i]);
    }
    print2hex(0 - cks);
    printf("\n");
}

static unsigned long getsplit(unsigned long address)
{
    return address >> 16;
}

// :020000042000DA
// :020000042001D9
// :020000042003D7
// :02 0000 04 2003 D7
static void printsegment(unsigned long address)
{
    unsigned char buff [2];

    buff [0] = (unsigned char) (address >> 24);
    buff [1] = (unsigned char) (address >> 16);
    hexdata(0, 0x04, buff, 2);
}

static void startlinearaddress(unsigned long address)
{
    unsigned char buff [4];

    buff [0] = (unsigned char) (address >> 24);
    buff [1] = (unsigned char) (address >> 16);
    buff [2] = (unsigned char) (address >> 8);
    buff [3] = (unsigned char) (address >> 0);
    hexdata(0, 0x05, buff, 4);
}


static void
processfile(FILE * fp, unsigned long address, unsigned long runaddress)
{
    enum { ROWSIZE = 32 };
    unsigned long pageoffset = 0;
    int c;
    unsigned length;
    unsigned char buff [ROWSIZE];
    length = 0;
    for (;;)
    {
        c = fgetc(fp);
        if (c == EOF)
            break;
        buff [length] = (unsigned char) c;
        if (++ length >= ROWSIZE)
        {
            if (getsplit(pageoffset) != getsplit(address))
            {
                printsegment(address);
                pageoffset = address;
            }
            hexdata(address, 0x00, buff, length);
            address += length;
            length = 0;
        }
    }
    if (length != 0)
    {
        if (getsplit(pageoffset) != getsplit(address))
        {
            printsegment(address);
            pageoffset = address;
        }
        hexdata(address, 0x00, buff, length);
        address += length;
        length = 0;
    }
    // Finbalize
    if (1)
    {
        hexdata(runaddress, 0x01, NULL, 0);

    }
    else
    {
        if (runaddress < 0x10000uL && pageoffset == 0)
            hexdata(runaddress, 0x01, NULL, 0);
        else
            startlinearaddress(runaddress);
    }
}

int main(int argc, char * * argv)
{
    unsigned long loadaddr = 0;
    FILE * fpi;
 	int opt;
	char *dest = NULL, *src = NULL;

	while ((opt = getopt(argc, argv, ":l:s:")) != -1) {
		switch (opt) {
		case 's':
			src = optarg;
			break;
		case 'd':
			dest = optarg;
			break;
		case 'l':
			loadaddr = strtoul(optarg, NULL, 16);
			break;
		default:
			
				fprintf(stderr, "Usage :\n");
				fprintf(stderr, " %s -s srcfile -l loadaddr\n", argv[0]);
			return -1;
		}
	}

    fpi = fopen(src, "rb");
    if (fpi == NULL)
        return 1;
    processfile(fpi, loadaddr, 0);
    fclose(fpi);
    return 0;
}