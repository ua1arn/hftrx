/* $Id$ */
/* board-specific CPU attached signals */
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//

// РЎРёРЅС‚РµР·Р°С‚РѕСЂ "Р’РѕСЂРѕР±РµР№-2" РЅР° РїСЂРѕС†РµСЃСЃРѕСЂРµ STM32F105RC СЃ РјРµСЃС‚РѕРј РїРѕРґ РєРѕРґРµРє TLV320AIC23B

#ifndef ARM_STM32FXXX_TQFP64_CPUSTYLE_V8B_H_INCLUDED
#define ARM_STM32FXXX_TQFP64_CPUSTYLE_V8B_H_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1
//#define HARDWARE_ARM_USEUSART1 1

#define WITHSPI16BIT	1		/* РІРѕР·РјРѕР¶РЅРѕ РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ 16-С‚Рё Р±РёС‚РЅС‹С… СЃР»РѕРІ РїСЂРё РѕР±РјРµРЅРµ РїРѕ SPI */
#define WITHSPIHW 		1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ Р°РїРїР°СЂР°С‚РЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° SPI */
#define WITHSPIHWDMA 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ DMA РїСЂРё РѕР±РјРµРЅРµ РїРѕ SPI */
#define WITHSPISW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РїСЂРѕРіСЂР°РјРјРЅРѕРіРѕ СѓРїСЂР°РІР»РµРЅРёСЏ SPI. РќРµР»СЊР·СЏ СѓР±РёСЂР°С‚СЊ СЌС‚Сѓ СЃС‚СЂРѕРєСѓ - С‚СЂРµР±СѓРµС‚СЃСЏ СЏРІРЅРѕРµ РѕС‚РєР»СЋС‡РµРЅРёРµ РёР·-Р·Р° РєРѕРЅС„Р»РёРєС‚Р° СЃ I2C */

#define WITHTWIHW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ Р°РїРїР°СЂР°С‚РЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° TWI (I2C) */
//#define WITHTWISW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РїСЂРѕРіСЂР°РјРјРЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° TWI (I2C) */
#define WITHCPUADCHW 	1	/* РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ ADC */

#if 0
	// РЎ РїРµСЂРµРґРµР»РєР°РјРё - СЃ USB
	// РѕР±СЂР°С‚РёС‚СЊ РІРЅРёРјР°РЅРёРµ РЅР° REF1_MUL 168 РёР»Рё 216

	#define WITHUSBHW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІСЃС‚СЂРѕРµРЅРЅР°СЏ РІ РїСЂРѕС†РµСЃСЃРѕСЂ РїРѕРґРґРµСЂР¶РєР° USB */
	#define WITHUSBHWVBUSSENSE	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїСЂРµРґРѕРїСЂРµРґРµР»РµРЅРЅС‹Р№ РІС‹РІРѕРґ VBUS_SENSE */
	//#define WITHUSBHWHIGHSPEED	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІСЃС‚СЂРѕРµРЅРЅР°СЏ РІ РїСЂРѕС†РµСЃСЃРѕСЂ РїРѕРґРґРµСЂР¶РєР° USB HS */
	//#define WITHUSBHWHIGHSPEEDDESC	1	/* РўСЂРµР±СѓРµС‚СЃСЏ С„РѕСЂРјРёСЂРѕРІР°С‚СЊ РґРµСЃРєСЂРёРїС‚РѕСЂС‹ РєР°Рє РґР»СЏ HIGH SPEED */
	#define WITHUSBUAC		1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РІРёСЂС‚СѓР°Р»СЊРЅСѓСЋ Р·РІСѓРєРѕРІСѓСЋ РїР»Р°С‚Сѓ РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
	#define WITHUSBCDC		1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РІРёСЂС‚СѓР°Р»СЊРЅС‹Р№ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅС‹Р№ РїРѕСЂС‚ РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */

	#define WITHCAT_CDC		1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РІРёСЂС‚СѓР°Р»СЊРЅС‹Р№ РІРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅС‹Р№ РїРѕСЂС‚ РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
	#define WITHMODEM_CDC	1

	#if 0
		//#define WITHUART2HW	1	/* Р’С‹С…РѕРґ С‡РµСЂРµР· PA2 РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёР№РЅС‹Р№ РєРѕРЅС‚СЂРѕР»Р»РµСЂ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕРіРѕ РїРѕСЂС‚Р° #2 */
		//#define WITHDEBUG_USART2	1
		//#define WITHNMEA_USART2		1
		#define HARDWARE_USART2_INITIALIZE() do { \
				arm_hardware_pioa_altfn2((1U << 2), AF_USART2); /* PA2: TX DATA line (2 MHz) */ \
			} while (0)
	#endif
#else
	// РћСЂРёРіРёРЅР°Р»СЊРЅС‹Р№ РІР°СЂРёР°РЅС‚ - С‚РѕР»СЊРєРѕ СЃ CAT СЃРµСЂРµР· UART1 Рё FT232RL

	#define WITHUART1HW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёР№РЅС‹Р№ РєРѕРЅС‚СЂРѕР»Р»РµСЂ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕРіРѕ РїРѕСЂС‚Р° #1 */

	#define WITHCAT_USART1		1
	#define WITHMODEM_USART1	1
	#define WITHMODEM_USART1	1
	#define WITHNMEA_USART1		1

	#if 0
		#define HARDWARE_USART1_INITIALIZE() do { \
				arm_hardware_pioa_altfn2((1U << 9) | (1U << 10), AF_USART1); /* PA9: TX DATA line (2 MHz), PA10: RX data line */ \
				arm_hardware_pioa_updown((1U << 10), 0);	/* PA10: pull-up RX data */ \
			} while (0)

	#endif
#endif

#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_pioc_outputs2m(LS020_RS, LS020_RS); \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
	arm_hardware_pioc_outputs2m(LS020_RST, LS020_RST); \
	} while (0)


#if LCDMODE_SPI_NA || LCDMODE_SPI_RA
	// СЌС‚Рё РєРѕРЅС‚СЂРѕР»Р»РµСЂС‹ С‚СЂРµР±СѓСЋС‚ С‚РѕР»СЊРєРѕ RS
	// Р­С‚Рё РєРѕРЅС‚СЂРѕР»Р»РµСЂС‹ С‚СЂРµР±СѓСЋС‚ RESET Рё RS

	#define LS020_RS_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define LS020_RS_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define LS020_RS			(1u << 9)			// PC9

#endif

#if WITHENCODER

	// Encoder inputs: PB11 - PHASE A, PB10 = PHASE B
	#define ENCODER_INPUT_PORT			(GPIOB->IDR) 
	#define ENCODER_BITS ((1u << 11) | (1u << 10))
	#define ENCODER_SHIFT 10

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_piob_inputs(ENCODER_BITS); \
			arm_hardware_piob_updown(ENCODER_BITS, 0); \
			arm_hardware_piob_onchangeinterrupt(ENCODER_BITS, ENCODER_BITS, ENCODER_BITS, ARM_OVERREALTIME_PRIORITY); \
		} while (0)

#endif

/* Р Р°СЃРїСЂРµРґРµР»РµРЅРёРµ Р±РёС‚РѕРІ РІ ARM РєРѕРЅС‚СЂРѕР»Р»РµСЂР°С… */

#if WITHCAT || WITHNMEA
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

	// txpath outputs
	#define TXPATH_TARGET_PORT_S(v)		do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TXPATH_TARGET_PORT_C(v)		do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
	#if 1
		// РЈРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - СЃРёРіРЅР°Р»С‹ TXPATH_ENABLE_SSB (PC7) Рё TXPATH_ENABLE_CW (PC8) - Р°РєС‚РёРІРЅС‹ РїСЂРё РЅСѓР»Рµ РЅР° РІС‹С…РѕРґРµ.
		#define TXPATH_BIT_ENABLE_SSB		(1u << 7)		// PC7
		#define TXPATH_BIT_ENABLE_CW		(1u << 8)		// PC8
		#define TXPATH_BITS_ENABLE	(TXPATH_BIT_ENABLE_SSB | TXPATH_BIT_ENABLE_CW)

		// РџРѕРґРіРѕС‚РѕРІР»РµРЅРЅС‹Рµ СѓРїСЂР°РІР»СЏСЋС‰РёРµ СЃР»РѕРІР°
		#define TXGFV_RX		0
		#define TXGFV_TRANS		0			// РїРµСЂРµС…РѕРґ РјРµР¶РґСѓ СЂРµР¶РёРјР°РјРё РїСЂРёС‘РјР° Рё РїРµСЂРµРґР°С‡Рё
		#define TXGFV_TX_SSB	TXPATH_BIT_ENABLE_SSB
		#define TXGFV_TX_CW		TXPATH_BIT_ENABLE_CW
		#define TXGFV_TX_AM		TXPATH_BIT_ENABLE_CW
		#define TXGFV_TX_NFM	TXPATH_BIT_ENABLE_CW

		// РЈРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - СЃРёРіРЅР°Р»С‹ TXPATH_ENABLE (PA11) Рё TXPATH_ENABLE_CW (PA10) - Р°РєС‚РёРІРЅС‹ РїСЂРё РЅСѓР»Рµ РЅР° РІС‹С…РѕРґРµ.
		// РЈСЃС‚Р°РЅРѕРІРєР° РЅР°С‡Р°Р»СЊРЅРѕРіРѕ СЃС‚СЃС‚РѕСЏРЅРёСЏ РІС‹С…РѕРґРѕРІ
		#define TXPATH_INITIALIZE() \
			do { \
				arm_hardware_pioc_opendrain(TXPATH_BITS_ENABLE, TXPATH_BITS_ENABLE); \
			} while (0)

	#else
		// РЈРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - РµРґРёРЅСЃС‚РІРµРЅРЅС‹Р№ СЃРёРіРЅР°Р» СЂР°Р·СЂРµС€РµРЅРёСЏ С‚СЂР°РєС‚Р°
		#define TXPATH_BIT_GATE (1u << 7)	// PC7 - РІС‹С…РѕРґРЅРѕР№ СЃРёРіРЅР°Р» РёР· РїСЂРѕС†РµСЃСЃРѕСЂР° - СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј.
		//#define TXPATH_BIT_GATE_RX TXPATH_BIT_GATE	// СЃРёРіРЅР°Р» tx2 - СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј. РџСЂРё РїСЂРёС‘РјРµ Р°РєС‚РёРІРµРЅ
		#define TXPATH_BIT_GATE_RX 0	// СЃРёРіРЅР°Р» tx2 - СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј. РџСЂРё РїСЂРёС‘РјРµ РЅРµ Р°РєС‚РёРІРµРЅ

		// РџРѕРґРіРѕС‚РѕРІР»РµРЅРЅС‹Рµ СѓРїСЂР°РІР»СЏСЋС‰РёРµ СЃР»РѕРІР°
		#define TXGFV_RX		TXPATH_BIT_GATE_RX
		#define TXGFV_TRANS		0			// РїРµСЂРµС…РѕРґ РјРµР¶РґСѓ СЂРµР¶РёРјР°РјРё РїСЂРёС‘РјР° Рё РїРµСЂРµРґР°С‡Рё
		#define TXGFV_TX_SSB	TXPATH_BIT_GATE
		#define TXGFV_TX_CW		TXPATH_BIT_GATE
		#define TXGFV_TX_AM		TXPATH_BIT_GATE
		#define TXGFV_TX_NFM	TXPATH_BIT_GATE
		// РЈРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - СЃРёРіРЅР°Р» TXPATH_BIT_GATE
		// РЈСЃС‚Р°РЅРѕРІРєР° РЅР°С‡Р°Р»СЊРЅРѕРіРѕ СЃС‚СЃС‚РѕСЏРЅРёСЏ РІС‹С…РѕРґРѕРІ
		#define TXPATH_INITIALIZE() \
			do { \
				arm_hardware_pioc_outputs2m(TXPATH_BIT_GATE, 0); \
			} while (0)

	#endif

	// PTT input - PD2
	#define PTT_TARGET_PIN				(GPIOD->IDR)
	#define PTT_BIT_PTT					(1U << 2)		// PD2

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
			arm_hardware_piod_inputs(PTT_BIT_PTT); \
			arm_hardware_piod_updown(PTT_BIT_PTT, 0); \
		} while (0)

#endif /* WITHTX */

#if WITHELKEY

	// Electronic key inputs
	#define ELKEY_TARGET_PIN			(GPIOB->IDR)
	#define ELKEY_BIT_LEFT				(1U << 9)		// PB9
	#define ELKEY_BIT_RIGHT				(1U << 14)		// PB14

	#define ELKEY_INITIALIZE() \
		do { \
			arm_hardware_piob_inputs(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); \
			arm_hardware_piob_updown(ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT, 0); \
		} while (0)

#endif /* WITHELKEY */


// IOUPDATE = PA8

#define SPI_IOUPDATE_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_IOUPDATE_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_IOUPDATE_BIT		(1U << 8)	// PA8

// Р•СЃС‚СЊ РІРЅРµС€РЅРёР№ РґРµС€РёС„СЂР°С‚РѕСЂ РЅР° С€РёРЅРµ Р°РґСЂРµСЃР° SPI 

// Р±РёС‚С‹ РІС‹РІРѕРґР° Р°РґСЂРµСЃР° С‡РёРїСЃРµР»РµРєС‚ РґРµС€РёС„СЂР°С‚РѕСЂР°
#define SPI_ADDRESS_PORT_S(v)	do { GPIOC->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_ADDRESS_PORT_C(v)	do { GPIOC->BSRR = BSRR_C(v); __DSB(); } while (0)
//#define SPI_ADDRESS_DDR_S			(GPIOC->PIO_OER)	// was DDRA
//#define SPI_ADDRESS_DDR_C			(GPIOC->PIO_ODR)	// was DDRA

#define SPI_A0 ((1u << 10))			// PC10 Р±РёС‚С‹ Р°РґСЂРµСЃР° РґР»СЏ РґРµС€РёС„СЂР°С‚РѕСЂР° SPI
#define SPI_A1 ((1u << 11))			// PC11
#define SPI_A2 ((1u << 12))			// PC12


#define SPI_NAEN_PORT_S(v)	do { GPIOA->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_NAEN_PORT_C(v)	do { GPIOA->BSRR = BSRR_C(v); __DSB(); } while (0)
//#define SPI_NAEN_DDR_S			(GPIOA->PIO_OER)	// was DDRA
//#define SPI_NAEN_DDR_C			(GPIOA->PIO_ODR)	// was DDRA

#define SPI_NAEN_BIT (1u << 15)		// PA15 used
#define SPI_ALLCS_BITS	0		// С‚СЂРµР±СѓРµС‚СЃСЏ РґР»СЏ СѓРєР°Р·Р°РЅРёСЏ С‚РѕРіРѕ, С‡С‚Рѕ СЂР°Р±РѕС‚Р° СЃ РїСЂСЏРјС‹Рј РІС‹Р±РѕСЂРѕРј CS (Р±РµР· РґРµС€РёС„СЂР°С‚РѕСЂР°) РЅРµ С‚СЂРµР±СѓРµС‚СЃСЏ

/* РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ Р»РёРёР№ РІС‹Р±РѕСЂР° РїРµСЂРёС„РµСЂРёР№РЅС‹С… РјРёРєСЂРѕСЃС…РµРј */
#define SPI_ALLCS_INITIALIZE() \
	do { \
	} while (0)
/* РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ СЃРёРіРЅР°Р»РѕРІ СѓРїСЂР°РІР»РґРµРЅРёСЏ РґРµС€РёС„СЂР°С‚РѕСЂРѕРј CS */
#define SPI_ADDRESS_AEN_INITIALIZE() \
	do { \
	} while (0)
/* РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ СЃРёРіРЅР°Р»РѕРІ СѓРїСЂР°РІР»РґРµРЅРёСЏ РґРµС€РёС„СЂР°С‚РѕСЂРѕРј CS */
#define SPI_ADDRESS_NAEN_INITIALIZE() \
	do { \
		arm_hardware_pioc_outputs2m(SPI_ADDRESS_BITS, 0); \
		arm_hardware_pioa_outputs2m(SPI_NAEN_BIT, SPI_NAEN_BIT); \
	} while (0)
/* РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ СЃРёРіРЅР°Р»Р° IOUPDATE РЅР° DDS */
/* РЅР°С‡Р°Р»СЊРЅРѕРµ СЃРѕСЃС‚РѕСЏРЅРёРµ = 1 */
#define SPI_IOUPDATE_INITIALIZE() \
	do { \
		arm_hardware_pioa_outputs2m(SPI_IOUPDATE_BIT, SPI_IOUPDATE_BIT); \
	} while (0)
/* РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ СЃРёРіРЅР°Р»Р° IOUPDATE РЅР° DDS */
/* РЅР°С‡Р°Р»СЊРЅРѕРµ СЃРѕСЃС‚РѕСЏРЅРёРµ = 0 */
#define SPI_IORESET_INITIALIZE() \
	do { \
	} while (0)

// SPI control pins
// SPI1 hardware used.

// MOSI & SCK port
// STM32F303: SPI1_NSS can be placed on PA4 or PA15
#define SPI_TARGET_SCLK_PORT_C(v)	do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_SCLK_PORT_S(v)	do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_C(v)	do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
#define SPI_TARGET_MOSI_PORT_S(v)	do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)

#define	SPI_SCLK_BIT			(1U << 3)	// * PB3 Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ СЃРёРЅС…СЂРѕРЅРёР·Р°С†РёСЏ SPI
#define	SPI_MOSI_BIT			(1U << 5)	// * PB5 Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІС‹РІРѕРґ (РёР»Рё РІРІРѕРґ РІ СЃР»СѓС‡Р°Рµ РґРІСѓРЅР°РїСЂР°РІР»РµРЅРЅРѕРіРѕ SPI).

#define SPI_TARGET_MISO_PIN		(GPIOB->IDR)		// was PINA 
#define	SPI_MISO_BIT			(1U << 4)	// * PB4 Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІРІРѕРґ СЃ SPI.

#define SPIIO_INITIALIZE() do { \
		arm_hardware_piob_outputs(SPI_MOSI_BIT | SPI_SCLK_BIT, SPI_MOSI_BIT | SPI_SCLK_BIT); \
		arm_hardware_piob_inputs(SPI_MISO_BIT); \
	} while (0)

#define SIDETONE_TARGET_BIT		(1u << 8)	// (PB8, base mapping) output TIM4_CH3 

#if 1 // WITHTWISW
	#define TARGET_TWI_TWCK_PORT_C(v) do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_C(v) do { GPIOB->BSRR = BSRR_C(v); __DSB(); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do { GPIOB->BSRR = BSRR_S(v); __DSB(); } while (0)
	#define TARGET_TWI_TWCK_PIN		(GPIOB->IDR)
	#define TARGET_TWI_TWD_PIN		(GPIOB->IDR)
	#define TARGET_TWI_TWCK		(1u << 6)		// * PB6
	#define TARGET_TWI_TWD		(1u << 7)		// * PB6

	// РРЅРёС†РёР°Р»РёР·Р°С†РёСЏ Р±РёС‚РѕРІ РїРѕСЂС‚РѕРІ РІРІРѕРґР°-РІС‹РІРѕРґР° РґР»СЏ РїСЂРѕРіСЂР°РјРјРЅРѕР№ СЂРµР°Р»РёР·Р°С†РёРё I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_piob_opendrain(TARGET_TWI_TWCK | TARGET_TWI_TWD, TARGET_TWI_TWCK | TARGET_TWI_TWD); \
		} while (0) 
	// РРЅРёС†РёР°Р»РёР·Р°С†РёСЏ Р±РёС‚РѕРІ РїРѕСЂС‚РѕРІ РІРІРѕРґР°-РІС‹РІРѕРґР° РґР»СЏ Р°РїРїР°СЂР°С‚РЅРѕР№ СЂРµР°Р»РёР·Р°С†РёРё I2C
	// РїСЂРёСЃРѕРµРґРёРЅРµРЅРёРµ РІС‹РІРѕРґРѕРІ Рє РїРµСЂРёС„РµСЂРёР№РЅРѕРјСѓ СѓСЃС‚СЂРѕР№СЃС‚РІСѓ
	#if CPUSTYLE_STM32F1XX

		#define	TWIHARD_INITIALIZE() do { \
				arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, 255);	/* AF=4 */ \
			} while (0) 

	#elif CPUSTYLE_STM32F30X

		#define	TWIHARD_INITIALIZE() do { \
				arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, 4 /* AF_I2C1 */);	/* AF=4 */ \
			} while (0) 

	#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

		#define	TWIHARD_INITIALIZE() do { \
				arm_hardware_piob_periphopendrain_altfn2(TARGET_TWI_TWCK | TARGET_TWI_TWD, AF_I2C1);	/* AF=4 */ \
			} while (0) 

	#endif
#endif // WITHTWISW

#if WITHCPUADCHW
	#define HARDWARE_ADC_INITIALIZE(ainmask) do { \
			arm_hardware_pioa_analoginput(((ainmask) >> 0) & 0xff);	/* ADC12_IN0..ADC12_IN7 */ \
			arm_hardware_piob_analoginput(((ainmask) >> 8) & 0x03);	/* ADC12_IN8..ADC12_IN0 */ \
			arm_hardware_pioc_analoginput(((ainmask) >> 10) & 0x3f);	/* ADC12_IN10..ADC12_IN15 */ \
		} while (0)
#endif /* WITHCPUADCHW */

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

#if CPUSTYLE_STM32F1XX

	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_piob_altfn50(SPI_MOSI_BIT | SPI_SCLK_BIT, 255); \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_piob_outputs(SPI_SCLK_BIT | SPI_MOSI_BIT, SPI_SCLK_BIT | SPI_MOSI_BIT); /* connect back to GPIO */ \
			arm_hardware_piob_inputs(SPI_MISO_BIT); /* connect back to GPIO */ \
		} while (0)
	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_piob_altfn50(SPI_MOSI_BIT, 255);	/* PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_piob_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)
	#define HARDWARE_USART1_INITIALIZE() do { \
			arm_hardware_pioa_altfn2((1U << 9), 255);	/* TX DATA line (2 MHz) */ \
			arm_hardware_pioa_inputs((1U << 10));		/* RX data line */ \
			arm_hardware_pioa_updown((1U << 10), 0);	/* pull-up RX data */ \
		} while (0)
	#define HARDWARE_SIDETONE_INITIALIZE() do { \
			arm_hardware_piob_altfn2(SIDETONE_TARGET_BIT, 255);	/* output TIM4_CH3 (PB8, base mapping) */ \
		} while (0)

	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pioa_inputs(KBD_MASK); \
			} while (0)
	#endif

#elif CPUSTYLE_STM32F30X

	#define HARDWARE_SPI_CONNECT() do { \
			arm_hardware_piob_altfn50(SPI_MOSI_BIT | SPI_SCLK_BIT | SPI_MISO_BIT, 5 /*AF_SPI1 */); /* Р’ СЌС‚РёС… РїСЂРѕС†РµСЃСЃРѕСЂР°С… Рё РІС…РѕРґС‹ Рё РІС‹С…РѕРґС‹ РїРµСЂРµРєРґСЋС‡Р°СЋС‚СЃСЏ РЅР° ALT FN */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT() do { \
			arm_hardware_piob_outputs(SPI_SCLK_BIT | SPI_MOSI_BIT, SPI_SCLK_BIT | SPI_MOSI_BIT); /* connect back to GPIO */ \
			arm_hardware_piob_inputs(SPI_MISO_BIT); /* connect back to GPIO */ \
		} while (0)
	#define HARDWARE_SPI_CONNECT_MOSI() do { \
			arm_hardware_piob_altfn50(SPI_MOSI_BIT, 5 /*AF_SPI1 */);	/* PIO disable for MOSI bit (SD CARD read support) */ \
		} while (0)
	#define HARDWARE_SPI_DISCONNECT_MOSI() do { \
			arm_hardware_piob_outputs(SPI_MOSI_BIT, SPI_MOSI_BIT);	/* PIO enable for MOSI bit (SD CARD read support)  */ \
		} while (0)
	#define HARDWARE_USART1_INITIALIZE() do { \
			arm_hardware_pioa_altfn2((1U << 9) | (1U << 10), 7 /* AF_USART1 */); /* TX DATA line (2 MHz), RX data line */ \
			arm_hardware_pioa_updown((1U << 10), 0);	/* pull-up RX data */ \
		} while (0)
	#define HARDWARE_SIDETONE_INITIALIZE() do { \
		arm_hardware_piob_altfn2(SIDETONE_TARGET_BIT, 2 /*AF_TIM4 */);/* output TIM4_CH3 (PB8, base mapping) */ \
		} while (0)

	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		#define HARDWARE_KBD_INITIALIZE() do { \
			arm_hardware_pioa_inputs(KBD_MASK); \
			} while (0)
	#endif

#endif

#endif /* ARM_STM32FXXX_TQFP64_CPUSTYLE_V8B_H_INCLUDED */
