/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Трансивер с DSP обработкой "Storch" на процессоре Renesas R7S721020VCFP с кодеком NAU8822L и FPGA EP4CE22E22I7N
// Rmainunit_v7cm.pcb

#ifndef ARM_R7S72_TQFP176_CPUSTYLE_RAVEN_V2_H_INCLUDED
#define ARM_R7S72_TQFP176_CPUSTYLE_RAVEN_V2_H_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1		// US0:
//#define HARDWARE_ARM_USEUSART1 1		// US1: PA9/PA10 pins

#define WITHSPI16BIT	1		/* возможно использование 16-ти битных слов при обмене по SPI */
#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
//#define WITHSPISW 	1	/* Использование программного управления SPI. */

//#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */

#define WITHI2SHW	1	/* Использование SSIF0 I2S 2*16 bit - аудио кодек */
#define WITHSAI1HW	1	/* Использование SSIF1 I2S 8*32 bit - FPGA IF codec */
#if WITHRTS192
	#define WITHSAI2HW	1	/* Использование SSIF2 I2S 2*32 (2*32) bit - FPGA панорама	*/
#endif /* WITHRTS192 */

//#define WITHCPUDACHW	1	/* использование DAC - в renesas R7S72 нету */
#define WITHCPUADCHW 	1	/* использование ADC */

#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */

#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
#define WITHUSBHWVBUSSENSE	1	/* используется предопределенный вывод VBUS_SENSE */
#define WITHDEVONHIGHSPEED	1	/* Для DEVICE используется встроенная в процессор поддержка USB HS */
#define WITHHIGHSPEEDDESC	1	/* Требуется формировать дескрипторы как для HIGH SPEED */
#define WITHUSBHW_DEVICE	(& USB200)	/* на этом устройстве поддерживается функциональность DEVUCE	*/

#define WITHUART1HW	1	/* Используется периферийный контроллер последовательного порта #1 SCIF0 */
//#define WITHUART2HW	1	/* Используется периферийный контроллер последовательного порта #2 SCIF3 */

//#define WITHCAT_USART1	1
#define WITHCAT_CDC			1	/* использовать виртуальный последовательный порт на USB соединении */
//#define WITHMODEM_CDC		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART1		1	/* порт подключения GPS/GLONASS */

#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */
#define WITHUSBCDC		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */

#define WRITEE_BIT				(1u << 12)	// RD/~WR  P3_12 - должен быть в "0" - как при записи - для управления буферами на шине данных LCD

/* В данной версии платы ошибка в схеме - для работы преобразователей уровня "наружу" WRITEE_BIT долже быть в "1".
*/
#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_pio3_outputs(WRITEE_BIT, 0); /* в новой версии платы выдавать "0" */ \
		arm_hardware_pio3_outputs(LS020_RS, LS020_RS); \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		arm_hardware_pio3_outputs(WRITEE_BIT, 0); /* в новой версии платы выдавать "0" */ \
		arm_hardware_pio3_outputs(LS020_RST, LS020_RST); \
	} while (0)

#define LS020_RS_SET(v) do { \
		if ((v) != 0) LS020_RS_PORT_S(LS020_RS); \
		else  LS020_RS_PORT_C(LS020_RS); \
	} while (0)

#define LS020_RST_SET(v) do { \
		if ((v) != 0) LS020_RST_PORT_S(LS020_RST); \
		else  LS020_RST_PORT_C(LS020_RST); \
	} while (0)

#if LCDMODE_SPI_NA
	// эти контроллеры требуют только RS

	#define LS020_RS_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LS020_RS_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)
	#define LS020_RS			(1u << 11)			// P3_11 D7 signal in HD44780 socket

#elif LCDMODE_SPI_RN
	// эти контроллеры требуют только RESET

	#define LS020_RESET_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LS020_RESET_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)
	#define LS020_RST			(1u << 10)			// * P3_10 D6 signal in HD44780 socket

#elif LCDMODE_SPI_RA
	// Эти контроллеры требуют RESET и RS

	#define LS020_RS_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LS020_RS_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)
	#define LS020_RS			(1u << 11)			// P3_11 D7 signal in HD44780 socket

	#define LS020_RESET_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LS020_RESET_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)
	#define LS020_RST			(1u << 10)			// P3_10 D6 signal in HD44780 socket

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	// Выводы подключения ЖКИ индикатора WH2002 или аналогичного HD44780.
	#define LCD_DATA_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)		// P3_11,,P3_8
	#define LCD_DATA_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)		// P3_11,,P3_8

	#define LCD_DATA_INPUT			(R7S721_INPUT_PORT(3))

	// E (enable) bit
	#define LCD_STROBE_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LCD_STROBE_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)

	// RS bit
	#define LCD_RS_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LCD_RS_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)

	// W bit
	#define LCD_WE_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LCD_WE_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)

	#define LCD_STROBE_BIT			(1u << 14)	// E: P3_14

	#define WRITEE_BIT				(1u << 12)	// RD/~WR  P3_12
	//#define WRITEE_BIT_ZERO				(1u << 12)	// RD/~WR  P3_12
	
	#define ADDRES_BIT				(1u << 13)	// * P3_13

	#define LCD_DATAS_BITS			((1u << 11) | (1u << 10) | (1u << 9) | (1u << 8))	// P3_11,,P3_8
	#define LCD_DATAS_BIT_LOW		8		// какой бит данных младший в слове считанном с порта

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* получить данные с шины LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { /* выдача данных (не сдвинуьых) */ \
			const portholder_t t = (portholder_t) (v) << LCD_DATAS_BIT_LOW; \
			LCD_DATA_PORT_S(LCD_DATAS_BITS & t); \
			LCD_DATA_PORT_C(LCD_DATAS_BITS & ~ t); \
		} while (0)

	/* инициализация управляющих выходов процессора для управления HD44780 - полный набор выходов */
	#define LCD_CONTROL_INITIALIZE() \
		do { \
			arm_hardware_pio3_outputs(LCD_STROBE_BIT | WRITEE_BIT | ADDRES_BIT, 0); \
		} while (0)
	/* инициализация управляющих выходов процессора для управления HD44780 - WE=0 */
	#define LCD_CONTROL_INITIALIZE_WEEZERO() \
		do { \
			arm_hardware_pio3_outputs(LCD_STROBE_BIT | WRITEE_BIT_ZERO | ADDRES_BIT, 0); \
		} while (0)
	/* инициализация управляющих выходов процессора для управления HD44780 - WE отсутствует - сигнал к индикатору заземлён */
	#define LCD_CONTROL_INITIALIZE_WEENONE() \
		do { \
			arm_hardware_pio3_outputs(LCD_STROBE_BIT | ADDRES_BIT, 0); \
		} while (0)

	#define LCD_DATA_INITIALIZE_READ() \
		do { \
			arm_hardware_pio3_inputs(LCD_DATAS_BITS);	/* переключить порт на чтение с выводов */ \
		} while (0)

	#define LCD_DATA_INITIALIZE_WRITE(v) \
		do { \
			arm_hardware_pio3_outputs(LCD_DATAS_BITS, (v) << LCD_DATAS_BIT_LOW);	/* открыть выходы порта */ \
		} while (0)

#endif

#if 0 && LCDMODE_UC1608
	#define UC1608_CSP_PORT_S(v) do {	R7S721_TARGET_PORT_S(7, v); } while (0)
	#define UC1608_CSP_PORT_C(v) do {	R7S721_TARGET_PORT_C(7, v); } while (0)
	#define UC1608_CSP			(1u << 15)			// * PE15
	#define SPI_CSEL255			255				// по этому чипселекту выбираем положительным сигналом

	#define UC1608_CSP_INITIALIZE() do { \
			arm_hardware_pio1_outputs(UC1608_CSP, 0); \
		} while (0)
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

	#define ENCODER_INPUT_PORT			(R7S721_INPUT_PORT(5)) 
	#define ENCODER_BITA (1u << 11)	// P5_11 IRQ6
	#define ENCODER_BITB (1u << 10)	// P5_10 IRQ5
	#define ENCODER_BITS (ENCODER_BITA | ENCODER_BITB)
	#define ENCODER_SHIFT 10	// Отсутствие этого значения означает что биты не подряд

	#define ENCODER2_INPUT_PORT			(R7S721_INPUT_PORT(5)) 
	#define ENCODER2_BITA (1u << 9)	// P5_9 IRQ4
	#define ENCODER2_BITB (1u << 8)	// P5_8 IRQ3
	#define ENCODER2_BITS (ENCODER2_BITA | ENCODER2_BITB)
	#define ENCODER2_SHIFT 8	// Отсутствие этого значения означает что биты не подряд

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_pio5_alternative(ENCODER_BITS, R7S721_PIOALT_4); \
			arm_hardware_irqn_interrupt(5, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt); /* IRQ5, both edges */ \
			arm_hardware_irqn_interrupt(6, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt); /* IRQ6, both edges */ \
			arm_hardware_pio5_alternative(ENCODER2_BITS, R7S721_PIOALT_4); \
			/*arm_hardware_irqn_interrupt(3, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt2); *//* IRQ3, both edges */ \
			/*arm_hardware_irqn_interrupt(4, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt2); *//* IRQ4, both edges */ \
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
#endif /* WITHSAI1HW */

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

	// PTT input - P2_2 (было P6_12)
	#define PTT_TARGET_PIN				(R7S721_INPUT_PORT(2))
	#define PTT_BIT_PTT					(1U << 2)		// P2_2

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(PTT_BIT_PTT); \
		} while (0)

#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_TARGET_PIN			(R7S721_INPUT_PORT(6))
	#define ELKEY_BIT_LEFT				(1U << 13)		// P6_13
	#define ELKEY_BIT_RIGHT				(1U << 12)		// P6_12

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_pio6_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
		} while (0)

#endif /* WITHELKEY */

#if WITHUSBHW
	#define USB_INITIALIZE() \
		do { \
		} while (0)

#endif /* WITHUSBHW */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_S(v) do {	R7S721_TARGET_PORT_S(1, v); } while (0)
//#define SPI_IOUPDATE_PORT_C(v) do {	R7S721_TARGET_PORT_C(1, v); } while (0)
//#define SPI_IOUPDATE_BIT		(1U << 15)	// * PA15

/* Назначение адресов на SPI шине */
#define targetdds1_u	(1U << 2)	// P7_2  FPGA NCO registers CS
#define targetfpga1		(1U << 6)	// P7_6  FPGA control registers CS1
#define targetrtc1		(1U << 7)	// P7_7  RTC DS1305EN
#define targetext1		(1U << 8)	// P7_8  external spi device (LCD)
#define targetnvram		(1U << 9)	// P7_9  nvmem FM25L16B
#define targetctl1		(1U << 10)	// P7_10 board control registers chain
#define targetcodec1	(1U << 11)	// P7_11 on-board codec1 NAU8822L

#define targetuc1608	targetext1 

// Здесь должны быть перечислены все биты формирования CS в устройстве.
#define SPI_ALLCS_BITS ( \
	targetdds1_u	| /* P7_2  FPGA NCO registers CS */ \
	targetfpga1		| /* P7_6  FPGA control registers CS1 */ \
	targetrtc1		| /* P7_7  RTC DS1305EN */ \
	targetext1		| /* P7_8  external spi device (LCD) */ \
	targetnvram		| /* P7_9  nvmem FM25L16B */ \
	targetctl1		| /* P7_10 board control registers chain */ \
	targetcodec1	| /* P7_11 on-board codec1 NAU8822L */ \
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
#define SPI_ALLCS_PORT_S(v) do {	R7S721_TARGET_PORT_S(7, v); } while (0)
#define SPI_ALLCS_PORT_C(v) do {	R7S721_TARGET_PORT_C(7, v); } while (0)

/* инициализация лиий выбора периферийных микросхем */
#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_pio7_outputs(SPI_ALLCS_BITS, SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG); \
	} while (0)

// reset кодека
////#define TARGET_CS4272_RESET_PORT_S(v)		do { R7S721_TARGET_PORT_S(7, v); } while (0)
////#define TARGET_CS4272_RESET_PORT_C(v)		do { R7S721_TARGET_PORT_C(7, v); } while (0)
////#define TARGET_CS4272_RESET_BIT		(1U << 2)	// PD2

// RSPI0 used
#define HW_SPIUSED (& RSPI0)
// MOSI & SCK port
#define SPI_TARGET_SCLK_PORT_S(v) do {	R7S721_TARGET_PORT_S(6, v); } while (0)
#define SPI_TARGET_SCLK_PORT_C(v) do {	R7S721_TARGET_PORT_C(6, v); } while (0)
#define	SPI_SCLK_BIT			(1U << 0)	// * P6_0 бит, через который идет синхронизация SPI

#define SPI_TARGET_MOSI_PORT_S(v) do {	R7S721_TARGET_PORT_S(6, v); } while (0)
#define SPI_TARGET_MOSI_PORT_C(v) do {	R7S721_TARGET_PORT_C(6, v); } while (0)
#define	SPI_MOSI_BIT			(1U << 2)	// * P6_2 бит, через который идет вывод (или ввод в случае двунаправленного SPI).

#define SPI_TARGET_MISO_PIN		(R7S721_INPUT_PORT(6))		// was PINA 
#define	SPI_MISO_BIT			(1U << 3)	// * P6_3 бит, через который идет ввод с SPI.

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

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { R7S721_TARGET_PORT_S(6, v); } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { R7S721_TARGET_PORT_C(6, v); } while (0)

	#define FPGA_NCONFIG_BIT		(1UL << 5)	/* P6_5 bit conneced to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	(R7S721_INPUT_PORT(6))
	#define FPGA_NSTATUS_INPUT		(R7S721_INPUT_PORT(6))

	#define FPGA_CONF_DONE_BIT		(1UL << 4)	/* P6_4 bit conneced to CONF_DONE pin ALTERA FPGA */
	#define FPGA_NSTATUS_BIT		(1UL << 6)	/* P6_6 bit conneced to NSTATUS pin ALTERA FPGA */

	/* Инициадизация выводов GPIO процессора для получения состояния и управлением загрузкой FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
			arm_hardware_pio6_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
			arm_hardware_pio6_inputs(FPGA_CONF_DONE_BIT | FPGA_NSTATUS_BIT); \
		} while (0)

	/* по косвенным признакам проверяем, проинициализировалась ли FPGA (вошла в user mode). */
	#define HARDWARE_FPGA_IS_USER_MODE() (local_delay_ms(400), 1)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR
	// Биты доступа к массиву коэффициентов FIR фильтра в FPGA
	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { R7S721_TARGET_PORT_C(5, v); } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { R7S721_TARGET_PORT_S(5, v); } while (0)
	#define TARGET_FPGA_FIR_CS_BIT (1U << 15)	/* P5_15 - fir clock */

	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { R7S721_TARGET_PORT_C(7, v); } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { R7S721_TARGET_PORT_S(7, v); } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (1U << 0)	/* P7_0 - fir1 WE */

	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { R7S721_TARGET_PORT_C(7, v); } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { R7S721_TARGET_PORT_S(7, v); } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT (1U << 1)	/* P7_1 - fir2 WE */

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
				arm_hardware_pio7_outputs(TARGET_FPGA_FIR1_WE_BIT, TARGET_FPGA_FIR1_WE_BIT); \
				arm_hardware_pio7_outputs(TARGET_FPGA_FIR2_WE_BIT, TARGET_FPGA_FIR2_WE_BIT); \
				arm_hardware_pio5_outputs(TARGET_FPGA_FIR_CS_BIT, TARGET_FPGA_FIR_CS_BIT); \
			} while (0)
#endif /* WITHDSPEXTFIR */

	/* получение состояния переполнения АЦП */
	#define TARGET_FPGA_OVF_BIT		(1u << 1)	// P6_1
	#define TARGET_FPGA_OVF_GET		((R7S721_INPUT_PORT(6) & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
	#define TARGET_FPGA_OVF_INITIALIZE() do { \
				arm_hardware_pio6_inputs(TARGET_FPGA_OVF_BIT); \
			} while (0)

	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		#define KBD_TARGET_PIN (R7S721_INPUT_PORT(1))

		#define KBD_MASK 0 //(0x01 | 0x02)	// все используемые биты
		#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pio1_inputs(KBD_MASK); \
			} while (0)
	#endif

	#define HARDWARE_ADC_INITIALIZE(ainmask) do { \
			arm_hardware_pio1_alternative((ainmask) << 8, R7S721_PIOALT_1);	/* P1_8..P1_15 - AN0..AN7 inputs */ \
		} while (0)

	#define HARDWARE_DAC_INITIALIZE() do { \
		} while (0)

	#define HARDWARE_SIDETONE_INITIALIZE() do { \
		} while (0)

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
		HARDWARE_SIDETONE_INITIALIZE(); \
		HARDWARE_KBD_INITIALIZE(); \
		HARDWARE_DAC_INITIALIZE(); \
		} while (0)

#endif /* ARM_R7S72_TQFP176_CPUSTYLE_RAVEN_V2_H_INCLUDED */
