/* $Id$ */
/* board-specific CPU attached signals */
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//

// modem only
// Rmainunit_v6bm.pcb

#ifndef ARM_STM32F4XX_TQFP144_CPUSTYLE_RAVEN_V8_H_INCLUDED
#define ARM_STM32F4XX_TQFP144_CPUSTYLE_RAVEN_V8_H_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1		// US0:
//#define HARDWARE_ARM_USEUSART1 1		// US1: PA9/PA10 pins

#define WITHSPI16BIT	1		/* РІРѕР·РјРѕР¶РЅРѕ РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ 16-С‚Рё Р±РёС‚РЅС‹С… СЃР»РѕРІ РїСЂРё РѕР±РјРµРЅРµ РїРѕ SPI */
#define WITHSPIHW 		1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ Р°РїРїР°СЂР°С‚РЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° SPI */
#define WITHSPIHWDMA 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ DMA РїСЂРё РѕР±РјРµРЅРµ РїРѕ SPI */
//#define WITHSPISW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РїСЂРѕРіСЂР°РјРјРЅРѕРіРѕ СѓРїСЂР°РІР»РµРЅРёСЏ SPI. РќРµР»СЊР·СЏ СѓР±РёСЂР°С‚СЊ СЌС‚Сѓ СЃС‚СЂРѕРєСѓ - С‚СЂРµР±СѓРµС‚СЃСЏ СЏРІРЅРѕРµ РѕС‚РєР»СЋС‡РµРЅРёРµ РёР·-Р·Р° РєРѕРЅС„Р»РёРєС‚Р° СЃ I2C */
//#define WITHDMA2DHW		1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ DMA2D РґР»СЏ С„РѕСЂРјРёСЂРѕРІР°РЅРёСЏ РёР·РѕР±СЂР°Р¶РµРЅРёР№	*/

//#define WITHTWIHW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ Р°РїРїР°СЂР°С‚РЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° TWI (I2C) */
//#define WITHTWISW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РїСЂРѕРіСЂР°РјРјРЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° TWI (I2C) */

//#define WITHI2SHW	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ I2S2 & I2S3 - Р°СѓРґРёРѕРєРѕРґРµРє	*/
#define WITHSAI1HW	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ SAI1 - FPGA РёР»Рё IF codec	*/
//#define WITHSAI2HW	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ SAI2 - FPGA РёР»Рё IF codec	*/

#define WITHCPUDACHW	1	/* РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РІСЃС‚СЂРѕРµРЅРЅРѕРіРѕ РІ РїСЂРѕС†РµСЃСЃРѕСЂ DAC */
#define WITHCPUADCHW 	1	/* РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РІСЃС‚СЂРѕРµРЅРЅРѕРіРѕ РІ РїСЂРѕС†РµСЃСЃРѕСЂ ADC */

//#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
//#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER РІ 4-bit bus width */

//#define BSRR_S(v) ((v) * GPIO_BSRR_BS_0)	/* РџСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµ Р·РЅР°С‡РµРЅРёСЏ РґР»СЏ СѓСЃС‚Р°РЅРѕРІРєРё Р±РёС‚Р° РІ СЂРµРіРёСЃС‚СЂРµ */
//#define BSRR_C(v) ((v) * GPIO_BSRR_BR_0)	/* РџСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµ Р·РЅР°С‡РµРЅРёСЏ РґР»СЏ СЃР±СЂРѕСЃР° Р±РёС‚Р° РІ СЂРµРіРёСЃС‚СЂРµ */

#define WITHUART1HW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёР№РЅС‹Р№ РєРѕРЅС‚СЂРѕР»Р»РµСЂ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕРіРѕ РїРѕСЂС‚Р° #1 */
#define WITHUART2HW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёР№РЅС‹Р№ РєРѕРЅС‚СЂРѕР»Р»РµСЂ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕРіРѕ РїРѕСЂС‚Р° #2 */

//#define WITHUSBHW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІСЃС‚СЂРѕРµРЅРЅР°СЏ РІ РїСЂРѕС†РµСЃСЃРѕСЂ РїРѕРґРґРµСЂР¶РєР° USB */
//#define WITHUSBHWVBUSSENSE	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїСЂРµРґРѕРїСЂРµРґРµР»РµРЅРЅС‹Р№ РІС‹РІРѕРґ VBUS_SENSE */
//#define WITHUSBHWHIGHSPEED	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІСЃС‚СЂРѕРµРЅРЅР°СЏ РІ РїСЂРѕС†РµСЃСЃРѕСЂ РїРѕРґРґРµСЂР¶РєР° USB HS */
//#define WITHUSBHWHIGHSPEEDDESC	1	/* РўСЂРµР±СѓРµС‚СЃСЏ С„РѕСЂРјРёСЂРѕРІР°С‚СЊ РґРµСЃРєСЂРёРїС‚РѕСЂС‹ РєР°Рє РґР»СЏ HIGH SPEED */
//#define WITHUSBHW_DEVICE	USB_OTG_FS	/* РЅР° СЌС‚РѕРј СѓСЃС‚СЂРѕР№СЃС‚РІРµ РїРѕРґРґРµСЂР¶РёРІР°РµС‚СЃСЏ С„СѓРЅРєС†РёР»РЅР°Р»СЊРЅРѕСЃС‚СЊ DEVUCE	*/
//#define WITHUSBUAC		1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РІРёСЂС‚СѓР°Р»СЊРЅСѓСЋ Р·РІСѓРєРѕРІСѓСЋ РїР»Р°С‚Сѓ РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
//#define WITHUSBCDC		1	/* ACM РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РІРёСЂС‚СѓР°Р»СЊРЅС‹Р№ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅС‹Р№ РїРѕСЂС‚ РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
//#define WITHUSBCDCEEM	1	/* EEM РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ Ethernet Emulation Model РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
//#define WITHUSBCDCECM	1	/* ECM РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ Ethernet Control Model РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
//#define WITHUSBHID		1	/* HID РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ Human Interface Device РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */

//#define WITHCAT_CDC		1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РІРёСЂС‚СѓР°Р»СЊРЅС‹Р№ РІРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅС‹Р№ РїРѕСЂС‚ РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
#define WITHCAT_USART1		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART2		1	/* РїРѕСЂС‚ РїРѕРґРєР»СЋС‡РµРЅРёСЏ GPS/GLONASS */

#if WITHI2SHW
	// РРЅРёС†РёР°Р»РёР·РёСЂСѓСЋС‚СЃСЏ I2S2 Рё I2S3
	#define I2S2HW_INITIALIZE() do { \
		enum { \
			I2S2_WS = (1U << 12),		/* PB12	*/ \
			I2S2_CK = (1U << 13),		/* PB13	*/ \
			I2S2_SD = (1U << 15),		/* PB15	- РїРµСЂРµРґР°С‡Р° */ \
			\
			I2S3_WS = (1U << 15),		/* PA15	*/ \
			I2S3_CK = (1U << 3),		/* PB3	*/ \
			I2S3_SD = (1U << 5),		/* PB5	- РїСЂРёС‘Рј */ \
			\
		}; \
		arm_hardware_piob_altfn2(I2S2_WS, AF_SPI2); \
		arm_hardware_piob_altfn2(I2S2_CK, AF_SPI2); \
		arm_hardware_piob_altfn2(I2S2_SD, AF_SPI2); \
		arm_hardware_pioa_altfn2(I2S3_WS, AF_SPI3); \
		arm_hardware_piob_altfn2(I2S3_CK, AF_SPI3); \
		arm_hardware_piob_altfn2(I2S3_SD, AF_SPI3); \
	} while (0)
#endif /* WITHSAI1HW */

#if WITHSAI1HW
	#define SAI1HW_INITIALIZE()	do { \
		/*arm_hardware_pioe_altfn20(1U << 2, AF_SAI); */	/* PE2 - SAI1_MCK_A - 12.288 MHz	*/ \
		arm_hardware_pioe_altfn2(1U << 4, AF_SAI);			/* PE4 - SAI1_FS_A	- 48 kHz	*/ \
		arm_hardware_pioe_altfn20(1U << 5, AF_SAI);			/* PE5 - SAI1_SCK_A	*/ \
		arm_hardware_pioe_altfn2(1U << 6, AF_SAI);			/* PE6 - SAI1_SD_A	(i2s data to codec)	*/ \
		arm_hardware_pioe_altfn2(1U << 3, AF_SAI);			/* PE3 - SAI1_SD_B	(i2s data from codec)	*/ \
	} while (0)
#endif /* WITHSAI1HW */

/* Р Р°СЃРїСЂРµРґРµР»РµРЅРёРµ Р±РёС‚РѕРІ РІ ARM РєРѕРЅС‚СЂРѕР»Р»РµСЂР°С… */

#if WITHCAT || WITHNMEA
	// CAT data lites
	// RXD at PA10, TXD at PA9

	// CAT control lines
	#define FROMCAT_TARGET_PIN_RTS		(GPIOA->IDR) // was PINA 
	#define FROMCAT_BIT_RTS				(1u << 11)	/* СЃРёРіРЅР°Р» RTS РѕС‚ FT232RL	*/

	/* РјР°РЅРёРїСѓР»СЏС†РёСЏ РѕС‚ РїРѕСЂС‚Р° RS-232, СЃРёРіРЅР°Р» PPS РѕС‚ GPS/GLONASS/GALILEO РјРѕРґСѓР»СЏ */
	#define FROMCAT_TARGET_PIN_DTR		(GPIOA->IDR) // was PINA 
	#define FROMCAT_BIT_DTR				(1u << 12)	/* СЃРёРіРЅР°Р» DTR РѕС‚ FT232RL	*/

	/* РјР°РЅРёРїСѓР»СЏС†РёСЏ РѕС‚ РїРѕСЂС‚Р° RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
			arm_hardware_pioa_inputs(FROMCAT_BIT_DTR); \
			arm_hardware_pioa_updown(FROMCAT_BIT_DTR, 0); \
		} while (0)

	/* РїРµСЂРµС…РѕРґ РЅР° РїРµСЂРµРґР°С‡Сѓ РѕС‚ РїРѕСЂС‚Р° RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
			arm_hardware_pioa_inputs(FROMCAT_BIT_RTS); \
			arm_hardware_pioa_updown(FROMCAT_BIT_RTS, 0); \
		} while (0)

	/* СЃРёРіРЅР°Р» PPS РѕС‚ GPS/GLONASS/GALILEO РјРѕРґСѓР»СЏ */
	#define NMEA_INITIALIZE() \
		do { \
			arm_hardware_pioa_inputs(FROMCAT_BIT_DTR); \
			arm_hardware_pioa_onchangeinterrupt(FROMCAT_BIT_DTR, FROMCAT_BIT_DTR, FROMCAT_BIT_DTR, ARM_SYSTEM_PRIORITY); \
		} while (0)

#endif

#if WITHTX

	// txpath outputs not used
	////#define TXPATH_TARGET_PORT_S(v)		do { GPIOD->BSRR = BSRR_S(v); __DSB(); } while (0)
	////#define TXPATH_TARGET_PORT_C(v)		do { GPIOD->BSRR = BSRR_C(v); __DSB(); } while (0)
	// 
	#define TXGFV_RX		(1u << 4)
	#define TXGFV_TRANS		0			// РїРµСЂРµС…РѕРґ РјРµР¶РґСѓ СЂРµР¶РёРјР°РјРё РїСЂРёС‘РјР° Рё РїРµСЂРµРґР°С‡Рё
	#define TXGFV_TX_SSB	(1u << 0)
	#define TXGFV_TX_CW		(1u << 1)
	#define TXGFV_TX_AM		(1u << 2)
	#define TXGFV_TX_NFM	(1u << 3)

	#define TXPATH_INITIALIZE() \
		do { \
		} while (0)

	#define HARDWARE_GET_PTT() (0)
	#define PTT_INITIALIZE() \
		do { \
		} while (0)

#endif /* WITHTX */

#if 0 && WITHELKEY
	// Electronic key inputs
	#define ELKEY_TARGET_PIN			(GPIOD->IDR)
	#define ELKEY_BIT_LEFT				(1U << 8)		// PD8
	#define ELKEY_BIT_RIGHT				(1U << 9)		// PD9

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_piod_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_piod_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */

// IOUPDATE = PA15
//#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
//#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
//#define SPI_IOUPDATE_BIT		(1U << 15)	// * PA15

// РќР°Р±РѕСЂ РѕРїСЂРµРґРµР»РµРЅРёР№ РґР»СЏ СЂР°Р±РѕС‚С‹ Р±РµР· РІРЅРµС€РЅРµРіРѕ РґРµС€РёС„СЂР°С‚РѕСЂР°
#define SPI_ALLCS_PORT_S(v)	do { GPIOG->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_ALLCS_PORT_C(v)	do { GPIOG->BSRR = BSRR_C(v); __DSB(); } while (0)

#define SPI_CSEL_PG15	(1U << 15)	// PG15 ext1
#define SPI_CSEL_PG8	(1U << 8)	// PG8 nvmem FM25L16B
#define SPI_CSEL_PG7	(1U << 7)	// PG7 board control registers chain
#define SPI_CSEL_PG6	(1U << 6)	// PG6 dummy codec interface
//#define SPI_CSEL_PG5	(1U << 5)	// PG5 FPGA CS2 - used as overflov signal from ADC
#define SPI_CSEL_PG4	(1U << 5)	// PG5 FPGA CS3
#define SPI_CSEL_PG3	(1U << 5)	// PG5 FPGA CS4
#define SPI_CSEL_PG1	(1U << 1)	// PG0 FPGA control registers CS
#define SPI_CSEL_PG0	(1U << 0)	// PG1 FPGA NCO1 registers CS

// Р—РґРµСЃСЊ РґРѕР»Р¶РЅС‹ Р±С‹С‚СЊ РїРµСЂРµС‡РёСЃР»РµРЅС‹ РІСЃРµ Р±РёС‚С‹ С„РѕСЂРјРёСЂРѕРІР°РЅРёСЏ CS РІ СѓСЃС‚СЂРѕР№СЃС‚РІРµ.
#define SPI_ALLCS_BITS ( \
	SPI_CSEL_PG15	| 	/* PG15 ext1 */ \
	SPI_CSEL_PG8	| 	/* PG8 nvmem FM25L16B */ \
	SPI_CSEL_PG7	| 	/* PG7 board control registers chain */ \
	SPI_CSEL_PG6	|	/* dummy codec interface */ \
	/*SPI_CSEL_PG5	| */	/* PG5 FPGA CS2 */ \
	SPI_CSEL_PG4	| 	/* PG5 FPGA CS3 */ \
	SPI_CSEL_PG3	| 	/* PG5 FPGA CS4 */ \
	SPI_CSEL_PG1	| 	/* PG0 FPGA control registers CS */ \
	SPI_CSEL_PG0	| 	/* PG1 FPGA NCO1 registers CS */ \
	0)

#define SPI_ALLCS_BITSNEG 0		// Р’С‹С…РѕРґС‹, Р°РєС‚РёРІРЅС‹Рµ РїСЂРё "1"

//#define SPI_NAEN_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
//#define SPI_NAEN_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)

//#define SPI_NAEN_BIT (1u << 7)		// * PE7 used

/* РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ Р»РёРёР№ РІС‹Р±РѕСЂР° РїРµСЂРёС„РµСЂРёР№РЅС‹С… РјРёРєСЂРѕСЃС…РµРј */
#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_piog_outputs(SPI_ALLCS_BITS, SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG); \
	} while (0)

/* РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ СЃРёРіРЅР°Р»РѕРІ СѓРїСЂР°РІР»РґРµРЅРёСЏ РґРµС€РёС„СЂР°С‚РѕСЂРѕРј CS */
#define SPI_ADDRESS_NAEN_INITIALIZE() \
	do { \
	} while (0)
/* РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ СЃРёРіРЅР°Р»РѕРІ СѓРїСЂР°РІР»РґРµРЅРёСЏ РґРµС€РёС„СЂР°С‚РѕСЂРѕРј CS */
#define SPI_ADDRESS_AEN_INITIALIZE() \
	do { \
	} while (0)
/* РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ СЃРёРіРЅР°Р»Р° IOUPDATE РЅР° DDS */
/* РЅР°С‡Р°Р»СЊРЅРѕРµ СЃРѕСЃС‚РѕСЏРЅРёРµ = 1 */
#define SPI_IOUPDATE_INITIALIZE() \
	do { \
	} while (0)
#define SPI_IORESET_INITIALIZE() \
	do { \
	} while (0)

// MOSI & SCK port
#define SPI_TARGET_SCLK_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_SCLK_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
#define	SPI_SCLK_BIT			(1U << 5)	// * PA5 Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ СЃРёРЅС…СЂРѕРЅРёР·Р°С†РёСЏ SPI

#define SPI_TARGET_MOSI_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
#define	SPI_MOSI_BIT			(1U << 7)	// * PA7 Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІС‹РІРѕРґ (РёР»Рё РІРІРѕРґ РІ СЃР»СѓС‡Р°Рµ РґРІСѓРЅР°РїСЂР°РІР»РµРЅРЅРѕРіРѕ SPI).

#define SPI_TARGET_MISO_PIN		(GPIOB->IDR)		// was PINA 
#define	SPI_MISO_BIT			(1U << 4)	// * PB4 Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІРІРѕРґ СЃ SPI.

#define SPIIO_INITIALIZE() do { \
		arm_hardware_pioa_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
		arm_hardware_piob_inputs(SPI_MISO_BIT); \
	} while (0)


#define HARDWARE_SPI_CONNECT() do { \
		arm_hardware_piob_altfn50(SPI_MISO_BIT, AF_SPI1); /* Р’ СЌС‚РёС… РїСЂРѕС†РµСЃСЃРѕСЂР°С… Рё РІС…РѕРґС‹ Рё РІС‹С…РѕРґС‹ РїРµСЂРµРєРґСЋС‡Р°СЋС‚СЃСЏ РЅР° ALT FN */ \
		arm_hardware_pioa_altfn50(SPI_MOSI_BIT | SPI_SCLK_BIT, AF_SPI1); /* Р’ СЌС‚РёС… РїСЂРѕС†РµСЃСЃРѕСЂР°С… Рё РІС…РѕРґС‹ Рё РІС‹С…РѕРґС‹ РїРµСЂРµРєРґСЋС‡Р°СЋС‚СЃСЏ РЅР° ALT FN */ \
	} while (0)
#define HARDWARE_SPI_DISCONNECT() do { \
		arm_hardware_pioa_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
		arm_hardware_piob_inputs(SPI_MISO_BIT); \
	} while (0)
#define HARDWARE_SPI_CONNECT_MOSI() do { \
		arm_hardware_pioa_altfn50(SPI_MOSI_BIT, AF_SPI1);	/* PIO disable for MOSI bit (SD CARD read support) */ \
	} while (0)
#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
		arm_hardware_pioa_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
	} while (0)

#define HARDWARE_USART1_INITIALIZE() do { \
		arm_hardware_pioa_altfn2((1U << 9) | (1U << 10), AF_USART1); /* PA9: TX DATA line (2 MHz), PA10: RX data line */ \
		arm_hardware_pioa_updown((1U << 10), 0);	/* PA10: pull-up RX data */ \
	} while (0)
#define HARDWARE_USART2_INITIALIZE() do { \
		arm_hardware_piod_altfn2((1U << 5) | (1U << 6), AF_USART2); /* PD5: TX DATA line (2 MHz), PD6: RX data line */ \
		arm_hardware_piod_updown((1U << 6), 0);	/* PD6: pull-up RX data */ \
	} while (0)

#define HARDWARE_SIDETONE_INITIALIZE() do { \
	} while (0)

#if KEYBOARD_USE_ADC
	#define HARDWARE_KBD_INITIALIZE() do { \
		} while (0)
#else
	#define HARDWARE_KBD_INITIALIZE() do { \
		arm_hardware_pioa_inputs(KBD_MASK); \
		} while (0)
#endif

#if 1 // WITHTWISW
	#define TARGET_TWI_TWCK_PORT_C(v) do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_C(v) do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PIN		(GPIOB->IDR)
	#define TARGET_TWI_TWD_PIN		(GPIOB->IDR)
	#define TARGET_TWI_TWCK		(1u << 6)		// PB6 SCL
	#define TARGET_TWI_TWD		(1u << 7)		// PB7 SDA

	// РРЅРёС†РёР°Р»РёР·Р°С†РёСЏ Р±РёС‚РѕРІ РїРѕСЂС‚РѕРІ РІРІРѕРґР°-РІС‹РІРѕРґР° РґР»СЏ РїСЂРѕРіСЂР°РјРјРЅРѕР№ СЂРµР°Р»РёР·Р°С†РёРё I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_piob_opendrain(TARGET_TWI_TWCK | TARGET_TWI_TWD, TARGET_TWI_TWCK | TARGET_TWI_TWD); \
		} while (0) 
	// РРЅРёС†РёР°Р»РёР·Р°С†РёСЏ Р±РёС‚РѕРІ РїРѕСЂС‚РѕРІ РІРІРѕРґР°-РІС‹РІРѕРґР° РґР»СЏ Р°РїРїР°СЂР°С‚РЅРѕР№ СЂРµР°Р»РёР·Р°С†РёРё I2C
	// РїСЂРёСЃРѕРµРґРёРЅРµРЅРёРµ РІС‹РІРѕРґРѕРІ Рє РїРµСЂРёС„РµСЂРёР№РЅРѕРјСѓ СѓСЃС‚СЂРѕР№СЃС‚РІСѓ
	#define	TWIHARD_INITIALIZE() do { \
			arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, AF_I2C1);	/* AF=4 */ \
		} while (0) 


#endif // WITHTWISW

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS

	/* outputs */
	#define FPGA_NCONFIG_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define FPGA_NCONFIG_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define FPGA_NCONFIG_BIT		(1UL << 13)	/* PE13 bit conneced to nCONFIG pin ALTERA FPGA */

	/* inputs */
	#define FPGA_CONF_DONE_INPUT	(GPIOE->IDR)
	#define FPGA_CONF_DONE_BIT		(1UL << 14)	/* PE14 bit conneced to CONF_DONE pin ALTERA FPGA */

	#define FPGA_NSTATUS_INPUT		(GPIOE->IDR)
	#define FPGA_NSTATUS_BIT		(1UL << 15)	/* PE15 bit conneced to NSTATUS pin ALTERA FPGA */

	/* РРЅРёС†РёР°РґРёР·Р°С†РёСЏ РІС‹РІРѕРґРѕРІ GPIO РїСЂРѕС†РµСЃСЃРѕСЂР° РґР»СЏ РїРѕР»СѓС‡РµРЅРёСЏ СЃРѕСЃС‚РѕСЏРЅРёСЏ Рё СѓРїСЂР°РІР»РµРЅРёРµРј Р·Р°РіСЂСѓР·РєРѕР№ FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
			arm_hardware_pioe_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
			arm_hardware_pioe_inputs(FPGA_CONF_DONE_BIT | FPGA_NSTATUS_BIT); \
		} while (0)

	/* РїРѕ РєРѕСЃРІРµРЅРЅС‹Рј РїСЂРёР·РЅР°РєР°Рј РїСЂРѕРІРµСЂСЏРµРј, РїСЂРѕРёРЅРёС†РёР°Р»РёР·РёСЂРѕРІР°Р»Р°СЃСЊ Р»Рё FPGA (РІРѕС€Р»Р° РІ user mode). */
	#define HARDWARE_FPGA_IS_USER_MODE() (local_delay_ms(400), 1)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR
	// Р‘РёС‚С‹ РґРѕСЃС‚СѓРїР° Рє РјР°СЃСЃРёРІСѓ РєРѕСЌС„С„РёС†РёРµРЅС‚РѕРІ FIR С„РёР»СЊС‚СЂР° РІ FPGA
	#define TARGET_FPGA_FIR_CS_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR_CS_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR_CS_BIT (1U << 7)	/* PE7 - fir CLK */

	#define TARGET_FPGA_FIR1_WE_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR1_WE_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR1_WE_BIT (1U << 8)	/* PE8 - fir1 WE */

	#define TARGET_FPGA_FIR2_WE_PORT_C(v)	do { GPIOE->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR2_WE_PORT_S(v)	do { GPIOE->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_FPGA_FIR2_WE_BIT (1U << 9)	/* PE9 - fir2 WE */

	#define TARGET_FPGA_FIR_INITIALIZE() do { \
			arm_hardware_pioe_outputs(TARGET_FPGA_FIR1_WE_BIT, TARGET_FPGA_FIR1_WE_BIT); \
			arm_hardware_pioe_outputs(TARGET_FPGA_FIR2_WE_BIT, TARGET_FPGA_FIR2_WE_BIT); \
			arm_hardware_pioe_outputs(TARGET_FPGA_FIR_CS_BIT, TARGET_FPGA_FIR_CS_BIT); \
		} while (0)
#endif /* WITHDSPEXTFIR */

/* РїРѕР»СѓС‡РµРЅРёРµ СЃРѕСЃС‚РѕСЏРЅРёСЏ РїРµСЂРµРїРѕР»РЅРµРЅРёСЏ РђР¦Рџ */
#define TARGET_FPGA_OVF_PIN		(GPIOG->IDR)
#define TARGET_FPGA_OVF_BIT		(1u << 5)	// PG5
#define TARGET_FPGA_OVF_GET		((TARGET_FPGA_OVF_PIN & TARGET_FPGA_OVF_BIT) == 0)	// 1 - overflow active
#define TARGET_FPGA_OVF_INITIALIZE() do { \
			arm_hardware_piog_inputs(TARGET_FPGA_OVF_BIT); \
		} while (0)

#if WITHCPUDACHW
	/* РІРєР»СЋС‡РёС‚СЊ РЅСѓР¶РЅС‹Рµ РєР°РЅР°Р»С‹ */
	#define HARDWARE_DAC_INITIALIZE() do { \
			DAC1->CR = DAC_CR_EN1; /* DAC1 enable */ \
		} while (0)
	#define HARDWARE_DAC_ALC(v) do { /* РІС‹РІРѕРґ 12-Р±РёС‚РЅРѕРіРѕ Р·РЅР°С‡РµРЅРёСЏ РЅР° Р¦РђРџ - РєР°РЅР°Р» 1 */ \
			DAC1->DHR12R1 = (v); /* DAC1 set value */ \
		} while (0)
#endif /* WITHCPUDACHW */

#if WITHCPUADCHW
	#define HARDWARE_ADC_INITIALIZE(ainmask) do { \
			arm_hardware_pioa_analoginput(((ainmask) >> 0) & 0xff);	/* ADC12_IN0..ADC12_IN7 */ \
			arm_hardware_piob_analoginput(((ainmask) >> 8) & 0x03);	/* ADC12_IN8..ADC12_IN0 */ \
			arm_hardware_pioc_analoginput(((ainmask) >> 10) & 0x3f);	/* ADC12_IN10..ADC12_IN15 */ \
		} while (0)
#endif /* WITHCPUADCHW */

// РќР°Р·РЅР°С‡РµРЅРёСЏ РІС…РѕРґРѕРІ РђР¦Рџ РїСЂРѕС†РµСЃСЃРѕСЂР°.
enum 
{ 
#if WITHREFSENSOR
	VREFIX = 17,		// Reference voltage
#endif /* WITHREFSENSOR */
#if WITHTEMPSENSOR
	TEMPIX = 16,
#endif /* WITHTEMPSENSOR */
#if WITHVOLTLEVEL 
	VOLTSOURCE = 8,		// PB0 РЎСЂРµРґРЅСЏСЏ С‚РѕС‡РєР° РґРµР»РёС‚РµР»СЏ РЅР°РїСЂСЏР¶РµРЅРёСЏ, РґР»СЏ РђРљР‘
#endif /* WITHVOLTLEVEL */

#if WITHPOTGAIN
	POTIFGAIN = 2,		// PA2 IF GAIN
	POTAFGAIN = 3,		// PA3 AF GAIN
#endif /* WITHPOTGAIN */

#if WITHPOTWPM
	POTWPM = 6,			// PA6 РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂ СѓРїСЂР°РІР»РµРЅРёСЏ СЃРєРѕСЂРѕСЃС‚СЊСЋ РїРµСЂРµРґР°С‡Рё РІ С‚РµР»РµРіСЂР°С„Рµ
#endif /* WITHPOTWPM */
#if WITHPOTPOWER
	POTPOWER = 6,			// СЂРµРіСѓР»РёСЂРѕРІРєР° РјРѕС‰РЅРѕСЃС‚Рё
#endif /* WITHPOTPOWER */

	ALCINIX = 9,		// PB1 ALC IN

#if WITHCURRLEVEL
	PASENSEIX = 1,		// PA1 PA current sense - ACS712-05 chip
#endif /* WITHCURRLEVEL */

#if WITHSWRMTR
	PWRI = 14,			// PC4
	FWD = 14, REF = 15,	// PC5	SWR-meter
#endif /* WITHSWRMTR */
	//KI0 = 10, KI1 = 11, KI2 = 12, KI3 = 13, KI4 = 0	// РєР»Р°РІРёР°С‚СѓСЂР°
};

#define KI_COUNT 0	// РєРѕР»РёС‡РµСЃС‚РІРѕ РёСЃРїРѕР»СЊР·СѓРµРјС‹С… РїРѕРґ РєР»Р°РІРёР°С‚СѓСЂСѓ РІС…РѕРґРѕРІ РђР¦Рџ

#if WITHUSBHW
	/**USB_OTG_FS GPIO Configuration    
	PA9     ------> USB_OTG_FS_VBUS
	PA11     ------> USB_OTG_FS_DM
	PA12     ------> USB_OTG_FS_DP 
	*/
	#define	USBD_FS_INITIALIZE() do { \
		arm_hardware_pioa_altfn50((1U << 11) | (1U << 12), AF_OTGFS);			/* PA11, PA12 - USB_OTG_FS	*/ \
		arm_hardware_pioa_inputs(1U << 9);		/* PA9 - USB_OTG_FS_VBUS */ \
		arm_hardware_pioa_updownoff((1U << 9) | (1U << 11) | (1U << 12)); \
		} while (0)

	/**USB_OTG_HS GPIO Configuration    
	PB13     ------> USB_OTG_HS_VBUS
	PB14     ------> USB_OTG_HS_DM
	PB15     ------> USB_OTG_HS_DP 
	*/
	#define	USBD_HS_FS_INITIALIZE() do { \
		arm_hardware_piob_altfn50((1U << 14) | (1U << 15), AF_OTGHS_FS);			/* PB14, PB15 - USB_OTG_HS	*/ \
		arm_hardware_piob_inputs(1U << 13);		/* PB13 - USB_OTG_HS_VBUS */ \
		arm_hardware_piob_updownoff((1U << 13) | (1U << 14) | (1U << 15)); \
		} while (0)

	#define	USBD_HS_ULPI_INITIALIZE() do { \
		} while (0)
#endif /* WITHUSBHW */

	/* РјР°РєСЂРѕРѕРїСЂРµРґРµР»РµРЅРёРµ, РєРѕС‚РѕСЂРѕРµ РґРѕР»Р¶РЅРѕ РІРєР»СЋС‡РёС‚СЊ РІ СЃРµР±СЏ РІСЃРµ РёРЅРёС†РёР°Р»РёР·Р°С†РёРё */
	#define	HARDWARE_INITIALIZE() do { \
		HARDWARE_SIDETONE_INITIALIZE(); \
		HARDWARE_KBD_INITIALIZE(); \
		HARDWARE_DAC_INITIALIZE(); \
		} while (0)

#endif /* ARM_STM32F4XX_TQFP144_CPUSTYLE_RAVEN_V8_H_INCLUDED */
