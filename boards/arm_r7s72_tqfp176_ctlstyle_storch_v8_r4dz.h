/* $Id$ */
/* board-specific CPU attached signals */
//
// РџСЂРѕРµРєС‚ HF Dream Receiver (РљР’ РїСЂРёС‘РјРЅРёРє РјРµС‡С‚С‹)
// Р°РІС‚РѕСЂ Р“РµРЅР° Р—Р°РІРёРґРѕРІСЃРєРёР№ mgs2001@mail.ru
// UA1ARN
//
// rmainunit_v5km2.pcb rmainunit_v5km3
// R7S721020VCFP, TFT 4.3" or 7", 2xUSB, NAU8822L Рё FPGA EP4CE22E22I7N, AD9246BCPZ-125

#ifndef ARM_R7S72_TQFP176_CTLSTYLE_STORCH_V8_R4DZ_H_INCLUDED
#define ARM_R7S72_TQFP176_CTLSTYLE_STORCH_V8_R4DZ_H_INCLUDED 1

	#define BOOTLOADER_APPAREA 0x20000000	/* Р°РґСЂРµСЃ РћР—РЈ, РєСѓРґР° РїРµСЂРµРјРµР·Р°С‚СЊ application */
	#define BOOTLOADER_APPFULL (1024uL * 2048)	// 2MB

	#define BOOTLOADER_SELFBASE 0x18000000	/* Р°РґСЂРµСЃ РіРґРµ Р»РµР¶РёС‚ РІРѕ FLASH РѕР±СЂР°Р· application */
	#define BOOTLOADER_SELFSIZE (1024uL * 128)	// 128

	#define BOOTLOADER_APPBASE 0x18020000	/* Р°РґСЂРµСЃ РіРґРµ Р»РµР¶РёС‚ РІРѕ FLASH РѕР±СЂР°Р· application */
	#define BOOTLOADER_APPSIZE (BOOTLOADER_APPFULL - BOOTLOADER_SELFSIZE)	// 2048 - 128

	#define BOOTLOADER_PAGESIZE (1024uL * 64)	// M25Px with 64 KB pages

	//#define WITHSAICLOCKFROMI2S 1	/* Р‘Р»РѕРє SAI1 С‚Р°РєС‚РёСЂСѓРµС‚СЃСЏ РѕС‚ PLL I2S */
	#define WITHI2SCLOCKFROMPIN 1	// С‚Р°РєС‚РѕРІР°СЏ С‡Р°СЃС‚РѕС‚Р° РЅР° SPI2 (I2S) РїРѕРґР°РµС‚СЃСЏ СЃ РІРЅРµС€РЅРµРіРѕ РіРµРЅРµСЂР°С‚РѕСЂР°, РІ РїСЂРѕС†РµСЃСЃРѕСЂ РІРІРѕРґРёС‚СЃСЏ С‡РµСЂРµР· AUDIO_X1 СЃРёРіРЅР°Р» РёРЅС‚РµСЂС„РµР№СЃР°
	#define WITHSAICLOCKFROMPIN 1	// С‚Р°РєС‚РѕРІР°СЏ С‡Р°СЃС‚РѕС‚Р° РЅР° SAI1 РїРѕРґР°РµС‚СЃСЏ СЃ РІРЅРµС€РЅРµРіРѕ РіРµРЅРµСЂР°С‚РѕСЂР°, РІ РїСЂРѕС†РµСЃСЃРѕСЂ РІРІРѕРґРёС‚СЃСЏ С‡РµСЂРµР· AUDIO_X1 СЃРёРіРЅР°Р» РёРЅС‚РµСЂС„РµР№СЃР°

	#define WITHCPUXTAL 12000000uL			/* РќР° РїСЂРѕС†РµСЃСЃРѕСЂРµ СѓСЃС‚Р°РЅРѕРІР»РµРЅ РєРІР°СЂС† 12.000 РњР“С† */

	#if WITHI2SCLOCKFROMPIN
		#define FPGADECIMATION 2560
		#define FPGADIVIDERATIO 5
		#define EXTI2S_FREQ (REFERENCE_FREQ * DDS1_CLK_MUL / FPGADIVIDERATIO)
		#define EXTSAI_FREQ (REFERENCE_FREQ * DDS1_CLK_MUL / FPGADIVIDERATIO)

		#define ARMI2SMCLK	(REFERENCE_FREQ * DDS1_CLK_MUL / (FPGADECIMATION / 256))
		#define ARMSAIMCLK	(REFERENCE_FREQ * DDS1_CLK_MUL / (FPGADECIMATION / 256))
	#else /* WITHI2SCLOCKFROMPIN */
		#define PLLI2SN_MUL 336		// 344.064 (192 <= PLLI2SN <= 432)
		#define SAIREF1_MUL 240		// 245.76 / 1.024 = 240 (49 <= PLLSAIN <= 432)
		// Р§Р°СЃС‚РѕС‚Р° С„РѕСЂРјРёСЂСѓРµС‚СЃСЏ РїСЂРѕС†РµСЃСЃРѕСЂРѕРј
		#define ARMI2SMCLK	(12288000uL)
		#define ARMSAIMCLK	(12288000uL)
	#endif /* WITHI2SCLOCKFROMPIN */

	/* РјРѕРґРµР»Рё СЃРёРЅС‚РµР·Р°С‚РѕСЂРѕРІ - СЃС…РµРјС‹ С‡Р°СЃС‚РѕС‚РѕРѕР±СЂР°Р·РѕРІР°РЅРёСЏ */

	/* Р’РµСЂСЃРёРё С‡Р°СЃС‚РѕС‚РЅС‹С… СЃС…РµРј - СЃ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРµРј "РЅР°РІРµСЂС…" */
	//#define FQMODEL_45_IF8868_UHF430	1	// SW2011
	//#define FQMODEL_73050		1	// 1-st IF=73.050, 2-nd IF=0.455 MHz
	//#define FQMODEL_73050_IF0P5		1	// 1-st IF=73.050, 2-nd IF=0.5 MHz
	//#define FQMODEL_80455		1	// 1-st IF=80.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455_IF0P5		1	// 1-st IF=64.455, 2-nd IF=0.5 MHz
	//#define FQMODEL_70455		1	// 1-st IF=70.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	//#define FQMODEL_60700_IF05	1	// 60.7 -> 10.7 -> 0.5
	//#define FQMODEL_60725_IF05	1	// 60.725 -> 10.725 -> 0.5
	//#define FQMODEL_60700_IF02	1	// 60.7 -> 10.7 -> 0.2
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz

	#define FQMODEL_FPGA		1	// FPGA + IQ over I2S
	#if 0
		#define DIRECT_80M0_X1		1	/* РўР°РєС‚РѕРІС‹Р№ РіРµРЅРµСЂР°С‚РѕСЂ РЅР° РїР»Р°С‚Рµ 80.0 РњР“С† */	
		#define BANDSELSTYLERE_UPCONV56M_36M	1	/* Up-conversion with working band .030..36 MHz */
	#elif 0
		#define DIRECT_100M0_X1		1	/* РўР°РєС‚РѕРІС‹Р№ РіРµРЅРµСЂР°С‚РѕСЂ РЅР° РїР»Р°С‚Рµ 100.0 РњР“С† */	
		#define BANDSELSTYLERE_UPCONV56M_45M	1	/* Up-conversion with working band .030..45 MHz */
	#else
		//#define DIRECT_125M0_X1		1	/* РўР°РєС‚РѕРІС‹Р№ РіРµРЅРµСЂР°С‚РѕСЂ РЅР° РїР»Р°С‚Рµ 125.0 РњР“С† */	
		#define DIRECT_122M88_X1	1	/* РўР°РєС‚РѕРІС‹Р№ РіРµРЅРµСЂР°С‚РѕСЂ 122.880 РњР“С† */	
		#define BANDSELSTYLERE_UPCONV56M	1	/* Up-conversion with working band .030..56 MHz */
	#endif
	// --- РІР°СЂРёР°С†РёРё РїСЂРѕС€РёРІРєРё, СЃРїРµС†РёС„РёС‡РµСЃРєРёРµ РґР»СЏ СЂР°Р·РЅС‹С… РїР»Р°С‚

	#define WITHPREAMPATT2_6DB		1	// РЈРїСЂР°РІР»РµРЅРёРµ РЈР’Р§ Рё РґРІСѓС…РєР°СЃРєР°РґРЅС‹Рј Р°С‚С‚РµРЅСЋР°С‚РѕСЂРѕРј СЃ Р·Р°С‚СѓС…Р°РЅРёСЏРјРё 0 - 6 - 12 - 18 dB */
	#define DEFPREAMPSTATE 	0	/* РЈР’Р§ РїРѕ СѓРјРѕР»С‡Р°РЅРёСЋ РІРєР»СЋС‡С‘РЅ (1) РёР»Рё РІС‹РєР»СЋС‡РµРЅ (0) */
	//#define WITHATT2_6DB		1		// LTC2217 or AD9246 РЈРїСЂР°РІР»РµРЅРёРµ РґРІСѓС…РєР°СЃРєР°РґРЅС‹Рј Р°С‚С‚РµРЅСЋР°С‚РѕСЂРѕРј СЃ Р·Р°С‚СѓС…Р°РЅРёСЏРјРё 0 - 6 - 12 - 18 dB Р±РµР· РЈР’Р§

	#define WITHAGCMODEONOFF	1	// РђР РЈ РІРєР»/РІС‹РєР»
	#define	WITHMIC1LEVEL		1	// СѓСЃС‚Р°РЅРѕРІРєР° СѓСЃРёР»РµРЅРёСЏ РјРёРєСЂРѕС„РѕРЅР°

	//#define DSTYLE_UR3LMZMOD	1	// Р Р°СЃРїРѕР»РѕР¶РµРЅРёРµ СЌР»РµРјРµРЅС‚РѕРІ СЌРєСЂР°РЅР° РІ С‚СЂР°РЅСЃРёРІРµСЂР°С… UR3LMZ
	#define	FONTSTYLE_ITALIC	1	// РСЃРїРѕР»СЊР·РѕРІР°С‚СЊ Р°Р»СЊС‚РµСЂРЅР°С‚РёРІРЅС‹Р№ С€СЂРёС„С‚

	// +++ РћСЃРѕР±С‹Рµ РІР°СЂРёР°РЅС‚С‹ СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ РєРЅРѕРїРѕРє РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ
	#define KEYB_FPAMEL20_V0A	1	/* 20 РєРЅРѕРїРѕРє РЅР° 5 Р»РёРЅРёР№ - РїР»Р°С‚Р° rfrontpanel_v0 + LCDMODE_UC1608 РІ РЅРѕСЂРјР°Р»СЊРЅРѕРј СЂР°СЃРїРѕР»РѕР¶РµРЅРёРё */
	//#define KEYB_RAVEN20_V5	1		/* 5 Р»РёРЅРёР№ РєР»Р°РІРёР°С‚СѓСЂС‹: СЂР°СЃРїРѕР»РѕР¶РµРЅРёРµ РєРЅРѕРїРѕРє РґР»СЏ Р’РѕСЂРѕР±РµР№ СЃ DSP РѕР±СЂР°Р±РѕС‚РєРѕР№ */
	// --- РћСЃРѕР±С‹Рµ РІР°СЂРёР°РЅС‚С‹ СЂР°СЃРїРѕР»РѕР¶РµРЅРёСЏ РєРЅРѕРїРѕРє РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ
	#define WITHSPLIT	1	/* СѓРїСЂР°РІР»РµРЅРёРµ СЂРµР¶РёРјР°РјРё СЂР°СЃСЃС‚СЂРѕР№РєРё РѕРґРЅРѕР№ РєРЅРѕРїРєРѕР№ */
	//#define WITHSPLITEX	1	/* РўСЂРµС…РєРЅРѕРїРѕС‡РЅРѕРµ СѓРїСЂР°РІР»РµРЅРёРµ СЂРµР¶РёРјР°РјРё СЂР°СЃСЃС‚СЂРѕР№РєРё */

	// +++ РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°
#if WITHISBOOTLOADER

	#define LCDMODE_ILI9341	1	/* 320*240 SF-TC240T-9370-T СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9341 - STM32F4DISCO */

#elif 0

	#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */

	#define LCDMODE_LTDC		1	/* Use VDC5 peripherial block */
	#define LCDMODE_LTDC_L8		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ 8 Р±РёС‚ РЅР° РїРёРєСЃРµР»СЊ РїСЂРµРґСЃС‚Р°РІР»РµРЅРёРµ СЌРєСЂР°РЅР°. РРЅР°С‡Рµ - 16 Р±РёС‚. */
	#define LCDMODE_LTDC_PIP16	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ PIP СЃ С„РѕСЂРјР°С‚РѕРј 16 Р±РёС‚ - RGB565 */

	//#define LCDMODE_LTDC_L24	1	/* 32-bit РЅР° РїРёРєСЃРµР»СЊ РІ РїР°РјСЏС‚Рё (3 Р±Р°Р№С‚Р°) */

#elif 1

	#define LCDMODE_AT070TN90 1	/* AT070TN90 panel (800*480) - 7" display */

	#define LCDMODE_LTDC		1	/* Use VDC5 peripherial block */
	#define LCDMODE_LTDC_L8		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ 8 Р±РёС‚ РЅР° РїРёРєСЃРµР»СЊ РїСЂРµРґСЃС‚Р°РІР»РµРЅРёРµ СЌРєСЂР°РЅР°. РРЅР°С‡Рµ - 16 Р±РёС‚. */
	#define LCDMODE_LTDC_PIP16	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ PIP СЃ С„РѕСЂРјР°С‚РѕРј 16 Р±РёС‚ - RGB565 */

	//#define LCDMODE_LTDC_L24	1	/* 32-bit РЅР° РїРёРєСЃРµР»СЊ РІ РїР°РјСЏС‚Рё (3 Р±Р°Р№С‚Р°) */

#elif 0

	#define LCDMODE_AT070TNA2 1	/* AT070TNA2 panel (1024*600) - 7" display */

	#define LCDMODE_LTDC		1	/* Use VDC5 peripherial block */
	#define LCDMODE_LTDC_L8		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ 8 Р±РёС‚ РЅР° РїРёРєСЃРµР»СЊ РїСЂРµРґСЃС‚Р°РІР»РµРЅРёРµ СЌРєСЂР°РЅР°. РРЅР°С‡Рµ - 16 Р±РёС‚. */
	#define LCDMODE_LTDC_PIP16	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ PIP СЃ С„РѕСЂРјР°С‚РѕРј 16 Р±РёС‚ - RGB565 */

	//#define LCDMODE_LTDC_L24	1	/* 32-bit РЅР° РїРёРєСЃРµР»СЊ РІ РїР°РјСЏС‚Рё (3 Р±Р°Р№С‚Р°) */

#elif 0

	#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	#define LCDMODE_S1D13781	1	/* РРЅРЅРґРёРєР°С‚РѕСЂ 480*272 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Epson S1D13781 */
	//#define LCDMODE_S1D13781_NHWACCEL 1	/* РќРµРёСЃРїРѕР»СЊР·РѕРІР°РЅРµ Р°РїРїР°СЂР°С‚РЅС‹С… РѕСЃРѕР±РµРЅРЅРѕСЃС‚РµР№ EPSON S1D13781 РїСЂРё РІС‹РІРѕРґРµ РіСЂР°С„РёРєРё */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	#define LCDMODE_S1D13781_REFOSC_MHZ	50	/* Р§Р°СЃС‚РѕС‚Р° РіРµРЅРµСЂР°С‚РѕСЂР°, СѓСЃС‚Р°РЅРѕРІР»РµРЅРЅРѕРіРѕ РЅР° РєРѕРЅС‚СЂРѕР»Р»РµСЂРµ РґРёСЃРїР»РµСЏ */

#elif 0

	// not tested
	#define LCDMODE_ILI8961	1	/* 320 * 240 HHT270C-8961-6A6, RGB, ILI8961, use LCDMODE_LTDC_L24 */
	#define LCDMODE_LTDC		1	/* Use VDC5 peripherial block */
	#define LCDMODE_LTDC_L8		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ 8 Р±РёС‚ РЅР° РїРёРєСЃРµР»СЊ РїСЂРµРґСЃС‚Р°РІР»РµРЅРёРµ СЌРєСЂР°РЅР°. РРЅР°С‡Рµ - 16 Р±РёС‚. */
	#define LCDMODE_LTDC_PIP16	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ PIP СЃ С„РѕСЂРјР°С‚РѕРј 16 Р±РёС‚ - RGB565 */

	//#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	//#define LCDMODE_LTDC_L24	1	/* 32-bit РЅР° РїРёРєСЃРµР»СЊ РІ РїР°РјСЏС‚Рё (3 Р±Р°Р№С‚Р°) */
	//#define LCDMODE_ILI8961_TOPDOWN	1

#elif 0

	#define LCDMODE_ILI9341	1	/* 320*240 SF-TC240T-9370-T СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9341 - STM32F4DISCO */
	//#define LCDMODE_ILI9341_TOPDOWN	1	/* LCDMODE_ILI9341 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРїСЂР°РІР°) */

#else
	//#define LCDMODE_HARD_SPI	1	/* LCD over SPI line */
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

	//#define LCDMODE_ILI9225	1	/* РРЅРґРёРєР°С‚РѕСЂ 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9225РЎ */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃР»РµРІР° РѕС‚ СЌРєСЂР°РЅР°) */
	//#define LCDMODE_UC1608	1		/* РРЅРґРёРєР°С‚РѕСЂ 240*128 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј UC1608.- РјРѕРЅРѕС…СЂРѕРјРЅС‹Р№ */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРІРµСЂС…Сѓ) */
	//#define LCDMODE_ST7735	1	/* РРЅРґРёРєР°С‚РѕСЂ 160*128 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Sitronix ST7735 - TFT РїР°РЅРµР»СЊ 160 * 128 HY-1.8-SPI */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРїСЂР°РІР°) */
	//#define LCDMODE_ST7565S	1	/* РРЅРґРёРєР°С‚РѕСЂ WO12864C2-TFH# 128*64 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРІРµСЂС…Сѓ) */
	//#define LCDMODE_ILI9320	1	/* РРЅРґРёРєР°С‚РѕСЂ 248*320 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILI9320 */
	//#define LCDMODE_LS020 	1	/* РРЅРґРёРєР°С‚РѕСЂ 176*132 Sharp LS020B8UD06 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	//#define LCDMODE_LPH88		1	/* РРЅРґРёРєР°С‚РѕСЂ 176*132 LPH8836-2 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ */
	//#define LCDMODE_ILI9163	1	/* РРЅРґРёРєР°С‚РѕСЂ LPH9157-2 176*132 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј ILITEK ILI9163 - Р›РµРЅС‚Р° РґРёСЃРїР»РµСЏ СЃРїСЂР°РІР°, Р° РІС‹РІРѕРґС‹ СЃР»РµРІР°. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - РїРµСЂРµРІРµСЂРЅСѓС‚СЊ РёР·РѕР±СЂР°Р¶РµРЅРёРµ (РґР»СЏ РІС‹РІРѕРґРѕРІ СЃРїСЂР°РІР°, Р»РµРЅС‚Р° РґРёСЃРїР»РµСЏ СЃР»РµРІР°) */
	//#define LCDMODE_L2F50	1	/* РРЅРґРёРєР°С‚РѕСЂ 176*132 СЃ РєРѕРЅС‚СЂРѕР»Р»РµСЂРѕРј Epson L2F50126 */
	//#define LCDMODE_L2F50_TOPDOWN	1	/* РџРµСЂРµРІРѕСЂРѕС‚ РёР·РѕР±СЂР°Р¶РµРЅРё СЏ РІ СЃР»СѓС‡Р°Рµ LCDMODE_L2F50 */
	// --- РћРґРЅР° РёР· СЌС‚РёС… СЃС‚СЂРѕРє РѕРїСЂРµРґРµР»СЏРµС‚ С‚РёРї РґРёСЃРїР»РµСЏ, РґР»СЏ РєРѕС‚РѕСЂРѕРіРѕ РєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ РїСЂРѕС€РёРІРєР°
#endif

#if LCDMODE_LQ043T3DX02K || LCDMODE_AT070TN90|| LCDMODE_AT070TNA2
	#define WITHLCDBACKLIGHT	1	// РРјРµРµС‚СЃСЏ СѓРїСЂР°РІР»РµРЅРёРµ РїРѕРґСЃРІРµС‚РєРѕР№ РґРёСЃРїР»РµСЏ 
	#define WITHLCDBACKLIGHTMIN	0	// РќРёР¶РЅРёР№ РїСЂРµРґРµР» СЂРµРіСѓР»РёСЂРѕРІРєРё (РїРѕРєР°Р·С‹РІР°РµС‚С‹Р№ РЅР° РґРёСЃРїР»РµРµ)
	#define WITHLCDBACKLIGHTMAX	4	// Р’РµСЂС…РЅРёР№ РїСЂРµРґРµР» СЂРµРіСѓР»РёСЂРѕРІРєРё (РїРѕРєР°Р·С‹РІР°РµС‚С‹Р№ РЅР° РґРёСЃРїР»РµРµ)
	#define WITHKBDBACKLIGHT	1	// РРјРµРµС‚СЃСЏ СѓРїСЂР°РІР»РµРЅРёРµ РїРѕРґСЃРІРµС‚РєРѕР№ РєР»Р°РІРёР°С‚СѓСЂС‹ 
#else
	#define WITHLCDBACKLIGHT	1	// РРјРµРµС‚СЃСЏ СѓРїСЂР°РІР»РµРЅРёРµ РїРѕРґСЃРІРµС‚РєРѕР№ РґРёСЃРїР»РµСЏ 
	#define WITHLCDBACKLIGHTMIN	0	// РќРёР¶РЅРёР№ РїСЂРµРґРµР» СЂРµРіСѓР»РёСЂРѕРІРєРё (РїРѕРєР°Р·С‹РІР°РµС‚С‹Р№ РЅР° РґРёСЃРїР»РµРµ)
	#define WITHLCDBACKLIGHTMAX	3	// Р’РµСЂС…РЅРёР№ РїСЂРµРґРµР» СЂРµРіСѓР»РёСЂРѕРІРєРё (РїРѕРєР°Р·С‹РІР°РµС‚С‹Р№ РЅР° РґРёСЃРїР»РµРµ)
	#define WITHKBDBACKLIGHT	1	// РРјРµРµС‚СЃСЏ СѓРїСЂР°РІР»РµРЅРёРµ РїРѕРґСЃРІРµС‚РєРѕР№ РєР»Р°РІРёР°С‚СѓСЂС‹ 
#endif

#if WITHISBOOTLOADER


	#define CTLREGMODE_STORCH_V7	1	/* TFT 4.3" "РђРёСЃС‚" СЃ DSP Рё FPGA STM32H743IIT6 РёР»Рё R7S721020VCFP */
	//#define CTLREGSTYLE_NOCTLREG 1
	/* РєРѕРґС‹ РІС…РѕРґРѕРІ РєРѕРјРјСѓС‚Р°С‚РѕСЂР° РёСЃС‚РѕС‡РЅРёРєРѕРІ СЃРёРіРЅР°Р»Р° РґР»СЏ РЈРќР§ РїСЂРёС‘РјРЅРёРєР° */
	#define BOARD_DETECTOR_SSB 	0		// Р—Р°РіР»СѓС€РєР°

	#define BOARD_DETECTOR_FM	0
	#define BOARD_DETECTOR_AM	0
	#define BOARD_DETECTOR_MUTE	0
	#define BOARD_DETECTOR_TUNE	0

	// +++ Р·Р°РіР»СѓС€РєРё РґР»СЏ РїР»Р°С‚ СЃ DSP РѕР±СЂР°Р±РѕС‚РєРѕР№
	#define	BOARD_AGCCODE_ON	0
	#define	BOARD_AGCCODE_OFF	0

	/* РєРѕРґС‹ С„РёР»СЊС‚СЂРѕРІ РІС‚РѕСЂРѕР№ РџР§, РІС‹РґР°РІР°РµРјС‹Рµ РЅР° РґРµС€РёС„СЂР°С‚РѕСЂС‹ */
	#define BOARD_FILTER_0P5		0	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_3P1		0	/* 3.1 or 2.75 kHz filter */
	//#define BOARD_FILTER_6P0		0	/* 6.0 kHz filter */
	//#define BOARD_FILTER_8P0		0	/* 6.0 kHz filter */
	// --- Р·Р°РіР»СѓС€РєРё РґР»СЏ РїР»Р°С‚ СЃ DSP РѕР±СЂР°Р±РѕС‚РєРѕР№
	// --- Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ

	#define WITHMODESETMIXONLY 1	// Use only product detector

	/* С‡С‚Рѕ Р·Р° РїР°РјСЏС‚СЊ РЅР°СЃС‚СЂРѕРµРє Рё С‡Р°СЃС‚РѕС‚ РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІ РєРѕРЅС‚СЂРѕР»Р»РµСЂРµ */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// РўР°Рє Р¶Рµ РїСЂРё РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРё FM25040A - 5 РІРѕР»СЊС‚, 512 Р±Р°Р№С‚
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16		// demo board with atxmega128a4u
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A
	#define NVRAM_TYPE NVRAM_TYPE_NOTHING	// РЅРµС‚ NVRAM
	#define HARDWARE_IGNORENONVRAM	1		// РѕС‚Р»Р°РґРєР° РЅР° РїР»Р°С‚Р°С… РіРґРµ РЅРµС‚ РЅРёРєР°РєРѕРіРѕ NVRAM

	// End of NVRAM definitions section

#else /* WITHISBOOTLOADER */
	// app

	#define CTLREGMODE_STORCH_V7	1	/* TFT 4.3" "РђРёСЃС‚" СЃ DSP Рё FPGA STM32H743IIT6 РёР»Рё R7S721020VCFP */

	/* РєРѕРґС‹ РІС…РѕРґРѕРІ РєРѕРјРјСѓС‚Р°С‚РѕСЂР° РёСЃС‚РѕС‡РЅРёРєРѕРІ СЃРёРіРЅР°Р»Р° РґР»СЏ РЈРќР§ РїСЂРёС‘РјРЅРёРєР° */
	#define BOARD_DETECTOR_SSB 	0		// Р—Р°РіР»СѓС€РєР°

	// +++ Р·Р°РіР»СѓС€РєРё РґР»СЏ РїР»Р°С‚ СЃ DSP РѕР±СЂР°Р±РѕС‚РєРѕР№
	#define	BOARD_AGCCODE_ON	0x00
	#define	BOARD_AGCCODE_OFF	0x01

	/* РєРѕРґС‹ С„РёР»СЊС‚СЂРѕРІ РІС‚РѕСЂРѕР№ РџР§, РІС‹РґР°РІР°РµРјС‹Рµ РЅР° РґРµС€РёС„СЂР°С‚РѕСЂС‹ */
	#define BOARD_FILTER_0P5		1	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_3P1		0	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_6P0		0	/* 6.0 kHz filter */
	#define BOARD_FILTER_8P0		0	/* 6.0 kHz filter */
	// --- Р·Р°РіР»СѓС€РєРё РґР»СЏ РїР»Р°С‚ СЃ DSP РѕР±СЂР°Р±РѕС‚РєРѕР№

	//#define WITHRTS192	1		// Р•СЃС‚СЊ РєР°РЅР°Р» СЃРїРµРєС‚СЂРѕР°РЅР°Р»РёР·Р°С‚РѕСЂР° - РЅРµ Р·Р°Р±С‹С‚СЊ РІРєР»СЋС‡РёС‚СЊ WITHSAI2HW
	#define WITHRTS96		1		/* РІРјРµСЃС‚Рѕ РІС‹С…РѕРґРЅРѕРіРѕ Р°СѓРґРёРѕСЃРёРіРЅР°Р»Р° РїРµСЂРµРґР°С‡Р° РєРІР°РґСЂР°С‚СѓСЂ РїРѕ USB */

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define	WITHENCODER	1	/* РґР»СЏ РёР·РјРµРЅРµРЅРёСЏ С‡Р°СЃС‚РѕС‚С‹ РёРјРµРµС‚СЃСЏ РµРЅРєРѕРґРµСЂ */
	#define WITHENCODER2	1		/* РµСЃС‚СЊ РІС‚РѕСЂРѕР№ РІР°Р»РєРѕРґРµСЂ */

	#define WITHUSEDUALWATCH	1	// Р’С‚РѕСЂРѕР№ РїСЂРёРµРјРЅРёРє
	#define WITHFQMETER	1	/* РµСЃС‚СЊ СЃС…РµРјР° РёР·РјРµСЂРµРЅРёСЏ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹, РїРѕ РІРЅРµС€РЅРµРјСѓ PPS */

	// FPGA section
	//#define	WITHFPGAWAIT_AS	1	/* FPGA Р·Р°РіСЂСѓР¶Р°РµС‚СЃСЏ РёР· СЃРѕР±СЃС‚РІРµРЅРЅРѕР№ РјРёРєСЂРѕСЃС…РµРјС‹ Р·Р°РіСЂСѓР·С‡РёРєР° - РґРѕР¶РґР°С‚СЊСЃСЏ РѕРєРѕРЅС‡Р°РЅРёСЏ Р·Р°РіСЂСѓР·РєРё РїРµСЂРµРґ РёРЅРёС†РёР°Р»РёР·Р°С†РёРµР№ SPI РІ РїСЂРѕС†РµСЃСЃРѕСЂРµ */
	#define	WITHFPGALOAD_PS	1	/* FPGA Р·Р°РіСЂСѓР¶Р°РµС‚СЃСЏ РїСЂРѕС†РµСЃСЃРѕСЂРѕРј СЃ РїРѕРјРѕС‰СЊСЋ SPI */

	// +++ Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ
	//#define WITHRFSG	1	/* РІРєР»СЋС‡РµРЅРѕ СѓРїСЂР°РІР»РµРЅРёРµ Р’Р§ СЃРёРіРЅР°Р»-РіРµРЅРµСЂР°С‚РѕСЂРѕРј. */
	#define WITHTX		1	/* РІРєР»СЋС‡РµРЅРѕ СѓРїСЂР°РІР»РµРЅРёРµ РїРµСЂРµРґР°С‚С‡РёРєРѕРј - СЃРёРєРІРµРЅСЃРѕСЂ, СЌР»РµРєС‚СЂРѕРЅРЅС‹Р№ РєР»СЋС‡. */
	//#define WITHAUTOTUNER	1	/* Р•СЃС‚СЊ С„СѓРЅРєС†РёСЏ Р°РІС‚РѕС‚СЋРЅРµСЂР° */
	#define WITHIFSHIFT	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* РќР°С‡Р°Р»СЊРЅРѕРµ Р·Р°РЅС‡РµРЅРёРµ IF SHIFT */
	//#define WITHPBT		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ PBT (РµСЃР»Рё LO3 РµСЃС‚СЊ) */
	#define WITHCAT			1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ CAT (С‡РµСЂРµР· USB CDC) */
	//#define WITHDEBUG		1	/* РћС‚Р»Р°РґРѕС‡РЅР°СЏ РїРµС‡Р°С‚СЊ С‡РµСЂРµР· COM-РїРѕСЂС‚. */
	//#define WITHNMEA		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ NMEA parser */
	//#define WITHMODEM		1	/* РЈСЃС‚СЂРѕР№СЃС‚РІРѕ СЂР°Р±РѕС‚Р°РµС‚ РєР°Рє СЂР°РґРёРѕРјРѕРґРµРј СЃ РїРѕСЃР»РµРґРѕРІР°С‚РµР»СЊРЅС‹Рј РёРЅС‚РµСЂС„РµР№СЃРѕРј */
	//#define WITHFREEDV	1	/* РїРѕРґРґРµСЂР¶РєР° СЂРµР¶РёРјР° FreeDV - http://freedv.org/ */ 
	//#define WITHBEACON	1	/* РСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ СЂРµР¶РёРј РјР°СЏРєР° */
	#define WITHVOX			1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ VOX */
	#define WITHSHOWSWRPWR 1	/* РЅР° РґРёСЃРїР»РµРµ РѕРґРЅРѕРІСЂРµРјРµРЅРЅРѕ РѕС‚РѕР±СЂР°Р¶Р°СЋС‚СЃСЏ SWR-meter Рё PWR-meter */
	#define WITHSWRMTR	1		/* РР·РјРµСЂРёС‚РµР»СЊ РљРЎР’ */
	//#define WITHPWRMTR	1	/* РРЅРґРёРєР°С‚РѕСЂ РІС‹С…РѕРґРЅРѕР№ РјРѕС‰РЅРѕСЃС‚Рё РёР»Рё */
	//#define WITHPWRLIN	1	/* РРЅРґРёРєР°С‚РѕСЂ РІС‹С…РѕРґРЅРѕР№ РјРѕС‰РЅРѕСЃС‚Рё РїРѕРєР°Р·С‹РІР°РµС‚ РЅР°РїСЂСЏР¶РµРЅРёРµ Р° РЅРµ РјРѕС‰РЅРѕСЃС‚СЊ */


	/* Board hardware configuration */
	#define CODEC1_TYPE CODEC_TYPE_NAU8822L
	#define CODEC_TYPE_NAU8822_USE_SPI	1
	//#define	CODEC1_TYPE	CODEC_TYPE_TLV320AIC23B
	//#define CODEC_TYPE_TLV320AIC23B_USE_SPI	1
	//#define CODEC_TYPE_NAU8822_USE_8KS	1	/* РєРѕРґРµРє СЂР°Р±РѕС‚Р°РµС‚ СЃ sample rate 8 kHz */

	#define CODEC2_TYPE	CODEC_TYPE_FPGAV1	/* РєРІР°РґСЂР°С‚СѓСЂС‹ РїРѕР»СѓС‡Р°РµРј РѕС‚ FPGA */

	#define WITHI2S_FORMATI2S_PHILIPS 1	// Р’РѕР·РјРѕР¶РЅРѕ РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРµ РїСЂРё РїРµСЂРµРґР°С‡Рµ РґР°РЅРЅС‹С… РІ РєРѕРґРµРє, РїРѕРґРєР»СЋС‡РµРЅРЅС‹Р№ Рє РЅР°СѓС€РЅРёРєР°Рј Рё РјРёРєСЂРѕС„РѕРЅСѓ

	#define WITHSAI1_FORMATI2S_PHILIPS 1	// С‚СЂРµР±СѓРµС‚СЃСЏ РїСЂРё РїРѕР»СѓС‡РµРЅРёРё РґР°РЅРЅС‹С… РѕС‚ FPGA
	#define WITHSAI1_FRAMEBITS 256	// РџРѕР»РЅС‹Р№ СЂР°Р·РјРµСЂ С„СЂРµР№РјР° РґР»СЏ РґРІСѓС… РєРІР°РґСЂР°С‚СѓСЂ - РєР°РЅР°Р» С‚СЂР°РЅСЃРёРІРµСЂР°


	#define WITHSAI2_FORMATI2S_PHILIPS 1	// С‚СЂРµР±СѓРµС‚СЃСЏ РїСЂРё РїРѕР»СѓС‡РµРЅРёРё РґР°РЅРЅС‹С… РѕС‚ FPGA
	#define WITHSAI2_FRAMEBITS 64	// РџРѕР»РЅС‹Р№ СЂР°Р·РјРµСЂ С„СЂРµР№РјР° РґР»СЏ РґРІСѓС… РєРІР°РґСЂР°С‚СѓСЂ РїРѕ 24 Р±РёС‚Р° - РєР°РЅР°Р» СЃРїРµРєС‚СЂРѕР°РЅР°Р»РёР·Р°С‚РѕСЂР°

	#define WITHNESTEDINTERRUPTS	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РїСЂРё РЅР°Р»РёС‡РёРё real-time С‡Р°СЃС‚Рё. */
	#define WITHINTEGRATEDDSP		1	/* РІ РїСЂРѕРіСЂР°РјРјСѓ РІРєР»СЋС‡РµРЅР° РёРЅРёС†РёР°Р»РёР·Р°С†РёСЏ Рё Р·Р°РїСѓСЃРє DSP С‡Р°СЃС‚Рё. */
	#define WITHIFDACWIDTH	32		// 1 Р±РёС‚ Р·РЅР°Рє Рё 31 Р±РёС‚ Р·РЅР°С‡Р°С‰РёС…
	#define WITHIFADCWIDTH	32		// 1 Р±РёС‚ Р·РЅР°Рє Рё 31 Р±РёС‚ Р·РЅР°С‡Р°С‰РёС…
	#define WITHAFADCWIDTH	16		// 1 Р±РёС‚ Р·РЅР°Рє Рё 15 Р±РёС‚ Р·РЅР°С‡Р°С‰РёС…
	#define WITHAFDACWIDTH	16		// 1 Р±РёС‚ Р·РЅР°Рє Рё 15 Р±РёС‚ Р·РЅР°С‡Р°С‰РёС…
	//#define WITHDACOUTDSPAGC		1	/* РђР РЈ СЂРµР°Р»РёР·РѕРІР°РЅРѕ РєР°Рє РІС‹С…РѕРґ Р¦РђРџ РЅР° Р°РЅР°Р»РѕРіРѕРІСѓСЋ С‡Р°СЃС‚СЊ. */
	//#define WITHEXTERNALDDSP		1	/* РёРјРµРµС‚СЃСЏ СѓРїСЂР°РІР»РµРЅРёРµ РІРЅРµС€РЅРµР№ DSP РїР»Р°С‚РѕР№. */
	//#define WITHLOOPBACKTEST	1
	#define WITHDSPEXTDDC 1			/* РљРІР°РґСЂР°С‚СѓСЂС‹ РїРѕР»СѓС‡Р°СЋС‚СЃСЏ РІРЅРµС€РЅРµР№ Р°РїРїР°СЂР°С‚СѓСЂРѕР№ */
	#define WITHDSPEXTFIR 1			/* Р¤РёР»СЊС‚СЂР°С†РёСЏ РєРІР°РґСЂР°С‚СѓСЂ РѕСЃСѓС‰РµСЃС‚РІР»СЏРµС‚СЃСЏ РІРЅРµС€РЅРµР№ Р°РїРїР°СЂР°С‚СѓСЂРѕР№ */
	#define WITHIF4DSP	1	// "Р”СЏС‚РµР»"
	#define WITHDACSTRAIGHT 1		/* РўСЂРµР±СѓРµС‚СЃСЏ С„РѕСЂРјРёСЂРѕРІР°РЅРёРµ РєРѕРґР° РґР»СЏ Р¦РђРџ РІ СЂРµР¶РёРјРµ Р±РµР·Р·РЅР°РєРѕРІРѕРіРѕ РєРѕРґР° */


	#define WITHBARS		1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ S-РјРµС‚СЂР° Рё SWR-РјРµС‚СЂР° */

	#define WITHVOLTLEVEL	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ РЅР°РїСЂСЏР¶РµРЅРёСЏ РђРљР‘ */
	#define WITHCURRLEVEL	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ С‚РѕРєР° РѕРєРѕРЅРµС‡РЅРѕРіРѕ РєР°СЃРєР°РґР° */
	////#define WITHTHERMOLEVEL	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ С‚РµРјРїРµСЂР°С‚СѓСЂС‹ */

	//#define WITHSWLMODE	1	/* РїРѕРґРґРµСЂР¶РєР° Р·Р°РїРѕРјРёРЅР°РЅРёСЏ РјРЅРѕР¶РµСЃС‚РІР° С‡Р°СЃС‚РѕС‚ РІ swl-mode */
	#define WITHVIBROPLEX	1	/* РІРѕР·РјРѕР¶РЅРѕСЃС‚СЊ СЌРјСѓР»СЏС†РёРё РїРµСЂРµРґР°С‡Рё РІРёР±СЂРѕРїР»РµРєСЃРѕРј */

	// Р•СЃС‚СЊ Р»Рё СЂРµРіСѓР»РёСЂРѕРІРєР° РїР°СЂР°РјРµС‚СЂРѕРІ РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂР°РјРё
	//#define WITHPOTNOTCH	1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ СЂРµРіСѓР»РёСЂРѕРІРєР° С‡Р°СЃС‚РѕС‚С‹ NOTCH С„РёР»СЊС‚СЂР°С‡ РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂРѕРј */
	//#define WITHPOTIFSHIFT	1	/* СЂРµРіСѓР»РёСЂРѕРІРєР° IF SHIFT	*/
	//#define WITHPOTWPM		1	/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ СЂРµРіСѓР»РёСЂРѕРІРєР° СЃРєРѕСЂРѕСЃС‚Рё РїРµСЂРµРґР°С‡Рё РІ С‚РµР»РµРіСЂР°С„Рµ РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂРѕРј */
	#define WITHPOTIFGAIN		1	/* СЂРµРіСѓР»СЏС‚РѕСЂС‹ СѓСЃРёР»РµРЅРёСЏ РџР§ РЅР° РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂР°С… */
	#define WITHPOTAFGAIN		1	/* СЂРµРіСѓР»СЏС‚РѕСЂС‹ СѓСЃРёР»РµРЅРёСЏ РќР§ РЅР° РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂР°С… */

	#define WITHMENU 	1	/* С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РјРµРЅСЋ РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕС‚РєР»СЋС‡РµРЅР° - РµСЃР»Рё РЅР°СЃС‚СЂР°РёРІР°С‚СЊ РЅРµС‡РµРіРѕ */

	//#define WITHONLYBANDS 1		/* РџРµСЂРµСЃС‚СЂРѕР№РєР° РјРѕР¶РµС‚ Р±С‹С‚СЊ РѕРіСЂР°РЅРёС‡РµРЅР° Р»СЋР±РёС‚РµР»СЊСЃРєРёРјРё РґРёР°РїР°Р·РѕРЅР°РјРё */
	//#define WITHBCBANDS	1		/* РІ С‚Р°Р±Р»РёС†Рµ РґРёР°РїР°Р·РѕРЅРѕРІ РїСЂРёСЃСѓС‚СЃС‚РІСѓСЋС‚ РІРµС‰Р°С‚РµР»СЊРЅС‹Рµ РґРёР°РїР°Р·РѕРЅС‹ */
	#define WITHWARCBANDS	1	/* Р’ С‚Р°Р±Р»РёС†Рµ РґРёР°РїР°Р·РѕРЅРѕРІ РїСЂРёСЃСѓС‚СЃС‚РІСѓСЋС‚ HF WARC РґРёР°РїР°Р·РѕРЅС‹ */
	//#define WITHLFM		1	/* LFM MODE */
	//#define WITHTEMPSENSOR	1	/* РѕС‚РѕР±СЂР°Р¶РµРЅРёРµ РґР°РЅРЅС‹С… СЃ РґР°С‚С‡РёРєР° С‚РµРјРїРµСЂР°С‚СѓСЂС‹ */
	#define WITHDIRECTBANDS 1	/* РџСЂСЏРјРѕР№ РїРµСЂРµС…РѕРґ Рє РґРёР°РїР°Р·РѕРЅР°Рј РїРѕ РЅР°Р¶Р°С‚РёСЏРј РЅР° РєР»Р°РІРёР°С‚СѓСЂРµ */
	#define WITHSPKMUTE		1	/* СѓРїСЂР°РІР»РµРЅРёРµ РІС‹РєР»СЋС‡РµРЅРёРµРј РґРёРЅР°РјРёРєР° */
	#define WITHDATAMODE	1	/* СѓРїСЂР°РІР»РµРЅРёРµ СЃ РєР»Р°РІРёР°С‚СѓСЂС‹ РїРµСЂРµРґР°С‡РµР№ СЃ USB AUDIO РєР°РЅР°Р»Р° */
	#define WITHSLEEPTIMER	1	/* РІС‹РєР»СЋС‡РёС‚СЊ РёРЅРґРёРєР°С‚РѕСЂ Рё РІС‹РІРѕРґ Р·РІСѓРєР° РїРѕ РёСЃС‚РµС‡РµРЅРёРёРё СѓРєР°Р·Р°РЅРЅРѕРіРѕ РІСЂРµРјРµРЅРё */
	#define WITHFANTIMER	1	/* РІС‹РєР»СЋС‡Р°СЋС‰РёР№СЃСЏ РїРѕ С‚Р°Р№РјРµСЂСѓ РІРµРЅС‚РёР»СЏС‚РѕСЂ РІ СѓСЃРёР»РёС‚РµР»Рµ РјРѕС‰РЅРѕСЃС‚Рё */
	// --- Р­С‚Рё СЃС‚СЂРѕРєРё РјРѕР¶РЅРѕ РѕС‚РєР»СЋС‡Р°С‚СЊ, СѓРјРµРЅСЊС€Р°СЏ С„СѓРЅРєС†РёРѕРЅР°Р»СЊРЅРѕСЃС‚СЊ РіРѕС‚РѕРІРѕРіРѕ РёР·РґРµР»РёСЏ

	#define WITHMODESETFULLNFM 1


	#define WITHCATEXT	1	/* Р Р°СЃС€РёСЂРµРЅРЅС‹Р№ РЅР°Р±РѕСЂ РєРѕРјР°РЅРґ CAT */
	#define WITHELKEY	1
	//#define WITHKBDENCODER 1	// РїРµСЂРµСЃС‚СЂРѕР№РєР° С‡Р°СЃС‚РѕС‚С‹ РєРЅРѕРїРєР°РјРё
	#define WITHKEYBOARD 1	/* РІ РґР°РЅРЅРѕРј СѓСЃС‚СЂРѕР№СЃС‚РІРµ РµСЃС‚СЊ РєР»Р°РІРёР°С‚СѓСЂР° */
	#define KEYBOARD_USE_ADC	1	/* РЅР° РѕРґРЅРѕР№ Р»РёРЅРёРё СѓСЃС‚Р°РЅРѕРІР»РµРЅРѕ  С‡РµС‚С‹СЂРµ  РєР»Р°РІРёС€Рё. РЅР° vref - 6.8K, РґР°Р»РµРµ 2.2Рљ, 4.7Рљ Рё 13K. */

	/* С‡С‚Рѕ Р·Р° РїР°РјСЏС‚СЊ РЅР°СЃС‚СЂРѕРµРє Рё С‡Р°СЃС‚РѕС‚ РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ РІ РєРѕРЅС‚СЂРѕР»Р»РµСЂРµ */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// РўР°Рє Р¶Рµ РїСЂРё РёСЃРїРѕР»СЊР·РѕРІР°РЅРёРё FM25040A - 5 РІРѕР»СЊС‚, 512 Р±Р°Р№С‚
	#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16		// demo board with atxmega128a4u
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A
	//#define NVRAM_TYPE NVRAM_TYPE_NOTHING	// РЅРµС‚ NVRAM
	//#define HARDWARE_IGNORENONVRAM	1		// РѕС‚Р»Р°РґРєР° РЅР° РїР»Р°С‚Р°С… РіРґРµ РЅРµС‚ РЅРёРєР°РєРѕРіРѕ NVRAM

	// End of NVRAM definitions section
	/* Board hardware configuration */

	#define ADC1_TYPE ADC_TYPE_AD9246	/* ADC AD9246 chip - 14 bit */
	#define DDS1_TYPE DDS_TYPE_FPGAV1
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define PLL1_TYPE PLL_TYPE_ADF4001
	//#define DDS2_TYPE DDS_TYPE_AD9834
	#define RTC1_TYPE RTC_TYPE_DS1305	/* MAXIM DS1305EN RTC clock chip with SPI interface */
	//#define TSC1_TYPE TSC_TYPE_TSC2046	/* Resistive touch screen controller TI TSC2046 */
	//#define DAC1_TYPE	99999		/* РЅР°Р»РёС‡РёРµ Р¦РђРџ РґР»СЏ РїРѕРґСЃС‚СЂРѕР№РєРё С‚Р°РєС‚РѕРІРѕР№ С‡Р°СЃС‚РѕС‚С‹ */

#endif /* WITHISBOOTLOADER */


	#if 0
		#define WITHUSBHEADSET 1	/* С‚СЂР°РЅСЃРёРІРµСЂ СЂР°Р±РѕС‚Р°РµС‚ USB РіР°СЂРЅРёС‚СѓСЂРѕР№ РґР»СЏ РєРѕРјРїСЋС‚РµСЂР° - СЂРµР¶РёРј С‚РµСЃС‚РёСЂРѕРІР°РЅРёСЏ */
		#define WITHBBOX	1	// Black Box mode - СѓСЃС‚СЂРѕР№СЃС‚РІРѕ Р±РµР· РѕСЂРіР°РЅРѕРІ СѓРїСЂР°РІР»РµРЅРёСЏ
		#define	WITHBBOXMIKESRC	BOARD_TXAUDIO_USB
	#elif 0
		#define WITHBBOX	1	// Black Box mode - СѓСЃС‚СЂРѕР№СЃС‚РІРѕ Р±РµР· РѕСЂРіР°РЅРѕРІ СѓРїСЂР°РІР»РµРЅРёСЏ
		#define	WITHBBOXFREQ	26985000L		// С‡Р°СЃС‚РѕС‚Р° РїРѕСЃР»Рµ РІРєР»СЋС‡РµРЅРёСЏ
		//#define	WITHBBOXFREQ	(26985000L - 260)		// С‡Р°СЃС‚РѕС‚Р° РїРѕСЃР»Рµ РІРєР»СЋС‡РµРЅРёСЏ - 135 РєРѕСЂСЂРµРєС†РёСЏ С‡Р°СЃС‚РѕС‚С‹ РїР»Р°С‚С‹ СЃ  122.88 РґР»СЏ РїРѕРїР°РґР°РЅРёСЏ РІ РїСЂРёС‘Рј РїР»Р°С‚С‹ СЃ 100 РњР“С† РіРµРЅРµСЂР°С‚РѕСЂРѕРј Р±РµР· РєРѕСЂСЂРµРєС†РёРё
		//#define	WITHBBOXFREQ	(26985000L - 1600)		// С‡Р°СЃС‚РѕС‚Р° РїРѕСЃР»Рµ РІРєР»СЋС‡РµРЅРёСЏ
		//#define	WITHBBOXFREQ	(14070000L - 1000)		// РїСЂРѕСЃР»СѓС€РёРІР°РЅРёРµ BPSK С‡Р°СЃС‚РѕС‚
		//#define	WITHBBOXFREQ	(14065000L - 135)		// С‡Р°СЃС‚РѕС‚Р° РїРѕСЃР»Рµ РІРєР»СЋС‡РµРЅРёСЏ - 135 РєРѕСЂСЂРµРєС†РёСЏ С‡Р°СЃС‚РѕС‚С‹ РїР»Р°С‚С‹ СЃ  122.88 РґР»СЏ РїРѕРїР°РґР°РЅРёСЏ РІ РїСЂРёС‘Рј РїР»Р°С‚С‹ СЃ 100 РњР“С† РіРµРЅРµСЂР°С‚РѕСЂРѕРј Р±РµР· РєРѕСЂСЂРµРєС†РёРё
		//#define	WITHBBOXFREQ	14065000L		// С‡Р°СЃС‚РѕС‚Р° РїРѕСЃР»Рµ РІРєР»СЋС‡РµРЅРёСЏ
		//#define	WITHBBOXFREQ	(14065000L - 1000)		// С‡Р°СЃС‚РѕС‚Р° РїРѕСЃР»Рµ РІРєР»СЋС‡РµРЅРёСЏ
		//#define	WITHBBOXSUBMODE	SUBMODE_USB	// РµРґРёРЅСЃС‚РІРµРЅРЅС‹Р№ СЂРµР¶РёРј СЂР°Р±РѕС‚С‹
		#define	WITHBBOXSUBMODE	SUBMODE_BPSK	// РµРґРёРЅСЃС‚РІРµРЅРЅС‹Р№ СЂРµР¶РёРј СЂР°Р±РѕС‚С‹
		//#define	WITHBBOXFREQ	27100000L		// С‡Р°СЃС‚РѕС‚Р° РїРѕСЃР»Рµ РІРєР»СЋС‡РµРЅРёСЏ
		//#define	WITHBBOXSUBMODE	SUBMODE_CW	// РµРґРёРЅСЃС‚РІРµРЅРЅС‹Р№ СЂРµР¶РёРј СЂР°Р±РѕС‚С‹
		//#define	WITHBBOXTX		1		// Р°РІС‚РѕРјР°С‚РёС‡РµСЃРєРёР№ РїРµСЂРµС…РѕРґ РЅР° РїРµСЂРµРґР°С‡Сѓ
	#elif 0
		#define WITHBBOX	1	// Black Box mode - СѓСЃС‚СЂРѕР№СЃС‚РІРѕ Р±РµР· РѕСЂРіР°РЅРѕРІ СѓРїСЂР°РІР»РµРЅРёСЏ
		#define	WITHBBOXFREQ	136000L		// С‡Р°СЃС‚РѕС‚Р° РїРѕСЃР»Рµ РІРєР»СЋС‡РµРЅРёСЏ
		#define	WITHBBOXSUBMODE	SUBMODE_USB	// РµРґРёРЅСЃС‚РІРµРЅРЅС‹Р№ СЂРµР¶РёРј СЂР°Р±РѕС‚С‹
		#define WITHBBOXREC	1		// Р°РІС‚РѕРјР°С‚РёС‡РµСЃРєРѕРµ РІРєР»СЋС‡РµРЅРёРµ СЂРµР¶РёРјР° Р·Р°РїРёСЃРё РїРѕСЃР»Рµ РїРѕРґР°С‡Рё РїРёС‚Р°РЅРёСЏ
	#elif 0
		#define WITHBBOX	1	// Black Box mode - СѓСЃС‚СЂРѕР№СЃС‚РІРѕ Р±РµР· РѕСЂРіР°РЅРѕРІ СѓРїСЂР°РІР»РµРЅРёСЏ
		#define	WITHBBOXFREQ	7030000L		// С‡Р°СЃС‚РѕС‚Р° РїРѕСЃР»Рµ РІРєР»СЋС‡РµРЅРёСЏ
		#define	WITHBBOXSUBMODE	SUBMODE_LSB	// РµРґРёРЅСЃС‚РІРµРЅРЅС‹Р№ СЂРµР¶РёРј СЂР°Р±РѕС‚С‹
		#define	WITHBBOXTX		1		// Р°РІС‚РѕРјР°С‚РёС‡РµСЃРєРёР№ РїРµСЂРµС…РѕРґ РЅР° РїРµСЂРµРґР°С‡Сѓ
		#define	WITHBBOXMIKESRC	BOARD_TXAUDIO_2TONE	// 2: 2tone, 1: noise 
	#endif
	//#define WITHUSESDCARD 1			// Р’РєР»СЋС‡РµРЅРёРµ РїРѕРґРґРµСЂР¶РєРё SD CARD
	//#define WITHUSEAUDIOREC	1	// Р—Р°РїРёСЃСЊ Р·РІСѓРєР° РЅР° SD CARD
	//#define WITHUSEAUDIORECCLASSIC	1	// СЃС‚Р°РЅРґР°СЂС‚РЅС‹Р№ С„РѕСЂРјР°С‚ Р·Р°РїРёСЃРё, Р±РµР· "РґС‹СЂ"
	//#define WITHUSEAUDIOREC2CH	1	// Р—Р°РїРёСЃСЊ Р·РІСѓРєР° РЅР° SD CARD РІ СЃС‚РµСЂРµРѕ

	#define FTW_RESOLUTION 32	/* СЂР°Р·СЂСЏРґРЅРѕСЃС‚СЊ FTW РІС‹Р±СЂР°РЅРЅРѕРіРѕ DDS */

	#define MODEL_DIRECT	1	/* РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РїСЂСЏРјРѕР№ СЃРёРЅС‚РµР·, Р° РЅРµ РіРёР±СЂРёРґРЅС‹Р№ */

	#define DDS1_CLK_DIV	1		/* Р”РµР»РёС‚РµР»СЊ РѕРїРѕСЂРЅРѕР№ С‡Р°СЃС‚РѕС‚С‹ РїРµСЂРµРґ РїРѕРґР°С‡РµР№ РІ DDS1 */


	//#define WITHWFM	1			/* РёСЃРїРѕР»СЊР·СѓРµС‚СЃСЏ WFM */
	/* РІСЃРµ РІРѕР·РјРѕР¶РЅС‹Рµ РІ РґР°РЅРЅРѕР№ РєРѕРЅС„РёРіСѓСЂР°С†РёРё С„РёР»СЊС‚СЂС‹ */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1 /* | IF3_FMASK_6P0 | IF3_FMASK_8P0*/)
	/* РІСЃРµ РІРѕР·РјРѕР¶РЅС‹Рµ РІ РґР°РЅРЅРѕР№ РєРѕРЅС„РёРіСѓСЂР°С†РёРё С„РёР»СЊС‚СЂС‹ РґР»СЏ РїРµСЂРµРґР°С‡Рё */
	#define IF3_FMASKTX	(IF3_FMASK_3P1 /*| IF3_FMASK_6P0 */)
	/* С„РёР»СЊС‚СЂС‹, РґР»СЏ РєРѕС‚РѕСЂС‹С… СЃС‚РѕРёС‚ РїСЂРёР·РЅР°Рє HAVE */
	#define IF3_FHAVE	( IF3_FMASK_0P5 | IF3_FMASK_3P1 /*| IF3_FMASK_6P0 | IF3_FMASK_8P0*/)

	#define WITHDCDCFREQCTL	1		// РРјРµРµС‚СЃСЏ СѓРїСЂР°РІР»РµРЅРёРµ С‡Р°СЃС‚РѕС‚РѕР№ РїСЂРµРѕР±СЂР°Р·РѕРІР°С‚РµР»РµР№ Р±Р»РѕРєР° РїРёС‚Р°РЅРёСЏ Рё/РёР»Рё РїРѕРґСЃРІРµС‚РєРё РґРёСЃРїР»РµСЏ

	// РќР°Р·РЅР°С‡РµРЅРёСЏ РІС…РѕРґРѕРІ РђР¦Рџ РїСЂРѕС†РµСЃСЃРѕСЂР°.
	enum 
	{ 
	#if WITHPOTIFGAIN
		POTIFGAIN = BOARD_ADCXIN(0),		// MCP3208 CH0 IF GAIN
	#endif /* WITHPOTIFGAIN */
	#if WITHPOTAFGAIN
		//POTAFGAIN = BOARD_ADCXIN(2),		// MCP3208 CH2 РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂ СѓРїСЂР°РІР»РµРЅРёСЏ СЃРєРѕСЂРѕСЃС‚СЊСЋ РїРµСЂРµРґР°С‡Рё РІ С‚РµР»РµРіСЂР°С„Рµ
		POTAFGAIN = BOARD_ADCXIN(1),		// MCP3208 CH1 AF GAIN
	#endif /* WITHPOTAFGAIN */
	#if WITHPOTWPM
		POTWPM = BOARD_ADCXIN(2),			// MCP3208 CH2 РїРѕС‚РµРЅС†РёРѕРјРµС‚СЂ СѓРїСЂР°РІР»РµРЅРёСЏ СЃРєРѕСЂРѕСЃС‚СЊСЋ РїРµСЂРµРґР°С‡Рё РІ С‚РµР»РµРіСЂР°С„Рµ
	#endif /* WITHPOTWPM */
	#if WITHTHERMOLEVEL
		XTHERMOIX = BOARD_ADCXIN(6),		// MCP3208 CH6 Exernal thermo sensor ST LM235Z
	#endif /* WITHTHERMOLEVEL */
	#if WITHVOLTLEVEL 
		VOLTSOURCE = BOARD_ADCXIN(7),		// MCP3208 CH7 РЎСЂРµРґРЅСЏСЏ С‚РѕС‡РєР° РґРµР»РёС‚РµР»СЏ РЅР°РїСЂСЏР¶РµРЅРёСЏ, РґР»СЏ РђРљР‘
	#endif /* WITHVOLTLEVEL */

	#if WITHSWRMTR
		PWRI = 0,			// PB1
		FWD = 0, REF = 1,	// PB0	SWR-meter
	#endif /* WITHSWRMTR */

	#if WITHCURRLEVEL
		PASENSEIX = 2,		// PA1 PA current sense - ACS712-05 chip
	#endif /* WITHCURRLEVEL */

		KI0 = 3, KI1 = 4, KI2 = 5, KI3 = 6, KI4 = 7		// РєР»Р°РІРёР°С‚СѓСЂР°
	};

	#define VOLTLEVEL_UPPER		47	// 4.7 kOhm - РІРµСЂС…РЅРёР№ СЂРµР·РёСЃС‚РѕСЂ РґРµР»РёС‚РµР»СЏ РґР°С‚С‡РёРєР° РЅР°РїСЂСЏР¶РµРЅРёСЏ
	#define VOLTLEVEL_LOWER		10	// 1.0 kOhm - РЅРёР¶РЅРёР№ СЂРµР·РёСЃС‚РѕСЂ
	#define XTHERMOLEVEL_UPPER		47	// 4.7 kOhm - РІРµСЂС…РЅРёР№ СЂРµР·РёСЃС‚РѕСЂ РґРµР»РёС‚РµР»СЏ РґР°С‚С‡РёРєР° С‚РµРјРїРµСЂР°С‚СѓСЂС‹
	#define XTHERMOLEVEL_LOWER		10	// 1.0 kOhm - РЅРёР¶РЅРёР№ СЂРµР·РёСЃС‚РѕСЂ

	#define KI_COUNT 5	// РєРѕР»РёС‡РµСЃС‚РІРѕ РёСЃРїРѕР»СЊР·СѓРµРјС‹С… РїРѕРґ РєР»Р°РІРёР°С‚СѓСЂСѓ РІС…РѕРґРѕРІ РђР¦Рџ
	#define KI_LIST	KI4, KI3, KI2, KI1, KI0,	// РёРЅРёС†РёР°Р»РёР·Р°С‚РѕСЂС‹ РґР»СЏ С„СѓРЅРєС†РёРё РїРµСЂРµРєРѕРґРёСЂРѕРІРєРё

#endif /* ARM_R7S72_TQFP176_CTLSTYLE_STORCH_V8_R4DZ_H_INCLUDED */
