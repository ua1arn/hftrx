//aarch64 test

#include "../../arch/aw_a64/device.h"

void _start(void)
{
	while ((UART0->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		;
	UART0->UART_RBR_THR_DLL = '#';
	for (;;)
		;
}
