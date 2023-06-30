#include "../../arch/t113s3/allwnr_t113s3.h"

void dbg_puts(const char * s);
void dbg_putchar(uint_fast8_t c);

void dbg_putchar(uint_fast8_t c)
{
	if (c == '\n')
		dbg_putchar('\r');

	while ((UART0->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		;
	UART0->UART_RBR_THR_DLL = c;
}


void dbg_puts(const char * s)
{
	char c;

	while ((c = * s ++) != '\0')
		dbg_putchar(c);
}

//char msg0 [] = "Hello, Word (I am HiFi4 DSP)!\n";
char msg0 [] = "Hello, Word!\n";
void xmain(int v1, int v2)
{
	(void) v1;
	(void) v2;

	dbg_putchar('H');
	dbg_putchar('e');
	dbg_putchar('l');
	dbg_putchar('l');
	dbg_putchar('o');
	dbg_putchar('!');
	dbg_putchar('\n');

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
