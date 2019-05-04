/* $Id$ */
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//
// РџР»Р°С‚Р° Р’РѕСЂРѕР±РµР№
//
// Р”Р»СЏ РєРѕРјРїРёР»СЏС†РёРё СЃРєРѕРїРёСЂРѕРІР°С‚СЊ РґР°РЅРЅС‹Р№ С„Р°Р№Р» РїРѕРґ СЂР°СЃС€РёСЂРµРЅРёРµРј .h 
//
// copy atmega644_ctlstyle_v1.h.prototype atmega644_ctlstyle_v1.h

#ifndef ATMEGA644_CTLSTYLE_V1_H_INCLUDED
#define ATMEGA644_CTLSTYLE_V1_H_INCLUDED 1

	#define WITHUSEPLL		1	/* Р“Р»Р°РІРЅР°СЏ PLL	*/
	//#define WITHUSESAIPLL	1	/* SAI PLL	*/
	//#define WITHUSESAII2S	1	/* I2S PLL	*/

	#if CPUSTYLE_STM32F1XX
		// STM32F101RB, 103, 107 processors
		//#define WITHCPUXTAL 8000000UL	// Р•СЃР»Рё РµСЃС‚СЊ РІРЅРµС€РЅРёР№ РєРІР°СЂС† РЅР° РїСЂРѕС†РµСЃСЃРѕСЂРµ.
		#if WITHCPUXTAL
			#define	REFINFREQ WITHCPUXTAL
			#define REF1_DIV 1
			#define REF1_MUL 9	// Up to 16 supported
		#else
			#define	REFINFREQ 8000000UL
			#define REF1_DIV 2
			#define REF1_MUL 9	// Up to 16 supported
		#endif

	#elif CPUSTYLE_STM32F4XX
		#if 1
			// РїСЂРё РЅР°Р»РёС‡РёРё РІРЅРµС€РЅРµРіРѕ РєРІР°СЂС†РµРІРѕРіРѕ СЂРµР·РѕРЅР°С‚РѕСЂР°
			#define WITHCPUXTAL 8000000UL	/* РќР° РїСЂРѕС†РµСЃСЃРѕСЂРµ СѓСЃС‚Р°РЅРѕРІР»РµРЅ РєРІР°СЂС† 8.000 РњР“С† */
			#define REF1_DIV 4			// ref freq = 2.000 MHz

			#if 0
				// overclocking
				#define REF1_MUL 192		// 2*192.000 MHz (192 <= PLLN <= 432) - overclocking, but USB 48 MHz clock generation
				#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_6WS	// overvlocking
			#else
				// normal operation frequency
				//#define REF1_MUL 180		// 2*180.000 MHz (192 <= PLLN <= 432)
				#define REF1_MUL 168	// for USB
				#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_5WS	// Р—Р°РґРµСЂР¶РєР° РґР»СЏ СЂР°Р±РѕС‚С‹ СЃ РїР°РјСЏС‚СЊСЋ 5 WS for 168 MHz at 3.3 volt
			#endif

		#else
			// С‚Р°РєС‚РёСЂРѕРІР°РЅРёРµ РѕС‚ РІРЅСѓС‚СЂРµРЅРЅРµРіРѕ RC РіРµРЅРµСЂР°С‚РѕСЂР° 16 РњР“С†
			#define REF1_DIV 8			// ref freq = 2.000 MHz

			#if 0
				// overclocking
				#define REF1_MUL 192		// 2*192.000 MHz (192 <= PLLN <= 432) - overclocking, but USB 48 MHz clock generation
				#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_6WS	// overvlocking
			#else
				// normal operation frequency
				#define REF1_MUL 180		// 2*180.000 MHz (192 <= PLLN <= 432)
				#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_5WS	// Р—Р°РґРµСЂР¶РєР° РґР»СЏ СЂР°Р±РѕС‚С‹ СЃ РїР°РјСЏС‚СЊСЋ 5 WS for 168 MHz at 3.3 volt
			#endif
		#endif

	#endif

/*
#if CPUSTYLE_ATMEGA && F_CPU != 10000000
	#error Set F_CPU right value in project file
#endif
*/
	/* РјРѕРґРµР»Рё СЃРёРЅС‚РµР·Р°С‚РѕСЂРѕРІ - СЃС…РµРјС‹ С‡Р°СЃС‚РѕС‚РѕРѕР±СЂР°Р·РѕРІР°РЅРёСЏ */

#if 0
	/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РЅР°РІРµСЂС…" */
	//#define FQMODEL_45_IF8868_UHF430	1	// SW2011
	//#define FQMODEL_45_IF8868	1			// SW2011 mini
	//#define FQMODEL_45_IF6000_UHF144	1			// SW2016VHF
	//#define FQMODEL_73050		1	// 1-st IF=73.050, 2-nd IF=0.455 MHz
	//#define FQMODEL_73050_IF0P5		1	// 1-st IF=73.050, 2-nd IF=0.5 MHz
	//#define FQMODEL_80455		1	// 1-st IF=80.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455		1	// 1-st IF=64.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455_IF0P5		1	// 1-st IF=64.455, 2-nd IF=0.5 MHz
	//#define FQMODEL_70455		1	// 1-st IF=70.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	#define FQMODEL_60700_IF05	1	// 60.7 -> 10.7 -> 0.5
	//#define FQMODEL_60700_IF0455	1	// 60.4 -> 10.7 -> 455 kHz
	//#define FQMODEL_60725_IF05	1	// 60.725 -> 10.725 -> 0.5
	//#define FQMODEL_60700_IF02	1	// 60.7 -> 10.7 -> 0.2
	//#define FQMODEL_FPGA		1	// FPGA + IQ over I2S
	//#define DIRECT_100M0_X1		1	/* Board hardware configuration */
	#define BANDSELSTYLERE_UPCONV56M	1	/* Up-conversion with working band .030..56 MHz */
	#define MODEL_DIRECT	1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїСЂСЏРјРѕР№ СЃРёРЅС‚РµР·, Р° РЅРµ РіРёР±СЂРёРґРЅС‹Р№ */


#elif 0
	/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РЅР°РІРµСЂС…" */
	//#define FQMODEL_45_IF8868	1	// SW2011
	//#define FQMODEL_45_IF0P5	1	// 1-st IF=45MHz, 2-nd IF=500 kHz
	//#define FQMODEL_45_IF455	1	// 1-st IF=45MHz, 2-nd IF=455 kHz
	//#define FQMODEL_45_IF10700_IF200		1	// 1st IF=45.0 MHz, 2nd IF=10.7 MHz, 3rd IF=0.2 MHz
	//#define FQMODEL_45_IF8868_IF200	1	// RA6LPO version
	//#define FQMODEL_58M11_IF8868	1	// for gena, DL6RF
	//#define FQMODEL_45_IF6000	1	// up to 32 MHz
	//#define FQMODEL_45_IF8868_UHF144	1	// SW2011
	//#define FQMODEL_45_IF8868	1	// SW2011
	//#define FQMODEL_45_IF0P5	1	// 1-st IF=45MHz, 2-nd IF=500 kHz
	//#define FQMODEL_45_IF455	1	// 1-st IF=45MHz, 2-nd IF=455 kHz
	//#define FQMODEL_45_IF128	1	// 1-st IF=45MHz, 2-nd IF=128 kHz 
	//#define FQMODEL_40_IF8000	1	// 1-st IF=48MHz, 2-nd IF=8000 kHz
	//#define FQMODEL_40_IF8000_UHF144	1	// 1-st IF=48MHz, 2-nd IF=8000 kHz
	//#define FQMODEL_45_IF5000_UHF144	1	
	//#define FQMODEL_45_IF8000_UHF433_REF53	1	// 1-st if=45MHz, REF=53, 2-nd if=8000 kHz ladvabest@gmail.com
	#define FQMODEL_36000_IF500	1	// 1-st IF=36 MHz, 2-nd if=500 kHz
	//#define BANDSELSTYLERE_UPCONV36M	1	/* Up-conversion with working band 0.1 MHz..36 MHz */
	#define BANDSELSTYLERE_UPCONV32M	1	/* Up-conversion with working band 0.1 MHz..32 MHz */
	//#define	BANDSELSTYLE_LADVABEST 1	/* ladvabest@gmail.com/ Note: 1600000L */

	#define MODEL_DIRECT	1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїСЂСЏРјРѕР№ СЃРёРЅС‚РµР·, Р° РЅРµ РіРёР±СЂРёРґРЅС‹Р№ */
	//#define WITHLO4FREQPOWER2 1	/* СЃ СѓРїСЂР°РІР»РµРЅРёРµРј СѓРјРЅРѕР¶РёС‚РµР»РµРј РІРѕ РІС‚РѕСЂРѕРј РіРµС‚РµСЂРѕРґРёРЅРµ 8868 */

#elif 0
	/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РІРЅРёР·" */
	#define FQMODEL_TRX500K		1	// РџРµСЂРІР°СЏ (Рё РµРґРёРЅСЃС‚РІРµРЅРЅР°) РџР§ 500 РєР“С†
	//#define FQMODEL_TRX455K		1	// РџРµСЂРІР°СЏ (Рё РµРґРёРЅСЃС‚РІРµРЅРЅР°СЏ) РџР§ 455 РєР“С†
	//#define WITHCLASSICSIDE	1	/* СЃС‚РѕСЂРѕРЅР° РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° Р·Р°РІРёСЃРёС‚ РѕС‚ СЂР°Р±РѕС‡РµР№ С‡Р°СЃС‚РѕС‚С‹ */
	#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	#define BANDSELSTYLERE_LOCONV15M	1	/* Down-conversion with working band .030..15 MHz */
	#define MODEL_DIRECT	1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїСЂСЏРјРѕР№ СЃРёРЅС‚РµР·, Р° РЅРµ РіРёР±СЂРёРґРЅС‹Р№ */
	#define WITHLO4FREQPOWER2 1	/* СЃ СѓРїСЂР°РІР»РµРЅРёРµРј СѓРјРЅРѕР¶РёС‚РµР»РµРј РІРѕ РІС‚РѕСЂРѕРј РіРµС‚РµСЂРѕРґРёРЅРµ 8868 */
#elif 0
	/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂСЏРјС‹Рј РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј  */
	#define FQMODEL_DCTRX		1	// РїСЂСЏРјРѕРµ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµ
	#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	//#define DIRECT_27M0_X1		1	/* Board hardware configuration - see WITHSI5351AREPLACE */
	//#define DIRECT_100M0_X4		1	/* Board hardware configuration */
	#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	//#define BANDSELSTYLERE_UPCONV56M	1	/* 0.03..56 M */
	#define MODEL_DIRECT	1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїСЂСЏРјРѕР№ СЃРёРЅС‚РµР·, Р° РЅРµ РіРёР±СЂРёРґРЅС‹Р№ */
#elif 0
	/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РІРЅРёР·" СЃРёРЅС‚РµР·Р°С‚РѕСЂ MAXLAB */
	#define FQMODEL_UW3DI		1	// РџРµСЂРІР°СЏ (Рё РµРґРёРЅСЃС‚РІРµРЅРЅР°) РџР§ 500 РєР“С†
	//#define DIRECT_100M0_X4		1	/* Board hardware configuration */
	#define HYBRID_DDS_ATTINY2313 1
	//#define MAXLAB_7M2			1	/* Board hardware configuration */
	#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	//#define MODEL_MAXLAB	1	/* PLL СЃ СѓРІРѕРґРѕРј РѕРїРѕСЂРЅРѕРіРѕ РіРµРЅРµСЂР°С‚РѕСЂР° РІР°СЂРёРєР°РїРѕРј С‡РµСЂРµР· Р¦РђРџ */
	//#define MODEL_DIRECT	1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїСЂСЏРјРѕР№ СЃРёРЅС‚РµР·, Р° РЅРµ РіРёР±СЂРёРґРЅС‹Р№ */
	#define MODEL_HYBRID 1	/* Р“РёР±СЂРёРґРЅС‹Р№ СЏРІРЅРѕ С‚РµСЂР±СѓРµС‚СЃСЏ */
#elif 0
	/* RU6BK: 1-st if=10.7 MHz, 2-nd if=500k, fixed BFO */
	#define FQMODEL_10M7_500K		1	// РџРµСЂРІР°СЏ РЅРёР·РєР°СЏ РџР§ 8 РњР“С†, 6 РњР“С†, 5.5 РњР“С† Рё РґСЂСѓРіРёРµ
	#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	#define WITHFIXEDBFO	1	/* РџРµСЂРµРІРѕСЂРѕС‚ Р±РѕРєРѕРІС‹С… Р·Р° СЃС‡С‘С‚ 2-РіРѕ РіРµС‚СЂРѕРґРёРЅР° 10.2 MHz / 10.9 MHz */
	#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	#define MODEL_DIRECT	1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїСЂСЏРјРѕР№ СЃРёРЅС‚РµР·, Р° РЅРµ РіРёР±СЂРёРґРЅС‹Р№ */
	#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
#elif 1
	/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РІРЅРёР·" */
	#define FQMODEL_TRX8M		1	// РџРµСЂРІР°СЏ РЅРёР·РєР°СЏ РџР§ 8 РњР“С†, 6 РњР“С†, 5.5 РњР“С† Рё РґСЂСѓРіРёРµ
	//#define WITHCLASSICSIDE	1	/* СЃС‚РѕСЂРѕРЅР° РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° Р·Р°РІРёСЃРёС‚ РѕС‚ СЂР°Р±РѕС‡РµР№ С‡Р°СЃС‚РѕС‚С‹ */
	
	#if 0
		// Р’С‹Р±РѕСЂ РџР§
		//#define IF3_MODEL IF3_TYPE_5500
		//#define IF3_MODEL IF3_TYPE_5645	// Drake R-4C and Drake T-4XC (Drake Twins) - 5645 kHz
		#define IF3_MODEL IF3_TYPE_8000
		//#define IF3_MODEL IF3_TYPE_8215
		//#define IF3_MODEL IF3_TYPE_5250
		//#define IF3_MODEL IF3_TYPE_6000
		//#define IF3_MODEL IF3_TYPE_8868
		//#define IF3_MODEL IF3_TYPE_5000
		//#define IF3_MODEL IF3_TYPE_9000
		//#define IF3_MODEL IF3_TYPE_9045
		#define WITHLO4FREQPOWER2 1	/* СЃ СѓРїСЂР°РІР»РµРЅРёРµРј СѓРјРЅРѕР¶РёС‚РµР»РµРј РІРѕ РІС‚РѕСЂРѕРј РіРµС‚РµСЂРѕРґРёРЅРµ 8868 */
	#else
		//#define IF3DF (4607000L - 8865850L)
		#define IF3_CUSTOM_CW_CENTER	((7997557uL + 7998126uL) / 2)
		#define IF3_CUSTOM_SSB_LOWER	(7997184uL)
		#define IF3_CUSTOM_SSB_UPPER	(7999930uL)
		#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

		#undef IF3_MODEL
		#define IF3_MODEL	IF3_TYPE_CUSTOM

		#define LO1FDIV_ADJ	1		/* РµСЃР»Рё РѕРїСЂРµРґРµР»РµРЅРѕ - РїРѕРґСЃС‚СЂРѕР№РєР° РґРµР»РёС‚РµР»СЏ РІ С‚СЂР°РєС‚Рµ РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° СЂР°Р·СЂРµС€РµРЅР° С‡РµСЂРµР· РјРµРЅСЋ */
		#define LO1_POWER2	1		/* 8 - Р­С‚Рѕ РѕР±С‰РёРµРµ РґРµР»РµРЅРёРµ (4 - РІ СЃРјРµСЃРёС‚РµР»Рµ РўРџРџ, 2 - С„РѕСЂРјРёСЂРѕРІР°С‚РµР»СЊ РјРµР°РЅРґСЂР° РЅР° РІС‹С…РѕРґРµ СѓРїСЂР°РІР»СЏРµРјРѕРіРѕ РґРµР»РёС‚РµР»СЏ) */
		//#define LO1_POWER2	0		/* 0 - РЅРµС‚ РґРµР»РёС‚РµР»СЏ РїРѕСЃР»Рµ РіРµРЅРµСЂР°С‚РѕСЂР° LO1 РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РЅР° СЃРјРµСЃРёС‚РµР»СЊ */
		
		#define LO2_POWER2	0		/* 0 - РЅРµС‚ РґРµР»РёС‚РµР»СЏ РїРѕСЃР»Рµ РіРµРЅРµСЂР°С‚РѕСЂР° LO2 РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РЅР° СЃРјРµСЃРёС‚РµР»СЊ */

		#define WITHLO4FREQPOWER2 1	/* СЃ СѓРїСЂР°РІР»РµРЅРёРµРј СѓРјРЅРѕР¶РёС‚РµР»РµРј РІРѕ РІС‚РѕСЂРѕРј РіРµС‚РµСЂРѕРґРёРЅРµ 8868 */
	#endif

	// 6862 
	//#define DIRECT_39M00_X10_EXACT	1	/* SW2011 TRX scheme V.2 */
	//#define DIRECT_25M0_X1		1
	#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	//#define DIRECT_50M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_60M0_X6		1	/* Board hardware configuration */
	//#define DIRECT_48M0_X8		1	/* Board hardware configuration */
	//#define DIRECT_66M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_80M0_X5		1	/* Board hardware configuration */
	//#define DIRECT_DDS_ATTINY2313 1
	//#define HYBRID_DDS_ATTINY2313 1
	//#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
	//#define REQUEST_FOR_RN4NAB 1	/* РЎРїРµС†РёР°Р»СЊРЅС‹Р№ РІР°СЂРёР°РЅС‚ РґРёР°РїР°Р·РѕРЅРЅС‹С… С„РёР»СЊС‚СЂРѕРІ Рё blue bg */
	#define MODEL_DIRECT	1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїСЂСЏРјРѕР№ СЃРёРЅС‚РµР·, Р° РЅРµ РіРёР±СЂРёРґРЅС‹Р№ */
#elif 0
	/* РЎРїРµС†РёР°Р»СЊРЅС‹Рµ РІРµСЂСЃРёРё, РІРѕР·РјРѕР¶РЅРѕ Рё РЅРµ РїСЂРёС‘РјРЅРёРєРё. */
	//#define FQMODEL_GEN500	1	// CML evaluation board with CMX992 chip, 1-st IF = 45 MHz
	//#define FQMODEL_RFSG_SI570	1	// РЎРёРіРЅР°Р»-РіРµРЅРµСЂР°С‚РѕСЂ РЅР° Si570 "B" grade
	//#define MODEL_DIRECT	1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїСЂСЏРјРѕР№ СЃРёРЅС‚РµР·, Р° РЅРµ РіРёР±СЂРёРґРЅС‹Р№ */
#endif	
	// --- РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ СЂР°Р·РЅС‹С… РїР»Р°С‚


	#define CTLREGMODE32_V1	1		/* СѓРїСЂР°РІР»СЏСЋС‰РёР№ СЂРµРіРёСЃС‚СЂ - "Р’РѕСЂРѕР±РµР№-3" СЃ 3*ULN2003 */
	//#define CTLREGMODE24_V1	1		/* СѓРїСЂР°РІР»СЏСЋС‰РёР№ СЂРµРіРёСЃС‚СЂ - "Р’РѕСЂРѕР±РµР№" Рё "РљРѕР»РёР±СЂРё" */
	//#define CTLREGMODE16	1	// "СЃРёРЅС‚РµР·Р°С‚РѕСЂ СЃ С…РѕСЂРѕС€РёРј СЃРµСЂРІРёСЃРѕРј"

	#define WITHAGCMODE5STAGES 1
	#define WITHAGCMODEONOFF 1	/* СЃСЂРµРґРё СЂРµР¶РёРјРѕРІ РђР РЈ РµСЃС‚СЊ С‚РѕР»СЊРєРѕ РІРєР»СЋС‡РµРЅРѕ-РІС‹РєР»СЋС‡РµРЅРѕ */
	#define	BOARD_AGCCODE_ON		0x00
	#define	BOARD_AGCCODE_OFF		0x01

	#define WITHPREAMPATT2_6DB		1	// РЈРїСЂР°РІР»РµРЅРёРµ РЈР’Р§ Рё РґРІСѓС…РєР°СЃРєР°РґРЅС‹Рј Р°С‚С‚РµРЅСЋР°С‚РѕСЂРѕРј СЃ Р·Р°С‚СѓС…Р°РЅРёСЏРјРё 0 - 6 - 12 - 18 dB */
	//#define WITHPREAMPATT2_10DB		1	// РЈРїСЂР°РІР»РµРЅРёРµ РЈР’Р§ Рё РґРІСѓС…РєР°СЃРєР°РґРЅС‹Рј Р°С‚С‚РµРЅСЋР°С‚РѕСЂРѕРј СЃ Р·Р°С‚СѓС…Р°РЅРёСЏРјРё 0 - 10 - 20 - 30 dB */
	//#define WITHATT1PRE1		1	// РЈР’Р§ Рё РѕРґРЅРѕРєР°СЃРєР°РґРЅС‹Р№ Р°С‚С‚РµРЅСЋР°С‚РѕСЂ

	//#define CTLREGMODE24_IGOR	1	/* Р’Р°СЂРёР°РЅС‚ РґР»СЏ РРіРѕСЂСЏ - СѓРїСЂР°РІР»СЏСЋС‰РёР№ СЂРµРіРёСЃС‚СЂ 24 Р±РёС‚Р° - "Р’РѕСЂРѕР±РµР№" Рё "РљРѕР»РёР±СЂРё" */
	//#define CTLREGMODE16	1		/* СѓРїСЂР°РІР»СЏСЋС‰РёР№ СЂРµРіРёСЃС‚СЂ 16 Р±РёС‚	*/
	//#define CTLREGMODE24	1		/* СѓРїСЂР°РІР»СЏСЋС‰РёР№ СЂРµРіРёСЃС‚СЂ 24 Р±РёС‚Р°	*/
	//#define CTLREGMODE16_GEN500	1

	//#define DSTYLE_UR3LMZMOD	1	// Р Р°СЃРїРѕР»РѕР¶РµРЅРёРµ СЌР»РµРјРµРЅС‚РѕРІ СЌРєСЂР°РЅР° РІ С‚СЂР°РЅСЃРёРІРµСЂР°С… UR3LMZ
	#define	FONTSTYLE_ITALIC	1	// РСЃРїРѕР»СЊР·РѕРІР°С‚СЊ Р°Р»СЊС‚РµСЂРЅР°С‚РёРІРЅС‹Р№ С€СЂРёС„С‚
	//#define DSTYLE_SIMPLEFREQ	1	// РўРѕР»СЊРєРѕ РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ С‡Р°СЃС‚РѕС‚С‹ (Рё РїСЂСЏРјРѕР№ РІРІРѕРґ С‡Р°СЃС‚РѕС‚С‹)

	// +++ РћСЃРѕР±С‹Рµ РІР°СЂРёР°РЅС‚С‹ СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ РєРЅРѕРїРѕРє РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ
	//#define KEYB_RA1AGO	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ RA1AGO (РїСЂРёС‘РјРЅРёРє РІРѕСЂРѕРЅС‘РЅРѕРє СЃ СЃРёРјРІРѕР»СЊРЅС‹Рј РґРёСЃРїР»РµРµРј) */
	//#define KEYB_VERTICAL_REV	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ РїР»Р°С‚ "Р’РѕСЂРѕР±РµР№" Рё "РљРѕР»РёР±СЂРё" */
	//#define KEYB_VERTICAL_REV_SIMPLEFREQ	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ РїР»Р°С‚ "Р’РѕСЂРѕР±РµР№" Рё "РљРѕР»РёР±СЂРё" - С‚РѕР»СЊРєРѕ РїСЂСЏРјРѕР№ РІРІРѕРґ С‡Р°СЃС‚РѕС‚С‹ */
	#define KEYB_VERTICAL_REV_TOPDOWN	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ РџР•Р Р•Р’РЃР РќРЈРўР«РҐ РїР»Р°С‚ "Р’РѕСЂРѕР±РµР№" Рё "РљРѕР»РёР±СЂРё" */
	//#define KEYB_VERTICAL	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ РїР»Р°С‚ "РџР°РІР»РёРЅ" */
	//#define KEYB_V8S_DK1VS	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РґР»СЏ РєРѕРЅС‚СЂРѕР»Р»РµСЂР° DK1VS */
	//#define KEYB_VERTICAL_REV_RU6BK	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ РїР»Р°С‚ "Р’РѕСЂРѕР±РµР№" Рё "РљРѕР»РёР±СЂРё" */
	//#define KEYB_VERTICAL_REV_RU6BK_TOPDOWN	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ РїР»Р°С‚ "Р’РѕСЂРѕР±РµР№" Рё "РљРѕР»РёР±СЂРё" - Р”Р»СЏ РєРЅРѕРїРѕРє СЃРїСЂР°РІР° */
	//#define KEYBOARD_USE_ADC6	1	/* С€РµСЃС‚СЊ РєРЅРѕРїРѕРє РЅР° РєР°Р¶РґРѕРј РІС…РѕРґРµ ADCx */
	//#define KEYBOARD_USE_ADC6_V1	1	/* С€РµСЃС‚СЊ РєРЅРѕРїРѕРє РЅР° РєР°Р¶РґРѕРј РІС…РѕРґРµ ADCx */
	//#define KEYB_LADVABEST	1		/* 3 * 6 РєРЅРѕРїРѕРє ladvabest@gmail.com */
	//#define KEYB_M0SERG	1	/* СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ Serge Moisseyev */
	// --- РћСЃРѕР±С‹Рµ РІР°СЂРёР°РЅС‚С‹ СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ РєРЅРѕРїРѕРє РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ

	// +++ РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°
	//#define LCDMODE_HARD_SPI	1	/* LCD over SPI line */
	//#define LCDMODE_LTDC	1		/* Use STM32F4xxx with LCD-TFT Controller (LTDC) */
	//#define LCDMODE_LTDC_L8	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ 8 Р±РёС‚ РЅР° РїРёРєСЃРµР»СЊ РїСЂРµРґСЃС‚Р°РІР»РµРЅРёРµ СЌРєСЂР°РЅР°. РРЅР°С‡Рµ - 16 Р±РёС‚. */
	//#define LCDMODE_WH2002	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 20*2, РІРѕР·РјРѕР¶РЅРѕ РІРјРµСЃС‚Рµ СЃ LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 16*2 */
	//#define LCDMODE_WH1604	1	/* С‚РёРї РїСЂРёРјРµРЅСЏРµРјРѕРіРѕ РёРЅРґРёРєР°С‚РѕСЂР° 16*4 */
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
	//#define LCDMODE_L2F50_TOPDOWN	1	/* РџРµСЂРµРІРѕСЂРѕС‚ РёР·РѕР±СЂР°Р¶РµРЅРёСЏ РІ СЃР»СѓС‡Р°Рµ LCDMODE_L2F50 */
	//#define LCDMODE_S1D13781	1	/* РРЅРЅРґРёРєР°С‚РѕСЂ 480*272 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	//#define LCDMODE_ILI9225	1	/* РРЅРґРёРєР°С‚РѕСЂ 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9225РЎ */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃР»РµРІР° РѕС‚ СЌРєСЂР°РЅР°) */
	#define LCDMODE_UC1608	1		/* РРЅРґРёРєР°С‚РѕСЂ 240*128 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1608.- РјРѕРЅРѕС…СЂРѕРјРЅС‹Р№ */
	#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРІРµСЂС…Сѓ) */
	//#define LCDMODE_ST7735	1	/* РРЅРґРёРєР°С‚РѕСЂ 160*128 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Sitronix ST7735 - TFT РїР°РЅРµР»СЊ 160 * 128 HY-1.8-SPI */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРїСЂР°РІР°) */
	//#define LCDMODE_ST7565S	1	/* РРЅРґРёРєР°С‚РѕСЂ WO12864C2-TFH# 128*64 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРІРµСЂС…Сѓ) */
	//#define LCDMODE_ILI9320	1	/* РРЅРґРёРєР°С‚РѕСЂ 248*320 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9320 */
	//#define LCDMODE_ILI9341	1	/* 320*240 SF-TC240T-9370-T СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9341 - STM32F4DISCO */
	//#define LCDMODE_ILI9341_TOPDOWN	1	/* LCDMODE_ILI9341 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРїСЂР°РІР°) */
	//#define LCDMODE_ILI8961	1	/* 320 * 240, RGB, ILI8961 */
	//#define LCDMODE_ILI8961_TOPDOWN	1
	//#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	// --- РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define	WITHENCODER	1	/* РґР»СЏ РёР·РјРµРЅРµРЅРёСЏ С‡Р°СЃС‚РѕС‚С‹ РёРјРµРµС‚СЃСЏ РµРЅРєРѕРґРµСЂ */
	#define WITHSPLIT	1	/* СѓРїСЂР°РІР»РµРЅРёРµ СЂРµР¶РёРјР°РјРё СЂР°СЃСЃС‚СЂРѕР№РєРё РѕРґРЅРѕР№ РєРЅРѕРїРєРѕР№ */
	//#define WITHSPLITEX	1	/* РўСЂРµС…РєРЅРѕРїРѕС‡РЅРѕРµ СѓРїСЂР°РІР»РµРЅРёРµ СЂРµР¶РёРјР°РјРё СЂР°СЃСЃС‚СЂРѕР№РєРё */
	#define WITHCATEXT	1	/* Р Р°СЃС€РёСЂРµРЅРЅС‹Р№ РЅР°Р±РѕСЂ РєРѕРјР°РЅРґ CAT */
	#define WITHELKEY	1
	#define WITHKBDENCODER 1	// РїРµСЂРµСЃС‚СЂРѕР№РєР° С‡Р°СЃС‚РѕС‚С‹ РєРЅРѕРїРєР°РјРё
	#define WITHKEYBOARD 1	/* РІ РґР°РЅРЅРѕРј СѓСЃС‚СЂРѕР№СЃС‚РІРµ РµСЃС‚СЊ РєР»Р°РІРёР°С‚СѓСЂР° */
	#define KEYBOARD_USE_ADC	1	/* РЅР° РѕРґРЅРѕР№ Р»РёРЅРёРё СѓСЃС‚Р°РЅРѕРІР»РµРЅРѕ  С‡РµС‚С‹СЂРµ  РєР»Р°РІРёС€Рё. РЅР° vref - 6.8K, РґР°Р»РµРµ 2.2Рљ, 4.7Рљ Рё 13K. */
	//#define WITHNESTEDINTERRUPTS	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїСЂРё РЅР°Р»РёС‡РёРё real-time С‡Р°СЃС‚Рё. */

	// +++ Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ
	//#define WITHRFSG	1	/* РІРєР»СЋС‡РµРЅРѕ СѓРїСЂР°РІР»РµРЅРёРµ Р’Р§ СЃРёРіРЅР°Р»-РіРµРЅРµСЂР°С‚РѕСЂРѕРј. */
	#define WITHTX		1	/* РІРєР»СЋС‡РµРЅРѕ СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - СЃРёРєРІРµРЅСЃРѕСЂ, СЌР»РµРєС‚СЂРѕРЅРЅС‹Р№ РєР»СЋС‡. */
	//#define WITHAUTOTUNER	1	/* Р•СЃС‚СЊ С„СѓРЅРєС†РёСЏ Р°РІС‚РѕС‚СЋРЅРµСЂР° */
	//#define SHORTSET7	1	
	#define WITHIFSHIFT	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* РќР°С‡Р°Р»СЊРЅРѕРµ Р·Р°РЅС‡РµРЅРёРµ IF SHIFT */
	#define WITHPOTIFSHIFT	1		// РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂ СѓРїСЂР°РІР»РµРЅРёСЏ СЃРґРІРёРіРѕРј РїРѕР»РѕСЃС‹ РџР§

	//#define WITHPBT		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ PBT (РµСЃР»Рё LO3 РµСЃС‚СЊ) */
	//#define WITHPOTPBT		1		// РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂ СѓРїСЂР°РІР»РµРЅРёСЏ СЃСѓР¶РµРЅРёРµРј РїРѕР»РѕСЃС‹ РџР§

	#define WITHCAT		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ CAT */
	//#define WITHDEBUG		1	/* РћС‚Р»Р°РґРѕС‡РЅР°СЏ РїРµС‡Р°С‚СЊ С‡РµСЂРµР· COM-РїРѕСЂС‚. Р‘РµР· CAT (WITHCAT) */
	//#define WITHNMEA		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ NMEA parser */
	//#define WITHBEACON	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ СЂРµР¶РёРј РјР°СЏРєР° */
	//#define WITHVOX		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ VOX */
	#define WITHSHOWSWRPWR 1	/* РЅР° РґРёСЃРїР»РµРµ РѕРґРЅРѕРІСЂРµРјРµРЅРЅРѕ РѕС‚РѕР±СЂР°Р¶Р°СЋС‚СЃСЏ SWR-meter Рё PWR-meter */
	#define WITHSWRMTR	1	/* РР·РјРµСЂРёС‚РµР»СЊ РљРЎР’ */
	//#define WITHPWRMTR	1	/* РРЅРґРёРєР°С‚РѕСЂ РІС‹С…РѕРґРЅРѕР№ РјРѕС‰РЅРѕСЃС‚Рё РёР»Рё */
	//#define WITHPWRLIN	1	/* РРЅРґРёРєР°С‚РѕСЂ РІС‹С…РѕРґРЅРѕР№ РјРѕС‰РЅРѕСЃС‚Рё РїРѕРєР°Р·С‹РІР°РµС‚ РЅР°РїСЂСЏР¶РµРЅРёРµ Р° РЅРµ РјРѕС‰РЅРѕСЃС‚СЊ */
	#define WITHBARS	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ S-РјРµС‚СЂР° Рё SWR-РјРµС‚СЂР° */
	//#define WITHVOLTLEVEL	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ РЅР°РїСЂСЏР¶РµРЅРёСЏ РђРљР‘ */
	//#define WITHSWLMODE	1	/* РїРѕРґРґРµСЂР¶РєР° Р·Р°РїРѕРјРёРЅР°РЅРёСЏ РјРЅРѕР¶РµСЃС‚РІР° С‡Р°СЃС‚РѕС‚ РІ swl-mode */
	#define WITHPOTWPM		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ СЂРµРіСѓР»РёСЂРѕРІРєР° СЃРєРѕСЂРѕСЃС‚Рё РїРµСЂРµРґР°С‡Рё РІ С‚РµР»РµРіСЂР°С„Рµ РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂРѕРј */
	#define WITHVIBROPLEX	1	/* РІРѕР·РјРѕР¶РЅРѕСЃС‚СЊ СЌРјСѓР»СЏС†РёРё РїРµСЂРµРґР°С‡Рё РІРёР±СЂРѕРїР»РµРєСЃРѕРј */
	//#define WITHSUBTONES	1	/* РІС‹РїРѕР»РЅСЏРµС‚СЃСЏ С„РѕСЂРјРёСЂРѕРІР°РЅРёРµ СЃСѓР±С‚РѕРЅР° РїСЂРё РїРµСЂРµРґР°С‡Рµ NFM */
	#define WITHMENU 	1	/* С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РјРµРЅСЋ РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕС‚РєР»СЋС‡РµРЅР° - РµСЃР»Рё РЅР°СЃС‚СЂР°РёРІР°С‚СЊ РЅРµС‡РµРіРѕ */

	//#define WITHFIXEDBFO	1	/* РџРµСЂРµРІРѕСЂРѕС‚ Р±РѕРєРѕРІС‹С… Р·Р° СЃС‡С‘С‚ 1-РіРѕ РіРµС‚СЂРѕРґРёРЅР° (РѕСЃРѕР±РµРЅРЅРѕ, РµСЃР»Рё РЅРµС‚ РїРѕРґСЃС‚СЂР°РёРІР°РµРјРѕРіРѕ BFO) */
	//#define WITHDUALBFO		1	/* РџРµСЂРµРІРѕСЂРѕС‚ Р±РѕРєРѕРІС‹С… Р·Р° СЃС‡С‘С‚ РїРµСЂРµРєР»СЋС‡РµРЅРёСЏ С‡Р°СЃС‚РѕС‚С‹ BFO РІРЅРµС€РЅРёРј СЃРёРіРЅР°Р»РѕРј */
	//#define WITHDUALFLTR	1	/* РџРµСЂРµРІРѕСЂРѕС‚ Р±РѕРєРѕРІС‹С… Р·Р° СЃС‡С‘С‚ РїРµСЂРµРєР»СЋС‡РµРЅРёСЏ С„РёР»СЊС‚СЂР° РІРµСЂС…РЅРµР№ РёР»Рё РЅРёР¶РЅРµР№ Р±РѕРєРѕРІРѕР№ РїРѕР»РѕСЃС‹ */
	//#define WITHANTSELECT	1	// РЈРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРєР»СЋС‡РµРЅРёРµРј Р°РЅС‚РµРЅРЅ

	#define WITHSAMEBFO	1	/* РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РѕР±С‰РёС… РЅР°СЃС‚СЂРѕРµРє BFO РґР»СЏ РїСЂРёС‘РјР° Рё РїРµСЂРµРґР°С‡Рё */
	//#define WITHONLYBANDS 1		/* РџРµСЂРµСЃС‚СЂРѕР№РєР° РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕРіСЂР°РЅРёС‡РµРЅР° Р»СЋР±РёС‚РµР»СЊСЃРєРёРјРё РґРёР°РїР°Р·РѕРЅР°РјРё */
	//#define WITHBCBANDS	1		/* РІ С‚Р°Р±Р»РёС†Рµ РґРёР°РїР°Р·РѕРЅРѕРІ РїСЂРёСЃСѓС‚СЃС‚РІСѓСЋС‚ РІРµС‰Р°С‚РµР»СЊРЅС‹Рµ РґРёР°РїР°Р·РѕРЅС‹ */
	#define WITHWARCBANDS	1	/* Р’ С‚Р°Р±Р»РёС†Рµ РґРёР°РїР°Р·РѕРЅРѕРІ РїСЂРёСЃСѓС‚СЃС‚РІСѓСЋС‚ HF WARC РґРёР°РїР°Р·РѕРЅС‹ */
	//#define WITHLO1LEVELADJ		1	/* РІРєР»СЋС‡РµРЅРѕ СѓРїСЂР°РІР»РµРЅРёРµ СѓСЂРѕРІРЅРµРј (Р°РјРїР»РёС‚СѓРґРѕР№) LO1 */
	//#define WITHLO1LEVELADJINITIAL	60	/* РќР°С‡Р°Р»СЊРЅРѕРµ РїРѕР»РѕР¶РµРЅРёРµ СЂРµРіСѓР»РёСЂРѕРІРєРё СѓСЂРѕРІРЅСЏ LO1 */
	//#define WITHLFM		1	/* LFM MODE */
	//#define LFMTICKSFREQ 1000
	//#define WITHTEMPSENSOR	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ РґР°РЅРЅС‹С… СЃ РґР°С‚С‡РёРєР° С‚РµРјРїРµСЂР°С‚СѓСЂС‹ */
	//#define WITHDIRECTBANDS 1	/* РџСЂСЏРјРѕР№ РїРµСЂРµС…РѕРґ Рє РґРёР°РїР°Р·РѕРЅР°Рј РїРѕ РЅР°Р¶Р°С‚РёСЏРј РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ */
	//#define WITHIF4DSP	1	// "Р”СЏС‚РµР»"
	//#define DEFAULT_DSP_IF	12000
	//#define WITHEXTERNALDDSP		1	/* РёРјРµРµС‚СЃСЏ СѓРїСЂР°РІР»РµРЅРёРµ РІРЅРµС€РЅРµР№ DSP РїР»Р°С‚РѕР№. */
	//#define WITHSLEEPTIMER	1	/* РІС‹РєР»СЋС‡РёС‚СЊ РёРЅРґРёРєР°С‚РѕСЂ Рё РІС‹РІРѕРґ Р·РІСѓРєР° РїРѕ РёСЃС‚РµС‡РµРЅРёРёРё СѓРєР°Р·Р°РЅРЅРѕРіРѕ РІСЂРµРјРµРЅРё */
	// --- Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ

	#if 0
		#define WITHBBOX	1	// Black Box mode - СѓСЃС‚СЂРѕР№СЃС‚РІРѕ Р±РµР· РѕСЂРіР°РЅРѕРІ СѓРїСЂР°РІР»РµРЅРёСЏ
		#define	WITHBBOXFREQ	14063710L		// С‡Р°СЃС‚РѕС‚Р° РїРѕСЃР»Рµ РІРєР»СЋС‡РµРЅРёСЏ
		#define	WITHBBOXSUBMODE	SUBMODE_CW	// РµРґРёРЅСЃС‚РІРµРЅРЅС‹Р№ СЂРµР¶РёРј СЂР°Р±РѕС‚С‹
		#define	WITHBBOXTX		1		// Р°РІС‚РѕРјР°С‚РёС‡РµСЃРєРёР№ РїРµСЂРµС…РѕРґ РЅР° РїРµСЂРµРґР°С‡Сѓ
	#endif

	//#define LO1PHASES	1		/* РџСЂСЏРјРѕР№ СЃРёРЅС‚РµР· РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° РґРІСѓРјСЏ DDS СЃ РїСЂРѕРіСЂР°РјРёРјСЂСѓРµРјС‹Рј СЃРґРІРёРіРѕРј С„Р°Р·С‹ */
	#define DEFPREAMPSTATE 	1	/* РЈР’Р§ РїРѕ СѓРјРѕР»С‡Р°РЅРёСЋ РІРєР»СЋС‡С‘РЅ (1) РёР»Рё РІС‹РєР»СЋС‡РµРЅ (0) */

	//#define HARDWARE_IGNORENONVRAM	1		// РѕС‚Р»Р°РґРєР° РЅР° РїР»Р°С‚Р°С… РіРґРµ РЅРµС‚ РЅРёРєР°РєРѕРіРѕ NVRAM
	/* С‡С‚Рѕ Р·Р° РїР°РјСЏС‚СЊ РЅР°СЃС‚СЂРѕРµРє Рё С‡Р°СЃС‚РѕС‚ РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІ РєРѕРЅС‚СЂРѕР»Р»РµСЂРµ */
	#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// РўР°Рє Р¶Рµ РїСЂРё РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРё FM25040A - 5 РІРѕР»СЊС‚, 512 Р±Р°Р№С‚
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16		// demo board with atxmega128a4u - 25LC160A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* СЂР°Р·СЂСЏРґРЅРѕСЃС‚СЊ FTW РІС‹Р±СЂР°РЅРЅРѕРіРѕ DDS */

	/* Board hardware configuration */
	//#define PLL1_TYPE PLL_TYPE_SI5351A
	//#define PLL1_FRACTIONAL_LENGTH	10	/* Si5351a: lower 10 bits is a fractional part */
	//#define PLL2_TYPE PLL_TYPE_SI5351A
	//#define PLL2_FRACTIONAL_LENGTH	10	/* Si5351a: lower 10 bits is a fractional part */
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9852
	//#define DDS1_TYPE DDS_TYPE_AD9851
	#define DDS2_TYPE DDS_TYPE_AD9834
	//#define DDS2_TYPE DDS_TYPE_AD9851
	//#define PLL2_TYPE PLL_TYPE_CMX992
	//#define DDS3_TYPE DDS_TYPE_AD9834

	//#define DDS1_TYPE DDS_TYPE_ATTINY2313
	//#define PLL1_TYPE PLL_TYPE_LM7001
	//#define targetpll1 SPI_CSEL1

	//#define WITHSI5351AREPLACE 1

	//#define RTC1_TYPE RTC_TYPE_M41T81	/* ST M41T81M6 RTC clock chip with I2C interface */
	//#define RTC1_TYPE RTC_TYPE_DS1307	/* MAXIM DS1307/DS3231 RTC clock chip with I2C interface */

	#define DDS1_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 */
	#define DDS2_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS2 */
	#define DDS3_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS3 */

	///#define DDS1_TYPE DDS_TYPE_AD9834
	///#define targetdds1 SPI_CSEL1 	/* DDS1 - LO1 output - СЂР°Р±РѕС‚Р°РµС‚ AD9834 (Vasiliy, Ufa). */
	/* РќР°Р·РЅР°С‡РµРЅРёРµ Р°РґСЂРµСЃРѕРІ РЅР° SPI С€РёРЅРµ */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	//#define targetdds3 SPI_CSEL1 	/* DDS2 - LO2 output - RU6NFK with FQMODEL_10M7_500K */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	#define targetdds3 SPI_CSEL2 	/* DDS3 - PBT output */
	//#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. Р”Р»СЏ РґРІРѕР№РЅРѕР№ DDS РїРµСЂРІРѕРіРѕ РіРµС‚РµСЂРѕРґРёРЅР° - РІС‚РѕСЂР°СЏ DDS */
	//#define targetpll2 SPI_CSEL3 	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetext1 SPI_CSEL4 	/* external devices control */
#ifndef SPI_IOUPDATE_BIT 
	#define targetupd1 SPI_CSEL5 	/* DDS IOUPDATE signals at output of SPI chip-select register */
#endif
	#define targetctl1 SPI_CSEL6 	/* control register */
	#define targetnvram SPI_CSEL7  	/* serial nvram */

	#define targetlcd targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/
	#define targetdsp1 targetext1 	/* external devices control */

	#define targetcodec2 SPI_CSEL2 	/* PD1 CS4272 */
	#define targetcodec1 SPI_CSEL3 	/* PD3 TLV320AIC23B */

	/* РєРѕРґС‹ С„РёР»СЊС‚СЂРѕРІ РІС‚РѕСЂРѕР№ РџР§, РІС‹РґР°РІР°РµРјС‹Рµ РЅР° РґРµС€РёС„СЂР°С‚РѕСЂС‹ */
	#define BOARD_FILTER_3P1		BOARD_FILTERCODE_0	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_0P5		BOARD_FILTERCODE_1	/* 0.5 or 0.3 kHz filter */
	//#define BOARD_FILTER_1P8		BOARD_FILTERCODE_1	/* 1.8 kHz filter - РЅР° РѕС‡РµРЅСЊ СЃС‚Р°СЂС‹С… РїР»Р°С‚Р°С… */
	//#define BOARD_FILTER_2P7		BOARD_FILTERCODE_1	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_6P0		BOARD_FILTERCODE_2	/* 6.0 kHz filter */

	#define BOARD_FILTER_15P0		BOARD_FILTERCODE_2	/* 6.0 kHz filter */
	//#define BOARD_FILTER_15P0_NFM		BOARD_FILTERCODE_2	/* 6.0 kHz filter */



	//#define WITHMODESETSMART 1	/* РІ Р·Р°РІР°РёСЃРјРѕСЃС‚Рё РѕС‚ С‡Р°СЃС‚РѕС‚С‹ РјРµРЅСЏРµС‚СЃСЏ Р±РѕРєРѕРІР°СЏ, РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІРјРµСЃС‚Рµ СЃ WITHFIXEDBFO */
	#define WITHMODESETFULL 1
	//#define WITHMODESETMIXONLY3	1	// USB, LSB, CW, CWR, DGU, DGL, CWZ
	//#define WITHMODESETFULLNFM 1
	//#define WITHDEDICATEDNFM 1		/* Р”Р»СЏ NFM РѕС‚РґРµР»СЊРЅС‹Р№ С‚СЂР°РєС‚ СЃРѕ СЃРІРѕРёРј С„РёР»СЊС‚СЂРѕРј */
	//#define WITHWFM	1			/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ WFM */
	/* РІСЃРµ РІРѕР·РјРѕР¶РЅС‹Рµ РІ РґР°РЅРЅРѕР№ РєРѕРЅС„РёРіСѓСЂР°С†РёРё С„РёР»СЊС‚СЂС‹ */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1/* | IF3_FMASK_6P0 | IF3_FMASK_15P0*/)
	/* РІСЃРµ РІРѕР·РјРѕР¶РЅС‹Рµ РІ РґР°РЅРЅРѕР№ РєРѕРЅС„РёРіСѓСЂР°С†РёРё С„РёР»СЊС‚СЂС‹ РґР»СЏ РїРµСЂРµРґР°С‡Рё */
	#define IF3_FMASKTX	(IF3_FMASK_3P1 /*| IF3_FMASK_15P0*/)
	/* С„РёР»СЊС‚СЂС‹, РґР»СЏ РєРѕС‚РѕСЂС‹С… СЃС‚РѕРёС‚ РїСЂРёР·РЅР°Рє HAVE */
	#define IF3_FHAVE	(IF3_FMASK_0P5 |  IF3_FMASK_3P1/* | IF3_FMASK_6P0 | IF3_FMASK_15P0*/)



	// РќР°Р·РЅР°С‡РµРЅРёСЏ РІС…РѕРґРѕРІ РђР¦Рџ РїСЂРѕС†РµСЃСЃРѕСЂР°.
	enum 
	{ 
	#if WITHTEMPSENSOR
		TEMPIX = 16,		// ADC1->CR2 |= ADC_CR2_TSVREFE;	// РґР»СЏ С‚РµСЃС‚РѕРІ
	#endif
	#if WITHVOLTLEVEL 
		VOLTSOURCE = 4,		// РЎСЂРµРґРЅСЏСЏ С‚РѕС‡РєР° РґРµР»РёС‚РµР»СЏ РЅР°РїСЂСЏР¶РµРЅРёСЏ, РґР»СЏ РђРљР‘
	#endif /* WITHVOLTLEVEL */
	#if WITHBARS
		SMETERIX = 0,	// S-meter
	#endif /* WITHBARS */
	#if WITHVOX
		VOXIX = 2, AVOXIX = 1,	// VOX
	#endif /* WITHVOX */
	#if WITHPOTPBT
		POTPBT = 2,
	#endif /* WITHPOTPBT */
	#if WITHPOTIFSHIFT
		POTIFSHIFT = 1,
	#endif /* WITHPOTIFSHIFT */
	#if WITHPOTWPM
		POTWPM = 2,
	#endif /* WITHPOTWPM */
	#if WITHSWRMTR
		PWRI = 4,
		FWD = 4, REF = 3,	// SWR-meter
	#endif /* WITHSWRMTR */
		KI0 = 5, KI1 = 6, KI2 = 7	// РєР»Р°РІРёР°С‚СѓСЂР°
	};

	#define KI_COUNT 3	// РєРѕР»РёС‡РµСЃС‚РІРѕ РёСЃРїРѕР»СЊР·СѓРµРјС‹С… РїРѕРґ РєР»Р°РІРёР°С‚СѓСЂСѓ РІС…РѕРґРѕРІ РђР¦Рџ
	#define KI_LIST	KI2, KI1, KI0,	// РёРЅРёС†РёР°Р»РёР·Р°С‚РѕСЂС‹ РґР»СЏ С„СѓРЅРєС†РёРё РїРµСЂРµРєРѕРґРёСЂРѕРІРєРё

	#define VOLTLEVEL_UPPER		47	// 4.7 kOhm - РІРµСЂС…РЅРёР№ СЂРµР·РёСЃС‚РѕСЂ РґРµР»РёС‚РµР»СЏ РґР°С‚С‡РёРєР° РЅР°РїСЂСЏР¶РµРЅРёСЏ
	#define VOLTLEVEL_LOWER		10	// 1.0 kOhm - РЅРёР¶РЅРёР№ СЂРµР·РёСЃС‚РѕСЂ

#endif /* ATMEGA644_CTLSTYLE_V1_H_INCLUDED */
