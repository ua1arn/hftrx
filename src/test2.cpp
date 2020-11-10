
#include <new>
#include <stdio.h>
#include <stddef.h>

#include <cxxabi.h>

#include "hardware.h"
#include "formats.h"

#if 0

class tcl0 {
public:
	tcl0() {
		PRINTF("*********** tcl0 @%p\n", this);
	}
	~tcl0() {
		PRINTF("----------- tcl0 @%p\n", this);
	}
};

//static class tcl0 gltest;
//static RAMDTCM class tcl0 gltest2;
//static RAMBIG class tcl0 gltest3;

static RAMDTCM char xxxx [32];

extern "C" {
	void cpptest(void);
}

void cpptest() {
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

}

#endif

// Taken from https://wiki.osdev.org/C++#The_Operators_.27new.27_and_.27delete.27

#if WITHUSEMALLOC

void *operator new(size_t size) {
	return malloc(size);
}

void *operator new[](size_t size) {
	return malloc(size);
}

void operator delete(void *p) {
	free(p);
}

void operator delete[](void *p) {
	free(p);
}

#endif /* WITHUSEMALLOC */

std::bad_alloc::~bad_alloc()
{
	ASSERT(0);
	for (;;)
		;
}

std::exception::~exception()
{
	ASSERT(0);
	for (;;)
		;
}

const char* std::bad_alloc::what() const
{
	return NULL;
}

extern "C" {

	void __cxa_pure_virtual(void)
	{
		// Do nothing or print an error message.
	}

	int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso)
	{

		return 0; /*I would prefer if functions returned 1 on success, but the ABI says...*/
	}

	void __cxa_finalize(void *f)
	{

	}

	// See: https://habr.com/ru/post/279151/
	// See: https://github.com/shuobeige11/rn-web/blob/aace070ad8c064b762d7ac96bad8657baabb30b1/third-party/folly-2016.09.26.00/folly/experimental/exception_tracer/ExceptionTracerLib.cpp

	void __cxa_begin_catch()
	{
	    //printf("begin FTW\n");
	}

	void __cxa_end_catch()
	{
	    //printf("end FTW\n");
	}

	void __cxa_rethrow()
	{
		ASSERT(0);
		for (;;)
			;
	}
	void __cxa_throw_bad_array_length()
	{
		ASSERT(0);
		for (;;)
			;
	}

	void __cxa_throw_bad_array_new_length()
	{
		ASSERT(0);
		for (;;)
			;
	}

	void __cxa_throw(
	    void* thrownException,
	    std::type_info* type,
	    void (*destructor)(void*))
	{
		ASSERT(0);
		for (;;)
			;
	}

	void * __cxa_allocate_exception(size_t thrown_size) throw()
	{
		ASSERT(0);
		for (;;)
			;
		return NULL;
	}
}
