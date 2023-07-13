#include <stdint.h>
#include "../../arch/aw_a64/cmsis_a64.h"
void _start(void)
{
	while ((UART0->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		;
	UART0->UART_RBR_THR_DLL = '#';
	for (;;)
		;
}
