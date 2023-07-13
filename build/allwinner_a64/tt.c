#include <stdint.h>
#include "../../arch/aw_a64/cmsis_a64.h"
void _start(void)
{
	UART0->UART_RBR_THR_DLL = '#';
	for (;;)
		;
}
