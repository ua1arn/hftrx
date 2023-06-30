#include "../../arch/t113s3/allwnr_t113s3.h"

void dbg_puts(const char * s);
void dbg_putchar(uint_fast8_t c);

static void usdelay(int v)
{
	volatile int i;
	for (i = 0; i < v; ++ i)
		;
}
void dbg_putchar(uint_fast8_t c)
{
	if (c == '\n')
		dbg_putchar('\r');
	//usdelay(8);
	while ((UART0->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		;
	//usdelay(8);
	UART0->UART_RBR_THR_DLL = c;
	//usdelay(8);
}

static const char hex [16] = "0123456789ABCDEF";

void printv32(uintptr_t v)
{
	dbg_putchar(hex [(v >> 28) & 0x0F]);
	dbg_putchar(hex [(v >> 24) & 0x0F]);
	dbg_putchar(hex [(v >> 20) & 0x0F]);
	dbg_putchar(hex [(v >> 16) & 0x0F]);
	dbg_putchar(hex [(v >> 12) & 0x0F]);
	dbg_putchar(hex [(v >> 8) & 0x0F]);
	dbg_putchar(hex [(v >> 4) & 0x0F]);
	dbg_putchar(hex [(v >> 0) & 0x0F]);
}

void printv8(uintptr_t v)
{
	dbg_putchar(hex [(v >> 4) & 0x0F]);
	dbg_putchar(hex [(v >> 0) & 0x0F]);
}

void dbg_puts(const char * s)
{
	char c;

	while ((c = * s ++) != '\0')
		dbg_putchar(c);
}

void dbg_printhex(const uint8_t * p, int size)
{
	unsigned offs = 0;

//	printv32((uintptr_t) p);
//	dbg_putchar(' ');

	while (size > 0)
	{
		printv32((uintptr_t) p);

		dbg_putchar(' ');
		printv8(p [0]);
		dbg_putchar(' ');
		printv8(p [1]);
		dbg_putchar(' ');
		printv8(p [2]);
		dbg_putchar(' ');
		printv8(p [3]);
		dbg_putchar(' ');
		printv8(p [4]);
		dbg_putchar(' ');
		printv8(p [5]);
		dbg_putchar(' ');
		printv8(p [6]);
		dbg_putchar(' ');
		printv8(p [7]);
		dbg_putchar(' ');
		printv8(p [8]);
		dbg_putchar(' ');
		printv8(p [9]);
		dbg_putchar(' ');
		printv8(p [10]);
		dbg_putchar(' ');
		printv8(p [11]);
		dbg_putchar(' ');
		printv8(p [12]);
		dbg_putchar(' ');
		printv8(p [13]);
		dbg_putchar(' ');
		printv8(p [14]);
		dbg_putchar(' ');
		printv8(p [15]);

		dbg_putchar('\n');

		p += 16;
		size -= 16;

	}
}
//char msg0 [] = "Hello, Word (I am HiFi4 DSP)!\n";
//char msg0 [] = "Hello, Word!\n";
char msg0 [] = "eHello qqqqqqqqqqqewwwwwww ";
void xmain(int v1, int v2)
{
	int z;
	(void) v1;
	(void) v2;


	dbg_putchar('H');
	dbg_putchar('e');
	dbg_putchar('l');
	dbg_putchar('l');
	dbg_putchar('o');
	dbg_putchar('!');
	dbg_putchar('\n');

	printv32((uintptr_t) & z);
	dbg_putchar('\n');

	printv32((uintptr_t) msg0);
	dbg_putchar('\n');

	dbg_printhex(msg0, sizeof msg0);

	dbg_putchar('H');
	dbg_putchar('e');
	dbg_putchar('l');
	dbg_putchar('l');
	dbg_putchar('o');
	dbg_putchar('2');
	dbg_putchar('!');
	dbg_putchar('\n');

    dbg_puts(msg0);
    dbg_putchar('\n');
    dbg_puts(msg0);
    dbg_putchar('\n');
    dbg_puts(msg0);
    dbg_putchar('\n');
    //dbg_puts("Hello, Word (I am HiFi4 DSP)!\n");


	dbg_putchar('H');
	dbg_putchar('e');
	dbg_putchar('l');
	dbg_putchar('l');
	dbg_putchar('o');
	dbg_putchar('3');
	dbg_putchar('!');
	dbg_putchar('\n');

    for (;;)
		;
}
