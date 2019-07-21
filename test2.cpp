#include <stdio.h>
#include "armcpu/stm32h7xx.h"

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

class test2
{
public:
	test2()
	{
		debug_printf_P("*********** test2 @%p\n", this);
	}
};

#if 0
static class test2 gltest;
static __attribute__((section(".dtcm")))  class test2  gltest2;
static __attribute__((section(".ram_d1")))  class test2  gltest3;

#endif
extern "C" { void ttt(void); }

void ttt()
{
	test t1;
	test t2;

}

