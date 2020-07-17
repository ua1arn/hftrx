/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Трансивер с DSP обработкой "Storch" на процессоре Renesas R7S721020VCFP с кодеком NAU8822L и FPGA EP4CE22E22I7N
// rmainunit_v5km2.pcb rmainunit_v5km3

#ifndef ARM_R7S72_TQFP176_CPUSTYLE_STORCH_V8_H_INCLUDED
#define ARM_R7S72_TQFP176_CPUSTYLE_STORCH_V8_H_INCLUDED 1

#define WITHSPI16BIT	1		/* возможно использование 16-ти битных слов при обмене по SPI */
#define WITHSPI32BIT	1		/* возможно использование 32-ти битных слов при обмене по SPI */
#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
//#define WITHSPISW 	1	/* Использование программного управления SPI. */

//#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */

//#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
//#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */

//#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
#define WIHSPIDFHW	1	/* аппаратное обслуживание DATA FLASH */
#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 2-м проводам */
#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 4-м проводам */

#if WITHISBOOTLOADER

	//#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	*/

	#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
	#define WITHUSBDEV_VBUSSENSE	1	/* используется предопределенный вывод VBUS_SENSE */
	//#define WITHUSBDEV_HSDESC	1	/* Требуется формировать дескрипторы как для HIGH SPEED */
	#define WITHUSBHW_DEVICE	(& USB200)	/* на этом устройстве поддерживается функциональность DEVICE	*/
	//#define WITHUSBHW_HOST	(& USB200)	/* на этом устройстве поддерживается функциональность HOST	*/

	//#define WITHUART1HW	1	/* Используется периферийный контроллер последовательного порта #1 SCIF0 */
	#define WITHUART2HW	1	/* Используется периферийный контроллер последовательного порта #2 SCIF3 */

	//#define WITHCAT_USART2	1
	//#define WITHCAT_CDC			1	/* использовать виртуальный последовательный порт на USB соединении */
	//#define WITHMODEM_CDC		1
	#define WITHDEBUG_USART2	1
	#define WITHMODEM_USART2	1
	#define WITHNMEA_USART2		1	/* порт подключения GPS/GLONASS */

	//#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */
	//#define WITHUSBCDC		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
	//#define WITHUSBHWCDC_N	2	/* количество виртуальных последовательных портов */
	//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
	//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */
	#define WITHUSBDFU	1	/* DFU USB Device Firmware Upgrade support */
	//#define WITHMOVEDFU 1	// Переместить интерфейс DFU в область меньших номеров. Утилита dfu-util 0.9 не работает с DFU на интерфейсе с индексом 10
	#define WITHUSBWCID	1

#else /* WITHISBOOTLOADER */

	#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	*/
	//#define WITHCPUDACHW	1	/* использование DAC - в renesas R7S72 нету */
	#define WITHCPUADCHW 	1	/* использование ADC */

	#define WITHI2SHW	1	/* Использование SSIF0 I2S 2*16 bit - аудио кодек */
	#define WITHSAI1HW	1	/* Использование SSIF1 I2S 8*32 bit - FPGA IF codec */
	#if WITHRTS192
		#define WITHSAI2HW	1	/* Использование SSIF2 I2S 2*32 (2*32) bit - FPGA панорама	*/
	#endif /* WITHRTS192 */

	#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */

	// USB device parameters
	#define WITHUSBHW_DEVICE	(& USB201)	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBDEV_VBUSSENSE	1	/* используется предопределенный вывод VBUS_SENSE */
	//#define WITHUSBDEV_HSDESC	1	/* Требуется формировать дескрипторы как для HIGH SPEED */

	// USB host parameters
	//#define WITHUSBHW_HOST	(& USB200)	/* на этом устройстве поддерживается функциональность HOST	*/

	//#define WITHUART1HW	1	/* Используется периферийный контроллер последовательного порта #1 SCIF0 */
	#define WITHUART2HW	1	/* Используется периферийный контроллер последовательного порта #2 SCIF3 */

	//#define WITHCAT_USART2	1
	#define WITHCAT_CDC			1	/* использовать виртуальный последовательный порт на USB соединении */
	//#define WITHMODEM_CDC		1
	#define WITHDEBUG_USART2	1
	#define WITHMODEM_USART2	1
	#define WITHNMEA_USART2		1	/* порт подключения GPS/GLONASS */

	//#define WITHUAC2		1	/* UAC2 support */
	#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */
	#define WITHUSBCDC		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
	#define WITHUSBHWCDC_N	2	/* количество виртуальных последовательных портов */
	//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
	//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */
	//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
	//#define WITHUSBHID		1	/* HID использовать Human Interface Device на USB соединении */
	#define WITHUSBDFU	1	/* DFU USB Device Firmware Upgrade support */
	//#define WITHMOVEDFU 1	// Переместить интерфейс DFU в область меньших номеров. Утилита dfu-util 0.9 не работает с DFU на интерфейсе с индексом 10
	#define WITHUSBWCID	1

#endif /* WITHISBOOTLOADER */

#if LCDMODE_SPI_NA
	// эти контроллеры требуют только RS

	#define LS020_RS_PORT_S(v) do {	R7S721_TARGET_PORT_S(7, v); } while (0)
	#define LS020_RS_PORT_C(v) do {	R7S721_TARGET_PORT_C(7, v); } while (0)
	#define LS020_RS			(1u << 0)			// P7_0 D7 signal in HD44780 socket

#elif LCDMODE_SPI_RN
	// эти контроллеры требуют только RESET

	#define LS020_RESET_PORT_S(v) do {	R7S721_TARGET_PORT_S(7, v); } while (0)
	#define LS020_RESET_PORT_C(v) do {	R7S721_TARGET_PORT_C(7, v); } while (0)
	#define LS020_RESET			(1u << 1)			// * P7_1 D6 signal in HD44780 socket

#elif LCDMODE_SPI_RA
	// Эти контроллеры требуют RESET и RS

	#define LS020_RS_PORT_S(v) do {	R7S721_TARGET_PORT_S(7, v); } while (0)
	#define LS020_RS_PORT_C(v) do {	R7S721_TARGET_PORT_C(7, v); } while (0)
	#define LS020_RS			(1u << 0)			// P7_0 D7 signal in HD44780 socket

	#define LS020_RESET_PORT_S(v) do {	R7S721_TARGET_PORT_S(7, v); } while (0)
	#define LS020_RESET_PORT_C(v) do {	R7S721_TARGET_PORT_C(7, v); } while (0)
	#define LS020_RESET			(1u << 1)			// P7_1 D6 signal in HD44780 socket

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)


#endif

#if WITHENCODER
	// Выводы подключения енкодера
	// Encoder #1 inputs: P5_9 - PHASE A, P5_8 = PHASE B
	// Encoder #2 inputs: P5_11 - PHASE A, P5_10 = PHASE B

	// Допустимы:
	// IRQ0:			P6_7,	P1_8, P1_4, 
	// IRQ1:			P6_8,	P1_9, P1_5,									P2_9
	// IRQ2:			P6_9,	P1_10, P1_6,								P2_7
	// IRQ3:	P5_8,	P6_10,	P1_11, P1_7,	P7_3, P7_8
	// IRQ4:	P5_9,			P1_12, P1_0,	P7_2,				P0_0
	// IRQ5:	P5_10,			P1_13. P1_1,	P7_7, P7_9,					P2_2
	// IRQ6:	P5_11,			P1_14, P1_2,	P7_6, 		P8_6,			P2_1
	// IRQ7:					P1_15, P1_3,				P8_7,	P0_2,	P2_0,	

	// Обязательно буфер (входы процессора низковольтные).

	#define ENCODER2_INPUT_PORT			(R7S721_INPUT_PORT(5)) 
	#define ENCODER2_BITA (1u << 11)	// P5_11 IRQ6
	#define ENCODER2_BITB (1u << 10)	// P5_10 IRQ5
	#define ENCODER2_BITS (ENCODER2_BITA | ENCODER2_BITB)
	#define ENCODER2_SHIFT 10	// Отсутствие этого значения означает что биты не подряд

	#define ENCODER_INPUT_PORT			(R7S721_INPUT_PORT(5)) 
	#define ENCODER_BITA (1u << 9)	// P5_9 IRQ4
	#define ENCODER_BITB (1u << 8)	// P5_8 IRQ3
	#define ENCODER_BITS (ENCODER_BITA | ENCODER_BITB)
	#define ENCODER_SHIFT 8	// Отсутствие этого значения означает что биты не подряд

	/*
		edge values
		00: Interrupt request is detected on low level of IRQn input
		01: Interrupt request is detected on falling edge of IRQn input
		10: Interrupt request is detected on rising edge of IRQn input
		11: Interrupt request is detected on both edges of IRQn input
	*/
	#define ENCODER_INITIALIZE() \
		do { \
			/* arm_hardware_pio5_alternative(ENCODER2_BITS, R7S721_PIOALT_4); */ \
			arm_hardware_pio5_inputs(ENCODER2_BITS); \
			/* arm_hardware_irqn_interrupt(5, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt2); */ /* IRQ5, both edges */ \
			/* arm_hardware_irqn_interrupt(6, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt2); */ /* IRQ6, both edges */ \
			arm_hardware_pio5_alternative(ENCODER_BITS, R7S721_PIOALT_4); \
			arm_hardware_irqn_interrupt(3, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt); /* IRQ3, both edges */ \
			arm_hardware_irqn_interrupt(4, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt); /* IRQ4, both edges */ \
		} while (0)

#endif /* WITHENCODER */

#define R7S721_USE_AUDIO_CLK 0	// CKS 1: AUDIO_CLK input 0: AUDIO_X1 input

#if WITHI2SHW
	#define HARDWARE_SSIF0_INITIALIZE() do { \
		arm_hardware_pio6_alternative(1U << 8, R7S721_PIOALT_3); /* P6_8 SSISCK0 */ \
		arm_hardware_pio6_alternative(1U << 9, R7S721_PIOALT_3); /* P6_9 SSIWS0 */ \
		arm_hardware_pio6_alternative(1U << 10, R7S721_PIOALT_3); /* P6_10 SSITxD0 */ \
		arm_hardware_pio6_alternative(1U << 11, R7S721_PIOALT_3); /* P6_11 SSIRxD0 */ \
	} while (0)
#endif /* WITHI2SHW */

#if WITHSAI1HW
	#define HARDWARE_SSIF1_INITIALIZE() do { \
		arm_hardware_pio5_alternative(1U << 4, R7S721_PIOALT_3); /* P5_4 SSISCK1 */ \
		arm_hardware_pio5_alternative(1U << 5, R7S721_PIOALT_3); /* P5_5 SSIWS1 */ \
		arm_hardware_pio5_alternative(1U << 6, R7S721_PIOALT_3); /* P5_6 SSITxD1 */ \
		arm_hardware_pio5_alternative(1U << 7, R7S721_PIOALT_3); /* P5_7 SSIRxD1 */ \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI2HW
	#define HARDWARE_SSIF2_INITIALIZE() do { \
		arm_hardware_pio5_alternative(1U << 12, R7S721_PIOALT_2); /* P5_12 SSISCK2 */ \
		arm_hardware_pio5_alternative(1U << 13, R7S721_PIOALT_2); /* P5_13 SSIWS2 */ \
		arm_hardware_pio5_alternative(1U << 14, R7S721_PIOALT_2); /* P5_14 SSIDATA2 */ \
	} while (0)
#endif /* WITHSAI2HW */

#if WITHSDHCHW

	#define HARDWARE_SDIO_WP_BIT	(1U << 6)	/* P3_6 SD_WP_0 */
	#define HARDWARE_SDIO_CD_BIT	(1U << 7)	/* P3_7 SD_CD_0 */

	#define HARDWARE_SDIO_INITIALIZE() do { \
		arm_hardware_pio3_alternative(1U << 0, R7S721_PIOALT_2); /* P3_0 SD_D2_0 */ \
		arm_hardware_pio3_alternative(1U << 1, R7S721_PIOALT_2); /* P3_1 SD_D3_0 */ \
		arm_hardware_pio3_alternative(1U << 2, R7S721_PIOALT_2); /* P3_2 SD_CMD_0 */ \
		arm_hardware_pio3_alternative(1U << 3, R7S721_PIOALT_2); /* P3_3 SD_CLK_0 */ \
		arm_hardware_pio3_alternative(1U << 4, R7S721_PIOALT_2); /* P3_4 SD_D0_0 */ \
		arm_hardware_pio3_alternative(1U << 5, R7S721_PIOALT_2); /* P3_5 SD_D1_0 */ \
		/* arm_hardware_pio3_alternative(HARDWARE_SDIO_WP_BIT, R7S721_PIOALT_2); */ /* P3_6 SD_WP_0 */ \
		/* arm_hardware_pio3_alternative(HARDWARE_SDIO_CD_BIT, R7S721_PIOALT_2); */ /* P3_7 SD_CD_0 */ \
	} while (0)

	/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
	#define HARDWARE_SDIO_HANGOFF()	do { \
		arm_hardware_pio3_inputs(1U << 0); /* P3_0 SD_D2_0 */ \
		arm_hardware_pio3_inputs(1U << 1); /* P3_1 SD_D3_0 */ \
		arm_hardware_pio3_inputs(1U << 2); /* P3_2 SD_CMD_0 */ \
		arm_hardware_pio3_inputs(1U << 3); /* P3_3 SD_CLK_0 */ \
		arm_hardware_pio3_inputs(1U << 4); /* P3_4 SD_D0_0 */ \
		arm_hardware_pio3_inputs(1U << 5); /* P3_5 SD_D1_0 */ \
		/* arm_hardware_pio3_alternative(HARDWARE_SDIO_WP_BIT, R7S721_PIOALT_2); */ /* P3_6 SD_WP_0 */ \
		/* arm_hardware_pio3_alternative(HARDWARE_SDIO_CD_BIT, R7S721_PIOALT_2); */ /* P3_7 SD_CD_0 */ \
	} while (0)

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
		arm_hardware_pio3_inputs(HARDWARE_SDIO_WP_BIT); /* P3_6 SD_WP_0 */ \
		arm_hardware_pio3_inputs(HARDWARE_SDIO_CD_BIT); /* P3_7 SD_CD_0 */ \
	} while (0)

	#define HARDWARE_SDIOSENSE_CD() ((R7S721_INPUT_PORT(3) & HARDWARE_SDIO_CD_BIT) == 0)	/* получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() ((R7S721_INPUT_PORT(3) & HARDWARE_SDIO_WP_BIT) != 0)	/* получить состояние датчика CARD WRITE PROTECT */

	/* если питание SD CARD управляется прямо с процессора */
	#define HARDWARE_SDIO_POWER_BIT (1uL << 15)	// P3_15
	#define HARDWARE_SDIO_POWER_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define HARDWARE_SDIO_POWER_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)

	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		arm_hardware_pio3_outputs(HARDWARE_SDIO_POWER_BIT, 0); /* питание выключено */ \
	} while (0)

	/* parameter on not zero for powering SD CARD */
	#define HARDWARE_SDIOPOWER_SET(on)	do { \
		if ((on) != 0) \
			HARDWARE_SDIO_POWER_S(HARDWARE_SDIO_POWER_BIT); \
		else \
			HARDWARE_SDIO_POWER_C(HARDWARE_SDIO_POWER_BIT); \
	} while (0)

#endif /* WITHSDHCHW */
/* Распределение битов в ARM контроллерах */

#if (WITHCAT && WITHCAT_USART1)
	// CAT data lites
	// RXD at P6_14, TXD at P6_15

	// CAT control lines
	#define FROMCAT_TARGET_PIN_RTS		(R7S721_INPUT_PORT(2)) // P2_0
	#define FROMCAT_BIT_RTS				(1u << 0)	/* сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	#define FROMCAT_TARGET_PIN_DTR		(R7S721_INPUT_PORT(2)) // P2_1
	#define FROMCAT_BIT_DTR				(1u << 1)	/* сигнал DTR от FT232RL	*/

	/* манипуляция от порта RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(FROMCAT_BIT_DTR); \
		} while (0)

	/* переход на передачу от порта RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(FROMCAT_BIT_RTS); \
		} while (0)

#endif /* (WITHCAT && WITHCAT_USART1) */

#if (WITHCAT && WITHCAT_CDC)
	// CAT data lites
	// RXD at P6_14, TXD at P6_15

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(R7S721_INPUT_PORT(2)) // P2_0
	//#define FROMCAT_BIT_RTS				(1u << 0)	/* сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(GPIO.PPR2) // P2_1
	//#define FROMCAT_BIT_DTR				(1u << 1)	/* сигнал DTR от FT232RL	*/

	/* манипуляция от порта RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* переход на передачу от порта RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_CDC) */

#if (WITHNMEA && WITHNMEA_USART1)
	// CAT data lites
	// RXD at P6_14, TXD at P6_15

	// CAT control lines
	#define FROMCAT_TARGET_PIN_RTS		(R7S721_INPUT_PORT(2)) // P2_0
	#define FROMCAT_BIT_RTS				(1u << 0)	/* сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	#define FROMCAT_TARGET_PIN_DTR		(R7S721_INPUT_PORT(2)) // P2_1
	#define FROMCAT_BIT_DTR				(1u << 1)	/* сигнал DTR от FT232RL	*/

	/* манипуляция от порта RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(FROMCAT_BIT_DTR); \
		} while (0)

	/* переход на передачу от порта RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(FROMCAT_BIT_RTS); \
		} while (0)

	/* сигнал PPS от GPS/GLONASS/GALILEO модуля */
	#define NMEA_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(FROMCAT_BIT_DTR); \
			arm_hardware_pio2_onchangeinterrupt(FROMCAT_BIT_DTR, FROMCAT_BIT_DTR, ARM_SYSTEM_PRIORITY, r7s721_nmea_interrupt); \
		} while (0)

#endif /* (WITHNMEA && WITHNMEA_USART1) */

#if WITHUART1HW
	#define HARDWARE_USART1_INITIALIZE() do { \
			arm_hardware_pio6_alternative(1U << 14, R7S721_PIOALT_5);	/* P6_14: RXD0: RX DATA line */ \
			arm_hardware_pio6_alternative(1U << 15, R7S721_PIOALT_5);	/* P6_15: TXD0: TX DATA line */ \
		} while (0)
#endif /* WITHUART1HW */

#if WITHUART2HW
	#define HARDWARE_USART2_INITIALIZE() do { \
			arm_hardware_pio7_alternative(1U << 10, R7S721_PIOALT_5);	/* P7_10: RXD3: RX DATA line */ \
			arm_hardware_pio7_alternative(1U << 11, R7S721_PIOALT_5);	/* P7_11: TXD3: TX DATA line */ \
		} while (0)
#endif /* WITHUART1HW */

#if WITHTX

	// txpath outputs not used
	////#define TXPATH_TARGET_PORT_S(v) do {	R7S721_TARGET_PORT_S(7, v); } while (0)
	////#define TXPATH_TARGET_PORT_C(v) do {	R7S721_TARGET_PORT_C(7, v); } while (0)
	// 
	#define TXGFV_RX		(1u << 4)
	#define TXGFV_TRANS		0			// переход между режимами приёма и передачи
	#define TXGFV_TX_SSB	(1u << 0)
	#define TXGFV_TX_CW		(1u << 1)
	#define TXGFV_TX_AM		(1u << 2)
	#define TXGFV_TX_NFM	(1u << 3)

	#define TXPATH_INITIALIZE() \
		do { \
		} while (0)

	// PTT input - P2_2
	#define PTT_TARGET_PIN				(R7S721_INPUT_PORT(5))
	#define PTT_BIT_PTT					(1U << 0)		// P5_0
	// PTT2 input - P2_2
	#define PTT2_TARGET_PIN				(R7S721_INPUT_PORT(5))
	#define PTT2_BIT_PTT					(1U << 1)		// P5_1

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0 || (PTT2_TARGET_PIN & PTT2_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_pio5_inputs(PTT_BIT_PTT); \
			arm_hardware_pio5_inputs(PTT2_BIT_PTT); \
		} while (0)

	// TUNE input - P3_15
	#define TUNE_TARGET_PIN				(R7S721_INPUT_PORT(3))
	#define TUNE_BIT_TUNE				(1U << 15)		// P3_15
	#define HARDWARE_GET_TUNE() ((TUNE_TARGET_PIN & TUNE_BIT_TUNE) == 0)
	#define TUNE_INITIALIZE() \
		do { \
			arm_hardware_pio3_inputs(TUNE_BIT_TUNE); \
		} while (0)

#else /* WITHTX */
	#define TUNE_INITIALIZE() \
		do { \
		} while (0)
#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_TARGET_PIN			(R7S721_INPUT_PORT(4))
	#define ELKEY_BIT_LEFT				(1U << 1)		// P4_1
	#define ELKEY_BIT_RIGHT				(1U << 0)		// P4_0

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_pio4_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_S(v) do {	R7S721_TARGET_PORT_S(1, v); } while (0)
//#define SPI_IOUPDATE_PORT_C(v) do {	R7S721_TARGET_PORT_C(1, v); } while (0)
//#define SPI_IOUPDATE_BIT		(1U << 15)	// * PA15

/* Назначение адресов на SPI шине */
#define targetfpga1		(1U << 5)	// P2_5 FPGA control registers CS1
#define targetrtc1		(1U << 2)	// P2_2 RTC DS1305EN
#define targetext1		(1U << 1)	// P2_1 front panel SPI device (LCD)
#define targetxad2		(1U << 0)	// P2_0 external SPI device (PA BOARD ADC)
#define targetnvram		(1U << 9)	// P2_9 nvmem FM25L16B
#define targetctl1		(1U << 7)	// P2_7 board control registers chain
#define targetcodec1	(1U << 6)	// P2_6 on-board codec1 NAU8822L
#define targetadc1		(1U << 4) 	/* P2_4 ADC AD9246 chip select */
#define targetadc2		(1U << 3) 	/* P2_3 ADC MCP3208-BI/SL chip select */

#define targetuc1608	targetext1 
#define targetlcd		targetext1

// Здесь должны быть перечислены все биты формирования CS в устройстве.
#define SPI_ALLCS_BITS ( \
	targetfpga1	| /*	(1U << 5)	P2_5  FPGA control registers CS1 */ \
	targetrtc1	| /*	(1U << 2)	P2_2  RTC DS1305EN */ \
	targetext1	| /*	(1U << 1)	P2_1  external spi device (LCD) */ \
	targetxad2	| /*	(1U << 0)	P2_0  external spi device (PA BOARD ADC) */ \
	targetnvram	| /*	(1U << 9)	P2_9  nvmem FM25L16B */ \
	targetctl1	| /*	(1U << 7)	P2_7 board control registers chain */ \
	targetcodec1| /*	(1U << 6)	P2_6 on-board codec1 NAU8822L */ \
	targetadc1	| /*	(1U << 4) 	P2_4 ADC AD9246 chip select */ \
	targetadc2	| /*	(1U << 3) 	P2_3 ADC MCP3208-BI/SL chip select */ \
	0)

#define SPI_ALLCS_BITSNEG (targetrtc1)		// Выходы, активные при "1"

#if 0

	#define SPI_ADDRESS_PORT_S(v) do {	R7S721_TARGET_PORT_S(xx, v); } while (0)
	#define SPI_ADDRESS_PORT_C(v) do {	R7S721_TARGET_PORT_C(xx, v); } while (0)

	// Биты адреса для дешифратора SPI
	#define SPI_A0 ((1u << xx))			// * PE13 
	#define SPI_A1 ((1u << xx))			// * PE14 
	#define SPI_A2 ((1u << xx))			// * PE15 

	#define SPI_NAEN_PORT_S(v) do {	R7S721_TARGET_PORT_S(xx, v); } while (0)
	#define SPI_NAEN_PORT_C(v) do {	R7S721_TARGET_PORT_C(xx, v); } while (0)

	#define SPI_NAEN_BIT (1u << xx)		// * PE7 used

#endif


// Набор определений для работы без внешнего дешифратора
#define SPI_ALLCS_PORT_S(v) do {	R7S721_TARGET_PORT_S(2, v); } while (0)
#define SPI_ALLCS_PORT_C(v) do {	R7S721_TARGET_PORT_C(2, v); } while (0)

/* инициализация лиий выбора периферийных микросхем */
#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_pio2_outputs(SPI_ALLCS_BITS, SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG); \
	} while (0)

// reset кодека
////#define TARGET_CS4272_RESET_PORT_S(v)		do { R7S721_TARGET_PORT_S(7, v); } while (0)
////#define TARGET_CS4272_RESET_PORT_C(v)		do { R7S721_TARGET_PORT_C(7, v); } while (0)
////#define TARGET_CS4272_RESET_BIT		(1U << 2)	// PD2

// RSPI0 used
#define HW_SPIUSED (& RSPI1)
// MOSI & SCK port
#define SPI_TARGET_SCLK_PORT_S(v) do {	R7S721_TARGET_PORT_S(6, v); } while (0)
#define SPI_TARGET_SCLK_PORT_C(v) do {	R7S721_TARGET_PORT_C(6, v); } while (0)
#define	SPI_SCLK_BIT			(1U << 12)	// * P6_12 бит, через который идет синхронизация SPI RSPI1

#define SPI_TARGET_MOSI_PORT_S(v) do {	R7S721_TARGET_PORT_S(6, v); } while (0)
#define SPI_TARGET_MOSI_PORT_C(v) do {	R7S721_TARGET_PORT_C(6, v); } while (0)
#define	SPI_MOSI_BIT			(1U << 14)	// * P6_14 бит, через который идет вывод (или ввод в случае двунаправленного SPI).RSPI1

#define SPI_TARGET_MISO_PIN		(R7S721_INPUT_PORT(6))		// was PINA 
#define	SPI_MISO_BIT			(1U << 15)	// * P6_15 бит, через который идет ввод с SPI.RSPI1

	#define SPIIO_INITIALIZE() do { \
			arm_hardware_pio6_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
			arm_hardware_pio6_inputs(SPI_MISO_BIT); \
		} while (0)

#if WITHSPIHW

	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_pio6_alternative(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT, R7S721_PIOALT_3);	/* PIO disable */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_pio6_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT);	/* PIO enable */ \
		} while (0)

	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_pio6_alternative(SPI_MOSI_BIT, R7S721_PIOALT_3);	/* PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_pio6_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)

#endif /* WITHSPIHW */

#if 1 // WITHTWISW
	// открытый сток P1 by design
	// P1_6 - SCL3
	// P1_7 - SDA3
	#define TARGET_TWI_TWCK_PORT_C(v) do {	R7S721_TARGET_PORT_C(1, v); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do {	R7S721_TARGET_PORT_S(1, v); } while (0)
	#define TARGET_TWI_TWCK_PIN		(R7S721_INPUT_PORT(1))
	#define TARGET_TWI_TWCK		(1u << 6)		// P1_6 - SCL3

	#define TARGET_TWI_TWD_PORT_C(v) do {	R7S721_TARGET_PORT_C(1, v); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do {	R7S721_TARGET_PORT_S(1, v); } while (0)
	#define TARGET_TWI_TWD_PIN		(R7S721_INPUT_PORT(1))
	#define TARGET_TWI_TWD		(1u << 7)		// P1_7 - SDA3

	// Инициализация битов портов ввода-вывода для программной реализации I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_pio1_outputs(TARGET_TWI_TWCK | TARGET_TWI_TWD, TARGET_TWI_TWCK | TARGET_TWI_TWD); \
		} while (0) 
	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#define	TWIHARD_INITIALIZE() do { \
			arm_hardware_pio1_alternative(TARGET_TWI_TWCK | TARGET_TWI_TWD, R7S721_PIOALT_1);	/* */ \
		} while (0) 


#endif

#if 1 || WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { R7S721_TARGET_PORT_S(1, v); } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { R7S721_TARGET_PORT_C(1, v); } while (0)
	#define FPGA_NCONFIG_BIT		(1UL << 1)	/* P1_1 bit conneced to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	(R7S721_INPUT_PORT(1))
	#define FPGA_CONF_DONE_BIT		(1UL << 2)	/* P1_2 bit conneced to CONF_DONE pin ALTERA FPGA */

	#define FPGA_NSTATUS_INPUT		(R7S721_INPUT_PORT(1))
	#define FPGA_NSTATUS_BIT		(1UL << 3)	/* P1_3 bit conneced to NSTATUS pin ALTERA FPGA */

	#define FPGA_INIT_DONE_INPUT	(R7S721_INPUT_PORT(1))
	#define FPGA_INIT_DONE_BIT		(1UL << 0)	/* P1_0 bit conneced to INIT_DONE pin ALTERA FPGA */

	/* Проверяем, проинициализировалась ли FPGA (вошла в user mode). */
	/*
		After the option bit to enable INIT_DONE is programmed into the device (during the first
		frame of configuration data), the INIT_DONE pin goes low.
		When initialization is complete, the INIT_DONE pin is released and pulled high. 
		This low-to-high transition signals that the device has entered user mode.
	*/
	#define HARDWARE_FPGA_IS_USER_MODE() ((FPGA_INIT_DONE_INPUT & FPGA_INIT_DONE_BIT) != 0)

	/* Инициадизация выводов GPIO процессора для получения состояния и управлением загрузкой FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
			arm_hardware_pio1_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
			arm_hardware_pio1_inputs(FPGA_CONF_DONE_BIT); \
			arm_hardware_pio1_inputs(FPGA_NSTATUS_BIT); \
			arm_hardware_pio1_inputs(FPGA_INIT_DONE_BIT); \
		} while (0)

	#define HARDWARE_FPGA_RESET() do { \
			/*board_fpga_reset(); */ \
		} while (0)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR
	// Биты доступа к массиву коэффициентов FIR фильтра в FPGA
	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { R7S721_TARGET_PORT_C(3, v); } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { R7S721_TARGET_PORT_S(3, v); } while (0)
	#define TARGET_FPGA_FIR_CS_BIT (1U << 14)	/* P3_14 - fir clock */

	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { R7S721_TARGET_PORT_C(1, v); } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { R7S721_TARGET_PORT_S(1, v); } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (1U << 4)	/* P1_4 - fir1 WE */

	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { R7S721_TARGET_PORT_C(1, v); } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { R7S721_TARGET_PORT_S(1, v); } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT (1U << 5)	/* P1_5 - fir2 WE */

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
				arm_hardware_pio3_outputs(TARGET_FPGA_FIR_CS_BIT, TARGET_FPGA_FIR_CS_BIT); \
				arm_hardware_pio1_outputs(TARGET_FPGA_FIR1_WE_BIT, TARGET_FPGA_FIR1_WE_BIT); \
				arm_hardware_pio1_outputs(TARGET_FPGA_FIR2_WE_BIT, TARGET_FPGA_FIR2_WE_BIT); \
			} while (0)
#endif /* WITHDSPEXTFIR */

	/* получение состояния переполнения АЦП */
	#define TARGET_FPGA_OVF_BIT		(1u << 8)	// P3_8
	#define TARGET_FPGA_OVF_GET		((R7S721_INPUT_PORT(3) & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
	#define TARGET_FPGA_OVF_INITIALIZE() do { \
				arm_hardware_pio3_inputs(TARGET_FPGA_OVF_BIT); \
			} while (0)

#if WITHKEYBOARD
	/* P7_8: second encoder button with pull-up */
	//#define KBD_MASK (1U << 8)	// P7_8
	//#define KBD_TARGET_PIN (R7S721_INPUT_PORT(7))


	#define TARGET_ENC2BTN_BIT (1U << 8)	// P7_8 - second encoder button with pull-up
#if WITHENCODER2
	// P7_8
	#define TARGET_ENC2BTN_GET	((R7S721_INPUT_PORT(7) & TARGET_ENC2BTN_BIT) == 0)
#endif /* WITHENCODER2 */

	#define TARGET_POWERBTN_BIT (1U << 3)	// P5_3 - ~CPU_POWER_SW signal
#if WITHPWBUTTON
	// P5_3 - ~CPU_POWER_SW signal
	#define TARGET_POWERBTN_GET	((R7S721_INPUT_PORT(5) & TARGET_POWERBTN_BIT) == 0)
#endif /* WITHPWBUTTON */

	#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pio7_inputs(TARGET_ENC2BTN_BIT); \
			arm_hardware_pio5_inputs(TARGET_POWERBTN_BIT); \
		} while (0)

#else /* WITHKEYBOARD */

	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)

#endif /* WITHKEYBOARD */

	#define HARDWARE_ADC_INITIALIZE(ainmask) do { \
			arm_hardware_pio1_alternative((ainmask) << 8, R7S721_PIOALT_1);	/* P1_8..P1_15 - AN0..AN7 inputs */ \
		} while (0)

	#if LCDMODE_LQ043T3DX02K
		#define WITHLCDBACKLIGHTOFF	1	// Имеется управление включением/выключением подсветки дисплея
		#define WITHLCDBACKLIGHT	1	// Имеется управление яркостью дисплея
		#define WITHLCDBACKLIGHTMIN	0	// Нижний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
		#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры
	#elif LCDMODE_AT070TN90 || LCDMODE_AT070TNA2
		#define WITHLCDBACKLIGHTOFF	1	// Имеется управление включением/выключением подсветки дисплея
		#define WITHLCDBACKLIGHT	1	// Имеется управление яркостью дисплея
		#define WITHLCDBACKLIGHTMIN	0	// Нижний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTMAX	2	// Верхний предел регулировки (показываемый на дисплее)
		#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры
	#else
		/* Заглушка для работы без дисплея */
		#define WITHLCDBACKLIGHTMIN	0
	#endif

	#define	HARDWARE_BL_INITIALIZE() do { \
		const portholder_t enpins = (1U << 9); /* P7_9 */ \
		const portholder_t blpins = (1U << 3) | (1U << 2); /* P7_3:P7_2 */ \
		const portholder_t blstate = (~ (3) & 0x03) << 2; \
		arm_hardware_pio7_inputs(blpins & blstate); /* BL ADJ - open (open drain simulate) */ \
		arm_hardware_pio7_outputs(blpins & ~ blstate, 0); /* BL ADJ - grounded (open drain simulate) */ \
		arm_hardware_pio7_outputs(enpins, 0 ? enpins : 0);	/* BL ENABLE */ \
		} while (0)
	/* установка яркости и включение/выключение преобразователя подсветки */
	/* level: 0..3 */
	#define HARDWARE_BL_SET(en, level) do { \
		const portholder_t enpins = (1U << 9); /* P7_9 */ \
		const portholder_t blpins = (1U << 3) | (1U << 2); /* P7_3:P7_2 */ \
		const portholder_t blstate = (~ (level) & 0x03) << 2; \
		arm_hardware_pio7_inputs(blpins & blstate); /* BL ADJ - open (open drain simulate) */ \
		arm_hardware_pio7_outputs(blpins & ~ blstate, 0); /* BL ADJ - grounded (open drain simulate) */ \
		arm_hardware_pio7_outputs(enpins, en ? enpins : 0);	/* BL ENABLE */ \
	} while (0)

#if WITHDCDCFREQCTL
	// ST ST1S10 Synchronizable switching frequency from 400 kHz up to 1.2 MHz
	#define WITHHWDCDCFREQMIN 400000L
	#define WITHHWDCDCFREQMAX 1200000L

	#define	HARDWARE_DCDC_INITIALIZE() do { \
		hardware_dcdcfreq_tioc0a_mtu0_initialize(); \
		arm_hardware_pio2_alternative((1U << 8), R7S721_PIOALT_3);	/* P2_8 TIOC0A (MTU0 output) */ \
	} while (0)
	#define HARDWARE_DCDC_SETDIV(f) do { \
		hardware_dcdcfreq_tioc0a_mtu0_setdiv(f); \
	} while (0)
#else /* WITHDCDCFREQCTL */
	#define	HARDWARE_DCDC_INITIALIZE() do { \
	} while (0)
	#define HARDWARE_DCDC_SETDIV(f) do { \
		(void) (f); \
	} while (0)
#endif /* WITHDCDCFREQCTL */

#if LCDMODE_LTDC

	#define LS020_RESET_PORT_S(v) do {	R7S721_TARGET_PORT_S(7, v); } while (0)
	#define LS020_RESET_PORT_C(v) do {	R7S721_TARGET_PORT_C(7, v); } while (0)
	#define LS020_RESET			(1u << 1)			// * P7_1 D6 NRESET

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	/* Table 34.9 Bit Allocation of RGB Signal Input for RGB565 Output */ 
	#define HARDWARE_LTDC_INITIALIZE(demode) do { \
		const uint32_t MODE = (1U << 0); /* P7_0 MODE */ \
		const uint32_t DE = (1U << 7); /* P7_0 DE */ \
		const uint32_t VS = (1U << 5); /* P7_5 VSYNC */ \
		const uint32_t HS = (1U << 6); /* P7_6 HSYNC */ \
		arm_hardware_pio7_outputs(MODE, (demode != 0) * MODE);	/* P7_0 MODE=demode */ \
		/* Synchronisation signals */ \
		arm_hardware_pio7_alternative((1U << 4), R7S721_PIOALT_6);	/* P7_4 CLK LCD0_CLK */ \
		/* MODE=1: DE MODE */ \
		arm_hardware_pio7_outputs((demode != 0) * VS, VS);	/* P7_5 VSYNC */ \
		arm_hardware_pio7_outputs((demode != 0) * HS, HS);	/* P7_6 HSYNC */ \
		arm_hardware_pio7_alternative((demode != 0) * DE, R7S721_PIOALT_6); 	/* P7_7 DE */ \
		/* MODE=0: SYNC MODE */ \
		arm_hardware_pio7_alternative((demode == 0) * VS, R7S721_PIOALT_6);	/* P7_5 VSYNC */ \
		arm_hardware_pio7_alternative((demode == 0) * HS, R7S721_PIOALT_6);	/* P7_6 HSYNC */ \
		arm_hardware_pio7_outputs((demode == 0) * DE, 0);	/* P7_7 DE=0 */ \
		/* BLUE */ \
		arm_hardware_pio3_alternative((1U << 0), R7S721_PIOALT_3);	/* P3_0 LCD0_DATA0 B3 */ \
		arm_hardware_pio3_alternative((1U << 1), R7S721_PIOALT_3);	/* P3_1 LCD0_DATA1 B4 */ \
		arm_hardware_pio3_alternative((1U << 2), R7S721_PIOALT_3);	/* P3_2 LCD0_DATA2 B5 */ \
		arm_hardware_pio3_alternative((1U << 3), R7S721_PIOALT_3);	/* P3_3 LCD0_DATA3 B6 */ \
		arm_hardware_pio3_alternative((1U << 4), R7S721_PIOALT_3);	/* P3_4 LCD0_DATA4 B7 */ \
		/* GREEN */ \
		arm_hardware_pio3_alternative((1U << 5), R7S721_PIOALT_3);	/* P3_5 LCD0_DATA5 G2 */ \
		arm_hardware_pio3_alternative((1U << 6), R7S721_PIOALT_3);	/* P3_6 LCD0_DATA6 G3 */ \
		arm_hardware_pio3_alternative((1U << 7), R7S721_PIOALT_3);	/* P3_7 LCD0_DATA7 G4 */ \
		arm_hardware_pio6_alternative((1U << 0), R7S721_PIOALT_2);	/* P6_0 LCD0_DATA8 G5 */ \
		arm_hardware_pio6_alternative((1U << 1), R7S721_PIOALT_2);	/* P6_1 LCD0_DATA9 G6 */ \
		arm_hardware_pio6_alternative((1U << 2), R7S721_PIOALT_2);	/* P6_2 LCD0_DATA10 G7 */ \
		/* RED */ \
		arm_hardware_pio6_alternative((1U << 3), R7S721_PIOALT_2);	/* P6_3 LCD0_DATA11 R3 */ \
		arm_hardware_pio6_alternative((1U << 4), R7S721_PIOALT_2);	/* P6_4 LCD0_DATA12 R4 */ \
		arm_hardware_pio6_alternative((1U << 5), R7S721_PIOALT_2);	/* P6_5 LCD0_DATA13 R5 */ \
		arm_hardware_pio6_alternative((1U << 6), R7S721_PIOALT_2);	/* P6_6 LCD0_DATA14 R6 */ \
		arm_hardware_pio6_alternative((1U << 7), R7S721_PIOALT_2);	/* P6_7 LCD0_DATA15 R7 */ \
	} while (0)

	/* управление состоянием сигнала DISP панели */
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_SET_DISP(demode, state) do { \
		if (demode != 0) break; \
		const uint32_t mask = (1U << 7); /* P7_7 */ \
		arm_hardware_pio7_outputs(mask, (state != 0) * mask);	/* P7_7 DE=state */ \
	} while (0)
	/* управление состоянием сигнала MODE панели */
	#define HARDWARE_LTDC_SET_MODE(state) do { \
		const uint32_t MODE = (1U << 0); /* P7_0 MODE */ \
		arm_hardware_pio7_outputs(MODE, (state != 0) * MODE);	/* P7_0 MODE=demode */ \
	} while (0)


	#define LS020_RS_INITIALIZE() \
		do { \
		} while (0)

	#define LS020_RESET_INITIALIZE() \
		do { \
			arm_hardware_pio7_outputs(LS020_RESET, LS020_RESET); \
		} while (0)

	#define LS020_RS_SET(v) do { \
		} while (0)
	#define LS020_RESET_SET(v) do { \
			if ((v) != 0) LS020_RESET_PORT_S(LS020_RESET); \
			else  LS020_RESET_PORT_C(LS020_RESET); \
		} while (0)

#else /* LCDMODE_LTDC */

	//#define WRITEE_BIT				(1u << 12)	// RD/~WR  P3_12 - должен быть в "0" - как при записи - для управления буферами на шине данных LCD

	#define LS020_RS_INITIALIZE() \
		do { \
			arm_hardware_pio7_outputs(LS020_RS, LS020_RS); \
		} while (0)

	#define LS020_RESET_INITIALIZE() \
		do { \
			arm_hardware_pio7_outputs(LS020_RESET, LS020_RESET); \
		} while (0)

	#define LS020_RS_SET(v) do { \
			if ((v) != 0) LS020_RS_PORT_S(LS020_RS); \
			else  LS020_RS_PORT_C(LS020_RS); \
		} while (0)
	#define LS020_RESET_SET(v) do { \
			if ((v) != 0) LS020_RESET_PORT_S(LS020_RESET); \
			else  LS020_RESET_PORT_C(LS020_RESET); \
		} while (0)

#endif /* LCDMODE_LTDC */

// Signal P3_9 control
#if WITHFLATLINK && LCDMODE_LTDC
	// SN75LVDS83B FlatLink™ Transmitter shutdown control
	// #SHTDN is a CMOS IN with pull down resistor approx. 100..200 kOhm
	#define HARDWARE_LVDSTX_INITIALIZE() do { \
		const uint32_t mask = (1U << 9); /* P3_9 */ \
		arm_hardware_pio3_outputs(mask, 1 * mask); /* 0 - Transmitter off, 1 - Transmitter work */ \
	} while (0)
#else /* WITHFLATLINK && LCDMODE_LTDC */
	// SN75LVDS83B FlatLink™ Transmitter shutdown control
	// #SHTDN is a CMOS IN with pull down resistor approx. 100..200 kOhm
	#define HARDWARE_LVDSTX_INITIALIZE() do { \
		const uint32_t mask = (1U << 9); /* P3_9 */ \
		arm_hardware_pio3_outputs(mask, 0 * mask); /* 0 - Transmitter off, 1 - Transmitter work */ \
	} while (0)
#endif /* WITHFLATLINK && LCDMODE_LTDC */

	#define HARDWARE_VBUS_ON_MASK (1U << 2)	/* P5_2 ~VBUS_ON */

	#define TARGET_USBFS_VBUSON_SET(on)	do { \
		if ((on) != 0) \
			arm_hardware_pio5_outputs(HARDWARE_VBUS_ON_MASK, 0 * HARDWARE_VBUS_ON_MASK);	/* P5_2 ~VBUS_ON = 1*/ \
		else \
			arm_hardware_pio5_outputs(HARDWARE_VBUS_ON_MASK, 1 * HARDWARE_VBUS_ON_MASK);	/* P5_2 ~VBUS_ON = 0 */ \
	} while (0)

	#define HARDWARE_USB0_INITIALIZE() do { \
			arm_hardware_pio5_outputs(HARDWARE_VBUS_ON_MASK, 1 * HARDWARE_VBUS_ON_MASK);	/* P5_2 ~VBUS_ON = 1*/ \
			local_delay_ms(200); \
		} while (0)

	#define HARDWARE_USB1_INITIALIZE() do { \
		} while (0)

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
		HARDWARE_DCDC_INITIALIZE(); \
		HARDWARE_KBD_INITIALIZE(); \
		HARDWARE_FPGA_LOADER_INITIALIZE(); \
		HARDWARE_FPGA_RESET(); \
		TUNE_INITIALIZE(); \
		HARDWARE_USB0_INITIALIZE(); \
		HARDWARE_USB1_INITIALIZE(); \
		HARDWARE_BL_INITIALIZE(); \
		HARDWARE_LVDSTX_INITIALIZE(); \
		} while (0)


	// Bootloader parameters
	#define BOOTLOADER_RAMAREA Renesas_RZ_A1_ONCHIP_SRAM_BASE	/* адрес ОЗУ, куда перемещать application */
	#define BOOTLOADER_RAMSIZE (1024uL * 1024uL * 2)	// 2M
	#define BOOTLOADER_RAMPAGESIZE	(1024uL * 1024)	// при загрузке на исполнение используется размер страницы в 1 мегабайт
	#define USBD_DFU_RAM_XFER_SIZE 4096

	#define BOOTLOADER_FLASHSIZE (1024uL * 1024uL * 2)	// 2M FLASH CHIP
	#define BOOTLOADER_SELFBASE Renesas_RZ_A1_SPI_IO0	/* адрес где лежит во FLASH образ application */
	#define BOOTLOADER_SELFSIZE (1024uL * 128)	// 128k

	#define BOOTLOADER_APPBASE (BOOTLOADER_SELFBASE + BOOTLOADER_SELFSIZE)	/* адрес где лежит во FLASH образ application */
	#define BOOTLOADER_APPSIZE (BOOTLOADER_FLASHSIZE - BOOTLOADER_SELFSIZE)	// 2048 - 128

	#define BOOTLOADER_PAGESIZE (1024uL * 64)	// M25Px with 64 KB pages
	#define USBD_DFU_FLASH_XFER_SIZE 256	// match to (Q)SPI FLASH MEMORY page size
	#define USBD_DFU_FLASHNAME "M25P16"

	#if WIHSPIDFSW
		// P4_2: SPBIO20_0 WP#
		// P4_3: SPBIO30_0 HOLD#
		// P4_4: SPBCLK_0 SCLK
		// P4_5: SPBSSL_0 CS#
		// P4_6: SPBIO00_0 MOSI
		// P4_7: SPBIO10_0 MISO

		#define SPIDF_MISO() ((R7S721_INPUT_PORT(4) & (1U << 7)) != 0)
		#define SPIDF_MOSI(v) do { if (v) R7S721_TARGET_PORT_S(4, (1U << 6)); else R7S721_TARGET_PORT_C(4, (1U << 6)); } while (0)
		#define SPIDF_SCLK(v) do { if (v) R7S721_TARGET_PORT_S(4, (1U << 4)); else R7S721_TARGET_PORT_C(4, (1U << 4)); } while (0)
		#define SPIDF_SOFTINITIALIZE() do { \
				arm_hardware_pio4_outputs(1U << 2, 1U << 2);				/* P4_2 WP / SPBIO20_0 */ \
				arm_hardware_pio4_outputs(1U << 3, 1U << 3);				/* P4_3 NC / SPBIO30_0 */ \
				/* arm_hardware_pio4_alternative(1U << 4, R7S721_PIOALT_2);	*/ /* P4_4 SCLK / SPBCLK_0 */ \
				arm_hardware_pio4_outputs(1U << 4, 1U << 4);	/* P4_4 SCLK / SPBCLK_0 */ \
				/* arm_hardware_pio4_alternative(1U << 5, R7S721_PIOALT_2);	*/ /* P4_5 CS# / SPBSSL_0 */ \
				arm_hardware_pio4_outputs(1U << 5, 1 * (1U << 5));			/* P4_5 CS# / SPBSSL_0 */ \
				/* arm_hardware_pio4_alternative(1U << 6, R7S721_PIOALT_2);	*/ /* P4_6 MOSI / SPBIO00_0 */ \
				arm_hardware_pio4_outputs(1U << 6, 1U << 6);	/* P4_6 MOSI / SPBIO00_0 */ \
				/* arm_hardware_pio4_alternative(1U << 7, R7S721_PIOALT_2);	*/ /* P4_7 MISO / SPBIO10_0 */ \
				arm_hardware_pio4_inputs(1U << 7);	/* P4_7 MISO / SPBIO10_0 */ \
			} while (0)
		#define SPIDF_HANGOFF() do { \
				arm_hardware_pio4_inputs(0xFC); /* Отключить процессор от SERIAL FLASH */ \
			} while (0)
		#define SPIDF_SELECT() do { \
				arm_hardware_pio4_outputs(0x7C, 0x7C); \
				R7S721_TARGET_PORT_C(4, (1U << 5)); \
			} while (0)
		#define SPIDF_UNSELECT() do { \
				R7S721_TARGET_PORT_S(4, (1U << 5)); \
				arm_hardware_pio4_inputs(0xFC); /* Отключить процессор от SERIAL FLASH */ \
			} while (0)

	#elif WIHSPIDFHW
		// P4_2: SPBIO20_0 WP#
		// P4_3: SPBIO30_0 HOLD#
		// P4_4: SPBCLK_0 SCLK
		// P4_5: SPBSSL_0 CS#
		// P4_6: SPBIO00_0 MOSI
		// P4_7: SPBIO10_0 MISO

		#define SPIDF_HARDINITIALIZE() do { \
				arm_hardware_pio4_alternative(1U << 2, R7S721_PIOALT_2);	/* P4_2 WP / SPBIO20_0 */ \
				arm_hardware_pio4_alternative(1U << 3, R7S721_PIOALT_2);	/* P4_3 NC / SPBIO30_0 */ \
				arm_hardware_pio4_alternative(1U << 4, R7S721_PIOALT_2);	/* P4_4 SCLK / SPBCLK_0 */ \
				arm_hardware_pio4_alternative(1U << 6, R7S721_PIOALT_2);	/* P4_6 MOSI / SPBIO00_0 */ \
				arm_hardware_pio4_alternative(1U << 7, R7S721_PIOALT_2);	/* P4_7 MISO / SPBIO10_0 */ \
				arm_hardware_pio4_alternative(1U << 5, R7S721_PIOALT_2);	/* P4_5 CS# / SPBSSL_0 */ \
			} while (0)
		#define SPIDF_HANGOFF() do { \
				arm_hardware_pio4_inputs(0xFC); /* Отключить процессор от SERIAL FLASH */ \
			} while (0)

	#endif /* WIHSPIDFSW */

#endif /* ARM_R7S72_TQFP176_CPUSTYLE_STORCH_V8_H_INCLUDED */
