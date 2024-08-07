// genbuffif.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void genonjectset(const char * obj, const char * comment)
{
	fprintf(stdout, "/* %s */\n", comment);
	fprintf(stdout, "uintptr_t allocate_dmabuffer%s(void); /* take free buffer %s */\n", obj, comment);
	fprintf(stdout, "uintptr_t getfilled_dmabuffer%s(void); /* take from queue %s */\n", obj, comment);
	fprintf(stdout, "void release_dmabuffer%s(uintptr_t addr);  /* release %s */\n", obj, comment);
	fprintf(stdout, "void save_dmabuffer%s(uintptr_t addr); /* save to queue %s */\n", obj, comment);
	fprintf(stdout, "int_fast32_t cachesize_dmabuffer%s(void); /* parameter for cache manipulation functions %s */\n", obj, comment);
	fprintf(stdout, "int_fast32_t datasize_dmabuffer%s(void); /* parameter for DMA %s */\n", obj, comment);

	fprintf(stdout, "\n");


}

struct params
{
	const char * obj;
	const char * comment;
};

static const struct params params0 [] = 
{
	{	"32rx",	"FPGA to CPU", },
	{	"32tx",	"CPU to FPGA", },
	{	"32tx_sub",	"CPU to FPGA (additional channel)", },

	{	"16rx",	"CODEC to CPU", },
	{	"16tx",	"CPU to CODEC", },

	{	"16rx8k",	"CODEC to CPU, sample rate 8000", },
	{	"16tx8k",	"CPU to CODEC, sample rate 8000", },

	{	"32rts192",	"FPGA to CPU", },

	{	"uacin48",	"usb audio48 to host", },
	{	"uacinrts96",	"usb rts96 to host", },
	{	"uacinrts192",	"usb rts192 to host", },
	{	"uacout48",	"usb audio48 from host", },

	{	"btout8k",	"BT audio to radio, sample rate 8000", },
	{	"btout16k",	"BT audio to radio, sample rate 16000", },
	{	"btout32k",	"BT audio to radio, sample rate 32000", },
	{	"btout44p1k",	"BT audio to radio, sample rate 44100", },

	{	"btin8k",	"BT audio from radio, sample rate 8000", },
	{	"btin16k",	"BT audio from radio, sample rate 16000", },
	{	"btin32k",	"BT audio from radio, sample rate 32000", },

	{	"btin44p1k",	"BT audio from radio, sample rate 44100", },
	{	"colmain0fb",	"Frame buffer for display 0", },
	{	"colmain1fb",	"Frame buffer for display 1 (HDMI)", },

	{	"eth0io",	"Ethernet0 buffers", },

};

int main(int argc, char* argv[])
{

	unsigned i;

	for (i = 0; i < sizeof params0 / sizeof params0 [0]; ++ i)
	{
		genonjectset(params0 [i].obj, params0 [i].comment);
	}
	return 0;
}
