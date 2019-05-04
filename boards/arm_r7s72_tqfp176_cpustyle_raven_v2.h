/* $Id$ */
/* board-specific CPU attached signals */
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//

// РўСЂР°РЅСЃРёРІРµСЂ СЃ DSP РѕР±СЂР°Р±РѕС‚РєРѕР№ "Storch" РЅР° РїСЂРѕС†РµСЃСЃРѕСЂРµ Renesas R7S721020VCFP СЃ РєРѕРґРµРєРѕРј NAU8822L Рё FPGA EP4CE22E22I7N
// Rmainunit_v7cm.pcb

#ifndef ARM_R7S72_TQFP176_CPUSTYLE_RAVEN_V2_H_INCLUDED
#define ARM_R7S72_TQFP176_CPUSTYLE_RAVEN_V2_H_INCLUDED 1

//#define HARDWARE_ARM_USEUSART0 1		// US0:
//#define HARDWARE_ARM_USEUSART1 1		// US1: PA9/PA10 pins

#define WITHSPI16BIT	1		/* РІРѕР·РјРѕР¶РЅРѕ РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ 16-С‚Рё Р±РёС‚РЅС‹С… СЃР»РѕРІ РїСЂРё РѕР±РјРµРЅРµ РїРѕ SPI */
#define WITHSPIHW 		1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ Р°РїРїР°СЂР°С‚РЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° SPI */
#define WITHSPIHWDMA 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ DMA РїСЂРё РѕР±РјРµРЅРµ РїРѕ SPI */
//#define WITHSPISW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РїСЂРѕРіСЂР°РјРјРЅРѕРіРѕ СѓРїСЂР°РІР»РµРЅРёСЏ SPI. */

//#define WITHTWIHW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ Р°РїРїР°СЂР°С‚РЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° TWI (I2C) */
#define WITHTWISW 	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РїСЂРѕРіСЂР°РјРјРЅРѕРіРѕ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° TWI (I2C) */

#define WITHI2SHW	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ SSIF0 I2S 2*16 bit - Р°СѓРґРёРѕ РєРѕРґРµРє */
#define WITHSAI1HW	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ SSIF1 I2S 8*32 bit - FPGA IF codec */
#if WITHRTS192
	#define WITHSAI2HW	1	/* РСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ SSIF2 I2S 2*32 (2*32) bit - FPGA РїР°РЅРѕСЂР°РјР°	*/
#endif /* WITHRTS192 */

//#define WITHCPUDACHW	1	/* РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ DAC - РІ renesas R7S72 РЅРµС‚Сѓ */
#define WITHCPUADCHW 	1	/* РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ ADC */

#define WITHSDHCHW	1		/* Hardware SD HOST CONTROLLER */
#define WITHSDHCHW4BIT	1	/* Hardware SD HOST CONTROLLER РІ 4-bit bus width */

#define WITHUSBHW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІСЃС‚СЂРѕРµРЅРЅР°СЏ РІ РїСЂРѕС†РµСЃСЃРѕСЂ РїРѕРґРґРµСЂР¶РєР° USB */
#define WITHUSBHWVBUSSENSE	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїСЂРµРґРѕРїСЂРµРґРµР»РµРЅРЅС‹Р№ РІС‹РІРѕРґ VBUS_SENSE */
#define WITHUSBHWHIGHSPEED	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІСЃС‚СЂРѕРµРЅРЅР°СЏ РІ РїСЂРѕС†РµСЃСЃРѕСЂ РїРѕРґРґРµСЂР¶РєР° USB HS */
#define WITHUSBHWHIGHSPEEDDESC	1	/* РўСЂРµР±СѓРµС‚СЃСЏ С„РѕСЂРјРёСЂРѕРІР°С‚СЊ РґРµСЃРєСЂРёРїС‚РѕСЂС‹ РєР°Рє РґР»СЏ HIGH SPEED */
#define WITHUSBHW_DEVICE	(& USB200)	/* РЅР° СЌС‚РѕРј СѓСЃС‚СЂРѕР№СЃС‚РІРµ РїРѕРґРґРµСЂР¶РёРІР°РµС‚СЃСЏ С„СѓРЅРєС†РёР»РЅР°Р»СЊРЅРѕСЃС‚СЊ DEVUCE	*/

#define WITHUART1HW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёР№РЅС‹Р№ РєРѕРЅС‚СЂРѕР»Р»РµСЂ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕРіРѕ РїРѕСЂС‚Р° #1 SCIF0 */
//#define WITHUART2HW	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїРµСЂРёС„РµСЂРёР№РЅС‹Р№ РєРѕРЅС‚СЂРѕР»Р»РµСЂ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅРѕРіРѕ РїРѕСЂС‚Р° #2 SCIF3 */

//#define WITHCAT_USART1	1
#define WITHCAT_CDC			1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РІРёСЂС‚СѓР°Р»СЊРЅС‹Р№ РІРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅС‹Р№ РїРѕСЂС‚ РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
//#define WITHMODEM_CDC		1
#define WITHDEBUG_USART1	1
#define WITHMODEM_USART1	1
#define WITHNMEA_USART1		1	/* РїРѕСЂС‚ РїРѕРґРєР»СЋС‡РµРЅРёСЏ GPS/GLONASS */

#define WITHUSBUAC		1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РІРёСЂС‚СѓР°Р»СЊРЅСѓСЋ Р·РІСѓРєРѕРІСѓСЋ РїР»Р°С‚Сѓ РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
#define WITHUSBCDC		1	/* ACM РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РІРёСЂС‚СѓР°Р»СЊРЅС‹Р№ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅС‹Р№ РїРѕСЂС‚ РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
//#define WITHUSBCDCEEM	1	/* EEM РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ Ethernet Emulation Model РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
//#define WITHUSBCDCECM	1	/* ECM РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ Ethernet Control Model РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */
//#define WITHUSBHID	1	/* HID РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ Human Interface Device РЅР° USB СЃРѕРµРґРёРЅРµРЅРёРё */

#define WRITEE_BIT				(1u << 12)	// RD/~WR  P3_12 - РґРѕР»Р¶РµРЅ Р±С‹С‚СЊ РІ "0" - РєР°Рє РїСЂРё Р·Р°РїРёСЃРё - РґР»СЏ СѓРїСЂР°РІР»РµРЅРёСЏ Р±СѓС„РµСЂР°РјРё РЅР° С€РёРЅРµ РґР°РЅРЅС‹С… LCD

/* Р’ РґР°РЅРЅРѕР№ РІРµСЂСЃРёРё РїР»Р°С‚С‹ РѕС€РёР±РєР° РІ СЃС…РµРјРµ - РґР»СЏ СЂР°Р±РѕС‚С‹ РїСЂРµРѕР±СЂР°Р·РѕРІР°С‚РµР»РµР№ СѓСЂРѕРІРЅСЏ "РЅР°СЂСѓР¶Сѓ" WRITEE_BIT РґРѕР»Р¶Рµ Р±С‹С‚СЊ РІ "1". 
*/
#define LS020_RS_INITIALIZE() \
	do { \
		arm_hardware_pio3_outputs(WRITEE_BIT, 0); /* РІ РЅРѕРІРѕР№ РІРµСЂСЃРёРё РїР»Р°С‚С‹ РІС‹РґР°РІР°С‚СЊ "0" */ \
		arm_hardware_pio3_outputs(LS020_RS, LS020_RS); \
	} while (0)

#define LS020_RESET_INITIALIZE() \
	do { \
		arm_hardware_pio3_outputs(WRITEE_BIT, 0); /* РІ РЅРѕРІРѕР№ РІРµСЂСЃРёРё РїР»Р°С‚С‹ РІС‹РґР°РІР°С‚СЊ "0" */ \
		arm_hardware_pio3_outputs(LS020_RST, LS020_RST); \
	} while (0)

#if LCDMODE_SPI_NA
	// СЌС‚Рё РєРѕРЅС‚СЂРѕР»Р»РµСЂС‹ С‚СЂРµР±СѓСЋС‚ С‚РѕР»СЊРєРѕ RS

	#define LS020_RS_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LS020_RS_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)
	#define LS020_RS			(1u << 11)			// P3_11 D7 signal in HD44780 socket

#elif LCDMODE_SPI_RN
	// СЌС‚Рё РєРѕРЅС‚СЂРѕР»Р»РµСЂС‹ С‚СЂРµР±СѓСЋС‚ С‚РѕР»СЊРєРѕ RESET

	#define LS020_RESET_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LS020_RESET_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)
	#define LS020_RST			(1u << 10)			// * P3_10 D6 signal in HD44780 socket

#elif LCDMODE_SPI_RA
	// Р­С‚Рё РєРѕРЅС‚СЂРѕР»Р»РµСЂС‹ С‚СЂРµР±СѓСЋС‚ RESET Рё RS

	#define LS020_RS_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LS020_RS_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)
	#define LS020_RS			(1u << 11)			// P3_11 D7 signal in HD44780 socket

	#define LS020_RESET_PORT_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define LS020_RESET_PORT_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)
	#define LS020_RST			(1u << 10)			// P3_10 D6 signal in HD44780 socket

#elif LCDMODE_HD44780 && (LCDMODE_SPI == 0)

	// Р’С‹РІРѕРґС‹ РїРѕРґРєР»СЋС‡РµРЅРёСЏ Р–РљР РёРЅРґРёРєР°С‚РѕСЂР° WH2002 РёР»Рё Р°РЅР°Р»РѕРіРёС‡РЅРѕРіРѕ HD44780.
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
	#define LCD_DATAS_BIT_LOW		8		// РєР°РєРѕР№ Р±РёС‚ РґР°РЅРЅС‹С… РјР»Р°РґС€РёР№ РІ СЃР»РѕРІРµ СЃС‡РёС‚Р°РЅРЅРѕРј СЃ РїРѕСЂС‚Р°

	#define DISPLAY_BUS_DATA_GET() ((LCD_DATA_INPUT & LCD_DATAS_BITS) >> LCD_DATAS_BIT_LOW) /* РїРѕР»СѓС‡РёС‚СЊ РґР°РЅРЅС‹Рµ СЃ С€РёРЅС‹ LCD */
	#define DISPLAY_BUS_DATA_SET(v) do { /* РІС‹РґР°С‡Р° РґР°РЅРЅС‹С… (РЅРµ СЃРґРІРёРЅСѓСЊС‹С…) */ \
			const portholder_t t = (portholder_t) (v) << LCD_DATAS_BIT_LOW; \
			LCD_DATA_PORT_S(LCD_DATAS_BITS & t); \
			LCD_DATA_PORT_C(LCD_DATAS_BITS & ~ t); \
		} while (0)

	/* РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ СѓРїСЂР°РІР»СЏСЋС‰РёС… РІС‹С…РѕРґРѕРІ РїСЂРѕС†РµСЃСЃРѕСЂР° РґР»СЏ СѓРїСЂР°РІР»РµРЅРёСЏ HD44780 - РїРѕР»РЅС‹Р№ РЅР°Р±РѕСЂ РІС‹С…РѕРґРѕРІ */
	#define LCD_CONTROL_INITIALIZE() \
		do { \
			arm_hardware_pio3_outputs(LCD_STROBE_BIT | WRITEE_BIT | ADDRES_BIT, 0); \
		} while (0)
	/* РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ СѓРїСЂР°РІР»СЏСЋС‰РёС… РІС‹С…РѕРґРѕРІ РїСЂРѕС†РµСЃСЃРѕСЂР° РґР»СЏ СѓРїСЂР°РІР»РµРЅРёСЏ HD44780 - WE=0 */
	#define LCD_CONTROL_INITIALIZE_WEEZERO() \
		do { \
			arm_hardware_pio3_outputs(LCD_STROBE_BIT | WRITEE_BIT_ZERO | ADDRES_BIT, 0); \
		} while (0)
	/* РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ СѓРїСЂР°РІР»СЏСЋС‰РёС… РІС‹С…РѕРґРѕРІ РїСЂРѕС†РµСЃСЃРѕСЂР° РґР»СЏ СѓРїСЂР°РІР»РµРЅРёСЏ HD44780 - WE РѕС‚СЃСѓС‚СЃС‚РІСѓРµС‚ - СЃРёРіРЅР°Р» Рє РёРЅРґРёРєР°С‚РѕСЂСѓ Р·Р°Р·РµРјР»С‘РЅ */
	#define LCD_CONTROL_INITIALIZE_WEENONE() \
		do { \
			arm_hardware_pio3_outputs(LCD_STROBE_BIT | ADDRES_BIT, 0); \
		} while (0)

	#define LCD_DATA_INITIALIZE_READ() \
		do { \
			arm_hardware_pio3_inputs(LCD_DATAS_BITS);	/* РїРµСЂРµРєР»СЋС‡РёС‚СЊ РїРѕСЂС‚ РЅР° С‡С‚РµРЅРёРµ СЃ РІС‹РІРѕРґРѕРІ */ \
		} while (0)

	#define LCD_DATA_INITIALIZE_WRITE(v) \
		do { \
			arm_hardware_pio3_outputs(LCD_DATAS_BITS, (v) << LCD_DATAS_BIT_LOW);	/* РѕС‚РєСЂС‹С‚СЊ РІС‹С…РѕРґС‹ РїРѕСЂС‚Р° */ \
		} while (0)

#endif

#if 0 && LCDMODE_UC1608
	#define UC1608_CSP_PORT_S(v) do {	R7S721_TARGET_PORT_S(7, v); } while (0)
	#define UC1608_CSP_PORT_C(v) do {	R7S721_TARGET_PORT_C(7, v); } while (0)
	#define UC1608_CSP			(1u << 15)			// * PE15
	#define SPI_CSEL255			255				// РїРѕ СЌС‚РѕРјСѓ С‡РёРїСЃРµР»РµРєС‚Сѓ РІС‹Р±РёСЂР°РµРј РїРѕР»РѕР¶РёС‚РµР»СЊРЅС‹Рј СЃРёРіРЅР°Р»РѕРј

	#define UC1608_CSP_INITIALIZE() do { \
			arm_hardware_pio1_outputs(UC1608_CSP, 0); \
		} while (0)
#endif


#if WITHENCODER
	// Р’С‹РІРѕРґС‹ РїРѕРґРєР»СЋС‡РµРЅРёСЏ РµРЅРєРѕРґРµСЂР°
	// Encoder #1 inputs: P5_9 - PHASE A, P5_8 = PHASE B
	// Encoder #2 inputs: P5_11 - PHASE A, P5_10 = PHASE B

	// Р”РѕРїСѓСЃС‚РёРјС‹:
	// IRQ0:			P6_7,	P1_8, P1_4, 
	// IRQ1:			P6_8,	P1_9, P1_5,									P2_9
	// IRQ2:			P6_9,	P1_10, P1_6,								P2_7
	// IRQ3:	P5_8,	P6_10,	P1_11, P1_7,	P7_3, P7_8
	// IRQ4:	P5_9,			P1_12, P1_0,	P7_2,				P0_0
	// IRQ5:	P5_10,			P1_13. P1_1,	P7_7, P7_9,					P2_2
	// IRQ6:	P5_11,			P1_14, P1_2,	P7_6, 		P8_6,			P2_1
	// IRQ7:					P1_15, P1_3,				P8_7,	P0_2,	P2_0,	

	// РћР±СЏР·Р°С‚РµР»СЊРЅРѕ Р±СѓС„РµСЂ (РІС…РѕРґС‹ РїСЂРѕС†РµСЃСЃРѕСЂР° РЅРёР·РєРѕРІРѕР»СЊС‚РЅС‹Рµ).

	#define ENCODER_INPUT_PORT			(R7S721_INPUT_PORT(5)) 
	#define ENCODER_BITA (1u << 11)	// P5_11 IRQ6
	#define ENCODER_BITB (1u << 10)	// P5_10 IRQ5
	#define ENCODER_BITS (ENCODER_BITA | ENCODER_BITB)
	#define ENCODER_SHIFT 10	// РћС‚СЃСѓС‚СЃС‚РІРёРµ СЌС‚РѕРіРѕ Р·РЅР°С‡РµРЅРёСЏ РѕР·РЅР°С‡Р°РµС‚ С‡С‚Рѕ Р±РёС‚С‹ РЅРµ РїРѕРґСЂСЏРґ

	#define ENCODER2_INPUT_PORT			(R7S721_INPUT_PORT(5)) 
	#define ENCODER2_BITA (1u << 9)	// P5_9 IRQ4
	#define ENCODER2_BITB (1u << 8)	// P5_8 IRQ3
	#define ENCODER2_BITS (ENCODER2_BITA | ENCODER2_BITB)
	#define ENCODER2_SHIFT 8	// РћС‚СЃСѓС‚СЃС‚РІРёРµ СЌС‚РѕРіРѕ Р·РЅР°С‡РµРЅРёСЏ РѕР·РЅР°С‡Р°РµС‚ С‡С‚Рѕ Р±РёС‚С‹ РЅРµ РїРѕРґСЂСЏРґ

	#define ENCODER_INITIALIZE() \
		do { \
			arm_hardware_pio5_alternative(ENCODER_BITS, R7S721_PIOALT_4); \
			arm_hardware_irqn_interrupt(5, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt); /* IRQ5, both edges */ \
			arm_hardware_irqn_interrupt(6, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt); /* IRQ6, both edges */ \
			arm_hardware_pio5_alternative(ENCODER2_BITS, R7S721_PIOALT_4); \
			arm_hardware_irqn_interrupt(3, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt2); /* IRQ3, both edges */ \
			arm_hardware_irqn_interrupt(4, 3, ARM_OVERREALTIME_PRIORITY, spool_encinterrupt2); /* IRQ4, both edges */ \
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

	/* РѕС‚РєР»СЋС‡РёС‚СЊ РїСЂРѕС†РµСЃСЃРѕСЂ РѕС‚ SD РєР°СЂС‚С‹ - С‡С‚РѕР±С‹ РїСЂРё РІС‹РїРѕР»РЅРµРЅРёРё power cycle РЅРµ РІРѕР·РЅРёРєР°Р»Рѕ С„Р°РЅС‚РѕРјРЅРѕРµ РїРёС‚Р°РЅРёРµ С‡РµСЂРµР· СЃРёРіРЅР°Р»С‹ СѓРїСЂР°РІР»РµРЅРёСЏ. */
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

	#define HARDWARE_SDIOSENSE_CD() ((R7S721_INPUT_PORT(3) & HARDWARE_SDIO_CD_BIT) == 0)	/* РїРѕР»СѓС‡РёС‚СЊ СЃРѕСЃС‚РѕСЏРЅРёРµ РґР°С‚С‡РёРєР° CARD PRESENT */
	#define HARDWARE_SDIOSENSE_WP() ((R7S721_INPUT_PORT(3) & HARDWARE_SDIO_WP_BIT) != 0)	/* РїРѕР»СѓС‡РёС‚СЊ СЃРѕСЃС‚РѕСЏРЅРёРµ РґР°С‚С‡РёРєР° CARD WRITE PROTECT */

	/* РµСЃР»Рё РїРёС‚Р°РЅРёРµ SD CARD СѓРїСЂР°РІР»СЏРµС‚СЃСЏ РїСЂСЏРјРѕ СЃ РїСЂРѕС†РµСЃСЃРѕСЂР° */
	#define HARDWARE_SDIO_POWER_BIT (1uL << 15)	// P3_15
	#define HARDWARE_SDIO_POWER_S(v) do {	R7S721_TARGET_PORT_S(3, v); } while (0)
	#define HARDWARE_SDIO_POWER_C(v) do {	R7S721_TARGET_PORT_C(3, v); } while (0)

	#define HARDWARE_SDIOPOWER_INITIALIZE()	do { \
		arm_hardware_pio3_outputs(HARDWARE_SDIO_POWER_BIT, 0); /* РїРёС‚Р°РЅРёРµ РІС‹РєР»СЋС‡РµРЅРѕ */ \
	} while (0)

	/* parameter on not zero for powering SD CARD */
	#define HARDWARE_SDIOPOWER_SET(on)	do { \
		if ((on) != 0) \
			HARDWARE_SDIO_POWER_S(HARDWARE_SDIO_POWER_BIT); \
		else \
			HARDWARE_SDIO_POWER_C(HARDWARE_SDIO_POWER_BIT); \
	} while (0)

#endif /* WITHSDHCHW */
/* Р Р°СЃРїСЂРµРґРµР»РµРЅРёРµ Р±РёС‚РѕРІ РІ ARM РєРѕРЅС‚СЂРѕР»Р»РµСЂР°С… */

#if (WITHCAT && WITHCAT_USART1)
	// CAT data lites
	// RXD at P6_14, TXD at P6_15

	// CAT control lines
	#define FROMCAT_TARGET_PIN_RTS		(R7S721_INPUT_PORT(2)) // P2_0
	#define FROMCAT_BIT_RTS				(1u << 0)	/* СЃРёРіРЅР°Р» RTS РѕС‚ FT232RL	*/

	/* РјР°РЅРёРїСѓР»СЏС†РёСЏ РѕС‚ РїРѕСЂС‚Р° RS-232, СЃРёРіРЅР°Р» PPS РѕС‚ GPS/GLONASS/GALILEO РјРѕРґСѓР»СЏ */
	#define FROMCAT_TARGET_PIN_DTR		(R7S721_INPUT_PORT(2)) // P2_1
	#define FROMCAT_BIT_DTR				(1u << 1)	/* СЃРёРіРЅР°Р» DTR РѕС‚ FT232RL	*/

	/* РјР°РЅРёРїСѓР»СЏС†РёСЏ РѕС‚ РїРѕСЂС‚Р° RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(FROMCAT_BIT_DTR); \
		} while (0)

	/* РїРµСЂРµС…РѕРґ РЅР° РїРµСЂРµРґР°С‡Сѓ РѕС‚ РїРѕСЂС‚Р° RS-232 */
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
	//#define FROMCAT_BIT_RTS				(1u << 0)	/* СЃРёРіРЅР°Р» RTS РѕС‚ FT232RL	*/

	/* РјР°РЅРёРїСѓР»СЏС†РёСЏ РѕС‚ РїРѕСЂС‚Р° RS-232, СЃРёРіРЅР°Р» PPS РѕС‚ GPS/GLONASS/GALILEO РјРѕРґСѓР»СЏ */
	//#define FROMCAT_TARGET_PIN_DTR		(GPIO.PPR2) // P2_1
	//#define FROMCAT_BIT_DTR				(1u << 1)	/* СЃРёРіРЅР°Р» DTR РѕС‚ FT232RL	*/

	/* РјР°РЅРёРїСѓР»СЏС†РёСЏ РѕС‚ РїРѕСЂС‚Р° RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
		} while (0)

	/* РїРµСЂРµС…РѕРґ РЅР° РїРµСЂРµРґР°С‡Сѓ РѕС‚ РїРѕСЂС‚Р° RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
		} while (0)

#endif /* (WITHCAT && WITHCAT_CDC) */

#if (WITHNMEA && WITHNMEA_USART1)
	// CAT data lites
	// RXD at P6_14, TXD at P6_15

	// CAT control lines
	#define FROMCAT_TARGET_PIN_RTS		(R7S721_INPUT_PORT(2)) // P2_0
	#define FROMCAT_BIT_RTS				(1u << 0)	/* СЃРёРіРЅР°Р» RTS РѕС‚ FT232RL	*/

	/* РјР°РЅРёРїСѓР»СЏС†РёСЏ РѕС‚ РїРѕСЂС‚Р° RS-232, СЃРёРіРЅР°Р» PPS РѕС‚ GPS/GLONASS/GALILEO РјРѕРґСѓР»СЏ */
	#define FROMCAT_TARGET_PIN_DTR		(R7S721_INPUT_PORT(2)) // P2_1
	#define FROMCAT_BIT_DTR				(1u << 1)	/* СЃРёРіРЅР°Р» DTR РѕС‚ FT232RL	*/

	/* РјР°РЅРёРїСѓР»СЏС†РёСЏ РѕС‚ РїРѕСЂС‚Р° RS-232 */
	#define FROMCAT_DTR_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(FROMCAT_BIT_DTR); \
		} while (0)

	/* РїРµСЂРµС…РѕРґ РЅР° РїРµСЂРµРґР°С‡Сѓ РѕС‚ РїРѕСЂС‚Р° RS-232 */
	#define FROMCAT_RTS_INITIALIZE() \
		do { \
			arm_hardware_pio2_inputs(FROMCAT_BIT_RTS); \
		} while (0)

	/* СЃРёРіРЅР°Р» PPS РѕС‚ GPS/GLONASS/GALILEO РјРѕРґСѓР»СЏ */
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
	#define TXGFV_TRANS		0			// РїРµСЂРµС…РѕРґ РјРµР¶РґСѓ СЂРµР¶РёРјР°РјРё РїСЂРёС‘РјР° Рё РїРµСЂРµРґР°С‡Рё
	#define TXGFV_TX_SSB	(1u << 0)
	#define TXGFV_TX_CW		(1u << 1)
	#define TXGFV_TX_AM		(1u << 2)
	#define TXGFV_TX_NFM	(1u << 3)

	#define TXPATH_INITIALIZE() \
		do { \
		} while (0)

	// PTT input - P2_2 (Р±С‹Р»Рѕ P6_12)
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

/* РќР°Р·РЅР°С‡РµРЅРёРµ Р°РґСЂРµСЃРѕРІ РЅР° SPI С€РёРЅРµ */
#define targetdds1_u	(1U << 2)	// P7_2  FPGA NCO registers CS
#define targetfpga1		(1U << 6)	// P7_6  FPGA control registers CS1
#define targetrtc1		(1U << 7)	// P7_7  RTC DS1305EN
#define targetext1		(1U << 8)	// P7_8  external spi device (LCD)
#define targetnvram		(1U << 9)	// P7_9  nvmem FM25L16B
#define targetctl1		(1U << 10)	// P7_10 board control registers chain
#define targetcodec1	(1U << 11)	// P7_11 on-board codec1 NAU8822L

#define targetuc1608	targetext1 

// Р—РґРµСЃСЊ РґРѕР»Р¶РЅС‹ Р±С‹С‚СЊ РїРµСЂРµС‡РёСЃР»РµРЅС‹ РІСЃРµ Р±РёС‚С‹ С„РѕСЂРјРёСЂРѕРІР°РЅРёСЏ CS РІ СѓСЃС‚СЂРѕР№СЃС‚РІРµ.
#define SPI_ALLCS_BITS ( \
	targetdds1_u	| /* P7_2  FPGA NCO registers CS */ \
	targetfpga1		| /* P7_6  FPGA control registers CS1 */ \
	targetrtc1		| /* P7_7  RTC DS1305EN */ \
	targetext1		| /* P7_8  external spi device (LCD) */ \
	targetnvram		| /* P7_9  nvmem FM25L16B */ \
	targetctl1		| /* P7_10 board control registers chain */ \
	targetcodec1	| /* P7_11 on-board codec1 NAU8822L */ \
	0)

#define SPI_ALLCS_BITSNEG (targetrtc1)		// Р’С‹С…РѕРґС‹, Р°РєС‚РёРІРЅС‹Рµ РїСЂРё "1"

#if 0

	#define SPI_ADDRESS_PORT_S(v) do {	R7S721_TARGET_PORT_S(xx, v); } while (0)
	#define SPI_ADDRESS_PORT_C(v) do {	R7S721_TARGET_PORT_C(xx, v); } while (0)

	// Р‘РёС‚С‹ Р°РґСЂРµСЃР° РґР»СЏ РґРµС€РёС„СЂР°С‚РѕСЂР° SPI
	#define SPI_A0 ((1u << xx))			// * PE13 
	#define SPI_A1 ((1u << xx))			// * PE14 
	#define SPI_A2 ((1u << xx))			// * PE15 

	#define SPI_NAEN_PORT_S(v) do {	R7S721_TARGET_PORT_S(xx, v); } while (0)
	#define SPI_NAEN_PORT_C(v) do {	R7S721_TARGET_PORT_C(xx, v); } while (0)

	#define SPI_NAEN_BIT (1u << xx)		// * PE7 used

#endif


// РќР°Р±РѕСЂ РѕРїСЂРµРґРµР»РµРЅРёР№ РґР»СЏ СЂР°Р±РѕС‚С‹ Р±РµР· РІРЅРµС€РЅРµРіРѕ РґРµС€РёС„СЂР°С‚РѕСЂР°
#define SPI_ALLCS_PORT_S(v) do {	R7S721_TARGET_PORT_S(7, v); } while (0)
#define SPI_ALLCS_PORT_C(v) do {	R7S721_TARGET_PORT_C(7, v); } while (0)

/* РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ Р»РёРёР№ РІС‹Р±РѕСЂР° РїРµСЂРёС„РµСЂРёР№РЅС‹С… РјРёРєСЂРѕСЃС…РµРј */
#define SPI_ALLCS_INITIALIZE() \
	do { \
		arm_hardware_pio7_outputs(SPI_ALLCS_BITS, SPI_ALLCS_BITS ^ SPI_ALLCS_BITSNEG); \
	} while (0)

// reset РєРѕРґРµРєР°
////#define TARGET_CS4272_RESET_PORT_S(v)		do { R7S721_TARGET_PORT_S(7, v); } while (0)
////#define TARGET_CS4272_RESET_PORT_C(v)		do { R7S721_TARGET_PORT_C(7, v); } while (0)
////#define TARGET_CS4272_RESET_BIT		(1U << 2)	// PD2

// RSPI0 used
#define HW_SPIUSED (& RSPI0)
// MOSI & SCK port
#define SPI_TARGET_SCLK_PORT_S(v) do {	R7S721_TARGET_PORT_S(6, v); } while (0)
#define SPI_TARGET_SCLK_PORT_C(v) do {	R7S721_TARGET_PORT_C(6, v); } while (0)
#define	SPI_SCLK_BIT			(1U << 0)	// * P6_0 Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ СЃРёРЅС…СЂРѕРЅРёР·Р°С†РёСЏ SPI

#define SPI_TARGET_MOSI_PORT_S(v) do {	R7S721_TARGET_PORT_S(6, v); } while (0)
#define SPI_TARGET_MOSI_PORT_C(v) do {	R7S721_TARGET_PORT_C(6, v); } while (0)
#define	SPI_MOSI_BIT			(1U << 2)	// * P6_2 Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІС‹РІРѕРґ (РёР»Рё РІРІРѕРґ РІ СЃР»СѓС‡Р°Рµ РґРІСѓРЅР°РїСЂР°РІР»РµРЅРЅРѕРіРѕ SPI).

#define SPI_TARGET_MISO_PIN		(R7S721_INPUT_PORT(6))		// was PINA 
#define	SPI_MISO_BIT			(1U << 3)	// * P6_3 Р±РёС‚, С‡РµСЂРµР· РєРѕС‚РѕСЂС‹Р№ РёРґРµС‚ РІРІРѕРґ СЃ SPI.

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
	// РѕС‚РєСЂС‹С‚С‹Р№ СЃС‚РѕРє P1 by design
	// P1_6 - SCL3
	// P1_7 - SDA3
	#define TARGET_TWI_TWCK_PORT_C(v) do {	R7S721_TARGET_PORT_S(1, v); } while (0)
	#define TARGET_TWI_TWCK_PORT_S(v) do {	R7S721_TARGET_PORT_C(1, v); } while (0)
	#define TARGET_TWI_TWCK_PIN		(R7S721_INPUT_PORT(1))
	#define TARGET_TWI_TWCK		(1u << 6)		// P1_6 - SCL3

	#define TARGET_TWI_TWD_PORT_C(v) do {	R7S721_TARGET_PORT_S(1, v); } while (0)
	#define TARGET_TWI_TWD_PORT_S(v) do {	R7S721_TARGET_PORT_C(1, v); } while (0)
	#define TARGET_TWI_TWD_PIN		(R7S721_INPUT_PORT(1))
	#define TARGET_TWI_TWD		(1u << 7)		// P1_7 - SDA3

	// РРЅРёС†РёР°Р»РёР·Р°С†РёСЏ Р±РёС‚РѕРІ РїРѕСЂС‚РѕРІ РІРІРѕРґР°-РІС‹РІРѕРґР° РґР»СЏ РїСЂРѕРіСЂР°РјРјРЅРѕР№ СЂРµР°Р»РёР·Р°С†РёРё I2C
	#define	TWISOFT_INITIALIZE() do { \
			arm_hardware_pio1_outputs(TARGET_TWI_TWCK | TARGET_TWI_TWD, TARGET_TWI_TWCK | TARGET_TWI_TWD); \
		} while (0) 
	// РРЅРёС†РёР°Р»РёР·Р°С†РёСЏ Р±РёС‚РѕРІ РїРѕСЂС‚РѕРІ РІРІРѕРґР°-РІС‹РІРѕРґР° РґР»СЏ Р°РїРїР°СЂР°С‚РЅРѕР№ СЂРµР°Р»РёР·Р°С†РёРё I2C
	// РїСЂРёСЃРѕРµРґРёРЅРµРЅРёРµ РІС‹РІРѕРґРѕРІ Рє РїРµСЂРёС„РµСЂРёР№РЅРѕРјСѓ СѓСЃС‚СЂРѕР№СЃС‚РІСѓ
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

	/* РРЅРёС†РёР°РґРёР·Р°С†РёСЏ РІС‹РІРѕРґРѕРІ GPIO РїСЂРѕС†РµСЃСЃРѕСЂР° РґР»СЏ РїРѕР»СѓС‡РµРЅРёСЏ СЃРѕСЃС‚РѕСЏРЅРёСЏ Рё СѓРїСЂР°РІР»РµРЅРёРµРј Р·Р°РіСЂСѓР·РєРѕР№ FPGA */
	#define HARDWARE_FPGA_LOADER_INITIALIZE() do { \
			arm_hardware_pio6_outputs(FPGA_NCONFIG_BIT, FPGA_NCONFIG_BIT); \
			arm_hardware_pio6_inputs(FPGA_CONF_DONE_BIT | FPGA_NSTATUS_BIT); \
		} while (0)

	/* РїРѕ РєРѕСЃРІРµРЅРЅС‹Рј РїСЂРёР·РЅР°РєР°Рј РїСЂРѕРІРµСЂСЏРµРј, РїСЂРѕРёРЅРёС†РёР°Р»РёР·РёСЂРѕРІР°Р»Р°СЃСЊ Р»Рё FPGA (РІРѕС€Р»Р° РІ user mode). */
	#define HARDWARE_FPGA_IS_USER_MODE() (local_delay_ms(400), 1)

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if WITHDSPEXTFIR
	// Р‘РёС‚С‹ РґРѕСЃС‚СѓРїР° Рє РјР°СЃСЃРёРІСѓ РєРѕСЌС„С„РёС†РёРµРЅС‚РѕРІ FIR С„РёР»СЊС‚СЂР° РІ FPGA
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

	/* РїРѕР»СѓС‡РµРЅРёРµ СЃРѕСЃС‚РѕСЏРЅРёСЏ РїРµСЂРµРїРѕР»РЅРµРЅРёСЏ РђР¦Рџ */
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

		#define KBD_MASK 0 //(0x01 | 0x02)	// РІСЃРµ РёСЃРїРѕР»СЊР·СѓРµРјС‹Рµ Р±РёС‚С‹
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

	/* РјР°РєСЂРѕРѕРїСЂРµРґРµР»РµРЅРёРµ, РєРѕС‚РѕСЂРѕРµ РґРѕР»Р¶РЅРѕ РІРєР»СЋС‡РёС‚СЊ РІ СЃРµР±СЏ РІСЃРµ РёРЅРёС†РёР°Р»РёР·Р°С†РёРё */
	#define	HARDWARE_INITIALIZE() do { \
		HARDWARE_SIDETONE_INITIALIZE(); \
		HARDWARE_KBD_INITIALIZE(); \
		HARDWARE_DAC_INITIALIZE(); \
		} while (0)

#endif /* ARM_R7S72_TQFP176_CPUSTYLE_RAVEN_V2_H_INCLUDED */
