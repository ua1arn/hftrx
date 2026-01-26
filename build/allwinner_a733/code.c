#include "../../arch/aw_t507/allwnr_t507.h"


void sendch(void)
{
	int CODE = 0x03;
#if 0
	while ((UART0->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		;
	UART0->UART_RBR_THR_DLL = '#';
#endif
	//__set_CP(15, 0, CODE, 12, 0, 2);	// RMR_EL1 - work okay
	//__set_CP(15, 3, CODE, 12, 0, 2);	// RMR_EL2 - UndefHandler
	__set_CP(15, 6, CODE, 12, 0, 2);	// RMR_EL3 - UndefHandler

#if 0
	while ((UART0->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		;
	UART0->UART_RBR_THR_DLL = '@';
#endif
	__ISB();
	__WFI();

	for (;;)
	{
		__WFE();
	}
}
