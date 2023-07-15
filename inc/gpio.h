// $Id$

#ifndef PIO_H_INCLUDED
#define PIO_H_INCLUDED

#include "hardware.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if CPUSTYLE_STM32MP1

	#define BSRR_S(v) ((v) * GPIO_BSRR_BS0)	/* Преобразование значения для установки бита в регистре */
	#define BSRR_C(v) ((v) * GPIO_BSRR_BR0)	/* Преобразование значения для сброса бита в регистре */

	#define STM32MP1_GPIO_MODE_INPIUT	0
	#define STM32MP1_GPIO_MODE_GPIO	1
	#define STM32MP1_GPIO_MODE_ALT	2
	#define STM32MP1_GPIO_MODE_ANALOG	3

	#define STM32MP1_GPIO_OT_PP	0	// Push-pool output type
	#define STM32MP1_GPIO_OT_OD	1	// Open Drain output type

	#define STM32MP1_GPIO_SPEED_50M	2
	#define STM32MP1_GPIO_SPEED_20M	1
	#define STM32MP1_GPIO_SPEED_2M	0

#elif CPUSTYLE_STM32F

	#define BSRR_S(v) ((v) * GPIO_BSRR_BS0)	/* Преобразование значения для установки бита в регистре */
	#define BSRR_C(v) ((v) * GPIO_BSRR_BR0)	/* Преобразование значения для сброса бита в регистре */

	// Параметры для arm_stm32f30x_hardware_pio_prog
	#if CPUSTYLE_STM32L0XX		// 0: very low speed, 1: low speed, 2: medium speed, 3: high speed
		#define GPIO_BSRR_BS0 GPIO_BSRR_BS_0
		#define GPIO_BSRR_BR0 GPIO_BSRR_BR_0
		#define STM32F_GPIO_MODE_ALT	2
		#define STM32F_GPIO_SPEED_50M	3
		#define STM32F_GPIO_SPEED_20M	2
		#define STM32F_GPIO_SPEED_2M	1
	#elif CPUSTYLE_STM32F0XX	// 0, 2: low speed, 1: medium speed, 3: high speed
		#define GPIO_BSRR_BS0 GPIO_BSRR_BS_0
		#define GPIO_BSRR_BR0 GPIO_BSRR_BR_0
		#define STM32F_GPIO_MODE_ALT	2
		#define STM32F_GPIO_SPEED_50M	3
		#define STM32F_GPIO_SPEED_20M	1
		#define STM32F_GPIO_SPEED_2M	0
	#elif CPUSTYLE_STM32F30X	// 0: low speed, 1: medium speed, 3: high speed
		#define GPIO_BSRR_BS0 GPIO_BSRR_BS_0
		#define GPIO_BSRR_BR0 GPIO_BSRR_BR_0
		#define STM32F_GPIO_MODE_GPIO	1
		#define STM32F_GPIO_MODE_ALT	2
		#define STM32F_GPIO_MODE_ANALOG	3
		#define STM32F_GPIO_SPEED_50M	3
		#define STM32F_GPIO_SPEED_20M	1
		#define STM32F_GPIO_SPEED_2M	0
	#elif CPUSTYLE_STM32F4XX	// 0: low speed, 1: medium speed, 2: fast speed, 3: high speed
		#define STM32F_GPIO_MODE_GPIO	1
		#define STM32F_GPIO_MODE_ALT	2
		#define STM32F_GPIO_MODE_ANALOG	3
		#define STM32F_GPIO_SPEED_50M	2
		#define STM32F_GPIO_SPEED_20M	1
		#define STM32F_GPIO_SPEED_2M	0
	#elif CPUSTYLE_STM32F7XX	// 0: low speed, 1: medium speed, 2: fast speed, 3: high speed
		#define GPIO_BSRR_BS0 GPIO_BSRR_BS_0
		#define GPIO_BSRR_BR0 GPIO_BSRR_BR_0
		#define STM32F_GPIO_MODE_GPIO	1
		#define STM32F_GPIO_MODE_ALT	2
		#define STM32F_GPIO_MODE_ANALOG	3
		#define STM32F_GPIO_SPEED_50M	2
		#define STM32F_GPIO_SPEED_20M	1
		#define STM32F_GPIO_SPEED_2M	0
	#elif CPUSTYLE_STM32H7XX	// 0: low speed, 1: medium speed, 2: fast speed, 3: high speed
		#define STM32F_GPIO_MODE_GPIO	1
		#define STM32F_GPIO_MODE_ALT	2
		#define STM32F_GPIO_MODE_ANALOG	3
		#define STM32F_GPIO_SPEED_50M	2
		#define STM32F_GPIO_SPEED_20M	1
		#define STM32F_GPIO_SPEED_2M	0
	#endif

	#if CPUSTYLE_STM32L0XX
		/* Спасибо ST, что исправили опечатки в заголовочном файле */
		#define GPIO_MODER_MODER0			GPIO_MODER_MODE0
		#define GPIO_MODER_MODER0_0			GPIO_MODER_MODE0_0
		#define	GPIO_PUPDR_PUPDR0			GPIO_PUPDR_PUPD0
		#define	GPIO_PUPDR_PUPDR0_0			GPIO_PUPDR_PUPD0_0
		#define	GPIO_OSPEEDER_OSPEEDR0		GPIO_OSPEEDER_OSPEED0
		#define	GPIO_OSPEEDER_OSPEEDR0_0	GPIO_OSPEEDER_OSPEED0_0
	#endif /* CPUSTYLE_STM32L0XX */

#elif CPUSTYLE_ATMEGA

	#define HARDWARE_OUTPUT_INITIALIZE(portname, dirport, bitmask, initstate) do { \
		(portname) |= ((bitmask) & (initstate)); \
		(portname) &= ~ ((bitmask) & ~ (initstate)); \
		(dirport) |= (bitmask); \
		} while (0)


	#define HARDWARE_INPUT_INITIALIZE(portname, dirport, bitmask, pullup) do { \
		(portname) |= ((bitmask) & (pullup)); \
		(portname) &= ~ ((bitmask) & ~ (pullup)); \
		(dirport) &= ~ (bitmask); \
		} while (0)


	#define HARDWARE_OUTPUT_SETSTATE(portname, bitmask, initstate) do { \
		(portname) |= ((bitmask) & (initstate)); \
		(portname) &= ~ ((bitmask) & ~ (initstate)); \
		} while (0)

	#define HARDWARE_OUTPUT_TOGGLE(portname, bitmask) do { \
		(portname) ^= ((bitmask)); \
		} while (0)

#elif CPUSTYLE_R7S721

	#define BSRR_S(v) ((v) * 0x10001uL)	/* Преобразование значения для установки бита в регистре */
	#define BSRR_C(v) ((v) * 0x10000uL)	/* Преобразование значения для сброса бита в регистре */

	// Функций-макросы установки/сброса битов в указанном порту
	//
	//  Пример использования:
	//		R7S721_TARGET_PORT_S(6, 0x04);	/* P6_2=1 */
	//		R7S721_TARGET_PORT_C(6, 0x04);	/* P6_2=0 */

	#define R7S721_TARGET_PORT_S(p, v) do { const uint_fast16_t vv = 0xFFFF & (v); GPIO.PSR ## p = BSRR_S(vv); (void) GPIO.PSR ## p; } while (0)
	#define R7S721_TARGET_PORT_C(p, v) do { const uint_fast16_t vv = 0xFFFF & (v); GPIO.PSR ## p = BSRR_C(vv); (void) GPIO.PSR ## p; } while (0)

	#define R7S721_TARGET_JPORT_S(p, v) do { const uint_fast16_t vv = 0xFFFF & (v); GPIO.JPSR ## p = BSRR_S(vv); (void) GPIO.JPSR ## p; } while (0)
	#define R7S721_TARGET_JPORT_C(p, v) do { const uint_fast16_t vv = 0xFFFF & (v); GPIO.JPSR ## p = BSRR_C(vv); (void) GPIO.JPSR ## p; } while (0)

	#define R7S721_INPUT_PORT(p) ((uint16_t) GPIO.PPR ## p)
	#define R7S721_INPUT_JPORT(p) ((uint16_t) GPIO.JPPR ## p)

#elif (CPUSTYLE_XC7Z || CPUSTYLE_XCZU) && ! LINUX_SUBSYSTEM

	#define ZYNQ_IORW32(addr) (* (volatile uint32_t *) (addr))
	void gpiobank_lock(unsigned bank, IRQL_t * oldIrql);
	void gpiobank_unlock(unsigned bank, IRQL_t oldIrql);

	// ug585-Zynq-7000-TRM.pdf v1.12.2, page 1631

	#define MIO_PIN_VALUE(disablercvr, pullup, io_type, speed, l3_sel, l2_sel, l1_sel, l0_sel, tri_enable) \
		( \
				((uint_fast32_t) !! (disablercvr) << 13) | /* 1: disable HSTL Input Buffer */ \
				((uint_fast32_t) !! (pullup) << 12) | /* 1: enable Pullup on IO Buffer pin */ \
				((uint_fast32_t) (io_type) << 9) | \
				((uint_fast32_t) !! (speed) << 8) | /* 0: slow CMOS edge */ \
				((uint_fast32_t) (l3_sel) << 5) | \
				((uint_fast32_t) (l2_sel) << 3) | \
				((uint_fast32_t) !! (l1_sel) << 2) | \
				((uint_fast32_t) !! (l0_sel) << 1) | \
				((uint_fast32_t) !! (tri_enable) << 0) | /* Tri-state enable, active high. */ \
				0 \
		)
	/* EMIO сигналы нумеруются начиная с 54 - ZYNQ_MIO_CNT */
	#define GPIO_PINGAP(pin) (((pin) < ZYNQ_MIO_CNT) ? (pin) : ((pin) + (64 - ZYNQ_MIO_CNT)))
	#define GPIO_PIN2BANK(pin) (GPIO_PINGAP(pin) / 32)
	#define GPIO_PIN2BITPOS(pin) (GPIO_PINGAP(pin) % 32)
	#define GPIO_PIN2MASK(pin) ((portholder_t) 1 << GPIO_PIN2BITPOS(pin))

	#define GPIO_BANK_DEFINE(pin, Bank, PinNumber) do { \
		Bank = GPIO_PIN2BANK(pin);    \
		PinNumber = GPIO_PIN2BITPOS(pin); \
	} while(0)

	enum
	{
		GPIO_IOTYPE_LVCMOS18 = 0x01,
		GPIO_IOTYPE_LVCMOS25 = 0x02,
		GPIO_IOTYPE_LVCMOS33 = 0x03,
		GPIO_IOTYPE_HSTL = 0x04
	};
	// initial value = 0x00001601
	#define MIO_SET_MODE(pin, pinmode) do { \
		SCLR->MIO_PIN [(pin)] = (pinmode); /*  */ \
	} while (0)

	// set pin state (thread-safe)
	#define GPIO_BANK_SET_OUTPUTS(bank, mask, outstate) do { \
		const portholder_t maskmsw = ((~ (uint32_t) (mask)) >> 16) & 0xFFFF; \
		const portholder_t masklsw = ((~ (uint32_t) (mask)) >> 0) & 0xFFFF; \
		if (masklsw != 0xFFFF) { \
			const portholder_t datalsw = ((uint32_t) (outstate) >> 0) & 0xFFFF; \
			ZYNQ_IORW32(GPIO_MASK_DATA_LSW(bank)) = (masklsw << 16) | datalsw; \
			(void) ZYNQ_IORW32(GPIO_MASK_DATA_LSW(bank)); \
		} \
		if (maskmsw != 0xFFFF) { \
			const portholder_t datamsw = ((uint32_t) (outstate) >> 16) & 0xFFFF; \
			ZYNQ_IORW32(GPIO_MASK_DATA_MSW(bank)) = (maskmsw << 16) | datamsw; \
			(void) ZYNQ_IORW32(GPIO_MASK_DATA_MSW(bank)); \
		} \
	} while (0)

	//	DIRM: Direction Mode. This controls whether the I/O pin is acting as an input or an output.
	//	Since the input logic is always enabled, this effectively enables/disables the output driver. When
	//	DIRM[x]==0, the output driver is disabled.

	#define GPIO_BANK_SET_DIRM(bank, mask, odstate) do { \
		const uintptr_t dirm = GPIO_DIRM(bank); \
		ZYNQ_IORW32(dirm) = (ZYNQ_IORW32(dirm) & ~ (mask)) | ((mask) & (odstate)); /* Then DIRM[x]==0, the output driver is disabled. */ \
	} while (0)

	//	OEN: Output Enable. When the I/O is configured as an output, this controls whether the output
	//	is enabled or not. When the output is disabled, the pin is 3-stated. When OEN[x]==0, the output
	//	driver is disabled.
	//	Note: If MIO TRI_ENABLE is set to 1, enabling 3-state and disabling the driver, then OEN is
	//	ignored and the output is 3-stated.

	#define GPIO_BANK_SET_OEN(bank, mask, odstate) do { \
		const uintptr_t oen = GPIO_OEN(bank); \
		ZYNQ_IORW32(oen) = (ZYNQ_IORW32(oen) & ~ (mask)) | ((mask) & (odstate)); /* When OEN[x]==0, the output driver is disabled */ \
	} while (0)

	// set pin state (thread-safe)
	#define gpio_writepin(pin, state) do { \
		const portholder_t bank = GPIO_PIN2BANK(pin); \
		const portholder_t mask = GPIO_PIN2MASK(pin); \
		GPIO_BANK_SET_OUTPUTS(bank, mask, mask * !! (state)); \
	} while (0)

	// set pin mode (no thread-safe)
	#define gpio_output2(pin, state, pinmode) do { \
		const portholder_t bank = GPIO_PIN2BANK(pin); \
		const portholder_t mask = GPIO_PIN2MASK(pin); \
		SCLR->SLCR_UNLOCK = 0x0000DF0DuL; \
		if ((pin) < ZYNQ_MIO_CNT) { \
			MIO_SET_MODE((pin), (pinmode)); /* initial value - with pull-up, TRI_ENABLE=0, then 3-state is controlled by the gpio.OEN_x register. */ \
		} \
		GPIO_BANK_SET_OUTPUTS(bank, mask, mask * !! (state)); \
		GPIO_BANK_SET_OEN(bank, mask, mask); \
		GPIO_BANK_SET_DIRM(bank, mask, mask); \
	} while (0)

	#define gpio_input2(pin, pinmode) do { \
		const portholder_t bank = GPIO_PIN2BANK(pin); \
		const portholder_t mask = GPIO_PIN2MASK(pin); \
		SCLR->SLCR_UNLOCK = 0x0000DF0DuL; \
		if ((pin) < ZYNQ_MIO_CNT) { \
			MIO_SET_MODE((pin), (pinmode)); /* initial value - with pull-up, TRI_ENABLE=0, then 3-state is controlled by the gpio.OEN_x register. */ \
		} \
		GPIO_BANK_SET_OEN(bank, mask, 0); \
		GPIO_BANK_SET_DIRM(bank, mask, 0); \
	} while (0)

	void gpio_onchangeinterrupt(unsigned pin, void (* handler)(void), uint32_t priority, uint32_t tgcpu);
	void gpio_onrisinginterrupt(unsigned pin, void (* handler)(void), uint32_t priority, uint32_t tgcpu);
	void gpio_onfallinterrupt(unsigned pin, void (* handler)(void), uint32_t priority, uint32_t tgcpu);

	// Enable output drive for pin
	#define MIO_SET_TRI_ENABLE(pin, tri_enable) do { \
		if (!(tri_enable)) { SCLR->MIO_PIN [(pin)] &= ~ 0x01; } else { SCLR->MIO_PIN [(pin)] |= 0x01; } \
	} while (0)

	// set pin mode (no thread-safe)
	// MIO_PIN_VALUE(disablercvr, pullup, io_type, speed, l3_sel, l2_sel, l1_sel, l0_sel, tri_enable)
	#define gpio_opendrain2(pin, drive, pinmode) do { \
		const portholder_t bank = GPIO_PIN2BANK(pin); \
		const portholder_t mask = GPIO_PIN2MASK(pin); \
		SCLR->SLCR_UNLOCK = 0x0000DF0DuL; \
		GPIO_BANK_SET_OUTPUTS(bank, mask, 0); \
		if ((pin) < ZYNQ_MIO_CNT) { \
			MIO_SET_MODE((pin), (pinmode)); /* initial value - with pull-up, TRI_ENABLE=0, then 3-state is controlled by the gpio.OEN_x register. */ \
			MIO_SET_TRI_ENABLE((pin), ! (drive)); \
		} \
		GPIO_BANK_SET_DIRM(bank, mask, mask); \
		GPIO_BANK_SET_OEN(bank, mask, mask); \
	} while (0)

	// Enable output drive for pin (thread-safe, for different pins)
	#define gpio_drive(pin, drive) do { \
		if ((pin) < ZYNQ_MIO_CNT) { \
			MIO_SET_TRI_ENABLE((pin), ! (drive)); \
		} \
	} while (0)

	// set pin mode (no thread-safe)
	// MIO_PIN_VALUE(disablercvr, pullup, io_type, speed, l3_sel, l2_sel, l1_sel, l0_sel, tri_enable)
	#define emio_opendrain2(pin, drive) do { \
		const portholder_t bank = GPIO_PIN2BANK(pin); \
		const portholder_t mask = GPIO_PIN2MASK(pin); \
		SCLR->SLCR_UNLOCK = 0x0000DF0DuL; \
		GPIO_BANK_SET_OUTPUTS(bank, mask, 0); \
		GPIO_BANK_SET_DIRM(bank, mask, mask); \
		GPIO_BANK_SET_OEN(bank, mask, mask * !! (drive)); \
	} while (0)

	// Enable output drive for pin (no thread-safe)
	#define emio_drive(pin, drive) do { \
		const portholder_t bank = GPIO_PIN2BANK(pin); \
		const portholder_t mask = GPIO_PIN2MASK(pin); \
		GPIO_BANK_SET_OEN(bank, mask, mask * !! (drive)); \
	} while (0)

	#define gpio_peripherial(pin, pinmode) do { \
		const portholder_t bank = GPIO_PIN2BANK(pin); \
		const portholder_t mask = GPIO_PIN2MASK(pin); \
		SCLR->SLCR_UNLOCK = 0x0000DF0DuL; \
		if ((pin) < ZYNQ_MIO_CNT) { \
			MIO_SET_MODE((pin), (pinmode)); /* initial value - with pull-up, TRI_ENABLE=0, then 3-state is controlled by the gpio.OEN_x register. */ \
		/*GPIO_BANK_SET_DIRM(bank, mask, mask); */\
		/*GPIO_BANK_SET_OEN(bank, mask, mask); */\
		} \
	} while (0)

	#define gpio_readpin(pin) ((ZYNQ_IORW32(GPIO_DATA_RO(GPIO_PIN2BANK(pin))) & GPIO_PIN2MASK(pin)) != 0)
	/* чтение группы PIO/MIO, находящихся в одном банке.
	 * Указывается нмер самого правого из них.
	 * Значение возвращается в младших битах.
	 * */
	#define gpio_readbus(pin, lowmask) ((ZYNQ_IORW32(GPIO_DATA_RO(GPIO_PIN2BANK(pin))) >> GPIO_PIN2BITPOS(pin)) & (lowmask))
	/* запись группы PIO/MIO, находящихся в одном банке.
	 * Указывается номер самого правого из них.
	 * Значение передается в младших битах.
	 * */
	#define gpio_writebus(pin, lowmask, state) do { \
		const portholder_t bank = GPIO_PIN2BANK(pin); \
		const portholder_t mask = (lowmask) << GPIO_PIN2BITPOS(pin); \
		const portholder_t value = (state) << GPIO_PIN2BITPOS(pin); \
		GPIO_BANK_SET_OUTPUTS(bank, mask, value); \
	} while (0)
	#define gpio_fastinput2(pin, lowmask) do { \
		const portholder_t bank = GPIO_PIN2BANK(pin); \
		const portholder_t mask = (lowmask) << GPIO_PIN2BITPOS(pin); \
		/*GPIO_BANK_SET_OEN(bank, mask, 0); */\
		GPIO_BANK_SET_DIRM(bank, mask, 0); \
	} while (0)
	#define gpio_fastoutput2(pin, lowmask) do { \
		const portholder_t bank = GPIO_PIN2BANK(pin); \
		const portholder_t mask = (lowmask) << GPIO_PIN2BITPOS(pin); \
		/*GPIO_BANK_SET_OEN(bank, mask, mask); */\
		GPIO_BANK_SET_DIRM(bank, mask, mask); \
	} while (0)

#endif /* CPUSTYLE_STM32F */

void arm_hardware_pioa_inputs(unsigned long ipins);
void arm_hardware_pioa_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioa_outputs2m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioa_outputs50m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioa_opendrain(unsigned long opins, unsigned long initialstate);

void arm_hardware_piob_inputs(unsigned long ipins);
void arm_hardware_piob_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_piob_outputs2m(unsigned long opins, unsigned long initialstate);
void arm_hardware_piob_outputs50m(unsigned long opins, unsigned long initialstate);
void arm_hardware_piob_opendrain(unsigned long opins, unsigned long initialstate);

void arm_hardware_pioc_inputs(unsigned long ipins);
void arm_hardware_pioc_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioc_outputs2m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioc_outputs50m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioc_opendrain(unsigned long opins, unsigned long initialstate);

void arm_hardware_piod_inputs(unsigned long ipins);
void arm_hardware_piod_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_piod_outputs2m(unsigned long opins, unsigned long initialstate);
void arm_hardware_piod_outputs50m(unsigned long opins, unsigned long initialstate);
void arm_hardware_piod_opendrain(unsigned long opins, unsigned long initialstate);

void arm_hardware_pioe_inputs(unsigned long ipins);
void arm_hardware_pioe_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioe_outputs2m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioe_outputs50m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioe_opendrain(unsigned long opins, unsigned long initialstate);

void arm_hardware_piof_inputs(unsigned long ipins);
void arm_hardware_piof_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_piof_outputs2m(unsigned long opins, unsigned long initialstate);
void arm_hardware_piof_outputs50m(unsigned long opins, unsigned long initialstate);
void arm_hardware_piof_opendrain(unsigned long opins, unsigned long initialstate);

void arm_hardware_piog_inputs(unsigned long ipins);
void arm_hardware_piog_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_piog_outputs2m(unsigned long opins, unsigned long initialstate);
void arm_hardware_piog_outputs50m(unsigned long opins, unsigned long initialstate);
void arm_hardware_piog_opendrain(unsigned long opins, unsigned long initialstate);

void arm_hardware_pioh_inputs(unsigned long ipins);
void arm_hardware_pioh_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioh_outputs2m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioh_outputs50m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioh_opendrain(unsigned long opins, unsigned long initialstate);

void arm_hardware_pioi_inputs(unsigned long ipins);
void arm_hardware_pioi_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioi_outputs2m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioi_outputs50m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioi_opendrain(unsigned long opins, unsigned long initialstate);

void arm_hardware_pioj_inputs(unsigned long ipins);
void arm_hardware_pioj_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioj_outputs2m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioj_outputs50m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioj_opendrain(unsigned long opins, unsigned long initialstate);

void arm_hardware_piok_inputs(unsigned long ipins);
void arm_hardware_piok_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_piok_outputs2m(unsigned long opins, unsigned long initialstate);
void arm_hardware_piok_outputs50m(unsigned long opins, unsigned long initialstate);
void arm_hardware_piok_opendrain(unsigned long opins, unsigned long initialstate);

void arm_hardware_piol_inputs(unsigned long ipins);
void arm_hardware_piol_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_piol_outputs2m(unsigned long opins, unsigned long initialstate);
void arm_hardware_piol_outputs50m(unsigned long opins, unsigned long initialstate);
void arm_hardware_piol_opendrain(unsigned long opins, unsigned long initialstate);

void arm_hardware_pioz_inputs(unsigned long ipins);
void arm_hardware_pioz_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioz_outputs2m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioz_outputs50m(unsigned long opins, unsigned long initialstate);
void arm_hardware_pioz_opendrain(unsigned long opins, unsigned long initialstate);

void arm_hardware_pioa_peripha(unsigned long pins);		// выводы присоединены к periph A
void arm_hardware_pioa_periphb(unsigned long pins);		// выводы присоединены к periph B
void arm_hardware_pioa_periphc(unsigned long pins);		// выводы присоединены к periph C
void arm_hardware_pioa_periphd(unsigned long pins);		// выводы присоединены к periph D
void arm_hardware_piob_peripha(unsigned long pins);		// выводы присоединены к periph A
void arm_hardware_piob_periphb(unsigned long pins);		// выводы присоединены к periph B
void arm_hardware_piob_periphc(unsigned long pins);		// выводы присоединены к periph C
void arm_hardware_piob_periphd(unsigned long pins);		// выводы присоединены к periph D

void arm_hardware_pioa_only(unsigned long pins);		// выводы присоединены к PIO
void arm_hardware_piob_only(unsigned long pins);		// выводы присоединены к PIO

void arm_hardware_pioa_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu);
void arm_hardware_piob_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu);
void arm_hardware_pioc_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu);
void arm_hardware_piod_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu);
void arm_hardware_pioe_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu);
void arm_hardware_piof_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu);
void arm_hardware_piog_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu);
void arm_hardware_pioh_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu);
void arm_hardware_pioi_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu);
void arm_hardware_pioj_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu);
void arm_hardware_piok_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu);
void arm_hardware_pioz_onchangeinterrupt(unsigned long ipins, unsigned long raise, unsigned long fall, uint32_t priority, uint32_t tgcpu);

void arm_hardware_pioa_altfn2(unsigned long opins, unsigned af);
void arm_hardware_piob_altfn2(unsigned long opins, unsigned af);
void arm_hardware_pioc_altfn2(unsigned long opins, unsigned af);
void arm_hardware_piod_altfn2(unsigned long opins, unsigned af);
void arm_hardware_pioe_altfn2(unsigned long opins, unsigned af);
void arm_hardware_piof_altfn2(unsigned long opins, unsigned af);
void arm_hardware_piog_altfn2(unsigned long opins, unsigned af);
void arm_hardware_pioh_altfn2(unsigned long opins, unsigned af);
void arm_hardware_pioi_altfn2(unsigned long opins, unsigned af);
void arm_hardware_pioj_altfn2(unsigned long opins, unsigned af);
void arm_hardware_piok_altfn2(unsigned long opins, unsigned af);
void arm_hardware_piol_altfn2(unsigned long opins, unsigned af);
void arm_hardware_pioz_altfn2(unsigned long opins, unsigned af);

void arm_hardware_pioa_altfn20(unsigned long opins, unsigned af);
void arm_hardware_piob_altfn20(unsigned long opins, unsigned af);
void arm_hardware_pioc_altfn20(unsigned long opins, unsigned af);
void arm_hardware_piod_altfn20(unsigned long opins, unsigned af);
void arm_hardware_pioe_altfn20(unsigned long opins, unsigned af);
void arm_hardware_piof_altfn20(unsigned long opins, unsigned af);
void arm_hardware_piog_altfn20(unsigned long opins, unsigned af);
void arm_hardware_pioh_altfn20(unsigned long opins, unsigned af);
void arm_hardware_pioi_altfn20(unsigned long opins, unsigned af);
void arm_hardware_pioj_altfn20(unsigned long opins, unsigned af);
void arm_hardware_piok_altfn20(unsigned long opins, unsigned af);
void arm_hardware_piol_altfn20(unsigned long opins, unsigned af);
void arm_hardware_pioz_altfn20(unsigned long opins, unsigned af);

void arm_hardware_pioa_altfn50(unsigned long opins, unsigned af);
void arm_hardware_piob_altfn50(unsigned long opins, unsigned af);
void arm_hardware_pioc_altfn50(unsigned long opins, unsigned af);
void arm_hardware_piod_altfn50(unsigned long opins, unsigned af);
void arm_hardware_pioe_altfn50(unsigned long opins, unsigned af);
void arm_hardware_piof_altfn50(unsigned long opins, unsigned af);
void arm_hardware_piog_altfn50(unsigned long opins, unsigned af);
void arm_hardware_pioh_altfn50(unsigned long opins, unsigned af);
void arm_hardware_pioi_altfn50(unsigned long opins, unsigned af);
void arm_hardware_pioj_altfn50(unsigned long opins, unsigned af);
void arm_hardware_piok_altfn50(unsigned long opins, unsigned af);
void arm_hardware_piol_altfn50(unsigned long opins, unsigned af);
void arm_hardware_pioz_altfn50(unsigned long opins, unsigned af);

// включить подтяжку вверх или вниз
void arm_hardware_pioa_updown(unsigned long up, unsigned long down);
void arm_hardware_piob_updown(unsigned long up, unsigned long down);
void arm_hardware_pioc_updown(unsigned long up, unsigned long down);
void arm_hardware_piod_updown(unsigned long up, unsigned long down);
void arm_hardware_pioe_updown(unsigned long up, unsigned long down);
void arm_hardware_piof_updown(unsigned long up, unsigned long down);
void arm_hardware_piog_updown(unsigned long up, unsigned long down);
void arm_hardware_pioh_updown(unsigned long up, unsigned long down);
void arm_hardware_pioi_updown(unsigned long up, unsigned long down);
void arm_hardware_pioj_updown(unsigned long up, unsigned long down);
void arm_hardware_piok_updown(unsigned long up, unsigned long down);
void arm_hardware_piol_updown(unsigned long up, unsigned long down);
void arm_hardware_pioz_updown(unsigned long up, unsigned long down);

// отключить подтяжку вверх или вниз
void arm_hardware_pioa_updownoff(unsigned long ipins);
void arm_hardware_piob_updownoff(unsigned long ipins);
void arm_hardware_pioc_updownoff(unsigned long ipins);
void arm_hardware_piod_updownoff(unsigned long ipins);
void arm_hardware_pioe_updownoff(unsigned long ipins);
void arm_hardware_piof_updownoff(unsigned long ipins);
void arm_hardware_piog_updownoff(unsigned long ipins);
void arm_hardware_pioh_updownoff(unsigned long ipins);
void arm_hardware_pioi_updownoff(unsigned long ipins);
void arm_hardware_pioj_updownoff(unsigned long ipins);
void arm_hardware_piok_updownoff(unsigned long ipins);
void arm_hardware_piol_updownoff(unsigned long ipins);
void arm_hardware_pioz_updownoff(unsigned long ipins);

void arm_hardware_pioa_periphopendrain_altfn2(unsigned long opins, unsigned af);
void arm_hardware_piob_periphopendrain_altfn2(unsigned long opins, unsigned af);
void arm_hardware_pioc_periphopendrain_altfn2(unsigned long opins, unsigned af);
void arm_hardware_piod_periphopendrain_altfn2(unsigned long opins, unsigned af);
void arm_hardware_pioe_periphopendrain_altfn2(unsigned long opins, unsigned af);
void arm_hardware_piof_periphopendrain_altfn2(unsigned long opins, unsigned af);
void arm_hardware_piog_periphopendrain_altfn2(unsigned long opins, unsigned af);
void arm_hardware_pioh_periphopendrain_altfn2(unsigned long opins, unsigned af);
void arm_hardware_pioi_periphopendrain_altfn2(unsigned long opins, unsigned af);
void arm_hardware_pioj_periphopendrain_altfn2(unsigned long opins, unsigned af);
void arm_hardware_piok_periphopendrain_altfn2(unsigned long opins, unsigned af);
void arm_hardware_piol_periphopendrain_altfn2(unsigned long opins, unsigned af);
void arm_hardware_pioz_periphopendrain_altfn2(unsigned long opins, unsigned af);

void arm_hardware_pioa_analoginput(unsigned long ipins);
void arm_hardware_piob_analoginput(unsigned long ipins);
void arm_hardware_pioc_analoginput(unsigned long ipins);

// R7S721 ports
void arm_hardware_jpio0_inputs(unsigned long ipins);	// JTAG inputs access
void arm_hardware_pio0_inputs(unsigned long ipins);
void arm_hardware_pio1_inputs(unsigned long ipins);
void arm_hardware_pio2_inputs(unsigned long ipins);
void arm_hardware_pio3_inputs(unsigned long ipins);
void arm_hardware_pio4_inputs(unsigned long ipins);
void arm_hardware_pio5_inputs(unsigned long ipins);
void arm_hardware_pio6_inputs(unsigned long ipins);
void arm_hardware_pio7_inputs(unsigned long ipins);
void arm_hardware_pio8_inputs(unsigned long ipins);
void arm_hardware_pio9_inputs(unsigned long ipins);
void arm_hardware_pio10_inputs(unsigned long ipins);
void arm_hardware_pio11_inputs(unsigned long ipins);

// R7S721 ports
// PIOJ and PIO0 input-only ports
void arm_hardware_pio1_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pio2_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pio3_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pio4_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pio5_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pio6_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pio7_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pio8_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pio9_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pio10_outputs(unsigned long opins, unsigned long initialstate);
void arm_hardware_pio11_outputs(unsigned long opins, unsigned long initialstate);

// R7S721 peripherials
void arm_hardware_pio1_alternative(unsigned long iopins, unsigned alt);
void arm_hardware_pio2_alternative(unsigned long iopins, unsigned alt);
void arm_hardware_pio3_alternative(unsigned long iopins, unsigned alt);
void arm_hardware_pio4_alternative(unsigned long iopins, unsigned alt);
void arm_hardware_pio5_alternative(unsigned long iopins, unsigned alt);
void arm_hardware_pio6_alternative(unsigned long iopins, unsigned alt);
void arm_hardware_pio7_alternative(unsigned long iopins, unsigned alt);
void arm_hardware_pio8_alternative(unsigned long iopins, unsigned alt);
void arm_hardware_pio9_alternative(unsigned long iopins, unsigned alt);
void arm_hardware_pio10_alternative(unsigned long iopins, unsigned alt);
void arm_hardware_pio11_alternative(unsigned long iopins, unsigned alt);

// R7S721 pin change interrupts
void arm_hardware_piojp0_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void));
void arm_hardware_pio0_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void));
void arm_hardware_pio1_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void));
void arm_hardware_pio2_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void));
void arm_hardware_pio3_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void));
void arm_hardware_pio4_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void));
void arm_hardware_pio5_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void));
void arm_hardware_pio6_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void));
void arm_hardware_pio7_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void));
void arm_hardware_pio8_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void));
void arm_hardware_pio9_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void));
void arm_hardware_pio10_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void));	// RZ/A1H
void arm_hardware_pio11_onchangeinterrupt(unsigned long ipins, int edge, uint32_t priority, void (* vector)(void));	// RZ/A1H

void arm_hardware_irqn_interrupt(unsigned long irq, int edge, uint32_t priority, void (* vector)(void));

#if (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507)
	/*!< Atomic port state change */
	void gpioX_setstate(
		GPIO_TypeDef * gpio,
		portholder_t mask,
		portholder_t state
		);
	portholder_t gpioX_getinputs(
		GPIO_TypeDef * gpio
		);

#endif /* (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64) */


portholder_t power2(uint_fast8_t v);	// Перенос каждого бита в байте в позицию с увеличенным в 2 раза номером.
portholder_t power4(uint_fast8_t v);	// Перенос каждого бита в байте в позицию с увеличенным в 4 раза номером.
portholder_t power8(uint_fast8_t v);	// Перенос каждого бита в байте в позицию с увеличенным в 8 раз номером.

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PIO_H_INCLUDED */
