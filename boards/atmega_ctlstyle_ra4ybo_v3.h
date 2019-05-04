/* $Id$ */
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//
// Р°РїРїР°СЂР°С‚СѓС‚Р° С‚СЂР°РЅСЃРёРІРµСЂР° RA4YBO V3
// "РЎС‚Р°С†РёРѕРЅР°СЂРЅС‹Р№ СЃ РџР°РІР»РёРЅРѕРј" STM32F103C8T6, 2*HMC830, up-conversion to 64455 РґР»СЏ RA4YBO
//

#ifndef ATMEGA_CTLSTYLE_RA4YBO_V3_H_INCLUDED
#define ATMEGA_CTLSTYLE_RA4YBO_V3_H_INCLUDED 1

	#define WITHUSEPLL		1	/* Р“Р»Р°РІРЅР°СЏ PLL	*/

	// STM32F103C8T6 Medium-density performance line
	#if WITHCPUXTAL
		#define	REFINFREQ WITHCPUXTAL
		#define REF1_DIV 1
		#define REF1_MUL 9	// Up to 16 supported
	#else
		#define	REFINFREQ 8000000UL
		#define REF1_DIV 2
		#define REF1_MUL 8	// Up to 16 supported
	#endif

	/* РјРѕРґРµР»Рё СЃРёРЅС‚РµР·Р°С‚РѕСЂРѕРІ - СЃС…РµРјС‹ С‡Р°СЃС‚РѕС‚РѕРѕР±СЂР°Р·РѕРІР°РЅРёСЏ */

	// --- РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ СЂР°Р·РЅС‹С… РїР»Р°С‚
	//#define CTLREGMODE_RA4YBO_V3	1
	#define CTLREGMODE_RA4YBO_V3A	1

	#define WITHPREAMPATT2_10DB		1	// РЈРїСЂР°РІР»РµРЅРёРµ РЈР’Р§ Рё РґРІСѓС…РєР°СЃРєР°РґРЅС‹Рј Р°С‚С‚РµРЅСЋР°С‚РѕСЂРѕРј

	#define	BOARD_AGCCODE_0		0x00
	#define	BOARD_AGCCODE_1		0x01
	#define	BOARD_AGCCODE_2		0x02
	#define	BOARD_AGCCODE_3		0x03

	#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_0
	#define	BOARD_AGCCODE_MED	BOARD_AGCCODE_1
	#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_2
	#define	BOARD_AGCCODE_LONG	BOARD_AGCCODE_3
	#define	BOARD_AGCCODE_OFF	0xFF

	#define WITHAGCMODE4STAGES	1	// 4 СЃРєРѕСЂРѕСЃС‚Рё. РІС‹РєР»СЋС‡РµРЅРЅРѕ РЅРµ Р±С‹РІР°РµС‚

	#define	FONTSTYLE_ITALIC	1	//
	#define DSTYLE_UR3LMZMOD	1


	// +++ РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ РїР»Р°С‚ РЅР° ATMega
	// --- РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ РїР»Р°С‚ РЅР° ATMega

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
	//#define LCDMODE_ILI9163	1	/* РРЅРґРёРєР°С‚РѕСЂ LPH9157-2 176*132 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILITEK ILI9163 - Р›РµРЅС‚Р° РґРёСЃРїР»РµСЏ СЃРїСЂР°РІР°, Р° РІС‹РІРѕРґС‹ СЃР»РµРІР°. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРїСЂР°РІР°, Р»РµРЅС‚Р° РґРёСЃРїР»РµСЏ СЃР»РµРІР°) */
	//#define LCDMODE_L2F50	1	/* РРЅРґРёРєР°С‚РѕСЂ 176*132 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Epson L2F50126 */
	//#define LCDMODE_L2F50_TOPDOWN	1	/* РџРµСЂРµРІРѕСЂРѕС‚ РёР·РѕР±СЂР°Р¶РµРЅРё СЏ РІ СЃР»СѓС‡Р°Рµ LCDMODE_L2F50 */
	#define LCDMODE_S1D13781	1	/* РРЅРЅРґРёРєР°С‚РѕСЂ 480*272 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Epson S1D13781 */
	#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	//#define LCDMODE_ILI9225	1	/* РРЅРґРёРєР°С‚РѕСЂ 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9225РЎ */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃР»РµРІР° РѕС‚ СЌРєСЂР°РЅР°) */
	//#define LCDMODE_UC1608	1		/* РРЅРґРёРєР°С‚РѕСЂ 240*128 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1608.- РјРѕРЅРѕС…СЂРѕРјРЅС‹Р№ */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРІРµСЂС…Сѓ) */
	//#define LCDMODE_ST7735	1	/* РРЅРґРёРєР°С‚РѕСЂ 160*128 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Sitronix ST7735 */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРїСЂР°РІР°) */
	//#define LCDMODE_ST7565S	1	/* РРЅРґРёРєР°С‚РѕСЂ WO12864C2-TFH# 128*64 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРІРµСЂС…Сѓ) */
	//#define LCDMODE_ILI9320	1	/* РРЅРґРёРєР°С‚РѕСЂ 248*320 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9320 */
	// --- РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_100
	//#define ENCRES_DEFAULT ENCRES_24
	#define	WITHENCODER	1	/* РґР»СЏ РёР·РјРµРЅРµРЅРёСЏ С‡Р°СЃС‚РѕС‚С‹ РёРјРµРµС‚СЃСЏ РµРЅРєРѕРґРµСЂ */
	#define ENCODER_REVERSE	1	/* С€Р°РіР°С‚СЊ РІ РѕР±СЂС‚РЅСѓСЋ СЃС‚РѕСЂРѕРЅСѓ */

	// +++ Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ
	#define WITHTX		1	/* РІРєР»СЋС‡РµРЅРѕ СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - СЃРёРєРІРµРЅСЃРѕСЂ, СЌР»РµРєС‚СЂРѕРЅРЅС‹Р№ РєР»СЋС‡. */
	//#define WITHCAT		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ CAT */
	//#define WITHDEBUG		1	/* РћС‚Р»Р°РґРѕС‡РЅР°СЏ РїРµС‡Р°С‚СЊ С‡РµСЂРµР· COM-РїРѕСЂС‚. Р‘РµР· CAT (WITHCAT) */
	#define WITHVOX		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ VOX */
	#define WITHBARS	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ S-РјРµС‚СЂР° Рё SWR-РјРµС‚СЂР° */
	#define WITHSHOWSWRPWR 1	/* РЅР° РґРёСЃРїР»РµРµ РѕРґРЅРѕРІСЂРµРјРµРЅРЅРѕ РѕС‚РѕР±СЂР°Р¶Р°СЋС‚СЃСЏ SWR-meter Рё PWR-meter - РўСЂРµР±СѓРµС‚ WITHSWRMTR */
	#define WITHSWRMTR	1	/* РР·РјРµСЂРёС‚РµР»СЊ РљРЎР’ РёР»Рё */
	//#define WITHPWRMTR	1	/* РРЅРґРёРєР°С‚РѕСЂ РІС‹С…РѕРґРЅРѕР№ РјРѕС‰РЅРѕСЃС‚Рё  */
	//#define WITHVOLTLEVEL	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ РЅР°РїСЂСЏР¶РµРЅРёСЏ РђРљР‘ */
	//#define WITHSWLMODE	1	/* РїРѕРґРґРµСЂР¶РєР° Р·Р°РїРѕРјРёРЅР°РЅРёСЏ РјРЅРѕР¶РµСЃС‚РІР° С‡Р°СЃС‚РѕС‚ РІ swl-mode */
	//#define WITHPBT		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ PBT */
	#define WITHIFSHIFT	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* РќР°С‡Р°Р»СЊРЅРѕРµ Р·Р°РЅС‡РµРЅРёРµ IF SHIFT */
	#define WITHMODESETFULLNFM 1
	#define WITHWFM	1			/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ WFM */
	#define WITHCURRLEVEL	1

	#define WITHMENU 	1	/* С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РјРµРЅСЋ РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕС‚РєР»СЋС‡РµРЅР° - РµСЃР»Рё РЅР°СЃС‚СЂР°РёРІР°С‚СЊ РЅРµС‡РµРіРѕ */
	#define WITHVIBROPLEX	1	/* РІРѕР·РјРѕР¶РЅРѕСЃС‚СЊ СЌРјСѓР»СЏС†РёРё РїРµСЂРµРґР°С‡Рё РІРёР±СЂРѕРїР»РµРєСЃРѕРј */

	#define	WITHAUTOTUNER	1	/* Р•СЃС‚СЊ С„СѓРЅРєС†РёСЏ Р°РІС‚РѕС‚СЋРЅРµСЂР° */
	//#define FULLSET8 1
	#define SHORTSET8 1
	#define WITHSLEEPTIMER	1	/* РІС‹РєР»СЋС‡РёС‚СЊ РёРЅРґРёРєР°С‚РѕСЂ Рё РІС‹РІРѕРґ Р·РІСѓРєР° РїРѕ РёСЃС‚РµС‡РµРЅРёРёРё СѓРєР°Р·Р°РЅРЅРѕРіРѕ РІСЂРµРјРµРЅРё */
	// --- Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ

	//#define LO1PHASES	1		/* РџСЂСЏРјРѕР№ СЃРёРЅС‚РµР· РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° РґРІСѓРјСЏ DDS СЃ РїСЂРѕРіСЂР°РјРёРјСЂСѓРµРјС‹Рј СЃРґРІРёРіРѕРј С„Р°Р·С‹ */
	#define DEFPREAMPSTATE 	1	/* РЈР’Р§ РїРѕ СѓРјРѕР»С‡Р°РЅРёСЋ РІРєР»СЋС‡С‘РЅ (1) РёР»Рё РІС‹РєР»СЋС‡РµРЅ (0) */
	
	/* С‡С‚Рѕ Р·Р° РїР°РјСЏС‚СЊ РЅР°СЃС‚СЂРѕРµРє Рё С‡Р°СЃС‚РѕС‚ РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІ РєРѕРЅС‚СЂРѕР»Р»РµСЂРµ */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// РўР°Рє Р¶Рµ РїСЂРё РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРё FM25040A - 5 РІРѕР»СЊС‚, 512 Р±Р°Р№С‚
	#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section

	//#define FTW_RESOLUTION 28	/* СЂР°Р·СЂСЏРґРЅРѕСЃС‚СЊ FTW РІС‹Р±СЂР°РЅРЅРѕРіРѕ DDS */
	#define FTW_RESOLUTION 32	/* СЂР°Р·СЂСЏРґРЅРѕСЃС‚СЊ FTW РІС‹Р±СЂР°РЅРЅРѕРіРѕ DDS */
	//#define FTW_RESOLUTION 31	/* СЂР°Р·СЂСЏРґРЅРѕСЃС‚СЊ FTW РІС‹Р±СЂР°РЅРЅРѕРіРѕ DDS - ATINY2313 */

	#define LO1MODE_DIRECT		1

#if CTLREGMODE_RA4YBO_V3

	#define PLL1_TYPE PLL_TYPE_HMC830
	#define PLL1_FRACTIONAL_LENGTH	24	/* РєРѕР»РёС‡РµСЃС‚РІРѕ Р±РёС‚ РІ РґСЂРѕР±РЅРѕР№ С‡Р°СЃС‚Рё РґРµР»РёС‚РµР»СЏ */
	#define PLL2_TYPE PLL_TYPE_LMX2306
	//#define PLL2_TYPE PLL_TYPE_HMC830
	//#define PLL2_FRACTIONAL_LENGTH	24	/* РєРѕР»РёС‡РµСЃС‚РІРѕ Р±РёС‚ РІ РґСЂРѕР±РЅРѕР№ С‡Р°СЃС‚Рё РґРµР»РёС‚РµР»СЏ */

	#define REFERENCE_FREQ	50000000L
	#define	SYNTH_R1		1


#elif CTLREGMODE_RA4YBO_V3A

	#define PLL1_TYPE PLL_TYPE_SI570
	#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	#define DDS2_TYPE DDS_TYPE_AD9834
	#define PLL2_TYPE PLL_TYPE_LMX2306	/* Р”РµР»РёС‚РµР»СЊ РѕРєС‚Р°РІРЅРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° РґР»СЏ РїРѕР»СѓС‡РµРЅРёСЏ СЃРєРІРѕР·РЅРѕРіРѕ РґРёР°РїР°Р·РѕРЅР° */
	#define REFERENCE_FREQ	50000000UL	/* LO2 = 72595 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */


#if 1
	// Р§Р°СЃС‚РѕС‚Р° СЃСЂР°РІРЅРµРЅРёСЏ РѕРєРѕР»Рѕ 25 РєР“С†
	// Р’РµСЂСЃРёСЏ РїРѕРґ С„РёР»СЊС‚СЂ 1-Р№ РџР§ 80.455
	// IF2=500 kHz
	// LO2=79995000
	#define LO2_PLL_N500	3203	// 79,955,067 Hz 
	#define LO2_PLL_R500	2003
	// IF2=455 kHz
	// LO2=80000000
	#define LO2_PLL_N455	3200
	#define LO2_PLL_R455	2000
#else
	// Р§Р°СЃС‚РѕС‚Р° СЃСЂР°РІРЅРµРЅРёСЏ РѕРєРѕР»Рѕ 50 РєР“С†
	// Р’РµСЂСЃРёСЏ РїРѕРґ С„РёР»СЊС‚СЂ 1-Р№ РџР§ 80.455
	// IF2=500 kHz
	#define LO2_PLL_N500	1593
	#define LO2_PLL_R500	998
	// IF2=455 kHz
	#define LO2_PLL_N455	1600
	#define LO2_PLL_R455	1000
#endif
	/* Р±РёС‚С‹ РІ СЂРµРіРёСЃС‚СЂР°С… СѓРїСЂР°РІР»РµРЅРёСЏ ADG714 */
	#define ADG714_OFF	0x00u	/* РІСЃРµ РІС‹РєР»СЋС‡РµРЅС‹ */
	#define ADG714_S1D1 (0x01u << 0)
	#define ADG714_S2D2 (0x01u << 1)
	#define ADG714_S3D3 (0x01u << 2)
	#define ADG714_S4D4 (0x01u << 3)
	#define ADG714_S5D5 (0x01u << 4)
	#define ADG714_S6D6 (0x01u << 5)
	#define ADG714_S7D7 (0x01u << 6)
	#define ADG714_S8D8 (0x01u << 7)

	#define MAKEFLTCODE(in, out) ((in) * 256 + (out))	// in: РљРѕРјРјСѓС‚Р°С‚РѕСЂ IC1, out: РєРѕРјРјСѓС‚Р°С‚РѕСЂ IC2

	#define BOARD_FILTER_RX_WFM		MAKEFLTCODE(ADG714_OFF, ADG714_OFF)

	#define BOARD_FILTER500_RX_0P5	MAKEFLTCODE(ADG714_S1D1, ADG714_S1D1)		// emf-500-0.6-S

	#define BOARD_FILTER500_RX_3P1	MAKEFLTCODE(ADG714_S3D3, ADG714_S3D3)		// emf-500-3.1-N
	#define BOARD_FILTER500_TX_3P1	MAKEFLTCODE(ADG714_S4D4, ADG714_S4D4)		// emf-500-3.1-N

	#define BOARD_FILTER455_RX_3P0	MAKEFLTCODE(ADG714_S2D2, ADG714_S2D2)		// collins

	#define BOARD_FILTER455_RX_10P0	MAKEFLTCODE(ADG714_S6D6, ADG714_S6D6)		// piezo
	#define BOARD_FILTER455_TX_10P0	MAKEFLTCODE(ADG714_S5D5, ADG714_S5D5)		// piezo

	#define BOARD_FILTER455_RX_6P0	MAKEFLTCODE(ADG714_S7D7, ADG714_S7D7)		// piezo

	#define BOARD_FILTER455_RX_2P1	MAKEFLTCODE(ADG714_S8D8, ADG714_S8D8)		// piezo


	#define BOARD_FILTER_OFF MAKEFLTCODE(ADG714_OFF, ADG714_OFF)

#endif

	#define DDS1_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 */
	#define DDS1_CLK_MUL	1		/* РЈРјРЅРѕР¶РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 */
	#define DDS2_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 */
	#define DDS2_CLK_MUL	1		/* РЈРјРЅРѕР¶РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 */
	#define DDS3_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 */
	#define DDS3_CLK_MUL	1		/* РЈРјРЅРѕР¶РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 */

	//#define DDS1_TYPE DDS_TYPE_AD9834

	//#define WITHFIXEDBFO	1	/* РџРµСЂРµРІРѕСЂРѕС‚ Р±РѕРєРѕРІС‹С… Р·Р° СЃС‡С‘С‚ 1-РіРѕ РіРµС‚СЂРѕРґРёРЅР° (РѕСЃРѕР±РµРЅРЅРѕ, РµСЃР»Рё РЅРµС‚ РїРѕРґСЃС‚СЂР°РёРІР°РµРјРѕРіРѕ BFO) */
	//#define WITHDUALFLTR	1	/* РџРµСЂРµРІРѕСЂРѕС‚ Р±РѕРєРѕРІС‹С… Р·Р° СЃС‡С‘С‚ РїРµСЂРµРєР»СЋС‡РµРЅРёСЏ С„РёР»СЊС‚СЂР° РІРµСЂС…РЅРµР№ РёР»Рё РЅРёР¶РЅРµР№ Р±РѕРєРѕРІРѕР№ РїРѕР»РѕСЃС‹ */
	#define WITHSAMEBFO	1		/* РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РѕР±С‰РёС… РЅР°СЃС‚СЂРѕРµРє BFO РґР»СЏ РїСЂРёС‘РјР° Рё РїРµСЂРµРґР°С‡Рё */
	#define DDS2_TYPE DDS_TYPE_AD9834


	#define RTC1_TYPE RTC_TYPE_M41T81	/* ST M41T81M6 RTC clock chip with I2C interface */

	/* РќР°Р·РЅР°С‡РµРЅРёРµ Р°РґСЂРµСЃРѕРІ РЅР° SPI С€РёРЅРµ */
	#define targetdds2 SPI_CSEL_PA9 	/* DDS2 - LO3 output */
	#define targetpll1 SPI_CSEL_PA11 	/* PLL1 - HMC830 */ 
	#define targetpll2 SPI_CSEL_PA10 	/* PLL2 - HMC830 */ 
	#define targetctl1 SPI_CSEL_PA8 	/* control register */
	#define targetnvram SPI_CSEL_PA15  	/* serial nvram */
	#define targetlcd SPI_CSEL_PA12 	/* LCD over SPI line devices control */ 
	

	/* РєРѕРґС‹ РІС…РѕРґРѕРІ РєРѕРјРјСѓС‚Р°С‚РѕСЂР° РёСЃС‚РѕС‡РЅРёРєРѕРІ СЃРёРіРЅР°Р»Р° РґР»СЏ РЈРќР§ РїСЂРёС‘РјРЅРёРєР° */
	#define BOARD_DETECTOR_SSB 0x03
	#define BOARD_DETECTOR_AM 0x02
	#define BOARD_DETECTOR_FM 0x01
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x01	/* РєРѕРЅС„РёРіСѓСЂР°С†РёСЏ РїР»Р°С‚С‹ РґР»СЏ СЂРµР¶РёРјР° TUNE (CWZ РЅР° РїРµСЂРµРґР°С‡Сѓ) */

	#define BOARD_DETECTOR_MUTE 4

	//#define IF3_FMASK (IF3_FMASK_17P0 | IF3_FMASK_7P8 | IF3_FMASK_3P1 | IF3_FMASK_2P4 | IF3_FMASK_1P5 | IF3_FMASK_0P5 | IF3_FMASK_0P3)
	#define IF3_FMASK (IF3_FMASK_3P1 | IF3_FMASK_0P5 | IF3_FMASK_6P0)
	#define IF3_FHAVE IF3_FMASK
	#define IF3_FMASKTX	(IF3_FMASK_6P0)

	#define WITHSPLIT	1	/* СѓРїСЂР°РІР»РµРЅРёРµ СЂРµР¶РёРјР°РјРё СЂР°СЃСЃС‚СЂРѕР№РєРё РѕРґРЅРѕР№ РєРЅРѕРїРєРѕР№ */
	//#define WITHSPLITEX	1	/* РўСЂРµС…РєРЅРѕРїРѕС‡РЅРѕРµ СѓРїСЂР°РІР»РµРЅРёРµ СЂРµР¶РёРјР°РјРё СЂР°СЃСЃС‚СЂРѕР№РєРё */
	#define WITHCATEXT	1	/* Р Р°СЃС€РёСЂРµРЅРЅС‹Р№ РЅР°Р±РѕСЂ РєРѕРјР°РЅРґ CAT */
	#define WITHELKEY	1
	#define WITHKBDENCODER 1	// РїРµСЂРµСЃС‚СЂРѕР№РєР° С‡Р°СЃС‚РѕС‚С‹ РєРЅРѕРїРєР°РјРё
	#define WITHKEYBOARD 1	/* РІ РґР°РЅРЅРѕРј СѓСЃС‚СЂРѕР№СЃС‚РІРµ РµСЃС‚СЊ РєР»Р°РІРёР°С‚СѓСЂР° */
	#define KEYBOARD_USE_ADC	1	/* РЅР° РѕРґРЅРѕР№ Р»РёРЅРёРё СѓСЃС‚Р°РЅРѕРІР»РµРЅРѕ  С‡РµС‚С‹СЂРµ  РєР»Р°РІРёС€Рё. РЅР° vref - 6.8K, РґР°Р»РµРµ 2.2Рљ, 4.7Рљ Рё 13K. */
	#define KEYBOARD_USE_ADC6_V1	1
	//#define KEYBOARD_USE_ADC6	1
	#define WITHDIRECTFREQENER	1	/* РїСЂСЏРјРѕР№ РІРІРѕРґ С‡Р°СЃС‚РѕС‚С‹ СЃ РєР»Р°РІРёР°С‚СѓСЂС‹ */

	#define WITHSPKMUTE		1	/* СѓРїСЂР°РІР»РµРЅРёРµ РІС‹РєР»СЋС‡РµРЅРёРµРј РґРёРЅР°РјРёРєР° */
	#define WITHANTSELECT	1
	// +++ РћСЃРѕР±С‹Рµ РІР°СЂРёР°РЅС‚С‹ СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ РєРЅРѕРїРѕРє РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ
	#define KEYB_RA4YBO_V3	1	/*  */
	// --- РћСЃРѕР±С‹Рµ РІР°СЂРёР°РЅС‚С‹ СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ РєРЅРѕРїРѕРє РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ
	// РќР°Р·РЅР°С‡РµРЅРёСЏ РІС…РѕРґРѕРІ РђР¦Рџ РїСЂРѕС†РµСЃСЃРѕСЂР°.
	enum 
	{ 
	#if WITHBARS
		SMETERIX = 0,	// S-meter
	#endif
	#if WITHVOX
		VOXIX = 1, AVOXIX = 2,	// VOX
	#endif
	#if WITHSWRMTR
		FWD = 4, REF = 3,	// SWR-meter
		PWRI = 4, 
	#endif
	#if WITHCURRLEVEL
		PASENSEIX = 5,		// PA1 PA current sense - ACS712-30 chip
	#endif /* WITHCURRLEVEL */
		KI0 = 6, KI1 = 7, KI2 = 8, KI3 = 9	// РєР»Р°РІРёР°С‚СѓСЂР°
	};
	#define KI_COUNT 4	// РєРѕР»РёС‡РµСЃС‚РІРѕ РёСЃРїРѕР»СЊР·СѓРµРјС‹С… РїРѕРґ РєР»Р°РІРёР°С‚СѓСЂСѓ РІС…РѕРґРѕРІ РђР¦Рџ


	#define VOLTLEVEL_UPPER		47	// 4.7 kOhm - РІРµСЂС…РЅРёР№ СЂРµР·РёСЃС‚РѕСЂ РґРµР»РёС‚РµР»СЏ РґР°С‚С‡РёРєР° РЅР°РїСЂСЏР¶РµРЅРёСЏ
	#define VOLTLEVEL_LOWER		10	// 1.0 kOhm - РЅРёР¶РЅРёР№ СЂРµР·РёСЃС‚РѕСЂ


	/* РџРµСЂРІР°СЏ РџР§ - 64455, РІС‚РѕСЂР°СЏ - 500/455 РєР“С† */

	/* РћРїРёСЃР°РЅРёРµ СЃС‚СЂСѓРєС‚СѓСЂС‹ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёР№ С‡Р°СЃС‚РѕС‚С‹ РІ С‚СЂР°РєС‚Рµ */
	#define LO0_SIDE	LOCODE_INVALID	/* РїСЂРё РѕС‚СЃСѓС‚СЃС‚РІСѓСЋС‰РµРј РєРѕРЅРІРµСЂС‚РѕСЂРµ - РЅР° РЅС‘Рј РЅРµС‚ РёРЅРІРµСЂСЃРёРё СЃРїРµРєС‚СЂР° */
	//#define LO1_SIDE	LOCODE_UPPER	/* РџСЂРё РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРё РЅР° СЌС‚РѕРј РіРµС‚РµСЂРѕРґРёРЅРµ РїСЂРѕРёСЃС…РѕРґРёС‚ РёРЅРІРµСЂСЃРёСЏ СЃРїРµРєС‚СЂР° */
	#define LO1_SIDE_F(freq) (((freq) < 64455000UL) ? LOCODE_UPPER : LOCODE_LOWER)	/* СЃС‚РѕСЂРѕРЅР° Р·Р°РІРёСЃРёС‚ РѕС‚ СЂР°Р±РѕС‡РµР№ С‡Р°СЃС‚РѕС‚С‹ */
	#define LO2_SIDE	LOCODE_LOWER	/* РџСЂРё РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРё РЅР° СЌС‚РѕРј РіРµС‚РµСЂРѕРґРёРЅРµ РЅРµС‚ РёРЅРІРµСЂСЃРёРё СЃРїРµРєС‚СЂР° */
	#define LO3_SIDE	LOCODE_INVALID	/* РїСЂРё РѕС‚СЃСѓС‚СЃС‚РІСѓСЋС‰РµРј РіРµС‚РµСЂРѕРґРёРЅРµ - РЅР° РЅС‘Рј РЅРµС‚ РёРЅРІРµСЂСЃРёРё СЃРїРµРєС‚СЂР° */
	#if WITHIF4DSP
		// Р°СЂС…РёС‚РµРєС‚СѓСЂР° РґР»СЏ DSP РЅР° РїРѕСЃР»РµРґРЅРµР№ РџР§
		#define LO4_SIDE	LOCODE_LOWER	/* РїР°СЂР°РјРµС‚СЂ РїСЂРѕРІРµСЂРµРЅ РїРѕ РѕС‚СЃСѓС‚СЃС‚РІРёСЋ РїРѕРјРµС…Рё РїСЂРё РїСЂРёС‘РјРµ РґР»СЏ FQMODEL_80455. РџСЂРё РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРё РЅР° СЌС‚РѕРј РіРµС‚РµСЂРѕРґРёРЅРµ РЅРµС‚ РёРЅРІРµСЂСЃРёРё СЃРїРµРєС‚СЂР° */
		#define LO5_SIDE	LOCODE_LOWER	/* РџСЂРё РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРё РЅР° СЌС‚РѕРј РіРµС‚РµСЂРѕРґРёРЅРµ РЅРµС‚ РёРЅРІРµСЂСЃРёРё СЃРїРµРєС‚СЂР° */
	#else /* WITHIF4DSP */
		// РѕР±С‹С‡РЅР°СЏ Р°СЂС…РёС‚РµРєС‚СѓСЂР° РґР»СЏ СЃР»СѓС…РѕРІРѕРіРѕ РїСЂРёС‘РјРІ
		#define LO4_SIDE	LOCODE_TARGETED	/* Р­С‚РѕС‚ РіРµС‚РµСЂРѕРґРёРЅ СѓРїСЂР°РІР»СЏРµС‚СЃСЏ РґР»СЏ РїРѕР»СѓС‡РµРЅРёСЏ С‚СЂРµР±СѓРµРјРѕР№ Р±РѕРєРѕРІРѕР№ */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: РїСЂРё РѕС‚СЃСѓС‚СЃС‚РІСѓСЋС‰РµРј РіРµС‚РµСЂРѕРґРёРЅРµ - РЅР° РЅС‘Рј РЅРµС‚ РёРЅРІРµСЂСЃРёРё СЃРїРµРєС‚СЂР° */
	#endif /* WITHIF4DSP */

	//#define	LO3_FREQADJ	1	/* РїРѕРґСЃС‚СЂРѕР№РєР° С‡Р°СЃС‚РѕС‚С‹ РіРµС‚РµСЂРѕРґРёРЅР° С‡РµСЂРµР· РјРµРЅСЋ. */

	#define	LO2_POWER2	0		/* 0 - РЅРµС‚ РґРµР»РёС‚РµР»СЏ РїРѕСЃР»Рµ РіРµРЅРµСЂР°С‚РѕСЂР° LO2 РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РЅР° СЃРјРµСЃРёС‚РµР»СЊ */

	#define DDS1_CLK_MUL	1 		/* РЈРјРЅРѕР¶РёС‚РµР»СЊ РІ DDS1 - "Р·Р°С‚С‹С‡РєР°" */
	#define DDS2_CLK_MUL	1		/* РЈРјРЅРѕР¶РёС‚РµР»СЊ РІ DDS2 - AD9834 */

	#define	WFM_IF1	10700000ul

	#define LO1_POWER2	0		/* РµСЃР»Рё 0 - РґРµР»РёС‚РµР»РµР№ РІ С‚СЂР°РєС‚Рµ РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° РїРµСЂРµРґ СЃРјРµСЃРёС‚РµР»РµРј РЅРµС‚. */
	#define LO4_POWER2 0

	#define TUNE_BOTTOM 30000L		/* РЅРёР¶РЅСЏСЏ С‡Р°СЃС‚РѕС‚Р° РЅР°СЃС‚СЂРѕР№РєРё */
	#define TUNE_TOP 180000000L		/* РІРµСЂС…РЅСЏСЏ С‡Р°СЃС‚РѕС‚Р° РЅР°СЃС‚СЂРѕР№РєРё */


#endif /* ATMEGA_CTLSTYLE_RA4YBO_V3_H_INCLUDED */
