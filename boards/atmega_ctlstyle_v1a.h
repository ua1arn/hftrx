/* $Id$ */
// ATMega: DU VENT SI570+AD9834 (66MHz), LS020, ATMega32 - без внешнего дешифратора адресов - atmega_cpustyle_v1a.h, atmega_ctlstyle_v1a.h - похожа на sw2011
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef ATMEGA_CTLSTYLE_V1A_H_INCLUDED
#define ATMEGA_CTLSTYLE_V1A_H_INCLUDED 1

	#define FQMODEL_TRX8M	1	// Вторая ПЧ 8 МГц
	//#define WITHCLASSICSIDE	1	/* сторона первого гетеродина зависит от рабочей частоты */
	//#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
	// Выбор ПЧ
	//#define IF3_MODEL IF3_TYPE_9000
	#define IF3_MODEL IF3_TYPE_8868
	//#define IF3_MODEL IF3_TYPE_5645	// Drake R-4C and Drake T-4XC (Drake Twins) - 5645 kHz
	//#define IF3_MODEL IF3_TYPE_5500
	//#define IF3_MODEL IF3_TYPE_9045
	#define DIRECT_66M0_X1		1	/* Board hardware configuration */
	
	#define DSTYLE_UR3LMZMOD	1	// Тестирование - расположение элементов экрана в трансиверах UR3LMZ
	//#define	FONTSTYLE_ITALIC	1	//
	// --- вариации прошивки, специфические для разных плат
	#define	MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */

	#define CTLREGMODE16	1		/* управляющий регистр 16 бит - для SIMPLE синтезатора на AD8935	*/
	//#define CTLREGSTYLE_SW2011	1	// управляющие регистры SW2011RDX
	//#define KEYB_VERTICAL_REV	1	/* расположение кнопок для плат "Воробей" и "Колибри" */
	#define KEYB_V8S_DK1VS	1	/* расположение для контроллера DK1VS */
	#define WITHAGCMODEONOFF 1	/* среди режимов АРУ есть только включено-выключено */
	#define	BOARD_AGCCODE_ON	BOARD_AGCCODE_0
	#define	BOARD_AGCCODE_OFF	BOARD_AGCCODE_1


	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	//#define LCDMODE_WH2002	1	/* тип применяемого индикатора 20*2, возможно вместе с LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* тип применяемого индикатора 16*2 */
	//#define LCDMODE_WH2004	1	/* тип применяемого индикатора 20*4 */
	//#define LCDMODE_RDX0077	1	/* Индикатор 128*64 с контроллером UC1601.  */
	//#define LCDMODE_RDX0154	1	/* Индикатор 132*64 с контроллером UC1601. */
	//#define LCDMODE_RDX0120	1	/* Индикатор 64*32 с контроллером UC1601.  */
	//#define LCDMODE_UC1601S_TOPDOWN	1	/* LCDMODE_RDX0154 - перевернуть изображение */
	//#define LCDMODE_UC1601S_EXTPOWER	1	/* LCDMODE_RDX0154 - без внутреннего преобразователя */
	//#define LCDMODE_TIC154	1	/* Индикатор 133*65 с контроллером PCF8535 */
	//#define LCDMODE_TIC218	1	/* Индикатор 133*65 с контроллером PCF8535 */
	//#define LCDMODE_PCF8535_TOPDOWN	1	/* Контроллер PCF8535 - перевернуть изображение */
	#define LCDMODE_LS020 	1	/* Индикатор 176*132 Sharp LS020B8UD06 с контроллером LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - перевернуть изображение */
	//#define LCDMODE_LPH88		1	/* Индикатор 176*132 LPH8836-2 с контроллером Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - перевернуть изображение */
	//#define LCDMODE_S1D13781	1	/* Инндикатор 480*272 с контроллером Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - перевернуть изображение */
	//#define LCDMODE_ILI9320	1	/* Индикатор 248*320 с контроллером ILI9320 */
	//#define LCDMODE_ILI9225	1	/* Индикатор 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 с контроллером ILI9225С */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - перевернуть изображение (для выводов слева от экрана) */
	//#define LCDMODE_ILI9163	1	/* Индикатор LPH9157-2 176*132 с контроллером ILITEK ILI9163 - Лента дисплея справа, а выводы слева. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - перевернуть изображение (для выводов справа, лента дисплея слева) */
	// --- Одна из этих строк определяет тип дисплея, для которого компилируется прошивка

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define WITHENCODER	1	/* для изменения частоты имеется енкодер */

	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	#define WITHTX		1	/* включено управление передатчиком - сиквенсор, электронный ключ. */
	//#define WITHPBT		1	/* используется PBT */
	#define WITHIFSHIFT	1	/* используется IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* Начальное занчение IF SHIFT */
	//#define WITHCAT		1	/* используется CAT */
	//#define WITHDEBUG		1	/* Отладочная печать через COM-порт. Без CAT (WITHCAT) */
	#define WITHVOX		1	/* используется VOX */
	#define WITHSHOWSWRPWR 1	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
	#define WITHSWRMTR	1	/* Измеритель КСВ */
	//#define WITHPWRMTR	1	/* Индикатор выходной мощности или */
	#define WITHBARS	1	/* отображение S-метра и SWR-метра */
	//#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
	//#define WITHSWLMODE	1	/* поддержка запоминания множества частот в swl-mode */
	//#define WITHPOTWPM		1	/* используется регулировка скорости передачи в телеграфе потенциометром */
	#define WITHVIBROPLEX	1	/* возможность эмуляции передачи виброплексом */
	//#define WITHDIRECTBANDS 1	/* Прямой переход к диапазонам по наэатиям на клавиатуре */

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	//#define LO1PHASES	1		/* Прямой синтез первого гетеродина двумя DDS с програмимруемым сдвигом фазы */
	#define DEFPREAMPSTATE 	1	/* УВЧ по умолчанию включён (1) или выключен (0) */

	/* что за память настроек и частот используется в контроллере */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// Так же при использовании FM25040A - 5 вольт, 512 байт
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */

	/* Board hardware configuration */
	#define PLL1_TYPE PLL_TYPE_SI570
	#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9834
	
	//#define PLL1_TYPE PLL_TYPE_ADF4001

	//#define PLL1_TYPE PLL_TYPE_ADF4360
	//#define PLL2_TYPE PLL_TYPE_CMX992

	#define DDS2_TYPE DDS_TYPE_AD9834

	//#define WITHFIXEDBFO	1	/* Переворот боковых за счёт 1-го гетродина (особенно, если нет подстраиваемого BFO) */
	//#define WITHDUALFLTR	1	/* Переворот боковых за счёт переключения фильтра верхней или нижней боковой полосы */
	//#define WITHSAMEBFO	1		/* использование общих настроек BFO для приёма и передачи */
	//#define WITHONLYBANDS 1		/* Перестройка может быть ограничена любительскими диапазонами */
	#define WITHBCBANDS	1		/* в таблице диапазонов присутствуют вещательные диапазоны */
	#define WITHWARCBANDS	1	/* В таблице диапазонов присутствуют HF WARC диапазоны */
	#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */

	//#define DDS3_TYPE DDS_TYPE_AD9834

	//#define DDS1_CLK_DIV	7		/* Делитель опорной частоты перед подачей в DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
	#define DDS2_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS2 */

	/* Назначение адресов на SPI шине */
	#define targetdds1 SPI_CSEL0	/* DDS1 - LO1 generator */
	#define targetdds2 SPI_CSEL1	/* DDS2 - LO3 generator */
	//#define targetpll1 SPI_CSEL2	/* ADF4001 after DDS1 - divide by r1 and scale to n1. Для двойной DDS первого гетеродина - вторая DDS */
	#define targetlcd SPI_CSEL2	/* Чипселект индикатора LS020 */
	//#define targetpll2 SPI_CSEL3	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetctl1 SPI_CSEL4	/* control register */
	#define targetnvram SPI_CSEL5 	/* serial nvram */

	#define WITHMODESETFULL 1
	//#define WITHMODESETSMART 1	/* в заваисмости от частоты меняется боковая, используется вместе с WITHFIXEDBFO */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define BOARD_FILTER_0P5		0x00	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_2P7		0x01	/* 3.1 or 2.75 kHz filter */

	/* все возможные в данной конфигурации фильтры */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_2P7)
	#define IF3_FMASKTX	(IF3_FMASK_2P7)
	#define IF3_FHAVE 	(IF3_FMASK_2P7 | IF3_FMASK_0P5)

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
		KI0 = 5, KI1 = 6, KI2 = 7	// клавиатура
	};

	#define KI_COUNT 3	// количество используемых под клавиатуру входов АЦП

#endif /* ATMEGA_CTLSTYLE_V1A_H_INCLUDED */
