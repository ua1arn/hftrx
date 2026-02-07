#include "../../arch/aw_a733/allwnr_a733.h"


void sendch(void)
{
#if 1
	while ((UART0->UART_USR & (1u << 1)) == 0)	// TX FIFO Not Full
		;
	UART0->UART_RBR_THR_DLL = '#';
#endif
	//int CODE = 0x03;
	//__set_CP(15, 0, CODE, 12, 0, 2);	// RMR_EL1 - work okay
	//__set_CP(15, 3, CODE, 12, 0, 2);	// RMR_EL2 - UndefHandler
	//__set_CP(15, 6, CODE, 12, 0, 2);	// RMR_EL3 - UndefHandler

#if 1
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
