/* $Id$ */
/* satmod9_v0 ARM board */
/* board-specific CPU attached signals */
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//

#ifndef ARM_CTLSTYLE_V9_H_INCLUDED
#define ARM_CTLSTYLE_V9_H_INCLUDED 1
	/* РјРѕРґРµР»Рё СЃРёРЅС‚РµР·Р°С‚РѕСЂРѕРІ - СЃС…РµРјС‹ С‡Р°СЃС‚РѕС‚РѕРѕР±СЂР°Р·РѕРІР°РЅРёСЏ */
	#define DIRECT_50M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_125M0_X1		1	/* Board hardware configuration */
	#define FQMODEL_TRX8M		1	// РџРµСЂРІР°СЏ РЅРёР·РєР°СЏ РџР§ 8 РњР“С†, 6 РњР“С†, 5.5 РњР“С† Рё РґСЂСѓРіРёРµ
	//#define FQMODEL_DCTRX		1	// РїСЂСЏРјРѕРµ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµ
	#define WITHNESTEDINTERRUPTS	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїСЂРё РЅР°Р»РёС‡РёРё real-time С‡Р°СЃС‚Рё. */

	//#define BANDSELSTYLERE_NOTHING 1
	#define BANDSELSTYLERE_LOCONV32M_NLB 1
	// Р’С‹Р±РѕСЂ РџР§
	//#define IF3_MODEL IF3_TYPE_9000
	#define IF3_MODEL IF3_TYPE_8868
	#define	MODEL_DIRECT	1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїСЂСЏРјРѕР№ СЃРёРЅС‚РµР·, Р° РЅРµ РіРёР±СЂРёРґРЅС‹Р№ */

	#define DEFPREAMPSTATE 	1	/* РЈР’Р§ РїРѕ СѓРјРѕР»С‡Р°РЅРёСЋ РІРєР»СЋС‡С‘РЅ (1) РёР»Рё РІС‹РєР»СЋС‡РµРЅ (0) */

	/* РєРѕРґС‹ РІС…РѕРґРѕРІ РєРѕРјРјСѓС‚Р°С‚РѕСЂР° РёСЃС‚РѕС‡РЅРёРєРѕРІ СЃРёРіРЅР°Р»Р° РґР»СЏ РЈРќР§ РїСЂРёС‘РјРЅРёРєР° */
	//#define BOARD_DETECTOR_MUTE 0x02
	//#define BOARD_DETECTOR_SSB 0x00
	//#define BOARD_DETECTOR_AM 0x01
	//#define BOARD_DETECTOR_FM 0x03

	/* РєРѕРґС‹ С„РёР»СЊС‚СЂРѕРІ РІС‚РѕСЂРѕР№ РџР§, РІС‹РґР°РІР°РµРјС‹Рµ РЅР° РґРµС€РёС„СЂР°С‚РѕСЂС‹ */
	#define BOARD_FILTER_0P5		0x00	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_1P8		0x01	/* 1.8 kHz filter - РЅР° РѕС‡РµРЅСЊ СЃС‚Р°СЂС‹С… РїР»Р°С‚Р°С… */
	#define BOARD_FILTER_2P7		0x02	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_3P1		0x03	/* 3.1 or 2.75 kHz filter */

	#define WITHMODESETMIXONLY 1	/* CW/CWR/USB/LSB */

	#define ENCRES_DEFAULT ENCRES_128
	/* РІСЃРµ РІРѕР·РјРѕР¶РЅС‹Рµ РІ РґР°РЅРЅРѕР№ РєРѕРЅС„РёРіСѓСЂР°С†РёРё С„РёР»СЊС‚СЂС‹ */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
	#define IF3_FHAVE 	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
	#define IF3_FMASKTX	(IF3_FMASK_3P1)

	// +++ РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ РїР»Р°С‚ РЅР° ATMega
	// --- РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ РїР»Р°С‚ РЅР° ATMega

	// +++ РћСЃРѕР±С‹Рµ РІР°СЂРёР°РЅС‚С‹ СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ РєРЅРѕРїРѕРє РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ
	//#define KEYB_RA1AGG	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ РєРѕСЂРїСѓСЃР°, СЃРґРµР»Р°РЅРЅРѕРіРѕ RA1AGG - С‚СЂРё РіСЂСѓРїРїС‹ РїРѕ С‡РµС‚С‹СЂРµ Р»РёРЅРµР№РєРё РєРЅРѕРїРѕРє РґСЂСѓРі Р·Р° РґСЂСѓРіРѕРј РІ РѕРґРёРЅ СЂСЏРґ. */
	//#define KEYB_UA1CEI	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ UA1CEI */
	// --- РћСЃРѕР±С‹Рµ РІР°СЂРёР°РЅС‚С‹ СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ РєРЅРѕРїРѕРє РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ

	// +++ РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°
	//#define LCDMODE_S1D13781_NHWACCEL 1	/* РќРµРёСЃРїРѕР»СЊР·РѕРІР°РЅРµ Р°РїРїР°СЂР°С‚РЅС‹С… РѕСЃРѕР±РµРЅРЅРѕСЃС‚РµР№ EPSON S1D13781 РїСЂРё РІС‹РІРѕРґРµ РіСЂР°С„РёРєРё */
	//#define LCDMODE_S1D13781	1	/* РРЅРЅРґРёРєР°С‚РѕСЂ 480*272 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	#define LCDMODE_UC1608	1		/* РРЅРґРёРєР°С‚РѕСЂ 240*128 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1608.- РјРѕРЅРѕС…СЂРѕРјРЅС‹Р№ */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРІРµСЂС…Сѓ) */
	// --- РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°


	#define WITHMENU 	1	/* С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РјРµРЅСЋ РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕС‚РєР»СЋС‡РµРЅР° - РµСЃР»Рё РЅР°СЃС‚СЂР°РёРІР°С‚СЊ РЅРµС‡РµРіРѕ */
	#define WITHDEBUG		1	/* РћС‚Р»Р°РґРѕС‡РЅР°СЏ РїРµС‡Р°С‚СЊ С‡РµСЂРµР· COM-РїРѕСЂС‚. Р‘РµР· CAT (WITHCAT) */
	//#define WITHSLEEPTIMER	1	/* РІС‹РєР»СЋС‡РёС‚СЊ РёРЅРґРёРєР°С‚РѕСЂ Рё РІС‹РІРѕРґ Р·РІСѓРєР° РїРѕ РёСЃС‚РµС‡РµРЅРёРёРё СѓРєР°Р·Р°РЅРЅРѕРіРѕ РІСЂРµРјРµРЅРё */
	// --- Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ

	#define CTLREGSTYLE_NOCTLREG 1
	#define WITHAGCMODENONE		1	/* Р РµР¶РёРјР°РјРё РђР РЈ РЅРµ СѓРїСЂР°РІР»СЏРµРј */
	#define BOARD_AGCCODE_OFF	0	// stub
	#define WITHPREAMPATT2_6DB		1	// РЈРїСЂР°РІР»РµРЅРёРµ РЈР’Р§ Рё РґРІСѓС…РєР°СЃРєР°РґРЅС‹Рј Р°С‚С‚РµРЅСЋР°С‚РѕСЂРѕРј СЃ Р·Р°С‚СѓС…Р°РЅРёСЏРјРё 0 - 6 - 12 - 18 dB */
	#define WITHUSESDCARD 1			// Р’РєР»СЋС‡РµРЅРёРµ РїРѕРґРґРµСЂР¶РєРё SD CARD
	//#define WITHUSEAUDIOREC	1	// Р—Р°РїРёСЃСЊ Р·РІСѓРєР° РЅР° SD CARD

	/* С‡С‚Рѕ Р·Р° РїР°РјСЏС‚СЊ РЅР°СЃС‚СЂРѕРµРє Рё С‡Р°СЃС‚РѕС‚ РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІ РєРѕРЅС‚СЂРѕР»Р»РµСЂРµ */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// РўР°Рє Р¶Рµ РїСЂРё РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРё FM25040A - 5 РІРѕР»СЊС‚, 512 Р±Р°Р№С‚
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_ATMEGA

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A
	#define NVRAM_TYPE NVRAM_TYPE_NOTHING	// РЅРµС‚ NVRAM
	#define HARDWARE_IGNORENONVRAM	1		// РѕС‚Р»Р°РґРєР° РЅР° РїР»Р°С‚Р°С… РіРґРµ РЅРµС‚ РЅРёРєР°РєРѕРіРѕ NVRAM

	// End of NVRAM definitions section
	#define PLL1_TYPE PLL_TYPE_NONE		// Р¤РђРџР§ РІРЅРµС€РЅСЏСЏ, РЅРёРєР°Рє РЅРµ СѓРїСЂР°РІР»СЏРµС‚СЃСЏ. For zak user

/* РќР°Р·РЅР°С‡РµРЅРёРµ Р°РґСЂРµСЃРѕРІ РЅР° SPI С€РёРЅРµ */
#define targetsdcard	SPI_CSEL0 	/* SD CARD */
#define targetnvram		SPI_CSEL1  	/* serial nvram */
#define targetlcd	SPI_CSEL2 	/* TFT over SPI line devices control */
#define targetctl1		SPI_CSEL6 	/* control register */
#define targetuc1608 0

#define WITHKEYBOARD 1	/* РІ РґР°РЅРЅРѕРј СѓСЃС‚СЂРѕР№СЃС‚РІРµ РµСЃС‚СЊ РєР»Р°РІРёР°С‚СѓСЂР° */
#define KEYBOARD_USE_ADC	1	/* РЅР° РѕРґРЅРѕР№ Р»РёРЅРёРё СѓСЃС‚Р°РЅРѕРІР»РµРЅРѕ  С‡РµС‚С‹СЂРµ  РєР»Р°РІРёС€Рё. РЅР° vref - 6.8K, РґР°Р»РµРµ 2.2Рљ, 4.7Рљ Рё 15Рљ. */
// РќР°Р·РЅР°С‡РµРЅРёСЏ РІС…РѕРґРѕРІ РђР¦Рџ РїСЂРѕС†РµСЃСЃРѕСЂР°.
enum 
{ 
	KI0 = 5, KI1 = 6, KI2 = 7, // РєР»Р°РІРёР°С‚СѓСЂР°
};
#define	KI_COUNT 3
#define KI_LIST	KI2, KI1, KI0,	// РёРЅРёС†РёР°Р»РёР·Р°С‚РѕСЂС‹ РґР»СЏ С„СѓРЅРєС†РёРё РїРµСЂРµРєРѕРґРёСЂРѕРІРєРё

#endif /* ARM_CTLSTYLE_V9_H_INCLUDED */
