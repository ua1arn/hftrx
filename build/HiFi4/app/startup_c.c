#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

extern int main(void);

extern void *_sidata,*_sdata,*_edata;
extern void *_sbss,*_ebss;

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

void libc_init(void)
{
 void **pSource,**pDest;

 for(pSource=&_sidata,pDest=&_sdata;pDest!=&_edata;pSource++,pDest++)*pDest=*pSource; //copy LMA to VMA
 for(pDest=&_sbss;pDest!=&_ebss;pDest++)*pDest=0;                                     //clear BSS

// __libc_init_array(); //constructor init (C++)

 main();
}

#if 1

#include <reent.h>
#include <errno.h>

extern char __heap_start; //set by linker
extern char __heap_end;   //set by linker

struct _reent *_EXFUN(__getreent,(void))
{
 return (struct _reent*)0;
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
