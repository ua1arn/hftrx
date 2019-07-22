#include <stdio.h>
#include "hardware.h"

extern "C" { void debug_printf_P(const char *format, ... ); }

class test
{
public:
	test()
	{
		debug_printf_P("*********** test @%p\n", this);
	}
	~test()
	{
		debug_printf_P("----------- test @%p\n", this);
	}
};

static class test gltest;
static RAMDTCM  class test  gltest2;
static RAMBIG  class test  gltest3;


extern "C" { void ttt(void); }

void ttt()
{
	test t1;
	test t2;

}

