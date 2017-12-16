/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Приёмник "Воронёнок". up-conversion, 2 * AD9834 + ADF4001, 

#ifndef ATMEGA644_CTLSTYLE_IGOR_H_INCLUDED
#define ATMEGA644_CTLSTYLE_IGOR_H_INCLUDED 1

#if F_CPU != 10000000
	#error Set F_CPU right value in project file
#endif

	/* модели синтезаторов - схемы частотообразования */

	#define FQMODEL_45_IF128	1	// 1-st IF=45MHz, 2-nd IF=128 kHz
	#define BANDSELSTYLERE_UPCONV36M	1	/* Up-conversion with working band 0.1 MHz..36 MHz */
	//#define BANDSELSTYLERE_UPCONV32M	1	/* Up-conversion with working band 0.1 MHz..32 MHz */
	
	// --- вариации прошивки, специфические для разных плат
	#define	MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */

	//#define CTLREGMODE16	1		/* управляющий регистр 16 бит	*/
	//#define CTLREGMODE24	1		/* управляющий регистр 24 бита	*/
	//#define CTLREGMODE24_V1	1		/* управляющий регистр 24 бита - "Воробей" и "Колибри" */
	#define CTLREGMODE24_IGOR	1	/* Вариант для Игоря - управляющий регистр 24 бита - "Воробей" и "Колибри" */

	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея 
	#define WITHLCDBACKLIGHTMIN	0	// Нижний предел регулировки (показываетый на дисплее)
	#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываетый на дисплее)
	#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры 

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 	(0x02)
	#define BOARD_DETECTOR_SSB 	(0x00)
	#define BOARD_DETECTOR_AM 	(0x01)
	#define BOARD_DETECTOR_FM 	(0x03)
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */
	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01
	#define	BOARD_FILTERCODE_2	0x02
	#define	BOARD_FILTERCODE_3	0x03

	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */

	//#define CTLREGMODE16_GEN500	1
	#define WITHPREAMPATT2_6DB		1	// Управление УВЧ и двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB */

	//#define DSTYLE_UR3LMZMOD	1	// Тестирование - расположение элементов экрана в трансиверах UR3LMZ
	#define	FONTSTYLE_ITALIC	1	// Использовать альтернативный шрифт

	// +++ Особые варианты расположения кнопок на клавиатуре
	#define KEYB_VERTICAL_REV_IGOR	1	/* расположение кнопок для плат "Воробей" и "Колибри" */
	// --- Особые варианты расположения кнопок на клавиатуре

	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	//#define LCDMODE_WH2002	1	/* тип применяемого индикатора 20*2, возможно вместе с LCDMODE_HARD_SPI */
	#define LCDMODE_WH2002_IGOR 1
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
	//#define LCDMODE_LS020 	1	/* Индикатор 176*132 Sharp LS020B8UD06 с контроллером LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - перевернуть изображение */
	//#define LCDMODE_LPH88		1	/* Индикатор 176*132 LPH8836-2 с контроллером Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - перевернуть изображение */
	//#define LCDMODE_L2F50	1	/* Индикатор 176*132 с контроллером Epson L2F50126 */
	//#define LCDMODE_L2F50_TOPDOWN	1	/* Переворот изображени я в случае LCDMODE_L2F50 */
	//#define LCDMODE_S1D13781	1	/* Инндикатор 480*272 с контроллером Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - перевернуть изображение */
	//#define LCDMODE_ILI9320	1	/* Индикатор 248*320 с контроллером ILI9320 */
	//#define LCDMODE_ILI9225	1	/* Индикатор 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 с контроллером ILI9225С */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - перевернуть изображение (для выводов слева от экрана) */
	//#define LCDMODE_UC1608	1		/* Индикатор 240*128 с контроллером UC1608.- монохромный */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ST7735	1	/* Индикатор 160*128 с контроллером Sitronix ST7735 */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - перевернуть изображение (для выводов справа) */
	//#define LCDMODE_ST7565S	1	/* Индикатор WO12864C2-TFH# 128*64 с контроллером Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ILI9163	1	/* Индикатор LPH9157-2 176*132 с контроллером ILITEK ILI9163 - Лента дисплея справа, а выводы слева. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - перевернуть изображение (для выводов справа, лента дисплея слева) */
	// --- Одна из этих строк определяет тип дисплея, для которого компилируется прошивка

	//#define ENCRES_DEFAULT ENCRES_128
	#define ENCRES_DEFAULT ENCRES_64	// японский валкодер
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define	WITHENCODER	1	/* для изменения частоты имеется енкодер */

	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	//#define WITHTX		1	/* включено управление передатчиком - сиквенсор, электронный ключ. */
	////#define WITHPBT		1	/* используется PBT */
	////#define WITHIFSHIFT	1	/* используется IF SHIFT */
	#define WITHCAT		1	/* используется CAT */
	//#define WITHVOX		1	/* используется VOX */
	//#define WITHSWRMTR	1	/* Измеритель КСВ */
	//#define WITHPWRMTR	1	/* Индикатор выходной мощности или */
	//#define WITHPWRLIN	1	/* Индикатор выходной мощности показывает напряжение а не мощность */
	#define WITHBARS	1	/* отображение S-метра и SWR-метра */
	//#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
	//#define WITHSWLMODE	1	/* поддержка запоминания множества частот в swl-mode */
	//#define WITHPOTWPM		1	/* используется регулировка скорости передачи в телеграфе потенциометром */
	//#define WITHVIBROPLEX	1	/* возможность эмуляции передачи виброплексом */

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */
	//#define WITHDEBUG		1	/* Отладочная печать через COM-порт. Без CAT (WITHCAT) */
	//#define WITHFIXEDBFO	1	/* Переворот боковых за счёт 1-го гетродина (особенно, если нет подстраиваемого BFO) */
	#define WITHDUALFLTR	1	/* Переворот боковых за счёт переключения фильтра верхней или нижней боковой полосы */
	#define WITHSAMEBFO	1	/* использование общих настроек BFO для приёма и передачи */
	//#define WITHONLYBANDS 1		/* Перестройка может быть ограничена любительскими диапазонами */
	#define WITHBCBANDS	1		/* в таблице диапазонов присутствуют вещательные диапазоны */
	#define WITHWARCBANDS	1	/* В таблице диапазонов присутствуют HF WARC диапазоны */
	//#define WITHLO1LEVELADJ		1	/* включено управление уровнем (амплитудой) LO1 */
	//#define WITHLFM		1	/* LFM MODE */
	#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	#define DEFPREAMPSTATE 	1	/* УВЧ по умолчанию включён (1) или выключен (0) */

	/* что за память настроек и частот используется в контроллере */
	#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// Так же при использовании FM25040A - 5 вольт, 512 байт
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A


	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */

	/* Board hardware configuration */
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9834
	
	//#define PLL1_TYPE PLL_TYPE_ADF4001

	//#define PLL1_TYPE PLL_TYPE_ADF4360
	//#define PLL2_TYPE PLL_TYPE_CMX992

	//#define DDS2_TYPE DDS_TYPE_AD9835
	//#define DDS2_TYPE DDS_TYPE_AD9951
	#define DDS2_TYPE DDS_TYPE_AD9834

	#define DDS3_TYPE DDS_TYPE_AD9951
	//#define DDS3_TYPE DDS_TYPE_AD9834

	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
	#define DDS2_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS2 */
	#define DDS3_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS3 */

	/* Назначение адресов на SPI шине */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	#define targetdds3 SPI_CSEL2 	/* DDS3 - PBT output */
	//#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. Для двойной DDS первого гетеродина - вторая DDS */
	//#define targetpll2 SPI_CSEL3 	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetext1 SPI_CSEL4 	/* external devices control */ 
	#define targetupd1 SPI_CSEL5 	/* DDS IOUPDATE signals at output of SPI chip-select register */
	#define targetctl1 SPI_CSEL6 	/* control register */
	#define targetnvram SPI_CSEL7  	/* serial nvram */

	#define targetlcd targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 	(0x02)
	#define BOARD_DETECTOR_SSB 	(0x00)
	#define BOARD_DETECTOR_AM 	(0x01)
	#define BOARD_DETECTOR_FM 	(0x03)
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define BOARD_FILTER_0P5		0x00	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_1P8		0x01	/* 1.8 kHz filter - на очень старых платах */
	#define BOARD_FILTER_2P7		0x02	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_3P1		0x03	/* 3.1 or 2.75 kHz filter */

	#define BOARD_AGCCODE_OFF 0
#if 1
	//#define LO1PHASES	1		/* Прямой синтез первого гетеродина двумя DDS с програмимруемым сдвигом фазы */
	//#define WITHMODESETFULLNFM 1	/* используется FM */
	//#define WITHWFM	1			/* используется WFM */
	#define WITHMODESETFULL 1	/*  */
	/* все возможные в данной конфигурации фильтры */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
	/* все возможные в данной конфигурации фильтры для передачи */
	#define IF3_FMASKTX	(IF3_FMASK_3P1)
	/* фильтры, для которых стоит признак HAVE */
	#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
#else
	//#define LO1PHASES	1		/* Прямой синтез первого гетеродина двумя DDS с програмимруемым сдвигом фазы */
	//#define WITHMODESETFULLNFM 1	/* используется FM */
	//#define WITHWFM	1			/* используется WFM */
	#define WITHMODESETFULL 1	/*  */
	#define IF3_FMASK (IF3_FMASK_2P7 | IF3_FMASK_1P8 | IF3_FMASK_0P5)
	#define IF3_FMASKTX (IF3_FMASK_2P7)
	#define IF3_FHAVE (IF3_FMASK_2P7 | IF3_FMASK_0P5)
#endif

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
	#if WITHVOLTLEVEL 
		VOLTSOURCE = 4,		// Средняя точка делителя напряжения, для АКБ
	#endif /* WITHVOLTLEVEL */

	#if WITHBARS
		SMETERIX = 0,	// S-meter
	#endif
	#if WITHVOX
		VOXIX = 2, AVOXIX = 1,	// VOX
	#endif
	#if WITHSWRMTR
		FWD = 4, REF = 3,	// SWR-meter
	#endif
		KI0 = 5, KI1 = 6, KI2 = 7	// клавиатура
	};

	#define KI_COUNT 3	// количество используемых под клавиатуру входов АЦП
	#define KI_LIST	KI2, KI1, KI0,	// инициализаторы для функции перекодировки


#endif /* ATMEGA644_CTLSTYLE_IGOR_H_INCLUDED */
