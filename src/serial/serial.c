/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "serial.h"
#include "formats.h"	// for debug prints
#include "board.h"
#include "gpio.h"
#include <string.h>
#include <math.h>

// Set interrupt vector wrapper
void serial_set_handler(uint_fast16_t int_id, void (* handler)(void))
{
#if WITHNMEAOVERREALTIME
		arm_hardware_set_handler_overrealtime(int_id, handler);
#else /* WITHNMEAOVERREALTIME */
		arm_hardware_set_handler_system(int_id, handler);
#endif /* WITHNMEAOVERREALTIME */
}


#if CPUSTYLE_R7S721

// scemr:
// b0=1: 1: Base clock frequency is 8 times the bit rate,
// b0=0: 0: Base clock frequency is 16 times the bit rate
// scmsr:
// b1..b0: 0: /1, 1: /4, 2: /16, 3: /64
enum
{
	SCEMR_x16 = 0x00,	// ABCS=0
	SCEMR_x8 = 0x01,	// ABCS=1
	SCSMR_DIV1 = 0x00,
	SCSMR_DIV4 = 0x01,
	SCSMR_DIV16 = 0x02,
	SCSMR_DIV64 = 0x03,
};

const FLASHMEM struct spcr_spsr_tag scemr_scsmr [] =
{
	{ SCEMR_x8, 	SCSMR_DIV1, },		/* /8 = 8 * 1 */
	{ SCEMR_x16, 	SCSMR_DIV1, }, 		/* /16 = 16 * 1 */
	{ SCEMR_x8, 	SCSMR_DIV4, },		/* /32 = 8 * 4 */
	{ SCEMR_x16, 	SCSMR_DIV4, },		/* /64 = 16 * 4 */
	{ SCEMR_x8, 	SCSMR_DIV16, }, 	/* /128 = 8 * 16 */
	{ SCEMR_x16, 	SCSMR_DIV16, }, 	/* /256 = 16 * 16 */
	{ SCEMR_x8, 	SCSMR_DIV64, },  	/* /512 = 8 * 64 */
	{ SCEMR_x16, 	SCSMR_DIV64, }, 	/* /1024 = 16 * 64 */
};

#endif /* CPUSTYLE_R7S721 */



