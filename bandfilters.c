/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "synthcalcs.h"

// На 8-битном микропроцессоре для ускорения вычислений
// частота округляется (отбрасываются младшие 16 бит).

#if CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA

	#define BANDDIVPOWER	16	/* ~65.5 kHz granulation */
	typedef uint_fast16_t fseltype_t;

#elif CPUSTYLE_ARM

	#define BANDDIVPOWER	0	/* 1 Hz granulation */
	typedef uint_fast32_t fseltype_t;

#else

	#error Undefined CPUSTYLE_XXX

#endif

#if BANDSELSTYLERE_RX3QSP

	// диапазонные фильтры по запросу RX3QSP

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (1000000UL >> BANDDIVPOWER),	
		(fseltype_t) (2000000UL >> BANDDIVPOWER),
		(fseltype_t) (4000000UL >> BANDDIVPOWER),
		(fseltype_t) (8000000UL >> BANDDIVPOWER),
		(fseltype_t) (15000000UL >> BANDDIVPOWER),
		(fseltype_t) (16100000UL >> BANDDIVPOWER),
		(fseltype_t) (19530000UL >> BANDDIVPOWER),
		(fseltype_t) (23100000UL >> BANDDIVPOWER),
		(fseltype_t) (26450000UL >> BANDDIVPOWER),
	};
	#define BANDCALCS (sizeof board_bandfs / sizeof board_bandfs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif CTLREGMODE16_NIKOLAI

	// диапазонные фильтры по запросу RX3QSP

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (1600000UL >> BANDDIVPOWER),	
		(fseltype_t) (5500000UL >> BANDDIVPOWER),
		(fseltype_t) (15000000UL >> BANDDIVPOWER),
	};
	#define BANDCALCS (sizeof board_bandfs / sizeof board_bandfs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	BANDSELSTYLERE_RA4YBO_AM0 || \
	0
	// 2.2 MHz
	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (2200000UL >> BANDDIVPOWER),
	};
	#define BANDCALCS (sizeof board_bandfs / sizeof board_bandfs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif BANDSELSTYLERE_R3PAV 

	// 8 диапазонных ФНЧ где 12м и 10м совмещенны для дешифратора 74HCT238

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (  1600000UL >> BANDDIVPOWER), 
		(fseltype_t) (  2100000UL >> BANDDIVPOWER), // 0 001  1.6 -  2.1
		(fseltype_t) (  3800000UL >> BANDDIVPOWER), // 0 010  2.1 -  3.8
		(fseltype_t) (  8100000UL >> BANDDIVPOWER), // 0 011  3.8 -  8.1
		(fseltype_t) ( 10300000UL >> BANDDIVPOWER), // 0 100  8.1 - 10.3
		(fseltype_t) ( 16000000UL >> BANDDIVPOWER), // 0 101 10.3 - 16.0
		(fseltype_t) ( 18400000UL >> BANDDIVPOWER), // 0 110 16.0 - 18.4
		(fseltype_t) ( 21400000UL >> BANDDIVPOWER), // 0 111 18.4 - 21.4
		(fseltype_t) ( 29900000UL >> BANDDIVPOWER), // 1 000 21.4 - 29.9
		(fseltype_t) ( 45000000UL >> BANDDIVPOWER), 
		(fseltype_t) (100000000UL >> BANDDIVPOWER), 
		(fseltype_t) (250000000UL >> BANDDIVPOWER), 

	};
	#define BANDCALCS (sizeof board_bandfs / sizeof board_bandfs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif REQUEST_FOR_RN4NAB

	// диапазонные фильтры как в SDR RA4NAL http://ra4nal.qrz.ru http://ra4nal.lanstek.ru

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (2500000UL >> BANDDIVPOWER),
		(fseltype_t) (4000000UL >> BANDDIVPOWER),
		(fseltype_t) (6000000UL >> BANDDIVPOWER),
		(fseltype_t) (7300000UL >> BANDDIVPOWER),
		(fseltype_t) (12000000UL >> BANDDIVPOWER),
		(fseltype_t) (14500000UL >> BANDDIVPOWER),
		(fseltype_t) (21500000UL >> BANDDIVPOWER),
		(fseltype_t) (30000000UL >> BANDDIVPOWER),
	};
	#define BANDCALCS (sizeof board_bandfs / sizeof board_bandfs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif STYLE_TS850_V1

	/* up-conversion RX */

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (500000UL >> BANDDIVPOWER),	
		(fseltype_t) (1620000UL >> BANDDIVPOWER),
		(fseltype_t) (2500000UL >> BANDDIVPOWER),
		(fseltype_t) (4000000UL >> BANDDIVPOWER),
		(fseltype_t) (7000000UL >> BANDDIVPOWER),
		(fseltype_t) (7500000UL >> BANDDIVPOWER),
		(fseltype_t) (10500000UL >> BANDDIVPOWER),
		(fseltype_t) (14000000UL >> BANDDIVPOWER),
		(fseltype_t) (14500000UL >> BANDDIVPOWER),
		(fseltype_t) (21000000UL >> BANDDIVPOWER),
		(fseltype_t) (22000000UL >> BANDDIVPOWER),
		(fseltype_t) (30000000UL >> BANDDIVPOWER),
	};
	#define BANDCALCS (sizeof board_bandfs / sizeof board_bandfs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	CTLSTYLE_RA4YBO || \
	0

	/* up-conversion RX */
	/* for version 0...1.6, 1.6...56.0 MHz - 8 bands. */

	#define BANDF_FREQMIN 1600000UL
	#define BANDF_FREQ_SCALE 1661809UL
	#define BANDF_FREQ_DENOM 1000000UL

	#define BANDCALCS	12	//	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */
	#define BANDF_USE_BANDINIT 1	/* необходимость функции инициализации. */

	static fseltype_t board_bandfs [BANDCALCS];

#elif \
	CTLSTYLE_RA4YBO_V1 || \
	CTLSTYLE_RA4YBO_V2 || \
	CTLSTYLE_RA4YBO_V3 || \
	0

	/* up-conversion RX */

	#define BANDF_FREQMIN 1600000UL
	#define BANDF_FREQ_SCALE 1661809UL
	#define BANDF_FREQ_DENOM 1000000UL

	#define BANDCALCS	13	//	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */
	#define BANDF_USE_BANDINIT 1	/* необходимость функции инициализации. */

	static fseltype_t board_bandfs [BANDCALCS];

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
		(fseltype_t) (1600000UL >> BANDDIVPOWER),
		(fseltype_t) (2645751UL >> BANDDIVPOWER),
		(fseltype_t) (5157518UL >> BANDDIVPOWER),
		(fseltype_t) (8527602UL >> BANDDIVPOWER),
		(fseltype_t) (11920570UL >> BANDDIVPOWER),
		(fseltype_t) (16102043UL >> BANDDIVPOWER),
		(fseltype_t) (19532741UL >> BANDDIVPOWER),
		(fseltype_t) (23106070UL >> BANDDIVPOWER),
		(fseltype_t) (26452221UL >> BANDDIVPOWER),
		(fseltype_t) (45000000UL >> BANDDIVPOWER),
		(fseltype_t) (100000000UL >> BANDDIVPOWER),
		(fseltype_t) (250000000UL >> BANDDIVPOWER),
	};
	#define BANDCALCS	(sizeof board_bandfs / sizeof board_bandfs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	BANDSELSTYLERE_LOCONV15M_NLB || \
	BANDSELSTYLERE_LOCONV32M_NLB || \
	0

	// диапазонные фильтры ориентированы на любительские диапазоны
	/* for version 1.6...32.0 MHz - 9 bands. */
	//#define BANDF_FREQMIN 1600000UL

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (2645751UL >> BANDDIVPOWER),
		(fseltype_t) (5157518UL >> BANDDIVPOWER),
		(fseltype_t) (8527602UL >> BANDDIVPOWER),
		(fseltype_t) (11920570UL >> BANDDIVPOWER),
		(fseltype_t) (16102043UL >> BANDDIVPOWER),
		(fseltype_t) (19532741UL >> BANDDIVPOWER),
		(fseltype_t) (23106070UL >> BANDDIVPOWER),
		(fseltype_t) (26452221UL >> BANDDIVPOWER),
		(fseltype_t) (45000000UL >> BANDDIVPOWER),
		(fseltype_t) (100000000UL >> BANDDIVPOWER),
		(fseltype_t) (250000000UL >> BANDDIVPOWER),
	};
	#define BANDCALCS	(sizeof board_bandfs / sizeof board_bandfs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	BANDSELSTYLERE_UPCONV56M || \
	BANDSELSTYLERE_UPCONV56M_45M || \
	BANDSELSTYLERE_UPCONV56M_36M || \
	0

	/* up-conversion RX */
	/* for version 0...1.6, 1.6...56.0 MHz - 8 bands. */
	#define BANDF_FREQMIN 1600000UL
	#define BANDF_FREQ_SCALE 1661809UL
	#define BANDF_FREQ_DENOM 1000000UL

	#define BANDCALCS	7	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */
	#define BANDF_USE_BANDINIT 1	/* необходимость функции инициализации. */

	static fseltype_t board_bandfs [BANDCALCS];

#elif \
	BANDSELSTYLE_OLEG4Z || \
	0

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (  500000uL >> BANDDIVPOWER),
		(fseltype_t) ( 1600000uL >> BANDDIVPOWER),
		(fseltype_t) ( 2300000uL >> BANDDIVPOWER),
		(fseltype_t) ( 3300000uL >> BANDDIVPOWER),
		(fseltype_t) ( 5000000uL >> BANDDIVPOWER),
		(fseltype_t) ( 6700000uL >> BANDDIVPOWER),
		(fseltype_t) (10700000uL >> BANDDIVPOWER),
		(fseltype_t) (15500000uL >> BANDDIVPOWER),
		(fseltype_t) (20300000uL >> BANDDIVPOWER),
		(fseltype_t) (32000000uL >> BANDDIVPOWER),
		(fseltype_t) (50000000uL >> BANDDIVPOWER),
	};
	#define BANDCALCS	(sizeof board_bandfs / sizeof board_bandfs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	BANDSELSTYLE_OLEG4Z_V2 || \
	0

	static const fseltype_t board_bandfs [] =
	{
		(fseltype_t) (  500000uL >> BANDDIVPOWER),	// #1 BPF_ON_2		500 kHz-1600 kHz
		(fseltype_t) (  1600000uL >> BANDDIVPOWER),	// #2 BPF_ON_3		1,6 MHz-2,3 MHz
		(fseltype_t) (  2300000uL >> BANDDIVPOWER),	// #3 BPF_ON_4		2,3 MHz-3,3 MHz
		(fseltype_t) (  3300000uL >> BANDDIVPOWER),	// #4 BPF_ON_5		3,3 MHz-5 MHz
		(fseltype_t) (  5000000uL >> BANDDIVPOWER),	// #5 BPF_ON_6		5 MHz-6,7 MHz
		(fseltype_t) (  6700000uL >> BANDDIVPOWER),	// #6 BPF_ON_7		6,7 MHz-10,7 MHz
		(fseltype_t) ( 10700000uL >> BANDDIVPOWER),	// #7 BPF_ON_8		10,7 MHz-15,5 MHz
		(fseltype_t) ( 15500000uL >> BANDDIVPOWER),	// #8 BPF_ON_9		15,5 MHz-20,3 MHz
		(fseltype_t) ( 20300000uL >> BANDDIVPOWER),	// #9 BPF_ON_10	20,3 MHz-32 MHz
		(fseltype_t) ( 32000000uL >> BANDDIVPOWER),	// #10 BPF_ON_11	32 MHz-50 MHz
		(fseltype_t) ( 50000000uL >> BANDDIVPOWER),	// #11 BPF_ON_12	50 MHz-120 MHz
		(fseltype_t) (120000000uL >> BANDDIVPOWER),	// #12 BPF_ON_13	120 MHz-265 MHz
		(fseltype_t) (260000000uL >> BANDDIVPOWER),	// #13 BPF_ON_14	260 MHz-390 MHz
		(fseltype_t) (350000000uL >> BANDDIVPOWER),	// #14 0	350-520 MHz
		(fseltype_t) (390000000uL >> BANDDIVPOWER),	// #15 1	520-700 MHz
		(fseltype_t) (700000000uL >> BANDDIVPOWER),	// #16 2	700-900 MHz
		(fseltype_t) (900000000uL >> BANDDIVPOWER),	// #17 3	900-1200 MHz
		(fseltype_t) (1200000000uL >> BANDDIVPOWER),// #18 4	1200-1700 MHz
	};
	#define BANDCALCS	(sizeof board_bandfs / sizeof board_bandfs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	BANDSELSTYLE_LADVABEST || \
	0

	// ladvabest@gmail.com

	static const fseltype_t board_bandfs [] =
	{
		//(fseltype_t) (1600000 >> BANDDIVPOWER), // 2.645751 #0 1 выв.
		(fseltype_t) (2645751 >> BANDDIVPOWER), // 5.157518 #1 2 выв.
		(fseltype_t) (5157518 >> BANDDIVPOWER), // 8.527602 #2 3 выв.
		(fseltype_t) (8527602 >> BANDDIVPOWER), // 11.920570 #3 4 выв.
		(fseltype_t) (11920570 >> BANDDIVPOWER), // 16.102043 #4 5 выв.
		(fseltype_t) (16102043 >> BANDDIVPOWER), // 19.532741 #5 6 выв.
		(fseltype_t) (19532741 >> BANDDIVPOWER), // 23.106070 #6 7 выв.
		(fseltype_t) (23106070 >> BANDDIVPOWER), // 26.452221 #7 8 выв.
		(fseltype_t) (26452221 >> BANDDIVPOWER), // 27.980000 #8 9 выв.
		(fseltype_t) (27980000 >> BANDDIVPOWER), // 28.690000 #9 10 выв.
		(fseltype_t) (28690000 >> BANDDIVPOWER), // 35.000000 #10 11 выв.
		(fseltype_t) (35000000 >> BANDDIVPOWER), // 48.000000 #11 13 выв.
		(fseltype_t) (48000000 >> BANDDIVPOWER), // 100.000000 #12 14 выв.
		(fseltype_t) (100000000 >> BANDDIVPOWER), // 250.000000 #13 15 выв.
	};
	#define BANDCALCS	(sizeof board_bandfs / sizeof board_bandfs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

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
#elif \
	BANDSELSTYLERE_RA4YBO_AM0 || \
	0
	// Вычисления производятся в bandf_calc
#else
	#error No band selection hardware supported
#endif

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
#if CTLSTYLE_RA4YBO
		board_bandfs [7] = (fseltype_t) (56000000UL >> BANDDIVPOWER);
		board_bandfs [8] = (fseltype_t) (80000000UL >> BANDDIVPOWER);
		board_bandfs [9] = (fseltype_t) (100000000UL >> BANDDIVPOWER);
		board_bandfs [10] = (fseltype_t) (120000000UL >> BANDDIVPOWER);
		board_bandfs [11] = (fseltype_t) (140000000UL >> BANDDIVPOWER);
#endif /* CTLSTYLE_RA4YBO */
#if CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2
		board_bandfs [7] = (fseltype_t) (60000000UL >> BANDDIVPOWER);
		board_bandfs [8] = (fseltype_t) (90000000UL >> BANDDIVPOWER);
		board_bandfs [9] = (fseltype_t) (111000000UL >> BANDDIVPOWER);
		board_bandfs [10] = (fseltype_t) (120000000UL >> BANDDIVPOWER);
		board_bandfs [11] = (fseltype_t) (140000000UL >> BANDDIVPOWER);
		board_bandfs [12] = (fseltype_t) (160000000UL >> BANDDIVPOWER);
#endif /* CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 */
#endif /* BANDF_USE_BANDINIT */
}

void bandf2_calc_initialize(void)
{
}

#if \
	CTLREGSTYLE_SW2013SF_V2 || \
	CTLREGSTYLE_SW2013SF_V3 || \
	0

	static const fseltype_t board_band2fs [] =
	{
		(fseltype_t) (3000000uL >> BANDDIVPOWER),
		(fseltype_t) (5000000uL >> BANDDIVPOWER),
		(fseltype_t) (9000000uL >> BANDDIVPOWER),
	};
	#define BAND2CALCS	(sizeof board_band2fs / sizeof board_band2fs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	(CTLSTYLE_STORCH_V8 && ARM_R7S72_TQFP176_CTLSTYLE_STORCH_V8_R4DZ_H_INCLUDED) || \
	0
	/* плата усилителя 2*RD100 UA1CEI */
	// R4DZ version: 1.8-2.0  3.5-4.0,   7-7.2,   10-10.2.  14-18.2    21-30
	// R4DZ version: 2.4 3.9 7.4 12.8 20 30

	static const fseltype_t board_band2fs [] =
	{
		(fseltype_t) ( 2400000uL >> BANDDIVPOWER),
		(fseltype_t) ( 3900000uL >> BANDDIVPOWER),
		(fseltype_t) ( 7400000uL >> BANDDIVPOWER),
		(fseltype_t) (12800000uL >> BANDDIVPOWER),
		(fseltype_t) (20000000uL >> BANDDIVPOWER),
		(fseltype_t) (30000000uL >> BANDDIVPOWER),
	};
	#define BAND2CALCS	(sizeof board_band2fs / sizeof board_band2fs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	CTLREGMODE_STORCH_V1 || \
	CTLREGMODE_STORCH_V2 || \
	CTLREGMODE_STORCH_V3 || \
	CTLREGMODE_STORCH_V7 || \
	CTLREGMODE_STORCH_V9 || \
	0
	/* плата усилителя 2*RD100 UA1CEI */
	// 2.4 3.9 7.4 14.8 22 30

	static const fseltype_t board_band2fs [] =
	{
		(fseltype_t) ( 2400000uL >> BANDDIVPOWER),
		(fseltype_t) ( 3900000uL >> BANDDIVPOWER),
		(fseltype_t) ( 7400000uL >> BANDDIVPOWER),
		(fseltype_t) (14800000uL >> BANDDIVPOWER),
		(fseltype_t) (22000000uL >> BANDDIVPOWER),
		(fseltype_t) (30000000uL >> BANDDIVPOWER),
	};
	#define BAND2CALCS	(sizeof board_band2fs / sizeof board_band2fs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */
#elif \
	CTLREGMODE_STORCH_V6 ||		/* mimni RX + TFT 4.3" */ \
	0
	/* TUNER & PA board 2*RD16 by avbelnn@yandex.ru */
	// 2.4 3.9 7.4 14.8 22 30

	static const fseltype_t board_band2fs [] =
	{
		(fseltype_t) ( 2400000uL >> BANDDIVPOWER),
		(fseltype_t) ( 3900000uL >> BANDDIVPOWER),
		(fseltype_t) ( 7400000uL >> BANDDIVPOWER),
		(fseltype_t) (14800000uL >> BANDDIVPOWER),
		(fseltype_t) (22000000uL >> BANDDIVPOWER),
		(fseltype_t) (30000000uL >> BANDDIVPOWER),
	};
	#define BAND2CALCS	(sizeof board_band2fs / sizeof board_band2fs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#elif \
	CTLREGMODE_RAVENDSP_V1 || /* Трансивер Вороненок с IF DSP трактом */ \
	CTLREGMODE_RAVENDSP_V3 || /* "Воронёнок" с DSP и FPGA */ \
	CTLREGMODE_RAVENDSP_V4 || \
	CTLREGMODE_RAVENDSP_V5 || \
	CTLREGMODE_RAVENDSP_V6 || \
	CTLREGMODE_RAVENDSP_V7 || \
	CTLREGMODE_STORCH_V1_R4DR || \
	CTLREGMODE_STORCH_V4 ||		/* modem only v2 */ \
	CTLREGMODE_STORCH_V5 ||		/* mimni RX */ \
	CTLREGMODE_RAVENDSP_V9 || /* renesas */ \
	1 || \
	0
	// 3 5 9 16 22 35

	static const fseltype_t board_band2fs [] =
	{
		(fseltype_t) ( 3000000uL >> BANDDIVPOWER),
		(fseltype_t) ( 5000000uL >> BANDDIVPOWER),
		(fseltype_t) ( 9000000uL >> BANDDIVPOWER),
		(fseltype_t) (16000000uL >> BANDDIVPOWER),
		(fseltype_t) (22000000uL >> BANDDIVPOWER),
		(fseltype_t) (35000000uL >> BANDDIVPOWER),
	};
	#define BAND2CALCS	(sizeof board_band2fs / sizeof board_band2fs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */

#endif

#if 1
	// те трансиверы, у которых есть выход ACC

	/* границы диапазонов для управления через разъем ACC */
	/* FT-891 style */
	static const fseltype_t board_band3fs [] =
	{
		(fseltype_t) (  1700000UL >> BANDDIVPOWER),
		(fseltype_t) (  2500000UL >> BANDDIVPOWER),
		(fseltype_t) (  4100000UL >> BANDDIVPOWER),
		(fseltype_t) (  7500000UL >> BANDDIVPOWER),
		(fseltype_t) ( 11500000UL >> BANDDIVPOWER),
		(fseltype_t) ( 14500000UL >> BANDDIVPOWER),
		(fseltype_t) ( 20900000UL >> BANDDIVPOWER),
		(fseltype_t) ( 21500000UL >> BANDDIVPOWER),
		(fseltype_t) ( 25500000UL >> BANDDIVPOWER),
		(fseltype_t) ( 41500000UL >> BANDDIVPOWER),
		(fseltype_t) ( 56000000UL >> BANDDIVPOWER),
	};
	#define BAND3CALCS	(sizeof board_band3fs / sizeof board_band3fs [0])	/* Размерность массива границ диапазонов и необходимость функции поиска по нему. */
#else

#endif

void bandf3_calc_initialize(void)
{
}

/* получить номер диапазонного фильтра по частоте */
uint8_t bandf_calc(
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

/* получить номер диапазонного фильтра передатчика по частоте */
uint8_t bandf2_calc(
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

/* получить код для управления через разъем ACC */
uint8_t bandf3_calc(
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
