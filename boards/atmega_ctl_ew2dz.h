/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Плата UA3DKC Сергей (ATMega32, FM25040, Si570+AD9834+WH2002) - ref 40 MHz
// только с "обычным" регистром управления. AD9834 формирует сигнал переноса с 5530 кГц на 500 кГц
//

#ifndef ATMEGA_CTL_EW2DZ_H_INCLUDED
#define ATMEGA_CTL_EW2DZ_H_INCLUDED 1

#if F_CPU != 8000000
	#error Set F_CPU right value in project file
#endif
	/* модели синтезаторов - схемы частотообразования */

	#define	MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */
	#define LO1MODE_DIRECT	1

	#if 1
		#define KEYB_VERTICAL_REV	1	/* расположение кнопок для плат "Воробей" и "Колибри" */
		// EW2DZ
		/* Версии частотных схем - с преобразованием "вниз" */
		/*
			Исходные данные такие: ПЧ - 5 МГц; индикатор - RDX0154; память - 25L160AI; 
			режимы - SSB и CW; фильтры - 3.0 кГц и 1.8 кГц; генератор - 48 МГц; 
			только две 595-ые и клавиатура будет 4 ряда по 3 кнопки.
		*/
		#define FQMODEL_TRX8M		1	// Первая низкая ПЧ 8 МГц, 6 МГц, 5.5 МГц и другие
		#define IF3_MODEL IF3_TYPE_5000
		#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
		#define LCDMODE_RDX0154	1				/* тип применяемого индикатора  */
		#define DIRECT_48M0_X1		1
		#define CTLREGMODE16	1		/* управляющий регистр 16 бит - для SIMPLE синтезатора на AD8932/AD8935/AD8933	*/
		#define WITHMODESETMIXONLY 1		/* EW2DZ version */
		/* набор фильтров в тракте */
		#define IF3_FMASK	(IF3_FMASK_1P8 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FHAVE	(IF3_FMASK_1P8 | IF3_FMASK_3P1)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
		/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
		#define BOARD_FILTER_0P5		BOARD_FILTERCODE_0	/* 0.5 or 0.3 kHz filter */
		#define BOARD_FILTER_1P8		BOARD_FILTERCODE_0	/* 0.5 or 0.3 kHz filter */
		#define BOARD_FILTER_3P1		BOARD_FILTERCODE_1	/* 3.1 or 2.75 kHz filter */

		/* ару управляется FAST/SLOW */
		#define WITHAGCMODESLOWFAST 1	/* среди режимов АРУ есть только включено-выключено */
		#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_0
		#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_1
	#elif 1
		#define KEYB_VERTICAL_REV	1	/* расположение кнопок для плат "Воробей" и "Колибри" */
		// EW2DZ
		/* Версии частотных схем - с преобразованием "вниз" */
		/*
			ПЧ - 8.982 МГц; индикатор - RDX0154; память - 25L160AI; режимы - SSB и CW; 
			фильтры - 3.0 кГц и 1.8 кГц; генератор - 48 МГц; только две 595-ые и 
			клавиатура будет 4 ряда по 3 кнопки. 
			И сделайте такую же прошивку, только ПЧ будет 8982 кГц и фильтр не 1.8, а 0.5 кГц 
		*/
		#define FQMODEL_TRX8M		1	// Первая низкая ПЧ 8 МГц, 6 МГц, 5.5 МГц и другие
		#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
		#define LCDMODE_RDX0154	1				/* тип применяемого индикатора  */
		#define DIRECT_48M0_X1		1
		#define CTLREGMODE16	1		/* управляющий регистр 16 бит - для SIMPLE синтезатора на AD8932/AD8935/AD8933	*/
		#define WITHMODESETMIXONLY 1		/* EW2DZ version */
		/* набор фильтров в тракте */
		#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
		/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
		#define BOARD_FILTER_0P5		BOARD_FILTERCODE_0	/* 0.5 or 0.3 kHz filter */
		#define BOARD_FILTER_3P1		BOARD_FILTERCODE_1	/* 3.1 or 2.75 kHz filter */

		/* ару управляется FAST/SLOW */
		#define WITHAGCMODESLOWFAST 1	/* среди режимов АРУ есть только включено-выключено */
		#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_0
		#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_1

		#define IF3DF (8982000L - 8865850L)

		#define IF3_CUSTOM_CW_CENTER	(8866500L + IF3DF)
		#define IF3_CUSTOM_SSB_LOWER	(8865850L + IF3DF)
		#define IF3_CUSTOM_SSB_UPPER	(8868700L + IF3DF)
		#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

		#define IF3_MODEL	IF3_TYPE_CUSTOM

	#elif 1
		#define KEYB_V8S_DK1VS	1	/* расположение для контроллера DK1VS */
		// EW2DZ
		/* Версии частотных схем - с преобразованием "вниз" */
		/* ару управляется FAST/SLOW */
		#define WITHAGCMODESLOWFAST 1	/* среди режимов АРУ есть только включено-выключено */
		#define FQMODEL_TRX5M_UA1FA	1	// Первая низкая ПЧ, вторая ПЧ 500 кГц
		#define IF3_MODEL IF3_TYPE_500
		#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
		#define LCDMODE_WH1602	1	/* тип применяемого индикатора 16*2 */
		#define DIRECT_25M0_X1		1
		//#define	LSBONLO4DEFAULT	1		/* применён ЭМВ на нижнюю боковую */
		#define CTLREGMODE16	1		/* управляющий регистр 16 бит - для SIMPLE синтезатора на AD8932/AD8935/AD8933	*/
		#define BOARD_FILTER_0P5_LSB 1		/* Используется фильтр ЭМФ-500-0,5Н	*/
		#define BOARD_FILTER_3P1_LSB 1		/* Используется фильтр ЭМФ-500-3Н	*/
		#define WITHMODESETMIXONLY 1	/* EW2DZ version */
		#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
		/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
		#define BOARD_FILTER_0P5		BOARD_FILTERCODE_0	/* 0.5 or 0.3 kHz filter */
		#define BOARD_FILTER_3P1		BOARD_FILTERCODE_1	/* 3.1 or 2.75 kHz filter */
		#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_0
		#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_1
	#else
		#define KEYB_V8S_DK1VS	1	/* расположение для контроллера DK1VS */
		// EW2DZ
		/* Версии частотных схем - с преобразованием "вниз" */
		/* ару управляется ON/OFF */
		#define WITHAGCMODEONOFF 1	/* среди режимов АРУ есть только включено-выключено */
		#define FQMODEL_TRX5M_UA1FA	1	// Первая низкая ПЧ, вторая ПЧ 500 кГц
		#define IF3_MODEL IF3_TYPE_500
		#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
		#define LCDMODE_WH2002	1	/* тип применяемого индикатора 20*2 */
		#define DIRECT_25M0_X1		1
		//#define	LSBONLO4DEFAULT	0		/* применён ЭМВ на верхнюю боковую */
		#define CTLREGMODE16	1		/* управляющий регистр 16 бит - для SIMPLE синтезатора на AD8932/AD8935/AD8933	*/
		#define WITHMODESETMIXONLY 1	/* EW2DZ version */
		#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		#define IF3_FHAVE	(IF3_FMASK_3P1)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
		/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
		#define BOARD_FILTER_0P5		BOARD_FILTERCODE_0	/* 0.5 or 0.3 kHz filter */
		#define BOARD_FILTER_3P1		BOARD_FILTERCODE_1	/* 3.1 or 2.75 kHz filter */

		#define	BOARD_AGCCODE_ON	BOARD_AGCCODE_0
		#define	BOARD_AGCCODE_OFF	BOARD_AGCCODE_1
	#endif

	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	//#define DDS3_CLK_MUL	1 		/* Умножитель в DDS3 */

	#define	FONTSTYLE_ITALIC	1	//

	// --- вариации прошивки, специфические для разных плат

	//#define KEYB18_GENERIC	1		/* 18 кнопок - расширенныое поле */
	//#define KEYBOARD_USE_ADC6	1	/* шесть кнопок на каждом входе ADCx */
	// --- Одна из этих строк определяет тип дисплея, для которого компилируется прошивка

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_100
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define	WITHENCODER	1	/* для изменения частоты имеется енкодер */

	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	#define WITHTX		1	/* включено управление передатчиком - сиквенсор, электронный ключ. */
	//#define WITHAUTOTUNER	1	/* Есть функция автотюнера */
	#define WITHCAT		1	/* используется CAT */
	//#define WITHDEBUG		1	/* Отладочная печать через COM-порт. Без CAT (WITHCAT) */
	//#define WITHVOX		1	/* используется VOX */
	#define WITHSWRMTR	1	/* Измеритель КСВ или */
	//#define WITHPWRMTR	1	/* Индикатор выходной мощности  */
	#define WITHSHOWSWRPWR 1	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
	#define WITHBARS	1	/* отображение S-метра и SWR-метра */
	//#define WITHPWRLIN	1	/* Индикатор выходной мощности показывает напряжение а не мощность */
	//#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
	//#define WITHSWLMODE	1	/* поддержка запоминания множества частот в swl-mode */
	//#define WITHPBT		1	/* используется PBT */
	#define WITHIFSHIFT	1	/* используется IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* Начальное занчение IF SHIFT */
	//#define WITHFIXEDBFO	1	/* Переворот боковых за счёт 1-го гетродина (особенно, если нет подстраиваемого BFO) */
	//#define WITHDUALBFO		1	/* Переворот боковых за счёт переключения частоты BFO внешним сигналом */
	//#define WITHDUALFLTR	1	/* Переворот боковых за счёт переключения фильтра верхней или нижней боковой полосы */
	#define WITHSAMEBFO	1	/* использование общих настроек BFO для приёма и передачи */
	//#define WITHONLYBANDS 1		/* Перестройка может быть ограничена любительскими диапазонами */
	//#define WITHBCBANDS	1		/* в таблице диапазонов присутствуют вещательные диапазоны */
	#define WITHWARCBANDS	1	/* В таблице диапазонов присутствуют HF WARC диапазоны */
	//#define WITHDIRECTBANDS 1	/* Прямой переход к диапазонам по наэатиям на клавиатуре */

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */
	//#define WITHVIBROPLEX	1	/* возможность эмуляции передачи виброплексом */
	#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */
	#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	//#define LO1PHASES	1		/* Прямой синтез первого гетеродина двумя DDS с програмимруемым сдвигом фазы */
	#define DEFPREAMPSTATE 	1	/* УВЧ по умолчанию включён (1) или выключен (0) */
	
	/* что за память настроек и частот используется в контроллере */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// Так же при использовании FM25040A - 5 вольт, 512 байт
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section

	//#define FTW_RESOLUTION 28	/* разрядность FTW выбранного DDS */
	#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */
	//#define FTW_RESOLUTION 31	/* разрядность FTW выбранного DDS - ATINY2313 */

	//#define WITHLO1LEVELADJ		1	/* включено управление уровнем (амплитудой) LO1 */
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
	//#define PLL2_TYPE PLL_TYPE_ADF4001	/* Делитель октавного гетеродина для получения сквозного диапазона */
	//#define PLL2_TYPE PLL_TYPE_LMX2306	/* Делитель октавного гетеродина для получения сквозного диапазона */

	//#define DDS1_CLK_DIV	7		/* Делитель опорной частоты перед подачей в DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
	#define DDS2_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS2 */
	//#define DDS3_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS3 */

	/* Назначение адресов на SPI шине */
	//#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	//#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. Для двойной DDS первого гетеродина - вторая DDS */
	//#define targetpll2 SPI_CSEL3 	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetext1 SPI_CSEL4 	/* external devices control */ 
	//#define targetupd1 SPI_CSEL5 	/* DDS IOUPDATE signals at output of SPI chip-select register */
	#define targetctl1 SPI_CSEL6 	/* control register */
	#define targetnvram SPI_CSEL7  	/* serial nvram */

	#define targetlcd targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/


	/* все возможные в данной конфигурации фильтры */
	//#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_1P8 | IF3_FMASK_2P7 | IF3_FMASK_3P1)
	//#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_1P8 | IF3_FMASK_2P7 | IF3_FMASK_3P1)
	//#define IF3_FMASKTX	(IF3_FMASK_3P1 | IF3_FMASK_2P7)

#define WITHSPLIT	1	/* управление режимами расстройки одной кнопкой */
//#define WITHSPLITEX	1	/* Трехкнопочное управление режимами расстройки */
#define WITHCATEXT	1	/* Расширенный набор команд CAT */
#define WITHELKEY	1
#define WITHKBDENCODER 1	// перестройка частоты кнопками
#define WITHKEYBOARD 1	/* в данном устройстве есть клавиатура */
#define KEYBOARD_USE_ADC	1	/* на одной линии установлено  четыре  клавиши. на vref - 6.8K, далее 2.2К, 4.7К и 13K. */
	
// Назначения входов АЦП процессора.
enum 
{ 
#if WITHVOX
	//VOXIX = 2, AVOXIX = 1,	// VOX - Нельзя, выводы задействованы под входы ключа
#endif
	SMETERIX = 0,	// S-meter
	PWRI = 4, 
	FWD = 4, REF = 3,	// SWR-meter
	KI0 = 5, KI1 = 6, KI2 = 7	// клавиатура
};
#define KI_COUNT 3	// количество используемых под клавиатуру входов АЦП
#define KI_LIST	KI2, KI1, KI0,	// инициализаторы для функции перекодировки


#endif /* ATMEGA_CTL_EW2DZ_H_INCLUDED */
