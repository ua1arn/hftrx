/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "formats.h"	// for debug prints
#include "board.h"
#include "synthcalcs.h"

// На 8-битном микропроцессоре для ускорения вычислений
// частота округляется (отбрасываются младшие 16 бит).

#if CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA

	#define BANDDIVPOWER	16	/* ~65.5 kHz granulation */
	typedef uint_fast16_t fseltype_t;

#elif CPUSTYLE_ARM || CPUSTYLE_RISCV || CPUSTYLE_UBLAZE

	#define BANDDIVPOWER	0	/* 1 Hz granulation */
	typedef uint_fast32_t fseltype_t;

#else

	#error Undefined CPUSTYLE_XXX

#endif

#if BANDSELSTYLERE_RX3QSP

	// диапазонные фильтры по запросу RX3QSP

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (1000000u >> BANDDIVPOWER),
		(fseltype_t) (2000000u >> BANDDIVPOWER),
		(fseltype_t) (4000000u >> BANDDIVPOWER),
		(fseltype_t) (8000000u >> BANDDIVPOWER),
		(fseltype_t) (15000000u >> BANDDIVPOWER),
		(fseltype_t) (16100000u >> BANDDIVPOWER),
		(fseltype_t) (19530000u >> BANDDIVPOWER),
		(fseltype_t) (23100000u >> BANDDIVPOWER),
		(fseltype_t) (26450000u >> BANDDIVPOWER),
	};
	#define BANDCALCS ARRAY_SIZE(board_bandfs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
		BANDSELSTYLE_UA3REO_RFUNIT_V2

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (  800000u >> BANDDIVPOWER),
		(fseltype_t) ( 1500000u >> BANDDIVPOWER),
		(fseltype_t) ( 3400000u >> BANDDIVPOWER),
		(fseltype_t) ( 6000000u >> BANDDIVPOWER),
		(fseltype_t) ( 8000000u >> BANDDIVPOWER),
		(fseltype_t) (10000000u >> BANDDIVPOWER),
		(fseltype_t) (15000000u >> BANDDIVPOWER),
		(fseltype_t) (20000000u >> BANDDIVPOWER),
	};
	#define BANDCALCS	ARRAY_SIZE(board_bandfs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif CTLREGMODE16_NIKOLAI

	// диапазонные фильтры по запросу RX3QSP

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (1600000u >> BANDDIVPOWER),
		(fseltype_t) (5500000u >> BANDDIVPOWER),
		(fseltype_t) (15000000u >> BANDDIVPOWER),
	};
	#define BANDCALCS ARRAY_SIZE(board_bandfs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif BANDSELSTYLERE_R3PAV 

	// 8 диапазонных ФНЧ где 12м и 10м совмещенны для дешифратора 74HCT238

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (  1600000u >> BANDDIVPOWER),
		(fseltype_t) (  2100000u >> BANDDIVPOWER), // 0 001  1.6 -  2.1
		(fseltype_t) (  3800000u >> BANDDIVPOWER), // 0 010  2.1 -  3.8
		(fseltype_t) (  8100000u >> BANDDIVPOWER), // 0 011  3.8 -  8.1
		(fseltype_t) ( 10300000u >> BANDDIVPOWER), // 0 100  8.1 - 10.3
		(fseltype_t) ( 16000000u >> BANDDIVPOWER), // 0 101 10.3 - 16.0
		(fseltype_t) ( 18400000u >> BANDDIVPOWER), // 0 110 16.0 - 18.4
		(fseltype_t) ( 21400000u >> BANDDIVPOWER), // 0 111 18.4 - 21.4
		(fseltype_t) ( 29900000u >> BANDDIVPOWER), // 1 000 21.4 - 29.9
		(fseltype_t) ( 45000000u >> BANDDIVPOWER),
		(fseltype_t) (100000000u >> BANDDIVPOWER),
		(fseltype_t) (250000000u >> BANDDIVPOWER),

	};
	#define BANDCALCS ARRAY_SIZE(board_bandfs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif REQUEST_FOR_RN4NAB

	// диапазонные фильтры как в SDR RA4NAL http://ra4nal.qrz.ru http://ra4nal.lanstek.ru

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (2500000u >> BANDDIVPOWER),
		(fseltype_t) (4000000u >> BANDDIVPOWER),
		(fseltype_t) (6000000u >> BANDDIVPOWER),
		(fseltype_t) (7300000u >> BANDDIVPOWER),
		(fseltype_t) (12000000u >> BANDDIVPOWER),
		(fseltype_t) (14500000u >> BANDDIVPOWER),
		(fseltype_t) (21500000u >> BANDDIVPOWER),
		(fseltype_t) (30000000u >> BANDDIVPOWER),
	};
	#define BANDCALCS ARRAY_SIZE(board_bandfs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif STYLE_TS850_V1

	/* up-conversion RX */

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (500000u >> BANDDIVPOWER),
		(fseltype_t) (1620000u >> BANDDIVPOWER),
		(fseltype_t) (2500000u >> BANDDIVPOWER),
		(fseltype_t) (4000000u >> BANDDIVPOWER),
		(fseltype_t) (7000000u >> BANDDIVPOWER),
		(fseltype_t) (7500000u >> BANDDIVPOWER),
		(fseltype_t) (10500000u >> BANDDIVPOWER),
		(fseltype_t) (14000000u >> BANDDIVPOWER),
		(fseltype_t) (14500000u >> BANDDIVPOWER),
		(fseltype_t) (21000000u >> BANDDIVPOWER),
		(fseltype_t) (22000000u >> BANDDIVPOWER),
		(fseltype_t) (30000000u >> BANDDIVPOWER),
	};
	#define BANDCALCS ARRAY_SIZE(board_bandfs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	BANDSELSTYLERE_SW20XX || \
	BANDSELSTYLERE_SW20XX_UHF || \
	BANDSELSTYLERE_UPCONV36M || \
	BANDSELSTYLERE_UPCONV32M || \
	BANDSELSTYLERE_LOCONV32M || \
	BANDSELSTYLERE_LOCONV15M || \
	0

	// диапазонные фильтры ориентированы на любительские диапазоны
	/* for version 0...1.6, 1.6...32.0 MHz - 10 bands. */
	//#define BANDF_FREQMIN 1600000UL

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (1600000u >> BANDDIVPOWER),
		(fseltype_t) (2645751u >> BANDDIVPOWER),
		(fseltype_t) (5157518u >> BANDDIVPOWER),
		(fseltype_t) (8527602u >> BANDDIVPOWER),
		(fseltype_t) (11920570u >> BANDDIVPOWER),
		(fseltype_t) (16102043u >> BANDDIVPOWER),
		(fseltype_t) (19532741u >> BANDDIVPOWER),
		(fseltype_t) (23106070u >> BANDDIVPOWER),
		(fseltype_t) (26452221u >> BANDDIVPOWER),
		(fseltype_t) (45000000u >> BANDDIVPOWER),
		(fseltype_t) (100000000u >> BANDDIVPOWER),
		(fseltype_t) (250000000u >> BANDDIVPOWER),
	};
	#define BANDCALCS	ARRAY_SIZE(board_bandfs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	BANDSELSTYLERE_LOCONV15M_NLB || \
	BANDSELSTYLERE_LOCONV32M_NLB || \
	0

	// диапазонные фильтры ориентированы на любительские диапазоны
	/* for version 1.6...32.0 MHz - 9 bands. */
	//#define BANDF_FREQMIN 1600000UL

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (2645751u >> BANDDIVPOWER),
		(fseltype_t) (5157518u >> BANDDIVPOWER),
		(fseltype_t) (8527602u >> BANDDIVPOWER),
		(fseltype_t) (11920570u >> BANDDIVPOWER),
		(fseltype_t) (16102043u >> BANDDIVPOWER),
		(fseltype_t) (19532741u >> BANDDIVPOWER),
		(fseltype_t) (23106070u >> BANDDIVPOWER),
		(fseltype_t) (26452221u >> BANDDIVPOWER),
		(fseltype_t) (45000000u >> BANDDIVPOWER),
		(fseltype_t) (100000000u >> BANDDIVPOWER),
		(fseltype_t) (250000000u >> BANDDIVPOWER),
	};
	#define BANDCALCS	ARRAY_SIZE(board_bandfs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif CTLREGMODE_V3D

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (2800000u >> BANDDIVPOWER),
		(fseltype_t) (3900000u >> BANDDIVPOWER),
		(fseltype_t) (7200000u >> BANDDIVPOWER),
		(fseltype_t) (10200000u >> BANDDIVPOWER),
		(fseltype_t) (14500000u >> BANDDIVPOWER),
		(fseltype_t) (18300000u >> BANDDIVPOWER),
		(fseltype_t) (21500000u >> BANDDIVPOWER),
		(fseltype_t) (29000000u >> BANDDIVPOWER),
	};
	#define BANDCALCS	ARRAY_SIZE(board_bandfs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */


#elif \
	BANDSELSTYLERE_UPCONV56M || \
	BANDSELSTYLERE_UPCONV56M_45M || \
	BANDSELSTYLERE_UPCONV56M_36M || \
	0

	/* up-conversion RX */
	/* for version 0...1.6, 1.6...56.0 MHz - 8 bands. */
	#define BANDF_FREQMIN 1600000u
	#define BANDF_FREQ_SCALE 1661809u
	#define BANDF_FREQ_DENOM 1000000u
	#define BANDF_FREQ_TOP NOXVRTUNE_TOP		/* после этой частоты переходим к трансвертору */

	#define BANDCALCS	8	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */
	#define BANDF_USE_BANDINIT 1	/* необходимость функции инициализации. */

	static fseltype_t board_bandfs [BANDCALCS];

#elif \
	BANDSELSTYLE_OLEG4Z || \
	0

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (  500000u >> BANDDIVPOWER),
		(fseltype_t) ( 1600000u >> BANDDIVPOWER),
		(fseltype_t) ( 2300000u >> BANDDIVPOWER),
		(fseltype_t) ( 3300000u >> BANDDIVPOWER),
		(fseltype_t) ( 5000000u >> BANDDIVPOWER),
		(fseltype_t) ( 6700000u >> BANDDIVPOWER),
		(fseltype_t) (10700000u >> BANDDIVPOWER),
		(fseltype_t) (15500000u >> BANDDIVPOWER),
		(fseltype_t) (20300000u >> BANDDIVPOWER),
		(fseltype_t) (32000000u >> BANDDIVPOWER),
		(fseltype_t) (50000000u >> BANDDIVPOWER),
	};
	#define BANDCALCS	ARRAY_SIZE(board_bandfs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	BANDSELSTYLE_OLEG4Z_V2 || \
	0

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (  500000u >> BANDDIVPOWER),	// #1 BPF_ON_2		500 kHz-1600 kHz
		(fseltype_t) (  1600000u >> BANDDIVPOWER),	// #2 BPF_ON_3		1,6 MHz-2,3 MHz
		(fseltype_t) (  2300000u >> BANDDIVPOWER),	// #3 BPF_ON_4		2,3 MHz-3,3 MHz
		(fseltype_t) (  3300000u >> BANDDIVPOWER),	// #4 BPF_ON_5		3,3 MHz-5 MHz
		(fseltype_t) (  5000000u >> BANDDIVPOWER),	// #5 BPF_ON_6		5 MHz-6,7 MHz
		(fseltype_t) (  6700000u >> BANDDIVPOWER),	// #6 BPF_ON_7		6,7 MHz-10,7 MHz
		(fseltype_t) ( 10700000u >> BANDDIVPOWER),	// #7 BPF_ON_8		10,7 MHz-15,5 MHz
		(fseltype_t) ( 15500000u >> BANDDIVPOWER),	// #8 BPF_ON_9		15,5 MHz-20,3 MHz
		(fseltype_t) ( 20300000u >> BANDDIVPOWER),	// #9 BPF_ON_10	20,3 MHz-32 MHz
		(fseltype_t) ( 32000000u >> BANDDIVPOWER),	// #10 BPF_ON_11	32 MHz-50 MHz
		(fseltype_t) ( 50000000u >> BANDDIVPOWER),	// #11 BPF_ON_12	50 MHz-120 MHz
		(fseltype_t) (120000000u >> BANDDIVPOWER),	// #12 BPF_ON_13	120 MHz-265 MHz
		(fseltype_t) (260000000u >> BANDDIVPOWER),	// #13 BPF_ON_14	260 MHz-390 MHz
		(fseltype_t) (350000000u >> BANDDIVPOWER),	// #14 0	350-520 MHz
		(fseltype_t) (390000000u >> BANDDIVPOWER),	// #15 1	520-700 MHz
		(fseltype_t) (700000000u >> BANDDIVPOWER),	// #16 2	700-900 MHz
		(fseltype_t) (900000000u >> BANDDIVPOWER),	// #17 3	900-1200 MHz
		(fseltype_t) (1200000000u >> BANDDIVPOWER),// #18 4	1200-1700 MHz
	};
	#define BANDCALCS	ARRAY_SIZE(board_bandfs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	BANDSELSTYLE_LADVABEST || \
	0

	// ladvabest@gmail.com

	static const fseltype_t board_bandfs [] =
	{
		//(fseltype_t) (1600000 >> BANDDIVPOWER), // 2.645751 #0 1 выв.
		(fseltype_t) (2645751u >> BANDDIVPOWER), // 5.157518 #1 2 выв.
		(fseltype_t) (5157518u >> BANDDIVPOWER), // 8.527602 #2 3 выв.
		(fseltype_t) (8527602u >> BANDDIVPOWER), // 11.920570 #3 4 выв.
		(fseltype_t) (11920570u >> BANDDIVPOWER), // 16.102043 #4 5 выв.
		(fseltype_t) (16102043u >> BANDDIVPOWER), // 19.532741 #5 6 выв.
		(fseltype_t) (19532741u >> BANDDIVPOWER), // 23.106070 #6 7 выв.
		(fseltype_t) (23106070u >> BANDDIVPOWER), // 26.452221 #7 8 выв.
		(fseltype_t) (26452221u >> BANDDIVPOWER), // 27.980000 #8 9 выв.
		(fseltype_t) (27980000u >> BANDDIVPOWER), // 28.690000 #9 10 выв.
		(fseltype_t) (28690000u >> BANDDIVPOWER), // 35.000000 #10 11 выв.
		(fseltype_t) (35000000u >> BANDDIVPOWER), // 48.000000 #11 13 выв.
		(fseltype_t) (48000000u >> BANDDIVPOWER), // 100.000000 #12 14 выв.
		(fseltype_t) (100000000u >> BANDDIVPOWER), // 250.000000 #13 15 выв.
	};
	#define BANDCALCS	ARRAY_SIZE(board_bandfs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

	#define TUNE_BOTTOM 1600000L			/* нижняя частота настройки */

#elif \
		BANDSELSTYLERE_UHF_137M_174M || \
		0
	/* UHF 137..174 MHz */
	//#define TUNE_BOTTOM 137000000L		/* нижняя частота настройки */
	//#define TUNE_TOP 174000000L			/* верхняя частота настройки */

#elif \
		BANDSELSTYLERE_NOTHING || \
		CTLSTYLE_V5 || \
		FQMODEL_FMRADIO || \
		0
	// В этих аппаратах не требовалось выбирать диапазонные фильтры
#else
	#error No band selection hardware supported
#endif

uint_fast8_t
bandf_calc_getxvrtr(uint_fast8_t bandf)
{
#if BANDF_FREQ_TOP
	return bandf >= BANDCALCS;
#else /* BANDF_FREQ_TOP */
	return 0;
#endif /* BANDF_FREQ_TOP */
}

void bandf_calc_initialize(void)
{
#if BANDF_USE_BANDINIT
	uint_fast32_t tmp = BANDF_FREQMIN;	/* scale freq before calculate phase */
	uint_fast8_t i;
	
	for (i = 0; i < sizeof board_bandfs / sizeof board_bandfs [0]; ++ i)
	{
		board_bandfs [i] = (fseltype_t) (tmp >> BANDDIVPOWER);
		tmp = (uint_fast32_t) ((uint_fast64_t) tmp * BANDF_FREQ_SCALE / BANDF_FREQ_DENOM);
	}
	#if BANDF_FREQ_TOP
		/* если выше этой частоты - работает трансвертор */
		board_bandfs [BANDCALCS - 1] = (fseltype_t) (BANDF_FREQ_TOP >> BANDDIVPOWER);
	#endif /* BANDF_FREQ_TOP */

#endif /* BANDF_USE_BANDINIT */

	if (0)
	{
		/* отладочная печать границ диапазонов */
		uint_fast8_t i;
		for (i = 0; i < sizeof board_bandfs / sizeof board_bandfs [0]; ++ i)
		{
			uint_fast32_t freq = (uint_fast32_t) board_bandfs [i] << BANDDIVPOWER;
			PRINTF("board_bandfs[%d]=%lu\n", (int) i, (unsigned long) freq);
		}
	}
}

/* Частоы переключения bandf2 */

#if \
	WITHBANDF2_FT757 || \
	0

	// FT-757
	// 1.9, 3.5, 7, 10 & 14, 18 & 21, 24.5 & 30

	static const fseltype_t board_band2fs [] =
	{
		(fseltype_t) ( 2400000u >> BANDDIVPOWER),
		(fseltype_t) ( 3900000u >> BANDDIVPOWER),
		(fseltype_t) ( 7400000u >> BANDDIVPOWER),
		(fseltype_t) (14800000u >> BANDDIVPOWER),
		(fseltype_t) (22000000u >> BANDDIVPOWER),
		(fseltype_t) (30000000u >> BANDDIVPOWER),
	};
	#define BAND2CALCS	ARRAY_SIZE(board_band2fs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	CTLREGSTYLE_SW2013SF_V2 || \
	CTLREGSTYLE_SW2013SF_V3 || \
	0

	static const fseltype_t board_band2fs [] =
	{
		(fseltype_t) (3000000u >> BANDDIVPOWER),
		(fseltype_t) (5000000u >> BANDDIVPOWER),
		(fseltype_t) (9000000u >> BANDDIVPOWER),
	};
	#define BAND2CALCS	ARRAY_SIZE(board_band2fs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	WITH_R1YQ_2XRD70_POWERAMP || \
	0
	/* плата усилителя 2*RD100 UA1CEI */
	// R4DZ version: 1.8-2.0  3.5-4.0,   7-7.2,   10-10.2.  14-18.2    21-30
	// R4DZ version: 2.4 3.9 7.4 12.8 20 30

	static const fseltype_t board_band2fs [] =
	{
		(fseltype_t) ( 2400000u >> BANDDIVPOWER),
		(fseltype_t) ( 3900000u >> BANDDIVPOWER),
		(fseltype_t) ( 7400000u >> BANDDIVPOWER),
		(fseltype_t) (12800000u >> BANDDIVPOWER),
		(fseltype_t) (20000000u >> BANDDIVPOWER),
		(fseltype_t) (30000000u >> BANDDIVPOWER),
	};
	#define BAND2CALCS	ARRAY_SIZE(board_band2fs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	WITHAUTOTUNER_AVBELNN || \
	CTLREGMODE_STORCH_V6 ||		/* mimni RX + TFT 4.3" */ \
	0
	/* TUNER & PA board 2*RD16 by avbelnn@yandex.ru */
	// 2.4 3.9 7.4 14.8 22 30

	static const fseltype_t board_band2fs [] =
	{
		(fseltype_t) ( 2400000u >> BANDDIVPOWER),
		(fseltype_t) ( 3900000u >> BANDDIVPOWER),
		(fseltype_t) ( 7400000u >> BANDDIVPOWER),
		(fseltype_t) (14800000u >> BANDDIVPOWER),
		(fseltype_t) (22000000u >> BANDDIVPOWER),
		(fseltype_t) (30000000u >> BANDDIVPOWER),
	};
	#define BAND2CALCS	ARRAY_SIZE(board_band2fs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	WITHAUTOTUNER_UA1CEI_V2 || \
	0
	//	 2.000
	//	 3.850
	//	 7.200
	//	14.500
	//	21.500
	//	30.000

	static const fseltype_t board_band2fs [] =
	{
		(fseltype_t) ( 2000000u >> BANDDIVPOWER),
		(fseltype_t) ( 3850000u >> BANDDIVPOWER),
		(fseltype_t) ( 7200000u >> BANDDIVPOWER),
		(fseltype_t) (14500000u >> BANDDIVPOWER),
		(fseltype_t) (21500000u >> BANDDIVPOWER),
		(fseltype_t) (30000000u >> BANDDIVPOWER),
	};
	#define BAND2CALCS	ARRAY_SIZE(board_band2fs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
		WITH_PALPF_ICM710 || \
		0

	static const fseltype_t board_band2fs [] =
	{
		(fseltype_t) ( 2000000u >> BANDDIVPOWER),
		(fseltype_t) ( 3000000u >> BANDDIVPOWER),
		(fseltype_t) ( 5000000u >> BANDDIVPOWER),
		(fseltype_t) ( 7000000u >> BANDDIVPOWER),
		(fseltype_t) (14000000u >> BANDDIVPOWER),
		(fseltype_t) (20000000u >> BANDDIVPOWER),
	};
	#define BAND2CALCS	ARRAY_SIZE(board_band2fs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	CTLREGMODE_STORCH_V1 || \
	CTLREGMODE_STORCH_V2 || \
	CTLREGMODE_STORCH_V3 || \
	CTLREGMODE_STORCH_V7 || \
	CTLREGMODE_STORCH_V9 || \
	CTLREGMODE_STORCH_V9A || \
	0
	/* плата усилителя 2*RD100 UA1CEI */
	// 2.4 3.9 7.4 14.8 22 30

	static const fseltype_t board_band2fs [] =
	{
		(fseltype_t) ( 2400000u >> BANDDIVPOWER),
		(fseltype_t) ( 3900000u >> BANDDIVPOWER),
		(fseltype_t) ( 7400000u >> BANDDIVPOWER),
		(fseltype_t) (14800000u >> BANDDIVPOWER),
		(fseltype_t) (22000000u >> BANDDIVPOWER),
		(fseltype_t) (30000000u >> BANDDIVPOWER),
	};
	#define BAND2CALCS	ARRAY_SIZE(board_band2fs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	CTLREGMODE_RAVENDSP_V1 || /* Трансивер Вороненок с IF DSP трактом */ \
	CTLREGMODE_RAVENDSP_V5 || \
	CTLREGMODE_RAVENDSP_V6 || \
	CTLREGMODE_RAVENDSP_V7 || \
	CTLREGMODE_STORCH_V1_R4DR || \
	CTLREGMODE_STORCH_V4 ||		/* modem only v2 */ \
	CTLREGMODE_STORCH_V5 ||		/* mimni RX */ \
	1 || \
	0
	// 3 5 9 16 22 35

	static const fseltype_t board_band2fs [] =
	{
		(fseltype_t) ( 3000000u >> BANDDIVPOWER),
		(fseltype_t) ( 5000000u >> BANDDIVPOWER),
		(fseltype_t) ( 9000000u >> BANDDIVPOWER),
		(fseltype_t) (16000000u >> BANDDIVPOWER),
		(fseltype_t) (22000000u >> BANDDIVPOWER),
		(fseltype_t) (35000000u >> BANDDIVPOWER),
	};
	#define BAND2CALCS	ARRAY_SIZE(board_band2fs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#endif

#if 1
	// те трансиверы, у которых есть выход ACC

	/* границы диапазонов для управления через разъем ACC */
	/* FT-891 style */
	static const fseltype_t board_band3fs [] =
	{
		(fseltype_t) (  1700000u >> BANDDIVPOWER),
		(fseltype_t) (  2500000u >> BANDDIVPOWER),
		(fseltype_t) (  4100000u >> BANDDIVPOWER),
		(fseltype_t) (  7500000u >> BANDDIVPOWER),
		(fseltype_t) ( 11500000u >> BANDDIVPOWER),
		(fseltype_t) ( 14500000u >> BANDDIVPOWER),
		(fseltype_t) ( 20900000u >> BANDDIVPOWER),
		(fseltype_t) ( 21500000u >> BANDDIVPOWER),
		(fseltype_t) ( 25500000u >> BANDDIVPOWER),
		(fseltype_t) ( 29900000u >> BANDDIVPOWER),	// was: 41.5
		(fseltype_t) ( 56000000u >> BANDDIVPOWER),
	};
	#define BAND3CALCS	ARRAY_SIZE(board_band3fs)	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */
#else

#endif

/* получить номер диапазонного фильтра по частоте */
uint_fast8_t bandf_calc(
	uint_fast32_t freq
	 )
{
#ifdef BANDCALCS
  	const fseltype_t freqloc = (fseltype_t) (freq >> BANDDIVPOWER);	// приведённая к нужной размерности частота приёма

	uint_fast8_t bottom = 0;
	uint_fast8_t top = BANDCALCS - 1;
	// Двоичный поиск
	while (bottom < top)
	{
		const uint_fast8_t middle = (top - bottom) / 2 + bottom;

		if (board_bandfs [middle] > freqloc)
		{
			top = middle;	// нижняя граница диапазона значений текущего элемента сравнения больше значения поиска - продолжаем поиск в нижней половине
			continue;
		}
		if (board_bandfs [middle + 1] < freqloc)	
		{
			bottom = middle + 1;
			continue;
		}
		return middle + 1;
	}
	return bottom != 0 ? BANDCALCS : 0;

#else /* BANDCALCS */
	return 0;
#endif /* BANDCALCS */
}

void bandf2_calc_initialize(void)
{
#ifdef BAND2CALCS
	if (0)
	{
		/* отладочная печать границ диапазонов */
		uint_fast8_t i;
		for (i = 0; i < sizeof board_band2fs / sizeof board_band2fs [0]; ++ i)
		{
			uint_fast32_t freq = (uint_fast32_t) board_band2fs [i] << BANDDIVPOWER;
			PRINTF("board_band2fs[%d]=%lu\n", (int) i, (unsigned long) freq);
		}
	}
#endif /* BAND2CALCS */
}

/* получить номер диапазонного фильтра передатчика по частоте */
uint_fast8_t bandf2_calc(
	uint_fast32_t freq
	 )
{
#ifdef BAND2CALCS

  	const fseltype_t freqloc = (fseltype_t) (freq >> BANDDIVPOWER);	// приведённая к нужной размерности частота приёма

	uint_fast8_t bottom = 0;
	uint_fast8_t top = BAND2CALCS - 1;
	// Двоичный поиск
	while (bottom < top)
	{
		const uint_fast8_t middle = (top - bottom) / 2 + bottom;

		if (board_band2fs [middle] > freqloc)
		{
			top = middle;	// нижняя граница диапазона значений текущего элемента сравнения больше значения поиска - продолжаем поиск в нижней половине
			continue;
		}
		if (board_band2fs [middle + 1] < freqloc)
		{
			bottom = middle + 1;
			continue;
		}
		return middle + 1;
	}
	return bottom != 0 ? BAND2CALCS : 0;

#else
	return 0;
#endif
}


void bandf3_calc_initialize(void)
{
#ifdef BAND3CALCS
	if (0)
	{
		/* отладочная печать границ диапазонов */
		uint_fast8_t i;
		for (i = 0; i < sizeof board_band3fs / sizeof board_band3fs [0]; ++ i)
		{
			uint_fast32_t freq = (uint_fast32_t) board_band3fs [i] << BANDDIVPOWER;
			PRINTF("board_band3fs[%d]=%lu\n", (int) i, (unsigned long) freq);
		}
	}
#endif /* BAND3CALCS */
}

/* получить код для управления через разъем ACC */
uint_fast8_t bandf3_calc(
	uint_fast32_t freq
	 )
{
#ifdef BAND3CALCS

  	const fseltype_t freqloc = (fseltype_t) (freq >> BANDDIVPOWER);	// приведённая к нужной размерности частота приёма

	uint_fast8_t bottom = 0;
	uint_fast8_t top = BAND3CALCS - 1;
	// Двоичный поиск
	while (bottom < top)
	{
		const uint_fast8_t middle = (top - bottom) / 2 + bottom;

		if (board_band3fs [middle] > freqloc)
		{
			top = middle;	// нижняя граница диапазона значений текущего элемента сравнения больше значения поиска - продолжаем поиск в нижней половине
			continue;
		}
		if (board_band3fs [middle + 1] < freqloc)	
		{
			bottom = middle + 1;
			continue;
		}
		return middle + 1;
	}
	return bottom != 0 ? BAND3CALCS : 0;
#else
	return 0;
#endif
}
