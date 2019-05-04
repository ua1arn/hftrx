/* $Id$ */
/* board-specific CPU DIP40/TQFP44 attached signals */
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//

// РњР°Р»РµРЅСЊРєР°СЏ РїР»Р°С‚Р° РєРѕРЅС‚СЂРѕР»Р»РµСЂР° СЃ РґРІСѓРјСЏ AD9834 Рё 4 Р“РЈРќ
// Р‘РµР· SPI_BIDIRECTIONAL Рё СЃ РІРѕР·РјРѕР¶РЅРѕР№ РіРµРЅРµСЂР°Р№РёРµР№ Р·РІСѓРєРѕРІ.

#ifndef ATMEGA_CPUSTYLE_V5_H_INCLUDED
#define ATMEGA_CPUSTYLE_V5_H_INCLUDED 1

//#define WITHSPIHW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ Р°РїРїР°СЂР°С‚РЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° SPI */
#define WITHSPISW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РїСЂРѕРіСЂР°РјРјРЅРѕРіРѕ СѓРїСЂР°РІР»РµРЅРёСЏ SPI. */
#define WITHCPUADCHW 	1	/* РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ ADC */

//#define WITHUART1HW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёР№РЅС‹Р№ РєРѕРЅС‚СЂРѕР»Р»РµСЂ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕРіРѕ РїРѕСЂС‚Р° #1 */
//#define WITHUART2HW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёР№РЅС‹Р№ РєРѕРЅС‚СЂРѕР»Р»РµСЂ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕРіРѕ РїРѕСЂС‚Р° #2 */

//#define WITHCAT_CDC		1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РІРёСЂС‚СѓР°Р»СЊРЅС‹Р№ РІРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅС‹Р№ РїРѕСЂС‚ РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
#define WITHCAT_USART1		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART1		1

	// РґР°РЅРЅС‹Рµ LCD
	#define LCD_DATA_PORT PORTB
	#define LCD_DATA_DIRECTION DDRB
	#define LCD_DATA_INPUT PINB
	// СЃС‚СЂРѕР± E LCD
	#define LCD_STROBE_PORT PORTB
	#define LCD_STROBE_DIRECTION DDRB
	// СЃРёРіРЅР°Р»С‹ RS Рё WE LCD
	#define LCD_RS_WE_PORT PORTB
	#define LCD_RS_WE_DIRECTION DDRB

	#define LCD_STROBE_BIT (1U << PB3)	// 0x08

	#define LCD_DATAS_BIT_LOW PB4		// РєР°РєРѕР№ Р±РёС‚ РґР°РЅРЅС‹С… РјР»Р°РґС€РёР№ РІ СЃР»РѕРІРµ СЃС‡РёС‚Р°РЅРЅРѕРј СЃ РїРѕСЂС‚Р°
	#define LCD_DATAS_BITS (0x0f << LCD_DATAS_BIT_LOW)

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* РїРѕР»СѓС‡РёС‚СЊ РґР°РЅРЅС‹Рµ СЃ С€РёРЅС‹ LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { \
		LCD_DATA_PORT = \
			(LCD_DATA_PORT & ~ LCD_DATAS_BITS) | \
			(((v) << LCD_DATAS_BIT_LOW) & LCD_DATAS_BITS); \
		} while (0)

	#define WRITEE_BIT (1U << PB2) // 0x04
	#define ADDRES_BIT (1U << PB1)	// 0x02

#if WITHENCODER
	// encoder
	#define ENCODER_TARGET_PORT PORTD
	#define ENCODER_DIRECTION_PORT DDRD
	#define ENCODER_INPUT_PORT PIND

	#define ENCODER_BITS ((1U << PIND3) | (1U << PIND2))
	#define ENCODER_SHIFT PIND2
#endif /* WITHENCODER */

	//#define SPI_BIDIRECTIONAL 1	// РІРІРѕРґ РґР°РЅРЅС‹С… РїРѕ SPI РёРґРµС‚ С‡РµСЂРµР· С‚РѕС‚ Р¶Рµ СЃР°РјС‹Р№ pin, С‡С‚Рѕ Рё РІС‹РІРѕРґ

	/* РїРµСЂРµРєР»СЋС‡РµРЅРёРµ Р±РёС‚Р° СѓРїСЂР°РІР»РµРЅРёСЏ РЅРµСЃСѓС‰РµР№ РІС‹Р·С‹РІР°РµС‚СЃСЏ РёР· РїСЂРµСЂС‹РІР°РЅРёР№ -
	 РІСЃРµРј РѕСЃС‚Р°Р»СЊРЅС‹Рј, РјРѕРґРёС„РёС†РёСЂСѓСЋС‰РёРј Р±РёС‚С‹ РЅР° СЌС‚РѕРј РїРѕСЂС‚Сѓ, РІС‹РїРѕР»РЅСЏС‚СЊ РїРѕРґ Р·Р°РїСЂРµС‚РѕРј РїСЂРµСЂС‹РІР°РЅРёР№.
		*/

	#define TXPATH_TARGET_PORT PORTD	// РІС‹С…РѕРґС‹ РїСЂРѕС†РµСЃСЃРѕСЂР° - СѓРїСЂР°РІР»РµРЅРёРµ С‚СЂР°РєС‚РѕРј РїРїРµСЂРµРґР°С‡Рё Рё РјР°РЅРёРїСѓР»СЏС†РёРµР№
	#define TXPATH_TARGET_DDR DDRD		// РїРµСЂРµРєР»СЋС‡РµРЅРёРµ РЅР° РІС‹РІРѕРґ - СѓРїСЂР°РІР»РµРЅРёРµ С‚СЂР°РєС‚РѕРј РїРїРµСЂРµРґР°С‡Рё Рё РјР°РЅРёРїСѓР»СЏС†РёРµР№

	#define TXPATH_BIT_GATE (1U << PD6)	// РІС‹С…РѕРґРЅРѕР№ СЃРёРіРЅР°Р» РёР· РїСЂРѕС†РµСЃСЃРѕСЂР° - СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј.
	//#define TXPATH_BIT_GATE_RX TXPATH_BIT_GATE	// СЃРёРіРЅР°Р» tx2 - СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј. РџСЂРё РїСЂРёС‘РјРµ Р°РєС‚РёРІРµРЅ
	#define TXPATH_BIT_GATE_RX 0	// СЃРёРіРЅР°Р» tx2 - СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј. РџСЂРё РїСЂРёС‘РјРµ РЅРµ Р°РєС‚РёРІРµРЅ

	// РџРѕРґРіРѕС‚РѕРІР»РµРЅРЅС‹Рµ СѓРїСЂР°РІР»СЏСЋС‰РёРµ СЃР»РѕРІР°
	#define TXGFV_RX		TXPATH_BIT_GATE_RX
	#define TXGFV_TRANS		0			// РїРµСЂРµС…РѕРґ РјРµР¶РґСѓ СЂРµР¶РёРјР°РјРё РїСЂРёС‘РјР° Рё РїРµСЂРµРґР°С‡Рё
	#define TXGFV_TX_SSB	TXPATH_BIT_GATE
	#define TXGFV_TX_CW		TXPATH_BIT_GATE
	#define TXGFV_TX_AM		TXPATH_BIT_GATE
	#define TXGFV_TX_NFM	TXPATH_BIT_GATE


#if WITHELKEY
	// ++ Р­Р›Р•РљРўР РћРќРќР«Р™ РљР›Р®Р§
	#define ELKEY_TARGET_PORT PORTD		// РІС…РѕРґС‹ РїСЂРѕС†РµСЃСЃРѕСЂР° - СЃРёРіРЅР°Р»С‹ СЌР»РµРєС‚СЂРѕРЅРЅРѕРіРѕ РєР»СЋС‡Р°
	#define ELKEY_TARGET_DDR DDRD		// РїРµСЂРµРєР»СЋС‡РµРЅРёРµ РЅР° РІРІРѕРґ - СЃРёРіРЅР°Р»С‹ СЌР»РµРєС‚СЂРѕРЅРЅРѕРіРѕ РєР»СЋС‡Р°
	#define ELKEY_TARGET_PIN PIND		// РІРІРѕРґ - СЃРёРіРЅР°Р»С‹ СЌР»РµРєС‚СЂРѕРЅРЅРѕРіРѕ РєР»СЋС‡Р°

	#define ELKEY_BIT_LEFT (1U << PD4)
	#define ELKEY_BIT_RIGHT (1U << PD5)
	// СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РЅР°Р¶Р°С‚РёСЏРј РЅР° РєРѕРЅС‚Р°РєС‚С‹ РєР»СЋС‡Р°
	// РњРёРєСЂРѕРїСЂРѕС†РµСЃСЃРѕСЂР°С… РЅР°РїРѕРґРѕР±РёРё ATMega644
	#define PCMSK_PORT	PCMSK3			// PCMSK0 - PA, PCMSK1 - PB, PCMSK2 - PC, PCMSK3 - PD
	#define PCICR_BIT	(1U << PCIE3)	// PCIE0 - PA, PCIE1 - PB, PCIE2 - PC, PCIE3 - PD
	#define PCIVECT PCINT3_vect			// РІРµРєС‚РѕСЂ РѕР±СЂР°Р±РѕС‚С‡РёРєР°
	// -- Р­Р›Р•РљРўР РћРќРќР«Р™ РљР›Р®Р§
#endif

	// ++ РџР•Р”РђР›Р¬
	#define PTT_TARGET_PORT PORTC	
	#define PTT_TARGET_DDR DDRC
	#define PTT_TARGET_PIN PINC

	#define PTT_BIT_PTT (1U << PC7)
	// -- РџР•Р”РђР›Р¬

#if KEYBOARD_USE_ADC == 0
	#define KBD_TARGET_PORT PORTA
	#define KBD_TARGET_DDR DDRA
	#define KBD_TARGET_PIN PINA

	#define KBD_MASK 0xfe	// РІСЃРµ РёСЃРїРѕР»СЊР·СѓРµРјС‹Рµ Р±РёС‚С‹
#endif

// v6 only
	// РІ СЌС‚РѕР№ РІРµСЂСЃРёРё РїР»Р°С‚С‹ РЅРµС‚ СѓРїСЂР°РІР»РµРЅРёСЏ IORESET, IOUPDATE
	#define SPI_TARGET_SCLK_PORT PORTC
	#define SPI_TARGET_SCLK_DDR DDRC
	#define SPI_TARGET_SCLK_PIN PINC

	#define SPI_TARGET_MOSI_PORT PORTC
	#define SPI_TARGET_MOSI_DDR DDRC
	#define SPI_TARGET_MOSI_PIN PINC

	#define	SPI_SCLK_BIT 0x01	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ СЃРёРЅС…СЂРѕРЅРёР·Р°С†РёСЏ SPI
	#define	SPI_MOSI_BIT 0x02	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІС‹РІРѕРґ (РёР»Рё РІРІРѕРґ РІ СЃР»СѓС‡Р°Рµ РґРІСѓРЅР°РїСЂР°РІР»РµРЅРЅРѕРіРѕ SPI).

	#define SPI_AEN_PORT		PORTC
	#define SPI_AEN_DDR			DDRC
	#define SPI_AEN_PIN			PINC
	#define SPI_AEN_BIT 0x20
	#define SPI_ALLCS_BITS	0		// С‚СЂРµР±СѓРµС‚СЃСЏ РґР»СЏ СѓРєР°Р·Р°РЅРёСЏ С‚РѕРіРѕ, С‡С‚Рѕ СЂР°Р±РѕС‚Р° СЃ РїСЂСЏРјС‹Рј РІС‹Р±РѕСЂРѕРј CS (Р±РµР· РґРµС€РёС„СЂР°С‚РѕСЂР°) РЅРµ С‚СЂРµР±СѓРµС‚СЃСЏ

	// SPI address location
	#define SPI_ADDRESS_PORT		PORTC
	#define SPI_ADDRESS_DDR		DDRC
	//#define SPI_ADDRESS_PIN		PINC

	#define SPI_A0 (1U << PC2)			// Р±РёС‚С‹ Р°РґСЂРµСЃР° РґР»СЏ РґРµС€РёС„СЂР°С‚РѕСЂР° SPI
	#define SPI_A1 (1U << PC3)
	#define SPI_A2 (1U << PC4)

	// MISO location
	#define SPI_TARGET_MISO_PIN PINB
	#define SPI_TARGET_MISO_PORT PORTB
	#define SPI_TARGET_MISO_DDR DDRB

	#define	SPI_MISO_BIT		(1U << PB0)	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІРІРѕРґ СЃ SPI.

	// Variations of SPI_ALLCS_INITIALIZE
	#if SPI_ALLCS_BITS != 0
		#define SPI_ALLCS_INITIALIZE() do { \
			SPI_ALLCS_PORT |= (SPI_ALLCS_BITS); \
			SPI_ALLCS_DDR |= (SPI_ALLCS_BITS); \
			} while (0)
	#else /* SPI_ALLCS_BITS != 0 */
		#define SPI_ALLCS_INITIALIZE() do { \
			} while (0)
	#endif /* SPI_ALLCS_BITS != 0 */

	#define HARDWARE_SPI_CONNECT() do { \
		} while (0)

	#define HARDWARE_SPI_DISCONNECT() do { \
			SPCR = 0x00; \
		} while (0)

	#if SPI_BIDIRECTIONAL

		#define SPIIO_INITIALIZE() do { \
			HARDWARE_OUTPUT_INITIALIZE(SPI_TARGET_SCLK_PORT, SPI_TARGET_SCLK_DDR, SPI_SCLK_BIT, SPI_SCLK_BIT); \
			HARDWARE_OUTPUT_INITIALIZE(SPI_TARGET_MOSI_PORT, SPI_TARGET_MOSI_DDR, SPI_MOSI_BIT, SPI_MOSI_BIT); \
			} while (0)

	#else /* SPI_BIDIRECTIONAL */

		#define SPIIO_INITIALIZE() do { \
			HARDWARE_INPUT_INITIALIZE(SPI_TARGET_MISO_PORT, SPI_TARGET_MISO_DDR, SPI_MISO_BIT, SPI_MISO_BIT); /* enable pull-up on MISO */ \
			/* PB4(~SS) РґРѕР»Р¶РµРЅ Р±С‹С‚СЊ РІС‹С…РѕРґРѕРј. SPI_NAEN_BIT - СЂР°Р·СЂРµС€РµРЅРёРµ РґРµС€РёС„СЂР°С‚РѕСЂР°. */ \
			HARDWARE_OUTPUT_INITIALIZE(SPI_TARGET_SCLK_PORT, SPI_TARGET_SCLK_DDR, SPI_SCLK_BIT, SPI_SCLK_BIT); \
			HARDWARE_OUTPUT_INITIALIZE(SPI_TARGET_MOSI_PORT, SPI_TARGET_MOSI_DDR, SPI_MOSI_BIT, SPI_MOSI_BIT); \
			} while (0)

	#endif /* SPI_BIDIRECTIONAL */


	#define SIDETONE_TARGET_PORT	PORTD	/* РџРѕСЂС‚ РїСЂРѕС†РµСЃСЃРѕСЂР°, РЅР° РєРѕС‚РѕСЂРѕРј РЅР°С…РѕРґРёС‚СЃСЏ РІС‹РІРѕРґ С‚РѕРЅР° СЃР°РјРѕРєРѕРЅС‚СЂРѕР»СЏ */
	#define SIDETONE_TARGET_DDR		DDRD
	#define SIDETONE_TARGET_BIT		(1U << PD7)


	#if WITHTWISW
	//#if CPUSTYLE_ATMEGA
		#define TARGET_TWI_PORT PORTB
		#define TARGET_TWI_DDR DDRB
		#define TARGET_TWI_PIN PINB
		//#define TARGET_TWI_TWCK	(1U << PC0)	// Р°РїРїР°СЂР°С‚РЅС‹Р№ TWCK Р·РґРµСЃСЊ Р¶Рµ
		//#define TARGET_TWI_TWD	(1U << PC1)	// Р°РїРїР°СЂР°С‚РЅС‹Р№ TWD Р·РґРµСЃСЊ Р¶Рµ
		#define TARGET_TWI_TWCK	(1U << PB4)			// РЅР° СЌС‚РѕР№ РїР»Р°С‚Рµ РїРѕРґРєР»СЋС‡Р°РµРј Рє С‚РµРј Р¶Рµ Р±РёС‚Р°Рј, РєСѓРґР° РјРѕРіР»Рё Р±С‹ РїСЂРёРїР°РёРІР°С‚СЊСЃСЏ РґР°РЅРЅС‹Рµ РёРЅРґРёРєР°С‚РѕСЂР°
		#define TARGET_TWI_TWD	(1U << PB5)
	//#elif CPUSTYLE_ARM
		//#define TARGET_TWI_PORT_C	AT91C_BASE_PIOA->PIO_CODR
		//#define TARGET_TWI_PORT_S	AT91C_BASE_PIOA->PIO_SODR
		//#define TARGET_TWI_TWCK	AT91C_PA4_TWCK
		//#define TARGET_TWI_TWD	AT91C_PA3_TWD
	//#endif
	#endif

	/* РїСЂРѕРіСЂР°РјРјРёСЂРѕРІР°РЅРёРµ РІС‹РІРѕРґРѕРІ, СѓРїСЂР°РІР»СЏСЋС‰РёС… I2C */
	#define TWISOFT_INITIALIZE() do { \
		HARDWARE_INPUT_INITIALIZE(TARGET_TWI_TWCK_PORT, TARGET_TWI_TWCK_DDR, TARGET_TWI_TWCK, TARGET_TWI_TWCK); \
		HARDWARE_INPUT_INITIALIZE(TARGET_TWI_TWD_PORT, TARGET_TWI_TWD_DDR, TARGET_TWI_TWD, TARGET_TWI_TWD); \
		} while (0)

	// РРЅРёС†РёР°Р»РёР·Р°С†РёСЏ Р±РёС‚РѕРІ РїРѕСЂС‚РѕРІ РІРІРѕРґР°-РІС‹РІРѕРґР° РґР»СЏ Р°РїРїР°СЂР°С‚РЅРѕР№ СЂРµР°Р»РёР·Р°С†РёРё I2S
	// РїСЂРёСЃРѕРµРґРёРЅРµРЅРёРµ РІС‹РІРѕРґРѕРІ Рє РїРµСЂРёС„РµСЂРёР№РЅРѕРјСѓ СѓСЃС‚СЂРѕР№СЃС‚РІСѓ
	// РќР° ATMEGA РЅРµ С‚СЂРµР±СѓРµС‚СЃСЏ
	#define	TWIHARD_INITIALIZE() do { \
		} while (0) 

	#define CS4272_RESET_INITIALIZE() do { \
			HARDWARE_OUTPUT_INITIALIZE(TARGET_CS4272_RESET_PORT, TARGET_CS4272_RESET_DDR, TARGET_CS4272_RESET_BIT, TARGET_CS4272_RESET_BIT); \
		} while (0)

	#define LS020_RS_INITIALIZE() do { \
			HARDWARE_OUTPUT_INITIALIZE(LS020_RS_PORT, LS020_RS_DDR, LS020_RS, LS020_RS); \
		} while(0)

	#define LS020_RESET_INITIALIZE() do { \
			HARDWARE_OUTPUT_INITIALIZE(LS020_RESET_PORT, LS020_RESET_DDR, LS020_RST, LS020_RST); \
		} while(0)

	#define LS020_RESET_PORT_S(b)	do { LS020_RESET_PORT |= (b); } while (0)
	#define LS020_RESET_PORT_C(b)	do { LS020_RESET_PORT &= ~ (b); } while (0)
	#define LS020_RS_PORT_S(b)		do { LS020_RS_PORT |= (b); } while (0)
	#define LS020_RS_PORT_C(b)		do { LS020_RS_PORT &= ~ (b); } while (0)

	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		#define HARDWARE_KBD_INITIALIZE() do { \
				KBD_TARGET_PORT |= KBD_MASK;	/* tie up inputs */ \
				KBD_TARGET_DDR &= ~ KBD_MASK;	/* define these bits as inputs - enable pull-up */ \
			} while (0)
	#endif

#if SPI_BIDIRECTIONAL && CPUSTYLE_ATMEGA

	#define SPIIO_MOSI_TO_INPUT() do { \
		SPI_TARGET_MOSI_DDR &= ~ SPI_MOSI_BIT;	/* РїРµСЂРµРєР»СЋС‡РёС‚СЊ РїРѕСЂС‚ РЅР° С‡С‚РµРЅРёРµ СЃ РІС‹РІРѕРґРѕРІ */ \
		} while (0)
	#define SPIIO_MOSI_TO_OUTPUT() do { \
		SPI_TARGET_MOSI_DDR |= SPI_MOSI_BIT;	/* РѕС‚РєСЂС‹С‚СЊ РІС‹С…РѕРґС‹ РїРѕСЂС‚Р° */ \
		} while (0)

#endif /* SPI_BIDIRECTIONAL && CPUSTYLE_ATMEGA */


#if CPUSTYLE_ATMEGA328

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
		PTT_TARGET_PORT |= PTT_BIT_PTT;	/* РІРєР»СЋС‡С‚СЊ pull-up РЅР° СЌС‚РѕС‚ РІС…РѕРґ. */ \
		PTT_TARGET_DDR &= ~ PTT_BIT_PTT;	/* Р±РёС‚ РІС…РѕРґР° PTT */ \
	} while (0)

	#define ELKEY_INITIALIZE() do { \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_LEFT;	/* Р±РёС‚ РІС…РѕРґР° Р»РµРІРѕРіРѕ РєРѕРЅС‚Р°РєС‚Р° РјР°РЅРёРїСѓР»СЏС‚РѕСЂР° */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_LEFT; \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_RIGHT;	/* Р±РёС‚ РІС…РѕРґР° РїСЂР°РІРѕРіРѕ РєРѕРЅС‚Р°РєС‚Р° РјР°РЅРёРїСѓР»СЏС‚РѕСЂР° */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_RIGHT; \
		/* PCMSK_PORT |= (ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РЅР°Р¶Р°С‚РёСЏРј РЅР° РєРѕРЅС‚Р°РєС‚С‹ РєР»СЋС‡Р° */ \
		/* PCICR |= (PCICR_BIT); */ \
	} while (0)

	#define NMEA_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* Р±РёС‚ РІС…РѕРґР° DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РёР·РјРµРЅРµРЅРёСЋ СЃРѕСЃС‚РѕСЏРЅРёСЏ DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РёР·РјРµРЅРµРЅРёСЋ СЃРѕСЃС‚РѕСЏРЅРёСЏ DTR */ \
	} while (0)

	#define FROMCAT_DTR_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* Р±РёС‚ РІС…РѕРґР° DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РёР·РјРµРЅРµРЅРёСЋ СЃРѕСЃС‚РѕСЏРЅРёСЏ DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РёР·РјРµРЅРµРЅРёСЋ СЃРѕСЃС‚РѕСЏРЅРёСЏ DTR */ \
	} while (0)

	// FROMCAT_RTS_INITIALIZE variations
	#if FROMCAT_BIT_RTS != 0
		#define FROMCAT_RTS_INITIALIZE() do { \
			FROMCAT_TARGET_PORT_RTS |= FROMCAT_BIT_RTS;		/* enable pull-up */ \
			FROMCAT_TARGET_DDR_RTS &= ~ FROMCAT_BIT_RTS;	/* Р±РёС‚ РІС…РѕРґР° RTS */ \
		} while (0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)
	#endif /* FROMCAT_BIT_RTS != 0 */

	#define ENCODER_INITIALIZE() do { \
		EICRA |= 0x05; \
		EIMSK |= 0x03; \
		EIFR |= 0x03; \
		ENCODER_TARGET_PORT |= ENCODER_BITS; /* PULL-UP */  \
		ENCODER_DIRECTION_PORT &= ~ ENCODER_BITS; \
	} while (0)

#elif CPUSTYLE_ATMEGA_XXX4

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
		PTT_TARGET_PORT |= PTT_BIT_PTT;	/* РІРєР»СЋС‡С‚СЊ pull-up РЅР° СЌС‚РѕС‚ РІС…РѕРґ. */ \
		PTT_TARGET_DDR &= ~ PTT_BIT_PTT;	/* Р±РёС‚ РІС…РѕРґР° PTT */ \
	} while (0)

	#define ELKEY_INITIALIZE() do { \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_LEFT;	/* Р±РёС‚ РІС…РѕРґР° Р»РµРІРѕРіРѕ РєРѕРЅС‚Р°РєС‚Р° РјР°РЅРёРїСѓР»СЏС‚РѕСЂР° */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_LEFT; \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_RIGHT;	/* Р±РёС‚ РІС…РѕРґР° РїСЂР°РІРѕРіРѕ РєРѕРЅС‚Р°РєС‚Р° РјР°РЅРёРїСѓР»СЏС‚РѕСЂР° */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_RIGHT; \
		/* PCMSK_PORT |= (ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РЅР°Р¶Р°С‚РёСЏРј РЅР° РєРѕРЅС‚Р°РєС‚С‹ РєР»СЋС‡Р° */ \
		/* PCICR |= (PCICR_BIT); */ \
	} while (0)

	#define NMEA_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* Р±РёС‚ РІС…РѕРґР° DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РёР·РјРµРЅРµРЅРёСЋ СЃРѕСЃС‚РѕСЏРЅРёСЏ DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РёР·РјРµРЅРµРЅРёСЋ СЃРѕСЃС‚РѕСЏРЅРёСЏ DTR */ \
	} while (0)

	#define FROMCAT_DTR_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* Р±РёС‚ РІС…РѕРґР° DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РёР·РјРµРЅРµРЅРёСЋ СЃРѕСЃС‚РѕСЏРЅРёСЏ DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РёР·РјРµРЅРµРЅРёСЋ СЃРѕСЃС‚РѕСЏРЅРёСЏ DTR */ \
	} while (0)

	// FROMCAT_RTS_INITIALIZE variations
	#if FROMCAT_BIT_RTS != 0
		#define FROMCAT_RTS_INITIALIZE() do { \
			FROMCAT_TARGET_PORT_RTS |= FROMCAT_BIT_RTS;		/* enable pull-up */ \
			FROMCAT_TARGET_DDR_RTS &= ~ FROMCAT_BIT_RTS;	/* Р±РёС‚ РІС…РѕРґР° RTS */ \
		} while (0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)
	#endif /* FROMCAT_BIT_RTS != 0 */

	#define ENCODER_INITIALIZE() do { \
		EICRA |= 0x05; \
		EIMSK |= 0x03; \
		EIFR |= 0x03; \
		ENCODER_TARGET_PORT |= ENCODER_BITS; /* PULL-UP */  \
		ENCODER_DIRECTION_PORT &= ~ ENCODER_BITS; \
	} while (0)

#elif CPUSTYLE_ATMEGA32

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() do { \
		PTT_TARGET_PORT |= PTT_BIT_PTT;	/* РІРєР»СЋС‡С‚СЊ pull-up РЅР° СЌС‚РѕС‚ РІС…РѕРґ. */ \
		PTT_TARGET_DDR &= ~ PTT_BIT_PTT;	/* Р±РёС‚ РІС…РѕРґР° PTT */ \
	} while (0)

	#define ELKEY_INITIALIZE() do { \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_LEFT;	/* Р±РёС‚ РІС…РѕРґР° Р»РµРІРѕРіРѕ РєРѕРЅС‚Р°РєС‚Р° РјР°РЅРёРїСѓР»СЏС‚РѕСЂР° */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_LEFT; \
		ELKEY_TARGET_DDR &= ~ ELKEY_BIT_RIGHT;	/* Р±РёС‚ РІС…РѕРґР° РїСЂР°РІРѕРіРѕ РєРѕРЅС‚Р°РєС‚Р° РјР°РЅРёРїСѓР»СЏС‚РѕСЂР° */ \
		ELKEY_TARGET_PORT |= ELKEY_BIT_RIGHT; \
	} while (0)

	#define NMEA_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* Р±РёС‚ РІС…РѕРґР° DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РёР·РјРµРЅРµРЅРёСЋ СЃРѕСЃС‚РѕСЏРЅРёСЏ DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РёР·РјРµРЅРµРЅРёСЋ СЃРѕСЃС‚РѕСЏРЅРёСЏ DTR */ \
	} while (0)

	#define FROMCAT_DTR_INITIALIZE() do { \
		FROMCAT_TARGET_PORT_DTR |= FROMCAT_BIT_DTR;		/* enable pull-up */ \
		FROMCAT_TARGET_DDR_DTR &= ~ FROMCAT_BIT_DTR;	/* Р±РёС‚ РІС…РѕРґР° DTR */ \
		/* DTRPCMSK_PORT |= (FROMCAT_BIT_DTR); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РёР·РјРµРЅРµРЅРёСЋ СЃРѕСЃС‚РѕСЏРЅРёСЏ DTR */ \
		/* PCICR |= (DTRPCICR_BIT); */ /* СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РёР·РјРµРЅРµРЅРёСЋ СЃРѕСЃС‚РѕСЏРЅРёСЏ DTR */ \
	} while (0)

	// FROMCAT_RTS_INITIALIZE variations
	#if FROMCAT_BIT_RTS != 0
		#define FROMCAT_RTS_INITIALIZE() do { \
			FROMCAT_TARGET_PORT_RTS |= FROMCAT_BIT_RTS;		/* enable pull-up */ \
			FROMCAT_TARGET_DDR_RTS &= ~ FROMCAT_BIT_RTS;	/* Р±РёС‚ РІС…РѕРґР° RTS */ \
		} while (0)
	#else /* FROMCAT_BIT_RTS != 0 */
		#define FROMCAT_RTS_INITIALIZE() do { \
		} while (0)
	#endif /* FROMCAT_BIT_RTS != 0 */

	#define ENCODER_INITIALIZE() do { \
		GICR |= 0xC0; \
		MCUCR = 0x05; \
		MCUCSR = 0x00; \
		GIFR = 0xC0; \
		ENCODER_TARGET_PORT |= ENCODER_BITS; /* PULL-UP */  \
		ENCODER_DIRECTION_PORT &= ~ ENCODER_BITS; \
	} while (0)

#endif /* CPUSTYLE_ATMEGA_XXX4 */

#if CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA

	// Variations of SPI_ADDRESS_AEN_INITIALIZE
	// Variations of SPI_ADDRESS_NAEN_INITIALIZE
	#if SPI_AEN_BIT != 0

		#define SPI_ADDRESS_AEN_INITIALIZE() do { \
			/* РїСЂРѕРіСЂР°РјРјРёСЂРѕРІР°РЅРёРµ СЃРёРіРЅР°Р»РѕРІ Р°РґСЂРµСЃР° SPI */ \
			SPI_ADDRESS_DDR |= SPI_ADDRESS_BITS; \
			/* РїСЂРѕРіСЂР°РјРјРёСЂРѕРІР°РЅРёРµ SS SPI */ \
			SPI_AEN_PORT &= ~ (SPI_AEN_BIT); \
			SPI_AEN_DDR |= (SPI_AEN_BIT); \
			} while (0)

		#define SPI_ADDRESS_NAEN_INITIALIZE() do { \
			} while (0)

	#elif SPI_NAEN_BIT != 0

		#define SPI_ADDRESS_NAEN_INITIALIZE() do { \
			/* РїСЂРѕРіСЂР°РјРјРёСЂРѕРІР°РЅРёРµ СЃРёРіРЅР°Р»РѕРІ Р°РґСЂРµСЃР° SPI */ \
			SPI_ADDRESS_DDR |= SPI_ADDRESS_BITS; \
			/* РїСЂРѕРіСЂР°РјРјРёСЂРѕРІР°РЅРёРµ SS SPI */ \
			SPI_NAEN_PORT &= ~ (SPI_NAEN_BIT); \
			SPI_NAEN_DDR |= (SPI_NAEN_BIT); \
			} while (0)

		#define SPI_ADDRESS_AEN_INITIALIZE() do { \
			} while (0)

	#else /* SPI_AEN_BIT != 0 */

		#define SPI_ADDRESS_NAEN_INITIALIZE() do { \
			} while (0)

		#define SPI_ADDRESS_AEN_INITIALIZE() do { \
			} while (0)

	#endif /* SPI_AEN_BIT != 0 */

	// Variations of SPI_IOUPDATE_INITIALIZE
	#if SPI_IOUPDATE_BIT != 0

		#define SPI_IOUPDATE_INITIALIZE() do { \
			SPI_IOUPDATE_PORT |= SPI_IOUPDATE_BIT;	/* РќРµР°РєС‚РёРІРЅРѕРµ СЃРѕСЃС‚РѕСЏРЅРёРµ - "1" */ \
			SPI_IOUPDATE_DDR |= SPI_IOUPDATE_BIT; \
			} while (0)

	#else /* SPI_IOUPDATE_BIT != 0 */

		#define SPI_IOUPDATE_INITIALIZE() do { \
			} while (0)

	#endif /* SPI_IOUPDATE_BIT != 0 */

	// Variations of SPI_IORESET_INITIALIZE
	#if SPI_IORESET_BIT != 0

		// РїСЂРѕСЃС‚Рѕ СЃР±СЂР°СЃС‹РІР°РµРј. РЈРїСЂР°РІР»РµРЅРёСЏ С‡РµСЂРµР· СЌС‚РѕС‚ РІС‹РІРѕРґ РЅРµ РёРґС‘С‚.
		// Р’ РЅРѕРІС‹С… РїР»Р°С‚Р°С… СЌС‚РѕРіРѕ Р±РёС‚Р° РЅРµС‚.

		#define SPI_IORESET_INITIALIZE() do { \
			SPI_IORESET_PORT &= ~ SPI_IORESET_BIT; \
			SPI_IORESET_DDR |= SPI_IORESET_BIT; \
			} while (0)

	#else /* SPI_IOUPDATE_BIT != 0 */

		#define SPI_IORESET_INITIALIZE() do { \
			} while (0)

	#endif /* SPI_IOUPDATE_BIT != 0 */

#endif /* CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA */

#endif /* ATMEGA_CPUSTYLE_V5_H_INCLUDED */
