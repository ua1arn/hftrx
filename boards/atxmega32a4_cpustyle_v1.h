/* $Id$ */
/* board-specific CPU TQFP44 attached signals */
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//
//
// ATxmega128A4U, ATxmega32A4

#ifndef ATXMEGA32A4_CPUSTYLE_V1_H_INCLUDED
#define ATXMEGA32A4_CPUSTYLE_V1_H_INCLUDED 1

#define WITHSPIHW 		1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ Р°РїРїР°СЂР°С‚РЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° SPI */
//#define WITHSPIHWDMA 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ DMA РїСЂРё РѕР±РјРµРЅРµ РїРѕ SPI */
//#define WITHSPISW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РїСЂРѕРіСЂР°РјРјРЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° SPI */
////#define WITHTWIHW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ Р°РїРїР°СЂР°С‚РЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° TWI (I2C) */
//#define WITHTWISW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РїСЂРѕРіСЂР°РјРјРЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° TWI (I2C) */
//#define WITHCPUADCHW 	1	/* РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ ADC */

#define WITHUART1HW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёР№РЅС‹Р№ РєРѕРЅС‚СЂРѕР»Р»РµСЂ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕРіРѕ РїРѕСЂС‚Р° #1 */
//#define WITHUART2HW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёР№РЅС‹Р№ РєРѕРЅС‚СЂРѕР»Р»РµСЂ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕРіРѕ РїРѕСЂС‚Р° #2 */

//#define WITHUSBHW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІСЃС‚СЂРѕРµРЅРЅР°СЏ РІ РїСЂРѕС†РµСЃСЃРѕСЂ РїРѕРґРґРµСЂР¶РєР° USB */
//#define WITHUSBHWVBUSSENSE	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїСЂРµРґРѕРїСЂРµРґРµР»РµРЅРЅС‹Р№ РІС‹РІРѕРґ VBUS_SENSE */
//#define WITHUSBHWHIGHSPEED	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІСЃС‚СЂРѕРµРЅРЅР°СЏ РІ РїСЂРѕС†РµСЃСЃРѕСЂ РїРѕРґРґРµСЂР¶РєР° USB HS */
//#define WITHUSBHWHIGHSPEEDDESC	1	/* РўСЂРµР±СѓРµС‚СЃСЏ С„РѕСЂРјРёСЂРѕРІР°С‚СЊ РґРµСЃРєСЂРёРїС‚РѕСЂС‹ РєР°Рє РґР»СЏ HIGH SPEED */

//#define WITHCAT_CDC		1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РІРёСЂС‚СѓР°Р»СЊРЅС‹Р№ РІРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅС‹Р№ РїРѕСЂС‚ РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
#define WITHCAT_USART1		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART1		1

	//#define LS020_RESET_PORT_S(v)	do { PORTC.OUTSET = (v); } while (0)
	//#define LS020_RESET_PORT_C(v)	do { PORTC.OUTCLR = (v); } while (0)
	//#define LS020_RST	(PIN2_bm)			// 

	#define LS020_RS_PORT_S(v)	do { PORTC.OUTSET = (v); } while (0)
	#define LS020_RS_PORT_C(v)	do { PORTC.OUTCLR = (v); } while (0)
	#define LS020_RS	(PIN2_bm)			// PC2

	#define LS020_RS_DDR_S (PORTC.DIRSET)

	#define LS020_RS_INITIALIZE() do { \
			LS020_RS_PORT_S(LS020_RS); \
			LS020_RS_DDR_S = LS020_RS; \
		} while(0)

	#define LS020_RESET_INITIALIZE() do { \
			LS020_RESET_PORT_S(LS020_RST); \
			LS020_RESET_DDR_S = LS020_RST; \
		} while(0)

	// I2C interface pins
	#if WITHTWISW
	//	#error Why without WITHTWIHW?
	#endif
	#if ! WITHTWIHW
	//	#error Why without WITHTWIHW?
	#endif
	//#define WITHTWIHW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ Р°РїРїР°СЂР°С‚РЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° TWI (I2C) */

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* РїРѕР»СѓС‡РёС‚СЊ РґР°РЅРЅС‹Рµ СЃ С€РёРЅС‹ LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { /* РІС‹РґР°С‡Р° РґР°РЅРЅС‹С… (РЅРµ СЃРґРІРёРЅСѓСЊС‹С…) */ \
			const portholder_t t = (portholder_t) (v) << LCD_DATAS_BIT_LOW; \
			LCD_DATA_PORT_S(LCD_DATAS_BITS & t); \
			LCD_DATA_PORT_C(LCD_DATAS_BITS & ~ t); \
		} while (0)

	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
#if WITHENCODER

	#define ENCODER_INPUT_PORT (PORTD.IN)

	#define ENCODER_SHIFT PIN0_bp
	#define ENCODER_BITS (PIN1_bm | PIN0_bm)	// PD1, PD0

	#define ENCODER_INITIALIZE() do { \
			PORTD.PIN0CTRL = (PORTD.PIN0CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTD.PIN1CTRL = (PORTD.PIN1CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTD.DIRCLR = ENCODER_BITS;	/* С…РѕС‚СЏ РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёСЏ, PULL-UP С‚РѕР»СЊРєРѕ РїСЂРё РїСЂРѕРіСЂР°РјРјРёСЂРѕРІР°РЅРёРё РЅР° РІРІРѕРґ СЂР°Р±РѕС‚Р°РµС‚. */ \
		} while (0)

#endif

#if WITHTX
	/* РїРµСЂРµРєР»СЋС‡РµРЅРёРµ Р±РёС‚Р° СѓРїСЂР°РІР»РµРЅРёСЏ РЅРµСЃСѓС‰РµР№ РІС‹Р·С‹РІР°РµС‚СЃСЏ РёР· РїСЂРµСЂС‹РІР°РЅРёР№ -
	 РІСЃРµРј РѕСЃС‚Р°Р»СЊРЅС‹Рј, РјРѕРґРёС„РёС†РёСЂСѓСЋС‰РёРј Р±РёС‚С‹ РЅР° СЌС‚РѕРј РїРѕСЂС‚Сѓ, РІС‹РїРѕР»РЅСЏС‚СЊ РїРѕРґ Р·Р°РїСЂРµС‚РѕРј РїСЂРµСЂС‹РІР°РЅРёР№.
		*/

	#define TXPATH_TARGET_PORT_S(v)		do { PORTD.OUTSET = (v); } while (0)
	#define TXPATH_TARGET_PORT_C(v)		do { PORTD.OUTCLR = (v); } while (0)

	#if 1
		// РЈРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - СЃРёРіРЅР°Р»С‹ TXPATH_ENABLE (PA11) Рё TXPATH_ENABLE_CW (PA10) - Р°РєС‚РёРІРЅС‹ РїСЂРё РЅСѓР»Рµ РЅР° РІС‹С…РѕРґРµ.
		#define TXPATH_BIT_ENABLE_SSB		(PIN2_bm)	// PD2 - pin 22
		#define TXPATH_BIT_ENABLE_CW	(PIN3_bm)		// PD3 - pin 23
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
		#define TXPATH_BIT_GATE (PIN2_bm)	// PD2 РІС‹С…РѕРґРЅРѕР№ СЃРёРіРЅР°Р» РёР· РїСЂРѕС†РµСЃСЃРѕСЂР° - СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј.
		//#define TXPATH_BIT_GATE_RX TXPATH_BIT_GATE	// СЃРёРіРЅР°Р» tx2 - СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј. РџСЂРё РїСЂРёС‘РјРµ Р°РєС‚РёРІРµРЅ
		#define TXPATH_BIT_GATE_RX 0	// СЃРёРіРЅР°Р» tx2 - СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј. РџСЂРё РїСЂРёС‘РјРµ РЅРµ Р°РєС‚РёРІРµРЅ

		// РџРѕРґРіРѕС‚РѕРІР»РµРЅРЅС‹Рµ СѓРїСЂР°РІР»СЏСЋС‰РёРµ СЃР»РѕРІР°
		#define TXGFV_RX		TXPATH_BIT_GATE_RX // TXPATH_BIT_GATE РґР»СЏ СЃС‚СЂР°РєС‚РѕРІ СЃ РѕР±С‰РёРјРё РєР°СЃРєР°РґР°РјРё РЅР° РїСЂРёС‘Рј Рё РїРµСЂРµРґР°С‡Сѓ
		#define TXGFV_TRANS		0			// РїРµСЂРµС…РѕРґ РјРµР¶РґСѓ СЂРµР¶РёРјР°РјРё РїСЂРёС‘РјР° Рё РїРµСЂРµРґР°С‡Рё
		#define TXGFV_TX_SSB	TXPATH_BIT_GATE
		#define TXGFV_TX_CW		TXPATH_BIT_GATE
		#define TXGFV_TX_AM		TXPATH_BIT_GATE
		#define TXGFV_TX_NFM	TXPATH_BIT_GATE
	#endif

	// Р‘РёС‚С‹ СѓРїСЂР°РІР»РµРЅРёСЏ С‚СЂР°РєС‚РѕРј РЅР° РїРµСЂРµРґР°С‡Сѓ
	#if defined (TXPATH_BITS_ENABLE)
		// РЅРµР°РєС‚РёРІРЅРѕРµ СЃРѕСЃС‚РѕСЏРЅРёРµ - "1" РІ СЂРµРіРёСЃС‚СЂРµ РґР°РЅРЅС‹С…..
		// Р’С‹С…РѕРґС‹ Р·Р°РїСЂРѕРіСЂР°РјРјРёСЂРѕРІР°РЅС‹ РЅР° РІС‹С…РѕРґ - "РѕС‚РєСЂС‹С‚С‹Р№ СЃС‚РѕРє"

		#define TXPATH_INITIALIZE() do { \
			PORTD.PIN2CTRL = (PORTD.PIN2CTRL & PORT_OPC_gm) | PORT_OPC_WIREDANDPULL_gc; \
			PORTD.PIN3CTRL = (PORTD.PIN3CTRL & PORT_OPC_gm) | PORT_OPC_WIREDANDPULL_gc; \
			PORTD.OUTSET = (TXPATH_BIT_ENABLE_SSB | TXPATH_BIT_ENABLE_CW); \
			PORTD.OUTSET = (TXPATH_BIT_ENABLE_SSB | TXPATH_BIT_ENABLE_CW);	/* Р±РёС‚ РІС‹С…РѕРґР° РјР°РЅРёРїСѓР»СЏС†РёРё - РѕС‚РєСЂС‹С‚С‹Р№ СЃС‚РѕРє */ \
		} while (0)

	#elif defined (TXPATH_BIT_GATE)

		#define TXPATH_INITIALIZE() do { \
			PORTD.PIN2CTRL = (PORTD.PIN2CTRL & PORT_OPC_gm) | PORT_OPC_TOTEM_gc; \
			PORTD.OUTCLR = TXPATH_BIT_GATE;	/* РЅРµР°РєС‚РёРІРЅРѕРµ СЃРѕСЃС‚РѕСЏРЅРёРµ - "0" */ \
			PORTD.DIRSET = TXPATH_BIT_GATE;	/* Р±РёС‚ РІС‹С…РѕРґР° РјР°РЅРёРїСѓР»СЏС†РёРё */ \
		} while (0)
	
	#elif TXPATH_BIT_GATE_RX
		#error TODO: complete TXPATH_BIT_GATE_RX support

		#define TXPATH_INITIALIZE() do { \
		} while (0)
	#endif

	// СЂР°Р·СЂРµС€РµРЅРёРµ РїСЂРµСЂС‹РІР°РЅРёР№ РїРѕ РЅР°Р¶Р°С‚РёСЏРј РЅР° РєРѕРЅС‚Р°РєС‚С‹ РєР»СЋС‡Р°
	// РњРёРєСЂРѕРїСЂРѕС†РµСЃСЃРѕСЂР°С… РЅР°РїРѕРґРѕР±РёРё ATMega644
	//#define PCMSK_PORT	PCMSK2			// PCMSK0 - PA, PCMSK1 - PB, PCMSK2 - PC, PCMSK3 - PD
	//#define PCICR_BIT	(1U << PCIE2)	// PCIE0 - PA, PCIE1 - PB, PCIE2 - PC, PCIE3 - PD
	//#define PCIVECT PCINT2_vect			// РІРµРєС‚РѕСЂ РѕР±СЂР°Р±РѕС‚С‡РёРєР°

	// РїСЂРѕС†РµСЃСЃРѕСЂ С„РѕСЂРјРёСЂСѓРµС‚ Р·РІСѓРє СЃР°РјРѕРєРѕРЅС‚СЂРѕР»СЏ РїСЂРё РїРµСЂРµРґР°С‡Рµ С‚РµР»РµРіСЂР°С„РѕРј
	// РіРµРЅРµСЂР°С†РёСЏ СЃРёРіРЅР°Р»Р° СЃР°РјРѕРєРѕРЅС‚СЂРѕР»СЏ РЅР° PD7(OC2)

	//#define PTT_TARGET_PORT (PORTD.OUT)	
	#define PTT_TARGET_PINCTRL (PORTD.PIN5CTRL)	// write PORT_OPC_PULLUP_gc
	#define PTT_TARGET_DDR_C (PORTD.DIRCLR)
	#define PTT_TARGET_PIN (PORTD.IN)
	#define PTT_BIT_PTT (PIN5_bm)				// PD7

	#define HARDWARE_GET_PTT() ((PTT_TARGET_PIN & PTT_BIT_PTT) == 0)
	#define PTT_INITIALIZE() \
		do { \
		/* С…РѕС‚СЏ РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёСЏ, PULL-UP С‚РѕР»СЊРєРѕ РїСЂРё РїСЂРѕРіСЂР°РјРјРёСЂРѕРІР°РЅРёРё РЅР° РІРІРѕРґ СЂР°Р±РѕС‚Р°РµС‚. */ \
		PTT_TARGET_PINCTRL = (PTT_TARGET_PINCTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc;	/* РІРєР»СЋС‡С‚СЊ pull-up РЅР° СЌС‚РѕС‚ РІС…РѕРґ. */ \
		PTT_TARGET_DDR_C = PTT_BIT_PTT;	/* Р±РёС‚ РІС…РѕРґР° PTT */ \
		} while (0)


#endif /* WITHTX */

#if WITHCAT || WITHNMEA
	// РЎРёРіРЅР°Р»С‹ РїРµСЂРµРєР»СЋС‡РµРЅРёСЏ РЅР° РїРµСЂРµРґР°С‡Сѓ Рё РјР°РЅРёРїСѓР»СЏС†РёРё С‡РµСЂРµР· CAT РёРЅС‚РµСЂС„РµР№СЃ.
	#define FROMCAT_TARGET_PIN_RTS		(PORTE.IN)
	#define FROMCAT_BIT_RTS			(PIN1_bm)	/* PE1 СЃРёРіРЅР°Р» RTS РѕС‚ FT232RL	*/
	/* РјР°РЅРёРїСѓР»СЏС†РёСЏ РѕС‚ РїРѕСЂС‚Р° RS-232, СЃРёРіРЅР°Р» PPS РѕС‚ GPS/GLONASS/GALILEO РјРѕРґСѓР»СЏ */
	#define FROMCAT_TARGET_PIN_DTR		(PORTE.IN)
	#define FROMCAT_BIT_DTR			(PIN0_bm)	/* PE0 СЃРёРіРЅР°Р» DTR РѕС‚ FT232RL	*/

	#define FROMCAT_RTS_INITIALIZE() do { \
			PORTE.PIN1CTRL = (PORTE.PIN1CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc;	/* РІРєР»СЋС‡С‚СЊ pull-up РЅР° СЌС‚РѕС‚ РІС…РѕРґ. */ \
			PORTE.DIRCLR = FROMCAT_BIT_RTS;	/* Р±РёС‚ РІС…РѕРґР° RTS */ \
		} while (0)

	#define FROMCAT_DTR_INITIALIZE() do { \
			PORTE.PIN0CTRL = (PORTE.PIN0CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc;	/* РІРєР»СЋС‡С‚СЊ pull-up РЅР° СЌС‚РѕС‚ РІС…РѕРґ. */ \
			PORTE.DIRCLR = FROMCAT_BIT_DTR;	/* Р±РёС‚ РІС…РѕРґР° DTR */ \
		} while (0)
	
#endif

#if WITHELKEY

	#define ELKEY_TARGET_PIN (PORTD.IN)		// РІРІРѕРґ - СЃРёРіРЅР°Р»С‹ СЌР»РµРєС‚СЂРѕРЅРЅРѕРіРѕ РєР»СЋС‡Р°
	#define ELKEY_BIT_RIGHT (PIN7_bm)		// PD7
	#define ELKEY_BIT_LEFT (PIN6_bm)		// PD6

	#define ELKEY_INITIALIZE() do { \
		PORTD.DIRCLR = ELKEY_BIT_LEFT;	/* Р±РёС‚ РІС…РѕРґР° Р»РµРІРѕРіРѕ РєРѕРЅС‚Р°РєС‚Р° РјР°РЅРёРїСѓР»СЏС‚РѕСЂР° */ \
		PORTD.DIRCLR = ELKEY_BIT_RIGHT;	/* Р±РёС‚ РІС…РѕРґР° РїСЂР°РІРѕРіРѕ РєРѕРЅС‚Р°РєС‚Р° РјР°РЅРёРїСѓР»СЏС‚РѕСЂР° */ \
		PORTD.PIN6CTRL = (PORTD.PIN6CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
		PORTD.PIN7CTRL = (PORTD.PIN7CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
		} while (0)
#endif

//#define SIDETONE_TARGET_PORT	(PORTD.OUT)	/* РџРѕСЂС‚ РїСЂРѕС†РµСЃСЃРѕСЂР°, РЅР° РєРѕС‚РѕСЂРѕРј РЅР°С…РѕРґРёС‚СЃСЏ РІС‹РІРѕРґ С‚РѕРЅР° СЃР°РјРѕРєРѕРЅС‚СЂРѕР»СЏ */
#define SIDETONE_TARGET_DDR		(PORTD.DIR)
#define SIDETONE_TARGET_BIT		(PIN4_bm)		// PD4

#if 1 // WITHTWISW
	#define TARGET_TWI_PORT_S(v)	do { PORTC.OUTSET = (v); } while (0)
	#define TARGET_TWI_PORT_C(v)	do { PORTC.OUTCLR = (v); } while (0)
	#define TARGET_TWI_DDR_S (PORTC.DIRSET)
	#define TARGET_TWI_DDR_C (PORTC.DIRCLR)
	#define TARGET_TWI_PIN (PORTC.IN)
	#define TARGET_TWI_TWCK	(PIN1_bm)			// SCL - PC1
	#define TARGET_TWI_TWD	(PIN0_bm)			// SDA - PC0

	// РРЅРёС†РёР°Р»РёР·Р°С†РёСЏ Р±РёС‚РѕРІ РїРѕСЂС‚РѕРІ РІРІРѕРґР°-РІС‹РІРѕРґР° РґР»СЏ РїСЂРѕРіСЂР°РјРјРЅРѕР№ СЂРµР°Р»РёР·Р°С†РёРё I2C
	#define	TWISOFT_INITIALIZE() \
		do { \
			/* РїСЂРѕРіСЂР°РјРјРёСЂРѕРІР°РЅРёРµ РІС‹РІРѕРґРѕРІ, СѓРїСЂР°РІР»СЏСЋС‰РёС… I2C */ \
			TARGET_TWI_PORT_S(TARGET_TWI_TWCK | TARGET_TWI_TWD); \
			TARGET_TWI_DDR_S = (TARGET_TWI_TWCK | TARGET_TWI_TWD);	/* РќР° РІС‹Р·РѕРґРµ - "1" */ \
			PORTC.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc;	/* Wired AND w/ Pull-up */ \
			PORTC.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;	/* Wired AND w/ Pull-up */ \
		} while (0) 

	// РџСЂРѕРіСЂР°РјРјРёСЂРѕРІР°РЅРёРµ РІС‹РІРѕРґРѕРІ РґР»СЏ Р°РїРїР°СЂР°С‚РЅРѕР№ СЂРµР°Р»РёР·Р°С†РёРё I2C РЅРµ С‚СЂРµР±СѓРµС‚СЃСЏ.
	#define	TWIHARD_INITIALIZE() do { \
		} while (0) 

	#define TARGET_TWI	TWIC	/* TWI РїРѕСЂС‚Р° C */

#endif

#if LCDMODE_UC1608
	#define UC1608_CSP_PORT_S(v)	do { PORTC.OUTSET = (c); } while (0)
	#define UC1608_CSP_PORT_C(v)	do { PORTC.OUTCLR = (c); } while (0)
	//#define UC1608_CSP_DDR_C (PORTC.DIRCLR)
	#define UC1608_CSP_DDR_S (PORTC.DIRSET)
	#define UC1608_CSP 	(PIN3_bm)			// PC3 - DFU PIN - С‚Р°Рє Р¶Рµ РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РєР°Рє CSP РґР»СЏ uc1608
	#define SPI_CSEL255	255					// РїРѕ СЌС‚РѕРјСѓ С‡РёРїСЃРµР»РµРєС‚Сѓ РІС‹Р±РёСЂР°РµРј РїРѕР»РѕР¶РёС‚РµР»СЊРЅС‹Рј СЃРёРіРЅР°Р»РѕРј

	#define UC1608_CSP_INITIALIZE() do { \
			UC1608_CSP_PORT_C(UC1608_CSP);	/* РЅРµР°РєС‚РёРІРЅРѕРµ СЃРѕСЃС‚РѕСЏРЅРёРµ */ \
			UC1608_CSP_DDR_S = UC1608_CSP;	/* Р’С‹С…РѕРґ */ \
		} while (0)
#endif

	#define TARGET_SPI	PORTC

	#define SPI_TARGET_SCLK_PORT (TARGET_SPI.OUT)
	#define SPI_TARGET_SCLK_PORT_S(v)	do { TARGET_SPI.OUTSET = (v); } while (0)
	#define SPI_TARGET_SCLK_PORT_C(v)	do { TARGET_SPI.OUTCLR = (v); } while (0)
	#define SPI_TARGET_SCLK_PORT_T (TARGET_SPI.OUTTGL)	/* РїСЂРё Р·Р°РїРёСЃРё РІ СЌС‚РѕС‚ РїРѕСЂС‚ РїСЂРѕРёСЃС…РѕРґРёС‚ РёРЅРІРµСЂСЃРёСЏ СЃРѕСЃС‚РѕСЏРЅРёСЏ РІС‹С…РѕРґР° */
	
	#define SPI_TARGET_SCLK_DDR (TARGET_SPI.DIR)
	#define SPI_TARGET_SCLK_DDR_S (TARGET_SPI.DIRSET)
	#define SPI_TARGET_SCLK_DDR_C (TARGET_SPI.DIRCLR)
	//#define SPI_TARGET_SCLK_PIN (TARGET_SPI.IN)

	#define SPI_TARGET_MOSI_PORT (TARGET_SPI.OUT)
	#define SPI_TARGET_MOSI_PORT_S(v)	do { TARGET_SPI.OUTSET = (v); } while (0)
	#define SPI_TARGET_MOSI_PORT_C(v)	do { TARGET_SPI.OUTCLR = (v); } while (0)
	#define SPI_TARGET_MOSI_PORT_T (TARGET_SPI.OUTTGL)	/* РїСЂРё Р·Р°РїРёСЃРё РІ СЌС‚РѕС‚ РїРѕСЂС‚ РїСЂРѕРёСЃС…РѕРґРёС‚ РёРЅРІРµСЂСЃРёСЏ СЃРѕСЃС‚РѕСЏРЅРёСЏ РІС‹С…РѕРґР° */
	
	//#define SPI_TARGET_MOSI_DDR (TARGET_SPI.DIR)
	#define SPI_TARGET_MOSI_DDR_S (TARGET_SPI.DIRSET)
	//#define SPI_TARGET_MOSI_DDR_C (TARGET_SPI.DIRCLR)
	//#define SPI_TARGET_MOSI_PIN (TARGET_SPI.IN)

	#define	SPI_SCLK_BIT (PIN7_bm)	// PC7 Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ СЃРёРЅС…СЂРѕРЅРёР·Р°С†РёСЏ SPI
	#define	SPI_MOSI_BIT (PIN5_bm)	// PC5 Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІС‹РІРѕРґ (РёР»Рё РІРІРѕРґ РІ СЃР»СѓС‡Р°Рµ РґРІСѓРЅР°РїСЂР°РІР»РµРЅРЅРѕРіРѕ SPI).

	#define SPI_TARGET_MISO_PIN (TARGET_SPI.IN)
	//#define SPI_TARGET_MISO_PORT (TARGET_SPI.OUT)
	#define SPI_TARGET_MISO_PINCTRL (TARGET_SPI.PIN6CTRL)	// write PORT_OPC_PULLUP_gc
	//#define SPI_TARGET_MISO_DDR (TARGET_SPI.DIR)
	#define SPI_TARGET_MISO_DDR_C (TARGET_SPI.DIRCLR)

#if SPI_BIDIRECTIONAL

	#define SPIIO_MOSI_TO_INPUT() do { \
		SPI_TARGET_MOSI_DDR_C = SPI_MOSI_BIT;	/* РїРµСЂРµРєР»СЋС‡РёС‚СЊ РїРѕСЂС‚ РЅР° С‡С‚РµРЅРёРµ СЃ РІС‹РІРѕРґРѕРІ */ \
		} while (0)
	#define SPIIO_MOSI_TO_OUTPUT() do { \
		SPI_TARGET_MOSI_DDR_S = SPI_MOSI_BIT;	/* РѕС‚РєСЂС‹С‚СЊ РІС‹С…РѕРґС‹ РїРѕСЂС‚Р° */ \
		} while (0)

	#define SPIIO_INITIALIZE() do { \
			/* PB4(~SS) РґРѕР»Р¶РµРЅ Р±С‹С‚СЊ РІС‹С…РѕРґРѕРј. SPI_NAEN_BIT - СЂР°Р·СЂРµС€РµРЅРёРµ РґРµС€РёС„СЂР°С‚РѕСЂР°. */ \
			SPI_TARGET_SCLK_PORT_S(SPI_SCLK_BIT); \
			SPI_TARGET_SCLK_DDR_S = SPI_SCLK_BIT; \
			SPI_TARGET_MOSI_PORT_S(SPI_MOSI_BIT); \
			SPI_TARGET_MOSI_DDR_S = SPI_MOSI_BIT; \
			TARGETSOFT_SPI.DIRSET = SPI_NAEN_BIT;		/* Р­С‚Рѕ РІС‹РІРѕРґ CS -  РІСЃРµРіРґР° РґРѕР»Р¶РµРЅ Р±С‹С‚СЊ РІС‹С…РѕРґРѕРј РїСЂРё РёСЃРїРѕР»СЊР·РѕРІР°РЅРіРёРё Р°РїРїР°СЂР°С‚РЅРѕРіРѕ SPI */ \
		} while (0)

#else /* SPI_BIDIRECTIONAL */

	#define	SPI_MISO_BIT (PIN6_bm)	// PC6 Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІРІРѕРґ СЃ SPI.

	#define SPIIO_INITIALIZE() do { \
			SPI_TARGET_MISO_DDR_C  = SPI_MISO_BIT; \
			/* PB4(~SS) РґРѕР»Р¶РµРЅ Р±С‹С‚СЊ РІС‹С…РѕРґРѕРј. SPI_NAEN_BIT - СЂР°Р·СЂРµС€РµРЅРёРµ РґРµС€РёС„СЂР°С‚РѕСЂР°. */ \
			SPI_TARGET_SCLK_PORT_S(SPI_SCLK_BIT); \
			SPI_TARGET_SCLK_DDR_S = SPI_SCLK_BIT; \
			SPI_TARGET_MOSI_PORT_S(SPI_MOSI_BIT); \
			SPI_TARGET_MOSI_DDR_S = SPI_MOSI_BIT; \
			TARGETSOFT_SPI.DIRSET = SPI_NAEN_BIT;		/* Р­С‚Рѕ РІС‹РІРѕРґ CS -  РІСЃРµРіРґР° РґРѕР»Р¶РµРЅ Р±С‹С‚СЊ РІС‹С…РѕРґРѕРј РїСЂРё РёСЃРїРѕР»СЊР·РѕРІР°РЅРіРёРё Р°РїРїР°СЂР°С‚РЅРѕРіРѕ SPI */ \
		} while (0)

#endif /* SPI_BIDIRECTIONAL */



	// РІ СЌС‚РѕР№ РІРµСЂСЃРёРё РїР»Р°С‚С‹ РЅРµС‚ СѓРїСЂР°РІР»РµРЅРёСЏ IORESET, IOUPDATE РёРґС‘С‚ СЃ РІС‹С…РѕРґР° РґРµС€РёС„СЂР°С‚РѕСЂР° CS РёРЅС‚РµСЂС„РµР№СЃР° SPI
	//#define SPI_IOUPDATE_BIT (1U << PB3)	// РёСЃРїРѕР»СЊР·СѓСЋС‚СЃСЏ РїРѕСЂС‚С‹ SPI

	// Р•СЃС‚СЊ РІРЅРµС€РЅРёР№ РґРµС€РёС„СЂР°С‚РѕСЂ РЅР° С€РёРЅРµ Р°РґСЂРµСЃР° SPI 
	#define SPI_ADDRESS_PORT (PORTB.OUT)
	#define SPI_ADDRESS_PORT_S(v)	do { PORTB.OUTSET = (v); } while (0)
	#define SPI_ADDRESS_PORT_C(v)	do { PORTB.OUTCLR = (v); } while (0)
	//#define SPI_ADDRESS_DDR_S (PORTB.DIRSET)
	#define SPI_ADDRESS_DDR (PORTB.DIR)
	//#define SPI_ADDRESS_PIN (PORTB.IN)

	#define SPI_A0 (PIN1_bm)			// PB1 Р±РёС‚С‹ Р°РґСЂРµСЃР° РґР»СЏ РґРµС€РёС„СЂР°С‚РѕСЂР° SPI
	#define SPI_A1 (PIN2_bm)			// PB2
	#define SPI_A2 (PIN3_bm)			// PB3

	#define TARGETHARD_SPI	SPIC	/* SPI РїРѕСЂС‚Р° C */
	#define TARGETSOFT_SPI	PORTC	/* SPI РїРѕСЂС‚Р° C */

	#define SPI_NAEN_PORT (TARGETSOFT_SPI.OUT)
	#define SPI_NAEN_PORT_S(v)	do { TARGETSOFT_SPI.OUTSET = (v); } while (0)
	#define SPI_NAEN_PORT_C(v)	do { TARGETSOFT_SPI.OUTCLR = (v); } while (0)
	#define SPI_NAEN_DDR (TARGETSOFT_SPI.DIR)
	#define SPI_NAEN_DDR_S (TARGETSOFT_SPI.DIRSET)
	#define SPI_NAEN_DDR_C (TARGETSOFT_SPI.DIRCLR)

	#define SPI_NAEN_BIT	PIN4_bm	// PC4 "0" - СЂР°Р·СЂРµС€РµРЅРёРµ РІС‹С…РѕРґР° CS
	#define SPI_ALLCS_BITS	0		// С‚СЂРµР±СѓРµС‚СЃСЏ РґР»СЏ СѓРєР°Р·Р°РЅРёСЏ С‚РѕРіРѕ, С‡С‚Рѕ СЂР°Р±РѕС‚Р° СЃ РїСЂСЏРјС‹Рј РІС‹Р±РѕСЂРѕРј CS (Р±РµР· РґРµС€РёС„СЂР°С‚РѕСЂР°) РЅРµ С‚СЂРµР±СѓРµС‚СЃСЏ
	#define SPI_ALLCS_BITSNEG 0		// Р’С‹С…РѕРґС‹, Р°РєС‚РёРІРЅС‹Рµ РїСЂРё "1"

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
			TARGETHARD_SPI.CTRL = 0x00; \
		} while (0)


	#if KEYBOARD_USE_ADC
		#define HARDWARE_KBD_INITIALIZE() do { \
			} while (0)
	#else
		//#define KBD_TARGET_PORT PORTA.OUT
		#define KBD_TARGET_PIN PORTA.IN

		#define KBD_MASK (PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm)	// РІСЃРµ РёСЃРїРѕР»СЊР·СѓРµРјС‹Рµ Р±РёС‚С‹

		#define HARDWARE_KBD_INITIALIZE() do { \
			PORTA.PIN1CTRL = (PORTA.PIN1CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.PIN2CTRL = (PORTA.PIN2CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.PIN3CTRL = (PORTA.PIN3CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.PIN4CTRL = (PORTA.PIN4CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.PIN5CTRL = (PORTA.PIN5CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.PIN6CTRL = (PORTA.PIN6CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.PIN7CTRL = (PORTA.PIN7CTRL & ~ PORT_OPC_gm) | PORT_OPC_PULLUP_gc; \
			PORTA.DIR &= ~ KBD_MASK;	/* define these bits as inputs - enable pull-up */ \
			} while (0)
	#endif

	#define CS4272_RESET_INITIALIZE() do { \
			TARGET_CS4272_RESET_PORT_S(TARGET_CS4272_RESET_BIT); \
			TARGET_CS4272_RESET_DDR_S(TARGET_CS4272_RESET_BIT); \
		} while (0)

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

#endif /* ATXMEGA32A4_CPUSTYLE_V1_H_INCLUDED */
