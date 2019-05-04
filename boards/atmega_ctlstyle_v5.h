/* $Id$ */
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//

// РњР°Р»РµРЅСЊРєР°СЏ РїР»Р°С‚Р° РєРѕРЅС‚СЂРѕР»Р»РµСЂР° СЃ РґРІСѓРјСЏ AD9834 Рё 4 Р“РЈРќ
// Р‘РµР· SPI_BIDIRECTIONAL Рё СЃ РІРѕР·РјРѕР¶РЅРѕР№ РіРµРЅРµСЂР°Р№РёРµР№ Р·РІСѓРєРѕРІ.

#ifndef ATMEGA_CTLSTYLE_V5_H_INCLUDED
#define ATMEGA_CTLSTYLE_V5_H_INCLUDED 1
	/* РјРѕРґРµР»Рё СЃРёРЅС‚РµР·Р°С‚РѕСЂРѕРІ - СЃС…РµРјС‹ С‡Р°СЃС‚РѕС‚РѕРѕР±СЂР°Р·РѕРІР°РЅРёСЏ */

#if 0
	/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РЅР°РІРµСЂС…" */
	//#define FQMODEL_73050		1	// 1-st IF=73.050, 2-nd IF=0.455 MHz
	#define FQMODEL_73050_IF0P5		1	// 1-st IF=73.050, 2-nd IF=0.5 MHz
	//#define FQMODEL_80455		1	// 1-st IF=80.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455		1	// 1-st IF=64.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455_IF0P5		1	// 1-st IF=64.455, 2-nd IF=0.5 MHz
	//#define FQMODEL_70455		1	// 1-st IF=70.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	#define BANDSELSTYLERE_UPCONV56M	1	/* Up-conversion with working band .030..56 MHz */

#else
	/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РЅР°РІРµСЂС…" */
	//#define FQMODEL_45_IF8868	1	// SW2011
	#define FQMODEL_45_IF0P5	1	// 1-st IF=45MHz, 2-nd IF=500 kHz
	//#define FQMODEL_45_IF455	1	// 1-st IF=45MHz, 2-nd IF=455 kHz
	#define BANDSELSTYLERE_UPCONV36M	1	/* Up-conversion with working band 0.1 MHz..36 MHz */
	//#define BANDSELSTYLERE_UPCONV32M	1	/* Up-conversion with working band 0.1 MHz..32 MHz */
#endif

	#define	FONTSTYLE_ITALIC	1	//
	// --- РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ СЂР°Р·РЅС‹С… РїР»Р°С‚

	// +++ РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ РїР»Р°С‚ РЅР° ATMega
	// --- РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ РїР»Р°С‚ РЅР° ATMega

	// +++ РћСЃРѕР±С‹Рµ РІР°СЂРёР°РЅС‚С‹ СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ РєРЅРѕРїРѕРє РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ
	//#define KEYB_RA1AGG	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ РєРѕСЂРїСѓСЃР°, СЃРґРµР»Р°РЅРЅРѕРіРѕ RA1AGG - С‚СЂРё РіСЂСѓРїРїС‹ РїРѕ С‡РµС‚С‹СЂРµ Р»РёРЅРµР№РєРё РєРЅРѕРїРѕРє РґСЂСѓРі Р·Р° РґСЂСѓРіРѕРј РІ РѕРґРёРЅ СЂСЏРґ. */
	//#define KEYB_UA1CEI	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ UA1CEI */
	#define KEYB_7BUTTONSOLD	1
	// --- РћСЃРѕР±С‹Рµ РІР°СЂРёР°РЅС‚С‹ СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ РєРЅРѕРїРѕРє РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ

	// +++ РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	#define LCDMODE_WH1602	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 16*2 */
	//#define LCDMODE_WH2002	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 20*2, РІРѕР·РјРѕР¶РЅРѕ РІРјРµСЃС‚Рµ СЃ LCDMODE_HARD_SPI */
	//#define LCDMODE_WH2004	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 20*4 */
	//#define LCDMODE_RDX0154	1	/* РРЅРґРёРєР°С‚РѕСЂ 132*64 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1601.  */
	//#define LCDMODE_UC1601S_TOPDOWN	1	/* LCDMODE_RDX0154 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	//#define LCDMODE_UC1601S_EXTPOWER	1	/* LCDMODE_RDX0154 - Р±РµР· РІРЅСѓС‚СЂРµРЅРЅРµРіРѕ РїСЂРµРѕР±СЂР°Р·РѕРІР°С‚РµР»СЏ */
	//#define LCDMODE_RDX0120	1	/* РРЅРґРёРєР°С‚РѕСЂ 64*32 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1601.  */
	//#define LCDMODE_TIC218	1	/* РРЅРґРёРєР°С‚РѕСЂ 133*65 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј PCF8535 */
	//#define LCDMODE_TIC154	1	/* РРЅРґРёРєР°С‚РѕСЂ 133*65 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј PCF8535 */
	//#define LCDMODE_LS020 	1	/* РРЅРґРёРєР°С‚РѕСЂ 176*132 Sharp LS020B8UD06 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј LR38826 */
	//#define LCDMODE_LPH88		1	/* РРЅРґРёРєР°С‚РѕСЂ 176*132 LPH8836-2 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Hitachi HD66773 */
	// --- РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define	WITHENCODER	1	/* РґР»СЏ РёР·РјРµРЅРµРЅРёСЏ С‡Р°СЃС‚РѕС‚С‹ РёРјРµРµС‚СЃСЏ РµРЅРєРѕРґРµСЂ */

	// +++ Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ
	#define WITHTX		1	/* РІРєР»СЋС‡РµРЅРѕ СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - СЃРёРєРІРµРЅСЃРѕСЂ, СЌР»РµРєС‚СЂРѕРЅРЅС‹Р№ РєР»СЋС‡. */
	//#define WITHCAT		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ CAT */
	//#define WITHPBT		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ PBT */
	#define WITHIFSHIFT	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* РќР°С‡Р°Р»СЊРЅРѕРµ Р·Р°РЅС‡РµРЅРёРµ IF SHIFT */
	//#define WITHVOX		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ VOX */
	//#define WITHSWRMTR	1	/* РР·РјРµСЂРёС‚РµР»СЊ РљРЎР’ */
	//#define WITHPWRMTR	1	/* РРЅРґРёРєР°С‚РѕСЂ РІС‹С…РѕРґРЅРѕР№ РјРѕС‰РЅРѕСЃС‚Рё РёР»Рё */
	#define WITHBARS	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ S-РјРµС‚СЂР° Рё SWR-РјРµС‚СЂР° */
	//#define WITHVOLTLEVEL	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ РЅР°РїСЂСЏР¶РµРЅРёСЏ РђРљР‘ */
	//#define WITHSWLMODE	1	/* РїРѕРґРґРµСЂР¶РєР° Р·Р°РїРѕРјРёРЅР°РЅРёСЏ РјРЅРѕР¶РµСЃС‚РІР° С‡Р°СЃС‚РѕС‚ РІ swl-mode */

	#define WITHMENU 	1	/* С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РјРµРЅСЋ РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕС‚РєР»СЋС‡РµРЅР° - РµСЃР»Рё РЅР°СЃС‚СЂР°РёРІР°С‚СЊ РЅРµС‡РµРіРѕ */
	//#define WITHDEBUG		1	/* РћС‚Р»Р°РґРѕС‡РЅР°СЏ РїРµС‡Р°С‚СЊ С‡РµСЂРµР· COM-РїРѕСЂС‚. Р‘РµР· CAT (WITHCAT) */
	#define WITHVIBROPLEX	1	/* РІРѕР·РјРѕР¶РЅРѕСЃС‚СЊ СЌРјСѓР»СЏС†РёРё РїРµСЂРµРґР°С‡Рё РІРёР±СЂРѕРїР»РµРєСЃРѕРј */
	//#define WITHONLYBANDS 1		/* РџРµСЂРµСЃС‚СЂРѕР№РєР° РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕРіСЂР°РЅРёС‡РµРЅР° Р»СЋР±РёС‚РµР»СЊСЃРєРёРјРё РґРёР°РїР°Р·РѕРЅР°РјРё */
	//#define WITHBCBANDS	1		/* РІ С‚Р°Р±Р»РёС†Рµ РґРёР°РїР°Р·РѕРЅРѕРІ РїСЂРёСЃСѓС‚СЃС‚РІСѓСЋС‚ РІРµС‰Р°С‚РµР»СЊРЅС‹Рµ РґРёР°РїР°Р·РѕРЅС‹ */
	#define WITHWARCBANDS	1	/* Р’ С‚Р°Р±Р»РёС†Рµ РґРёР°РїР°Р·РѕРЅРѕРІ РїСЂРёСЃСѓС‚СЃС‚РІСѓСЋС‚ HF WARC РґРёР°РїР°Р·РѕРЅС‹ */
	#define WITHSLEEPTIMER	1	/* РІС‹РєР»СЋС‡РёС‚СЊ РёРЅРґРёРєР°С‚РѕСЂ Рё РІС‹РІРѕРґ Р·РІСѓРєР° РїРѕ РёСЃС‚РµС‡РµРЅРёРёРё СѓРєР°Р·Р°РЅРЅРѕРіРѕ РІСЂРµРјРµРЅРё */
	// --- Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ

	//#define LO1PHASES	1		/* РџСЂСЏРјРѕР№ СЃРёРЅС‚РµР· РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° РґРІСѓРјСЏ DDS СЃ РїСЂРѕРіСЂР°РјРёРјСЂСѓРµРјС‹Рј СЃРґРІРёРіРѕРј С„Р°Р·С‹ */
	#define DEFPREAMPSTATE 	1	/* РЈР’Р§ РїРѕ СѓРјРѕР»С‡Р°РЅРёСЋ РІРєР»СЋС‡С‘РЅ (1) РёР»Рё РІС‹РєР»СЋС‡РµРЅ (0) */


	/* С‡С‚Рѕ Р·Р° РїР°РјСЏС‚СЊ РЅР°СЃС‚СЂРѕРµРє Рё С‡Р°СЃС‚РѕС‚ РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІ РєРѕРЅС‚СЂРѕР»Р»РµСЂРµ */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// РўР°Рє Р¶Рµ РїСЂРё РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРё FM25040A - 5 РІРѕР»СЊС‚, 512 Р±Р°Р№С‚
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A


	// End of NVRAM definitions section


	/* РјР°Р»РµРЅСЊРєР°СЏ РїР»Р°С‚Р°, РЅР° РєРѕС‚РѕСЂРѕР№ РѕР±Р° DDS - СЌС‚Рѕ AD9834 */
	#define FTW_RESOLUTION 28	/* СЂР°Р·СЂСЏРґРЅРѕСЃС‚СЊ FTW РІС‹Р±СЂР°РЅРЅРѕРіРѕ DDS */

	/* Board hardware configuration */
	#define DDS1_TYPE DDS_TYPE_AD9834
	#define DDS2_TYPE DDS_TYPE_AD9834
	#define PLL1_TYPE PLL_TYPE_ADF4001
	//#define PLL2_TYPE PLL_TYPE_ADF4001

	#define DDS1_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 */
	#define DDS2_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS2 */
	#define HYBRID_NVFOS 4				/* РєРѕР»РёС‡РµСЃС‚РІРѕ Р“РЈРќ РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° */
	//#define HYBRID_PLL1_POSITIVE 1		/* РІ РїРµС‚Р»Рµ Р¤РђРџР§ РЅРµС‚ РёРЅРІРµСЂС‚РёСЂСѓСЋС‰РµРіРѕ СѓСЃРёР»РёС‚РµР»СЏ */
	#define HYBRID_PLL1_NEGATIVE 1		/* РІ РїРµС‚Р»Рµ Р¤РђРџР§ РµСЃС‚СЊ РёРЅРІРµСЂС‚РёСЂСѓСЋС‰РёР№ СѓСЃРёР»РёС‚РµР»СЊ */

	/* РќР°Р·РЅР°С‡РµРЅРёРµ Р°РґСЂРµСЃРѕРІ РЅР° SPI С€РёРЅРµ */
	#define targetdds1 SPI_CSEL0	/* DDS1 */
	#define targetdds2 SPI_CSEL1	/* DDS2 - LO3 output */
	#define targetpll1 SPI_CSEL2	/* ADF4001 after DDS1 - divide by r1 and scale to n1. Р”Р»СЏ РґРІРѕР№РЅРѕР№ DDS РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° - РІС‚РѕСЂР°СЏ DDS */
	#define targetpll2 SPI_CSEL3	/* ADF4001 - fixed 2-nd LO generate */ 
	#define targetnvram SPI_CSEL6 	/* serial nvram */
	#define targetctl1 SPI_CSEL7	/* control register */

	/* РєРѕРґС‹ РІС…РѕРґРѕРІ РєРѕРјРјСѓС‚Р°С‚РѕСЂР° РёСЃС‚РѕС‡РЅРёРєРѕРІ СЃРёРіРЅР°Р»Р° РґР»СЏ РЈРќР§ РїСЂРёС‘РјРЅРёРєР° */
	#define BOARD_DETECTOR_MUTE 0x02
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x01
	#define BOARD_DETECTOR_FM 0x03
	#define BOARD_DETECTOR_TUNE 0x00	/* РєРѕРЅС„РёРіСѓСЂР°С†РёСЏ РїР»Р°С‚С‹ РґР»СЏ СЂРµР¶РёРјР° TUNE (CWZ РЅР° РїРµСЂРµРґР°С‡Сѓ) */

/* РєРѕРґС‹ С„РёР»СЊС‚СЂРѕРІ РІС‚РѕСЂРѕР№ РџР§, РІС‹РґР°РІР°РµРјС‹Рµ РЅР° РґРµС€РёС„СЂР°С‚РѕСЂС‹ */
#define BOARD_FILTER_0P5		0x03	/* 0.5 or 0.3 kHz filter */
#define BOARD_FILTER_9P0		0x00	/* 9 kHz filter - РЅР° РѕС‡РµРЅСЊ СЃС‚Р°СЂС‹С… РїР»Р°С‚Р°С… */
#define BOARD_FILTER_1P8		0x01	/* 1.8 kHz filter - РЅР° РѕС‡РµРЅСЊ СЃС‚Р°СЂС‹С… РїР»Р°С‚Р°С… */
#define BOARD_FILTER_3P1		0x06	/* 3.1 or 2.75 kHz filter */
#define BOARD_FILTER_6P0		0x02	/* 6 kHz filter */
#define BOARD_FILTER_8P0		0x04	/* only 1-st IF filter used (6, 8 or 15 khz width) */
#define BOARD_FILTER_15P0	0x04	/* only 1-st IF filter used (6, 8 or 15 khz width) */

#define BOARD_FILTER_15P0_NFM		0x00	/* TODO: РїСЂРѕРІРµСЂРёС‚СЊ, С‡С‚Рѕ РЅР°РґРѕ РґР»СЏ РІРєР»СЋС‡РµРЅРёСЏ РѕС‚РґРµР»СЊРЅРѕРіРѕ С„РёР»СЊС‚СЂР° РґР»СЏ Р§Рњ */

	#define WITHAGCMODE5STAGES	1	// 4 СЃРєРѕСЂРѕСЃС‚Рё Рё РІС‹РєР»СЋС‡РµРЅРЅРѕ 

#if FQMODEL_64455_IF0P5 || FQMODEL_73050_IF0P5
	//#define WITHMODESETFULL 1
	#define WITHMODESETFULLNFM 1
	#define IF3_FMASK (IF3_FMASK_8P0 | IF3_FMASK_6P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
	#define IF3_FHAVE (IF3_FMASK_8P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
	#define IF3_FMASKTX	(IF3_FMASK_3P1)
#elif FQMODEL_64455 || FQMODEL_73050
	//#define WITHMODESETFULL 1
	#define WITHMODESETFULLNFM 1
	#define IF3_FMASK (IF3_FMASK_15P0 | IF3_FMASK_9P0 | IF3_FMASK_6P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
	#define IF3_FHAVE (IF3_FMASK_15P0 | IF3_FMASK_9P0 | IF3_FMASK_6P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
	#define IF3_FMASKTX	(IF3_FMASK_3P1)
#elif FQMODEL_45_IF0P5
	// UA1ZH version
	//#define WITHMODESETFULL 1
	#define WITHMODESETFULLNFM 1
	#define IF3_FMASK (IF3_FMASK_15P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
	#define IF3_FHAVE (IF3_FMASK_15P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
	#define IF3_FMASKTX	(IF3_FMASK_3P1)
#endif

#define WITHSPLIT	1	/* СѓРїСЂР°РІР»РµРЅРёРµ СЂРµР¶РёРјР°РјРё СЂР°СЃСЃС‚СЂРѕР№РєРё РѕРґРЅРѕР№ РєРЅРѕРїРєРѕР№ */
//#define WITHSPLITEX	1	/* РўСЂРµС…РєРЅРѕРїРѕС‡РЅРѕРµ СѓРїСЂР°РІР»РµРЅРёРµ СЂРµР¶РёРјР°РјРё СЂР°СЃСЃС‚СЂРѕР№РєРё */
#define WITHCATEXT	1	/* Р Р°СЃС€РёСЂРµРЅРЅС‹Р№ РЅР°Р±РѕСЂ РєРѕРјР°РЅРґ CAT */
#define WITHELKEY	1
#define WITHKBDENCODER 1	// РїРµСЂРµСЃС‚СЂРѕР№РєР° С‡Р°СЃС‚РѕС‚С‹ РєРЅРѕРїРєР°РјРё
#define WITHKEYBOARD 1	/* РІ РґР°РЅРЅРѕРј СѓСЃС‚СЂРѕР№СЃС‚РІРµ РµСЃС‚СЊ РєР»Р°РІРёР°С‚СѓСЂР° */
//#define KEYBOARD_USE_ADC	1	/* РЅР° РѕРґРЅРѕР№ Р»РёРЅРёРё СѓСЃС‚Р°РЅРѕРІР»РµРЅРѕ  С‡РµС‚С‹СЂРµ  РєР»Р°РІРёС€Рё. РЅР° vref - 6.8K, РґР°Р»РµРµ 2.2Рљ, 4.7Рљ Рё 13K. */

// РќР°Р·РЅР°С‡РµРЅРёСЏ РІС…РѕРґРѕРІ РђР¦Рџ РїСЂРѕС†РµСЃСЃРѕСЂР°.
enum 
{ 
	SMETERIX = 0,	// S-meter
#if KEYBOARD_USE_ADC
	#if WITHTX
		#if WITHVOX
			VOXIX = 2, AVOXIX = 1,	// VOX
		#endif
		#if WITHBARS
			FWD = 4, REF = 3,	// SWR-meter
		#endif
	#endif
	KI0 = 5, KI1 = 6, KI2 = 7	// РєР»Р°РІРёР°С‚СѓСЂР°
#endif
};
#if KEYBOARD_USE_ADC
	#define KI_COUNT 3	// РєРѕР»РёС‡РµСЃС‚РІРѕ РёСЃРїРѕР»СЊР·СѓРµРјС‹С… РїРѕРґ РєР»Р°РІРёР°С‚СѓСЂСѓ РІС…РѕРґРѕРІ РђР¦Рџ
#endif

#endif /* ATMEGA_CTLSTYLE_V5_H_INCLUDED */
