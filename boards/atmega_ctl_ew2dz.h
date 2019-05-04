/* $Id$ */
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//
// РџР»Р°С‚Р° UA3DKC РЎРµСЂРіРµР№ (ATMega32, FM25040, Si570+AD9834+WH2002) - ref 40 MHz
// С‚РѕР»СЊРєРѕ СЃ "РѕР±С‹С‡РЅС‹Рј" СЂРµРіРёСЃС‚СЂРѕРј СѓРїСЂР°РІР»РµРЅРёСЏ. AD9834 С„РѕСЂРјРёСЂСѓРµС‚ СЃРёРіРЅР°Р» РїРµСЂРµРЅРѕСЃР° СЃ 5530 РєР“С† РЅР° 500 РєР“С†
//

#ifndef ATMEGA_CTL_EW2DZ_H_INCLUDED
#define ATMEGA_CTL_EW2DZ_H_INCLUDED 1

#if F_CPU != 8000000
	#error Set F_CPU right value in project file
#endif
	/* РјРѕРґРµР»Рё СЃРёРЅС‚РµР·Р°С‚РѕСЂРѕРІ - СЃС…РµРјС‹ С‡Р°СЃС‚РѕС‚РѕРѕР±СЂР°Р·РѕРІР°РЅРёСЏ */

	#define	MODEL_DIRECT	1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїСЂСЏРјРѕР№ СЃРёРЅС‚РµР·, Р° РЅРµ РіРёР±СЂРёРґРЅС‹Р№ */
	#define LO1MODE_DIRECT	1

	#if 1
		#define KEYB_VERTICAL_REV	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ РїР»Р°С‚ "Р’РѕСЂРѕР±РµР№" Рё "РљРѕР»РёР±СЂРё" */
		// EW2DZ
		/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РІРЅРёР·" */
		/*
			РСЃС…РѕРґРЅС‹Рµ РґР°РЅРЅС‹Рµ С‚Р°РєРёРµ: РџР§ - 5 РњР“С†; РёРЅРґРёРєР°С‚РѕСЂ - RDX0154; РїР°РјСЏС‚СЊ - 25L160AI; 
			СЂРµР¶РёРјС‹ - SSB Рё CW; С„РёР»СЊС‚СЂС‹ - 3.0 РєР“С† Рё 1.8 РєР“С†; РіРµРЅРµСЂР°С‚РѕСЂ - 48 РњР“С†; 
			С‚РѕР»СЊРєРѕ РґРІРµ 595-С‹Рµ Рё РєР»Р°РІРёР°С‚СѓСЂР° Р±СѓРґРµС‚ 4 СЂСЏРґР° РїРѕ 3 РєРЅРѕРїРєРё.
		*/
		#define FQMODEL_TRX8M		1	// РџРµСЂРІР°СЏ РЅРёР·РєР°СЏ РџР§ 8 РњР“С†, 6 РњР“С†, 5.5 РњР“С† Рё РґСЂСѓРіРёРµ
		#define IF3_MODEL IF3_TYPE_5000
		#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
		#define LCDMODE_RDX0154	1				/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР°  */
		#define DIRECT_48M0_X1		1
		#define CTLREGMODE16	1		/* СѓРїСЂР°РІР»СЏСЋС‰РёР№ СЂРµРіРёСЃС‚СЂ 16 Р±РёС‚ - РґР»СЏ SIMPLE СЃРёРЅС‚РµР·Р°С‚РѕСЂР° РЅР° AD8932/AD8935/AD8933	*/
		#define WITHMODESETMIXONLY 1		/* EW2DZ version */
		/* РЅР°Р±РѕСЂ С„РёР»СЊС‚СЂРѕРІ РІ С‚СЂР°РєС‚Рµ */
		#define IF3_FMASK	(IF3_FMASK_1P8 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FHAVE	(IF3_FMASK_1P8 | IF3_FMASK_3P1)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
		/* РєРѕРґС‹ С„РёР»СЊС‚СЂРѕРІ РІС‚РѕСЂРѕР№ РџР§, РІС‹РґР°РІР°РµРјС‹Рµ РЅР° РґРµС€РёС„СЂР°С‚РѕСЂС‹ */
		#define BOARD_FILTER_0P5		BOARD_FILTERCODE_0	/* 0.5 or 0.3 kHz filter */
		#define BOARD_FILTER_1P8		BOARD_FILTERCODE_0	/* 0.5 or 0.3 kHz filter */
		#define BOARD_FILTER_3P1		BOARD_FILTERCODE_1	/* 3.1 or 2.75 kHz filter */

		/* Р°СЂСѓ СѓРїСЂР°РІР»СЏРµС‚СЃСЏ FAST/SLOW */
		#define WITHAGCMODESLOWFAST 1	/* СЃСЂРµРґРё СЂРµР¶РёРјРѕРІ РђР РЈ РµСЃС‚СЊ С‚РѕР»СЊРєРѕ РІРєР»СЋС‡РµРЅРѕ-РІС‹РєР»СЋС‡РµРЅРѕ */
		#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_0
		#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_1
	#elif 1
		#define KEYB_VERTICAL_REV	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ РїР»Р°С‚ "Р’РѕСЂРѕР±РµР№" Рё "РљРѕР»РёР±СЂРё" */
		// EW2DZ
		/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РІРЅРёР·" */
		/*
			РџР§ - 8.982 РњР“С†; РёРЅРґРёРєР°С‚РѕСЂ - RDX0154; РїР°РјСЏС‚СЊ - 25L160AI; СЂРµР¶РёРјС‹ - SSB Рё CW; 
			С„РёР»СЊС‚СЂС‹ - 3.0 РєР“С† Рё 1.8 РєР“С†; РіРµРЅРµСЂР°С‚РѕСЂ - 48 РњР“С†; С‚РѕР»СЊРєРѕ РґРІРµ 595-С‹Рµ Рё 
			РєР»Р°РІРёР°С‚СѓСЂР° Р±СѓРґРµС‚ 4 СЂСЏРґР° РїРѕ 3 РєРЅРѕРїРєРё. 
			Р СЃРґРµР»Р°Р№С‚Рµ С‚Р°РєСѓСЋ Р¶Рµ РїСЂРѕС€РёРІРєСѓ, С‚РѕР»СЊРєРѕ РџР§ Р±СѓРґРµС‚ 8982 РєР“С† Рё С„РёР»СЊС‚СЂ РЅРµ 1.8, Р° 0.5 РєР“С† 
		*/
		#define FQMODEL_TRX8M		1	// РџРµСЂРІР°СЏ РЅРёР·РєР°СЏ РџР§ 8 РњР“С†, 6 РњР“С†, 5.5 РњР“С† Рё РґСЂСѓРіРёРµ
		#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
		#define LCDMODE_RDX0154	1				/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР°  */
		#define DIRECT_48M0_X1		1
		#define CTLREGMODE16	1		/* СѓРїСЂР°РІР»СЏСЋС‰РёР№ СЂРµРіРёСЃС‚СЂ 16 Р±РёС‚ - РґР»СЏ SIMPLE СЃРёРЅС‚РµР·Р°С‚РѕСЂР° РЅР° AD8932/AD8935/AD8933	*/
		#define WITHMODESETMIXONLY 1		/* EW2DZ version */
		/* РЅР°Р±РѕСЂ С„РёР»СЊС‚СЂРѕРІ РІ С‚СЂР°РєС‚Рµ */
		#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
		/* РєРѕРґС‹ С„РёР»СЊС‚СЂРѕРІ РІС‚РѕСЂРѕР№ РџР§, РІС‹РґР°РІР°РµРјС‹Рµ РЅР° РґРµС€РёС„СЂР°С‚РѕСЂС‹ */
		#define BOARD_FILTER_0P5		BOARD_FILTERCODE_0	/* 0.5 or 0.3 kHz filter */
		#define BOARD_FILTER_3P1		BOARD_FILTERCODE_1	/* 3.1 or 2.75 kHz filter */

		/* Р°СЂСѓ СѓРїСЂР°РІР»СЏРµС‚СЃСЏ FAST/SLOW */
		#define WITHAGCMODESLOWFAST 1	/* СЃСЂРµРґРё СЂРµР¶РёРјРѕРІ РђР РЈ РµСЃС‚СЊ С‚РѕР»СЊРєРѕ РІРєР»СЋС‡РµРЅРѕ-РІС‹РєР»СЋС‡РµРЅРѕ */
		#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_0
		#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_1

		#define IF3DF (8982000L - 8865850L)

		#define IF3_CUSTOM_CW_CENTER	(8866500L + IF3DF)
		#define IF3_CUSTOM_SSB_LOWER	(8865850L + IF3DF)
		#define IF3_CUSTOM_SSB_UPPER	(8868700L + IF3DF)
		#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

		#define IF3_MODEL	IF3_TYPE_CUSTOM

	#elif 1
		#define KEYB_V8S_DK1VS	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РґР»СЏ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° DK1VS */
		// EW2DZ
		/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РІРЅРёР·" */
		/* Р°СЂСѓ СѓРїСЂР°РІР»СЏРµС‚СЃСЏ FAST/SLOW */
		#define WITHAGCMODESLOWFAST 1	/* СЃСЂРµРґРё СЂРµР¶РёРјРѕРІ РђР РЈ РµСЃС‚СЊ С‚РѕР»СЊРєРѕ РІРєР»СЋС‡РµРЅРѕ-РІС‹РєР»СЋС‡РµРЅРѕ */
		#define FQMODEL_TRX5M_UA1FA	1	// РџРµСЂРІР°СЏ РЅРёР·РєР°СЏ РџР§, РІС‚РѕСЂР°СЏ РџР§ 500 РєР“С†
		#define IF3_MODEL IF3_TYPE_500
		#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
		#define LCDMODE_WH1602	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 16*2 */
		#define DIRECT_25M0_X1		1
		//#define	LSBONLO4DEFAULT	1		/* РїСЂРёРјРµРЅС‘РЅ Р­РњР’ РЅР° РЅРёР¶РЅСЋСЋ Р±РѕРєРѕРІСѓСЋ */
		#define CTLREGMODE16	1		/* СѓРїСЂР°РІР»СЏСЋС‰РёР№ СЂРµРіРёСЃС‚СЂ 16 Р±РёС‚ - РґР»СЏ SIMPLE СЃРёРЅС‚РµР·Р°С‚РѕСЂР° РЅР° AD8932/AD8935/AD8933	*/
		#define BOARD_FILTER_0P5_LSB 1		/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ С„РёР»СЊС‚СЂ Р­РњР¤-500-0,5Рќ	*/
		#define BOARD_FILTER_3P1_LSB 1		/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ С„РёР»СЊС‚СЂ Р­РњР¤-500-3Рќ	*/
		#define WITHMODESETMIXONLY 1	/* EW2DZ version */
		#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
		/* РєРѕРґС‹ С„РёР»СЊС‚СЂРѕРІ РІС‚РѕСЂРѕР№ РџР§, РІС‹РґР°РІР°РµРјС‹Рµ РЅР° РґРµС€РёС„СЂР°С‚РѕСЂС‹ */
		#define BOARD_FILTER_0P5		BOARD_FILTERCODE_0	/* 0.5 or 0.3 kHz filter */
		#define BOARD_FILTER_3P1		BOARD_FILTERCODE_1	/* 3.1 or 2.75 kHz filter */
		#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_0
		#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_1
	#else
		#define KEYB_V8S_DK1VS	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РґР»СЏ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° DK1VS */
		// EW2DZ
		/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РІРЅРёР·" */
		/* Р°СЂСѓ СѓРїСЂР°РІР»СЏРµС‚СЃСЏ ON/OFF */
		#define WITHAGCMODEONOFF 1	/* СЃСЂРµРґРё СЂРµР¶РёРјРѕРІ РђР РЈ РµСЃС‚СЊ С‚РѕР»СЊРєРѕ РІРєР»СЋС‡РµРЅРѕ-РІС‹РєР»СЋС‡РµРЅРѕ */
		#define FQMODEL_TRX5M_UA1FA	1	// РџРµСЂРІР°СЏ РЅРёР·РєР°СЏ РџР§, РІС‚РѕСЂР°СЏ РџР§ 500 РєР“С†
		#define IF3_MODEL IF3_TYPE_500
		#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
		#define LCDMODE_WH2002	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 20*2 */
		#define DIRECT_25M0_X1		1
		//#define	LSBONLO4DEFAULT	0		/* РїСЂРёРјРµРЅС‘РЅ Р­РњР’ РЅР° РІРµСЂС…РЅСЋСЋ Р±РѕРєРѕРІСѓСЋ */
		#define CTLREGMODE16	1		/* СѓРїСЂР°РІР»СЏСЋС‰РёР№ СЂРµРіРёСЃС‚СЂ 16 Р±РёС‚ - РґР»СЏ SIMPLE СЃРёРЅС‚РµР·Р°С‚РѕСЂР° РЅР° AD8932/AD8935/AD8933	*/
		#define WITHMODESETMIXONLY 1	/* EW2DZ version */
		#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FHAVE	(IF3_FMASK_3P1)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
		/* РєРѕРґС‹ С„РёР»СЊС‚СЂРѕРІ РІС‚РѕСЂРѕР№ РџР§, РІС‹РґР°РІР°РµРјС‹Рµ РЅР° РґРµС€РёС„СЂР°С‚РѕСЂС‹ */
		#define BOARD_FILTER_0P5		BOARD_FILTERCODE_0	/* 0.5 or 0.3 kHz filter */
		#define BOARD_FILTER_3P1		BOARD_FILTERCODE_1	/* 3.1 or 2.75 kHz filter */

		#define	BOARD_AGCCODE_ON	BOARD_AGCCODE_0
		#define	BOARD_AGCCODE_OFF	BOARD_AGCCODE_1
	#endif

	#define DDS1_CLK_MUL	1 		/* РЈРјРЅРѕР¶РёС‚РµР»СЊ РІ DDS1 */
	#define DDS2_CLK_MUL	1		/* РЈРјРЅРѕР¶РёС‚РµР»СЊ РІ DDS2 */
	//#define DDS3_CLK_MUL	1 		/* РЈРјРЅРѕР¶РёС‚РµР»СЊ РІ DDS3 */

	#define	FONTSTYLE_ITALIC	1	//

	// --- РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ СЂР°Р·РЅС‹С… РїР»Р°С‚

	//#define KEYB18_GENERIC	1		/* 18 РєРЅРѕРїРѕРє - СЂР°СЃС€РёСЂРµРЅРЅС‹РѕРµ РїРѕР»Рµ */
	//#define KEYBOARD_USE_ADC6	1	/* С€РµСЃС‚СЊ РєРЅРѕРїРѕРє РЅР° РєР°Р¶РґРѕРј РІС…РѕРґРµ ADCx */
	// --- РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_100
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define	WITHENCODER	1	/* РґР»СЏ РёР·РјРµРЅРµРЅРёСЏ С‡Р°СЃС‚РѕС‚С‹ РёРјРµРµС‚СЃСЏ РµРЅРєРѕРґРµСЂ */

	// +++ Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ
	#define WITHTX		1	/* РІРєР»СЋС‡РµРЅРѕ СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - СЃРёРєРІРµРЅСЃРѕСЂ, СЌР»РµРєС‚СЂРѕРЅРЅС‹Р№ РєР»СЋС‡. */
	//#define WITHAUTOTUNER	1	/* Р•СЃС‚СЊ С„СѓРЅРєС†РёСЏ Р°РІС‚РѕС‚СЋРЅРµСЂР° */
	#define WITHCAT		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ CAT */
	//#define WITHDEBUG		1	/* РћС‚Р»Р°РґРѕС‡РЅР°СЏ РїРµС‡Р°С‚СЊ С‡РµСЂРµР· COM-РїРѕСЂС‚. Р‘РµР· CAT (WITHCAT) */
	//#define WITHVOX		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ VOX */
	#define WITHSWRMTR	1	/* РР·РјРµСЂРёС‚РµР»СЊ РљРЎР’ РёР»Рё */
	//#define WITHPWRMTR	1	/* РРЅРґРёРєР°С‚РѕСЂ РІС‹С…РѕРґРЅРѕР№ РјРѕС‰РЅРѕСЃС‚Рё  */
	#define WITHSHOWSWRPWR 1	/* РЅР° РґРёСЃРїР»РµРµ РѕРґРЅРѕРІСЂРµРјРµРЅРЅРѕ РѕС‚РѕР±СЂР°Р¶Р°СЋС‚СЃСЏ SWR-meter Рё PWR-meter */
	#define WITHBARS	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ S-РјРµС‚СЂР° Рё SWR-РјРµС‚СЂР° */
	//#define WITHPWRLIN	1	/* РРЅРґРёРєР°С‚РѕСЂ РІС‹С…РѕРґРЅРѕР№ РјРѕС‰РЅРѕСЃС‚Рё РїРѕРєР°Р·С‹РІР°РµС‚ РЅР°РїСЂСЏР¶РµРЅРёРµ Р° РЅРµ РјРѕС‰РЅРѕСЃС‚СЊ */
	//#define WITHVOLTLEVEL	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ РЅР°РїСЂСЏР¶РµРЅРёСЏ РђРљР‘ */
	//#define WITHSWLMODE	1	/* РїРѕРґРґРµСЂР¶РєР° Р·Р°РїРѕРјРёРЅР°РЅРёСЏ РјРЅРѕР¶РµСЃС‚РІР° С‡Р°СЃС‚РѕС‚ РІ swl-mode */
	//#define WITHPBT		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ PBT */
	#define WITHIFSHIFT	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* РќР°С‡Р°Р»СЊРЅРѕРµ Р·Р°РЅС‡РµРЅРёРµ IF SHIFT */
	//#define WITHFIXEDBFO	1	/* РџРµСЂРµРІРѕСЂРѕС‚ Р±РѕРєРѕРІС‹С… Р·Р° СЃС‡С‘С‚ 1-РіРѕ РіРµС‚СЂРѕРґРёРЅР° (РѕСЃРѕР±РµРЅРЅРѕ, РµСЃР»Рё РЅРµС‚ РїРѕРґСЃС‚СЂР°РёРІР°РµРјРѕРіРѕ BFO) */
	//#define WITHDUALBFO		1	/* РџРµСЂРµРІРѕСЂРѕС‚ Р±РѕРєРѕРІС‹С… Р·Р° СЃС‡С‘С‚ РїРµСЂРµРєР»СЋС‡РµРЅРёСЏ С‡Р°СЃС‚РѕС‚С‹ BFO РІРЅРµС€РЅРёРј СЃРёРіРЅР°Р»РѕРј */
	//#define WITHDUALFLTR	1	/* РџРµСЂРµРІРѕСЂРѕС‚ Р±РѕРєРѕРІС‹С… Р·Р° СЃС‡С‘С‚ РїРµСЂРµРєР»СЋС‡РµРЅРёСЏ С„РёР»СЊС‚СЂР° РІРµСЂС…РЅРµР№ РёР»Рё РЅРёР¶РЅРµР№ Р±РѕРєРѕРІРѕР№ РїРѕР»РѕСЃС‹ */
	#define WITHSAMEBFO	1	/* РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РѕР±С‰РёС… РЅР°СЃС‚СЂРѕРµРє BFO РґР»СЏ РїСЂРёС‘РјР° Рё РїРµСЂРµРґР°С‡Рё */
	//#define WITHONLYBANDS 1		/* РџРµСЂРµСЃС‚СЂРѕР№РєР° РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕРіСЂР°РЅРёС‡РµРЅР° Р»СЋР±РёС‚РµР»СЊСЃРєРёРјРё РґРёР°РїР°Р·РѕРЅР°РјРё */
	//#define WITHBCBANDS	1		/* РІ С‚Р°Р±Р»РёС†Рµ РґРёР°РїР°Р·РѕРЅРѕРІ РїСЂРёСЃСѓС‚СЃС‚РІСѓСЋС‚ РІРµС‰Р°С‚РµР»СЊРЅС‹Рµ РґРёР°РїР°Р·РѕРЅС‹ */
	#define WITHWARCBANDS	1	/* Р’ С‚Р°Р±Р»РёС†Рµ РґРёР°РїР°Р·РѕРЅРѕРІ РїСЂРёСЃСѓС‚СЃС‚РІСѓСЋС‚ HF WARC РґРёР°РїР°Р·РѕРЅС‹ */
	//#define WITHDIRECTBANDS 1	/* РџСЂСЏРјРѕР№ РїРµСЂРµС…РѕРґ Рє РґРёР°РїР°Р·РѕРЅР°Рј РїРѕ РЅР°СЌР°С‚РёСЏРј РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ */

	#define WITHMENU 	1	/* С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РјРµРЅСЋ РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕС‚РєР»СЋС‡РµРЅР° - РµСЃР»Рё РЅР°СЃС‚СЂР°РёРІР°С‚СЊ РЅРµС‡РµРіРѕ */
	//#define WITHVIBROPLEX	1	/* РІРѕР·РјРѕР¶РЅРѕСЃС‚СЊ СЌРјСѓР»СЏС†РёРё РїРµСЂРµРґР°С‡Рё РІРёР±СЂРѕРїР»РµРєСЃРѕРј */
	#define WITHSLEEPTIMER	1	/* РІС‹РєР»СЋС‡РёС‚СЊ РёРЅРґРёРєР°С‚РѕСЂ Рё РІС‹РІРѕРґ Р·РІСѓРєР° РїРѕ РёСЃС‚РµС‡РµРЅРёРёРё СѓРєР°Р·Р°РЅРЅРѕРіРѕ РІСЂРµРјРµРЅРё */
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
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section

	//#define FTW_RESOLUTION 28	/* СЂР°Р·СЂСЏРґРЅРѕСЃС‚СЊ FTW РІС‹Р±СЂР°РЅРЅРѕРіРѕ DDS */
	#define FTW_RESOLUTION 32	/* СЂР°Р·СЂСЏРґРЅРѕСЃС‚СЊ FTW РІС‹Р±СЂР°РЅРЅРѕРіРѕ DDS */
	//#define FTW_RESOLUTION 31	/* СЂР°Р·СЂСЏРґРЅРѕСЃС‚СЊ FTW РІС‹Р±СЂР°РЅРЅРѕРіРѕ DDS - ATINY2313 */

	//#define WITHLO1LEVELADJ		1	/* РІРєР»СЋС‡РµРЅРѕ СѓРїСЂР°РІР»РµРЅРёРµ СѓСЂРѕРІРЅРµРј (Р°РјРїР»РёС‚СѓРґРѕР№) LO1 */
	/* Board hardware configuration */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9835
	//#define DDS1_TYPE DDS_TYPE_AD9851
	#define PLL1_TYPE PLL_TYPE_SI570
	#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_ATTINY2313
	//#define DDS1_TYPE DDS_TYPE_AD9834
	#define DDS2_TYPE DDS_TYPE_AD9834
	//#define DDS3_TYPE DDS_TYPE_AD9834
	//#define PLL1_TYPE PLL_TYPE_ADF4001
	//#define PLL2_TYPE PLL_TYPE_ADF4001	/* Р”РµР»РёС‚РµР»СЊ РѕРєС‚Р°РІРЅРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° РґР»СЏ РїРѕР»СѓС‡РµРЅРёСЏ СЃРєРІРѕР·РЅРѕРіРѕ РґРёР°РїР°Р·РѕРЅР° */
	//#define PLL2_TYPE PLL_TYPE_LMX2306	/* Р”РµР»РёС‚РµР»СЊ РѕРєС‚Р°РІРЅРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° РґР»СЏ РїРѕР»СѓС‡РµРЅРёСЏ СЃРєРІРѕР·РЅРѕРіРѕ РґРёР°РїР°Р·РѕРЅР° */

	//#define DDS1_CLK_DIV	7		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 */
	#define DDS2_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS2 */
	//#define DDS3_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS3 */

	/* РќР°Р·РЅР°С‡РµРЅРёРµ Р°РґСЂРµСЃРѕРІ РЅР° SPI С€РёРЅРµ */
	//#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	//#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. Р”Р»СЏ РґРІРѕР№РЅРѕР№ DDS РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° - РІС‚РѕСЂР°СЏ DDS */
	//#define targetpll2 SPI_CSEL3 	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetext1 SPI_CSEL4 	/* external devices control */ 
	//#define targetupd1 SPI_CSEL5 	/* DDS IOUPDATE signals at output of SPI chip-select register */
	#define targetctl1 SPI_CSEL6 	/* control register */
	#define targetnvram SPI_CSEL7  	/* serial nvram */

	#define targetlcd targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/


	/* РІСЃРµ РІРѕР·РјРѕР¶РЅС‹Рµ РІ РґР°РЅРЅРѕР№ РєРѕРЅС„РёРіСѓСЂР°С†РёРё С„РёР»СЊС‚СЂС‹ */
	//#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_1P8 | IF3_FMASK_2P7 | IF3_FMASK_3P1)
	//#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_1P8 | IF3_FMASK_2P7 | IF3_FMASK_3P1)
	//#define IF3_FMASKTX	(IF3_FMASK_3P1 | IF3_FMASK_2P7)

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
#if WITHVOX
	//VOXIX = 2, AVOXIX = 1,	// VOX - РќРµР»СЊР·СЏ, РІС‹РІРѕРґС‹ Р·Р°РґРµР№СЃС‚РІРѕРІР°РЅС‹ РїРѕРґ РІС…РѕРґС‹ РєР»СЋС‡Р°
#endif
	SMETERIX = 0,	// S-meter
	PWRI = 4, 
	FWD = 4, REF = 3,	// SWR-meter
	KI0 = 5, KI1 = 6, KI2 = 7	// РєР»Р°РІРёР°С‚СѓСЂР°
};
#define KI_COUNT 3	// РєРѕР»РёС‡РµСЃС‚РІРѕ РёСЃРїРѕР»СЊР·СѓРµРјС‹С… РїРѕРґ РєР»Р°РІРёР°С‚СѓСЂСѓ РІС…РѕРґРѕРІ РђР¦Рџ
#define KI_LIST	KI2, KI1, KI0,	// РёРЅРёС†РёР°Р»РёР·Р°С‚РѕСЂС‹ РґР»СЏ С„СѓРЅРєС†РёРё РїРµСЂРµРєРѕРґРёСЂРѕРІРєРё


#endif /* ATMEGA_CTL_EW2DZ_H_INCLUDED */
