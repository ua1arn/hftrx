#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern int main(void);

/*
extern void (*__preinit_array_start []) (void) __attribute__((weak));
extern void (*__preinit_array_end []) (void) __attribute__((weak));
extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));
extern void (*__fini_array_start []) (void) __attribute__((weak));
extern void (*__fini_array_end []) (void) __attribute__((weak));

static void __libc_init_array(void)
{
    size_t count, i;

    count = __preinit_array_end - __preinit_array_start;
    for (i = 0; i < count; i++)
        __preinit_array_start[i]();

    count = __init_array_end - __init_array_start;
    for (i = 0; i < count; i++)
         __init_array_start[i]();
}
*/

extern char _sidata,_sdata,_edata; //set by linker
extern char _sbss,_ebss;           //set by linker

void libc_init(void)
{
 memcpy(&_sdata,&_sidata,&_edata-&_sdata); //copy LMA to VMA
 memset(&_sbss,0,&_ebss-&_sbss);           //clear BSS
 main();
}

#if 1

#include <reent.h>
#include <errno.h>

extern char __heap_start; //set by linker
extern char __heap_end;   //set by linker

struct _reent *_EXFUN(__getreent,(void))
{
	static struct _reent REENT; //memory space for srand() & rand()
	return &REENT;
}

void *_sbrk(int incr)
{
 static char *heap_end=NULL;

 char        *prev_heap_end;

 if(!heap_end)heap_end=&__heap_start;

 prev_heap_end=heap_end;
 heap_end+=incr;

 if(heap_end<(&__heap_end))return (void*)prev_heap_end;

 errno=ENOMEM;
 return (void*)-1;
}

void *_DEFUN(_sbrk_r,(ptr, incr),struct _reent *ptr _AND ptrdiff_t incr)
{
 char *ret;

 errno=0;

 if((ret=(char*)(_sbrk(incr)))==(void*)-1&&errno!=0)ptr->_errno=errno;

 return ret;
}

#endif
