/* $Id$ */
/* board-specific CPU attached signals */
/*
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
 *
 * Трансивер с DSP обработкой "Аист" на плате Alinx AXU2CGA + AD9640 board, Petalinux, by RA4ASN
 *
 */

#ifndef ARM_XCZUXX_CPUSTYLE_AXU2CGA_AD9640_H_INCLUDED
#define ARM_XCZUXX_CPUSTYLE_AXU2CGA_AD9640_H_INCLUDED 1

//#define WITHSPI16BIT	1	/* возможно использование 16-ти битных слов при обмене по SPI */
//#define WITHSPI32BIT	1	/* возможно использование 32-ти битных слов при обмене по SPI */
//#define WITHSPIHW 		1	/* Использование аппаратного контроллера SPI */
//#define WITHSPIHWDMA 	1	/* Использование DMA при обмене по SPI */
#define WITHSPISW 	1	/* Использование программного управления SPI. Нельзя убирать эту строку - требуется явное отключение из-за конфликта с I2C */
//#define WITHDMA2DHW		1	/* Использование DMA2D для формирования изображений	- у STM32MP1 его нет */

#define WITHTWIHW 	1	/* Использование аппаратного контроллера TWI (I2C) */
//#define WITHTWISW 	1	/* Использование программного контроллера TWI (I2C) */

#if WITHINTEGRATEDDSP
	//#define WITHI2S2HW	1	/* Использование I2S - аудиокодек на I2S2 и I2S2_alt или I2S2 и I2S3	*/
	//#define WITHSAI1HW	1	/* Использование SAI1 - FPGA или IF codec	*/
	//#define WITHSAI2HW	1	/* Использование SAI2 - FPGA или IF codec	*/
	//#define WITHSAI3HW	1	/* Использование SAI3 - FPGA скоростной канал записи спктра	*/
#endif /* WITHINTEGRATEDDSP */

//#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
//#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */
#define USERFIRSTSBLOCK 0

enum {
	XGPI0,	// C13 encoder2 bit A
	XGPI1,	// B10 encoder2 bit B
	XGPI2,	// C14 encoder2 button
	XGPI3,	// B8 spi_miso
//	XGPI4,	// C8 ptt_in
};

enum {
	XGPO0,	// A7 cs_ctrl_int
	XGPO1,	// C6 spi_mosi
	XGPO2,	// A6 spi_sck
};

#define AXI_IQ_FTW_ADDR			0x8004a000
#define AXI_IQ_FTW_SUB_ADDR		0x80045000
#define AXI_IQ_RTS_ADDR			0x80042000
#define AXI_MODEM_CTRL_ADDR		0x80049000
#define AXI_IQ_FIFO_RX_ADDR		0x80041000
#define AXI_IQ_FIFO_TX_ADDR		0x8004c000
#define AXI_XGPI_ADDR			0x80046000
#define AXI_XGPO_ADDR			0x80047000
#define AXI_IQ_RX_COUNT_ADDR	0x80044000
#define AXI_IQ_TX_COUNT_ADDR	0x8004d000
#define AXI_ADI_ADDR			0x80040000
#define AXI_FIFO_PHONES_ADDR	0x80043000
#define AXI_FIFO_MIC_ADDR		0x8004e000
#define AXI_FIR_RELOAD_ADDR		0x8004b000
#define AXI_DCDC_PWM_ADDR		0x80048000

#define CALIBRATION_IQ_FIR_RX_SHIFT		51
#define CALIBRATION_IQ_CIC_RX_SHIFT		58
#define CALIBRATION_TX_SHIFT			25

//#define WITHUART2HW	1	/*	Используется периферийный контроллер последовательного порта UART1 */
//#define WITHUARTFIFO	1	/* испольование FIFO */

//#define WITHCAT_USART1		1
//#define WITHDEBUG_USART2	1
//#define WITHNMEA_USART2		1	/* порт подключения GPS/GLONASS */
//#define WITHETHHW 1	/* Hardware Ethernet controller */

#define LINUX_NMEA_FILE		"/dev/ttyPS1"
#define LINUX_IQ_INT_FILE	"/dev/uio0"
#define LINUX_PPS_INT_FILE	"/dev/uio1"
#define LINUX_I2C_FILE		"/dev/i2c-0"
#define LINUX_FB_FILE		"/dev/fb0"
#define LINUX_TTY_FILE		"/dev/tty0"

#if WITHISBOOTLOADER

	#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
	//#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
	//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 2-м проводам */
	#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 4-м проводам */

	#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
	#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER в 4-bit bus width */

	#define WITHSDRAMHW	1		/* В процессоре есть внешняя память */
	//#define WITHSDRAM_PMC1	1	/* power management chip */

	//#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHGPUHW	1	/* Graphic processor unit */
	//#define WITHEHCIHW	1	/* USB_EHCI controller */
	//#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */

	//#define WITHUSBHW_DEVICE	USB_OTG_HS	/* на этом устройстве поддерживается функциональность DEVICE	*/
	#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1
	#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USBH_HS_DP & USBH_HS_DM
	//#define WITHUSBDEV_DMAENABLE 1

	/* For H7 exist: Legacy defines */
	//#define USB_OTG_HS                   USB1_OTG_HS
	//#define USB_OTG_FS                   USB2_OTG_FS

	//#define WITHUSBHW_HOST		USB_OTG_HS
	#define WITHUSBHOST_HIGHSPEEDULPI	1
	//#define WITHUSBHOST_DMAENABLE 1


	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHMODEM_CDC	1

	//#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */
	//#define WITHUSBUACIN2		1	/* формируются три канала передачи звука */
	//#define WITHUABUACOUTAUDIO48MONO	1	/* для уменьшения размера буферов в endpoints */

	//#define WITHUSBCDCACM		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
	//#define WITHUSBCDCACM_N	1	/* количество виртуальных последовательных портов */
    //#define WITHUSBCDCACMINTSHARING 1    /* Использование общей notification endpoint на всех CDC ACM устрйоствах */
	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */
	#define WITHUSBDFU	1	/* DFU USB Device Firmware Upgrade support */
	#define WITHMOVEDFU 1	// Переместить интерфейс DFU в область меньших номеров. Утилита dfu-util 0.9 не работает с DFU на интерфейсе с индексом 10
	#define WITHUSBWCID	1

	//#define WITHLWIP 1
	//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
	//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
	//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */

#else /* WITHISBOOTLOADER */

	//#define WIHSPIDFSW	1	/* программное обслуживание DATA FLASH */
	//#define WIHSPIDFHW		1	/* аппаратное обслуживание DATA FLASH */
	//#define WIHSPIDFHW2BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 2-м проводам */
	//#define WIHSPIDFHW4BIT	1	/* аппаратное обслуживание DATA FLASH с подддержкой QSPI подключения по 4-м проводам */

	//#define WITHMDMAHW		1	/* Использование MDMA для формирования изображений */
	//#define WITHCPUDACHW	1	/* использование встроенного в процессор DAC */
	//#define WITHCPUADCHW 	1	/* использование встроенного в процессор ADC */

	#define WITHLTDCHW		1	/* Наличие контроллера дисплея с framebuffer-ом */
	//#define WITHGPUHW	1	/* Graphic processor unit */
	//#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */

	//#define WITHUSBHW_DEVICE	USB_OTG_HS	/* на этом устройстве поддерживается функциональность DEVICE	*/
	//#define WITHUSBDEV_VBUSSENSE	1		/* используется предопределенный вывод OTG_VBUS */
	//#define WITHUSBDEV_HSDESC	1			/* Требуется формировать дескрипторы как для HIGH SPEED */
	//#define WITHUSBDEV_HIGHSPEEDULPI	1
	//#define WITHUSBDEV_HIGHSPEEDPHYC	1	// UTMI -> USB_DP2 & USB_DM2
	//#define WITHUSBDEV_DMAENABLE 1

#if 0
	#define WITHUSBHW	1	/* Используется встроенная в процессор поддержка USB */
	#define WITHEHCIHW	1	/* USB_EHCI controller */
	#define WITHUSBHW_EHCI		EHCI0
	#define WITHEHCIHW_EHCIPORT 0	// 0 - use 1st PHY port, 1 - 2nd PHY port.
	#define WITHUSBHOST_HIGHSPEEDULPI	1
	//#define WITHUSBHOST_DMAENABLE 1	// not need for EHCI
#endif


	#define WITHCAT_CDC		1	/* использовать виртуальный последовательный порт на USB соединении */
	#define WITHMODEM_CDC	1

	#if 0//WITHINTEGRATEDDSP

		#define WITHUSBUAC		1	/* использовать виртуальную звуковую плату на USB соединении */
		#if WITHRTS96 || WITHRTS192
			#define WITHUSBUACIN2		1	/* формируются три канала передачи звука */
		#endif /* WITHRTS96 || WITHRTS192 */
		//#define WITHUABUACOUTAUDIO48MONO	1	/* для уменьшения размера буферов в endpoints */
	#endif /* WITHINTEGRATEDDSP */

	//#define WITHUSBCDCACM		1	/* ACM использовать виртуальный последовательный порт на USB соединении */
	//#define WITHUSBCDCACM_N	2	/* количество виртуальных последовательных портов */
    //#define WITHUSBCDCACMINTSHARING 1    /* Использование общей notification endpoint на всех CDC ACM устрйоствах */
	#if WITHLWIP
		//#define WITHUSBCDCEEM	1	/* EEM использовать Ethernet Emulation Model на USB соединении */
		//#define WITHUSBCDCECM	1	/* ECM использовать Ethernet Control Model на USB соединении */
		//#define WITHUSBRNDIS	1	/* RNDIS использовать Remote NDIS на USB соединении */
	#endif /* WITHLWIP */
	//#define WITHUSBHID	1	/* HID использовать Human Interface Device на USB соединении */

//	#define WITHUSBDFU	1	/* DFU USB Device Firmware Upgrade support */
//	#define WITHMOVEDFU 1	// Переместить интерфейс DFU в область меньших номеров. Утилита dfu-util 0.9 не работает с DFU на интерфейсе с индексом 10
//	#define WITHUSBWCID	1

#endif /* WITHISBOOTLOADER */

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_GT911)

	void gt911_interrupt_handler(void);

#if TSC_RESET_BY_REG
	void board_tsc_reset_state(uint_fast8_t v);
	#define GT911_RESET(v) 			do { board_tsc_reset_state(v); board_update(); } while (0)
	#define GT911_RESET_INIT(v) 	do { board_tsc_reset_state(1); board_update(); } while (0)
#else
#define BOARD_GT911_RESET_PIN 	XGPO0
	#define BOARD_GT911_RESET_SET(v) do { if (v) gpio_writepin(BOARD_GT911_RESET_PIN, 1); else gpio_writepin(BOARD_GT911_RESET_PIN, 0);  } while (0)
	#define GT911_RESET_INIT(v) {}
#endif /* TSC_RESET_BY_REG */

	#define BOARD_GT911_RESET_SET(v) do { GT911_RESET(v); } while (0)
	#define BOARD_GT911_RESET_INITIO_1() do { } while (0)
	#define BOARD_GT911_RESET_INITIO_2() do { } while (0)
	#define BOARD_GT911_INT_SET(v) do { } while (0)
	#define BOARD_GT911_INT_CONNECT() do { } while (0)

#endif

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_ILI2102)

	#define BOARD_ILI2102_INT_PIN 	64
	#define BOARD_ILI2102_RESET_PIN 65

	#define BOARD_ILI2102_RESET_SET(v) do { if (v) xc7z_writepin(BOARD_ILI2102_RESET_PIN, 1); else xc7z_writepin(BOARD_ILI2102_RESET_PIN, 0);  } while (0)
	#define BOARD_ILI2102_INT_SET(v) do { if (v) xc7z_writepin(BOARD_ILI2102_INT_PIN, 1); else xc7z_writepin(BOARD_ILI2102_INT_PIN, 0); } while (0)

#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_ILI2102) */

#if WITHCPUTEMPERATURE
	#define GET_CPU_TEMPERATURE() (xczu_get_cpu_temperature())
#endif /* WITHCPUTEMPERATURE */

//#define PREAMP_MIO	35

#define LS020_RS_INITIALIZE() \
	do { \
		/*arm_hardware_piod_outputs2m(LS020_RS, LS020_RS); *//* PD3 */ \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		/*arm_hardware_piod_outputs2m(LS020_RESET, LS020_RESET); *//* PD4 */ \
	} while (0)

#define LS020_RS_SET(v) do { \
		if ((v) != 0) LS020_RS_PORT_S(LS020_RS); \
		else  LS020_RS_PORT_C(LS020_RS); \
	} while (0)

#define LS020_RESET_SET(v) do { \
		if ((v) != 0) LS020_RESET_PORT_S(LS020_RESET); \
		else  LS020_RESET_PORT_C(LS020_RESET); \
	} while (0)

#if LCDMODE_SPI_NA
	// эти контроллеры требуют только RS

	#define LS020_RS_PORT_S(v)		do { /* GPIOD->BSRR = BSRR_S(v); */__DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { /* GPIOD->BSRR = BSRR_C(v); */__DSB(); } while (0)
	#define LS020_RS			(1u << 3)			// PD3 signal

#elif LCDMODE_SPI_RN
	// эти контроллеры требуют только RESET

	#define LS020_RESET_PORT_S(v)		do {  } while (0)
	#define LS020_RESET_PORT_C(v)		do {  } while (0)
	#define LS020_RESET			(1u << 4)			// PD4 signal

#elif LCDMODE_SPI_RA
	// Эти контроллеры требуют RESET и RS
	// LCDMODE_UC1608

	#define LS020_RS_PORT_S(v)		do { /* GPIOD->BSRR = BSRR_S(v);  */__DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { /* GPIOD->BSRR = BSRR_C(v);  */__DSB(); } while (0)
	#define LS020_RS			(1u << 3)			// PD3 signal

	#define LS020_RESET_PORT_S(v)		do { /* GPIOD->BSRR = BSRR_S(v);  */__DSB(); } while (0)
	#define LS020_RESET_PORT_C(v)		do { /* GPIOD->BSRR = BSRR_C(v);  */__DSB(); } while (0)
	#define LS020_RESET			(1u << 2)			// PD4 signal

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	#error Unsupported LCDMODE_HD44780

#endif

#if WITHENCODER

	// Выводы подключения енкодера #1

	// Выводы подключения енкодера #2
	#define ENCODER2_BITA		XGPI0
	#define ENCODER2_BITB		XGPI1

	#define ENCODER_INITIALIZE() \
		do { \
		} while (0)
#endif

#if WITHI2S2HW
	// Инициализируются I2S2 в дуплексном режиме.
	#define I2S2HW_INITIALIZE() do { \
		SPI2->CFG2 |= SPI_CFG2_IOSWP; \
		arm_hardware_piob_altfn2(1uL << 12,	AF_SPI2); /* PB12 I2S2_WS	*/ \
		arm_hardware_piob_updown(0, 1uL << 12); \
		arm_hardware_piob_altfn2(1uL << 13,	AF_SPI2); /* PB13 I2S2_CK	*/ \
		arm_hardware_piob_updown(0, 1uL << 13); \
		arm_hardware_piob_altfn2(1uL << 15,	AF_SPI2); /* PB15 I2S2_SDO - передача */ \
		arm_hardware_piob_updown(0, 1uL << 15); \
		arm_hardware_piob_altfn2(1uL << 14,	AF_SPI2); /* PB14 I2S2_SDI, - приём от кодека */ \
		arm_hardware_piob_updown(0, 1uL << 14); \
	} while (0)
#endif /* WITHI2S2HW */

#if WITHSAI1HW
	/*
	 *
	 */
	#define SAI1HW_INITIALIZE()	do { \
		/*arm_hardware_pioe_altfn20(1uL << 2, AF_SAI); */	/* PE2 - SAI1_MCK_A - 12.288 MHz	*/ \
		arm_hardware_pioe_altfn2(1uL << 4,	AF_SAI);			/* PE4 - SAI1_FS_A	- 48 kHz	*/ \
		arm_hardware_pioe_altfn20(1uL << 5,	AF_SAI);			/* PE5 - SAI1_SCK_A	*/ \
		arm_hardware_pioe_altfn2(1uL << 6,	AF_SAI);			/* PE6 - SAI1_SD_A	(i2s data to codec)	*/ \
		arm_hardware_pioe_altfn2(1uL << 3,	AF_SAI);			/* PE3 - SAI1_SD_B	(i2s data from codec)	*/ \
		arm_hardware_pioe_updown(1uL << 3, 0); \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI2HW
	/* 
	Поскольку блок SAI2 инициализируется как SLAVE с синхронизацией от SAI1,
	из внешних сигналов требуется только SAI2_SD_A
	*/
	#define SAI2HW_INITIALIZE()	do { \
		arm_hardware_pioe_altfn2(1uL << 11, AF_SAI2);	/* PE11 - SAI2_SD_B	(i2s data from FPGA)	*/ \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI3HW
	/*
	*/
	#define SAI3HW_INITIALIZE()	do { \
		arm_hardware_piod_altfn50(1uL << 12, AF_SAI2); 		/* PD12 - SAI2_FS_A	- WS from FPGA	*/ \
		arm_hardware_piod_altfn50(1uL << 13, AF_SAI2); 		/* PD13 - SAI2_SCK_A	*/ \
		arm_hardware_pioe_altfn50(1uL << 11, AF_SAI2);		/* PE11 - SAI2_SD_B	(i2s data from FPGA)	*/ \
	} while (0)
#endif /* WITHSAI1HW */

/* Распределение битов в ARM контроллерах */

#if (WITHCAT && WITHCAT_USART2)
	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(GPIOA->IDR) 
	//#define FROMCAT_BIT_RTS				(1u << 11)	/* PA11 сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(GPIOA->IDR)
	//#define FROMCAT_BIT_DTR				(1u << 12)	/* PA12 сигнал DTR от FT232RL	*/

	/* манипуляция от порта RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* переход на передачу от порта RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_USART2) */

#if (WITHCAT && WITHCAT_CDC)

	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	//#define FROMCAT_TARGET_PIN_RTS		(GPIOA->IDR) // was PINA 
	//#define FROMCAT_BIT_RTS				(1u << 11)	/* сигнал RTS от FT232RL	*/

	/* манипуляция от порта RS-232, сигнал PPS от GPS/GLONASS/GALILEO модуля */
	//#define FROMCAT_TARGET_PIN_DTR		(GPIOA->IDR) // was PINA 
	//#define FROMCAT_BIT_DTR				(1u << 12)	/* сигнал DTR от FT232RL	*/

	/* манипуляция от виртуального CDC порта */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* переход на передачу от виртуального CDC порта*/
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_CDC) */

#if WITHSDHCHW

	// J11
	// SD0 signals
	//	PS_MIO40_CD_CLK
	//	PS_MIO41_CD_CMD
	//	PS_MIO42_CD_D0
	//	PS_MIO43_CD_D1
	//	PS_MIO44_CD_D2
	//	PS_MIO45_CD_D3
	//	--- PS_MIO46_CD_SW
	//	--- PS_MIO50_CD_WP

	// return: 0 - no disk
	#define HARDWARE_SDIOSENSE_CD() 1//((SD0->PRESENT_STATE & (1uL << 18)) != 0) /* получить состояние датчика CARD PRESENT */
	// return: ! 0 - write protect
	#define HARDWARE_SDIOSENSE_WP() 0//((SD0->PRESENT_STATE & (1uL << 19)) == 0) /* получить состояние датчика CARD WRITE PROTECT */

	#define HARDWARE_SDIO_HANGOFF() do { \
		} while (0)
	// SD0 signals
	//	PS_MIO40_CD_CLK
	//	PS_MIO41_CD_CMD
	//	PS_MIO42_CD_D0
	//	PS_MIO43_CD_D1
	//	PS_MIO44_CD_D2
	//	PS_MIO45_CD_D3
	// 46 SDIO 0 CD Select
	// 50 SDIO 0 WP Select
	//EMIT_MASKWRITE(0XF8000830, 0x003F003FU ,0x00380037U),	// SD0_WP_CD_SEL
	#define HARDWARE_SDIO_INITIALIZE() do { \
			SCLR->SD0_WP_CD_SEL = \
					(46uL << 16) |	/* 46 SDIO 0 CD Select */ \
					(50uL << 0) |	/* 50 SDIO 0 WP Select */ \
					0; \
			MIO_SET_MODE(40, 0x00001680uL);	/*  PS_MIO40_CD_CLK */ \
			MIO_SET_MODE(41, 0x00001680uL);	/*  PS_MIO41_CD_CMD */ \
			MIO_SET_MODE(42, 0x00001680uL);	/*  PS_MIO42_CD_D0 */ \
			MIO_SET_MODE(43, 0x00001680uL);	/*  PS_MIO43_CD_D1 */ \
			MIO_SET_MODE(44, 0x00001680uL);	/*  PS_MIO44_CD_D2 */ \
			MIO_SET_MODE(45, 0x00001680uL);	/*  PS_MIO45_CD_D3 */ \
		} while (0)
	#define HARDWARE_SDIOSENSE_INITIALIZE() do { \
		} while (0)
	#define HARDWARE_SDIOPOWER_INITIALIZE() do { \
		} while (0)

#endif /* WITHSDHCHW */


#if WITHSDHCHW && 0
	#if WITHSDHCHW4BIT
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(1uL << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(1uL << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(1uL << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_altfn50(1uL << 9, AF_SDIO);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_altfn50(1uL << 10, AF_SDIO);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_altfn50(1uL << 11, AF_SDIO);	/* PC11 - SDIO_D3	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(1uL << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(1uL << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(1uL << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_inputs(1uL << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_inputs(1uL << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_inputs(1uL << 11);	/* PC11 - SDIO_D3	*/ \
			arm_hardware_piod_updown(0, 1uL << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(0, 1uL << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(0, 1uL << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_pioc_updown(0, 1uL << 9);	/* PC9 - SDIO_D1	*/ \
			arm_hardware_pioc_updown(0, 1uL << 10);	/* PC10 - SDIO_D2	*/ \
			arm_hardware_pioc_updown(0, 1uL << 11);	/* PC11 - SDIO_D3	*/ \
		} while (0)
	#else /* WITHSDHCHW4BIT */
		#define HARDWARE_SDIO_INITIALIZE()	do { \
			arm_hardware_piod_altfn50(1uL << 2, AF_SDIO);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_altfn50(1uL << 12, AF_SDIO);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_altfn50(1uL << 8, AF_SDIO);	/* PC8 - SDIO_D0	*/ \
		} while (0)
		/* отключить процессор от SD карты - чтобы при выполнении power cycle не возникало фантомное питание через сигналы управления. */
		#define HARDWARE_SDIO_HANGOFF()	do { \
			arm_hardware_piod_inputs(1uL << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_inputs(1uL << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_inputs(1uL << 8);	/* PC8 - SDIO_D0	*/ \
			arm_hardware_piod_updown(0, 1uL << 2);	/* PD2 - SDIO_CMD	*/ \
			arm_hardware_pioc_updown(0, 1uL << 12);	/* PC12 - SDIO_CK	*/ \
			arm_hardware_pioc_updown(0, 1uL << 8);	/* PC8 - SDIO_D0	*/ \
		} while (0)
	#endif /* WITHSDHCHW4BIT */

	#define HARDWARE_SDIO_WP_BIT	(1U << 8)	/* PG8 - SDIO_WP */
	#define HARDWARE_SDIO_CD_BIT	(1U << 7)	/* PG7 - SDIO_SENSE */

	#define HARDWARE_SDIOSENSE_INITIALIZE()	do { \
			arm_hardware_piog_inputs(HARDWARE_SDIO_WP_BIT); /* PD1 - SDIO_WP */ \
			arm_hardware_piog_updown(HARDWARE_SDIO_WP_BIT, 0); \
			arm_hardware_piog_inputs(HARDWARE_SDIO_CD_BIT); /* PD0 - SDIO_SENSE */ \
			arm_hardware_piog_updown(HARDWARE_SDIO_CD_BIT, 0); \
	} while (0)

	#define HARDWARE_SDIOSENSE_CD() ((GPIOG->IDR & HARDWARE_SDIO_CD_BIT) == 0)	/* получить состояние датчика CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() ((GPIOG->IDR & HARDWARE_SDIO_WP_BIT) != 0)	/* получить состояние датчика CARD WRITE PROTECT */

	#define HARDWARE_SDIOPOWER_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define HARDWARE_SDIOPOWER_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define HARDWARE_SDIOPOWER_BIT (1u << 7)	/* PC7 */
	/* если питание SD CARD управляется прямо с процессора */
	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		arm_hardware_pioc_outputs2m(HARDWARE_SDIOPOWER_BIT, HARDWARE_SDIOPOWER_BIT); /* питание выключено */ \
		} while (0)
	/* parameter on not zero for powering SD CARD */
	#define HARDWARE_SDIOPOWER_SET(on)	do { \
		if ((on) != 0) \
			HARDWARE_SDIOPOWER_C(HARDWARE_SDIOPOWER_BIT); \
		else \
			HARDWARE_SDIOPOWER_S(HARDWARE_SDIOPOWER_BIT); \
	} while (0)

#endif /* WITHSDHCHW */

#if WITHTX

	// txpath outputs not used
	////#define TXPATH_TARGET_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	////#define TXPATH_TARGET_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
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


	// +++
	// TXDISABLE input - PD10
	#define TXDISABLE_TARGET_PIN				1//(GPIOD->IDR)
	#define TXDISABLE_BIT_TXDISABLE				0//(1U << 10)		// PD10 - TX INHIBIT
	// получить бит запрета передачи (от усилителя мощности)
	#define HARDWARE_GET_TXDISABLE() (0) //((TXDISABLE_TARGET_PIN & TXDISABLE_BIT_TXDISABLE) != 0)
	#define TXDISABLE_INITIALIZE() \
		do { \
		} while (0)
	// ---

	// +++
	// PTT input - PD10
	// PTT2 input - PD9
	#define PTT_TARGET_PIN				1//(GPIOD->IDR)
	#define PTT_BIT_PTT					1//(1uL << 10)		// PD10 - PTT
	#define PTT2_TARGET_PIN				1//(GPIOD->IDR)
	#define PTT2_BIT_PTT				1//(1uL << 9)		// PD9 - PTT2
	// получить бит запроса оператором перехода на пердачу
	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0 || (PTT2_TARGET_PIN & PTT2_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
		} while (0)
	// ---

	#define HARDWARE_GET_TUNE() (0)
	#define TUNE_INITIALIZE() \
		do { } while (0)

#else /* WITHTX */

	#define TXDISABLE_INITIALIZE() \
		do { \
		} while (0)
	#define PTT_INITIALIZE() \
		do { \
		} while (0)
	#define TUNE_INITIALIZE() \
		do { \
		} while (0)
#endif /* WITHTX */

#if WITHELKEY
	// Electronic key inputs
	#define ELKEY_BIT_LEFT				(1uL << 14)		// PD14
	#define ELKEY_BIT_RIGHT				(1uL << 15)		// PD15

	#define ELKEY_TARGET_PIN			(GPIOD->IDR)

	#define HARDWARE_GET_ELKEY_LEFT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_LEFT) == 0)
	#define HARDWARE_GET_ELKEY_RIGHT() 	((ELKEY_TARGET_PIN & ELKEY_BIT_RIGHT) == 0)


	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_piod_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_piod_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
//#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
//#define SPI_IOUPDATE_BIT		(1uL << 15)	// * PA15

#if WITHSPIHW || WITHSPISW

	#define targetctl1		XGPO0

	#define CS_BY_I2C		1
	#define PCF8575CTS_ADDR	0x40	// A0=0, A1=0, A2=0

	#define targetext1		(1 << 8)
	#define targetext2		(2 << 8)
	#define targetnvram		(3 << 8)

	/* Select specified chip. */
	#define SPI_CS_ASSERT(target)	do { \
		cs_i2c_assert(target); \
	} while (0)

	/* Unelect specified chip. */
	#define SPI_CS_DEASSERT(target)	do { \
		cs_i2c_deassert(target); \
	} while (0)

	#define SPI_ALLCS_DISABLE() \
		do { \
			cs_i2c_disable();		\
		} while(0)

	/* инициализация линий выбора периферийных микросхем */
	#define SPI_ALLCS_INITIALIZE() \
		do { \
		} while (0)

	// MOSI & SCK port
	#define	SPI_SCLK_MIO 	XGPO2
	#define	SPI_MOSI_MIO 	XGPO1
	#define	SPI_MISO_MIO 	XGPI3

	#define SPI_SCLK_C()	do { gpio_writepin(SPI_SCLK_MIO, 0); } while (0)
	#define SPI_SCLK_S()	do { gpio_writepin(SPI_SCLK_MIO, 1); } while (0)

	#define SPI_MOSI_C()	do { gpio_writepin(SPI_MOSI_MIO, 0); } while (0)
	#define SPI_MOSI_S()	do { gpio_writepin(SPI_MOSI_MIO, 1); } while (0)

	#define SPI_TARGET_MISO_PIN		(gpio_readpin(SPI_MISO_MIO))

	#define SPIIO_INITIALIZE() do { \
		} while (0)

	#define HARDWARE_SPI_CONNECT() do { \
		} while (0)

	#define HARDWARE_SPI_DISCONNECT() do { \
		} while (0)

	#define HARDWARE_SPI_CONNECT_MOSI() do { \
		} while (0)

	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
		} while (0)

#endif /* WITHSPIHW || WITHSPISW */

#if WITHUART2HW
	// RXD: U3.V13
	// TXD: U3.U12
	// little board
	// WITHUART1HW
	#define HARDWARE_UART2_INITIALIZE() do { \
		MIO_SET_MODE(24, 0x000016E0uL);	/*  MIO_PIN_24 UART1_TXD */ \
		MIO_SET_MODE(25, 0x000016E1uL);	/*  MIO_PIN_25 UART1_RXD */ \
		} while (0)

#endif /* WITHUART2HW */

#if WITHKEYBOARD

#if WITHENCODER2
	#define TARGET_ENC2BTN_GET (gpio_readpin(XGPI2) == 0)
#endif /* WITHENCODER2 */

#if WITHPWBUTTON
	// P5_3 - ~CPU_POWER_SW signal
#define TARGET_POWERBTN_BIT 0//(1U << 8)	// PAxx - ~CPU_POWER_SW signal
	#define TARGET_POWERBTN_GET	0//(((GPIOx->IDR) & TARGET_POWERBTN_BIT) == 0)
#endif /* WITHPWBUTTON */

	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)

#else /* WITHKEYBOARD */

	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)

#endif /* WITHKEYBOARD */

#if WITHTWISW
	#define TARGET_TWI_TWCK_MIO			58		// EMIO 58
	#define TARGET_TWI_TWD_MIO			57		// EMIO 57

	// Инициализация битов портов ввода-вывода для аппаратной реализации I2C
	// присоединение выводов к периферийному устройству
	#define	TWIHARD_INITIALIZE() do { \
		/*gpio_peripherial(TARGET_TWI_TWD_MIO, pinmode);*/	/*  PS_MIO43_501 SDA */ \
		/*gpio_peripherial(TARGET_TWI_TWCK_MIO, pinmode);*/	/*  PS_MIO42_501 SCL */ \
		/* i2chw_initialize(); */ \
	} while (0)


#endif // WITHTWISW

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define FPGA_NCONFIG_BIT		(1UL << 11)	/* PC11 bit conneced to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	(GPIOC->IDR)
	#define FPGA_CONF_DONE_BIT		(1UL << 10)	/* PC10 bit conneced to CONF_DONE pin ALTERA FPGA */

	#define FPGA_NSTATUS_INPUT		(GPIOC->IDR)
	#define FPGA_NSTATUS_BIT		(1UL << 9)	/* PC9 bit conneced to NSTATUS pin ALTERA FPGA */

	#define FPGA_INIT_DONE_INPUT	(GPIOC->IDR)
	#define FPGA_INIT_DONE_BIT		(1UL << 12)	/* PC12 bit conneced to INIT_DONE pin ALTERA FPGA */

	/* Инициадизация выводов GPIO процессора для получения состояния и управлением загрузкой FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
			arm_hardware_pioc_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
			arm_hardware_pioc_inputs(FPGA_NSTATUS_BIT); \
			arm_hardware_pioc_inputs(FPGA_CONF_DONE_BIT); \
			arm_hardware_pioc_inputs(FPGA_INIT_DONE_BIT); \
		} while (0)

	/* Проверяем, проинициализировалась ли FPGA (вошла в user mode). */
	/*
		After the option bit to enable INIT_DONE is programmed into the device (during the first
		frame of configuration data), the INIT_DONE pin goes low.
		When initialization is complete, the INIT_DONE pin is released and pulled high. 
		This low-to-high transition signals that the device has entered user mode.
	*/
	#define HARDWARE_FPGA_IS_USER_MODE() (local_delay_ms(100), (FPGA_INIT_DONE_INPUT & FPGA_INIT_DONE_BIT) != 0)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR

#define AXIDMA_FIR_COEFFS_ID	XPAR_AXI_DMA_FIR_RELOAD_DEVICE_ID

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
		} while (0)
#endif /* WITHDSPEXTFIR */

#if 0
	/* получение состояния переполнения АЦП */
	#define TARGET_FPGA_OVF_INPUT		(GPIOC->IDR)
	#define TARGET_FPGA_OVF_BIT			(1u << 8)	// PC8
	#define TARGET_FPGA_OVF_GET			((TARGET_FPGA_OVF_INPUT & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
	#define TARGET_FPGA_OVF_INITIALIZE() do { \
				arm_hardware_pioc_inputs(TARGET_FPGA_OVF_BIT); \
			} while (0)
#endif

#if WITHCPUDACHW
	/* включить нужные каналы */
	#define HARDWARE_DAC_INITIALIZE() do { \
			DAC1->CR = DAC_CR_EN1; /* DAC1 enable */ \
		} while (0)
	#define HARDWARE_DAC_ALC(v) do { /* вывод 12-битного значения на ЦАП - канал 1 */ \
			DAC1->DHR12R1 = (v); /* DAC1 set value */ \
		} while (0)

#else /* WITHCPUDACHW */
	#define HARDWARE_DAC_INITIALIZE() do { \
		} while (0)

#endif /* WITHCPUDACHW */

#if WITHCPUADCHW
	#define HARDWARE_ADC_INITIALIZE(ainmask) do { \
			arm_hardware_pioa_analoginput(((ainmask) >> 0) & 0xff);	/* ADC12_IN0..ADC12_IN7 */ \
			arm_hardware_piob_analoginput(((ainmask) >> 8) & 0x03);	/* ADC12_IN8..ADC12_IN0 */ \
			arm_hardware_pioc_analoginput(((ainmask) >> 10) & 0x3f);	/* ADC12_IN10..ADC12_IN15 */ \
		} while (0)
#endif /* WITHCPUADCHW */

#if WITHUSBHW
	#define TARGET_USBFS_VBUSON_PORT_C(v)	do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_USBFS_VBUSON_PORT_S(v)	do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_USBFS_VBUSON_BIT (1uL << 2)	// PD2 - нулём включение питания для device
	/**USB_OTG_FS GPIO Configuration    
	PA9     ------> USB_OTG_FS_VBUS
	PA10     ------> USB_OTG_FS_ID
	PA11     ------> USB_OTG_FS_DM
	PA12     ------> USB_OTG_FS_DP 
	*/
	#define	USBD_FS_INITIALIZE() do { \
		arm_hardware_piod_outputs(TARGET_USBFS_VBUSON_BIT, TARGET_USBFS_VBUSON_BIT); /* PD2 */ \
		} while (0)

	#define TARGET_USBFS_VBUSON_SET(on)	do { \
		if ((on) != 0) \
			TARGET_USBFS_VBUSON_PORT_C(TARGET_USBFS_VBUSON_BIT); \
		else \
			TARGET_USBFS_VBUSON_PORT_S(TARGET_USBFS_VBUSON_BIT); \
	} while (0)

	/**USB_OTG_HS GPIO Configuration    
	PB13     ------> USB_OTG_HS_VBUS
	PB14     ------> USB_OTG_HS_DM
	PB15     ------> USB_OTG_HS_DP 
	*/
	#define	USBD_HS_FS_INITIALIZE() do { \
		/*arm_hardware_pioa_altfn50((1uL << 11) | (1uL << 12), AF_OTGFS);	*/		/* PA10, PA11, PA12 - USB_OTG_FS	*/ \
		/* arm_hardware_pioa_inputs(1uL << 9);	*/	/* PA9 - USB_OTG_FS_VBUS */ \
		} while (0)

	#define	USBD_HS_ULPI_INITIALIZE() do { \
		} while (0)
#endif /* WITHUSBHW */

#if WITHDCDCFREQCTL
	// ST ST1S10 Synchronizable switching frequency from 400 kHz up to 1.2 MHz
	#define WITHHWDCDCFREQMIN 400000L
	#define WITHHWDCDCFREQMAX 1200000L

	// PB9 - DC-DC synchro output
	// TIM17_CH1 AF1
	// TIM4_CH4	AF2	AF_TIM4
	#define	HARDWARE_DCDC_INITIALIZE() do { \
		arm_hardware_piob_altfn2((1U << 9), AF_TIM17); /* PB9 - TIM17_CH1 */ \
		hardware_dcdcfreq_tim17_ch1_initialize(); \
	} while (0)
	#define HARDWARE_DCDC_SETDIV(f) do { \
		hardware_dcdcfreq_tim17_ch1_setdiv(f); \
	} while (0)
#else /* WITHDCDCFREQCTL */
	#define	HARDWARE_DCDC_INITIALIZE() do { \
	} while (0)
	#define HARDWARE_DCDC_SETDIV(f) do { \
		(void) (f); \
	} while (0)
#endif /* WITHDCDCFREQCTL */

	#if LCDMODE_LQ043T3DX02K
		#define WITHLCDBACKLIGHTOFF	1	// Имеется управление включением/выключением подсветки дисплея
		#define WITHLCDBACKLIGHT	1	// Имеется управление яркостью дисплея
		#define WITHLCDBACKLIGHTMIN	0	// Нижний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
		//#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры
	#elif LCDMODE_AT070TN90 || LCDMODE_AT070TNA2
		#define WITHLCDBACKLIGHTOFF	1	// Имеется управление включением/выключением подсветки дисплея
		#define WITHLCDBACKLIGHT	1	// Имеется управление яркостью дисплея
		#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
		#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
		//#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры
	#else
		/* Заглушка для работы без дисплея */
		#define WITHLCDBACKLIGHTMIN	0
		#define WITHLCDBACKLIGHTMAX	2	// Верхний предел регулировки (показываемый на дисплее)
	#endif

#if 0
	#define TARGET_BL_ENABLE_MIO	28

	#define	HARDWARE_BL_INITIALIZE() do { \
		xc7z_gpio_output(TARGET_BL_ENABLE_MIO); \
		} while (0)

	/* установка яркости и включение/выключение преобразователя подсветки */

	#define HARDWARE_BL_SET(en, level) do { \
		xc7z_writepin(TARGET_BL_ENABLE_MIO, (en)); \
	} while (0)
#endif

#if WITHLTDCHW && 0
	enum
	{
		GPIO_AF_LTDC14 = 14,  /* LCD-TFT Alternate Function mapping */
		GPIO_AF_LTDC9 = 9,  /* LCD-TFT Alternate Function mapping */
		//GPIO_AF_LTDC3 = 3  /* LCD-TFT Alternate Function mapping */
	};
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_INITIALIZE(demode) do { \
		const uint32_t MODEmask = (1U << 3); /* PD3 - MODEmask */ \
		const uint32_t DEmask = (1U << 13); /* PE13 - DE */ \
		const uint32_t HSmask = (1U << 6); /* PC6 - HSYNC */ \
		const uint32_t VSmask = (1U << 4); 	/* PA4 - VSYNC */ \
		/* Bit clock */ \
		arm_hardware_piog_altfn50((1U << 7), GPIO_AF_LTDC14);		/* CLK PG7 */ \
		/* Control */ \
		arm_hardware_piod_outputs(MODEmask, (demode != 0) * MODEmask);	/* PD3 MODEmask=state */ \
		/* Synchronisation signals in SYNC mode */ \
		arm_hardware_pioe_outputs((demode == 0) * DEmask, 0);	/* DE=0 (DISP, pin 31) */ \
		arm_hardware_pioa_altfn50((demode == 0) * VSmask, GPIO_AF_LTDC14);	/* VSYNC */ \
		arm_hardware_pioc_altfn50((demode == 0) * HSmask, GPIO_AF_LTDC14);	/* HSYNC */ \
		/* Synchronisation signals in DE mode*/ \
		arm_hardware_pioe_altfn50((demode != 0) * DEmask, GPIO_AF_LTDC14);	/* DE */ \
		arm_hardware_pioa_outputs((demode != 0) * VSmask, VSmask);	/* VSYNC */ \
		arm_hardware_pioc_outputs((demode != 0) * HSmask, HSmask);	/* HSYNC */ \
		/* RED */ \
		arm_hardware_piob_altfn50((1U << 0), GPIO_AF_LTDC14);		/* PB0 R3 */ \
		arm_hardware_pioa_altfn50((1U << 11), GPIO_AF_LTDC14);		/* PA11 R4 */ \
		arm_hardware_pioa_altfn50((1U << 9), GPIO_AF_LTDC14);		/* PA9 R5 */ \
		arm_hardware_pioa_altfn50((1U << 8), GPIO_AF_LTDC14);		/* PA8 R6 */ \
		arm_hardware_piog_altfn50((1U << 6), GPIO_AF_LTDC14);		/* PG6 R7 */ \
		/* GREEN */ \
		arm_hardware_pioa_altfn50((1U << 6), GPIO_AF_LTDC14);		/* PA6 G2 */ \
		arm_hardware_piog_altfn50((1U << 10), GPIO_AF_LTDC9);		/* PG10 G3 */ \
		arm_hardware_piob_altfn50((1U << 10), GPIO_AF_LTDC14);		/* PB10 G4 */ \
		arm_hardware_piof_altfn50((1U << 11), GPIO_AF_LTDC14);		/* PF11 G5 */ \
		arm_hardware_pioc_altfn50((1U << 7), GPIO_AF_LTDC14);		/* PC7 G6 */ \
		arm_hardware_piog_altfn50((1U << 8), GPIO_AF_LTDC14);		/* PG8 G7 */ \
		/* BLUE */ \
		arm_hardware_piog_altfn50((1U << 11), GPIO_AF_LTDC14);		/* PG11 B3 */ \
		arm_hardware_piog_altfn50((1U << 12), GPIO_AF_LTDC9);		/* PG12 B4 */ \
		arm_hardware_pioa_altfn50((1U << 3), GPIO_AF_LTDC14);		/* PA3 B5 */ \
		arm_hardware_piob_altfn50((1U << 8), GPIO_AF_LTDC14);		/* PB8 B6 */ \
		arm_hardware_piod_altfn50((1U << 8), GPIO_AF_LTDC14);		/* PD8 B7 */ \
	} while (0)

	/* управление состоянием сигнала DISP панели */
	/* demode values: 0: static signal, 1: DE controlled */
	#define HARDWARE_LTDC_SET_DISP(state) do { \
		const uint32_t VSmask = (1U << 4); 	/* PA4 - VSYNC */ \
		const uint32_t DEmask = (1U << 13); /* PE13 */ \
		/* while ((GPIOA->IDR & VSmask) != 0) ; */ /* схема синхронизации стоит на плате дисплея. дождаться 0 */ \
		/* while ((GPIOA->IDR & VSmask) == 0) ; */ /* дождаться 1 */ \
		arm_hardware_pioe_outputs(DEmask, ((state) != 0) * DEmask); /* DE=DISP, pin 31 - можно менять только при VSYNC=1 */ \
	} while (0)
#endif /* WITHLTDCHW */


	/* Выводы соединения с QSPI BOOT NOR FLASH */
	#define SPDIF_MISO_BIT (1u << 9)	// PF9	QUADSPI_BK1_IO1
	#define SPDIF_MOSI_BIT (1u << 8)	// PF8	QUADSPI_BK1_IO0
	#define SPDIF_SCLK_BIT (1u << 10)	// PF10	QUADSPI_CLK
	#define SPDIF_NCS_BIT (1u << 6)		// PB6	QUADSPI_BK1_NCS

	#define SPDIF_D2_BIT (1u << 7)		// PF7	QUADSPI_BK1_IO2
	#define SPDIF_D3_BIT (1u << 6)		// PF6	QUADSPI_BK1_IO3
	/* Отсоединить процессор от BOOT ROM - для возможности работы внешнего программатора. */
	#define SPIDF_HANGOFF() do { \
			/* arm_hardware_piob_inputs(SPDIF_NCS_BIT);  */ \
			/* arm_hardware_piof_inputs(SPDIF_SCLK_BIT);  */ \
			/* arm_hardware_piof_inputs(SPDIF_MOSI_BIT);  */ \
			/* arm_hardware_piof_inputs(SPDIF_MISO_BIT);  */ \
		} while (0)

	#if WIHSPIDFSW || WIHSPIDFHW

		#if WIHSPIDFHW
			#define SPIDF_HARDINITIALIZE() do { \
					arm_hardware_piof_altfn50(SPDIF_D2_BIT, AF_QUADSPI_AF9);  	/* PF7 D2 QUADSPI_BK1_IO2 */ \
					arm_hardware_piof_altfn50(SPDIF_D3_BIT, AF_QUADSPI_AF9);  	/* PF6 D3 QUADSPI_BK1_IO3 */ \
					/*arm_hardware_piof_outputs(SPDIF_D2_BIT, SPDIF_D2_BIT); */ /* PF7 D2 tie-up */ \
					/*arm_hardware_piof_outputs(SPDIF_D3_BIT, SPDIF_D3_BIT); */ /* PF6 D3 tie-up */ \
					arm_hardware_piof_altfn50(SPDIF_SCLK_BIT, AF_QUADSPI_AF9); /* PF10 SCLK */ \
					arm_hardware_piof_altfn50(SPDIF_MOSI_BIT, AF_QUADSPI_AF10); /* PF8 MOSI */ \
					arm_hardware_piof_altfn50(SPDIF_MISO_BIT, AF_QUADSPI_AF10); /* PF9 MISO */ \
					arm_hardware_piob_altfn50(SPDIF_NCS_BIT, AF_QUADSPI_AF10); /* PB6 CS */ \
				} while (0)

		#else /* WIHSPIDFHW */

			#define SPIDF_MISO() 1//((GPIOF->IDR & SPDIF_MISO_BIT) != 0)
			#define SPIDF_MOSI(v) //do { if (v) GPIOF->BSRR = BSRR_S(SPDIF_MOSI_BIT); else GPIOF->BSRR = BSRR_C(SPDIF_MOSI_BIT); } while (0)
			#define SPIDF_SCLK(v) //do { if (v) GPIOF->BSRR = BSRR_S(SPDIF_SCLK_BIT); else GPIOF->BSRR = BSRR_C(SPDIF_SCLK_BIT); } while (0)
			#define SPIDF_SOFTINITIALIZE() do { \
					/*arm_hardware_piof_outputs(SPDIF_D2_BIT, SPDIF_D2_BIT); */ /* D2 tie-up */ \
					/*arm_hardware_piof_outputs(SPDIF_D3_BIT, SPDIF_D3_BIT); */ /* D3 tie-up */ \
					/* arm_hardware_piob_outputs(SPDIF_NCS_BIT, SPDIF_NCS_BIT); */  \
					/* arm_hardware_piof_outputs(SPDIF_SCLK_BIT, SPDIF_SCLK_BIT); */  \
					/* arm_hardware_piof_outputs(SPDIF_MOSI_BIT, SPDIF_MOSI_BIT); */  \
					/* arm_hardware_piof_inputs(SPDIF_MISO_BIT); */  \
				} while (0)
			#define SPIDF_SELECT() do { \
					/* arm_hardware_piob_outputs(SPDIF_NCS_BIT, SPDIF_NCS_BIT); */  \
					/* arm_hardware_piof_outputs(SPDIF_SCLK_BIT, SPDIF_SCLK_BIT); */  \
					/* arm_hardware_piof_outputs(SPDIF_MOSI_BIT, SPDIF_MOSI_BIT); */  \
					/* arm_hardware_piof_inputs(SPDIF_MISO_BIT); */  \
					/*GPIOB->BSRR = BSRR_C(SPDIF_NCS_BIT);*/ \
					__DSB(); \
				} while (0)
			#define SPIDF_UNSELECT() do { \
					/*GPIOB->BSRR = BSRR_S(SPDIF_NCS_BIT); */ \
					/* arm_hardware_piob_inputs(SPDIF_NCS_BIT); */  \
					/* arm_hardware_piof_inputs(SPDIF_SCLK_BIT); */  \
					/* arm_hardware_piof_inputs(SPDIF_MOSI_BIT); */  \
					/* arm_hardware_piof_inputs(SPDIF_MISO_BIT); */  \
					__DSB(); \
				} while (0)

		#endif /* WIHSPIDFHW */

	#endif /* WIHSPIDFSW || WIHSPIDFHW */

#if 0
	#define ZYNQBOARD_BLINK_LED 58 /* LED_R */

	#define BOARD_BLINK_INITIALIZE() do { \
		xc7z_gpio_output(ZYNQBOARD_BLINK_LED); \
		} while (0)
	#define BOARD_BLINK_SETSTATE(state) do { \
			if (state) \
			{ \
				xc7z_writepin(ZYNQBOARD_BLINK_LED, 1); \
			} else { \
				xc7z_writepin(ZYNQBOARD_BLINK_LED, 0); \
			} \
		} while (0)

#endif

	/* запрос на вход в режим загрузчика */
	#define BOARD_USERBOOT_BIT	0//(1uL << 1)	/* PB1: ~USER_BOOT */
	#define BOARD_IS_USERBOOT() 0//(((GPIOB->IDR) & BOARD_USERBOOT_BIT) == 0)
	#define BOARD_USERBOOT_INITIALIZE() do { \
		/*arm_hardware_piob_inputs(BOARD_USERBOOT_BIT); */ /* set as input with pull-up */ \
		} while (0)

	/* макроопределение, которое должно включить в себя все инициализации */
	#define	HARDWARE_INITIALIZE() do { \
			/*BOARD_BLINK_INITIALIZE(); */\
			HARDWARE_KBD_INITIALIZE(); \
			HARDWARE_DAC_INITIALIZE(); \
			/*HARDWARE_BL_INITIALIZE(); */ \
			HARDWARE_DCDC_INITIALIZE(); \
			TXDISABLE_INITIALIZE(); \
			TUNE_INITIALIZE(); \
			BOARD_USERBOOT_INITIALIZE(); \
			/*USBD_FS_INITIALIZE(); */\
		} while (0)

#endif /* ARM_XCZUXX_CPUSTYLE_AXU2CGA_AD9640_H_INCLUDED */
