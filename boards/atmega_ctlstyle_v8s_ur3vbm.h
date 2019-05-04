/* $Id$ */
/* board-specific CPU attached signals */
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//

// РѕРїРёСЃР°РЅРёРµ Р°РїРїР°СЂР°С‚СѓСЂС‹ "РїСЂРѕСЃС‚РѕР№ СЃРёРЅС‚РµР·Р°С‚РѕСЂ РЅР° AD9835"
// РЎСЂРµРґРё РёСЃРїРѕР»СЊР·СѓСЋС‰РёС… ur5zvu@yandex.ru - RDX0077, Si570

#ifndef ATMEGA_CTLSTYLE_V8S_UR3VBM_INCLUDED
#define ATMEGA_CTLSTYLE_V8S_UR3VBM_INCLUDED 1

	#if F_CPU != 8000000
		#error Set F_CPU right value in project file
	#endif

	/* РјРѕРґРµР»Рё СЃРёРЅС‚РµР·Р°С‚РѕСЂРѕРІ - СЃС…РµРјС‹ С‡Р°СЃС‚РѕС‚РѕРѕР±СЂР°Р·РѕРІР°РЅРёСЏ */

	//#define ELKEY328 		1	/* СЌР»РµРєС‚СЂРѕРЅРЅС‹Р№ РєР»СЋС‡	*/
#if 0
	/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РІРЅРёР·" */
	//#define DIRECT_75M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_66M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_66M66_X1		1	/* Board hardware configuration */
	//#define DIRECT_48M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_50M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_80M0_X1		1	/* Board hardware configuration */
	#define DIRECT_27M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_26M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_60M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_125M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_XXXX_X1	1
	//#define REFERENCE_FREQ	24000000uL	/* Р”Р»СЏ Si5351 */

	#define FQMODEL_TRX8M		1	// РџРµСЂРІР°СЏ РЅРёР·РєР°СЏ РџР§ 8 РњР“С†, 6 РњР“С†, 5.5 РњР“С† Рё РґСЂСѓРіРёРµ

	//#define BANDSELSTYLERE_LOCONV32M	1
	#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
	// Р’С‹Р±РѕСЂ РџР§
	//#define IF3_MODEL IF3_TYPE_9000
	//#define IF3_MODEL IF3_TYPE_8000
	#define IF3_MODEL IF3_TYPE_8868
	//#define IF3_MODEL IF3_TYPE_6000
	//#define IF3_MODEL IF3_TYPE_8192
	//#define IF3_MODEL IF3_TYPE_5645	// Drake R-4C and Drake T-4XC (Drake Twins) - 5645 kHz
	//#define IF3_MODEL IF3_TYPE_5500
	//#define IF3_MODEL IF3_TYPE_9045 
	//#define IF3_MODEL IF3_TYPE_10700
#elif 1
	/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РІРЅРёР·" */
	//#define DIRECT_75M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_66M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_48M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_50M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_60M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_125M0_X1		1	/* Board hardware configuration */
	#define DIRECT_27M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_26M0_X1		1	/* Board hardware configuration */

	#define FQMODEL_TRX500K		1	// РџРµСЂРІР°СЏ (Рё РµРґРёРЅСЃС‚РІРµРЅРЅР°) РџР§ 500 РєР“С†
	//#define BANDSELSTYLERE_LOCONV15M	1
	#define BANDSELSTYLERE_LOCONV15M_NLB	1	/* Down-conversion with working band 1.6..15 MHz */
#elif 1
	/* РџСЂСЏРјРѕРµ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµ */
	//#define DIRECT_50M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_125M0_X1		1	/* Board hardware configuration */
	#define DIRECT_27M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_26M0_X1		1	/* Board hardware configuration */
	#define FQMODEL_DCTRX		1	// РїСЂСЏРјРѕРµ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµ
	//#define BANDSELSTYLERE_LOCONV32M	1
	#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
#endif	
	// --- РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ СЂР°Р·РЅС‹С… РїР»Р°С‚
	#define	MODEL_DIRECT	1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїСЂСЏРјРѕР№ СЃРёРЅС‚РµР·, Р° РЅРµ РіРёР±СЂРёРґРЅС‹Р№ */

	// --- РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ СЂР°Р·РЅС‹С… РїР»Р°С‚

	#define DSTYLE_UR3LMZMOD	1
	//#define	FONTSTYLE_ITALIC	1	//

	#define CTLREGMODE16_UR3VBM	1		/* СѓРїСЂР°РІР»СЏСЋС‰РёР№ СЂРµРіРёСЃС‚СЂ 16 Р±РёС‚ - РґР»СЏ SIMPLE СЃРёРЅС‚РµР·Р°С‚РѕСЂР° РЅР° AD8932/AD8935/AD8933	*/
	//#define CTLREGMODE8_UR5YFV	1	/* 8 Р±РёС‚ */
	//#define CTLREGMODE16_RN3ZOB	1		/* СѓРїСЂР°РІР»СЏСЋС‰РёР№ СЂРµРіРёСЃС‚СЂ 16 Р±РёС‚ - РґР»СЏ SIMPLE СЃРёРЅС‚РµР·Р°С‚РѕСЂР° РЅР° AD8932/AD8935/AD8933	*/
	//
	//#define CTLREGSTYLE_NOCTLREG	1	/* РѕС‚СЃСѓС‚СЃС‚РІСѓРµС‚ СѓРїСЂР°РІР»СЏСЋС‰РёР№ СЂРµРіРёСЃС‚СЂ - РґР»СЏ С‚РµСЃС‚РёСЂРѕРІР°РЅРёСЏ. */

	// +++ РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ РїР»Р°С‚ РЅР° ATMega
	// --- РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ РїР»Р°С‚ РЅР° ATMega

	// +++ РћСЃРѕР±С‹Рµ РІР°СЂРёР°РЅС‚С‹ СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ РєРЅРѕРїРѕРє РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ
	#define KEYB_VERTICAL_UR3VBM	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ РїР»Р°С‚ "Р’РѕСЂРѕР±РµР№" Рё "РљРѕР»РёР±СЂРё" */

	#define WITHSPLIT	1	/* СѓРїСЂР°РІР»РµРЅРёРµ СЂРµР¶РёРјР°РјРё СЂР°СЃСЃС‚СЂРѕР№РєРё РѕРґРЅРѕР№ РєРЅРѕРїРєРѕР№ */
	//#define WITHSPLITEX	1	/* РўСЂРµС…РєРЅРѕРїРѕС‡РЅРѕРµ СѓРїСЂР°РІР»РµРЅРёРµ СЂРµР¶РёРјР°РјРё СЂР°СЃСЃС‚СЂРѕР№РєРё */
	//#define WITHCATEXT	1	/* Р Р°СЃС€РёСЂРµРЅРЅС‹Р№ РЅР°Р±РѕСЂ РєРѕРјР°РЅРґ CAT */
	//#define WITHELKEY	1
	//#define WITHKBDENCODER 1	// РїРµСЂРµСЃС‚СЂРѕР№РєР° С‡Р°СЃС‚РѕС‚С‹ РєРЅРѕРїРєР°РјРё
	#define WITHKEYBOARD 1	/* РІ РґР°РЅРЅРѕРј СѓСЃС‚СЂРѕР№СЃС‚РІРµ РµСЃС‚СЊ РєР»Р°РІРёР°С‚СѓСЂР° */
	#define KEYBOARD_USE_ADC	1	/* РЅР° РѕРґРЅРѕР№ Р»РёРЅРёРё СѓСЃС‚Р°РЅРѕРІР»РµРЅРѕ  С‡РµС‚С‹СЂРµ  РєР»Р°РІРёС€Рё. РЅР° vref - 6.8K, РґР°Р»РµРµ 2.2Рљ, 4.7Рљ Рё 13K. */
	//#define KEYBOARD_USE_ADC6	1	/* С€РµСЃС‚СЊ РєРЅРѕРїРѕРє РЅР° РєР°Р¶РґРѕРј РІС…РѕРґРµ ADCx	*/
	// --- РћСЃРѕР±С‹Рµ РІР°СЂРёР°РЅС‚С‹ СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ РєРЅРѕРїРѕРє РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ

	// +++ РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	//#define LCDMODE_WH2002	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 20*2, РІРѕР·РјРѕР¶РЅРѕ РІРјРµСЃС‚Рµ СЃ LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 16*2 */
	//#define LCDMODE_WH2004	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 20*4 */
	//#define LCDMODE_RDX0077	1	/* РРЅРґРёРєР°С‚РѕСЂ 128*64 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1601.  */
	//#define LCDMODE_RDX0154	1	/* РРЅРґРёРєР°С‚РѕСЂ 132*64 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1601. */
	//#define LCDMODE_RDX0120	1	/* РРЅРґРёРєР°С‚РѕСЂ 64*32 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1601.  */
	//#define LCDMODE_UC1601S_XMIRROR	1	/* РРЅРґРёРєР°С‚РѕСЂ 132*64 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1601.  */
	//#define LCDMODE_UC1601S_TOPDOWN	1	/* LCDMODE_RDX0154 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	//#define LCDMODE_UC1601S_EXTPOWER	1	/* LCDMODE_RDX0154 - Р±РµР· РІРЅСѓС‚СЂРµРЅРЅРµРіРѕ РїСЂРµРѕР±СЂР°Р·РѕРІР°С‚РµР»СЏ */
	//#define LCDMODE_TIC154	1	/* РРЅРґРёРєР°С‚РѕСЂ 133*65 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј PCF8535 */
	//#define LCDMODE_TIC218	1	/* РРЅРґРёРєР°С‚РѕСЂ 133*65 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј PCF8535 */
	//#define LCDMODE_PCF8535_TOPDOWN	1	/* РљРѕРЅС‚СЂРѕР»Р»РµСЂ PCF8535 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	//#define LCDMODE_LS020 	1	/* РРЅРґРёРєР°С‚РѕСЂ 176*132 Sharp LS020B8UD06 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	//#define LCDMODE_LPH88		1	/* РРЅРґРёРєР°С‚РѕСЂ 176*132 LPH8836-2 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	#define LCDMODE_ILI9163	1	/* РРЅРґРёРєР°С‚РѕСЂ LPH9157-2 176*132 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILITEK ILI9163 - Р›РµРЅС‚Р° РґРёСЃРїР»РµСЏ СЃРїСЂР°РІР°, Р° РІС‹РІРѕРґС‹ СЃР»РµРІР°. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРїСЂР°РІР°, Р»РµРЅС‚Р° РґРёСЃРїР»РµСЏ СЃР»РµРІР°) */
	//#define LCDMODE_L2F50	1	/* РРЅРґРёРєР°С‚РѕСЂ 176*132 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Epson L2F50126 */
	//#define LCDMODE_L2F50_TOPDOWN	1	/* РџРµСЂРµРІРѕСЂРѕС‚ РёР·РѕР±СЂР°Р¶РµРЅРё СЏ РІ СЃР»СѓС‡Р°Рµ LCDMODE_L2F50 */
	//#define LCDMODE_S1D13781	1	/* РРЅРЅРґРёРєР°С‚РѕСЂ 480*272 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	//#define LCDMODE_ILI9225	1	/* РРЅРґРёРєР°С‚РѕСЂ 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9225РЎ */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃР»РµРІР° РѕС‚ СЌРєСЂР°РЅР°) */
	//#define LCDMODE_UC1608	1		/* РРЅРґРёРєР°С‚РѕСЂ 240*128 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1608.- РјРѕРЅРѕС…СЂРѕРјРЅС‹Р№ */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРІРµСЂС…Сѓ) */
	//#define LCDMODE_ST7735	1	/* РРЅРґРёРєР°С‚РѕСЂ 160*128 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Sitronix ST7735 */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРїСЂР°РІР°) */
	//#define LCDMODE_ST7565S	1	/* РРЅРґРёРєР°С‚РѕСЂ WO12864C2-TFH# 128*64 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРІРµСЂС…Сѓ) */
	//#define LCDMODE_ILI9341	1	/* 320*240 SF-TC240T-9370-T СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9341 - STM32F4DISCO */
	//#define LCDMODE_ILI9341_TOPDOWN	1	/* LCDMODE_ILI9341 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРїСЂР°РІР°) */
	//#define LCDMODE_ILI9320	1	/* РРЅРґРёРєР°С‚РѕСЂ 248*320 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9320 */
	// --- РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°

	//#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_100
	#define ENCRES_DEFAULT ENCRES_24
	//#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define	WITHENCODER	1	/* РґР»СЏ РёР·РјРµРЅРµРЅРёСЏ С‡Р°СЃС‚РѕС‚С‹ РёРјРµРµС‚СЃСЏ РµРЅРєРѕРґРµСЂ */



	// +++ Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ
	//#define WITHRFSG	1	/* РІРєР»СЋС‡РµРЅРѕ СѓРїСЂР°РІР»РµРЅРёРµ Р’Р§ СЃРёРіРЅР°Р»-РіРµРЅРµСЂР°С‚РѕСЂРѕРј. */
	#define WITHTX		1	/* РІРєР»СЋС‡РµРЅРѕ СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - СЃРёРєРІРµРЅСЃРѕСЂ, СЌР»РµРєС‚СЂРѕРЅРЅС‹Р№ РєР»СЋС‡. */
	//#define WITHPBT		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ PBT */
	//#define WITHIFSHIFT	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* РќР°С‡Р°Р»СЊРЅРѕРµ Р·Р°РЅС‡РµРЅРёРµ IF SHIFT */
	#define WITHCAT		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ CAT */
	//#define WITHDEBUG		1	/* РћС‚Р»Р°РґРѕС‡РЅР°СЏ РїРµС‡Р°С‚СЊ С‡РµСЂРµР· COM-РїРѕСЂС‚. Р‘РµР· CAT (WITHCAT) */
	//#define WITHVOX		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ VOX */
	#define WITHBARS	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ S-РјРµС‚СЂР° Рё SWR-РјРµС‚СЂР° */
	#define WITHSHOWSWRPWR 1	/* РЅР° РґРёСЃРїР»РµРµ РѕРґРЅРѕРІСЂРµРјРµРЅРЅРѕ РѕС‚РѕР±СЂР°Р¶Р°СЋС‚СЃСЏ SWR-meter Рё PWR-meter - РўСЂРµР±СѓРµС‚ WITHSWRMTR */
	#define WITHSWRMTR	1	/* РР·РјРµСЂРёС‚РµР»СЊ РљРЎР’ РёР»Рё */
	//#define WITHPWRMTR	1	/* РРЅРґРёРєР°С‚РѕСЂ РІС‹С…РѕРґРЅРѕР№ РјРѕС‰РЅРѕСЃС‚Рё  */
	//#define WITHVOLTLEVEL	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ РЅР°РїСЂСЏР¶РµРЅРёСЏ РђРљР‘ */
	//#define WITHSWLMODE	1	/* РїРѕРґРґРµСЂР¶РєР° Р·Р°РїРѕРјРёРЅР°РЅРёСЏ РјРЅРѕР¶РµСЃС‚РІР° С‡Р°СЃС‚РѕС‚ РІ swl-mode */

	#define WITHMENU 	1	/* С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РјРµРЅСЋ РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕС‚РєР»СЋС‡РµРЅР° - РµСЃР»Рё РЅР°СЃС‚СЂР°РёРІР°С‚СЊ РЅРµС‡РµРіРѕ */
	//#define WITHVIBROPLEX	1	/* РІРѕР·РјРѕР¶РЅРѕСЃС‚СЊ СЌРјСѓР»СЏС†РёРё РїРµСЂРµРґР°С‡Рё РІРёР±СЂРѕРїР»РµРєСЃРѕРј */
	//#define WITHPOTWPM		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ СЂРµРіСѓР»РёСЂРѕРІРєР° СЃРєРѕСЂРѕСЃС‚Рё РїРµСЂРµРґР°С‡Рё РІ С‚РµР»РіСЂР°С„Рµ РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂРѕРј */
	#define WITHSLEEPTIMER	1	/* РІС‹РєР»СЋС‡РёС‚СЊ РёРЅРґРёРєР°С‚РѕСЂ Рё РІС‹РІРѕРґ Р·РІСѓРєР° РїРѕ РёСЃС‚РµС‡РµРЅРёРёРё СѓРєР°Р·Р°РЅРЅРѕРіРѕ РІСЂРµРјРµРЅРё */
	// --- Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ

	//#define LO1PHASES	1		/* РџСЂСЏРјРѕР№ СЃРёРЅС‚РµР· РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° РґРІСѓРјСЏ DDS СЃ РїСЂРѕРіСЂР°РјРёРјСЂСѓРµРјС‹Рј СЃРґРІРёРіРѕРј С„Р°Р·С‹ */
	#define DEFPREAMPSTATE 	1	/* РЈР’Р§ РїРѕ СѓРјРѕР»С‡Р°РЅРёСЋ РІРєР»СЋС‡С‘РЅ (1) РёР»Рё РІС‹РєР»СЋС‡РµРЅ (0) */
	
	//#define REQUEST_FOR_RN4NAB 1	/* РЎРїРµС†РёР°Р»СЊРЅС‹Р№ РІР°СЂРёР°РЅС‚ РґРёР°РїР°Р·РѕРЅРЅС‹С… С„РёР»СЊС‚СЂРѕРІ */

	//#define LSBONLO4DEFAULT	1	/* РїСЂРёРјРµРЅС‘РЅ Р­РњР’ РЅР° РЅРёР¶РЅСЋСЋ Р±РѕРєРѕРІСѓСЋ */
	//#define WITHFIXEDBFO	1	/* РџРµСЂРµРІРѕСЂРѕС‚ Р±РѕРєРѕРІС‹С… Р·Р° СЃС‡С‘С‚ 1-РіРѕ РіРµС‚СЂРѕРґРёРЅР° (РѕСЃРѕР±РµРЅРЅРѕ, РµСЃР»Рё РЅРµС‚ РїРѕРґСЃС‚СЂР°РёРІР°РµРјРѕРіРѕ BFO) */
	//#define WITHDUALFLTR	1	/* РџРµСЂРµРІРѕСЂРѕС‚ Р±РѕРєРѕРІС‹С… Р·Р° СЃС‡С‘С‚ РїРµСЂРµРєР»СЋС‡РµРЅРёСЏ С„РёР»СЊС‚СЂР° РІРµСЂС…РЅРµР№ РёР»Рё РЅРёР¶РЅРµР№ Р±РѕРєРѕРІРѕР№ РїРѕР»РѕСЃС‹ */
	#define WITHSAMEBFO	1	/* РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РѕР±С‰РёС… РЅР°СЃС‚СЂРѕРµРє BFO РґР»СЏ РїСЂРёС‘РјР° Рё РїРµСЂРµР¶Р°С‡Рё */
	//#define WITHONLYBANDS 1		/* РџРµСЂРµСЃС‚СЂРѕР№РєР° РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕРіСЂР°РЅРёС‡РµРЅР° Р»СЋР±РёС‚РµР»СЊСЃРєРёРјРё РґРёР°РїР°Р·РѕРЅР°РјРё */
	//#define WITHBCBANDS	1		/* РІ С‚Р°Р±Р»РёС†Рµ РґРёР°РїР°Р·РѕРЅРѕРІ РїСЂРёСЃСѓС‚СЃС‚РІСѓСЋС‚ РІРµС‰Р°С‚РµР»СЊРЅС‹Рµ РґРёР°РїР°Р·РѕРЅС‹ */
	#define WITHWARCBANDS	1	/* Р’ С‚Р°Р±Р»РёС†Рµ РґРёР°РїР°Р·РѕРЅРѕРІ РїСЂРёСЃСѓС‚СЃС‚РІСѓСЋС‚ HF WARC РґРёР°РїР°Р·РѕРЅС‹ */
	//#define WITHDIRECTBANDS 1	/* РџСЂСЏРјРѕР№ РїРµСЂРµС…РѕРґ Рє РґРёР°РїР°Р·РѕРЅР°Рј РїРѕ РЅР°СЌР°С‚РёСЏРј РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ */
	#define WITHFLATMENU	1
	/* С‡С‚Рѕ Р·Р° РїР°РјСЏС‚СЊ РЅР°СЃС‚СЂРѕРµРє Рё С‡Р°СЃС‚РѕС‚ РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІ РєРѕРЅС‚СЂРѕР»Р»РµСЂРµ */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// РўР°Рє Р¶Рµ РїСЂРё РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРё FM25040A - 5 РІРѕР»СЊС‚, 512 Р±Р°Р№С‚
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	#define FTW_RESOLUTION 32	/* СЂР°Р·СЂСЏРґРЅРѕСЃС‚СЊ FTW РІС‹Р±СЂР°РЅРЅРѕРіРѕ DDS */

	//#define WITHLO1LEVELADJ		1	/* РІРєР»СЋС‡РµРЅРѕ СѓРїСЂР°РІР»РµРЅРёРµ СѓСЂРѕРІРЅРµРј (Р°РјРїР»РёС‚СѓРґРѕР№) LO1 */
	/* Board hardware configuration */
	//#define PLL1_TYPE PLL_TYPE_SI5351A
	//#define PLL1_FRACTIONAL_LENGTH	10	/* Si5351a: lower 10 bits is a fractional part */
	//#define PLL2_TYPE PLL_TYPE_SI5351A
	//#define PLL2_FRACTIONAL_LENGTH	10	/* Si5351a: lower 10 bits is a fractional part */
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9835	/* AD9832, AD9835 */
	//#define PLL1_TYPE PLL_TYPE_LMX1601
	//#define DDS1_TYPE DDS_TYPE_AD9851	/* AD9851, AD9850 */
	//#define DDS1_TYPE DDS_TYPE_ATTINY2313
	//#define DDS1_TYPE DDS_TYPE_AD9834
	#define WITHSI5351AREPLACE 1

	//#define DDS1_CLK_DIV	7		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 */
	#define DDS2_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS2 */

	/* РќР°Р·РЅР°С‡РµРЅРёРµ Р°РґСЂРµСЃРѕРІ РЅР° SPI С€РёРЅРµ */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetnvram SPI_CSEL1  	/* serial nvram */
	#define targetlcd SPI_CSEL4 	/* LCD over SPI line devices control */ 
	#define targetctl1 SPI_CSEL6 	/* control register */
	//#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/

	/* РєРѕРґС‹ С„РёР»СЊС‚СЂРѕРІ РІС‚РѕСЂРѕР№ РџР§, РІС‹РґР°РІР°РµРјС‹Рµ РЅР° РґРµС€РёС„СЂР°С‚РѕСЂС‹ */
	#define BOARD_FILTER_0P5		BOARD_FILTERCODE_0	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_3P1		BOARD_FILTERCODE_1	/* 3.1 or 2.75 kHz filter */

	//#define BOARD_AGCCODE_OFF 0
	//#define WITHMODESETSMART 1	/* РІ Р·Р°РІР°РёСЃРјРѕСЃС‚Рё РѕС‚ С‡Р°СЃС‚РѕС‚С‹ РјРµРЅСЏРµС‚СЃСЏ Р±РѕРєРѕРІР°СЏ, РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІРјРµСЃС‚Рµ СЃ WITHFIXEDBFO */
	#if 0 //WITHFIXEDBFO || WITHDUALBFO
		#define	WITHMODESETMIXONLY	1	// USB/LSB, CW/CWR - РґР»СЏ FIXEDBFO
		/* РІСЃРµ РІРѕР·РјРѕР¶РЅС‹Рµ РІ РґР°РЅРЅРѕР№ РєРѕРЅС„РёРіСѓСЂР°С†РёРё С„РёР»СЊС‚СЂС‹ */
		#define IF3_FMASK	(IF3_FMASK_3P1)
		#define IF3_FHAVE 	(IF3_FMASK_3P1)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
	#elif 1//FQMODEL_DCTRX
		#define WITHMODESETMIXONLY 1	// USB/LSB, CW/CWR
		/* РІСЃРµ РІРѕР·РјРѕР¶РЅС‹Рµ РІ РґР°РЅРЅРѕР№ РєРѕРЅС„РёРіСѓСЂР°С†РёРё С„РёР»СЊС‚СЂС‹ */
		#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FHAVE 	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
	#else
		#error Config error
		/* РІСЃРµ РІРѕР·РјРѕР¶РЅС‹Рµ РІ РґР°РЅРЅРѕР№ РєРѕРЅС„РёРіСѓСЂР°С†РёРё С„РёР»СЊС‚СЂС‹ */
		//#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_1P8 | IF3_FMASK_2P7 | IF3_FMASK_3P1)
		//#define IF3_FHAVE 	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		//#define IF3_FMASKTX	(IF3_FMASK_3P1)
	#endif


// РќР°Р·РЅР°С‡РµРЅРёСЏ РІС…РѕРґРѕРІ РђР¦Рџ РїСЂРѕС†РµСЃСЃРѕСЂР°.
enum 
{ 
#if WITHVOX
	VOXIX = 2, AVOXIX = 1,	// VOX
#endif
#if WITHBARS
	SMETERIX = 0,	// S-meter
	#if WITHSWRMTR
		PWRI = 4,	// SWR-meter
		FWD = 4, REF = 3,	// SWR-meter
	#endif
	#if WITHPWRMTR
		PWRI = 4,	// SWR-meter
	#endif
#endif
#if WITHPOTWPM
	POTWPM = 3,		// РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂ СѓРїСЂР°РІР»РµРЅРёСЏ СЃРєРѕСЂРѕСЃС‚СЊСЋ РїРµСЂРµРґР°С‡Рё РІ С‚РµР»РµРіСЂР°С„Рµ
#endif
	KI0 = 6, KI1 = 7	// РєР»Р°РІРёР°С‚СѓСЂР°
};
#define KI_COUNT 2	// РєРѕР»РёС‡РµСЃС‚РІРѕ РёСЃРїРѕР»СЊР·СѓРµРјС‹С… РїРѕРґ РєР»Р°РІРёР°С‚СѓСЂСѓ СЃРѕСЃРµРґРЅРёС… РІС…РѕРґРѕРІ РђР¦Рџ


#endif /* ATMEGA_CTLSTYLE_V8S_UR3VBM_INCLUDED */
