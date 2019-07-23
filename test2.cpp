#if 0

#include <new>
#include <stdio.h>
#include <stddef.h>
#include "hardware.h"

extern "C" { void debug_printf_P(const char *format, ... ); }

class tcl0
{
public:
	tcl0()
	{
		debug_printf_P("*********** tcl0 @%p\n", this);
	}
	~tcl0()
	{
		debug_printf_P("----------- tcl0 @%p\n", this);
	}
};
/*
static class tcl0 gltest;
static RAMDTCM  class tcl0  gltest2;
static RAMBIG  class tcl0  gltest3;
*/
static RAMDTCM char xxxx [32];
extern "C" { void cpptest(void); }

void cpptest()
{
#if 0
	tcl0 t1;
	tcl0 t2;
	tcl0 * p1 = new tcl0;
	tcl0 * p2 = new tcl0;
	tcl0 * p3 = new tcl0;
	tcl0 * p4 = new (reinterpret_cast<void *>(xxxx)) tcl0;

	delete p1;
	delete p2;
	delete p3;
	p4->~tcl0();
#endif
}

// Taken from https://wiki.osdev.org/C++#The_Operators_.27new.27_and_.27delete.27

void *operator new(size_t size)
{
    return malloc(size);
}

void *operator new[](size_t size)
{
    return malloc(size);
}

void operator delete(void *p)
{
    free(p);
}

void operator delete[](void *p)
{
    free(p);
}



#endif
