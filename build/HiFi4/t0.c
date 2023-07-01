#include "../../arch/t113s3/allwnr_t113s3.h"

void dbg_puts(const char * s);
void dbg_putchar(uint_fast8_t c);

void dbg_putchar(uint_fast8_t c)
{
	while ((UART0->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		;
	UART0->UART_RBR_THR_DLL = c;
	if (c == '\n')
	{
		c = '\r';
		while ((UART0->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
			;
		UART0->UART_RBR_THR_DLL = c;

	}
}

const char hex [] = "0123456789ABCDEFghijklmnopqrst";

char ahex(unsigned v)
{
	return (v < 10) ? (v + '0') : (v + 'A' - 10);
}

void printv32(uintptr_t v)
{
	dbg_putchar(ahex((v >> 28) & 0x0F));
	dbg_putchar(ahex((v >> 24) & 0x0F));
	dbg_putchar(ahex((v >> 20) & 0x0F));
	dbg_putchar(ahex((v >> 16) & 0x0F));
	dbg_putchar(ahex((v >> 12) & 0x0F));
	dbg_putchar(ahex((v >> 8) & 0x0F));
	dbg_putchar(ahex((v >> 4) & 0x0F));
	dbg_putchar(ahex((v >> 0) & 0x0F));
}

void printv8(uintptr_t v)
{
	dbg_putchar(ahex((v >> 4) & 0x0F));
	dbg_putchar(ahex((v >> 0) & 0x0F));
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
char msg0 [] = "eHello qqqqqqqqqqqewwwwwww1234566 ";
void xmain(int v1, int v2)
{
	int z;
	(void) v1;
	(void) v2;

//
//	dbg_putchar('H');
//	dbg_putchar('e');
//	dbg_putchar('l');
//	dbg_putchar('l');
//	dbg_putchar('o');
//	dbg_putchar('!');
//	dbg_putchar('\n');
//
//	printv32((uintptr_t) & z);
//	dbg_putchar('\n');

	extern uint32_t __dram_base [];
	extern uint32_t __iram_base [];
	volatile const uint32_t * const p = (volatile const uint32_t *) __dram_base;

	{
		enum { row = 0 };
		printv32(& p [row * 4 + 0]);
		dbg_putchar(':');
		printv32(p [row * 4 + 0]);
		dbg_putchar(',');
		printv32(p [row * 4 + 1]);
		dbg_putchar(',');
		printv32(p [row * 4 + 2]);
		dbg_putchar(',');
		printv32(p [row * 4 + 3]);
		dbg_putchar('\n');
	}
	{
		enum { row = 1 };
		printv32(& p [row * 4 + 0]);
		dbg_putchar(':');
		printv32(p [row * 4 + 0]);
		dbg_putchar(',');
		printv32(p [row * 4 + 1]);
		dbg_putchar(',');
		printv32(p [row * 4 + 2]);
		dbg_putchar(',');
		printv32(p [row * 4 + 3]);
		dbg_putchar('\n');
	}
	{
		enum { row = 2 };
		printv32(& p [row * 4 + 0]);
		dbg_putchar(':');
		printv32(p [row * 4 + 0]);
		dbg_putchar(',');
		printv32(p [row * 4 + 1]);
		dbg_putchar(',');
		printv32(p [row * 4 + 2]);
		dbg_putchar(',');
		printv32(p [row * 4 + 3]);
		dbg_putchar('\n');
	}
	{
		enum { row = 3 };
		printv32(& p [row * 4 + 0]);
		dbg_putchar(':');
		printv32(p [row * 4 + 0]);
		dbg_putchar(',');
		printv32(p [row * 4 + 1]);
		dbg_putchar(',');
		printv32(p [row * 4 + 2]);
		dbg_putchar(',');
		printv32(p [row * 4 + 3]);
		dbg_putchar('\n');
	}
	{
		enum { row = 4 };
		printv32(& p [row * 4 + 0]);
		dbg_putchar(':');
		printv32(p [row * 4 + 0]);
		dbg_putchar(',');
		printv32(p [row * 4 + 1]);
		dbg_putchar(',');
		printv32(p [row * 4 + 2]);
		dbg_putchar(',');
		printv32(p [row * 4 + 3]);
		dbg_putchar('\n');
	}
	{
		enum { row = 5 };
		printv32(& p [row * 4 + 0]);
		dbg_putchar(':');
		printv32(p [row * 4 + 0]);
		dbg_putchar(',');
		printv32(p [row * 4 + 1]);
		dbg_putchar(',');
		printv32(p [row * 4 + 2]);
		dbg_putchar(',');
		printv32(p [row * 4 + 3]);
		dbg_putchar('\n');
	}
	{
		enum { row = 6 };
		printv32(& p [row * 4 + 0]);
		dbg_putchar(':');
		printv32(p [row * 4 + 0]);
		dbg_putchar(',');
		printv32(p [row * 4 + 1]);
		dbg_putchar(',');
		printv32(p [row * 4 + 2]);
		dbg_putchar(',');
		printv32(p [row * 4 + 3]);
		dbg_putchar('\n');
	}
	{
		enum { row = 7 };
		printv32(& p [row * 4 + 0]);
		dbg_putchar(':');
		printv32(p [row * 4 + 0]);
		dbg_putchar(',');
		printv32(p [row * 4 + 1]);
		dbg_putchar(',');
		printv32(p [row * 4 + 2]);
		dbg_putchar(',');
		printv32(p [row * 4 + 3]);
		dbg_putchar('\n');
	}
	{
		enum { row = 8 };
		printv32(& p [row * 4 + 0]);
		dbg_putchar(':');
		printv32(p [row * 4 + 0]);
		dbg_putchar(',');
		printv32(p [row * 4 + 1]);
		dbg_putchar(',');
		printv32(p [row * 4 + 2]);
		dbg_putchar(',');
		printv32(p [row * 4 + 3]);
		dbg_putchar('\n');
	}
	{
		enum { row = 9 };
		printv32(& p [row * 4 + 0]);
		dbg_putchar(':');
		printv32(p [row * 4 + 0]);
		dbg_putchar(',');
		printv32(p [row * 4 + 1]);
		dbg_putchar(',');
		printv32(p [row * 4 + 2]);
		dbg_putchar(',');
		printv32(p [row * 4 + 3]);
		dbg_putchar('\n');
	}
	{
		enum { row = 10 };
		printv32(& p [row * 4 + 0]);
		dbg_putchar(':');
		printv32(p [row * 4 + 0]);
		dbg_putchar(',');
		printv32(p [row * 4 + 1]);
		dbg_putchar(',');
		printv32(p [row * 4 + 2]);
		dbg_putchar(',');
		printv32(p [row * 4 + 3]);
		dbg_putchar('\n');
	}
	{
		enum { row = 11 };
		printv32(& p [row * 4 + 0]);
		dbg_putchar(':');
		printv32(p [row * 4 + 0]);
		dbg_putchar(',');
		printv32(p [row * 4 + 1]);
		dbg_putchar(',');
		printv32(p [row * 4 + 2]);
		dbg_putchar(',');
		printv32(p [row * 4 + 3]);
		dbg_putchar('\n');
	}

	//dbg_printhex(msg0, sizeof msg0);
	//dbg_printhex((void *) __dram_base, 256);

    dbg_puts(msg0);
    dbg_putchar('\n');
    dbg_puts(msg0);
    dbg_putchar('\n');
    dbg_puts(msg0);
    dbg_putchar('\n');
    dbg_puts("Hello, Word (I am HiFi4 DSP)!\n");

    for (;;)
		;
}
