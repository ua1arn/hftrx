/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "synthcalcs.h"
#include "board.h"
#include "formats.h"	/* sprintf() replacement */
#include "inc/spi.h"

//#define WITHSI5351AREPLACE 1

// LO1MODE_DIRECT
//	- одиночная DDS или целочисленная ФАПЧ с крупным шагом или дробная ФАПЧ с мелким шагом
//
// LO1MODE_HYBRID, 
// - ФАПЧ с переменным умножением, с опорой от DDS
//
// LO1MODE_FIXSCALE 
// - ФАПЧ с фиксированным умножением опоры от DDS
//
// LO1MODE_MAXLAB, 
// возможно с MAXLAB_DACARRAY - Управляющие напряжения для варикапа расчитываются заранее - хранятся в массиве
// - ФАПЧ с опорой от кварцевого генератора с уводом частоты варикапами от ЦАП

// статические переменные, не присваиваемые нигде - интерперетируются как константы
//#if defined(DDS1_TYPE) || defined(PLL1_TYPE)
	static const uint_fast8_t dds1refdiv = DDS1_CLK_DIV;
	static const uint_fast8_t dds1refmul = DDS1_CLK_MUL;
	static uint_fast32_t dds1ref; /* тактовая частота на входе DDS1 */
//#endif /* defined(DDS1_TYPE) */

#if defined(DDS2_TYPE)
	static const uint_fast8_t dds2refdiv = DDS2_CLK_DIV;
	static const uint_fast8_t dds2refmul = DDS2_CLK_MUL;
	static uint_fast32_t dds2ref; /* тактовая частота на входе DDS2 */
#else /* defined(DDS2_TYPE) */
	static const uint_fast8_t dds2refdiv = 1;
	static const uint_fast8_t dds2refmul = 1;
	static uint_fast32_t dds2ref; /* тактовая частота на входе DDS2 */
#endif /* defined(DDS2_TYPE) */

#if defined(DDS3_TYPE)
	static const uint_fast8_t dds3refdiv = DDS3_CLK_DIV;
	static const uint_fast8_t dds3refmul = DDS3_CLK_MUL;
	static uint_fast32_t dds3ref; /* тактовая частота на входе DDS3 */
#endif /* defined(DDS1_TYPE) */

#if defined(PLL1_TYPE)
	// статические переменные, не присваиваемые нигде - интерперетируются как константы
	const phase_t r1_ph = SYNTH_R1;

#endif /* defined(PLL1_TYPE) */

#if LO1MODE_HYBRID
	static uint_fast64_t ph1_min; // = freq2ftw(ddsosc, cf - df / 2);	/* приращение фазы, соответствующее минимальной частоте на выходе DDS */
#endif /* LO1MODE_HYBRID */


#if LO1MODE_MAXLAB
	//static uint_fast64_t ref1_min; // частота опорника (минимальная частота)
	static uint_fast64_t ref1_min_scaled; // частота опорника (минимальная частота) * MAXLAB_LINEARDACRANGE
	static uint_fast32_t df_maxlab;			// максимальный диапазон перестройки опорника на один шаг умножителя
#endif /* LO1MODE_MAXLAB */

#ifdef FTW_RESOLUTION
/* функция преобразования требуемой частоты в FTW при заданной тактовой частоте DDS */
/* divider - это делитель опорной частоты перед DDS, умноженный на делитель выходной частоты ЗА DDS. */
static ftw_t 
freq2ftw(uint_fast32_t freq, uint_fast16_t divider, uint_fast64_t ddsosc)
{
	const uint_fast64_t d = (uint_fast64_t) divider << FTW_RESOLUTION;
	return (freq * d) / ddsosc;
}
#endif /* FTW_RESOLUTION */

#if WITHLFM && LO1MODE_DIRECT


volatile uint_fast8_t spool_lfm_enable;
volatile uint_fast8_t spool_lfm_flag;

void spool_lfm(void)
{
	if (spool_lfm_enable != 0)
	{
		prog_pulse_ioupdate();
		spool_lfm_flag = 1;
		//return 1;
	}
	//return 0;
}

// Вызывается из обработчика PPS при совпадении времени начала.
void lfm_run(void)
{
	spool_lfm_enable = 1;
}

static void
board_waitextsync(void)
{

	for (;;)
	{
		uint_fast8_t f;
		disableIRQ();
		f = spool_lfm_flag;
		spool_lfm_flag = 0;
		enableIRQ();
		if (f != 0)
			break;	
	}
}


static void 
scanfreq(ftw_t ftw0, ftw_t ftw_last, long int lfm_nsteps)
{
	const ftw_t lfm_dy = ftw_last - ftw0; 
	const ftw_t lfm_SCALE = lfm_nsteps;
	const ftw_t lfm_SCALEDIV2 = lfm_nsteps / 2;
	ftw_t lfm_y1_scaled = ftw0 * lfm_SCALE;	// начальное значение
	long int lfm_position = 0;

	// Начальное выставление частоты, соответствующей началу сканируемого диапазона
	// Потом ждём первого импульса (в момент Tstart+delta), пришедьшего от аппаратуры синхронизации с GPS

	prog_dds1_ftw(& ftw0);


	//board_set_ddsext(1);	// внешнее управление IOUPDATE DDS1
	//board_update();

	spool_lfm_flag = 0;
	//spool_lfm_enable = 1;
	hardware_lfm_setupdatefreq(LFMTICKSFREQ);

	//display_freq(position, ftw0); // 
    do              
    {         
		// Подготавливаем параметры для программирования DDS
		const ftw_t lfm_value = ((lfm_y1_scaled += lfm_dy) + lfm_SCALEDIV2) / lfm_SCALE;
		// Выдаём FTW в микросхему синтезатора частоты, ждём прохода
		// очередного синхронизирующего импульса.
		board_waitextsync();
		disableIRQ();
		prog_dds1_ftw_noioupdate(& lfm_value);
		enableIRQ();
		//display_freq(position, value); // но это тоже может боком
    } while (++ lfm_position < lfm_nsteps);
	// цикл перестройки частоты завершён

	hardware_lfm_setupdatefreq(20);
	spool_lfm_enable = 0;

	//board_set_ddsext(0);	// внутреннее управление IOUPDATE DDS1
	//board_update();
} 

static long int lfm_speed;		// скорость перестройки герц / секунду
static long int lfm_start;	// начальная частота
static long int lfm_stop;	// конечная частота
static long int lfm_lo1div;	// делитель перед подачей на смеситель

void 
testlfm(void)
{
	enum { fi = 0 };	// номер тракта
	// LFMTICKSFREQ - количество обновлений частоты за секунду

	//lfm_speed = 100000;		// скорость перестройки герц / секунду
	//lfm_start = 2000000;	// начальная частота
	//lfm_stop =  9000000;	// конечная частота

	long int nsteps = (uint_fast64_t) (lfm_stop - lfm_start) * LFMTICKSFREQ / lfm_speed;
	//printf("nsteps = %d\n", nsteps);
	scanfreq(freq2ftw(synth_freq2lo1(lfm_start, fi), dds1refdiv * lfm_lo1div, dds1ref), freq2ftw(synth_freq2lo1(lfm_stop, fi), dds1refdiv * lfm_lo1div, dds1ref), nsteps);
}

void synth_lfm_setparams(uint_fast32_t astart, uint_fast32_t astop, uint_fast32_t aspeed, uint_fast8_t od)
{
	lfm_speed = aspeed;
	lfm_start = astart;
	lfm_stop = astop;
	lfm_lo1div = od;
} 

#else

void spool_lfm(void)
{
	//return 0;
}
#endif

#if LO1MODE_DIRECT && ! defined(DDS1_TYPE)

#if defined(PLL1_TYPE)

/* Целочисленные и дробные синтезаторы - зависит от PLL1_FRACTIONAL_LENGTH */

#if ! defined (PLL1_FRACTIONAL_LENGTH)
	#error PLL1_FRACTIONAL_LENGTH must be defibed
#endif /* ! defined (PLL1_FRACTIONAL_LENGTH) */

/* функция преобразования требуемой частоты в FTW при заданной частоте сравнения */
/* divider - это делитель опорной частоты перед DDS, умноженный на делитель выходной частоты ЗА DDS. */
static ftw_t 
pll1_freq2fract(uint_fast32_t freq, uint_fast16_t divider, uint_fast32_t pllref)
{
	const uint_fast64_t d = (uint_fast64_t) divider << PLL1_FRACTIONAL_LENGTH;
	return (freq * d) / pllref;
}

/* 	Синтезатор с дробной или целочисленной петлёй.
 */
static void synth_integer1_setfreq(
	uint_fast32_t f,			/* частота, которую хотим получить на выходе кольца ФАПЧ */
	uint_fast8_t om				/* умножитель перед подачей на смеситель (1, 2, 4, 8...) */
	)
{
	const pllhint_t hint = board_pll1_get_hint(f / om);	/* выбор требуемой конфигурации ГУН и делителя для данной частоты */
	const uint_fast16_t divider = board_pll1_get_divider(hint);	/* выходной делитель за VCO */
	const ftw_t n1 = pll1_freq2fract(f, divider * r1_ph, dds1ref * om); /* зависит от PLL1_FRACTIONAL_LENGTH */

	board_pll1_set_vco(hint);
	const uint_fast8_t fchange = board_pll1_set_n(& n1, hint, 1);
	if (fchange != 0)	// производилось перепрограммирование - разрешить работу PLL1
		board_pll1_set_n(& n1, hint, 0);
	board_pll1_set_vcodivider(hint);
}
#else
// stup для случая если установлен LO1MODE_DIRECT, но ни одной микросхемы
static void synth_integer1_setfreq(
	uint_fast32_t f,			/* частота, которую хотим получить на выходе кольца ФАПЧ */
	uint_fast8_t om				/* умножитель перед подачей на смеситель (1, 2, 4, 8...) */
	)
{
}
#endif

#elif LO1MODE_FIXSCALE

/* 	DDS выдает частоту сравнения для фазового детектора,
 *	от ГУН постоянный делитель с коэф. деления SYNTH_N1
 *	от DDS постоянный делитель с коэф. деления SYNTH_R1
 */
static void synth_scale1_setfreq(
	uint_fast32_t f,			/* частота, которую хотим получить на выходе кольца ФАПЧ */
	uint_fast8_t om				/* умножитель перед подачей на смеситель (1, 2, 4, 8...) */
	)
{
	const pllhint_t hint = board_pll1_get_hint(f / om);	/* выбор требуемой конфигурации ГУН и делителя для данной частоты */
	const uint_fast16_t divider = board_pll1_get_divider(hint);		/* выходной делитель за VCO */
	const ftw_t n1 = SYNTH_N1;		/* на сколько делится частота гетеродина в PLL */
	const ftw_t ph1 = freq2ftw(f, divider, dds1ref * (uint_fast64_t) SYNTH_N1 * om);    /* преобразование требуемой частоты в фазу */

	board_pll1_set_vco(hint);
	const uint_fast8_t fchange = board_pll1_set_n(& n1, hint, 1);
	prog_dds1_ftw(& ph1);
	if (fchange != 0)	// производилось перепрограммирование - разрешить работу PLL1
		board_pll1_set_n(& n1, hint, 0);
	board_pll1_set_vcodivider(hint);
}

#elif LO1MODE_MAXLAB

#if MAXLAB_DACARRAY

static uint_fast16_t dacvalues [MAXLAB_LINEARDACRANGE];

#endif /* MAXLAB_DACARRAY */

static uint_fast16_t ui16max(uint_fast16_t a, uint_fast16_t b)
{
	return a > b ? a : b;
}

static uint_fast16_t ui16min(uint_fast16_t a, uint_fast16_t b)
{
	return a < b ? a : b;
}


static uint_fast16_t
daci2dacval(
	uint_fast16_t i		// DAC value index 0 .. MAXLAB_LINEARDACRANGE-1
	)
{
	// заполнение массива значений для работы ЦАП
	// Аппроксимация по максимальной крутизне пересечением двух прямых
	// если наклон прямой #1 больше, чем у прямой #2
	// Аппроксимация по мтнимальной крутизне пересечением двух прямых
	// если наклон прямой #1 меньше, чем у прямой #2

	uint_fast16_t dac_min1 = 100;	// U в начале прямой #1
	uint_fast16_t dac_max1 = 500;	// U в конце прямой #1
	uint_fast16_t dac_min2 = 5;	// U в начале прямой #2
	uint_fast16_t dac_max2 = 1000;	// U в конце прямой #2

	const uint_fast16_t delta1 = (dac_max1 - dac_min1);
	const uint_fast16_t delta2 = (dac_max2 - dac_min2);

	const uint_fast16_t v1 = (uint_fast32_t) delta1 * i / MAXLAB_LINEARDACRANGE + dac_min1;
	const uint_fast16_t v2 = (uint_fast32_t) delta2 * i / MAXLAB_LINEARDACRANGE + dac_min2;
	if (delta1 > delta2)
		return ui16max(v1, v2);
	else
		return ui16min(v1, v2);
}


static void
synth_maxlabloop1_setfreq(
	uint_fast32_t f,		/* частота, которую хотим получить на выходе кольца ФАПЧ */
	uint_fast8_t om		/* умножитель перед подачей на смеситель (1, 2, 4, 8...) */
	)
{
	// MAXLAB_LINEARDACRANGE: до аппроксимации значения от 0 до 1023

	const pllhint_t hint = board_pll1_get_hint(f / om);	/* выбор требуемой конфигурации ГУН и делителя для данной частоты */
	// расчет параметров для указанной частоты
	const uint_fast64_t fscaled = f * r1_ph * MAXLAB_LINEARDACRANGE / om;   /* преобразование требуемой частоты в воображаемую (как при REF DIVIDER = 1) */
	const uint_fast64_t n1 = fscaled / ref1_min_scaled;	/* на сколько делится частота гетеродина в PLL */
	const uint_fast64_t fref_scaled = fscaled / n1;
	const uint_fast16_t dacindex = (fref_scaled - ref1_min_scaled) / df_maxlab;

#if MAXLAB_DACARRAY
	const uint_fast16_t dacvalue = dacvalues [dacindex < MAXLAB_LINEARDACRANGE ? dacindex : (MAXLAB_LINEARDACRANGE - 1)];	// работа на частотах ниже заявленной приводит к обращению за пределы таблицы
#else
	const uint_fast16_t dacvalue = daci2dacval(dacindex < MAXLAB_LINEARDACRANGE ? dacindex : (MAXLAB_LINEARDACRANGE - 1));	// работа на частотах ниже заявленной приводит к обращению за пределы таблицы
#endif
	// установка расчитанных значений
	board_pll1_set_vco(hint);
	const uint_fast8_t fchange = board_pll1_set_n(& n1, hint, 1);
	board_set_maxlabdac(dacvalue);
	if (fchange != 0)	// производилось перепрограммирование - разрешить работу PLL1
		board_pll1_set_n(& n1, hint, 0);
	board_pll1_set_vcodivider(hint);

#if WITHDEBUG
	const long fref2 = fref_scaled * 100 / MAXLAB_LINEARDACRANGE;
	debug_printf_P(PSTR("LM7001: fout=%ld.%03ld n1=%d DACi=%-4u DAC=%-4u (ref=%ld.%05ld)\n"), f / 1000, f % 1000, (int) n1, dacindex, dacvalue, fref2 / 100000, fref2 % 100000); 
#endif /* WITHDEBUG */
}

#elif LO1MODE_DIRECT

static void synth_direct1_setfreq(
	uint_fast8_t pathi,	/* номер тракта - 0/1: main/sub */
	uint_fast32_t f,	/* частота, которую хотим получить на выходе DDS */
	uint_fast8_t od,	/* делитель перед подачей на смеситель (1, 2, 4, 8...) */
	uint_fast8_t om		/* умножитель перед подачей на смеситель (1, 2, 4, 8...) */
	)
{
	//debug_printf_P(PSTR("synth_direct1_setfreq: pathi=%u, freq=%lu\n"), pathi, f);
#if defined(DDS1_TYPE)
	const ftw_t ph1 = freq2ftw(f, dds1refdiv * od, dds1ref * (uint_fast64_t) om);    /* преобразование требуемой частоты в фазу */
	switch (pathi)
	{
	case 0:
		prog_dds1_ftw(& ph1);
		break;
	case 1:
		prog_dds1_ftw_sub(& ph1);
		break;
	case 2:
		prog_dds1_ftw_sub3(& ph1);
		break;
	case 3:
		prog_dds1_ftw_sub4(& ph1);
		break;
	}
#endif
}

#elif LO1MODE_HYBRID && defined(DDS1_TYPE)

static void synth_loop1_setfreq(
	uint_fast8_t pathi,		/* номер тракта - 0/1: main/sub */
	uint_fast32_t f,		/* частота, которую хотим получить на выходе кольца ФАПЧ */
	uint_fast8_t om			/* умножитель перед подачей на смеситель (1, 2, 4, 8...) */
	)
{
	const pllhint_t hint = board_pll1_get_hint(f / om);
	const uint_fast16_t divider = board_pll1_get_divider(hint);	/* выходной делитель за VCO */
	const ftw_t t = freq2ftw(f, divider * dds1refdiv * SYNTH_R1, dds1ref * om);   /* преобразование требуемой частоты в воображаемую фазу */
	const ftw_t n1 = (t / ph1_min);		/* на сколько делится частота гетеродина в PLL */
	const ftw_t ph1a = t / n1;		/* FTW для DDS */
	//debug_printf_P(PSTR("synth_loop1_setfreq: f=%ld. n1=%ld, hint=%d, R1=%d\n"), (long) f, (long) n1, (int) hint, (int) SYNTH_R1);

	board_pll1_set_vco(hint);
	const uint_fast8_t fchange = board_pll1_set_n(& n1, hint, 1);
	prog_dds1_ftw(& ph1a);
	if (fchange != 0)	// производилось перепрограммирование - разрешить работу PLL1
		board_pll1_set_n(& n1, hint, 0);
	board_pll1_set_vcodivider(hint);
}
#endif


/* Возврат умножителя для первого гетеродина -
   во сколько умножается частота после синтезатора перед подачей на смеситель
 */
static uint_fast8_t
board_getscalelo1(
	uint_fast32_t f
	)
{
#if CTLSTYLE_SW2016VHF || CTLSTYLE_SW2018XVR
	// используется FQMODEL_45_IF8868_UHF144
	return 1;
#elif \
	FQMODEL_45_IF8868_UHF144 || FQMODEL_45_IF6000_UHF144 || \
	FQMODEL_45_IF8868_UHF430 || FQMODEL_45_IF6000_UHF430
	// в тракте требуется управление умножителем частоты первого гетродина
	if (f >= 250000000UL)
		return 8;
	if (f >= 85000000UL)
		return 2;
	return 1;

#else

	return 1;

#endif /* CTLSTYLE_SW2011 */
}

#if WITHSI5351AREPLACE
	#include "chip/si5351a.h"
#endif /* WITHSI5351AREPLACE */

void synth_lo1_setfrequ(
	uint_fast8_t pathi,		/* номер тракта - 0/1: main/sub */
	uint_fast32_t f,		/* частота, которую хотим получить на выходе LO1 */
	uint_fast8_t od			/* делитель перед подачей на смеситель (1, 2, 4, 8...) */
	)
{
	const uint_fast8_t om = board_getscalelo1(f * od);	/* Возврат умножителя для первого гетеродина - во сколько умножается частота после синтезатора перед подачей на смеситель */
	board_set_scalelo1(om);

#if WITHSI5351AREPLACE
	si5351aSetFrequencyA(f * od / om);
	return;
#endif /* WITHSI5351AREPLACE */

#if LO1MODE_DIRECT && ! defined(DDS1_TYPE)
	synth_integer1_setfreq(f * od, om);
#elif LO1MODE_DIRECT
	synth_direct1_setfreq(pathi, f, od, om);
#elif LO1MODE_HYBRID
	synth_loop1_setfreq(pathi, f * od, om);
#elif LO1MODE_MAXLAB
	synth_maxlabloop1_setfreq(pathi, f * od, om);
#elif LO1MODE_FIXSCALE
	synth_scale1_setfreq(pathi, f * od, om);
#else
	#error Select proper LO1MODE_XXX define!
#endif
}

/* установка частоты первого гетеродина, требуемой для приема */
void synth_lo1_setfreq(
	uint_fast8_t pathi,		/* номер тракта - 0/1: main/sub */
	int_fast32_t f,			/* частота, которую хотим получить на выходе LO1 */
	uint_fast8_t od			/* делитель перед подачей на смеситель (1, 2, 4, 8...) */
	)
{
	if (f < 0)
		f = - f;
	synth_lo1_setfrequ(pathi, f, od);
}

void synth_lo4_setfreq(
	uint_fast8_t pathi,		/* номер тракта - 0/1: main/sub */
	int_fast32_t f,			/* частота, которую хотим получить на выходе DDS */
	uint_fast8_t od,		/* делитель перед подачей на смеситель (1, 2, 4, 8...) */
	int_fast8_t enable
	)
{
	if (enable == 0)
		f = 0;
	else if (f < 0)
		f = - f;

#if WITHSI5351AREPLACE
	si5351aSetFrequencyB(f);
	return;
#endif /* WITHSI5351AREPLACE */

#if defined(DDS2_TYPE)
	ftw_t ph = freq2ftw(f, dds2refdiv * od, dds2ref);    /* преобразование требуемой частоты в фазу */
	prog_dds2_ftw(& ph);
#endif
}


// Установка частоты второго гетеродина в случае использования DDS3 ad9951
void synth_lo2_setfreq(
	uint_fast8_t pathi,		/* номер тракта - 0/1: main/sub */
	int_fast32_t f,			/* частота, которую хотим получить на выходе DDS */
	uint_fast8_t od			/* делитель перед подачей на смеситель (1, 2, 4, 8...) */
	)
{
	if (f < 0)
		f = - f;
#if LO2_DDSGENERATED
	#if defined(DDS3_TYPE)
		const ftw_t ph = freq2ftw(f, dds3refdiv * od, dds3ref);    /* преобразование требуемой частоты в фазу */
		prog_dds3_ftw(& ph);
	#else
		(void) od;
	#endif
#endif /* LO2_DDSGENERATED */
}

// Установка частоты гетеродина, осуществляющего passband tuning - PBT
void synth_lo3_setfreq(
	uint_fast8_t pathi,		/* номер тракта - 0/1: main/sub */
	int_fast32_t f,			/* частота, которую хотим получить на выходе DDS */
	uint_fast8_t od			/* делитель перед подачей на смеситель (1, 2, 4, 8...) */
	)
{
	if (f < 0)
		f = - f;
#if ! LO2_DDSGENERATED
	#if defined(DDS3_TYPE)
		const ftw_t ph = freq2ftw(f, dds3refdiv * od, dds3ref);    /* преобразование требуемой частоты в фазу */
		prog_dds3_ftw(& ph);
	#else
		(void) od;
	#endif
#endif /* ! LO2_DDSGENERATED */
}


// Установка центральной частоты панорамного индикатора
void synth_rts1_setfreq(
	uint_fast8_t pathi,		/* номер тракта - 0/1: main/sub */
	int_fast32_t f	/* частота, которую хотим получить в центре панорамы */
	)
{
	if (pathi == 0)
	{
		/* есть только один приемник панорамы */
		if (f < 0)
			f = - f;
		//debug_printf_P(PSTR("synth_rts1_setfreq: freq=%lu\n"), f);
	#if (WITHRTS192 || WITHRTS96) && WITHDSPEXTDDC
		const ftw_t ph = freq2ftw(f, dds1refdiv, dds1ref);    /* преобразование требуемой частоты в фазу */
		prog_rts1_ftw(& ph);
	#endif /* (WITHRTS192 || WITHRTS96) && WITHDSPEXTDDC */
	}
}

// установка частоты конвертора
void synth_lo0_setfreq(
	int_fast32_t f,	/* частота, которую хотим получить на выходе синтезатора */
	uint_fast8_t enable		/* включение конвертора */
	)
{
	if (f < 0)
		f = - f;
#if CTLSTYLE_OLEG4Z_V1
	prog_xvtr_freq(f, enable);
#endif /* CTLSTYLE_OLEG4Z_V1 */
}

// Установка опорной частоты длд синтезатороа LO1.
void
synth_lo1_setreference(
	uint_fast32_t refclock
	)
{
//#if defined(DDS1_TYPE) || defined(PLL1_TYPE)
	dds1ref =  refclock * dds1refmul;   /* тактовая частота на входе DDS1 */
//#endif

#if LO1MODE_HYBRID

	const long freqlow = SYNTHLOWFREQ;	/* минимальная выходная частота синтезатора */
	/* Константы синтезатора - расчитываются один раз и зависят от диапазона рабочих частот, фильтра и частоты сравнения */
	//r1_ph = SYNTH_R1;			/* делитель после DDS - коэфециент деления опоры для PLL */
	//freqlow = SYNTHLOWFREQ;	/* минимальная выходная частота синтезатора */


    const int_fast64_t cf =   SYNTH_CFCENTER;   /* Центр полосы пропускания фильтра после DDS */
    const int_fast64_t df =   (ftw_t) (((ftw_t) cf * cf) / (freqlow * r1_ph));   /* требуемая полоса пропускания фильтра после DDS */
    ph1_min = freq2ftw(cf - df / 2, dds1refdiv, dds1ref);	/* фаза, соответствующая минимальной частоте на выходе DDS */

#elif LO1MODE_MAXLAB

	// расчет параметров для заданной опорной частоты (минимальная частота подстраиваемого кварцевого генератора).
	// делается один раз при инициализации или подстройке частоты из меню.
	const long freqlow = SYNTHLOWFREQ;	/* минимальная выходная частота синтезатора */

	uint_fast64_t ref1_min = refclock;	 // частота опорника
	ref1_min_scaled = (uint_fast64_t) refclock * MAXLAB_LINEARDACRANGE;
    df_maxlab = ((uint_fast64_t) ref1_min * ref1_min) / (freqlow * r1_ph);   /* требуемая полоса пропускания фильтра после DDS */
#if WITHDEBUG
	const long df2 = df_maxlab * 100;
	debug_printf_P(PSTR("LM7001: df=%ld.%05ld\n"), df2 / 100000, df2 % 100000); 
#endif /* WITHDEBUG */

	// заполнение массива значений для работы ЦАП
	// Аппроксимация по максимальной крутизне пересечением двух прямых
	// если наклон прямой #1 больше, чем у прямой #2
	// Аппроксимация по мтнимальной крутизне пересечением двух прямых
	// если наклон прямой #1 меньше, чем у прямой #2
#if MAXLAB_DACARRAY

	uint_fast16_t i;
	for (i = 0; i < MAXLAB_LINEARDACRANGE; ++ i)
	{
		dacvalues [i] = daci2dacval(i);
	}

#endif

#elif LO1MODE_FIXSCALE

	// do nothing

#elif LO1MODE_DIRECT

	// do nothing

#elif LO1MODE_DIRECT && ! defined(DDS1_TYPE)

	// do nothing

#endif

}

// Установка опорной частоты длд всех синтезаторов, кроме LO1.
void synth_setreference(
	uint_fast32_t refclock 	/* частота опорника */
	)
{

//#if defined(DDS2_TYPE)
	dds2ref =  refclock * dds2refmul;   /* тактовая частота на входе DDS2 */
//#endif

#if defined(DDS3_TYPE)
	dds3ref =  refclock * dds3refmul;   /* тактовая частота на входе DDS3 */
#endif
}

void synthcalc_init(void)
{
#if WITHSI5351AREPLACE
	si5351aInitialize();
#endif /* WITHSI5351AREPLACE */
#if LO1MODE_DIRECT && ! defined(DDS1_TYPE)
#elif LO1MODE_HYBRID
#elif LO1MODE_MAXLAB
#elif LO1MODE_FIXSCALE
#elif LO1MODE_DIRECT
#endif
}
