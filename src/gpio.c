// $Id$
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"
#include "gpio.h"

#include <string.h>
#include <math.h>

#if CPUSTYLE_STM32F || CPUSTYLE_STM32MP1 || (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)
// Перенос каждого бита в байте в позицию с увеличенным в 4 раза номером.
portholder_t
power4(uint_fast8_t v)
{
#if 1
	portholder_t r = 0;

	r |= (v & (1U << 0)) ? (1uL << 0) : 0;
	r |= (v & (1U << 1)) ? (1uL << 4) : 0;
	r |= (v & (1U << 2)) ? (1uL << 8) : 0;
	r |= (v & (1U << 3)) ? (1uL << 12) : 0;
	r |= (v & (1U << 4)) ? (1uL << 16) : 0;
	r |= (v & (1U << 5)) ? (1uL << 20) : 0;
	r |= (v & (1U << 6)) ? (1uL << 24) : 0;
	r |= (v & (1U << 7)) ? (1uL << 28) : 0;

	return r;
#else
	static const FLASHMEM uint_fast32_t tablepow4 [256] =
	{
	 0x00000000UL,  0x00000001UL,  0x00000010UL,  0x00000011UL,  0x00000100UL,  0x00000101UL,  0x00000110UL,  0x00000111UL,
	 0x00001000UL,  0x00001001UL,  0x00001010UL,  0x00001011UL,  0x00001100UL,  0x00001101UL,  0x00001110UL,  0x00001111UL,
	 0x00010000UL,  0x00010001UL,  0x00010010UL,  0x00010011UL,  0x00010100UL,  0x00010101UL,  0x00010110UL,  0x00010111UL,
	 0x00011000UL,  0x00011001UL,  0x00011010UL,  0x00011011UL,  0x00011100UL,  0x00011101UL,  0x00011110UL,  0x00011111UL,
	 0x00100000UL,  0x00100001UL,  0x00100010UL,  0x00100011UL,  0x00100100UL,  0x00100101UL,  0x00100110UL,  0x00100111UL,
	 0x00101000UL,  0x00101001UL,  0x00101010UL,  0x00101011UL,  0x00101100UL,  0x00101101UL,  0x00101110UL,  0x00101111UL,
	 0x00110000UL,  0x00110001UL,  0x00110010UL,  0x00110011UL,  0x00110100UL,  0x00110101UL,  0x00110110UL,  0x00110111UL,
	 0x00111000UL,  0x00111001UL,  0x00111010UL,  0x00111011UL,  0x00111100UL,  0x00111101UL,  0x00111110UL,  0x00111111UL,
	 0x01000000UL,  0x01000001UL,  0x01000010UL,  0x01000011UL,  0x01000100UL,  0x01000101UL,  0x01000110UL,  0x01000111UL,
	 0x01001000UL,  0x01001001UL,  0x01001010UL,  0x01001011UL,  0x01001100UL,  0x01001101UL,  0x01001110UL,  0x01001111UL,
	 0x01010000UL,  0x01010001UL,  0x01010010UL,  0x01010011UL,  0x01010100UL,  0x01010101UL,  0x01010110UL,  0x01010111UL,
	 0x01011000UL,  0x01011001UL,  0x01011010UL,  0x01011011UL,  0x01011100UL,  0x01011101UL,  0x01011110UL,  0x01011111UL,
	 0x01100000UL,  0x01100001UL,  0x01100010UL,  0x01100011UL,  0x01100100UL,  0x01100101UL,  0x01100110UL,  0x01100111UL,
	 0x01101000UL,  0x01101001UL,  0x01101010UL,  0x01101011UL,  0x01101100UL,  0x01101101UL,  0x01101110UL,  0x01101111UL,
	 0x01110000UL,  0x01110001UL,  0x01110010UL,  0x01110011UL,  0x01110100UL,  0x01110101UL,  0x01110110UL,  0x01110111UL,
	 0x01111000UL,  0x01111001UL,  0x01111010UL,  0x01111011UL,  0x01111100UL,  0x01111101UL,  0x01111110UL,  0x01111111UL,
	 0x10000000UL,  0x10000001UL,  0x10000010UL,  0x10000011UL,  0x10000100UL,  0x10000101UL,  0x10000110UL,  0x10000111UL,
	 0x10001000UL,  0x10001001UL,  0x10001010UL,  0x10001011UL,  0x10001100UL,  0x10001101UL,  0x10001110UL,  0x10001111UL,
	 0x10010000UL,  0x10010001UL,  0x10010010UL,  0x10010011UL,  0x10010100UL,  0x10010101UL,  0x10010110UL,  0x10010111UL,
	 0x10011000UL,  0x10011001UL,  0x10011010UL,  0x10011011UL,  0x10011100UL,  0x10011101UL,  0x10011110UL,  0x10011111UL,
	 0x10100000UL,  0x10100001UL,  0x10100010UL,  0x10100011UL,  0x10100100UL,  0x10100101UL,  0x10100110UL,  0x10100111UL,
	 0x10101000UL,  0x10101001UL,  0x10101010UL,  0x10101011UL,  0x10101100UL,  0x10101101UL,  0x10101110UL,  0x10101111UL,
	 0x10110000UL,  0x10110001UL,  0x10110010UL,  0x10110011UL,  0x10110100UL,  0x10110101UL,  0x10110110UL,  0x10110111UL,
	 0x10111000UL,  0x10111001UL,  0x10111010UL,  0x10111011UL,  0x10111100UL,  0x10111101UL,  0x10111110UL,  0x10111111UL,
	 0x11000000UL,  0x11000001UL,  0x11000010UL,  0x11000011UL,  0x11000100UL,  0x11000101UL,  0x11000110UL,  0x11000111UL,
	 0x11001000UL,  0x11001001UL,  0x11001010UL,  0x11001011UL,  0x11001100UL,  0x11001101UL,  0x11001110UL,  0x11001111UL,
	 0x11010000UL,  0x11010001UL,  0x11010010UL,  0x11010011UL,  0x11010100UL,  0x11010101UL,  0x11010110UL,  0x11010111UL,
	 0x11011000UL,  0x11011001UL,  0x11011010UL,  0x11011011UL,  0x11011100UL,  0x11011101UL,  0x11011110UL,  0x11011111UL,
	 0x11100000UL,  0x11100001UL,  0x11100010UL,  0x11100011UL,  0x11100100UL,  0x11100101UL,  0x11100110UL,  0x11100111UL,
	 0x11101000UL,  0x11101001UL,  0x11101010UL,  0x11101011UL,  0x11101100UL,  0x11101101UL,  0x11101110UL,  0x11101111UL,
	 0x11110000UL,  0x11110001UL,  0x11110010UL,  0x11110011UL,  0x11110100UL,  0x11110101UL,  0x11110110UL,  0x11110111UL,
	 0x11111000UL,  0x11111001UL,  0x11111010UL,  0x11111011UL,  0x11111100UL,  0x11111101UL,  0x11111110UL,  0x11111111UL,
	};

	return tablepow4 [v & 0xff];
#endif
}

// Перенос каждого бита в байте в позицию с увеличенным в 2 раза номером.
portholder_t
power2(uint_fast16_t v)
{
	portholder_t r = 0;

	r |= (v & (1U << 0)) ? (1uL << 0) : 0;
	r |= (v & (1U << 1)) ? (1uL << 2) : 0;
	r |= (v & (1U << 2)) ? (1uL << 4) : 0;
	r |= (v & (1U << 3)) ? (1uL << 6) : 0;
	r |= (v & (1U << 4)) ? (1uL << 8) : 0;
	r |= (v & (1U << 5)) ? (1uL << 10) : 0;
	r |= (v & (1U << 6)) ? (1uL << 12) : 0;
	r |= (v & (1U << 7)) ? (1uL << 14) : 0;
	r |= (v & (1U << 8)) ? (1uL << 16) : 0;
	r |= (v & (1U << 9)) ? (1uL << 18) : 0;
	r |= (v & (1U << 10)) ? (1uL << 20) : 0;
	r |= (v & (1U << 11)) ? (1uL << 22) : 0;
	r |= (v & (1U << 12)) ? (1uL << 24) : 0;
	r |= (v & (1U << 13)) ? (1uL << 26) : 0;
	r |= (v & (1U << 14)) ? (1uL << 28) : 0;
	r |= (v & (1U << 15)) ? (1uL << 30) : 0;

	return r;
}

// Перенос каждого бита в байте в позицию с увеличенным в 8 раз номером.
portholder_t
power8(uint_fast8_t v)
{
	portholder_t r = 0;

	r |= (v & (1U << 0)) ? (1uL << 0) : 0;
	r |= (v & (1U << 1)) ? (1uL << 8) : 0;
	r |= (v & (1U << 2)) ? (1uL << 16) : 0;
	r |= (v & (1U << 3)) ? (1uL << 24) : 0;

	return r;
}

#endif /* CPUSTYLE_STM32F || CPUSTYLE_STM32MP1 || (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64) */

#if CPUSTYLE_R7S721

	// Port Register (Pn)
	//GPIO.P7 &= ~ (1U << 1);

	// Port Register (Pn)
	//GPIO.P7 |= (1U << 1);

#define r7s721_jpio_inputs(n, ipins1) do { \
		const portholder_t ipins2 = (ipins1); \
		GPIO.JPMC ## n &= ~ ipins2;	/* Port Mode Control Register: 0 - port, 1 - alternative */ \
		GPIO.JPIBC ## n |= ipins2;	/* Port Input Buffer Control Register (PIBCn): 0 - hiZ, 1 - input */ \
	} while (0)

#define r7s721_pio0_inputs(n, ipins1) do { \
		const portholder_t ipins2 = (ipins1); \
		GPIO.PMC ## n &= ~ ipins2;	/* Port Mode Control Register: 0 - port, 1 - alternative */ \
		GPIO.PIBC ## n |= ipins2;	/* Port Input Buffer Control Register (PIBCn): 0 - hiZ, 1 - input */ \
	} while (0)

#define r7s721_pio_inputs(n, ipins1) do { \
		const portholder_t ipins2 = (ipins1); \
		GPIO.PM ## n |= ipins2;	/* Port Mode Register (PMn): 0 - output, 1 - input */ \
		GPIO.PIPC ## n &= ~ ipins2;	/* Port IP Control Register: 0 - direction control from PMn, 1 - from alternative function */ \
		GPIO.PMC ## n &= ~ ipins2;	/* Port Mode Control Register: 0 - port, 1 - alternative */ \
		GPIO.PBDC ## n &= ~ ipins2;	/* Port Bidirection Control Register (PBDCn): 1: Bidirectional mode enabled */ \
		GPIO.PIBC ## n |= ipins2;	/* Port Input Buffer Control Register (PIBCn): 0 - hiZ, 1 - input */ \
	} while (0)

#define r7s721_pio_outputs(n, opins1, initialstate1) do { \
		const portholder_t opins2 = (opins1); \
		GPIO.PNOT ## n &= ~ opins2; /* Port NOT Register (PNOTn) */ \
		GPIO.PIPC ## n &= ~ opins2;	/* Port IP Control Register: 0 - direction control from PMn, 1 - from alternative function */ \
		GPIO.PSR ## n = (opins2 * 0x10000uL) | (initialstate1); \
		GPIO.PBDC ## n |= opins2;	/* Port Bidirection Control Register (PBDCn): 1: Bidirectional mode enabled */ \
		GPIO.PIBC ## n &= ~ opins2;	/* Port Input Buffer Control Register (PIBCn): 0 - hiZ, 1 - input */ \
		GPIO.PMC ## n &= ~ opins2;	/* Port Mode Control Register: 0 - port, 1 - alternative */ \
		GPIO.PM ## n &= ~ opins2;	/* Port Mode Register (PMn): 0 - output, 1 - input */ \
	} while (0)

#define R7S721_PIOX_ALTERNATIVE(n, iopins1, alt1) do { \
		const portholder_t iopins2 = (iopins1); \
		const int alt2 = (alt1); \
		const int pfcae = (alt2 & 0x04) != 0; \
		const int pfce = (alt2 & 0x02) != 0; \
		const int pfc = (alt2 & 0x01) != 0; \
		GPIO.PNOT ## n &= ~ iopins2; /* Port NOT Register (PNOTn) */ \
		GPIO.PM ## n |= iopins2;	/* Port Mode Register (PMn): 0 - output, 1 - input */ \
		GPIO.PMC ## n |= iopins2;	/* Port Mode Control Register: 0 - port, 1 - alternative */ \
		GPIO.PFCAE ## n = (GPIO.PFCAE ## n & ~ iopins2) | (iopins2 * pfcae); /* Port Function Control Additional Expansion Register (PFCAEn) */ \
		GPIO.PFCE ## n = (GPIO.PFCE ## n & ~ iopins2) | (iopins2 * pfce); /* Port Function Control Expansion Register (PFCEn) */ \
		GPIO.PFC ## n = (GPIO.PFC ## n & ~ iopins2) | (iopins2 * pfc); ; /* Port Function Control Register (PFCn) */ \
		GPIO.PBDC ## n |= iopins2;	/* Port Bidirection Control Register (PBDCn) 1: Bidirectional mode enabled */ \
		GPIO.PIPC ## n |= iopins2;	/* Port IP Control Register: 0 - direction control from PMn, 1 - from alternative function */ \
	} while (0)


// временная подготовка к работе с gpio.
// Вызывается из SystemInit() - после работы память будет затерта
void sysinit_gpio_initialize(void)
{
}

// R7S721 interrupts

static void r7s721_pio_onchangeinterrupt(
	int irqbase,	/* irq id для младшего бита указанного порта */
	int width, 
	unsigned long ipins, 
	int edge,	/* rising edge sensetive or level sensetive */
	uint32_t priority, 
	void (* vector)(void)
	)
{
	int bitpos;
	for (bitpos = 0; bitpos < width; ++ bitpos)
	{
		unsigned long mask = 1uL << bitpos;
		if ((ipins & mask) == 0)
			continue;
		const IRQn_ID_t int_id = irqbase + bitpos;
		arm_hardware_disable_handler(int_id);
		GIC_SetConfiguration(int_id, edge ? GIC_CONFIG_EDGE : GIC_CONFIG_LEVEL);// non-atomic operation
		arm_hardware_set_handler(int_id, vector, priority, 0x01uL << 0);	// CPU#0 is only one
	}
}

// JTAG inputs access
void arm_hardware_jpio0_inputs(unsigned long ipins)
{
	r7s721_jpio_inputs(0, ipins);
}

// inputs
void arm_hardware_pio0_inputs(unsigned long ipins)
{
	r7s721_pio0_inputs(0, ipins);
}

void arm_hardware_pio1_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(1, ipins);
}

void arm_hardware_pio2_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(2, ipins);
}

void arm_hardware_pio3_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(3, ipins);
}

void arm_hardware_pio4_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(4, ipins);
}

void arm_hardware_pio5_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(5, ipins);
}

void arm_hardware_pio6_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(6, ipins);
}

void arm_hardware_pio7_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(7, ipins);
}

void arm_hardware_pio8_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(8, ipins);
}

void arm_hardware_pio9_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(9, ipins);
}

#if CPUSTYLE_R7S721001	// RZ/A1H

void arm_hardware_pio10_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(10, ipins);
}

void arm_hardware_pio11_inputs(unsigned long ipins)
{
	r7s721_pio_inputs(11, ipins);
}
#endif /* CPUSTYLE_R7S721001 */

// outputs
void arm_hardware_pio1_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(1, opins, initialstate);
}

void arm_hardware_pio2_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(2, opins, initialstate);
}

void arm_hardware_pio3_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(3, opins, initialstate);
}

void arm_hardware_pio4_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(4, opins, initialstate);
}

void arm_hardware_pio5_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(5, opins, initialstate);
}

void arm_hardware_pio6_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(6, opins, initialstate);
}

void arm_hardware_pio7_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(7, opins, initialstate);
}

void arm_hardware_pio8_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(8, opins, initialstate);
}

void arm_hardware_pio9_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(9, opins, initialstate);
}

#if CPUSTYLE_R7S721001	// RZ/A1H

void arm_hardware_pio10_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(10, opins, initialstate);
}

void arm_hardware_pio11_outputs(unsigned long opins, unsigned long initialstate)
{
	r7s721_pio_outputs(11, opins, initialstate);
}
#endif /* CPUSTYLE_R7S721001 */

// alternative
void arm_hardware_pio1_alternative(unsigned long iopins, unsigned alt)
{
	R7S721_PIOX_ALTERNATIVE(1, iopins, alt);
}

void arm_hardware_pio2_alternative(unsigned long iopins, unsigned alt)
{
	R7S721_PIOX_ALTERNATIVE(2, iopins, alt);
}

void arm_hardware_pio3_alternative(unsigned long iopins, unsigned alt)
{
	R7S721_PIOX_ALTERNATIVE(3, iopins, alt);
}

void arm_hardware_pio4_alternative(unsigned long iopins, unsigned alt)
{
	R7S721_PIOX_ALTERNATIVE(4, iopins, alt);
}

void arm_hardware_pio5_alternative(unsigned long iopins, unsigned alt)
{
	R7S721_PIOX_ALTERNATIVE(5, iopins, alt);
}

void arm_hardware_pio6_alternative(unsigned long iopins, unsigned alt)
{
	R7S721_PIOX_ALTERNATIVE(6, iopins, alt);
}

void arm_hardware_pio7_alternative(unsigned long iopins, unsigned alt)
{
	R7S721_PIOX_ALTERNATIVE(7, iopins, alt);
}

void arm_hardware_pio8_alternative(unsigned long iopins, unsigned alt)
{
	R7S721_PIOX_ALTERNATIVE(8, iopins, alt);
}

void arm_hardware_pio9_alternative(unsigned long iopins, unsigned alt)
{
	R7S721_PIOX_ALTERNATIVE(9, iopins, alt);
}

#if CPUSTYLE_R7S721001	// RZ/A1H

void arm_hardware_pio10_alternative(unsigned long iopins, unsigned alt)
{
	R7S721_PIOX_ALTERNATIVE(10, iopins, alt);
}

void arm_hardware_pio11_alternative(unsigned long iopins, unsigned alt)
{
	R7S721_PIOX_ALTERNATIVE(11, iopins, alt);
}
#endif /* CPUSTYLE_R7S721001 */

// pin change interrupts
void arm_hardware_piojp0_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT0_IRQn, 2, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT0_IRQn, 2, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio0_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT2_IRQn, 4, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT2_IRQn, 6, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio1_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT6_IRQn, 16, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT8_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio2_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT22_IRQn, 10, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT24_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio3_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT32_IRQn, 16, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT40_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio4_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT48_IRQn, 8, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT56_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio5_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT56_IRQn, 16, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT72_IRQn, 11, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio6_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT72_IRQn, 16, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT83_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio7_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT88_IRQn, 12, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT99_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio8_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT100_IRQn, 16, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT115_IRQn, 16, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

void arm_hardware_pio9_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
#if CPUSTYLE_R7S721020		// RZ/A1L
	r7s721_pio_onchangeinterrupt(TINT116_IRQn, 6, ipins, edge, priority, vector);
#elif CPUSTYLE_R7S721001	// RZ/A1H
	r7s721_pio_onchangeinterrupt(TINT131_IRQn, 8, ipins, edge, priority, vector);
#else 
	#error Wrong CPUSTYLE_R7S721xxx
#endif
}

#if CPUSTYLE_R7S721001	// RZ/A1H

void arm_hardware_pio10_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
	r7s721_pio_onchangeinterrupt(TINT139_IRQn, 16, ipins, edge, priority, vector);
}

void arm_hardware_pio11_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void))
{
	r7s721_pio_onchangeinterrupt(TINT155_IRQn, 16, ipins, edge, priority, vector);
}

#endif /* CPUSTYLE_R7S721001 */

static void (* r7s721_IRQn_user [8])(void);

static void r7s721_IRQn_IRQHandler(void)
{
	const uint_fast8_t irqrr = INTC.IRQRR;
	if ((irqrr & (1U << 0)) != 0)
	{
		enum { irq = 0 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		ASSERT(r7s721_IRQn_user [irq] != NULL);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 1)) != 0)
	{
		enum { irq = 1 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		ASSERT(r7s721_IRQn_user [irq] != NULL);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 2)) != 0)
	{
		enum { irq = 2 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		ASSERT(r7s721_IRQn_user [irq] != NULL);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 3)) != 0)
	{
		enum { irq = 3 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		ASSERT(r7s721_IRQn_user [irq] != NULL);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 4)) != 0)
	{
		enum { irq = 4 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		ASSERT(r7s721_IRQn_user [irq] != NULL);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 5)) != 0)
	{
		enum { irq = 5 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		ASSERT(r7s721_IRQn_user [irq] != NULL);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 6)) != 0)
	{
		enum { irq = 6 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		ASSERT(r7s721_IRQn_user [irq] != NULL);
		(* r7s721_IRQn_user [irq])();
	}
	else if ((irqrr & (1U << 7)) != 0)
	{
		enum { irq = 7 };
		INTC.IRQRR = (uint16_t) ~ (1u << irq);
		ASSERT(r7s721_IRQn_user [irq] != NULL);
		(* r7s721_IRQn_user [irq])();
	}
	else
	{
		ASSERT(0);
	}
}

// Set handlers and mode for IRQ0..IRQ7
/*
	edge values
	00: Interrupt request is detected on low level of IRQn input
	01: Interrupt request is detected on falling edge of IRQn input
	10: Interrupt request is detected on rising edge of IRQn input
	11: Interrupt request is detected on both edges of IRQn input
*/

void arm_hardware_irqn_interrupt(unsigned long irq, int edge, uint32_t priority, void (* vector)(void))
{
	r7s721_IRQn_user [irq] = vector;

	INTC.ICR1 = (INTC.ICR1 & ~ (0x03uL << (irq * 2))) |
		(edge << (irq * 2)) |
		0;
	{
		const IRQn_ID_t int_id = IRQ0_IRQn + irq;
		arm_hardware_disable_handler(int_id);
		GIC_SetConfiguration(int_id, GIC_CONFIG_LEVEL);// non-atomic operation
		arm_hardware_set_handler(int_id, r7s721_IRQn_IRQHandler, priority, 0x01uL << 0);	// CPU#0 is only one
	}
}

#elif CPUSTYLE_XCZU && ! LINUX_SUBSYSTEM

void xc7z_gpio_input(uint8_t pin)
{
}

void xc7z_gpio_output(uint8_t pin)
{
}

void sysinit_gpio_initialize(void)
{
}

#elif CPUSTYLE_XC7Z && ! LINUX_SUBSYSTEM

static SPINLOCK_t gpiodata_locks [8] =
{
		SPINLOCK_INIT,
		SPINLOCK_INIT,
		SPINLOCK_INIT,
		SPINLOCK_INIT,
		SPINLOCK_INIT,
		SPINLOCK_INIT,
		SPINLOCK_INIT,
		SPINLOCK_INIT,
};

// временная подготовка к работе с gpio.
// Вызывается из SystemInit() - после работы память будет затерта
void sysinit_gpio_initialize(void)
{
	unsigned i;

	for (i = 0; i < ARRAY_SIZE(gpiodata_locks); ++ i)
	{
		SPINLOCK_t * const lck = & gpiodata_locks [i];
		SPINLOCK_INITIALIZE(lck);
	}
}

void gpiobank_lock(unsigned bank, IRQL_t * oldIrql)
{
	SPINLOCK_t * const lck = & gpiodata_locks [bank];
	RiseIrql(IRQL_ONLY_REALTIME, oldIrql);
	SPIN_LOCK(lck);
}

void gpiobank_unlock(unsigned bank, IRQL_t oldIrql)
{
	SPINLOCK_t * const lck = & gpiodata_locks [bank];
	SPIN_UNLOCK(lck);
	LowerIrql(oldIrql);
}


/* адреса функций - обработчиков на каждый GPIO/EMIO */
static void (* gpio_vectors [ZYNQ_MIO_CNT + 2 * 32])(void);

void GPIO_IRQHandler(void)
{
	unsigned pin;
	for (pin = 0; pin < ARRAY_SIZE(gpio_vectors); ++ pin)
	{
		const portholder_t bank = GPIO_PIN2BANK(pin);
		const portholder_t mask = GPIO_PIN2MASK(pin);
		const uintptr_t int_mask = GPIO_INT_MASK(bank);	// enable/disable result
		const uintptr_t int_stat = GPIO_INT_STAT(bank);
		const unsigned state = ZYNQ_IORW32(int_stat) & mask & ~ ZYNQ_IORW32(int_mask);

		if (state != 0)
		{
			ZYNQ_IORW32(int_stat) = mask;
			ASSERT(gpio_vectors [pin] != NULL);
			(* gpio_vectors [pin])();
		}
	}
}

void gpio_onchangeinterrupt(unsigned pin, void (* handler)(void), uint32_t priority, uint32_t tgcpu)
{
	const portholder_t bank = GPIO_PIN2BANK(pin);
	const portholder_t mask = GPIO_PIN2MASK(pin);

	const uintptr_t int_en = GPIO_INT_EN(bank);
	//const uintptr_t int_dis = GPIO_INT_DIS(bank);
	//const uintptr_t int_type = GPIO_INT_TYPE(bank);	// 0: level-sensitive, 1: edge-sensitive
	//const uintptr_t int_polatity = GPIO_INT_POLARITY(bank);
	const uintptr_t int_any = GPIO_INT_ANY(bank);

	ASSERT(pin < ARRAY_SIZE(gpio_vectors));

	ZYNQ_IORW32(int_any) |= mask;
	ZYNQ_IORW32(int_en) = mask;

	gpio_vectors [pin] = handler;
	arm_hardware_set_handler(GPIO_IRQn, GPIO_IRQHandler, priority, tgcpu);
}

void gpio_onrisinginterrupt(unsigned pin, void (* handler)(void), uint32_t priority, uint32_t tgcpu)
{
	const portholder_t bank = GPIO_PIN2BANK(pin);
	const portholder_t mask = GPIO_PIN2MASK(pin);

	const uintptr_t int_en = GPIO_INT_EN(bank);
	//const uintptr_t int_dis = GPIO_INT_DIS(bank);
	const uintptr_t int_type = GPIO_INT_TYPE(bank);	// 0: level-sensitive, 1: edge-sensitive
	const uintptr_t int_polatity = GPIO_INT_POLARITY(bank);
	const uintptr_t int_any = GPIO_INT_ANY(bank);

	ASSERT(pin < ARRAY_SIZE(gpio_vectors));

	ZYNQ_IORW32(int_type) |= mask;
	ZYNQ_IORW32(int_any) &= ~ mask;
	ZYNQ_IORW32(int_polatity) |= mask;
	ZYNQ_IORW32(int_en) = mask;

	gpio_vectors [pin] = handler;
	arm_hardware_set_handler(GPIO_IRQn, GPIO_IRQHandler, priority, tgcpu);
}

void gpio_onfallinterrupt(unsigned pin, void (* handler)(void), uint32_t priority, uint32_t tgcpu)
{
	const portholder_t bank = GPIO_PIN2BANK(pin);
	const portholder_t mask = GPIO_PIN2MASK(pin);

	const uintptr_t int_en = GPIO_INT_EN(bank);
	//const uintptr_t int_dis = GPIO_INT_DIS(bank);
	const uintptr_t int_type = GPIO_INT_TYPE(bank);	// 0: level-sensitive, 1: edge-sensitive
	const uintptr_t int_polatity = GPIO_INT_POLARITY(bank);
	const uintptr_t int_any = GPIO_INT_ANY(bank);

	ASSERT(pin < ARRAY_SIZE(gpio_vectors));

	ZYNQ_IORW32(int_type) |= mask;
	ZYNQ_IORW32(int_any) &= ~ mask;
	ZYNQ_IORW32(int_polatity) &= ~ mask;
	ZYNQ_IORW32(int_en) = mask;

	gpio_vectors [pin] = handler;
	arm_hardware_set_handler(GPIO_IRQn, GPIO_IRQHandler, priority, tgcpu);
}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64) || CPUSTYLE_A64

// DRV: 0x00 = level0, 0x01 = level1, 0x02 - level2, 0x03 - level3
// PULL: 0x00 = disable, 0x01 = pull-up, 0x02 - pull-down
#define ALWNR_GPIO_DRV_OUTPUT 0x03
#define ALWNR_GPIO_PULL_OUTPUT 0x00

#define ALWNR_GPIO_DRV_OUTPUT2M 0x01
#define ALWNR_GPIO_PULL_OUTPUT2M 0x00

#define ALWNR_GPIO_DRV_OUTPUT10M 0x02
#define ALWNR_GPIO_PULL_OUTPUT10M 0x00

#define ALWNR_GPIO_DRV_OUTPUT20M 0x02
#define ALWNR_GPIO_PULL_OUTPUT20M 0x00

#define ALWNR_GPIO_DRV_OUTPUT50M 0x03
#define ALWNR_GPIO_PULL_OUTPUT50M 0x00

#define ALWNR_GPIO_CFG_OPENDRAIN GPIO_CFG_OUT
#define ALWNR_GPIO_DRV_OPENDRAIN 0x03
#define ALWNR_GPIO_PULL_OPENDRAIN 0x00

#define ALWNR_GPIO_DRV_INPUT 0x02
#define ALWNR_GPIO_PULL_INPUT 0x01	// pull-up

#define ALWNR_GPIO_DRV_AF2M 0x02
#define ALWNR_GPIO_PULL_AF2M 0x01	// pull-up

#define ALWNR_GPIO_DRV_OPENDRAINAF2M 0x03
#define ALWNR_GPIO_PULL_OPENDRAINAF2M 0x00

#define ALWNR_GPIO_DRV_AF20M 0x02
#define ALWNR_GPIO_PULL_AF20M 0x00

#define ALWNR_GPIO_DRV_AF50M 0x03
#define ALWNR_GPIO_PULL_AF50M 0x00

static SPINLOCK_t gpiodata_locks [8] =
{
	SPINLOCK_INIT,	// GPIOA
	SPINLOCK_INIT,	// GPIOB - in T113-S3
	SPINLOCK_INIT,	// GPIOC - in T113-S3
	SPINLOCK_INIT,	// GPIOD - in T113-S3
	SPINLOCK_INIT,	// GPIOE - in T113-S3
	SPINLOCK_INIT,	// GPIOF - in T113-S3
	SPINLOCK_INIT,	// GPIOG - in T113-S3
	SPINLOCK_INIT,	// GPIOH
};

static SPINLOCK_t gpiodata_L_lock = SPINLOCK_INIT;

static SPINLOCK_t * gpioX_get_lock(GPIO_TypeDef * gpio)
{
#if CPUSTYLE_A64
	if (gpio == GPIOL)
		return & gpiodata_L_lock;
#endif /* CPUSTYLE_A64 */

	return & gpiodata_locks [gpio - (GPIO_TypeDef *) GPIOB_BASE + 1];
}


static uint32_t readl(uintptr_t addr)
{
	return * (volatile uint32_t *) addr;
}

static void writel(uint32_t value, uintptr_t addr)
{
	* (volatile uint32_t *) addr = value;
}

//#define	R_PRCM_BASE	 ((uintptr_t) 0x01F01400)

// временная подготовка к работе с gpio.
// Вызывается из SystemInit() - после работы память будет затерта
void sysinit_gpio_initialize(void)
{
	unsigned i;

	for (i = 0; i < ARRAY_SIZE(gpiodata_locks); ++ i)
	{
		SPINLOCK_t * const lck = & gpiodata_locks [i];
		SPINLOCK_INITIALIZE(lck);
	}

#if CPUSTYLE_A64

	SPINLOCK_INITIALIZE(& gpiodata_L_lock);
	CCU->BUS_CLK_GATING_REG2 |= (1u << 5);	// PIO_GATING - not need - already set
	RTC->GPL_HOLD_OUTPUT_REG = 0;

	uint32_t reg_val;
	// R_GPIO reset deassert
	reg_val = readl(R_PRCM_BASE+0xb0);
	reg_val |= (1u << 0);
	writel(reg_val, R_PRCM_BASE+0xb0);

	// R_GPIO GATING open
	// Valid bit 7..0
	// bit 0 = GPIO(L) gating
	// bit 6 = TWI gating
	reg_val = readl(R_PRCM_BASE+0x28);
	reg_val |= (1u << 0);
	writel(reg_val, R_PRCM_BASE+0x28);

#endif /* CPUSTYLE_A64 */
}

#if __riscv_xlen
typedef uint_xlen_t irqstatus_t;
#else
typedef uint32_t irqstatus_t;
#endif


static void gpioX_lock(GPIO_TypeDef * gpio, IRQL_t * oldIrql)
{
	SPINLOCK_t * const lck = gpioX_get_lock(gpio);
	RiseIrql(IRQL_ONLY_REALTIME, oldIrql);
	SPIN_LOCK(lck);
}

static void gpioX_unlock(GPIO_TypeDef * gpio, IRQL_t irql)
{
	SPINLOCK_t * const lck = gpioX_get_lock(gpio);
	SPIN_UNLOCK(lck);
	LowerIrql(irql);
}

/* Отсутствие атомарных операций модификации состояния выводов требует исключительного доступа */
/*!< Atomic port state change */
void gpioX_setstate(
	GPIO_TypeDef * gpio,
	portholder_t mask,
	portholder_t state
	)
{
	IRQL_t oldIrql;
	gpioX_lock(gpio, & oldIrql);

	gpio->DATA = (gpio->DATA & ~ mask) | (state & mask);
	(void) gpio->DATA;

	gpioX_unlock(gpio, oldIrql);
}

static void gpioX_prog(
	GPIO_TypeDef * gpio,
	portholder_t iopins,
	unsigned cfg,
	unsigned drv,
	unsigned pull
	)
{
#if CPUSTYLE_A64
	const portholder_t cfg0 = power4(iopins >> 0);	/* CFG0 bits */
	const portholder_t cfg1 = power4(iopins >> 8);	/* CFG1 bits */
	const portholder_t cfg2 = power4(iopins >> 16);	/* CFG2 bits */
	const portholder_t cfg3 = power4(iopins >> 24);	/* CFG3 bits */

	const portholder_t pull0 = power2(iopins >> 0);		/* PULL0 and DRV0 bits */
	const portholder_t pull1 = power2(iopins >> 16);	/* PULL1 and DRV1 bits */

	IRQL_t oldIrql;
	gpioX_lock(gpio, & oldIrql);

	gpio->CFG [0] = (gpio->CFG [0] & ~ (cfg0 * 0x0F)) | (cfg * cfg0);
	gpio->CFG [1] = (gpio->CFG [1] & ~ (cfg1 * 0x0F)) | (cfg * cfg1);
	gpio->CFG [2] = (gpio->CFG [2] & ~ (cfg2 * 0x0F)) | (cfg * cfg2);
	gpio->CFG [3] = (gpio->CFG [3] & ~ (cfg3 * 0x0F)) | (cfg * cfg3);

	gpio->DRV [0] = (gpio->DRV [0] & ~ (pull0 * 0x03)) | (drv * pull0);
	gpio->DRV [1] = (gpio->DRV [1] & ~ (pull1 * 0x03)) | (drv * pull1);

	// PULL: 0x00 = disable, 0x01 = pull-up, 0x02 - pull-down
	gpio->PULL [0] = (gpio->PULL [0] & ~ (pull0 * 0x03)) | (pull * pull0);
	gpio->PULL [1] = (gpio->PULL [1] & ~ (pull1 * 0x03)) | (pull * pull1);

	gpioX_unlock(gpio, oldIrql);

#else
	const portholder_t mask0 = power4(iopins >> 0);		/* CFG0 and DRV0 bits */
	const portholder_t mask1 = power4(iopins >> 8);		/* CFG1 and DRV1 bits */
	const portholder_t mask2 = power4(iopins >> 16);	/* CFG2 and DRV2 bits */
	const portholder_t mask3 = power4(iopins >> 24);	/* CFG3 and DRV3 bits */

	const portholder_t pull0 = power2(iopins >> 0);		/* PULL0 bits */
	const portholder_t pull1 = power2(iopins >> 16);	/* PULL1 bits */

	IRQL_t oldIrql;
	gpioX_lock(gpio, & oldIrql);

	gpio->CFG [0] = (gpio->CFG [0] & ~ (mask0 * 0x0F)) | (cfg * mask0);
	gpio->CFG [1] = (gpio->CFG [1] & ~ (mask1 * 0x0F)) | (cfg * mask1);
	gpio->CFG [2] = (gpio->CFG [2] & ~ (mask2 * 0x0F)) | (cfg * mask2);
	gpio->CFG [3] = (gpio->CFG [3] & ~ (mask3 * 0x0F)) | (cfg * mask3);

	gpio->DRV [0] = (gpio->DRV [0] & ~ (mask0 * 0x0F)) | (drv * mask0);
	gpio->DRV [1] = (gpio->DRV [1] & ~ (mask1 * 0x0F)) | (drv * mask1);
	gpio->DRV [2] = (gpio->DRV [2] & ~ (mask2 * 0x0F)) | (drv * mask2);
	gpio->DRV [3] = (gpio->DRV [3] & ~ (mask3 * 0x0F)) | (drv * mask3);

	// PULL: 0x00 = disable, 0x01 = pull-up, 0x02 - pull-down
	gpio->PULL [0] = (gpio->PULL [0] & ~ (pull0 * 0x03)) | (pull * pull0);
	gpio->PULL [1] = (gpio->PULL [1] & ~ (pull1 * 0x03)) | (pull * pull1);
	gpioX_unlock(gpio, oldIrql);
#endif
}

static void gpioX_updown(
	GPIO_TypeDef * gpio,
	portholder_t ioup,
	portholder_t iodown
	)
{
	const portholder_t iopins = ioup | iodown;
	const portholder_t pull0 = power2(iopins >> 0) * 0x03;		/* PULL0 bits */
	const portholder_t pull0up = power2(ioup >> 0);
	const portholder_t pull0down = power2(iodown >> 0);
	const portholder_t pull1 = power2(iopins >> 16) * 0x03;		/* PULL1 bits */
	const portholder_t pull1up = power2(ioup >> 16);
	const portholder_t pull1down = power2(iodown >> 16);

	IRQL_t oldIrql;
	gpioX_lock(gpio, & oldIrql);

	// PULL: 0x00 = disable, 0x01 = pull-up, 0x02 - pull-down
	gpio->PULL [0] = (gpio->PULL [0] & ~ pull0) | (0x01 * pull0up) | (0x02 * pull0down);
	gpio->PULL [1] = (gpio->PULL [1] & ~ pull1) | (0x01 * pull1up) | (0x02 * pull1down);

	gpioX_unlock(gpio, oldIrql);
}

static void gpioX_updownoff(
	GPIO_TypeDef * gpio,
	portholder_t iopins
	)
{
	// PULL: 0x00 = disable, 0x01 = pull-up, 0x02 - pull-down
	const portholder_t pull0 = power2(iopins >> 0) * 0x03;		/* PULL0 bits */
	const portholder_t pull1 = power2(iopins >> 16) * 0x03;		/* PULL1 bits */

	IRQL_t oldIrql;
	gpioX_lock(gpio, & oldIrql);

	gpio->PULL [0] = (gpio->PULL [0] & ~ pull0);
	gpio->PULL [1] = (gpio->PULL [1] & ~ pull1);

	gpioX_unlock(gpio, oldIrql);
}

//static void (* gpiohandlers [8][32])(void);

static void ALLW_GPIO_IRQ_Handler_GPIOA(void)
{
	ASSERT(0);
}

static void ALLW_GPIO_IRQ_Handler_GPIOB(void)
{
	const unsigned status = GPIOINTB->EINT_STATUS;
	GPIOINTB->EINT_STATUS = status;

}

static void ALLW_GPIO_IRQ_Handler_GPIOC(void)
{
	const unsigned status = GPIOINTC->EINT_STATUS;
	GPIOINTC->EINT_STATUS = status;

}

static void ALLW_GPIO_IRQ_Handler_GPIOD(void)
{
	const unsigned status = GPIOINTD->EINT_STATUS;
	GPIOINTD->EINT_STATUS = status;

}

static void ALLW_GPIO_IRQ_Handler_GPIOE(void)
{
	const unsigned status = GPIOINTE->EINT_STATUS;
	GPIOINTE->EINT_STATUS = status;

#if ENCODER_BITS
	if ((status & ENCODER_BITS) != 0)
		spool_encinterrupt();
#endif /* ENCODER_BITS */
#if BOARD_GT911_INT_PIN
//	if ((status & ENCODER_BITS) != 0)
//		xspool_encinterrupt();
#endif /* BOARD_GT911_INT_PIN */
#if BOARD_STMPE811_INT_PIN
//	if ((status & ENCODER_BITS) != 0)
//		xspool_encinterrupt();
#endif /* BOARD_STMPE811_INT_PIN */
}

static void ALLW_GPIO_IRQ_Handler_GPIOF(void)
{
	const unsigned status = GPIOINTF->EINT_STATUS;
	GPIOINTF->EINT_STATUS = status;

}

static void ALLW_GPIO_IRQ_Handler_GPIOG(void)
{
	const unsigned status = GPIOINTG->EINT_STATUS;
	GPIOINTG->EINT_STATUS = status;

}

static void ALLW_GPIO_IRQ_Handler_GPIOH(void)
{
	ASSERT(0);
}

void ALLW_GPIO_IRQ_Handler(void)	// Allwinner specific
{

}

/* разрешение прерывания по изменению состояния указанных групп выводов */
void
gpioX_onchangeinterrupt(
		GPIO_TypeDef * gpio,
		portholder_t ipins,
		portholder_t raise, portholder_t fall,
		uint32_t priority,
		uint_fast8_t targetcpu,
		void (* handler_unused)(void)
		)
{
#if CPUSTYLE_A64
	const unsigned gpioix = gpio == GPIOL ? 0 : (gpio - (GPIO_TypeDef *) GPIOB_BASE + 1);
	//GPIOINT_TypeDef * const ints = & GPIOBLOCK->GPIO_INTS [gpioix];
	GPIOBLOCK_TypeDef * const blk = gpio == GPIOL ? GPIOBLOCK_L : GPIOBLOCK;
#else /* CPUSTYLE_A64 */
	const unsigned gpioix = gpio - (GPIO_TypeDef *) GPIOB_BASE + 1;
	GPIOBLOCK_TypeDef * const blk = GPIOBLOCK;
#endif /* CPUSTYLE_A64 */
	unsigned pos;
	//	0x0: Positive Edge
	//	0x1: Negative Edge
	//	0x2: High Level
	//	0x3: Low Level
	//	0x4: Double Edge (Positive/Negative)

	static void (* const handlers [])(void) =
	{
		ALLW_GPIO_IRQ_Handler_GPIOA,
		ALLW_GPIO_IRQ_Handler_GPIOB,
		ALLW_GPIO_IRQ_Handler_GPIOC,
		ALLW_GPIO_IRQ_Handler_GPIOD,
		ALLW_GPIO_IRQ_Handler_GPIOE,
		ALLW_GPIO_IRQ_Handler_GPIOF,
		ALLW_GPIO_IRQ_Handler_GPIOG,
		ALLW_GPIO_IRQ_Handler_GPIOH,
	};
	unsigned cfgbits = 0;	// default - high level

	if (! raise && ! fall)
		cfgbits = 0x00;		// default - high level
	else if (raise && ! fall)
		cfgbits = 0x00;		// 0x0: Positive Edge
	else if (! raise && fall)
		cfgbits = 0x01;		// 0x1: Negative Edge
	else if (raise && fall)
		cfgbits = 0x04;		// 0x4: Double Edge (Positive/Negative)

	const portholder_t cfg0 = power4(ipins >> 0);		/* EINT_CFG0 bits */
	const portholder_t cfg1 = power4(ipins >> 8);		/* EINT_CFG1 bits */
	const portholder_t cfg2 = power4(ipins >> 16);		/* EINT_CFG2 bits */
	const portholder_t cfg3 = power4(ipins >> 24);		/* EINT_CFG3 bits */

	IRQL_t oldIrql;
	gpioX_lock(gpio, & oldIrql);

	blk->GPIO_INTS [gpioix].EINT_CFG [0] = (blk->GPIO_INTS [gpioix].EINT_CFG [0] & ~ (cfg0 * 0x0F)) | (cfgbits * cfg0);
	blk->GPIO_INTS [gpioix].EINT_CFG [1] = (blk->GPIO_INTS [gpioix].EINT_CFG [1] & ~ (cfg1 * 0x0F)) | (cfgbits * cfg1);
	blk->GPIO_INTS [gpioix].EINT_CFG [2] = (blk->GPIO_INTS [gpioix].EINT_CFG [2] & ~ (cfg2 * 0x0F)) | (cfgbits * cfg2);
	blk->GPIO_INTS [gpioix].EINT_CFG [3] = (blk->GPIO_INTS [gpioix].EINT_CFG [3] & ~ (cfg3 * 0x0F)) | (cfgbits * cfg3);

	for (pos = 0; pos < 32; ++ pos)
	{
		const portholder_t mask = (portholder_t) 0x01L << pos;
		if ((ipins & mask) == 0)
			continue;
		//gpiohandlers [gpioix] [pos] = handler;
		arm_hardware_set_handler(GPIOB_NS_IRQn + gpioix * 2 - 2, handlers [gpioix], priority, targetcpu);	/* GPIOx_NS */
	}

	blk->GPIO_INTS [gpioix].EINT_CTL |= ipins;
	// todo: EINT_STATUS = ipins; // for clear
	gpioX_unlock(gpio, oldIrql);
}

#elif CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

// временная подготовка к работе с gpio.
// Вызывается из SystemInit() - после работы память будет затерта
void sysinit_gpio_initialize(void)
{
}

#elif CPUSTYLE_VM14

// 1892ВМ14Я ELVEES multicore.ru

// временная подготовка к работе с gpio.
// Вызывается из SystemInit() - после работы память будет затерта
void sysinit_gpio_initialize(void)
{
}

#endif

#if CPUSTYLE_ARM || CPUSTYLE_RISCV

	/* Установка начального состояния битов  в GPIO STM32F4X */
	#define arm_stm32f4xx_hardware_pio_setstate(gpio, opins, initialstate) \
	  do { \
		const portholder_t op = (opins); \
		const portholder_t is = (initialstate); \
		(gpio)->BSRR = \
			BSRR_S((is) & (op)) | /* set bits */ \
			BSRR_C(~ (is) & (op)) | /* reset bits */ \
			0; \
		} while (0)

	#if CPUSTYLE_STM32F1XX


	/* установка битов в регистре AFIO_MAPR. Поддерживается режим переключения JTAG */
	void cpu_stm32f1xx_setmapr(
		unsigned long bits
		)
	{
		RCC->APB2ENR |=  RCC_APB2ENR_AFIOEN;     //включить тактирование альтернативных функций
		(void) RCC->APB2ENR;
		AFIO->MAPR = (AFIO->MAPR & ~ AFIO_MAPR_SWJ_CFG) | AFIO_MAPR_SWJ_CFG_JTAGDISABLE | bits;
	}

	#define arm_stm32f10x_hardware_pio_prog(gpio, iomask, cnf, mode) \
	  do { \
		const portholder_t lo = power4((iomask) >> 0);	\
		const portholder_t hi = power4((iomask) >> 8);	\
		(gpio)->CRL = ((gpio)->CRL & ~ ((GPIO_CRL_MODE0 | GPIO_CRL_CNF0) * lo)) | (((GPIO_CRL_MODE0_0 * (mode)) | (GPIO_CRL_CNF0_0 * (cnf))) * lo);	\
		(gpio)->CRH = ((gpio)->CRH & ~ ((GPIO_CRH_MODE8 | GPIO_CRH_CNF8) * hi)) | (((GPIO_CRH_MODE8_0 * (mode)) | (GPIO_CRH_CNF8_0 * (cnf))) * hi);	\
	  } while (0)

	// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
	#define stm32f10x_pioX_pupdr(gpio, up, down) \
	  do { \
		(gpio)->BSRR = \
			BSRR_S(up) |	/* Для включения pull-up при CNFy[1:0] = 10 */	\
			BSRR_C(down);	/* Для включения pull-down при CNFy[1:0] = 10 */	\
	  } while (0)
		// отключение встроенной подтяжки на входе (так как программирование на ввод всегда включает подтяжку
		// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
	#define arm_stm32f10x_hardware_pio_pupoff(gpio, ipins) \
	  do { \
		/* const portholder_t ipins3 = power2(ipins); */	\
		/* (gpio)->PUPDR = ((gpio)->PUPDR & ~ (ipins3 * GPIO_PUPDR_PUPDR0)) | ipins3 * (0) * GPIO_PUPDR_PUPDR0_0; */ \
	  } while (0)

	/* разрешение прерывания по изменению состояния указанных групп выводов */
	static void 
	stm32f10x_pioX_onchangeinterrupt(portholder_t ipins,
			portholder_t raise, portholder_t fall,
			portholder_t portcode, uint32_t priority)
	{
		//const portholder_t portcode = AFIO_EXTICR1_EXTI0_PB;	// PORT B
		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     //включить тактирование альтернативных функций
		(void) RCC->APB2ENR;

	#if 1
		{
			const portholder_t bitpos0 = power4((ipins >> 0) & 0x0f);
			AFIO->EXTICR [0] = (AFIO->EXTICR [0] & ~ (AFIO_EXTICR1_EXTI0 * bitpos0)) | (portcode * bitpos0);
		}
		{
			const portholder_t bitpos1 = power4((ipins >> 4) & 0x0f);
			AFIO->EXTICR [1] = (AFIO->EXTICR [1] & ~ (AFIO_EXTICR2_EXTI4 * bitpos1)) | (portcode * bitpos1);
		}
		{
			const portholder_t bitpos2 = power4((ipins >> 8) & 0x0f);
			AFIO->EXTICR [2] = (AFIO->EXTICR [2] & ~ (AFIO_EXTICR3_EXTI8 * bitpos2)) | (portcode * bitpos2);
		}
		{
			const portholder_t bitpos3 = power4((ipins >> 12) & 0x0f);
			AFIO->EXTICR [3] = (AFIO->EXTICR [3] & ~ (AFIO_EXTICR4_EXTI12 * bitpos3)) | (portcode * bitpos3);
		}
	#else
		uint_fast8_t i;
		for (i = 0; i < 16; ++ i)
		{
			const portholder_t pinmask = (portholder_t) 1uL << i;
			if ((ipins & pinmask) == 0)
				continue;	// Эти биты не трогаем

			const div_t d = div(i, 4);
			const portholder_t bitpos = (portholder_t) 1 << (d.rem * 4);
			const portholder_t bitmask = AFIO_EXTICR1_EXTI0 * bitpos;
			const portholder_t bitvalue = portcode * bitpos;
			AFIO->EXTICR [d.quot] = (AFIO->EXTICR [d.quot] & ~ bitmask) | bitvalue;
		}
	#endif

		EXTI->RTSR = (EXTI->RTSR & ~ ipins) | (ipins & raise);		// прерывание по нарастанию
		EXTI->FTSR = (EXTI->FTSR & ~ ipins) | (ipins & fall);		// прерывание по спаду
		EXTI->IMR |= ipins;		// разрешить прерывание

		enum { targetcpu = 0 };
		if ((ipins & EXTI_IMR_MR0) != 0)
			arm_hardware_set_handler(EXTI0_IRQn, EXTI0_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR_MR1) != 0)
			arm_hardware_set_handler(EXTI1_IRQn, EXTI1_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR_MR2) != 0)
			arm_hardware_set_handler(EXTI2_IRQn, EXTI2_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR_MR3) != 0)
			arm_hardware_set_handler(EXTI3_IRQn, EXTI3_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR_MR4) != 0)
			arm_hardware_set_handler(EXTI4_IRQn, EXTI4_IRQHandler, priority, targetcpu);
		if ((ipins & (EXTI_IMR_MR9 | EXTI_IMR_MR8 | EXTI_IMR_MR7 | EXTI_IMR_MR6 | EXTI_IMR_MR5)) != 0)
			arm_hardware_set_handler(EXTI9_5_IRQn, EXTI9_5_IRQHandler, priority, targetcpu);
		if ((ipins & (EXTI_IMR_MR15 | EXTI_IMR_MR14 | EXTI_IMR_MR14 | EXTI_IMR_MR13 | EXTI_IMR_MR12 | EXTI_IMR_MR11 | EXTI_IMR_MR10)) != 0)
			arm_hardware_set_handler(EXTI15_10_IRQn, EXTI15_10_IRQHandler, priority, targetcpu);

	}
	/* программирвоание битов в регистрах управления GPIO, указанных в iomask, в конфигурацию CNF И режим MODE */
	#elif \
		CPUSTYLE_STM32MP1 || \
		0

		#define stm32mp1_pioX_prog(gpio, iomask0, moder, speed, pupdr, typer) \
		  do { \
			const portholder_t iomask = (iomask0);	\
			const portholder_t mask3 = power2(iomask);	\
			(gpio)->MODER = ((gpio)->MODER & ~ (mask3 * GPIO_MODER_MODER0)) | mask3 * (moder) * GPIO_MODER_MODER0_0; \
			(gpio)->OSPEEDR = ((gpio)->OSPEEDR & ~ (mask3 * GPIO_OSPEEDR_OSPEEDR0)) | mask3 * (speed) * GPIO_OSPEEDR_OSPEEDR0_0; \
			(gpio)->PUPDR = ((gpio)->PUPDR & ~ (mask3 * GPIO_PUPDR_PUPDR0)) | mask3 * (pupdr) * GPIO_PUPDR_PUPDR0_0; \
			(gpio)->OTYPER = ((gpio)->OTYPER & ~ ((iomask) * GPIO_OTYPER_OT0)) | (iomask) * (typer); \
		  } while (0)
		// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
		#define tm32mp1_pioX_pupdr(gpio, up, down) \
		  do { \
			const portholder_t up3 = power2(up); \
			const portholder_t down3 = power2(down); \
			(gpio)->PUPDR = ((gpio)->PUPDR & ~ ((up3 | down3) * GPIO_PUPDR_PUPDR0)) | \
				up3 * (1) * GPIO_PUPDR_PUPDR0_0 | \
				down3 * (2) * GPIO_PUPDR_PUPDR0_0 | \
				0; \
		  } while (0)
		// отключение встроенной подтяжки на входе (так как программирование на ввод в данной библиотеке всегда включает подтяжку
		// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
		#define arm_stm32mp1_hardware_pio_pupoff(gpio, ipins) \
		  do { \
			const portholder_t ipins3 = power2(ipins);	\
			(gpio)->PUPDR = ((gpio)->PUPDR & ~ (ipins3 * GPIO_PUPDR_PUPDR0)) | ipins3 * (0) * GPIO_PUPDR_PUPDR0_0; \
		  } while (0)

		#define stm32mp1_pioX_altfn(gpio, opins, afn) \
			{ \
				const portholder_t op = (opins); \
				const portholder_t lo = power4((op) >> 0); \
				const portholder_t hi = power4((op) >> 8); \
				(gpio)->AFR [0] = ((gpio)->AFR [0] & ~ (lo * 0x0f)) | (lo * (afn)); \
				(gpio)->AFR [1] = ((gpio)->AFR [1] & ~ (hi * 0x0f)) | (hi * (afn)); \
			} while (0)

	/* разрешение прерывания по изменению состояния указанных групп выводов */
	static void
	stm32mp1_pioX_onchangeinterrupt(portholder_t ipins,
			portholder_t raise, portholder_t fall,
			portholder_t portcode, /* 0x00: PAxx, 0x01: PBxx, .. 0x0a: PKxx */
			uint32_t priority,
			uint_fast8_t targetcpu
			)
	{
		// CPU1 = MPU and CPU2 = MCU
		RCC->MP_APB3ENSETR |= RCC_MP_APB3ENSETR_SYSCFGEN;     // включить тактирование альтернативных функций
		(void) RCC->MP_APB3ENSETR;
		RCC->MP_APB3LPENSETR |= RCC_MP_APB3LPENSETR_SYSCFGLPEN;     // включить тактирование альтернативных функций
		(void) RCC->MP_APB3LPENSETR;

		#if 1
		{
			//
			const portholder_t bitpos0 = power8((ipins >> 0) & 0x0F);
			// EXTI_EXTICR1: PZ[3]..PA[3], PZ[2]..PA[2], PZ[1]..PA[0], PZ[3]..PA[0],
			EXTI->EXTICR [0] = (EXTI->EXTICR [0] & ~ (EXTI_EXTICR1_EXTI0 * bitpos0)) | (portcode * bitpos0);
			(void) EXTI->EXTICR [0];
		}
		{
			const portholder_t bitpos1 = power8((ipins >> 4) & 0x0F);
			// EXTI_EXTICR2: PZ[7]..PA[7], PZ[6]..PA[6], PZ[5]..PA[5], PZ[4]..PA[4],
			EXTI->EXTICR [1] = (EXTI->EXTICR [1] & ~ (EXTI_EXTICR2_EXTI4 * bitpos1)) | (portcode * bitpos1);
			(void) EXTI->EXTICR [1];
		}
		{
			const portholder_t bitpos2 = power8((ipins >> 8) & 0x0F);
			// EXTI_EXTICR3: PZ[11]..PA[11], PZ[10]..PA[10], PZ[9]..PA[9], PZ[8]..PA[8],
			EXTI->EXTICR [2] = (EXTI->EXTICR [2] & ~ (EXTI_EXTICR3_EXTI8 * bitpos2)) | (portcode * bitpos2);
			(void) EXTI->EXTICR [2];
		}
		{
			const portholder_t bitpos3 = power8((ipins >> 12) & 0x0F);
			// EXTI_EXTICR4: PZ[15]..PA[15], PZ[14]..PA[14], PZ[13]..PA[13], PZ[12]..PA[12],
			EXTI->EXTICR [3] = (EXTI->EXTICR [3] & ~ (EXTI_EXTICR4_EXTI12 * bitpos3)) | (portcode * bitpos3);
			(void) EXTI->EXTICR [3];
		}
		#else
		uint_fast8_t i;
		for (i = 0; i < 16; ++ i)
		{
			const portholder_t pinmask = (portholder_t) 1 << i;
			if ((ipins & pinmask) == 0)
				continue;	// Эти биты не трогаем

			const div_t d = div(i, 4);
			const portholder_t bitpos = (portholder_t) 1 << (d.rem * 8);
			const portholder_t bitmask = SYSCFG_EXTICR1_EXTI0 * bitpos;
			const portholder_t bitvalue = portcode * bitpos;
			EXTI->EXTICR [d.quot] = (EXTI->EXTICR [d.quot] & ~ bitmask) | bitvalue;
			(void) EXTI->EXTICR [d.quot];
	}
		#endif

		EXTI->RTSR1 = (EXTI->RTSR1 & ~ ipins) | (ipins & raise);		// прерывание по нарастанию
		(void) EXTI->RTSR1;
		EXTI->FTSR1 = (EXTI->FTSR1 & ~ ipins) | (ipins & fall);		// прерывание по спаду
		(void) EXTI->FTSR1;

		// CPU1 = MPU and CPU2 = MCU
		EXTI->C1IMR1 |= ipins;		// разрешить прерывание
		(void) EXTI->C1IMR1;

		if ((ipins & EXTI_IMR1_IM0) != 0)
			arm_hardware_set_handler(EXTI0_IRQn, EXTI0_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM1) != 0)
			arm_hardware_set_handler(EXTI1_IRQn, EXTI1_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM2) != 0)
			arm_hardware_set_handler(EXTI2_IRQn, EXTI2_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM3) != 0)
			arm_hardware_set_handler(EXTI3_IRQn, EXTI3_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM4) != 0)
			arm_hardware_set_handler(EXTI4_IRQn, EXTI4_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM5) != 0)
			arm_hardware_set_handler(EXTI5_IRQn, EXTI5_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM6) != 0)
			arm_hardware_set_handler(EXTI6_IRQn, EXTI6_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM7) != 0)
			arm_hardware_set_handler(EXTI7_IRQn, EXTI7_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM8) != 0)
			arm_hardware_set_handler(EXTI8_IRQn, EXTI8_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM9) != 0)
			arm_hardware_set_handler(EXTI9_IRQn, EXTI9_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM10) != 0)
			arm_hardware_set_handler(EXTI10_IRQn, EXTI10_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM11) != 0)
			arm_hardware_set_handler(EXTI11_IRQn, EXTI11_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM12) != 0)
			arm_hardware_set_handler(EXTI12_IRQn, EXTI12_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM13) != 0)
			arm_hardware_set_handler(EXTI13_IRQn, EXTI13_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM14) != 0)
			arm_hardware_set_handler(EXTI14_IRQn, EXTI14_IRQHandler, priority, targetcpu);
		if ((ipins & EXTI_IMR1_IM15) != 0)
			arm_hardware_set_handler(EXTI15_IRQn, EXTI15_IRQHandler, priority, targetcpu);

	}

	/* программирвоание битов в регистрах управления GPIO, указанных в iomask, в конфигурацию CNF И режим MODE */
	#elif \
		CPUSTYLE_STM32H7XX || \
		0

		/* программирование битов в регистрах управления GPIO, указанных в iomask, в конфигурацию MODER SPEED PUPDR TYPER */
		// modev: 0:input, 1:output, 2:alternative function mode, 3:analog mode
		// speed: 0:low speed, 1:maximum speed, 2:fast speed, 3:high speed
		// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
		// type: 0: Output push-pull, 1: output open-drain,
		#define stm32f30x_pioX_prog(gpio, iomask0, moder, speed, pupdr, typer) do { \
			const portholder_t iomask = (iomask0);	\
			const portholder_t mask3 = power2(iomask);	\
			(gpio)->MODER = ((gpio)->MODER & ~ (mask3 * GPIO_MODER_MODE0)) | mask3 * (moder) * GPIO_MODER_MODE0_0; \
			(gpio)->OSPEEDR = ((gpio)->OSPEEDR & ~ (mask3 * GPIO_OSPEEDR_OSPEED0)) | mask3 * (speed) * GPIO_OSPEEDR_OSPEED0_0; \
			(gpio)->PUPDR = ((gpio)->PUPDR & ~ (mask3 * GPIO_PUPDR_PUPD0)) | mask3 * (pupdr) * GPIO_PUPDR_PUPD0_0; \
			(gpio)->OTYPER = ((gpio)->OTYPER & ~ ((iomask) * GPIO_OTYPER_OT0)) | (iomask) * (typer); \
		  } while (0)
		// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
		#define tm32f30x_pioX_pupdr(gpio, up, down) do { \
			const portholder_t up3 = power2(up); \
			const portholder_t down3 = power2(down); \
			(gpio)->PUPDR = ((gpio)->PUPDR & ~ ((up3 | down3) * GPIO_PUPDR_PUPD0)) | \
				up3 * (1) * GPIO_PUPDR_PUPD0_0 | \
				down3 * (2) * GPIO_PUPDR_PUPD0_0 | \
				0; \
		  } while (0)
		// отключение встроенной подтяжки на входе (так как программирование на ввод в данной библиотеке всегда включает подтяжку
		// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
		#define arm_stm32f30x_hardware_pio_pupoff(gpio, ipins) do { \
			const portholder_t ipins3 = power2(ipins);	\
			(gpio)->PUPDR = ((gpio)->PUPDR & ~ (ipins3 * GPIO_PUPDR_PUPD0)) | ipins3 * (0) * GPIO_PUPDR_PUPD0_0; \
		  } while (0)

		#define stm32f30x_pioX_altfn(gpio, opins, afn) do { \
				const portholder_t op = (opins); \
				const portholder_t lo = power4((op) >> 0); \
				const portholder_t hi = power4((op) >> 8); \
				(gpio)->AFR [0] = ((gpio)->AFR [0] & ~ (lo * 0x0f)) | (lo * (afn)); \
				(gpio)->AFR [1] = ((gpio)->AFR [1] & ~ (hi * 0x0f)) | (hi * (afn)); \
			} while (0)

		/* разрешение прерывания по изменению состояния указанных групп выводов */
		static void
		stm32f30x_pioX_onchangeinterrupt(portholder_t ipins,
			portholder_t raise, portholder_t fall,
			portholder_t portcode,	/* 0x00: PAxx, 0x01: PBxx, .. 0x0a: PKxx */
			uint32_t priority
			)
		{
			#if CPUSTYLE_STM32H7XX
				RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;     // включить тактирование альтернативных функций
				(void) RCC->APB4ENR;
			#elif CPUSTYLE_STM32MP1
				#error wrong config
			#else /* CPUSTYLE_STM32H7XX */
				RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;     // включить тактирование альтернативных функций
				(void) RCC->APB2ENR;
			#endif /* CPUSTYLE_STM32H7XX */
			//const portholder_t portcode = AFIO_EXTICR1_EXTI0_PB;	// PORT B
			//RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     // включить тактирование альтернативных функций


		#if 1
			{
				const portholder_t bitpos0 = power4((ipins >> 0) & 0x0f);
				SYSCFG->EXTICR [0] = (SYSCFG->EXTICR [0] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos0)) | (portcode * bitpos0);
				(void) SYSCFG->EXTICR [0];
			}
			{
				const portholder_t bitpos1 = power4((ipins >> 4) & 0x0f);
				SYSCFG->EXTICR [1] = (SYSCFG->EXTICR [1] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos1)) | (portcode * bitpos1);
				(void) SYSCFG->EXTICR [1];
			}
			{
				const portholder_t bitpos2 = power4((ipins >> 8) & 0x0f);
				SYSCFG->EXTICR [2] = (SYSCFG->EXTICR [2] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos2)) | (portcode * bitpos2);
				(void) SYSCFG->EXTICR [2];
			}
			{
				const portholder_t bitpos3 = power4((ipins >> 12) & 0x0f);
				SYSCFG->EXTICR [3] = (SYSCFG->EXTICR [3] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos3)) | (portcode * bitpos3);
				(void) SYSCFG->EXTICR [3];
			}
		#else
			uint_fast8_t i;
			for (i = 0; i < 16; ++ i)
			{
				const portholder_t pinmask = (portholder_t) 1 << i;
				if ((ipins & pinmask) == 0)
					continue;	// Эти биты не трогаем

				const div_t d = div(i, 4);
				const portholder_t bitpos = (portholder_t) 1 << (d.rem * 4);
				const portholder_t bitmask = SYSCFG_EXTICR1_EXTI0 * bitpos;
				const portholder_t bitvalue = portcode * bitpos;
				SYSCFG->EXTICR [d.quot] = (SYSCFG->EXTICR [d.quot] & ~ bitmask) | bitvalue;
				(void) SYSCFG->EXTICR [d.quot];
			}
		#endif

			EXTI->RTSR1 = (EXTI->RTSR1 & ~ ipins) | (ipins & raise);		// прерывание по нарастанию
			(void) EXTI->RTSR1;
			EXTI->FTSR1 = (EXTI->FTSR1 & ~ ipins) | (ipins & fall);		// прерывание по спаду
			(void) EXTI->FTSR1;

			EXTI_D1->IMR1 |= ipins;		// разрешить прерывание
			(void) EXTI_D1->IMR1;
			EXTI_D1->EMR1 &= ~ ipins;		// запретить событие
			(void) EXTI_D1->EMR1;

			enum { targetcpu = 0 };	// stub

			if ((ipins & EXTI_IMR1_IM0) != 0)
				arm_hardware_set_handler(EXTI0_IRQn, EXTI0_IRQHandler, priority, targetcpu);
			if ((ipins & EXTI_IMR1_IM1) != 0)
				arm_hardware_set_handler(EXTI1_IRQn, EXTI1_IRQHandler, priority, targetcpu);
			if ((ipins & EXTI_IMR1_IM2) != 0)
				arm_hardware_set_handler(EXTI2_IRQn, EXTI2_IRQHandler, priority, targetcpu);
			if ((ipins & EXTI_IMR1_IM3) != 0)
				arm_hardware_set_handler(EXTI3_IRQn, EXTI3_IRQHandler, priority, targetcpu);
			if ((ipins & EXTI_IMR1_IM4) != 0)
				arm_hardware_set_handler(EXTI4_IRQn, EXTI4_IRQHandler, priority, targetcpu);
			if ((ipins & (EXTI_IMR1_IM9 | EXTI_IMR1_IM8 | EXTI_IMR1_IM7 | EXTI_IMR1_IM6 | EXTI_IMR1_IM5)) != 0)
				arm_hardware_set_handler(EXTI9_5_IRQn, EXTI9_5_IRQHandler, priority, targetcpu);
			if ((ipins & (EXTI_IMR1_IM15 | EXTI_IMR1_IM14 | EXTI_IMR1_IM13 | EXTI_IMR1_IM12 | EXTI_IMR1_IM11 | EXTI_IMR1_IM10)) != 0)
				arm_hardware_set_handler(EXTI15_10_IRQn, EXTI15_10_IRQHandler, priority, targetcpu);
		}

	/* программирвоание битов в регистрах управления GPIO, указанных в iomask, в конфигурацию CNF И режим MODE */
	#elif \
		CPUSTYLE_STM32F30X || \
		CPUSTYLE_STM32F4XX || \
		CPUSTYLE_STM32F0XX || \
		CPUSTYLE_STM32L0XX || \
		CPUSTYLE_STM32F7XX || \
		0

		/* программирование битов в регистрах управления GPIO, указанных в iomask, в конфигурацию MODER SPEED PUPDR TYPER */
		// modev: 0:input, 1:output, 2:alternative function mode, 3:analog mode
		// speed: 0:low speed, 1:maximum speed, 2:fast speed, 3:high speed
		// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
		// type: 0: Output push-pull, 1: output open-drain,
		#define stm32f30x_pioX_prog(gpio, iomask0, moder, speed, pupdr, typer) do { \
			const portholder_t iomask = (iomask0);	\
			const portholder_t mask3 = power2(iomask);	\
			(gpio)->MODER = ((gpio)->MODER & ~ (mask3 * GPIO_MODER_MODER0)) | mask3 * (moder) * GPIO_MODER_MODER0_0; \
			(gpio)->OSPEEDR = ((gpio)->OSPEEDR & ~ (mask3 * GPIO_OSPEEDER_OSPEEDR0)) | mask3 * (speed) * GPIO_OSPEEDER_OSPEEDR0_0; \
			(gpio)->PUPDR = ((gpio)->PUPDR & ~ (mask3 * GPIO_PUPDR_PUPDR0)) | mask3 * (pupdr) * GPIO_PUPDR_PUPDR0_0; \
			(gpio)->OTYPER = ((gpio)->OTYPER & ~ ((iomask) * GPIO_OTYPER_OT_0)) | (iomask) * (typer); \
		  } while (0)
		// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
		#define tm32f30x_pioX_pupdr(gpio, up, down) do { \
			const portholder_t up3 = power2(up); \
			const portholder_t down3 = power2(down); \
			(gpio)->PUPDR = ((gpio)->PUPDR & ~ ((up3 | down3) * GPIO_PUPDR_PUPDR0)) | \
				up3 * (1) * GPIO_PUPDR_PUPDR0_0 | \
				down3 * (2) * GPIO_PUPDR_PUPDR0_0 | \
				0; \
		  } while (0)
		// отключение встроенной подтяжки на входе (так как программирование на ввод в данной библиотеке всегда включает подтяжку
		// pupdr: 0:no pulls, 1:pull-up, 2: pull-down, 3:reserved
		#define arm_stm32f30x_hardware_pio_pupoff(gpio, ipins) do { \
			const portholder_t ipins3 = power2(ipins);	\
			(gpio)->PUPDR = ((gpio)->PUPDR & ~ (ipins3 * GPIO_PUPDR_PUPDR0)) | ipins3 * (0) * GPIO_PUPDR_PUPDR0_0; \
		  } while (0)

		#define stm32f30x_pioX_altfn(gpio, opins, afn) do { \
				const portholder_t op = (opins); \
				const portholder_t lo = power4((op) >> 0); \
				const portholder_t hi = power4((op) >> 8); \
				(gpio)->AFR [0] = ((gpio)->AFR [0] & ~ (lo * 0x0f)) | (lo * (afn)); \
				(gpio)->AFR [1] = ((gpio)->AFR [1] & ~ (hi * 0x0f)) | (hi * (afn)); \
			} while (0)

		/* разрешение прерывания по изменению состояния указанных групп выводов */
		static void 
		stm32f30x_pioX_onchangeinterrupt(portholder_t ipins,
				portholder_t raise, portholder_t fall,
				portholder_t portcode, uint32_t priority
				)
		{
			enum { targetcpu = 0 };

			#if CPUSTYLE_STM32H7XX
				RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;     // включить тактирование альтернативных функций
				(void) RCC->APB4ENR;
			#else /* CPUSTYLE_STM32H7XX */
				RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;     // включить тактирование альтернативных функций
				(void) RCC->APB2ENR;
			#endif /* CPUSTYLE_STM32H7XX */
			//const portholder_t portcode = AFIO_EXTICR1_EXTI0_PB;	// PORT B
			//RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     // включить тактирование альтернативных функций

		#if CPUSTYLE_STM32H7XX

			#if 1
				{
					const portholder_t bitpos0 = power4((ipins >> 0) & 0x0f);
					SYSCFG->EXTICR [0] = (SYSCFG->EXTICR [0] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos0)) | (portcode * bitpos0);
				}
				{
					const portholder_t bitpos1 = power4((ipins >> 4) & 0x0f);
					SYSCFG->EXTICR [1] = (SYSCFG->EXTICR [1] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos1)) | (portcode * bitpos1);
				}
				{
					const portholder_t bitpos2 = power4((ipins >> 8) & 0x0f);
					SYSCFG->EXTICR [2] = (SYSCFG->EXTICR [2] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos2)) | (portcode * bitpos2);
				}
				{
					const portholder_t bitpos3 = power4((ipins >> 12) & 0x0f);
					SYSCFG->EXTICR [3] = (SYSCFG->EXTICR [3] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos3)) | (portcode * bitpos3);
				}
			#else
				uint_fast8_t i;
				for (i = 0; i < 16; ++ i)
				{
					const portholder_t pinmask = (portholder_t) 1 << i;
					if ((ipins & pinmask) == 0)
						continue;	// Эти биты не трогаем

					const div_t d = div(i, 4);
					const portholder_t bitpos = (portholder_t) 1 << (d.rem * 4);
					const portholder_t bitmask = SYSCFG_EXTICR1_EXTI0 * bitpos;
					const portholder_t bitvalue = portcode * bitpos;
					AFIO->EXTICR [d.quot] = (SYSCFG->EXTICR [d.quot] & ~ bitmask) | bitvalue;
				}
			#endif

			EXTI->RTSR1 = (EXTI->RTSR1 & ~ ipins) | (ipins & raise);		// прерывание по нарастанию
			(void) EXTI->RTSR1;
			EXTI->FTSR1 = (EXTI->FTSR1 & ~ ipins) | (ipins & fall);		// прерывание по спаду
			(void) EXTI->FTSR1;

			EXTI_D1->IMR1 |= ipins;		// разрешить прерывание
			(void) EXTI_D1->IMR1;

		#else /* CPUSTYLE_STM32H7XX */

			#if 1
				{
					const portholder_t bitpos0 = power4((ipins >> 0) & 0x0f);
					SYSCFG->EXTICR [0] = (SYSCFG->EXTICR [0] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos0)) | (portcode * bitpos0);
				}
				{
					const portholder_t bitpos1 = power4((ipins >> 4) & 0x0f);
					SYSCFG->EXTICR [1] = (SYSCFG->EXTICR [1] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos1)) | (portcode * bitpos1);
				}
				{
					const portholder_t bitpos2 = power4((ipins >> 8) & 0x0f);
					SYSCFG->EXTICR [2] = (SYSCFG->EXTICR [2] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos2)) | (portcode * bitpos2);
				}
				{
					const portholder_t bitpos3 = power4((ipins >> 12) & 0x0f);
					SYSCFG->EXTICR [3] = (SYSCFG->EXTICR [3] & ~ (SYSCFG_EXTICR1_EXTI0 * bitpos3)) | (portcode * bitpos3);
				}
			#else
				uint_fast8_t i;
				for (i = 0; i < 16; ++ i)
				{
					const portholder_t pinmask = (portholder_t) 1 << i;
					if ((ipins & pinmask) == 0)
						continue;	// Эти биты не трогаем

					const div_t d = div(i, 4);
					const portholder_t bitpos = (portholder_t) 1 << (d.rem * 4);
					const portholder_t bitmask = SYSCFG_EXTICR1_EXTI0 * bitpos;
					const portholder_t bitvalue = portcode * bitpos;
					AFIO->EXTICR [d.quot] = (SYSCFG->EXTICR [d.quot] & ~ bitmask) | bitvalue;
				}
			#endif

			EXTI->RTSR = (EXTI->RTSR & ~ ipins) | (ipins & raise);		// прерывание по нарастанию
			EXTI->FTSR = (EXTI->FTSR & ~ ipins) | (ipins & fall);		// прерывание по спаду

			EXTI->IMR |= ipins;		// разрешить прерывание

		#endif /* CPUSTYLE_STM32H7XX */

		#if CPUSTYLE_STM32L0XX

			if ((ipins & (EXTI_IMR_IM1 | EXTI_IMR_IM0)) != 0)
				arm_hardware_set_handler(EXTI0_1_IRQn, EXTI0_1_IRQHandler, priority, targetcpu);
			else if ((ipins & (EXTI_IMR_IM2 | EXTI_IMR_IM3)) != 0)
				arm_hardware_set_handler(EXTI2_3_IRQn, EXTI2_3_IRQHandler, priority, targetcpu);
			else if ((ipins & (EXTI_IMR_IM4 | EXTI_IMR_IM5 | EXTI_IMR_IM6 | EXTI_IMR_IM7 | EXTI_IMR_IM8 | 
							EXTI_IMR_IM9 | EXTI_IMR_IM10 | EXTI_IMR_IM11 | EXTI_IMR_IM12 | EXTI_IMR_IM13 | 
							EXTI_IMR_IM14 | EXTI_IMR_IM15)) != 0)
				arm_hardware_set_handler(EXTI4_15_IRQn, EXTI4_15_IRQHandler, priority, targetcpu);

		#elif CPUSTYLE_STM32F0XX

			if ((ipins & (EXTI_IMR_MR1 | EXTI_IMR_MR0)) != 0)
				arm_hardware_set_handler(EXTI0_1_IRQn, EXTI0_1_IRQHandler, priority, targetcpu);
			else if ((ipins & (EXTI_IMR_MR2 | EXTI_IMR_MR3)) != 0)
				arm_hardware_set_handler(EXTI2_3_IRQn, EXTI2_3_IRQHandler, priority, targetcpu);
			else if ((ipins & (EXTI_MRR_MR4 | EXTI_MRR_MR5 | EXTI_MRR_MR6 | EXTI_MRR_MR7 | EXTI_MRR_MR8 | 
							EXTI_MRR_MR9 | EXTI_MRR_MR10 | EXTI_MRR_MR11 | EXTI_MRR_MR12 | EXTI_MRR_MR13 | 
							EXTI_MRR_MR14 | EXTI_MRR_MR15)) != 0)
				arm_hardware_set_handler(EXTI4_15_IRQn, EXTI4_15_IRQHandler, priority, targetcpu);

		#elif CPUSTYLE_STM32F7XX

			if ((ipins & (EXTI_IMR_MR0)) != 0)
				arm_hardware_set_handler(EXTI0_IRQn, EXTI0_IRQHandler, priority, targetcpu);
			if ((ipins & (EXTI_IMR_MR1)) != 0)
				arm_hardware_set_handler(EXTI1_IRQn, EXTI1_IRQHandler, priority, targetcpu);
			if ((ipins & (EXTI_IMR_MR2)) != 0)
				arm_hardware_set_handler(EXTI2_IRQn, EXTI2_IRQHandler, priority, targetcpu);
			if ((ipins & (EXTI_IMR_MR3)) != 0)
				arm_hardware_set_handler(EXTI3_IRQn, EXTI3_IRQHandler, priority, targetcpu);
			if ((ipins & (EXTI_IMR_MR4)) != 0)
				arm_hardware_set_handler(EXTI4_IRQn, EXTI4_IRQHandler, priority, targetcpu);
			if ((ipins & (EXTI_IMR_MR9 | EXTI_IMR_MR8 | EXTI_IMR_MR7 | EXTI_IMR_MR6 | EXTI_IMR_MR5)) != 0)
				arm_hardware_set_handler(EXTI9_5_IRQn, EXTI9_5_IRQHandler, priority, targetcpu);
			if ((ipins & (EXTI_IMR_MR15 | EXTI_IMR_MR14 | EXTI_IMR_MR14 | EXTI_IMR_MR13 | EXTI_IMR_MR12 | EXTI_IMR_MR11 | EXTI_IMR_MR10)) != 0)
				arm_hardware_set_handler(EXTI15_10_IRQn, EXTI15_10_IRQHandler, priority, targetcpu);

		#elif CPUSTYLE_STM32H7XX

			if ((ipins & EXTI_IMR1_IM0) != 0)
				arm_hardware_set_handler(EXTI0_IRQn, EXTI0_IRQHandler, priority, targetcpu);
			if ((ipins & EXTI_IMR1_IM1) != 0)
				arm_hardware_set_handler(EXTI1_IRQn, EXTI1_IRQHandler, priority, targetcpu);
			if ((ipins & EXTI_IMR1_IM2) != 0)
				arm_hardware_set_handler(EXTI2_IRQn, EXTI2_IRQHandler, priority, targetcpu);
			if ((ipins & EXTI_IMR1_IM3) != 0)
				arm_hardware_set_handler(EXTI3_IRQn, EXTI3_IRQHandler, priority, targetcpu);
			if ((ipins & EXTI_IMR1_IM4) != 0)
				arm_hardware_set_handler(EXTI4_IRQn, EXTI4_IRQHandler, priority, targetcpu);
			if ((ipins & (EXTI_IMR1_IM9 | EXTI_IMR1_IM8 | EXTI_IMR1_IM7 | EXTI_IMR1_IM6 | EXTI_IMR1_IM5)) != 0)
				arm_hardware_set_handler(EXTI9_5_IRQn, EXTI9_5_IRQHandler, priority, targetcpu);
			if ((ipins & (EXTI_IMR1_IM15 | EXTI_IMR1_IM14 | EXTI_IMR1_IM13 | EXTI_IMR1_IM12 | EXTI_IMR1_IM11 | EXTI_IMR1_IM10)) != 0)
				arm_hardware_set_handler(EXTI15_10_IRQn, EXTI15_10_IRQHandler, priority, targetcpu);

		#else /* CPUSTYLE_STM32F0XX */

			if ((ipins & EXTI_IMR_MR0) != 0)
				arm_hardware_set_handler(EXTI0_IRQn, EXTI0_IRQHandler, priority, targetcpu);
			if ((ipins & EXTI_IMR_MR1) != 0)
				arm_hardware_set_handler(EXTI1_IRQn, EXTI1_IRQHandler, priority, targetcpu);
			if ((ipins & EXTI_IMR_MR2) != 0)
			{
				#if CPUSTYLE_STM32F4XX
					arm_hardware_set_handler(EXTI2_IRQn, EXTI2_IRQHandler, priority, targetcpu);
				#else
					arm_hardware_set_handler(EXTI2_TS_IRQn, EXTI2_TS_IRQHandler, priority, targetcpu);
				#endif
			}
			if ((ipins & EXTI_IMR_MR3) != 0)
				arm_hardware_set_handler(EXTI3_IRQn, EXTI3_IRQHandler, priority, targetcpu);
			if ((ipins & EXTI_IMR_MR4) != 0)
				arm_hardware_set_handler(EXTI4_IRQn, EXTI4_IRQHandler, priority, targetcpu);
			if ((ipins & (EXTI_IMR_MR9 | EXTI_IMR_MR8 | EXTI_IMR_MR7 | EXTI_IMR_MR6 | EXTI_IMR_MR5)) != 0)
				arm_hardware_set_handler(EXTI9_5_IRQn, EXTI9_5_IRQHandler, priority, targetcpu);
			if ((ipins & (EXTI_IMR_MR15 | EXTI_IMR_MR14 | EXTI_IMR_MR13 | EXTI_IMR_MR12 | EXTI_IMR_MR11 | EXTI_IMR_MR10)) != 0)
				arm_hardware_set_handler(EXTI15_10_IRQn, EXTI15_10_IRQHandler, priority, targetcpu);

		#endif /* CPUSTYLE_STM32F0XX */

		}

	#else
		//#error Undefined CPUSTYLE_XXX
	#endif /* CPUSTYLE_STM32F30X */


/* программирование выводов на ввод, без присоединения к периферии */
void 
arm_hardware_pioa_inputs(unsigned long ipins)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = 1uL << AT91C_ID_PIOA; // разрешить тактированние этого блока

	AT91C_BASE_PIOA->PIO_PER = ipins;				// initial disconnect - PIO enable
	AT91C_BASE_PIOA->PIO_ODR = ipins;		// this pin not output.
	AT91C_BASE_PIOA->PIO_PPUER = ipins;	// enable pull-up resistors
	//AT91C_BASE_PIOA->PIO_IFER = ipins;	// glitch filter enable
#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1uL << ID_PIOA);	// разрешить тактированние этого блока

    PIOA->PIO_PER = ipins;	// initial disconnect - PIO enable
    PIOA->PIO_ODR = ipins; // эти выводы на  ввод
	PIOA->PIO_PUER = ipins;


#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	(void) RCC->APB2ENR;
	GPIOA->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOA, ipins, 2, 0);	/* Установить CNF=2 и MODE=0 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->IOPENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOALPEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOA, ipins, STM32MP1_GPIO_MODE_INPIUT, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

//	//gpioX_poweron(GPIOA);
//	gpioX_prog(GPIOA, ipins, GPIO_CFG_IN, ALWNR_GPIO_DRV_INPUT, ALWNR_GPIO_PULL_INPUT);

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}

void 
arm_hardware_piob_inputs(unsigned long ipins)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1uL << ID_PIOB);	// разрешить тактированние этого блока

    PIOA->PIO_PER = ipins;	// initial disconnect - PIO enable
    PIOB->PIO_ODR = ipins; // эти выводы на  ввод
	PIOB->PIO_PUER = ipins;

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	(void) RCC->APB2ENR;
	GPIOB->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOB, ipins, 2, 0);	/* Установить CNF=2 и MODE=0 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->IOPENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOBLPEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOB, ipins, STM32MP1_GPIO_MODE_INPIUT, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOB);
	gpioX_prog(GPIOB, ipins, GPIO_CFG_IN, ALWNR_GPIO_DRV_INPUT, ALWNR_GPIO_PULL_INPUT);

#elif defined (GPIOB)
	#error Undefined CPUSTYLE_XXX

#endif
}

void 
arm_hardware_pioc_inputs(unsigned long ipins)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1uL << ID_PIOC);	// разрешить тактированние этого блока

    PIOA->PIO_PER = ipins;	// initial disconnect - PIO enable
    PIOC->PIO_ODR = ipins; // эти выводы на  ввод
	PIOC->PIO_PUER = ipins;

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	(void) RCC->APB2ENR;
	GPIOC->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOC, ipins, 2, 0);	/* Установить CNF=2 и MODE=0 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->IOPENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOCLPEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOC, ipins, STM32MP1_GPIO_MODE_INPIUT, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOC);
	gpioX_prog(GPIOC, ipins, GPIO_CFG_IN, ALWNR_GPIO_DRV_INPUT, ALWNR_GPIO_PULL_INPUT);

#elif defined (GPIOC)
	#error Undefined CPUSTYLE_XXX

#endif
}

void 
arm_hardware_piod_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	(void) RCC->APB2ENR;
	GPIOD->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOD, ipins, 2, 0);	/* Установить CNF=2 и MODE=0 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->IOPENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIODLPEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOD, ipins, STM32MP1_GPIO_MODE_INPIUT, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOD);
	gpioX_prog(GPIOD, ipins, GPIO_CFG_IN, ALWNR_GPIO_DRV_INPUT, ALWNR_GPIO_PULL_INPUT);

#elif defined (GPIOD)
	#error Undefined CPUSTYLE_XXX

#endif
}

void 
arm_hardware_pioe_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	(void) RCC->APB2ENR;
	GPIOE->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOE, ipins, 2, 0);	/* Установить CNF=2 и MODE=0 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOELPEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOE, ipins, STM32MP1_GPIO_MODE_INPIUT, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOE);
	gpioX_prog(GPIOE, ipins, GPIO_CFG_IN, ALWNR_GPIO_DRV_INPUT, ALWNR_GPIO_PULL_INPUT);

#elif defined (GPIOE)
	#error Undefined CPUSTYLE_XXX

#endif
}


#if defined (GPIOF)
void 
arm_hardware_piof_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPFEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	(void) RCC->APB2ENR;
	GPIOF->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOF, ipins, 2, 0);	/* Установить CNF=2 и MODE=0 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOFEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOFLPEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOF, ipins, STM32MP1_GPIO_MODE_INPIUT, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOF);
	gpioX_prog(GPIOF, ipins, GPIO_CFG_IN, ALWNR_GPIO_DRV_INPUT, ALWNR_GPIO_PULL_INPUT);

#elif defined (GPIOF)
	#error Undefined CPUSTYLE_XXX

#endif
}
#endif /* defined (GPIOF) */

#if defined (GPIOG)
void 
arm_hardware_piog_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPGEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	(void) RCC->APB2ENR;
	GPIOG->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOG, ipins, 2, 0);	/* Установить CNF=2 и MODE=0 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOGEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOGLPEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOG, ipins, STM32MP1_GPIO_MODE_INPIUT, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOG);
	gpioX_prog(GPIOG, ipins, GPIO_CFG_IN, ALWNR_GPIO_DRV_INPUT, ALWNR_GPIO_PULL_INPUT);

#elif defined (GPIG)
	#error Undefined CPUSTYLE_XXX

#endif
}
#endif /* defined (GPIOG) */

void 
arm_hardware_pioa_analoginput(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	(void) RCC->APB2ENR;
	GPIOA->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOA, ipins, 0, 0);	/* Установить CNF=0 и MODE=0 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->IOPENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOALPEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOA, ipins, STM32MP1_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}

void 
arm_hardware_piob_analoginput(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	(void) RCC->APB2ENR;
	GPIOB->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOB, ipins, 0, 0);	/* Установить CNF=0 и MODE=0 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->IOPENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOBLPEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOB, ipins, STM32MP1_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

#elif defined (GPIOB)
	#error Undefined CPUSTYLE_XXX

#endif
}

void 
arm_hardware_pioc_analoginput(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	(void) RCC->APB2ENR;
	GPIOC->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOC, ipins, 0, 0);	/* Установить CNF=0 и MODE=0 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->IOPENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, ipins, STM32F_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOCLPEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOC, ipins, STM32MP1_GPIO_MODE_ANALOG, 1, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

#elif defined (GPIOC)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* программирование выводов на вывод, без присоединения к периферии */
/* Установка режима - вывод, с ограничением скорости (на STM32) 10 МГц	*/
void 
arm_hardware_pioa_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = 1uL << AT91C_ID_PIOA; // разрешить тактированние этого блока

	AT91C_BASE_PIOA->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	AT91C_BASE_PIOA->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

	AT91C_BASE_PIOA->PIO_OER = opins;	// Разрешение выхода
	AT91C_BASE_PIOA->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOA->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1uL << ID_PIOA);

 	PIOA->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	PIOA->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

	PIOA->PIO_OER = opins; // эти выводы на вывод - похоже, при работе с периферийными устройствами не требуется.
	PIOA->PIO_PUDR = opins;
	PIOA->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 0, 1);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOALPEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOA, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

//	//gpioX_poweron(GPIOA);
//	gpioX_setstate(GPIOA, opins, initialstate);
//	gpioX_prog(GPIOA, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT20M, ALWNR_GPIO_PULL_OUTPUT20M);

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}
/* программирование выводов на вывод, без присоединения к периферии */
/* Установка режима - вывод, с ограничением скорости (на STM32) 10 МГц	*/
void 
arm_hardware_pioa_outputs10m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = 1uL << AT91C_ID_PIOA; // разрешить тактированние этого блока

	AT91C_BASE_PIOA->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	AT91C_BASE_PIOA->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

	AT91C_BASE_PIOA->PIO_OER = opins;	// Разрешение выхода
	AT91C_BASE_PIOA->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOA->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1uL << ID_PIOA);

 	PIOA->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	PIOA->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

    PIOA->PIO_OER = opins; // эти выводы на вывод - похоже, при работе с периферийными устройствами не требуется.
	PIOA->PIO_PUDR = opins;
	PIOA->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов mode: 1: 10MHz, 2: 2MHzm 3: 50 MHz
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 0, 1);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOALPEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOA, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

//	//gpioX_poweron(GPIOA);
//	gpioX_setstate(GPIOA, opins, initialstate);
//	gpioX_prog(GPIOA, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT10M, ALWNR_GPIO_PULL_OUTPUT10M);

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}
/* программирование выводов на вывод, без присоединения к периферии */
/* Установка режима - вывод, без ограничения скорости (на STM32) 50 МГц	*/
void 
arm_hardware_pioa_outputs50m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = 1uL << AT91C_ID_PIOA; // разрешить тактированние этого блока

	AT91C_BASE_PIOA->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	AT91C_BASE_PIOA->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

	AT91C_BASE_PIOA->PIO_OER = opins;	// Разрешение выхода
	AT91C_BASE_PIOA->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOA->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1uL << ID_PIOA);

 	PIOA->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	PIOA->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

    PIOA->PIO_OER = opins; // эти выводы на вывод - похоже, при работе с периферийными устройствами не требуется.
	PIOA->PIO_PUDR = opins;
	PIOA->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов mode: 1: 10MHz, 2: 2MHzm 3: 50 MHz
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 0, 3);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */


#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOALPEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOA, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

//	//gpioX_poweron(GPIOA);
//	gpioX_setstate(GPIOA, opins, initialstate);
//	gpioX_prog(GPIOA, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT50M, ALWNR_GPIO_PULL_OUTPUT50M);

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* Установка режима - вывод, с ограничением скорости (на STM32) 10 МГц	*/
void 
arm_hardware_piob_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S
#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1uL << ID_PIOB);

 	PIOB->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	PIOB->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

    PIOB->PIO_OER = opins; // эти выводы на вывод - похоже, при работе с периферийными устройствами не требуется.
	PIOB->PIO_PUDR = opins;
	PIOB->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов mode: 1: 10MHz, 2: 2MHzm 3: 50 MHz
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 0, 1);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOBLPEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOB, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOB);
	gpioX_setstate(GPIOB, opins, initialstate);
	gpioX_prog(GPIOB, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT20M, ALWNR_GPIO_PULL_OUTPUT20M);

#elif defined (GPIOB)
	#error Undefined CPUSTYLE_XXX

#endif
}
/* программирование выводов на вывод, без присоединения к периферии */
/* Установка режима - вывод, без ограничения скорости (на STM32) 50 МГц	*/
void 
arm_hardware_piob_outputs50m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S
#elif CPUSTYLE_AT91SAM9XE

	AT91C_BASE_PMC->PMC_PCER = 1uL << AT91C_ID_PIOB; // разрешить тактированние этого блока

	AT91C_BASE_PIOB->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	AT91C_BASE_PIOB->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

	AT91C_BASE_PIOB->PIO_OER = opins;	// Разрешение выхода
	AT91C_BASE_PIOB->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOB->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1uL << ID_PIOB);

 	PIOB->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	PIOB->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

    PIOB->PIO_OER = opins; // эти выводы на вывод - похоже, при работе с периферийными устройствами не требуется.
	PIOB->PIO_PUDR = opins;
	PIOB->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов mode: 1: 10MHz, 2: 2MHzm 3: 50 MHz
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 0, 3);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOBLPEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOB, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOB);
	gpioX_setstate(GPIOB, opins, initialstate);
	gpioX_prog(GPIOB, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT50M, ALWNR_GPIO_PULL_OUTPUT50M);

#elif defined (GPIOB)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* программирование выводов на вывод, без присоединения к периферии */
/* Установка режима - вывод, без ограничения скорости (на STM32) 50 МГц	*/
void 
arm_hardware_pioc_outputs50m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S
#elif CPUSTYLE_AT91SAM9XE

	AT91C_BASE_PMC->PMC_PCER = 1uL << AT91C_ID_PIOC; // разрешить тактированние этого блока

	AT91C_BASE_PIOC->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	AT91C_BASE_PIOC->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

	AT91C_BASE_PIOC->PIO_OER = opins;	// Разрешение выхода
	AT91C_BASE_PIOC->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOC->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1uL << ID_PIOC);

 	PIOC->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	PIOC->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

    PIOC->PIO_OER = opins; // эти выводы на вывод - похоже, при работе с периферийными устройствами не требуется.
	PIOC->PIO_PUDR = opins;
	PIOC->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов mode: 1: 10MHz, 2: 2MHzm 3: 50 MHz
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 0, 3);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOCLPEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOC, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOC);
	gpioX_setstate(GPIOC, opins, initialstate);
	gpioX_prog(GPIOC, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT50M, ALWNR_GPIO_PULL_OUTPUT50M);

#elif defined (GPIOC)
	#error Undefined CPUSTYLE_XXX

#endif
}


/* программирование выводов на вывод, без присоединения к периферии */
/* Установка режима - вывод, без ограничения скорости (на STM32) 50 МГц	*/
void
arm_hardware_piod_outputs50m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S
#elif CPUSTYLE_AT91SAM9XE

	AT91C_BASE_PMC->PMC_PCER = 1uL << AT91C_ID_PIOD; // разрешить тактированние этого блока

	AT91C_BASE_PIOD->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	AT91C_BASE_PIOD->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

	AT91C_BASE_PIOD->PIO_OER = opins;	// Разрешение выхода
	AT91C_BASE_PIOD->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOD->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1uL << ID_PIOD);

 	PIOD->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
 	PIOD->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

 	PIOD->PIO_OER = opins; // эти выводы на вывод - похоже, при работе с периферийными устройствами не требуется.
 	PIOD->PIO_PUDR = opins;
 	PIOD->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов mode: 1: 10MHz, 2: 2MHzm 3: 50 MHz
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 0, 3);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port D clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIODLPEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOD, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOD);
	gpioX_setstate(GPIOD, opins, initialstate);
	gpioX_prog(GPIOD, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT50M, ALWNR_GPIO_PULL_OUTPUT50M);

#elif defined (GPIOD)
	#error Undefined CPUSTYLE_XXX

#endif
}


/* программирование выводов на вывод, без присоединения к периферии */
/* Установка режима - вывод, без ограничения скорости (на STM32) 50 МГц	*/
void
arm_hardware_pioe_outputs50m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S
#elif CPUSTYLE_AT91SAM9XE

	AT91C_BASE_PMC->PMC_PCER = 1uL << AT91C_ID_PIOE; // разрешить тактированние этого блока

	AT91C_BASE_PIOE->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	AT91C_BASE_PIOE->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

	AT91C_BASE_PIOE->PIO_OER = opins;	// Разрешение выхода
	AT91C_BASE_PIOE->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOE->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1uL << ID_PIOE);

 	PIOE->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
 	PIOE->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

 	PIOE->PIO_OER = opins; // эти выводы на вывод - похоже, при работе с периферийными устройствами не требуется.
 	PIOE->PIO_PUDR = opins;
 	PIOE->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов mode: 1: 10MHz, 2: 2MHzm 3: 50 MHz
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 0, 3);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOELPEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOE, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOE);
	gpioX_setstate(GPIOE, opins, initialstate);
	gpioX_prog(GPIOE, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT50M, ALWNR_GPIO_PULL_OUTPUT50M);

#elif defined (GPIOE)
	#error Undefined CPUSTYLE_XXX

#endif
}


/* Установка режима - вывод, с ограничением скорости (на STM32) 10 МГц	*/
void 
arm_hardware_pioc_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1uL << ID_PIOC);

 	PIOC->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	PIOC->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

    PIOC->PIO_OER = opins; // эти выводы на вывод - похоже, при работе с периферийными устройствами не требуется.
	PIOC->PIO_PUDR = opins;
	PIOC->PIO_MDDR = opins;	// Disable open drain output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 0, 1);	/* Установить CNF=0 и MODE=1 для указанных битов: Max. output speed 10 MHz */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOCLPEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOC, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOC);
	gpioX_setstate(GPIOC, opins, initialstate);
	gpioX_prog(GPIOC, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT20M, ALWNR_GPIO_PULL_OUTPUT20M);

#elif defined (GPIOC)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* Установка режима - вывод, с ограничением скорости 2 МГц	*/
void 
arm_hardware_pioa_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 0, 2);	/* Установить CNF=0 и MODE=2 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOALPEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOA, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

//	//gpioX_poweron(GPIOA);
//	gpioX_setstate(GPIOA, opins, initialstate);
//	gpioX_prog(GPIOA, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT2M, ALWNR_GPIO_PULL_OUTPUT2M);

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* Установка режима - вывод, с ограничением скорости 2 МГц	*/
void 
arm_hardware_piob_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 0, 2);	/* Установить CNF=0 и MODE=2 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOBLPEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOB, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOB);
	gpioX_setstate(GPIOB, opins, initialstate);
	gpioX_prog(GPIOB, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT2M, ALWNR_GPIO_PULL_OUTPUT2M);

#elif defined (GPIOB)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* Установка режима - вывод, с ограничением скорости 2 МГц	*/
void 
arm_hardware_pioc_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 0, 2);	/* Установить CNF=0 и MODE=2 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOCLPEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOC, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOC);
	gpioX_setstate(GPIOC, opins, initialstate);
	gpioX_prog(GPIOC, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT2M, ALWNR_GPIO_PULL_OUTPUT2M);

#elif defined (GPIOC)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* Установка режима - вывод, с ограничением скорости 2 МГц	*/
void 
arm_hardware_piod_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 0, 2);	/* Установить CNF=0 и MODE=2 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIODLPEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOD, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOD);
	gpioX_setstate(GPIOD, opins, initialstate);
	gpioX_prog(GPIOD, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT2M, ALWNR_GPIO_PULL_OUTPUT2M);

#elif defined (GPIOD)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* Установка режима - вывод, с ограничением скорости (на STM32) 10 МГц	*/
void 
arm_hardware_piod_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 0, 1);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIODLPEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOD, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOD);
	gpioX_setstate(GPIOD, opins, initialstate);
	gpioX_prog(GPIOD, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT20M, ALWNR_GPIO_PULL_OUTPUT20M);

#elif defined (GPIOD)
	#error Undefined CPUSTYLE_XXX

#endif
}
/* Установка режима - вывод, с ограничением скорости (на STM32) 10 МГц	*/
void 
arm_hardware_pioe_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 0, 1);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	//GPIOE->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOELPEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOE, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOE);
	gpioX_setstate(GPIOE, opins, initialstate);
	gpioX_prog(GPIOE, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT20M, ALWNR_GPIO_PULL_OUTPUT20M);

#elif defined (GPIOE)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* Установка режима - вывод, с ограничением скорости 2 МГц	*/
void 
arm_hardware_pioe_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 0, 2);	/* Установить CNF=0 и MODE=2 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOELPEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOE, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOE);
	gpioX_setstate(GPIOE, opins, initialstate);
	gpioX_prog(GPIOE, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT2M, ALWNR_GPIO_PULL_OUTPUT2M);

#elif defined (GPIOE)
	#error Undefined CPUSTYLE_XXX

#endif
}

#if defined(GPIOF)

/* Установка режима - вывод, с ограничением скорости (на STM32) 10 МГц	*/
void 
arm_hardware_piof_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPFEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 0, 1);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32H7XX

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	//GPIOF->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOFLPEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOF, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOF);
	gpioX_setstate(GPIOF, opins, initialstate);
	gpioX_prog(GPIOF, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT20M, ALWNR_GPIO_PULL_OUTPUT20M);

#elif defined (GPIOF)
	#error Undefined CPUSTYLE_XXX


#endif
}

/* Установка режима - вывод, с ограничением скорости 2 МГц	*/
void 
arm_hardware_piof_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPFEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 0, 2);	/* Установить CNF=0 и MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOFEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOFLPEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOF, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOF);
	gpioX_setstate(GPIOF, opins, initialstate);
	gpioX_prog(GPIOF, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT2M, ALWNR_GPIO_PULL_OUTPUT2M);

#elif defined (GPIOF)
	#error Undefined CPUSTYLE_XXX


#endif
}

#endif /* defined(GPIOF) */

#if defined(GPIOG)

/* Установка режима - вывод, с ограничением скорости (на STM32) 10 МГц	*/
void 
arm_hardware_piog_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPGEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 0, 1);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	//GPIOG->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOGLPEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOG, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOG);
	gpioX_setstate(GPIOG, opins, initialstate);
	gpioX_prog(GPIOG, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT20M, ALWNR_GPIO_PULL_OUTPUT20M);

#elif defined (GPIOG)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* Установка режима - вывод, с ограничением скорости 2 МГц	*/
void 
arm_hardware_piog_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPGEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 0, 2);	/* Установить CNF=0 и MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	//GPIOG->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOGLPEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOG, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOG);
	gpioX_setstate(GPIOG, opins, initialstate);
	gpioX_prog(GPIOG, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT2M, ALWNR_GPIO_PULL_OUTPUT2M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}
/* Установка режима - вывод, без ограничения скорости	*/
void 
arm_hardware_piog_outputs50m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPGEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 0, 2);	/* Установить CNF=0 и MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	//GPIOG->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOGLPEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOG, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOG);
	gpioX_setstate(GPIOG, opins, initialstate);
	gpioX_prog(GPIOG, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT50M, ALWNR_GPIO_PULL_OUTPUT50M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined(GPIOG) */

#if defined(GPIOH)

/* Установка режима - вывод, с ограничением скорости (на STM32) 10 МГц	*/
void 
arm_hardware_pioh_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPHEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOH, opins, 0, 1);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOHEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	//GPIOG->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOHLPEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOH, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOB);
	gpioX_setstate(GPIOH, opins, initialstate);
	gpioX_prog(GPIOH, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT20M, ALWNR_GPIO_PULL_OUTPUT20M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* Установка режима - вывод, с ограничением скорости 2 МГц	*/
void 
arm_hardware_pioh_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPHEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOH, opins, 0, 2);	/* Установить CNF=0 и MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOHEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOHLPEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOH, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_A64

	//gpioX_poweron(GPIOB);
	gpioX_setstate(GPIOH, opins, initialstate);
	gpioX_prog(GPIOH, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT2M, ALWNR_GPIO_PULL_OUTPUT2M);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined(GPIOH) */


#if defined(GPIOI)

/* Установка режима - вывод, с ограничением скорости (на STM32) 10 МГц	*/
void 
arm_hardware_pioi_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPIEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPIEN;	/* I/O port I clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOI, opins, 0, 1);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOIEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	//GPIOG->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOILPEN;	/* I/O port I clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOI, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* Установка режима - вывод, с ограничением скорости 2 МГц	*/
void 
arm_hardware_pioi_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPIEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPIEN;	/* I/O port I clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOI, opins, 0, 2);	/* Установить CNF=0 и MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOIEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOILPEN;	/* I/O port I clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOI, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOI, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined(GPIOI) */


#if defined(GPIOK)

/* Установка режима - вывод, с ограничением скорости (на STM32) 10 МГц	*/
void 
arm_hardware_piok_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPKEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOK, opins, 0, 1);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOKEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	//GPIOG->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOKLPEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOK, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* Установка режима - вывод, с ограничением скорости 2 МГц	*/
void 
arm_hardware_piok_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPKEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOK, opins, 0, 2);	/* Установить CNF=0 и MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOKEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOKLPEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOK, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOK, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined(GPIOK) */


#if defined(GPIOZ)

void
arm_hardware_pioz_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPZEN;	/* I/O port Z clock enable */
	(void) RCC->APB2ENR;
	GPIOE->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOZ, ipins, 2, 0);	/* Установить CNF=2 и MODE=0 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB5ENSETR = RCC_MP_AHB5ENSETR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->MP_AHB5ENSETR;
	RCC->MP_AHB5LPENSETR = RCC_MP_AHB5LPENSETR_GPIOZLPEN;	/* I/O port Z clock enable */
	(void) RCC->MP_AHB5LPENSETR;

	//GPIOZ->SECCFGR &= ~ (ipins); // GPIO_SECCFGR_SEC0_Msk .. GPIO_SECCFGR_SEC7_Msk
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOZ, ipins, STM32MP1_GPIO_MODE_INPIUT, 1, 1, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* Установка режима - вывод, с ограничением скорости (на STM32) 10 МГц	*/
void
arm_hardware_pioz_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPZEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPZEN;	/* I/O port Z clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOZ, opins, 0, 1);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	//GPIOZ->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB5ENSETR = RCC_MP_AHB5ENSETR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->MP_AHB5ENSETR;
	RCC->MP_AHB5LPENSETR = RCC_MP_AHB5LPENSETR_GPIOZLPEN;	/* I/O port Z clock enable */
	(void) RCC->MP_AHB5LPENSETR;

	//GPIOZ->SECCFGR &= ~ (opins); // GPIO_SECCFGR_SEC0_Msk .. GPIO_SECCFGR_SEC7_Msk
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOZ, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* Установка режима - вывод, с ограничением скорости 2 МГц	*/
void
arm_hardware_pioz_outputs2m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPGEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPZEN;	/* I/O port Z clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOZ, opins, 0, 2);	/* Установить CNF=0 и MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	//GPIOZ->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB5ENSETR = RCC_MP_AHB5ENSETR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->MP_AHB5ENSETR;
	RCC->MP_AHB5LPENSETR = RCC_MP_AHB5LPENSETR_GPIOZLPEN;	/* I/O port Z clock enable */
	(void) RCC->MP_AHB5LPENSETR;

	//GPIOZ->SECCFGR &= ~ (opins); // GPIO_SECCFGR_SEC0_Msk .. GPIO_SECCFGR_SEC7_Msk
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOZ, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}
/* Установка режима - вывод, без ограничения скорости	*/
void
arm_hardware_pioz_outputs50m(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPZEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOZ, opins, 0, 2);	/* Установить CNF=0 и MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	//GPIOZ->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB5ENSETR = RCC_MP_AHB5ENSETR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->MP_AHB5ENSETR;
	RCC->MP_AHB5LPENSETR = RCC_MP_AHB5LPENSETR_GPIOZLPEN;	/* I/O port Z clock enable */
	(void) RCC->MP_AHB5LPENSETR;

	//GPIOZ->SECCFGR &= ~ (opins); // GPIO_SECCFGR_SEC0_Msk .. GPIO_SECCFGR_SEC7_Msk
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOZ, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

void
arm_hardware_pioz_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPHEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPZEN;	/* I/O port Z clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOZ, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOHEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB5ENSETR = RCC_MP_AHB5ENSETR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->MP_AHB5ENSETR;
	RCC->MP_AHB5LPENSETR = RCC_MP_AHB5LPENSETR_GPIOZLPEN;	/* I/O port Z clock enable */
	(void) RCC->MP_AHB5LPENSETR;

	//GPIOZ->SECCFGR &= ~ (opins); // GPIO_SECCFGR_SEC0_Msk .. GPIO_SECCFGR_SEC7_Msk
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOZ, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOZ, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 20 МГц, push-pull */
void
arm_hardware_pioz_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPZEN;	/* I/O port Z clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOZ, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOZ, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOZ, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOZ, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOZ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB5ENSETR = RCC_MP_AHB5ENSETR_GPIOZEN;	/* I/O port Z clock enable */
	(void) RCC->MP_AHB5ENSETR;
	RCC->MP_AHB5LPENSETR = RCC_MP_AHB5LPENSETR_GPIOZLPEN;	/* I/O port Z clock enable */
	(void) RCC->MP_AHB5LPENSETR;

	//GPIOZ->SECCFGR &= ~ (opins); // GPIO_SECCFGR_SEC0_Msk .. GPIO_SECCFGR_SEC7_Msk

	stm32mp1_pioX_altfn(GPIOZ, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOZ, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined (GPIOZ) */


/* программирование выводов на вывод, без присоединения к периферии - открытый сток */
void 
arm_hardware_pioa_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = 1uL << AT91C_ID_PIOA; // разрешить тактированние этого блока

	AT91C_BASE_PIOA->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	AT91C_BASE_PIOA->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

	AT91C_BASE_PIOA->PIO_OER = opins;	// Разрешение выхода
	AT91C_BASE_PIOA->PIO_PPUDR = opins;	// disable pull-up resistors
	AT91C_BASE_PIOA->PIO_MDER = opins;	// 5 volt (open drain) output

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1uL << ID_PIOA);

 	PIOA->PIO_SODR = (opins & initialstate);	// Установка единицы в регистре данных
	PIOA->PIO_CODR = (opins & ~ initialstate);	// Установка нулей в регистре данных

    PIOA->PIO_OER = opins; // эти выводы на вывод - похоже, при работе с периферийными устройствами не требуется.
	PIOA->PIO_PUER = opins;
	PIOA->PIO_MDER = opins;	// 5 volt (open drain) output

#elif CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOALPEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOA, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOA, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

//	//gpioX_poweron(GPIOA);
//	gpioX_setstate(GPIOA, opins, initialstate);
//	gpioX_prog(GPIOA, opins, ALWNR_GPIO_CFG_OPENDRAIN, ALWNR_GPIO_DRV_OPENDRAIN, ALWNR_GPIO_PULL_OPENDRAIN);

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}

void 
arm_hardware_piob_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOBLPEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOB, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOB, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOB);
	gpioX_setstate(GPIOB, opins, initialstate);
	gpioX_prog(GPIOB, opins, ALWNR_GPIO_CFG_OPENDRAIN, ALWNR_GPIO_DRV_OPENDRAIN, ALWNR_GPIO_PULL_OPENDRAIN);

#elif defined (GPIOB)
	#error Undefined CPUSTYLE_XXX


#endif
}

void 
arm_hardware_pioc_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOCLPEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOC, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOC, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOC);
	gpioX_setstate(GPIOC, opins, initialstate);
	gpioX_prog(GPIOC, opins, ALWNR_GPIO_CFG_OPENDRAIN, ALWNR_GPIO_DRV_OPENDRAIN, ALWNR_GPIO_PULL_OPENDRAIN);

#elif defined (GPIOC)
	#error Undefined CPUSTYLE_XXX

#endif
}

void 
arm_hardware_piod_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIODLPEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOD, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOD, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOD);
	gpioX_setstate(GPIOD, opins, initialstate);
	gpioX_prog(GPIOD, opins, ALWNR_GPIO_CFG_OPENDRAIN, ALWNR_GPIO_DRV_OPENDRAIN, ALWNR_GPIO_PULL_OPENDRAIN);

#elif defined (GPIOD)
	#error Undefined CPUSTYLE_XXX

#endif
}

void 
arm_hardware_pioe_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPEEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOELPEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOE, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOE, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOE);
	gpioX_setstate(GPIOE, opins, initialstate);
	gpioX_prog(GPIOE, opins, ALWNR_GPIO_CFG_OPENDRAIN, ALWNR_GPIO_DRV_OPENDRAIN, ALWNR_GPIO_PULL_OPENDRAIN);

#elif defined (GPIOE)
	#error Undefined CPUSTYLE_XXX

#endif
}

#if defined (GPIOF)

void 
arm_hardware_piof_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPFEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOFEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOFLPEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOF, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOF, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOF);
	gpioX_setstate(GPIOF, opins, initialstate);
	gpioX_prog(GPIOF, opins, ALWNR_GPIO_CFG_OPENDRAIN, ALWNR_GPIO_DRV_OPENDRAIN, ALWNR_GPIO_PULL_OPENDRAIN);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}
#endif /* defined (GPIOF) */

#if defined (GPIOG)
void 
arm_hardware_piog_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPGEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX)

	RCC->IOPENR |= RCC_IOPENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->IOPENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOGLPEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOG, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOG, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOG);
	gpioX_setstate(GPIOG, opins, initialstate);
	gpioX_prog(GPIOG, opins, ALWNR_GPIO_CFG_OPENDRAIN, ALWNR_GPIO_DRV_OPENDRAIN, ALWNR_GPIO_PULL_OPENDRAIN);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}
#endif /* defined (GPIOG) */

#if defined (GPIOH)
void 
arm_hardware_pioh_opendrain(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPHEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOH, opins, 1, 2);	/* CNF=2, MODE=2: Open drain, Max. output speed 2 MHz */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOHEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_GPIO, 0, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOHLPEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOH, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOH, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_A64

#else
	#error Undefined CPUSTYLE_XXX

#endif
}
#endif /* defined (GPIOH) */

// выводы присоединены к periph A (Atmel specific)
void arm_hardware_pioa_peripha(unsigned long pins)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PIOA->PIO_ASR = pins;                // assign pins to periph A
	AT91C_BASE_PIOA->PIO_PDR = pins;				// outputs are peripherial

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S
    // setup PIO pins for SPI bus
    //PIOA->PIO_ASR = WORKMASK;                 // assign pins to SPI interface
     // enable uart pins on PIO
	PIOA->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOA->PIO_ABCDSR [0] &= ~ (pins); // assigns the I/O lines to peripheral A function
	PIOA->PIO_ABCDSR [1] &= ~ (pins); // assigns the I/O lines to peripheral A function
	PIOA->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}
// выводы присоединены к periph B (Atmel specific)
void arm_hardware_pioa_periphb(unsigned long pins)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PIOA->PIO_BSR = pins;                // assign pins to periph B
	AT91C_BASE_PIOA->PIO_PDR = pins;				// outputs are peripherial

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOA->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOA->PIO_ABCDSR [0] |= (pins); // assigns the I/O lines to peripheral B function
	PIOA->PIO_ABCDSR [1] &= ~ (pins); // assigns the I/O lines to peripheral B function
	PIOA->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}

// выводы присоединены к periph A (Atmel specific)
void arm_hardware_piob_peripha(unsigned long pins)
{
#if CPUSTYLE_AT91SAM7S

	//AT91C_BASE_PIOB->PIO_ASR = pins;                // assign pins to periph A
	//AT91C_BASE_PIOB->PIO_PDR = pins;				// outputs are peripherial

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S
    // setup PIO pins for SPI bus
    //PIOA->PIO_ASR = WORKMASK;                 // assign pins to SPI interface
     // enable uart pins on PIO
	PIOB->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOB->PIO_ABCDSR [0] &= ~ (pins); // assigns the I/O lines to peripheral A function
	PIOB->PIO_ABCDSR [1] &= ~ (pins); // assigns the I/O lines to peripheral A function
	PIOB->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}
// выводы присоединены к periph B (Atmel specific)
void arm_hardware_piob_periphb(unsigned long pins)
{
#if CPUSTYLE_AT91SAM7S

	//AT91C_BASE_PIOA->PIO_BSR = pins;                // assign pins to periph B
	//AT91C_BASE_PIOA->PIO_PDR = pins;				// outputs are peripherial

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOB->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOB->PIO_ABCDSR [0] |= (pins); // assigns the I/O lines to peripheral B function
	PIOB->PIO_ABCDSR [1] &= ~ (pins); // assigns the I/O lines to peripheral B function
	PIOB->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

// выводы присоединены к periph C (Atmel specific)
void arm_hardware_pioa_periphc(unsigned long pins)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOA->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOA->PIO_ABCDSR [0] &= ~ (pins); // assigns the I/O lines to peripheral C function
	PIOA->PIO_ABCDSR [1] |= (pins); // assigns the I/O lines to peripheral C function
	PIOA->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}

// выводы присоединены к periph D (Atmel specific)
void arm_hardware_pioa_periphd(unsigned long pins)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOA->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOA->PIO_ABCDSR [0] |= (pins); // assigns the I/O lines to peripheral D function
	PIOA->PIO_ABCDSR [1] |= (pins); // assigns the I/O lines to peripheral D function
	PIOA->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}

// выводы присоединены к periph C (Atmel specific)
void arm_hardware_piob_periphc(unsigned long pins)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOB->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOB->PIO_ABCDSR [0] &= ~ (pins); // assigns the I/O lines to peripheral C function
	PIOB->PIO_ABCDSR [1] |= (pins); // assigns the I/O lines to peripheral C function
	PIOB->PIO_PDR = pins;	// enable peripherial, connected to this pin.

#endif
}

// выводы присоединены к periph D (Atmel specific)
void arm_hardware_piob_periphd(unsigned long pins)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOB->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.
	PIOB->PIO_ABCDSR [0] |= (pins); // assigns the I/O lines to peripheral D function
	PIOB->PIO_ABCDSR [1] |= (pins); // assigns the I/O lines to peripheral D function
	PIOB->PIO_PDR = pins;	// PIO Disable Register - enable peripherial on this pin

#endif
}


#endif /* CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S */

// выводы присоединены к PIO (Atmel specific)
void arm_hardware_pioa_only(unsigned long pins)		
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PIOA->PIO_PER = pins;				// initial disconnect - PIO enable

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOA->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.

#endif
}

// выводы присоединены к PIO (Atmel specific)
void arm_hardware_piob_only(unsigned long pins)		
{
#if CPUSTYLE_AT91SAM7S

	//AT91C_BASE_PIOB->PIO_PER = pins;				// initial disconnect - PIO enable

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOB->PIO_PER = pins;	// disable peripherial, connected to this pin. Use as PIO.

#endif
}

/* подключаем к периферии, 50 МГц, push-pull */
void 
arm_hardware_pioa_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHBENR;
	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->IOPENR;
	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOALPEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOA, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

//	//gpioX_poweron(GPIOA);
//	gpioX_prog(GPIOA, opins, af, ALWNR_GPIO_DRV_AF50M, ALWNR_GPIO_PULL_AF50M);

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 50 МГц, push-pull */
void 
arm_hardware_piob_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов speed: 00: 2 MHz Low speed, 01: 25 MHz Medium speed, 10: 50 MHz Fast speed, 11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHBENR;
	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов speed x0: 2 MHz Low speed, 01: 10 MHz Medium speed, 11: 50 MHz High speed
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->IOPENR;
	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOBLPEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOB, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOB);
	gpioX_prog(GPIOB, opins, af, ALWNR_GPIO_DRV_AF50M, ALWNR_GPIO_PULL_AF50M);

#elif defined (GPIOB)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 50 МГц, push-pull */
void 
arm_hardware_pioc_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHBENR;
	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOCLPEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOC, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOC);
	gpioX_prog(GPIOC, opins, af, ALWNR_GPIO_DRV_AF50M, ALWNR_GPIO_PULL_AF50M);

#elif defined (GPIOC)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 20 МГц, push-pull */
void 
arm_hardware_piod_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIODLPEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOD, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOD);
	gpioX_prog(GPIOD, opins, af, ALWNR_GPIO_DRV_AF20M, ALWNR_GPIO_PULL_AF20M);

#elif defined (GPIOD)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 50 МГц, push-pull */
void 
arm_hardware_piod_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 2, 3);	/* Установить CNF=2, MODE=3 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIODLPEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOD, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOD);
	gpioX_prog(GPIOD, opins, af, ALWNR_GPIO_DRV_AF50M, ALWNR_GPIO_PULL_AF50M);

#elif defined (GPIOD)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 2 МГц, push-pull */
void 
arm_hardware_pioa_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 2, 2);	/* Установить CNF=2, MODE=2 (2MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->IOPENR;
	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOALPEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOA, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

//	//gpioX_poweron(GPIOA);
//	gpioX_prog(GPIOA, opins, af, ALWNR_GPIO_DRV_AF2M, ALWNR_GPIO_PULL_AF2M);

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 20 МГц, push-pull */
void 
arm_hardware_pioa_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->IOPENR;
	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOALPEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOA, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

//	//gpioX_poweron(GPIOA);
//	gpioX_prog(GPIOA, opins, af, ALWNR_GPIO_DRV_AF20M, ALWNR_GPIO_PULL_AF20M);

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 2 МГц, push-pull */
void 
arm_hardware_piob_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 2, 2);	/* Установить CNF=2, MODE=2 (2MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->IOPENR;
	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOBLPEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOB, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOB);
	gpioX_prog(GPIOB, opins, af, ALWNR_GPIO_DRV_AF2M, ALWNR_GPIO_PULL_AF2M);

#elif defined (GPIOB)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 20 МГц, push-pull */
void 
arm_hardware_piob_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->IOPENR;
	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOBLPEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOB, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOB);
	gpioX_prog(GPIOB, opins, af, ALWNR_GPIO_DRV_AF20M, ALWNR_GPIO_PULL_AF20M);

#elif defined (GPIOB)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 2 МГц, push-pull */
void 
arm_hardware_pioc_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 2, 2);	/* Установить CNF=2, MODE=2 (2MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->IOPENR;
	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOCLPEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOC, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOC);
	gpioX_prog(GPIOC, opins, af, ALWNR_GPIO_DRV_AF2M, ALWNR_GPIO_PULL_AF2M);

#elif defined (GPIOC)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 20 МГц, push-pull */
void 
arm_hardware_pioc_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->IOPENR;
	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOCLPEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOC, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOC);
	gpioX_prog(GPIOC, opins, af, ALWNR_GPIO_DRV_AF20M, ALWNR_GPIO_PULL_AF20M);

#elif defined (GPIOC)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 2 МГц, push-pull */
void 
arm_hardware_piod_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 2, 2);	/* Установить CNF=2, MODE=2 (2MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIODLPEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOD, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOD);
	gpioX_prog(GPIOD, opins, af, ALWNR_GPIO_DRV_AF2M, ALWNR_GPIO_PULL_AF2M);

#elif defined (GPIOD)
	#error Undefined CPUSTYLE_XXX

#endif
}

#if defined (GPIOE)

/* подключаем к периферии, 2 МГц, push-pull */
void 
arm_hardware_pioe_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 2, 2);	/* Установить CNF=2, MODE=2 (2MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOELPEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOE, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOE);
	gpioX_prog(GPIOE, opins, af, ALWNR_GPIO_DRV_AF2M, ALWNR_GPIO_PULL_AF2M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 20 МГц, push-pull */
void 
arm_hardware_pioe_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOELPEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOE, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOE);
	gpioX_prog(GPIOE, opins, af, ALWNR_GPIO_DRV_AF20M, ALWNR_GPIO_PULL_AF20M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 50 МГц, push-pull */
void 
arm_hardware_pioe_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOELPEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOE, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOF);
	gpioX_prog(GPIOE, opins, af, ALWNR_GPIO_DRV_AF50M, ALWNR_GPIO_PULL_AF50M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined (GPIOE) */


#if defined (GPIOF)

/* подключаем к периферии, 2 МГц, push-pull */
void 
arm_hardware_piof_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 2, 2);	/* Установить CNF=2, MODE=2 (2MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOFLPEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOF, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOF);
	gpioX_prog(GPIOF, opins, af, ALWNR_GPIO_DRV_AF2M, ALWNR_GPIO_PULL_AF2M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 20 МГц, push-pull */
void 
arm_hardware_piof_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOFLPEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOF, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOF);
	gpioX_prog(GPIOF, opins, af, ALWNR_GPIO_DRV_AF20M, ALWNR_GPIO_PULL_AF20M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 50 МГц, push-pull */
void 
arm_hardware_piof_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOFLPEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOF, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOF);
	gpioX_prog(GPIOF, opins, af, ALWNR_GPIO_DRV_AF50M, ALWNR_GPIO_PULL_AF50M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined (GPIOF) */

#if defined (GPIOG)

/* подключаем к периферии, 2 МГц, push-pull */
void 
arm_hardware_piog_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 2, 2);	/* Установить CNF=2, MODE=2 (2MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOGLPEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOG, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOG);
	gpioX_prog(GPIOG, opins, af, ALWNR_GPIO_DRV_AF2M, ALWNR_GPIO_PULL_AF2M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 20 МГц, push-pull */
void 
arm_hardware_piog_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOGLPEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOG, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOG);
	gpioX_prog(GPIOG, opins, af, ALWNR_GPIO_DRV_AF20M, ALWNR_GPIO_PULL_AF20M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 50 МГц, push-pull */
void 
arm_hardware_piog_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOGLPEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOG, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOG);
	gpioX_prog(GPIOG, opins, af, ALWNR_GPIO_DRV_AF50M, ALWNR_GPIO_PULL_AF50M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined (GPIOG) */

#if defined (GPIOH)

void 
arm_hardware_pioh_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	(void) RCC->APB2ENR;
	GPIOE->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOH, ipins, 2, 0);	/* Установить CNF=2 и MODE=0 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOHLPEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOH, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_A64

	//gpioX_poweron(GPIOH);
	gpioX_prog(GPIOH, ipins, GPIO_CFG_IN, ALWNR_GPIO_DRV_INPUT, ALWNR_GPIO_PULL_INPUT);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 2 МГц, push-pull */
void 
arm_hardware_pioh_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOH, opins, 2, 2);	/* Установить CNF=2, MODE=2 (2MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOH, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOH, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOH, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOHLPEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOH, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOH, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_A64

	//gpioX_poweron(GPIOB);
	gpioX_prog(GPIOH, opins, af, ALWNR_GPIO_DRV_AF2M, ALWNR_GPIO_PULL_AF2M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 20 МГц, push-pull */
void 
arm_hardware_pioh_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOH, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOH, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOH, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOH, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOHLPEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOH, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOH, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_A64

	//gpioX_poweron(GPIOB);
	gpioX_prog(GPIOH, opins, af, ALWNR_GPIO_DRV_AF20M, ALWNR_GPIO_PULL_AF20M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 50 МГц, push-pull */
void 
arm_hardware_pioh_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOH, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOH, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOH, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOH, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOH, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOHEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOHLPEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOH, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOH, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_A64

	//gpioX_poweron(GPIOH);
	gpioX_prog(GPIOH, opins, af, ALWNR_GPIO_DRV_AF50M, ALWNR_GPIO_PULL_AF50M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined (GPIOH) */

#if defined (GPIOI)

void 
arm_hardware_pioi_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPIEN;	/* I/O port I clock enable */
	(void) RCC->APB2ENR;
	GPIOE->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOI, ipins, 2, 0);	/* Установить CNF=2 и MODE=0 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOILPEN;	/* I/O port I clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOI, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 2 МГц, push-pull */
void 
arm_hardware_pioi_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPIEN;	/* I/O port I clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOI, opins, 2, 2);	/* Установить CNF=2, MODE=2 (2MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOI, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOI, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOI, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOILPEN;	/* I/O port I clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOI, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOI, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 20 МГц, push-pull */
void 
arm_hardware_pioi_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPIEN;	/* I/O port I clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOI, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOI, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOI, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOI, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOILPEN;	/* I/O port I clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOI, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOI, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 50 МГц, push-pull */
void 
arm_hardware_pioi_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPIEN;	/* I/O port I clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOI, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOI, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOI, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOI, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOI, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOIEN;	/* I/O port I clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOILPEN;	/* I/O port I clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOI, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOI, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined (GPIOI) */

#if defined (GPIOJ)

void 
arm_hardware_pioj_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPJEN;	/* I/O port J clock enable */
	(void) RCC->APB2ENR;
	GPIOE->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOJ, ipins, 2, 0);	/* Установить CNF=2 и MODE=0 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOJ, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOJ, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOJ, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->MP_AHB4ENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOJ, ipins, STM32MP1_GPIO_MODE_INPIUT, 1, 1, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 2 МГц, push-pull */
void 
arm_hardware_pioj_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPJEN;	/* I/O port J clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOJ, opins, 2, 2);	/* Установить CNF=2, MODE=2 (2MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOJ, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOJ, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOJ, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->MP_AHB4ENSETR;

	stm32mp1_pioX_altfn(GPIOJ, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOJ, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 20 МГц, push-pull */
void 
arm_hardware_pioj_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPJEN;	/* I/O port J clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOJ, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOJ, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOJ, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOJ, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->MP_AHB4ENSETR;

	stm32mp1_pioX_altfn(GPIOJ, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOJ, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 50 МГц, push-pull */
void 
arm_hardware_pioj_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPJEN;	/* I/O port J clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOJ, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOJ, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOJ, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOJ, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOJ, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOJEN;	/* I/O port J clock enable */
	(void) RCC->MP_AHB4ENSETR;

	stm32mp1_pioX_altfn(GPIOJ, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOJ, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined (GPIOJ) */

#if defined (GPIOK)

void 
arm_hardware_piok_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	(void) RCC->APB2ENR;
	GPIOE->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOK, ipins, 2, 0);	/* Установить CNF=2 и MODE=0 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOKLPEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOK, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 2 МГц, push-pull */
void 
arm_hardware_piok_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOK, opins, 2, 2);	/* Установить CNF=2, MODE=2 (2MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOK, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOK, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOK, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOKLPEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOK, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOK, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 20 МГц, push-pull */
void 
arm_hardware_piok_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOK, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOK, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOK, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOK, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOKLPEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOK, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOK, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 50 МГц, push-pull */
void 
arm_hardware_piok_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOK, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOK, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOK, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOK, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOK, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOKEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOKLPEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOK, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOK, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined (GPIOK) */

#if defined (GPIOL)

/* Установка режима - вывод, с ограничением скорости (на STM32) 10 МГц	*/
void
arm_hardware_piol_outputs(unsigned long opins, unsigned long initialstate)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPHEN)

	RCC->APB2ENR |= RCC_APB2ENR_IOPHEN;	/* I/O port H clock enable */
	(void) RCC->APB2ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOL, opins, initialstate);
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOL, opins, 0, 1);	/* Установить CNF=0 и MODE=1 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOLEN;	/* I/O port H clock enable */
	(void) RCC->AHB1ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOL, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOLEN;	/* I/O port H clock enable */
	(void) RCC->AHB4ENR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOL, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX) && defined (RCC_AHBENR_GPIOLEN)

	RCC->AHBENR |= RCC_AHBENR_GPIOLEN;	/* I/O port H clock enable */
	(void) RCC->AHBENR;
	// Установка начального состояния битов
	//GPIOG->BSRR = (GPIO_BSRR_BS_0 * (initialstate & opins)) | (GPIO_BSRR_BR_0 * (~ initialstate & opins));
	arm_stm32f4xx_hardware_pio_setstate(GPIOL, opins, initialstate);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, opins, STM32F_GPIO_MODE_GPIO, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOLEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOLLPEN;	/* I/O port H clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка начального состояния битов
	arm_stm32f4xx_hardware_pio_setstate(GPIOL, opins, initialstate);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOL, opins, STM32MP1_GPIO_MODE_GPIO, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOL);
	gpioX_setstate(GPIOL, opins, initialstate);
	gpioX_prog(GPIOL, opins, GPIO_CFG_OUT, ALWNR_GPIO_DRV_OUTPUT20M, ALWNR_GPIO_PULL_OUTPUT20M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

void
arm_hardware_piol_inputs(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	(void) RCC->APB2ENR;
	GPIOE->BSRR = BSRR_S(ipins);	/* Для включения pull-up при CNFy[1:0] = 10 */	\
	arm_stm32f10x_hardware_pio_prog(GPIOL, ipins, 2, 0);	/* Установить CNF=2 и MODE=0 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->AHB1ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->AHB4ENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->AHBENR;
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOLLPEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4LPENSETR;
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOL, ipins, 0, 1, 1, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_A64

	//gpioX_poweron(GPIOL);
	gpioX_prog(GPIOL, ipins, GPIO_CFG_IN, ALWNR_GPIO_DRV_INPUT, ALWNR_GPIO_PULL_INPUT);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 2 МГц, push-pull */
void
arm_hardware_piol_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOL, opins, 2, 2);	/* Установить CNF=2, MODE=2 (2MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOL, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOL, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOL, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOLLPEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOL, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOL, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_A64)

	//gpioX_poweron(GPIOL);
	gpioX_prog(GPIOL, opins, af, ALWNR_GPIO_DRV_AF2M, ALWNR_GPIO_PULL_AF2M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 20 МГц, push-pull */
void
arm_hardware_piol_altfn20(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOL, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOL, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOL, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOL, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOLLPEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOL, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOL, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_20M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_A64)

	//gpioX_poweron(GPIOL);
	gpioX_prog(GPIOL, opins, af, ALWNR_GPIO_DRV_AF20M, ALWNR_GPIO_PULL_AF20M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 50 МГц, push-pull */
void
arm_hardware_piol_altfn50(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPKEN;	/* I/O port K clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOL, opins, 2, 3);	/* Установить CNF=2, MODE=3 (20 MHz) для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOL, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOL, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOL, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOL, opins, STM32F_GPIO_MODE_ALT, STM32F_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOLEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOLLPEN;	/* I/O port K clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOL, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOL, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_50M, 0, 0);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_A64)

	//gpioX_poweron(GPIOL);
	gpioX_prog(GPIOL, opins, af, ALWNR_GPIO_DRV_AF50M, ALWNR_GPIO_PULL_AF50M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined (GPIOL) */


/* подключаем к периферии, 2 МГц, open-drain */
void arm_hardware_pioa_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;	/* I/O port A clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOA, opins, 3, 2);	/* Установить CNF=3, MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->IOPENR;
	stm32f30x_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOA, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOAEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOALPEN;	/* I/O port A clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOA, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOA, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 1, STM32MP1_GPIO_OT_OD);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

//	//gpioX_poweron(GPIOA);
//	gpioX_prog(GPIOA, opins, af, ALWNR_GPIO_DRV_OPENDRAINAF2M, ALWNR_GPIO_PULL_OPENDRAINAF2M);

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}
/* подключаем к периферии, 2 МГц, open-drain */
void arm_hardware_piob_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN;	/* I/O port B clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOB, opins, 3, 2);	/* Установить CNF=3, MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->IOPENR;
	stm32f30x_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOB, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOBEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOBLPEN;	/* I/O port B clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOB, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOB, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 1, STM32MP1_GPIO_OT_OD);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOB);
	gpioX_prog(GPIOB, opins, af, ALWNR_GPIO_DRV_OPENDRAINAF2M, ALWNR_GPIO_PULL_OPENDRAINAF2M);

#elif defined (GPIOB)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 2 МГц, open-drain */
void arm_hardware_pioc_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPCEN;	/* I/O port C clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOC, opins, 3, 2);	/* Установить CNF=3, MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->IOPENR;
	stm32f30x_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOC, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOCEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOCLPEN;	/* I/O port C clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOC, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOC, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 1, STM32MP1_GPIO_OT_OD);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOC);
	gpioX_prog(GPIOC, opins, af, ALWNR_GPIO_DRV_OPENDRAINAF2M, ALWNR_GPIO_PULL_OPENDRAINAF2M);

#elif defined (GPIOC)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 2 МГц, open-drain */
void arm_hardware_piod_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPDEN;	/* I/O port D clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOD, opins, 3, 2);	/* Установить CNF=3, MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32L0XX

	RCC->IOPENR |= RCC_IOPENR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->IOPENR;
	stm32f30x_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOD, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIODEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIODLPEN;	/* I/O port D clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOD, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOD, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 1, STM32MP1_GPIO_OT_OD);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOD);
	gpioX_prog(GPIOD, opins, af, ALWNR_GPIO_DRV_OPENDRAINAF2M, ALWNR_GPIO_PULL_OPENDRAINAF2M);

#elif defined (GPIOD)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* подключаем к периферии, 2 МГц, open-drain */
void arm_hardware_pioe_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPEEN;	/* I/O port E clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOE, opins, 3, 2);	/* Установить CNF=3, MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X

	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOE, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOEEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOELPEN;	/* I/O port E clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOE, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOE, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 1, STM32MP1_GPIO_OT_OD);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOE);
	gpioX_prog(GPIOE, opins, af, ALWNR_GPIO_DRV_OPENDRAINAF2M, ALWNR_GPIO_PULL_OPENDRAINAF2M);

#elif defined (GPIOE)
	#error Undefined CPUSTYLE_XXX

#endif
}

#if defined (GPIOF)

/* подключаем к периферии, 2 МГц, open-drain */
void arm_hardware_piof_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX && defined(RCC_APB2ENR_IOPFEN)

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPFEN;	/* I/O port F clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOF, opins, 3, 2);	/* Установить CNF=3, MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->AHBENR;

	stm32f30x_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOF, opins, 0, STM32F_GPIO_SPEED_2M, 0, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOFEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOFLPEN;	/* I/O port F clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOF, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOF, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 1, STM32MP1_GPIO_OT_OD);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOG);
	gpioX_prog(GPIOG, opins, af, ALWNR_GPIO_DRV_OPENDRAINAF2M, ALWNR_GPIO_PULL_OPENDRAINAF2M);

#else
	#error Undefined CPUSTYLE_XXX

#endif
}
#endif /* defined (GPIOF) */

#if defined (GPIOG)
/* подключаем к периферии, 2 МГц, open-drain */
void arm_hardware_piog_periphopendrain_altfn2(unsigned long opins, unsigned af)
{
#if CPUSTYLE_STM32F1XX && defined (RCC_APB2ENR_IOPGEN)

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPGEN;	/* I/O port G clock enable */
	(void) RCC->APB2ENR;
	// Установка режима выводов
	arm_stm32f10x_hardware_pio_prog(GPIOG, opins, 3, 2);	/* Установить CNF=3, MODE=2 для указанных битов */

#elif (CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX)

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB1ENR;
	stm32f30x_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_STM32H7XX)

	RCC->AHB4ENR |= RCC_AHB4ENR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->AHB4ENR;
	stm32f30x_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32f30x_pioX_prog(GPIOG, opins, 2, STM32F_GPIO_SPEED_2M, 1, 1);	/* mode, speed, pupdr, typer */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	// На этих процессорах порта GPIOG нет


#elif CPUSTYLE_STM32MP1

	RCC->MP_AHB4ENSETR = RCC_MP_AHB4ENSETR_GPIOGEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4ENSETR;
	RCC->MP_AHB4LPENSETR = RCC_MP_AHB4LPENSETR_GPIOGLPEN;	/* I/O port G clock enable */
	(void) RCC->MP_AHB4LPENSETR;

	stm32mp1_pioX_altfn(GPIOG, opins, af);
	// Установка режима выводов
	stm32mp1_pioX_prog(GPIOG, opins, STM32MP1_GPIO_MODE_ALT, STM32MP1_GPIO_SPEED_2M, 1, STM32MP1_GPIO_OT_OD);	/* mode, speed, pupdr, typer */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	//gpioX_poweron(GPIOG);
	gpioX_prog(GPIOG, opins, af, ALWNR_GPIO_DRV_OPENDRAINAF2M, ALWNR_GPIO_PULL_OPENDRAINAF2M);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOG) */

/* включение подтягивающих резисторов к питанию (up) или к земле (down). */
void 
arm_hardware_pioa_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	stm32f10x_pioX_pupdr(GPIOA, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	tm32f30x_pioX_pupdr(GPIOA, up, down);

#elif CPUSTYLE_STM32MP1

	tm32mp1_pioX_pupdr(GPIOA, up, down);

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

//	gpioX_updown(GPIOA, up, down);

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* включение подтягивающих резисторов к питанию (up) или к земле (down). */
void 
arm_hardware_piob_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	stm32f10x_pioX_pupdr(GPIOB, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	tm32f30x_pioX_pupdr(GPIOB, up, down);

#elif CPUSTYLE_STM32MP1

	tm32mp1_pioX_pupdr(GPIOB, up, down);

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_updown(GPIOB, up, down);

#elif defined (GPIOB)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* включение подтягивающих резисторов к питанию (up) или к земле (down). */
void 
arm_hardware_pioc_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	stm32f10x_pioX_pupdr(GPIOC, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	tm32f30x_pioX_pupdr(GPIOC, up, down);

#elif CPUSTYLE_STM32MP1

	tm32mp1_pioX_pupdr(GPIOC, up, down);

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_updown(GPIOC, up, down);

#elif defined (GPIOC)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* включение подтягивающих резисторов к питанию (up) или к земле (down). */
void 
arm_hardware_piod_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	stm32f10x_pioX_pupdr(GPIOD, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	tm32f30x_pioX_pupdr(GPIOD, up, down);

#elif CPUSTYLE_STM32MP1

	tm32mp1_pioX_pupdr(GPIOD, up, down);

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_updown(GPIOD, up, down);

#elif defined (GPIOD)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* включение подтягивающих резисторов к питанию (up) или к земле (down). */
void 
arm_hardware_pioe_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	stm32f10x_pioX_pupdr(GPIOE, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	tm32f30x_pioX_pupdr(GPIOE, up, down);

#elif CPUSTYLE_STM32MP1

	tm32mp1_pioX_pupdr(GPIOE, up, down);

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_updown(GPIOE, up, down);

#elif defined (GPIOE)
	#error Undefined CPUSTYLE_XXX

#endif
}

#if defined (GPIOF)

/* включение подтягивающих резисторов к питанию (up) или к земле (down). */
void 
arm_hardware_piof_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	stm32f10x_pioX_pupdr(GPIOF, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	tm32f30x_pioX_pupdr(GPIOF, up, down);

#elif CPUSTYLE_STM32MP1

	tm32mp1_pioX_pupdr(GPIOF, up, down);

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_updown(GPIOF, up, down);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOF) */

#if defined (GPIOG)

/* включение подтягивающих резисторов к питанию (up) или к земле (down). */
void 
arm_hardware_piog_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	stm32f10x_pioX_pupdr(GPIOG, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	tm32f30x_pioX_pupdr(GPIOG, up, down);

#elif CPUSTYLE_STM32MP1

	tm32mp1_pioX_pupdr(GPIOG, up, down);

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_updown(GPIOG, up, down);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOG) */

#if defined (GPIOH)

/* включение подтягивающих резисторов к питанию (up) или к земле (down). */
void 
arm_hardware_pioh_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	stm32f10x_pioX_pupdr(GPIOH, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	tm32f30x_pioX_pupdr(GPIOH, up, down);

#elif CPUSTYLE_STM32MP1

	tm32mp1_pioX_pupdr(GPIOH, up, down);

#elif CPUSTYLE_A64

	gpioX_updown(GPIOH, up, down);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOH) */

#if defined (GPIOI)

/* включение подтягивающих резисторов к питанию (up) или к земле (down). */
void 
arm_hardware_pioi_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	stm32f10x_pioX_pupdr(GPIOI, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	tm32f30x_pioX_pupdr(GPIOI, up, down);

#elif CPUSTYLE_STM32MP1

	tm32mp1_pioX_pupdr(GPIOI, up, down);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOI) */

#if defined (GPIOJ)

/* включение подтягивающих резисторов к питанию (up) или к земле (down). */
void 
arm_hardware_pioj_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	stm32f10x_pioX_pupdr(GPIOJ, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	tm32f30x_pioX_pupdr(GPIOJ, up, down);

#elif CPUSTYLE_STM32MP1

	tm32mp1_pioX_pupdr(GPIOJ, up, down);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOJ) */

#if defined (GPIOK)

/* включение подтягивающих резисторов к питанию (up) или к земле (down). */
void 
arm_hardware_piok_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	stm32f10x_pioX_pupdr(GPIOK, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	tm32f30x_pioX_pupdr(GPIOK, up, down);

#elif CPUSTYLE_STM32MP1

	tm32mp1_pioX_pupdr(GPIOK, up, down);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOK) */

#if defined (GPIOZ)

/* включение подтягивающих резисторов к питанию (up) или к земле (down). */
void
arm_hardware_pioz_updown(unsigned long up, unsigned long down)
{
#if CPUSTYLE_STM32F1XX
	stm32f10x_pioX_pupdr(GPIOZ, up, down);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	tm32f30x_pioX_pupdr(GPIOZ, up, down);

#elif CPUSTYLE_STM32MP1

	tm32mp1_pioX_pupdr(GPIOZ, up, down);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOZ) */

/* отключение подтягивающих резисторов. */
void 
arm_hardware_pioa_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOA, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOA, ipins);

#elif CPUSTYLE_STM32MP1

	arm_stm32mp1_hardware_pio_pupoff(GPIOA, ipins);

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

//	gpioX_updownoff(GPIOA, ipins);

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* отключение подтягивающих резисторов. */
void 
arm_hardware_piob_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOB, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOB, ipins);

#elif CPUSTYLE_STM32MP1

	arm_stm32mp1_hardware_pio_pupoff(GPIOB, ipins);

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_updownoff(GPIOB, ipins);

#elif defined (GPIOB)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* отключение подтягивающих резисторов. */
void 
arm_hardware_pioc_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOC, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOC, ipins);

#elif CPUSTYLE_STM32MP1

	arm_stm32mp1_hardware_pio_pupoff(GPIOC, ipins);

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_updownoff(GPIOC, ipins);

#elif defined (GPIOC)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* отключение подтягивающих резисторов. */
void 
arm_hardware_piod_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOD, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOD, ipins);

#elif CPUSTYLE_STM32MP1

	arm_stm32mp1_hardware_pio_pupoff(GPIOD, ipins);

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_updownoff(GPIOD, ipins);

#elif defined (GPIOD)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* отключение подтягивающих резисторов. */
void 
arm_hardware_pioe_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOE, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOE, ipins);

#elif CPUSTYLE_STM32MP1

	arm_stm32mp1_hardware_pio_pupoff(GPIOE, ipins);

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_updownoff(GPIOE, ipins);

#elif defined (GPIOE)
	#error Undefined CPUSTYLE_XXX

#endif
}

#if defined (GPIOF)

/* отключение подтягивающих резисторов. */
void 
arm_hardware_piof_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOF, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOF, ipins);

#elif CPUSTYLE_STM32MP1

	arm_stm32mp1_hardware_pio_pupoff(GPIOF, ipins);

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_updownoff(GPIOF, ipins);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOF) */

#if defined (GPIOG)

/* отключение подтягивающих резисторов. */
void 
arm_hardware_piog_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOG, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOG, ipins);

#elif CPUSTYLE_STM32MP1

	arm_stm32mp1_hardware_pio_pupoff(GPIOG, ipins);

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_updownoff(GPIOG, ipins);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOG) */


#if defined (GPIOH)

/* отключение подтягивающих резисторов. */
void 
arm_hardware_pioh_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOH, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOH, ipins);

#elif CPUSTYLE_STM32MP1

	arm_stm32mp1_hardware_pio_pupoff(GPIOH, ipins);

#elif CPUSTYLE_A64

    gpioX_updownoff(GPIOH, ipins);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOH) */

#if defined (GPIOI)

/* отключение подтягивающих резисторов. */
void 
arm_hardware_pioi_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOI, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOI, ipins);

#elif CPUSTYLE_STM32MP1

	arm_stm32mp1_hardware_pio_pupoff(GPIOI, ipins);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOI) */

#if defined (GPIOJ)

/* отключение подтягивающих резисторов. */
void 
arm_hardware_pioj_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOJ, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOJ, ipins);

#elif CPUSTYLE_STM32MP1

	arm_stm32mp1_hardware_pio_pupoff(GPIOJ, ipins);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOJ) */

#if defined (GPIOK)

/* отключение подтягивающих резисторов. */
void 
arm_hardware_piok_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOK, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOK, ipins);

#elif CPUSTYLE_STM32MP1

	arm_stm32mp1_hardware_pio_pupoff(GPIOK, ipins);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOK) */

#if defined (GPIOZ)

/* отключение подтягивающих резисторов. */
void
arm_hardware_pioz_updownoff(unsigned long ipins)
{
#if CPUSTYLE_STM32F1XX
	arm_stm32f10x_hardware_pio_pupoff(GPIOZ, ipins);
#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	arm_stm32f30x_hardware_pio_pupoff(GPIOZ, ipins);

#elif CPUSTYLE_STM32MP1

	arm_stm32mp1_hardware_pio_pupoff(GPIOZ, ipins);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* defined (GPIOZ) */


/* разрешить прерывание по изменению состояния указанных битов порта */
// эти функции не меняют программирование выводов (на ввод или на вывод),
// только подключают прерывания. Требуется иногда прерывания по переходу выводов присоединённых к периферии.
void 
arm_hardware_pioa_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu)
{
#if CPUSTYLE_AT91SAM7S

	AT91C_BASE_PIOA->PIO_IFER = (ipins);	// glitch filter enable
	(void) AT91C_BASE_PIOA->PIO_ISR; // consume interrupt request
	AT91C_BASE_PIOA->PIO_IER = (ipins);	// interrupt on change pin enable

	// programming interrupts from PIOA
	AT91C_BASE_AIC->AIC_IDCR = (1uL << AT91C_ID_PIOA);
	AT91C_BASE_AIC->AIC_SVR [AT91C_ID_PIOA] = (AT91_REG) AT91F_PIOA_IRQHandler;
	AT91C_BASE_AIC->AIC_SMR [AT91C_ID_PIOA] = 
		(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL) |
		(AT91C_AIC_PRIOR & priority);
	AT91C_BASE_AIC->AIC_ICCR = (1uL << AT91C_ID_PIOA);		// clear pending interrupt
	AT91C_BASE_AIC->AIC_IECR = (1uL << AT91C_ID_PIOA);	// enable inerrupt

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOA->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOA->PIO_ISR; // consume interrupt request
	PIOA->PIO_IER = (ipins);	// interrupt on change pin enable

	arm_hardware_set_handler(PIOA_IRQn, PIOA_IRQHandler, priority, tgcpu);

#elif CPUSTYLE_STM32F1XX

	stm32f10x_pioX_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PA, priority);	// PORT A

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	stm32f30x_pioX_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PA, priority);	// PORT A

#elif CPUSTYLE_STM32MP1

	stm32mp1_pioX_onchangeinterrupt(ipins, raise, fall, EXTI_EXTICR1_EXTI0_PA, priority, tgcpu);	// PORT A

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

//	gpioX_onchangeinterrupt(GPIOA, ipins, raise, fall, priority, tgcpu, ALLW_GPIO_IRQ_Handler);	// PORT A

#elif defined (GPIOA)
	#error Undefined CPUSTYLE_XXX

#endif
}

/* разрешить прерывание по изменению состояния указанных битов порта */
// эти функции не меняют программирование выводов (на ввод или на вывод),
// только подключают прерывания. Требуется иногда прерывания по переходу выводов присоединённых к периферии.
void 
arm_hardware_piob_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOB->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOB->PIO_ISR; // consume interrupt request
	PIOB->PIO_IER = (ipins);	// interrupt on change pin enable

	arm_hardware_set_handler(PIOB_IRQn, PIOB_IRQHandler, priority, tgcpu);

#elif CPUSTYLE_STM32F1XX

	stm32f10x_pioX_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PB, priority);	// PORT B

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	stm32f30x_pioX_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PB, priority);	// PORT B

#elif CPUSTYLE_STM32MP1

	stm32mp1_pioX_onchangeinterrupt(ipins, raise, fall, EXTI_EXTICR1_EXTI0_PB, priority, tgcpu);	// PORT B

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_onchangeinterrupt(GPIOB, ipins, raise, fall, priority, tgcpu, ALLW_GPIO_IRQ_Handler);	// PORT B

#elif defined (GPIOB)
	#error Undefined CPUSTYLE_XXX

#endif
}


/* разрешить прерывание по изменению состояния указанных битов порта */
// эти функции не меняют программирование выводов (на ввод или на вывод),
// только подключают прерывания. Требуется иногда прерывания по переходу выводов присоединённых к периферии.
void 
arm_hardware_pioc_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOC->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOC->PIO_ISR; // consume interrupt request
	PIOC->PIO_IER = (ipins);	// interrupt on change pin enable

	arm_hardware_set_handler(PIOC_IRQn, PIOC_IRQHandler, priority, tgcpu);

#elif CPUSTYLE_STM32F1XX

	stm32f10x_pioX_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PC, priority);	// PORT C

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	stm32f30x_pioX_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PC, priority);	// PORT C

#elif CPUSTYLE_STM32MP1

	stm32mp1_pioX_onchangeinterrupt(ipins, raise, fall, EXTI_EXTICR1_EXTI0_PC, priority, tgcpu);	// PORT C

#elif CPUSTYLE_AT91SAM7S

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_onchangeinterrupt(GPIOC, ipins, raise, fall, priority, tgcpu, ALLW_GPIO_IRQ_Handler);	// PORT C

#elif defined (GPIOC)
	#error Undefined CPUSTYLE_XXX

#endif
}

#if defined (GPIOD) || defined (PIOD)

// эти функции не меняют программирование выводов (на ввод или на вывод),
// только подключают прерывания. Требуется иногда прерывания по переходу выводов присоединённых к периферии.
void 
arm_hardware_piod_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOD->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOD->PIO_ISR; // consume interrupt request
	PIOD->PIO_IER = (ipins);	// interrupt on change pin enable

	arm_hardware_set_handler(PIOD_IRQn, PIOD_IRQHandler, priority, tgcpu);

#elif CPUSTYLE_STM32F1XX

	stm32f10x_pioX_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PD, priority);	// PORT D

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	stm32f30x_pioX_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PD, priority);	// PORT D

#elif CPUSTYLE_STM32MP1

	stm32mp1_pioX_onchangeinterrupt(ipins, raise, fall, EXTI_EXTICR1_EXTI0_PD, priority, tgcpu);	// PORTD

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_onchangeinterrupt(GPIOD, ipins, raise, fall, priority, tgcpu, ALLW_GPIO_IRQ_Handler);	// PORT D

#else
	#error Undefined CPUSTYLE_XXX

#endif
}
#endif /* defined (GPIOD) || defined (PIOD) */

#if defined (GPIOE) || defined (PIOE)

// эти функции не меняют программирование выводов (на ввод или на вывод),
// только подключают прерывания. Требуется иногда прерывания по переходу выводов присоединённых к периферии.
void 
arm_hardware_pioe_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOE->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOE->PIO_ISR; // consume interrupt request
	PIOE->PIO_IER = (ipins);	// interrupt on change pin enable

	arm_hardware_set_handler(PIOE_IRQn, PIOE_IRQHandler, priority, tgcpu);

#elif CPUSTYLE_STM32F1XX

	stm32f10x_pioX_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PE, priority);	// PORT E

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	stm32f30x_pioX_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PE, priority);	// PORT E

#elif CPUSTYLE_STM32MP1

	stm32mp1_pioX_onchangeinterrupt(ipins, raise, fall, EXTI_EXTICR1_EXTI0_PE, priority, tgcpu);	// PORT E

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_onchangeinterrupt(GPIOE, ipins, raise, fall, priority, tgcpu, ALLW_GPIO_IRQ_Handler);	// PORT E

#else
	#error Undefined CPUSTYLE_XXX

#endif
}
#endif /* defined (GPIOE) || defined (PIOE) */

#if defined (GPIOF) || defined (PIOF)

// эти функции не меняют программирование выводов (на ввод или на вывод),
// только подключают прерывания. Требуется иногда прерывания по переходу выводов присоединённых к периферии.
void 
arm_hardware_piof_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOF->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOF->PIO_ISR; // consume interrupt request
	PIOF->PIO_IER = (ipins);	// interrupt on change pin enable

	arm_hardware_set_handler(PIOF_IRQn, PIOF_IRQHandler, priority, tgcpu);

#elif CPUSTYLE_STM32F1XX

	stm32f10x_pioX_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PF, priority, tgcpu);	// PORT F

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	stm32f30x_pioX_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PF, priority);	// PORT F

#elif CPUSTYLE_STM32MP1

	stm32mp1_pioX_onchangeinterrupt(ipins, raise, fall, EXTI_EXTICR1_EXTI0_PF, priority, tgcpu);	// PORT F

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_onchangeinterrupt(GPIOF, ipins, raise, fall, priority, tgcpu, ALLW_GPIO_IRQ_Handler);	// PORT F

#else
	#error Undefined CPUSTYLE_XXX

#endif
}
#endif /* defined (GPIOF) || defined (PIOF) */

#if defined (GPIOG)
// эти функции не меняют программирование выводов (на ввод или на вывод),
// только подключают прерывания. Требуется иногда прерывания по переходу выводов присоединённых к периферии.
void 
arm_hardware_piog_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PIOG->PIO_IFER = (ipins);	// glitch filter enable
	(void) PIOG->PIO_ISR; // consume interrupt request
	PIOG->PIO_IER = (ipins);	// interrupt on change pin enable

	arm_hardware_set_handler(PIOG_IRQn, PIOG_IRQHandler, priority, tgcpu);

#elif CPUSTYLE_STM32F1XX

	stm32f10x_pioX_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PG, priority, tgcpu);	// PORT G

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	stm32f30x_pioX_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PG, priority);	// PORT G

#elif CPUSTYLE_STM32MP1

	stm32mp1_pioX_onchangeinterrupt(ipins, raise, fall, EXTI_EXTICR1_EXTI0_PG, priority, tgcpu);	// PORT G

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64)

	gpioX_onchangeinterrupt(GPIOG, ipins, raise, fall, priority, tgcpu, ALLW_GPIO_IRQ_Handler);	// PORT G

#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined (GPIOG) */

#if defined (GPIOH)
// эти функции не меняют программирование выводов (на ввод или на вывод),
// только подключают прерывания. Требуется иногда прерывания по переходу выводов присоединённых к периферии.
void 
arm_hardware_pioh_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu)
{
#if CPUSTYLE_STM32F1XX

	stm32f10x_pioX_onchangeinterrupt(ipins, raise, fall, AFIO_EXTICR1_EXTI0_PH, priority, tgcpu);	// PORT H

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	stm32f30x_pioX_onchangeinterrupt(ipins, raise, fall, SYSCFG_EXTICR1_EXTI0_PH, priority);	// PORT H

#elif CPUSTYLE_STM32MP1

	stm32mp1_pioX_onchangeinterrupt(ipins, raise, fall, EXTI_EXTICR1_EXTI0_PH, priority, tgcpu);	// PORT H

#elif CPUSTYLE_A64

	gpioX_onchangeinterrupt(GPIOH, ipins, raise, fall, priority, tgcpu, ALLW_GPIO_IRQ_Handler);	// PORT B

	#else
	#error Undefined CPUSTYLE_XXX

#endif
}

#endif /* defined (GPIOH) */

#elif CPUSTYLE_TMS320F2833X

/* Установка начального состояния битов  в GPIOA TMS320F2833X */
#define tms320f2833x_hardware_pioa_setstate(opins, initialstate) \
  do { \
	GPASET = ((initialstate) & (opins)); \
	GPACLEAR = (~ (initialstate) & (opins)); \
	} while (0)
/* Установка начального состояния битов  в GPIOB TMS320F2833X */
#define tms320f2833x_hardware_piob_setstate(opins, initialstate) \
  do { \
	GPBSET = ((initialstate) & (opins)); \
	GPBCLEAR = (~ (initialstate) & (opins)); \
	} while (0)
/* Установка начального состояния битов  в GPIOC TMS320F2833X */
#define tms320f2833x_hardware_pioc_setstate(opins, initialstate) \
  do { \
	GPCSET = ((initialstate) & (opins)); \
	GPCCLEAR = (~ (initialstate) & (opins)); \
	} while (0)
/* программирование битов в регистрах управления GPIOA, указанных в iomask, mux, direction, pullupdisable */
#define tms320f2833x_hardware_pioa_prog(iomask, mux, direction, pullupdisable, qual) \
  do { \
	const portholder_t mask3lo = power2(iomask >> 0);	\
	const portholder_t mask3hi = power2(iomask >> 16);	\
	GPAMUX1 = (GPAMUX1 & ~ (mask3lo * 0x03UL)) | (mask3lo * (mux)); /* GPIOxx as IO use */ \
	GPAMUX2 = (GPAMUX2 & ~ (mask3hi * 0x03UL)) | (mask3hi * (mux)); /* GPIOxx as IO use */ \
	GPAQSEL1 = (GPAQSEL1 & ~ (mask3lo * 0x03UL)) | (mask3lo * (qual)); /* GPIOxx as IO use */ \
	GPAQSEL2 = (GPAQSEL2 & ~ (mask3hi * 0x03UL)) | (mask3hi * (qual)); /* GPIOxx as IO use */ \
	GPADIR = (GPADIR | (iomask * (direction)));	/* set direction to out */ \
	GPAPUD = (GPAPUD | (iomask * (pullupdisable))); /* disable the pull up */ \
  } while (0)
/* программирование битов в регистрах управления GPIOB, указанных в iomask, mux, direction, pullupdisable */
#define tms320f2833x_hardware_piob_prog(iomask, mux, direction, pullupdisable, qual) \
  do { \
	const portholder_t mask3lo = power2(iomask >> 0);	\
	const portholder_t mask3hi = power2(iomask >> 16);	\
	GPBMUX1 = (GPBMUX1 & ~ (mask3lo * 0x03UL)) | (mask3lo * (mux)); /* GPIOxx as IO use */ \
	GPBMUX2 = (GPBMUX2 & ~ (mask3hi * 0x03UL)) | (mask3hi * (mux)); /* GPIOxx as IO use */ \
	GPBQSEL1 = (GPBQSEL1 & ~ (mask3lo * 0x03UL)) | (mask3lo * (qual)); /* GPIOxx as IO use */ \
	GPBQSEL2 = (GPBQSEL2 & ~ (mask3hi * 0x03UL)) | (mask3hi * (qual)); /* GPIOxx as IO use */ \
	GPBDIR = (GPBDIR | (iomask * (direction)));	/* set direction to out */ \
	GPBPUD = (GPBPUD | (iomask * (pullupdisable))); /* disable the pull up */ \
  } while (0)
/* программирование битов в регистрах управления GPIOC, указанных в iomask, direction, pullupdisable */
#define tms320f2833x_hardware_pioc_prog(iomask, mux, direction, pullupdisable) \
  do { \
	const portholder_t mask3lo = power2(iomask >> 0);	\
	const portholder_t mask3hi = power2(iomask >> 16);	\
	GPCMUX1 = (GPCMUX1 & ~ (mask3lo * 0x03UL)) | (mask3lo * (mux)); /* GPIOxx as IO use */ \
	GPCMUX2 = (GPCMUX2 & ~ (mask3hi * 0x03UL)) | (mask3hi * (mux)); /* GPIOxx as IO use */ \
	GPCDIR = (GPCDIR | (iomask * (direction)));	/* set direction to out */ \
	GPCPUD = (GPCPUD | (iomask * (pullupdisable))); /* disable the pull up */ \
  } while (0)


/* программирование выводов на вывод, без присоединения к периферии */
void
tms320_hardware_pioa_outputs(unsigned long opins, unsigned long initialstate)
{
	// Установка начального состояния битов
	tms320f2833x_hardware_pioa_setstate(opins, initialstate);
	// Установка режима выводов
	tms320f2833x_hardware_pioa_prog(opins, 0, 1, 1, 0);	/* На вывод, без pull-up */
}
/* программирование выводов на вывод, без присоединения к периферии */
void
tms320_hardware_piob_outputs(unsigned long opins, unsigned long initialstate)
{
	// Установка начального состояния битов
	tms320f2833x_hardware_piob_setstate(opins, initialstate);
	// Установка режима выводов
	tms320f2833x_hardware_piob_prog(opins, 0, 1, 1, 0);	/* На вывод, без pull-up */
}
/* программирование выводов на вывод, без присоединения к периферии */
void
tms320_hardware_pioc_outputs(unsigned long opins, unsigned long initialstate)
{
	// Установка начального состояния битов
	tms320f2833x_hardware_pioc_setstate(opins, initialstate);
	// Установка режима выводов
	tms320f2833x_hardware_pioc_prog(opins, 0, 1, 1);	/* На вывод, без pull-up */
}
/* программирование выводов на вывод, присоединение к периферии */
void
tms320_hardware_pioa_periph(unsigned long opins, uint_fast8_t mux)
{
	// Установка режима выводов
	tms320f2833x_hardware_pioa_prog(opins, mux, 0, 0, 0);
}

/* программирование выводов на вывод, присоединение к периферии */
void
tms320_hardware_piob_periph(unsigned long opins, uint_fast8_t mux)
{
	// Установка режима выводов
	tms320f2833x_hardware_piob_prog(opins, mux, 0, 0, 0);
}
/* программирование выводов на вывод, присоединение к периферии */
void
tms320_hardware_pioc_periph(unsigned long opins, uint_fast8_t mux)
{
	// Установка режима выводов
	tms320f2833x_hardware_pioc_prog(opins, mux, 0, 0);
}

/* программирование выводов на ввод, без присоединения к периферии */
void
tms320_hardware_pioa_inputs(unsigned long ipins)
{
	// Установка режима выводов
	tms320f2833x_hardware_pioa_prog(ipins, 0, 0, 0, 0);	/* На ввод, с pull-up */
}
/* программирование выводов на ввод, без присоединения к периферии */
void
tms320_hardware_piob_inputs(unsigned long ipins)
{
	// Установка режима выводов
	tms320f2833x_hardware_piob_prog(ipins, 0, 0, 0, 0);	/* На ввод, с pull-up */
}
/* программирование выводов на ввод, без присоединения к периферии */
void
tms320_hardware_pioс_inputs(unsigned long ipins)
{
	// Установка режима выводов
	tms320f2833x_hardware_pioc_prog(ipins, 0, 0, 0);	/* На ввод, с pull-up */
}

#endif /* CPUSTYLE_ARM || CPUSTYLE_RISCV */
