/* $Id$ */
// ATMega: DU VENT SI570+AD9834 (66MHz), LS020, ATMega32 - Р±РµР· РІРЅРµС€РЅРµРіРѕ РґРµС€РёС„СЂР°С‚РѕСЂР° Р°РґСЂРµСЃРѕРІ - atmega_cpustyle_v1a.h, atmega_ctlstyle_v1a.h - РїРѕС…РѕР¶Р° РЅР° sw2011
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//

#ifndef ATMEGA_CTLSTYLE_V1A_H_INCLUDED
#define ATMEGA_CTLSTYLE_V1A_H_INCLUDED 1

	#define FQMODEL_TRX8M	1	// Р’С‚РѕСЂР°СЏ РџР§ 8 РњР“С†
	//#define WITHCLASSICSIDE	1	/* СЃС‚РѕСЂРѕРЅР° РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° Р·Р°РІРёСЃРёС‚ РѕС‚ СЂР°Р±РѕС‡РµР№ С‡Р°СЃС‚РѕС‚С‹ */
	//#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
	// Р’С‹Р±РѕСЂ РџР§
	//#define IF3_MODEL IF3_TYPE_9000
	#define IF3_MODEL IF3_TYPE_8868
	//#define IF3_MODEL IF3_TYPE_5645	// Drake R-4C and Drake T-4XC (Drake Twins) - 5645 kHz
	//#define IF3_MODEL IF3_TYPE_5500
	//#define IF3_MODEL IF3_TYPE_9045
	#define DIRECT_66M0_X1		1	/* Board hardware configuration */
	
	#define DSTYLE_UR3LMZMOD	1	// РўРµСЃС‚РёСЂРѕРІР°РЅРёРµ - СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ СЌР»РµРјРµРЅС‚РѕРІ СЌРєСЂР°РЅР° РІ С‚СЂР°РЅСЃРёРІРµСЂР°С… UR3LMZ
	//#define	FONTSTYLE_ITALIC	1	//
	// --- РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ СЂР°Р·РЅС‹С… РїР»Р°С‚
	#define	MODEL_DIRECT	1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїСЂСЏРјРѕР№ СЃРёРЅС‚РµР·, Р° РЅРµ РіРёР±СЂРёРґРЅС‹Р№ */

	#define CTLREGMODE16	1		/* СѓРїСЂР°РІР»СЏСЋС‰РёР№ СЂРµРіРёСЃС‚СЂ 16 Р±РёС‚ - РґР»СЏ SIMPLE СЃРёРЅС‚РµР·Р°С‚РѕСЂР° РЅР° AD8935	*/
	//#define CTLREGSTYLE_SW2011	1	// СѓРїСЂР°РІР»СЏСЋС‰РёРµ СЂРµРіРёСЃС‚СЂС‹ SW2011RDX
	//#define KEYB_VERTICAL_REV	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ РїР»Р°С‚ "Р’РѕСЂРѕР±РµР№" Рё "РљРѕР»РёР±СЂРё" */
	#define KEYB_V8S_DK1VS	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РґР»СЏ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° DK1VS */
	#define WITHAGCMODEONOFF 1	/* СЃСЂРµРґРё СЂРµР¶РёРјРѕРІ РђР РЈ РµСЃС‚СЊ С‚РѕР»СЊРєРѕ РІРєР»СЋС‡РµРЅРѕ-РІС‹РєР»СЋС‡РµРЅРѕ */
	#define	BOARD_AGCCODE_ON	BOARD_AGCCODE_0
	#define	BOARD_AGCCODE_OFF	BOARD_AGCCODE_1


	// +++ РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	//#define LCDMODE_WH2002	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 20*2, РІРѕР·РјРѕР¶РЅРѕ РІРјРµСЃС‚Рµ СЃ LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 16*2 */
	//#define LCDMODE_WH2004	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 20*4 */
	//#define LCDMODE_RDX0077	1	/* РРЅРґРёРєР°С‚РѕСЂ 128*64 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1601.  */
	//#define LCDMODE_RDX0154	1	/* РРЅРґРёРєР°С‚РѕСЂ 132*64 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1601. */
	//#define LCDMODE_RDX0120	1	/* РРЅРґРёРєР°С‚РѕСЂ 64*32 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1601.  */
	//#define LCDMODE_UC1601S_TOPDOWN	1	/* LCDMODE_RDX0154 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	//#define LCDMODE_UC1601S_EXTPOWER	1	/* LCDMODE_RDX0154 - Р±РµР· РІРЅСѓС‚СЂРµРЅРЅРµРіРѕ РїСЂРµРѕР±СЂР°Р·РѕРІР°С‚РµР»СЏ */
	//#define LCDMODE_TIC154	1	/* РРЅРґРёРєР°С‚РѕСЂ 133*65 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј PCF8535 */
	//#define LCDMODE_TIC218	1	/* РРЅРґРёРєР°С‚РѕСЂ 133*65 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј PCF8535 */
	//#define LCDMODE_PCF8535_TOPDOWN	1	/* РљРѕРЅС‚СЂРѕР»Р»РµСЂ PCF8535 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	#define LCDMODE_LS020 	1	/* РРЅРґРёРєР°С‚РѕСЂ 176*132 Sharp LS020B8UD06 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	//#define LCDMODE_LPH88		1	/* РРЅРґРёРєР°С‚РѕСЂ 176*132 LPH8836-2 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	//#define LCDMODE_S1D13781	1	/* РРЅРЅРґРёРєР°С‚РѕСЂ 480*272 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	//#define LCDMODE_ILI9320	1	/* РРЅРґРёРєР°С‚РѕСЂ 248*320 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9320 */
	//#define LCDMODE_ILI9225	1	/* РРЅРґРёРєР°С‚РѕСЂ 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9225РЎ */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃР»РµРІР° РѕС‚ СЌРєСЂР°РЅР°) */
	//#define LCDMODE_ILI9163	1	/* РРЅРґРёРєР°С‚РѕСЂ LPH9157-2 176*132 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILITEK ILI9163 - Р›РµРЅС‚Р° РґРёСЃРїР»РµСЏ СЃРїСЂР°РІР°, Р° РІС‹РІРѕРґС‹ СЃР»РµРІР°. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРїСЂР°РІР°, Р»РµРЅС‚Р° РґРёСЃРїР»РµСЏ СЃР»РµРІР°) */
	// --- РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define	WITHENCODER	1	/* РґР»СЏ РёР·РјРµРЅРµРЅРёСЏ С‡Р°СЃС‚РѕС‚С‹ РёРјРµРµС‚СЃСЏ РµРЅРєРѕРґРµСЂ */

	// +++ Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ
	#define WITHTX		1	/* РІРєР»СЋС‡РµРЅРѕ СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - СЃРёРєРІРµРЅСЃРѕСЂ, СЌР»РµРєС‚СЂРѕРЅРЅС‹Р№ РєР»СЋС‡. */
	//#define WITHPBT		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ PBT */
	#define WITHIFSHIFT	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* РќР°С‡Р°Р»СЊРЅРѕРµ Р·Р°РЅС‡РµРЅРёРµ IF SHIFT */
	//#define WITHCAT		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ CAT */
	//#define WITHDEBUG		1	/* РћС‚Р»Р°РґРѕС‡РЅР°СЏ РїРµС‡Р°С‚СЊ С‡РµСЂРµР· COM-РїРѕСЂС‚. Р‘РµР· CAT (WITHCAT) */
	#define WITHVOX		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ VOX */
	#define WITHSHOWSWRPWR 1	/* РЅР° РґРёСЃРїР»РµРµ РѕРґРЅРѕРІСЂРµРјРµРЅРЅРѕ РѕС‚РѕР±СЂР°Р¶Р°СЋС‚СЃСЏ SWR-meter Рё PWR-meter */
	#define WITHSWRMTR	1	/* РР·РјРµСЂРёС‚РµР»СЊ РљРЎР’ */
	//#define WITHPWRMTR	1	/* РРЅРґРёРєР°С‚РѕСЂ РІС‹С…РѕРґРЅРѕР№ РјРѕС‰РЅРѕСЃС‚Рё РёР»Рё */
	#define WITHBARS	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ S-РјРµС‚СЂР° Рё SWR-РјРµС‚СЂР° */
	//#define WITHVOLTLEVEL	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ РЅР°РїСЂСЏР¶РµРЅРёСЏ РђРљР‘ */
	//#define WITHSWLMODE	1	/* РїРѕРґРґРµСЂР¶РєР° Р·Р°РїРѕРјРёРЅР°РЅРёСЏ РјРЅРѕР¶РµСЃС‚РІР° С‡Р°СЃС‚РѕС‚ РІ swl-mode */
	//#define WITHPOTWPM		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ СЂРµРіСѓР»РёСЂРѕРІРєР° СЃРєРѕСЂРѕСЃС‚Рё РїРµСЂРµРґР°С‡Рё РІ С‚РµР»РµРіСЂР°С„Рµ РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂРѕРј */
	#define WITHVIBROPLEX	1	/* РІРѕР·РјРѕР¶РЅРѕСЃС‚СЊ СЌРјСѓР»СЏС†РёРё РїРµСЂРµРґР°С‡Рё РІРёР±СЂРѕРїР»РµРєСЃРѕРј */
	//#define WITHDIRECTBANDS 1	/* РџСЂСЏРјРѕР№ РїРµСЂРµС…РѕРґ Рє РґРёР°РїР°Р·РѕРЅР°Рј РїРѕ РЅР°СЌР°С‚РёСЏРј РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ */

	#define WITHMENU 	1	/* С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РјРµРЅСЋ РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕС‚РєР»СЋС‡РµРЅР° - РµСЃР»Рё РЅР°СЃС‚СЂР°РёРІР°С‚СЊ РЅРµС‡РµРіРѕ */
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
	#define FTW_RESOLUTION 32	/* СЂР°Р·СЂСЏРґРЅРѕСЃС‚СЊ FTW РІС‹Р±СЂР°РЅРЅРѕРіРѕ DDS */

	/* Board hardware configuration */
	#define PLL1_TYPE PLL_TYPE_SI570
	#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9834
	
	//#define PLL1_TYPE PLL_TYPE_ADF4001

	//#define PLL1_TYPE PLL_TYPE_ADF4360
	//#define PLL2_TYPE PLL_TYPE_CMX992

	#define DDS2_TYPE DDS_TYPE_AD9834

	//#define WITHFIXEDBFO	1	/* РџРµСЂРµРІРѕСЂРѕС‚ Р±РѕРєРѕРІС‹С… Р·Р° СЃС‡С‘С‚ 1-РіРѕ РіРµС‚СЂРѕРґРёРЅР° (РѕСЃРѕР±РµРЅРЅРѕ, РµСЃР»Рё РЅРµС‚ РїРѕРґСЃС‚СЂР°РёРІР°РµРјРѕРіРѕ BFO) */
	//#define WITHDUALFLTR	1	/* РџРµСЂРµРІРѕСЂРѕС‚ Р±РѕРєРѕРІС‹С… Р·Р° СЃС‡С‘С‚ РїРµСЂРµРєР»СЋС‡РµРЅРёСЏ С„РёР»СЊС‚СЂР° РІРµСЂС…РЅРµР№ РёР»Рё РЅРёР¶РЅРµР№ Р±РѕРєРѕРІРѕР№ РїРѕР»РѕСЃС‹ */
	//#define WITHSAMEBFO	1		/* РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РѕР±С‰РёС… РЅР°СЃС‚СЂРѕРµРє BFO РґР»СЏ РїСЂРёС‘РјР° Рё РїРµСЂРµРґР°С‡Рё */
	//#define WITHONLYBANDS 1		/* РџРµСЂРµСЃС‚СЂРѕР№РєР° РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕРіСЂР°РЅРёС‡РµРЅР° Р»СЋР±РёС‚РµР»СЊСЃРєРёРјРё РґРёР°РїР°Р·РѕРЅР°РјРё */
	#define WITHBCBANDS	1		/* РІ С‚Р°Р±Р»РёС†Рµ РґРёР°РїР°Р·РѕРЅРѕРІ РїСЂРёСЃСѓС‚СЃС‚РІСѓСЋС‚ РІРµС‰Р°С‚РµР»СЊРЅС‹Рµ РґРёР°РїР°Р·РѕРЅС‹ */
	#define WITHWARCBANDS	1	/* Р’ С‚Р°Р±Р»РёС†Рµ РґРёР°РїР°Р·РѕРЅРѕРІ РїСЂРёСЃСѓС‚СЃС‚РІСѓСЋС‚ HF WARC РґРёР°РїР°Р·РѕРЅС‹ */
	#define WITHSLEEPTIMER	1	/* РІС‹РєР»СЋС‡РёС‚СЊ РёРЅРґРёРєР°С‚РѕСЂ Рё РІС‹РІРѕРґ Р·РІСѓРєР° РїРѕ РёСЃС‚РµС‡РµРЅРёРёРё СѓРєР°Р·Р°РЅРЅРѕРіРѕ РІСЂРµРјРµРЅРё */

	//#define DDS3_TYPE DDS_TYPE_AD9834

	//#define DDS1_CLK_DIV	7		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 */
	#define DDS2_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS2 */

	/* РќР°Р·РЅР°С‡РµРЅРёРµ Р°РґСЂРµСЃРѕРІ РЅР° SPI С€РёРЅРµ */
	#define targetdds1 SPI_CSEL0	/* DDS1 - LO1 generator */
	#define targetdds2 SPI_CSEL1	/* DDS2 - LO3 generator */
	//#define targetpll1 SPI_CSEL2	/* ADF4001 after DDS1 - divide by r1 and scale to n1. Р”Р»СЏ РґРІРѕР№РЅРѕР№ DDS РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° - РІС‚РѕСЂР°СЏ DDS */
	#define targetlcd SPI_CSEL2	/* Р§РёРїСЃРµР»РµРєС‚ РёРЅРґРёРєР°С‚РѕСЂР° LS020 */
	//#define targetpll2 SPI_CSEL3	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetctl1 SPI_CSEL4	/* control register */
	#define targetnvram SPI_CSEL5 	/* serial nvram */

	#define WITHMODESETFULL 1
	//#define WITHMODESETSMART 1	/* РІ Р·Р°РІР°РёСЃРјРѕСЃС‚Рё РѕС‚ С‡Р°СЃС‚РѕС‚С‹ РјРµРЅСЏРµС‚СЃСЏ Р±РѕРєРѕРІР°СЏ, РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІРјРµСЃС‚Рµ СЃ WITHFIXEDBFO */

	/* РєРѕРґС‹ С„РёР»СЊС‚СЂРѕРІ РІС‚РѕСЂРѕР№ РџР§, РІС‹РґР°РІР°РµРјС‹Рµ РЅР° РґРµС€РёС„СЂР°С‚РѕСЂС‹ */
	#define BOARD_FILTER_0P5		0x00	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_2P7		0x01	/* 3.1 or 2.75 kHz filter */

	/* РІСЃРµ РІРѕР·РјРѕР¶РЅС‹Рµ РІ РґР°РЅРЅРѕР№ РєРѕРЅС„РёРіСѓСЂР°С†РёРё С„РёР»СЊС‚СЂС‹ */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_2P7)
	#define IF3_FMASKTX	(IF3_FMASK_2P7)
	#define IF3_FHAVE 	(IF3_FMASK_2P7 | IF3_FMASK_0P5)

	#define WITHSPLIT	1	/* СѓРїСЂР°РІР»РµРЅРёРµ СЂРµР¶РёРјР°РјРё СЂР°СЃСЃС‚СЂРѕР№РєРё РѕРґРЅРѕР№ РєРЅРѕРїРєРѕР№ */
	//#define WITHSPLITEX	1	/* РўСЂРµС…РєРЅРѕРїРѕС‡РЅРѕРµ СѓРїСЂР°РІР»РµРЅРёРµ СЂРµР¶РёРјР°РјРё СЂР°СЃСЃС‚СЂРѕР№РєРё */
	#define WITHCATEXT	1	/* Р Р°СЃС€РёСЂРµРЅРЅС‹Р№ РЅР°Р±РѕСЂ РєРѕРјР°РЅРґ CAT */
	#define WITHELKEY	1
	#define WITHKBDENCODER 1	// РїРµСЂРµСЃС‚СЂРѕР№РєР° С‡Р°СЃС‚РѕС‚С‹ РєРЅРѕРїРєР°РјРё
	#define WITHKEYBOARD 1	/* РІ РґР°РЅРЅРѕРј СѓСЃС‚СЂРѕР№СЃС‚РІРµ РµСЃС‚СЊ РєР»Р°РІРёР°С‚СѓСЂР° */
	#define KEYBOARD_USE_ADC	1	/* РЅР° РѕРґРЅРѕР№ Р»РёРЅРёРё СѓСЃС‚Р°РЅРѕРІР»РµРЅРѕ  С‡РµС‚С‹СЂРµ  РєР»Р°РІРёС€Рё. РЅР° vref - 6.8K, РґР°Р»РµРµ 2.2Рљ, 4.7Рљ Рё 13K. */

	// РќР°Р·РЅР°С‡РµРЅРёСЏ РІС…РѕРґРѕРІ РђР¦Рџ РїСЂРѕС†РµСЃСЃРѕСЂР°.
	enum 
	{ 
	#if WITHBARS
		SMETERIX = 0,	// S-meter
	#endif
	#if WITHVOX
		VOXIX = 2, AVOXIX = 1,	// VOX
	#endif
	#if WITHSWRMTR
		PWRI = 4, 
		FWD = 4, REF = 3,	// SWR-meter
	#endif
		KI0 = 5, KI1 = 6, KI2 = 7	// РєР»Р°РІРёР°С‚СѓСЂР°
	};

	#define KI_COUNT 3	// РєРѕР»РёС‡РµСЃС‚РІРѕ РёСЃРїРѕР»СЊР·СѓРµРјС‹С… РїРѕРґ РєР»Р°РІРёР°С‚СѓСЂСѓ РІС…РѕРґРѕРІ РђР¦Рџ

#endif /* ATMEGA_CTLSTYLE_V1A_H_INCLUDED */
