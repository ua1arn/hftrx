/* $Id$ */
/* board-specific CPU DIP28/TQFP32 attached signals */
// companion file for boards/atmega328_ctl_4z5ky_v1.h
//
/* РЎРёРЅС‚РµР·Р°С‚РѕСЂ 4Z5KY СЃ РґРІСѓС…СЃС‚СЂРѕС‡РЅРёРєРѕРј http://www.cqham.ru/trx92_19.htm */
/* AD9951+AD9834 */
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//

#ifndef ATMEGA328_CPU_4Z5KY_V1_H_INCLUDED
#define ATMEGA328_CPU_4Z5KY_V1_H_INCLUDED 1

//#define WITHSPIHW 		1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ Р°РїРїР°СЂР°С‚РЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° SPI */
//#define WITHSPIHWDMA 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ DMA РїСЂРё РѕР±РјРµРЅРµ РїРѕ SPI */
#define WITHSPISW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РїСЂРѕРіСЂР°РјРјРЅРѕРіРѕ СѓРїСЂР°РІР»РµРЅРёСЏ SPI. */
//#define WITHTWIHW	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ Р°РїРїР°СЂР°С‚РЅС‹Р№ РєРѕРЅС‚СЂРѕР»Р»РµСЂ TWI (I2C) */
#undef WITHELKEY
#define WITHCPUADCHW 	1	/* РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ ADC */

#define WITHUART1HW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёР№РЅС‹Р№ РєРѕРЅС‚СЂРѕР»Р»РµСЂ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕРіРѕ РїРѕСЂС‚Р° #1 */
//#define WITHUART2HW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёР№РЅС‹Р№ РєРѕРЅС‚СЂРѕР»Р»РµСЂ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕРіРѕ РїРѕСЂС‚Р° #2 */

//#define WITHCAT_CDC		1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РІРёСЂС‚СѓР°Р»СЊРЅС‹Р№ РІРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅС‹Р№ РїРѕСЂС‚ РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
#define WITHCAT_USART1		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART1		1

#if LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	// РґР°РЅРЅС‹Рµ LCD
	#define LCD_DATA_PORT PORTB
	#define LCD_DATA_DIRECTION DDRB
	#define LCD_DATA_INPUT PINB

	#define LCD_DATAS_BIT_LOW PB0		// РєР°РєРѕР№ Р±РёС‚ РґР°РЅРЅС‹С… РјР»Р°РґС€РёР№ РІ СЃР»РѕРІРµ СЃС‡РёС‚Р°РЅРЅРѕРј СЃ РїРѕСЂС‚Р°
	#define LCD_DATAS_BITS (0x0f << LCD_DATAS_BIT_LOW)

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* РїРѕР»СѓС‡РёС‚СЊ РґР°РЅРЅС‹Рµ СЃ С€РёРЅС‹ LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { \
		LCD_DATA_PORT = \
			(LCD_DATA_PORT & ~ LCD_DATAS_BITS) | \
			(((v) << LCD_DATAS_BIT_LOW) & LCD_DATAS_BITS); \
		} while (0)

	// СЃС‚СЂРѕР± E LCD
	#define LCD_STROBE_PORT PORTB
	#define LCD_STROBE_DIRECTION DDRB
	#define LCD_STROBE_BIT (1U << PB4)

	// СЃРёРіРЅР°Р»С‹ RS Рё WE LCD
	#define LCD_RS_WE_PORT PORTB
	#define LCD_RS_WE_DIRECTION DDRB
	#define ADDRES_BIT (1U << PB5)
	//#define WRITEE_BIT (1U << PB1)
	//#define WRITEE_BIT_ZERO (1U << PB1)		// РґР»СЏ С‚РµСЃС‚РёСЂРѕРІР°РЅРёСЏ РѕР±РјРµРЅР° СЃ HD44780 РІСЃРµРіРґР° РЅР° Р·Р°РїРёСЃРё

#elif LCDMODE_SPI_RA || LCDMODE_SPI_NA || LCDMODE_SPI_RN

	#define LS020_RESET_PORT PORTB
	#define LS020_RESET_DDR DDRB
	#define LS020_RST	(1U << PB0)			// D4 signal in HD44780 socket

	#define LS020_RS_PORT PORTB
	#define LS020_RS_DDR DDRB
	#define LS020_RS	(1U << PB1)			// D5 signal in HD44780 socket

	#define SPI2_TARGET_PORT PORTB
	#define SPI2_TARGET_DDR DDRB
	#define SPI2_TARGET_PIN PINB

	#define	SPI2_CS_BIT (1U << PB2)	// SS Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІС‹Р±РѕСЂ СѓСЃС‚СЂРѕР№СЃС‚Р° SPI
	#define	SPI2_MOSI_BIT (1U << PB3)	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІС‹РІРѕРґ (РёР»Рё РІРІРѕРґ РІ СЃР»СѓС‡Р°Рµ РґРІСѓРЅР°РїСЂР°РІР»РµРЅРЅРѕРіРѕ SPI).
	// PB4 - MISO - do not use
	#define	SPI2_SCLK_BIT (1U << PB5)	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ СЃРёРЅС…СЂРѕРЅРёР·Р°С†РёСЏ SPI

	#define	SPI_CSEL2	3		// LCD

#endif

#if WITHENCODER

	#define ENCODER_TARGET_PORT PORTD
	#define ENCODER_DIRECTION_PORT DDRD
	#define ENCODER_INPUT_PORT PIND

	#define ENCODER_SHIFT PIND2
	#define ENCODER_BITS ((1U << PIND3) | (1U << PIND2))

#endif

	/* РїРµСЂРµРєР»СЋС‡РµРЅРёРµ Р±РёС‚Р° СѓРїСЂР°РІР»РµРЅРёСЏ РЅРµСЃСѓС‰РµР№ РІС‹Р·С‹РІР°РµС‚СЃСЏ РёР· РїСЂРµСЂС‹РІР°РЅРёР№ -
	 РІСЃРµРј РѕСЃС‚Р°Р»СЊРЅС‹Рј, РјРѕРґРёС„РёС†РёСЂСѓСЋС‰РёРј Р±РёС‚С‹ РЅР° СЌС‚РѕРј РїРѕСЂС‚Сѓ, РІС‹РїРѕР»РЅСЏС‚СЊ РїРѕРґ Р·Р°РїСЂРµС‚РѕРј РїСЂРµСЂС‹РІР°РЅРёР№.
		*/
#if 0
	#define TXPATH_TARGET_PORT PORTD	// РІС‹С…РѕРґС‹ РїСЂРѕС†РµСЃСЃРѕСЂР° - СѓРїСЂР°РІР»РµРЅРёРµ С‚СЂР°РєС‚РѕРј РїРїРµСЂРµРґР°С‡Рё Рё РјР°РЅРёРїСѓР»СЏС†РёРµР№
	#define TXPATH_TARGET_DDR DDRD		// РїРµСЂРµРєР»СЋС‡РµРЅРёРµ РЅР° РІС‹РІРѕРґ - СѓРїСЂР°РІР»РµРЅРёРµ С‚СЂР°РєС‚РѕРј РїРµСЂРµРґР°С‡Рё Рё РјР°РЅРёРїСѓР»СЏС†РёРµР№

	#if ! ELKEY328
		// РЈРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - СЃРёРіРЅР°Р»С‹ TXPATH_ENABLE (PA11) Рё TXPATH_ENABLE_CW (PA10) - Р°РєС‚РёРІРЅС‹ РїСЂРё РЅСѓР»Рµ РЅР° РІС‹С…РѕРґРµ.
		#define TXPATH_BIT_ENABLE_SSB		(1U << PD5)
		#define TXPATH_BIT_ENABLE_CW	(1U << PD7)
		#define TXPATH_BITS_ENABLE	(TXPATH_BIT_ENABLE_SSB | TXPATH_BIT_ENABLE_CW)
		// РџРѕРґРіРѕС‚РѕРІР»РµРЅРЅС‹Рµ СѓРїСЂР°РІР»СЏСЋС‰РёРµ СЃР»РѕРІР°
		#define TXGFV_RX		0
		#define TXGFV_TRANS		0			// РїРµСЂРµС…РѕРґ РјРµР¶РґСѓ СЂРµР¶РёРјР°РјРё РїСЂРёС‘РјР° Рё РїРµСЂРµРґР°С‡Рё
		#define TXGFV_TX_SSB	TXPATH_BIT_ENABLE_SSB
		#define TXGFV_TX_CW		TXPATH_BIT_ENABLE_CW
		#define TXGFV_TX_AM		TXPATH_BIT_ENABLE_CW
		#define TXGFV_TX_NFM	TXPATH_BIT_ENABLE_CW
	#else
		// РЈРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - РµРґРёРЅСЃС‚РІРµРЅРЅС‹Р№ СЃРёРіРЅР°Р» СЂР°Р·СЂРµС€РµРЅРёСЏ С‚СЂР°РєС‚Р°
		#define TXPATH_BIT_GATE (1U << PD5)	// РІС‹С…РѕРґРЅРѕР№ СЃРёРіРЅР°Р» РёР· РїСЂРѕС†РµСЃСЃРѕСЂР° - СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј.
		//#define TXPATH_BIT_GATE_RX TXPATH_BIT_GATE	// СЃРёРіРЅР°Р» tx2 - СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј. РџСЂРё РїСЂРёС‘РјРµ Р°РєС‚РёРІРµРЅ
		#define TXPATH_BIT_GATE_RX 0	// СЃРёРіРЅР°Р» tx2 - СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј. РџСЂРё РїСЂРёС‘РјРµ РЅРµ Р°РєС‚РёРІРµРЅ
		// РџРѕРґРіРѕС‚РѕРІР»РµРЅРЅС‹Рµ СѓРїСЂР°РІР»СЏСЋС‰РёРµ СЃР»РѕРІР°
		#define TXGFV_RX		TXPATH_BIT_GATE_RX
		#define TXGFV_TRANS		0			// РїРµСЂРµС…РѕРґ РјРµР¶РґСѓ СЂРµР¶РёРјР°РјРё РїСЂРёС‘РјР° Рё РїРµСЂРµРґР°С‡Рё
		#define TXGFV_TX_SSB	TXPATH_BIT_GATE
		#define TXGFV_TX_CW		TXPATH_BIT_GATE
		#define TXGFV_TX_AM		TXPATH_BIT_GATE
		#define TXGFV_TX_NFM	TXPATH_BIT_GATE
	#endif
#else
	// РџРѕРґРіРѕС‚РѕРІР»РµРЅРЅС‹Рµ СѓРїСЂР°РІР»СЏСЋС‰РёРµ СЃР»РѕРІР° (Р·Р°РіР»СѓС€РєРё)
	#define TXGFV_RX		0
	#define TXGFV_TRANS		0			// РїРµСЂРµС…РѕРґ РјРµР¶РґСѓ СЂРµР¶РёРјР°РјРё РїСЂРёС‘РјР° Рё РїРµСЂРµРґР°С‡Рё
	#define TXGFV_TX_SSB	0
	#define TXGFV_TX_CW		0
	#define TXGFV_TX_AM		0
	#define TXGFV_TX_NFM	0

#endif

#if 0
	// РїСЂРѕС†РµСЃСЃРѕСЂ С„РѕСЂРјРёСЂСѓРµС‚ Р·РІСѓРє СЃР°РјРѕРєРѕРЅС‚СЂРѕР»СЏ РїСЂРё РїРµСЂРµРґР°С‡Рµ С‚РµР»РµРіСЂР°С„РѕРј
	// РіРµРЅРµСЂР°С†РёСЏ СЃРёРіРЅР°Р»Р° СЃР°РјРѕРєРѕРЅС‚СЂРѕР»СЏ РЅР° PD7(OC2)
	/* СЃР°РјРѕРєРѕРЅС‚СЂРѕР»СЊ РїРµСЂРµРґР°С‡Рё С‚РµР»РµРіСЂР°С„Р° Рё РѕР·РІСѓС‡РєР° РЅР°Р¶Р°С‚РёР№ */
	#define SIDETONE_TARGET_PORT	PORTD	/* РџРѕСЂС‚ РїСЂРѕС†РµСЃСЃРѕСЂР°, РЅР° РєРѕС‚РѕСЂРѕРј РЅР°С…РѕРґРёС‚СЃСЏ РІС‹РІРѕРґ С‚РѕРЅР° СЃР°РјРѕРєРѕРЅС‚СЂРѕР»СЏ */
	#define SIDETONE_TARGET_DDR		DDRD
	#define SIDETONE_TARGET_BIT		(1U << PD6)
#endif

	#define TARGET_TWI_TWCK_PORT PORTC
	#define TARGET_TWI_TWCK_DDR DDRC
	#define TARGET_TWI_TWCK_PIN PINC
	#define TARGET_TWI_TWCK	(1U << PC5)

	#define TARGET_TWI_TWD_PORT PORTC
	#define TARGET_TWI_TWD_DDR DDRC
	#define TARGET_TWI_TWD_PIN PINC
	#define TARGET_TWI_TWD	(1U << PC4)

#if WITHTX
	// PTT

	#define PTT_TARGET_PORT PORTD	
	#define PTT_TARGET_DDR DDRD
	#define PTT_TARGET_PIN PIND

	#define PTT_BIT_PTT (1U << PD7)		

	#if 0 && (WITHCAT || WITHNMEA)
		// РЎРёРіРЅР°Р»С‹ РїРµСЂРµРєР»СЋС‡РµРЅРёСЏ РЅР° РїРµСЂРµРґР°С‡Сѓ Рё РјР°РЅРёРїСѓР»СЏС†РёРё С‡РµСЂРµР· CAT РёРЅС‚РµСЂС„РµР№СЃ.
		#define FROMCAT_TARGET_PIN_RTS		PIND
		#define FROMCAT_TARGET_DDR_RTS		DDRD
		#define FROMCAT_BIT_RTS			(1U << PD4)	/* СЃРёРіРЅР°Р» RTS РѕС‚ FT232RL	*/
		/* РјР°РЅРёРїСѓР»СЏС†РёСЏ РѕС‚ РїРѕСЂС‚Р° RS-232, СЃРёРіРЅР°Р» PPS РѕС‚ GPS/GLONASS/GALILEO РјРѕРґСѓР»СЏ */
		#define FROMCAT_TARGET_PIN_DTR		PINB
		#define FROMCAT_TARGET_DDR_DTR		DDRB
		#define FROMCAT_BIT_DTR			(1U << PB3)	/* СЃРёРіРЅР°Р» DTR РѕС‚ FT232RL	*/
		// СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РёР·РјРµРЅРµРЅРёСЋ СЃРѕСЃС‚РѕСЏРЅРёСЏ РЅР° РІС…РѕРґРµ РјР°РЅРёРїСѓР»СЏС†РёРё РїРѕ CAT
		// #2 at ATMega328 - PORT D
		#define DTRPCMSK_PORT	PCMSK0			// PCMSK0 - PB, PCMSK1 - PC, PCMSK2 - PD
		#define DTRPCICR_BIT	(1U << PCIE0)	// PCIE0 - PB, PCIE1 - PC, PCIE2 - PD
		#define DTRPCIVECT PCINT0_vect			// РІРµРєС‚РѕСЂ РѕР±СЂР°Р±РѕС‚С‡РёРєР°
	#endif /* WITHCAT */

	#if 0 && WITHELKEY
		#define ELKEY_TARGET_PORT PORTD		// РІС…РѕРґС‹ РїСЂРѕС†РµСЃСЃРѕСЂР° - СЃРёРіРЅР°Р»С‹ СЌР»РµРєС‚СЂРѕРЅРЅРѕРіРѕ РєР»СЋС‡Р°
		#define ELKEY_TARGET_DDR DDRD		// РїРµСЂРµРєР»СЋС‡РµРЅРёРµ РЅР° РІРІРѕРґ - СЃРёРіРЅР°Р»С‹ СЌР»РµРєС‚СЂРѕРЅРЅРѕРіРѕ РєР»СЋС‡Р°
		#define ELKEY_TARGET_PIN PIND		// РІРІРѕРґ - СЃРёРіРЅР°Р»С‹ СЌР»РµРєС‚СЂРѕРЅРЅРѕРіРѕ РєР»СЋС‡Р°

		#define ELKEY_BIT_LEFT (1U << PD0)
		#define ELKEY_BIT_RIGHT (1U << PD1)
		// СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РЅР°Р¶Р°С‚РёСЏРј РЅР° РєРѕРЅС‚Р°РєС‚С‹ РєР»СЋС‡Р°
		// РњРёРєСЂРѕРїСЂРѕС†РµСЃСЃРѕСЂР°С… РЅР°РїРѕРґРѕР±РёРё ATMega644, ATMega328
		// #2 at ATMega328 - PORT D
		#define PCMSK_PORT	PCMSK2			// PCMSK0 - PB, PCMSK1 - PC, PCMSK2 - PD
		#define PCICR_BIT	(1U << PCIE2)	// PCIE0 - PB, PCIE1 - PC, PCIE2 - PD
		#define PCIVECT PCINT2_vect			// РІРµРєС‚РѕСЂ РѕР±СЂР°Р±РѕС‚С‡РёРєР°
	#endif

#endif /* WITHTX */


	#define HARDWARE_SPI_CONNECT() do { \
		} while (0)

	#define HARDWARE_SPI_DISCONNECT() do { \
			SPCR = 0x00; \
		} while (0)
	//#define SPI_IOUPDATE_BIT (1U << PB3)	// РІРјРµСЃС‚Рѕ РїРѕСЂС‚Р° РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІС‹С…РѕРґ РґРµС€РёС„СЂР°С‚РѕСЂР° CS РёРЅС‚РµСЂС„РµР№СЃР° SPI

	// PC3: IOUPDATE
	#define SPI_IOUPDATE_PORT PORTC
	#define SPI_IOUPDATE_DDR DDRC
	#define SPI_IOUPDATE_BIT (1U << PC3)	// IOUPDATE

	#define WITHSPISPLIT	1	/* РґР»СЏ РґРІСѓС… СЂР°Р·РЅС‹С… РїРѕС‚СЂРµР±РёС‚РµР»РµР№ С„РѕСЂРјРёСЂСѓСЋС‚СЃСЏ РѕС‚РґРµР»СЊРЅС‹Рµ СЃРёРіРЅР°Р»С‹ MOSI, SCK, CS */
	#define SPI_ALLCS_BITS	0		// С‚СЂРµР±СѓРµС‚СЃСЏ РґР»СЏ СѓРєР°Р·Р°РЅРёСЏ С‚РѕРіРѕ, С‡С‚Рѕ СЂР°Р±РѕС‚Р° СЃ РїСЂСЏРјС‹Рј РІС‹Р±РѕСЂРѕРј CS (Р±РµР· РґРµС€РёС„СЂР°С‚РѕСЂР°) РЅРµ С‚СЂРµР±СѓРµС‚СЃСЏ
	
	// AD9951: PC0: RESET
	#define	TARGET_DDSRESET_PORT	PORTC
	#define	TARGET_DDSRESET_DIR	DDRC
	#define	TARGET_DDSRESET_BIT	(1U << PC0)

	#define	TARGET_DDSRESET_PORT_C(v) do { TARGET_DDSRESET_PORT &= ~ (v); } while (0)
	#define	TARGET_DDSRESET_PORT_S(v) do { TARGET_DDSRESET_PORT |= (v); } while (0)

	#define TARGET_DDSRESET_INITIALIZE() do { \
		TARGET_DDSRESET_DIR |= TARGET_DDSRESET_BIT; \
	} while (0)

	// AD9951: PC1: SCLK
	#define SPI0_TARGET_SCLK_PORT PORTC
	#define SPI0_TARGET_SCLK_DDR DDRC
	#define SPI0_TARGET_SCLK_PIN PINC
	#define	SPI0_SCLK_BIT (1U << PC1)	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ СЃРёРЅС…СЂРѕРЅРёР·Р°С†РёСЏ SPI

	// AD9951: PB3: SCLK
	#define SPI0_TARGET_MOSI_PORT PORTB
	#define SPI0_TARGET_MOSI_DDR DDRB
	#define SPI0_TARGET_MOSI_PIN PINB
	#define	SPI0_MOSI_BIT (1U << PB3)	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІС‹РІРѕРґ (РёР»Рё РІРІРѕРґ РІ СЃР»СѓС‡Р°Рµ РґРІСѓРЅР°РїСЂР°РІР»РµРЅРЅРѕРіРѕ SPI).

	// AD9951: PC0: RESET
	#define SPI0_TARGET_CS_PORT PORTC
	#define SPI0_TARGET_CS_DDR DDRC
	#define SPI0_TARGET_CS_PIN PINC
	#define	SPI0_CS_BIT 0//(1U << PC3)	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІС‹Р±РѕСЂ СѓСЃС‚СЂРѕР№СЃС‚Р° SPI

	#define	SPI_CSEL0	0

	// AD9834: PD4: SCLK
	#define SPI1_TARGET_SCLK_PORT PORTD
	#define SPI1_TARGET_SCLK_DDR DDRD
	#define SPI1_TARGET_SCLK_PIN PIND
	#define	SPI1_SCLK_BIT (1U << PD4)	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ СЃРёРЅС…СЂРѕРЅРёР·Р°С†РёСЏ SPI

	// AD9834: PB2: MOSI
	#define SPI1_TARGET_MOSI_PORT PORTB
	#define SPI1_TARGET_MOSI_DDR DDRB
	#define SPI1_TARGET_MOSI_PIN PINB
	#define	SPI1_MOSI_BIT (1U << PB2)	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІС‹РІРѕРґ (РёР»Рё РІРІРѕРґ РІ СЃР»СѓС‡Р°Рµ РґРІСѓРЅР°РїСЂР°РІР»РµРЅРЅРѕРіРѕ SPI).

	// AD9834: PC2: CS (FSYNC)
	#define SPI1_TARGET_CS_PORT PORTC
	#define SPI1_TARGET_CS_DDR DDRC
	#define SPI1_TARGET_CS_PIN PINC
	#define	SPI1_CS_BIT (1U << PC2)	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІС‹Р±РѕСЂ СѓСЃС‚СЂРѕР№СЃС‚Р° SPI

	#define	SPI_CSEL1	1

	// CONTROL REGISTER: PD5: SCLK (74HC595, pin 11)
	#define SPI2_TARGET_SCLK_PORT PORTD
	#define SPI2_TARGET_SCLK_DDR DDRD
	#define SPI2_TARGET_SCLK_PIN PIND
	#define	SPI2_SCLK_BIT (1U << PD5)	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ СЃРёРЅС…СЂРѕРЅРёР·Р°С†РёСЏ SPI

	// CONTROL REGISTER: PB1: MOSI
	#define SPI2_TARGET_MOSI_PORT PORTB
	#define SPI2_TARGET_MOSI_DDR DDRB
	#define SPI2_TARGET_MOSI_PIN PINB
	#define	SPI2_MOSI_BIT (1U << PB1)	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІС‹РІРѕРґ (РёР»Рё РІРІРѕРґ РІ СЃР»СѓС‡Р°Рµ РґРІСѓРЅР°РїСЂР°РІР»РµРЅРЅРѕРіРѕ SPI).

	// CONTROL REGISTER: PD6: CS (74HC595, pin 12)
	#define SPI2_TARGET_CS_PORT PORTD
	#define SPI2_TARGET_CS_DDR DDRD
	#define SPI2_TARGET_CS_PIN PIND
	#define	SPI2_CS_BIT (1U << PD6)	// Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІС‹Р±РѕСЂ СѓСЃС‚СЂРѕР№СЃС‚Р° SPI

	#define SPI_CSEL2	2

	#define SPIIO_INITIALIZE() do { \
		/* РґР»СЏ РґРІСѓС… СЂР°Р·РЅС‹С… РїРѕС‚СЂРµР±РёС‚РµР»РµР№ С„РѕСЂРјРёСЂСѓСЋС‚СЃСЏ РѕС‚РґРµР»СЊРЅС‹Рµ СЃРёРіРЅР°Р»С‹ MOSI, SCK, CS */ \
		SPI0_TARGET_SCLK_PORT |= (SPI0_SCLK_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
		SPI0_TARGET_SCLK_DDR |= (SPI0_SCLK_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
		SPI0_TARGET_MOSI_PORT |= (SPI0_MOSI_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
		SPI0_TARGET_MOSI_DDR |= (SPI0_MOSI_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
		\
		SPI1_TARGET_SCLK_PORT |= (SPI1_SCLK_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
		SPI1_TARGET_SCLK_DDR |= (SPI1_SCLK_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
		SPI1_TARGET_MOSI_PORT |= (SPI1_MOSI_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
		SPI1_TARGET_MOSI_DDR |= (SPI1_MOSI_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
		\
		SPI2_TARGET_SCLK_PORT |= (SPI2_SCLK_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
		SPI2_TARGET_SCLK_DDR |= (SPI2_SCLK_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
		SPI2_TARGET_MOSI_PORT |= (SPI2_MOSI_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
		SPI2_TARGET_MOSI_DDR |= (SPI2_MOSI_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
		} while (0)

	#define SPI_ALLCS_INITIALIZE() do { \
			/* РґР»СЏ РґРІСѓС… СЂР°Р·РЅС‹С… РїРѕС‚СЂРµР±РёС‚РµР»РµР№ С„РѕСЂРјРёСЂСѓСЋС‚СЃСЏ РѕС‚РґРµР»СЊРЅС‹Рµ СЃРёРіРЅР°Р»С‹ MOSI, SCK, CS */ \
			SPI0_TARGET_CS_PORT |= (SPI0_CS_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
			SPI0_TARGET_CS_DDR |= (SPI0_CS_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
			\
			SPI1_TARGET_CS_PORT |= (SPI1_CS_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
			SPI1_TARGET_CS_DDR |= (SPI1_CS_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
			\
			SPI2_TARGET_CS_PORT |= (SPI2_CS_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
			SPI2_TARGET_CS_DDR |= (SPI2_CS_BIT);	/* РїРѕСЂС‚ РїСЂРѕРіСЂР°РјРјРёСЂСѓРµС‚СЃСЏ РЅР° РІС‹РІРѕРґ РїРѕ РІСЃРµРј РІС‹РІРѕРґР°Рј */ \
		} while (0)





#if KEYBOARD_USE_ADC == 0
	#define KBD_TARGET_PORT PORTC
	#define KBD_TARGET_DDR DDRC
	#define KBD_TARGET_PIN PINC

	#define KBD_MASK 0xcf	// РІСЃРµ РёСЃРїРѕР»СЊР·СѓРµРјС‹Рµ Р±РёС‚С‹
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

#endif /* ATMEGA328_CPU_4Z5KY_V1_H_INCLUDED */
