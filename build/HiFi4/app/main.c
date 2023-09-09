
#include "../../../arch/t113s3/allwnr_t113s3.h"
//-------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>


void dbg_puts(const char * s);
void dbg_putchar(uint_fast8_t c);

void UART_putc(uint_fast8_t c)
{
	if (c == '\n')
	{
		UART_putc('\r');

	}

	while ((UART0->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		;
	UART0->UART_RBR_THR_DLL = c;
}


void UART_puts(const char *s)
{
 while(*s)UART_putc(*s++);
}

void DelayUS(unsigned us)
{
	while (us --)
		;//_NOP();
	return;
// unsigned t0=AVS_CNT0_REG;
// while(AVS_CNT0_REG-t0<(24UL/4)*us);
}

void DelayMS(unsigned ms)
{
 DelayUS(ms*1000);
}

void delay(volatile uint64_t d)
{
 while(d--);
}

const char HEX[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

char *puthex(char *a,unsigned h)
{
 *a++='0';
 *a++='x';
 *a++=HEX[(h>>28)&0xF];
 *a++=HEX[(h>>24)&0xF];
 *a++=HEX[(h>>20)&0xF];
 *a++=HEX[(h>>16)&0xF];
 *a++=HEX[(h>>12)&0xF];
 *a++=HEX[(h>> 8)&0xF];
 *a++=HEX[(h>> 4)&0xF];
 *a++=HEX[(h    )&0xF];
 *a++='\n';
 *a=0;
 return a-11;
}

void foo1(int v1,int v2)
{
 UART_puts("foo 1\n");

 int z='!';
(void)v1;
(void)v2;

 Loop:

 UART_putc(z);

 //delay(50000);
 DelayMS(100);

 goto Loop;
}

void foo0(int v1,int v2)
{
 UART_puts("\nfoo 0\n");

 int z;

 (void)v1;
 (void)v2;

 char *abuf=alloca(24);
 if(abuf==NULL)UART_puts("alloca error!\n");
 else          UART_puts("alloca OK!\n");

 strcpy(abuf,"alloca String\n");
 UART_puts(abuf);

 foo1(7,8);
}

      char msg0[]="\nNOT CONST STRING!\n"; //.data

const char msg1[] ="CONST STRING!\n";      //.rodata

char bss;				   //.bss

extern char __heap_start;
extern char __heap_end;
extern char __stack;

int main(void)
{
 UART_putc('+');

 UART_putc(bss+'0'); //must be 0 (.bss was cleared in libc_init)

 bss=7;
 UART_putc(bss+'0'); //must be 7

 UART_puts(msg0);    //must be correct text (not garbage)

 UART_puts(msg1);    //must be correct text (not garbage)

 char a[20];          //.stack

 UART_puts(puthex(a,0x12345678)); //must be correct text

 UART_puts(puthex(a,(unsigned)&__stack));

 UART_puts(puthex(a,(unsigned)&__heap_start));
 UART_puts(puthex(a,(unsigned)&__heap_end));

#if 1

 char *buf=malloc(64);
 if(buf==NULL)UART_puts("malloc error!\n");
 else         UART_puts("malloc OK!\n");

 memset(buf,0,64);

 strcpy(buf,"malloc String\n");
 UART_puts(buf);

 sprintf(buf,"Stack: 0x%X, Heap Start: 0x%X, Heap End: 0x%X",(unsigned)&__stack,(unsigned)&__heap_start,(unsigned)&__heap_end);
 UART_puts(buf);

 free(buf);

#endif

 foo0(0,0);

 while(1);

 return 0;
}
