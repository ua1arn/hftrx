/* $Id$ */
/* SW2016MINI - с вольтметром и КСВ-метром	*/
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

// Описание для трансивера SW2013C - mini "Color" - с цветным дисплеем и управлением выходной мощностью.

#ifndef ATMEGA_CTLSTYLE_V9CN6_UY5UM_H_INCLUDED
#define ATMEGA_CTLSTYLE_V9CN6_UY5UM_H_INCLUDED 1
	
	#define CTLSTYLE_SW2011ALL	1

	#if F_CPU != 8000000
		#error Set F_CPU right value in project file
	#endif
	
	#if 0
		#define WITHAUTOTUNER	1	/* Есть функция автотюнера */
		#define SHORTSET7 1
	#endif
	
	/* модели синтезаторов - схемы частотообразования */
	#if 1
		#define FQMODEL_45_IF6000	1	// up to 32 MHz
		//#define FQMODEL_45_IF8868	1	// up to 32 MHz
	#else
		// Для случаев использования платы синтезатора в трансиверах с одним преобразованием.
		#define FQMODEL_TRX8M		1	// Первая низкая ПЧ 8 МГц, 6 МГц, 5.5 МГц и другие
		#define DIRECT_27M0_X1		1	/* Board hardware configuration */
		//#define DIRECT_80M0_X5		1	/* Board hardware configuration */
		//#define DIRECT_60M0_X6		1	/* Board hardware configuration */
		//#define DIRECT_125M0_X3		1	/* Board hardware configuration */

		//#define BANDSELSTYLERE_LOCONV32M	1
		#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
		//#define BANDSELSTYLE_LADVABEST	1		/* Down-conversion with working band 1.6..56 MHz */
		// Выбор ПЧ
		//#define IF3_MODEL IF3_TYPE_9000
		//#define IF3_MODEL IF3_TYPE_8000
		//#define IF3_MODEL IF3_TYPE_6000
		#define IF3_MODEL IF3_TYPE_8868
	#endif
	#define	MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */
	
	#define CTLSTYLE_SW2012_MINI 1
	#define CTLREGSTYLE_SW2012CN	1	// управляющие регистры SW2012MINI c цветным дисплеем и регулировкой выходной мощности
	//#define CTLREGSTYLE_SW2012CN_RN3ZOB 1 // с платой автотюнера

	#define DSTYLE_UR3LMZMOD	1
	//#define	FONTSTYLE_ITALIC	1	//
	// --- вариации прошивки, специфические для разных плат

	// +++ вариации прошивки, специфические для плат на ATMega
	// --- вариации прошивки, специфические для плат на ATMega

	// +++ Особые варианты расположения кнопок на клавиатуре
	#define KEYBSTYLE_SW2012CN5_UY5UM	1
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	// --- Особые варианты расположения кнопок на клавиатуре

	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	//#define LCDMODE_WH2002	1	/* тип применяемого индикатора 20*2, возможно вместе с LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* тип применяемого индикатора 16*2 */
	//#define LCDMODE_WH2004	1	/* тип применяемого индикатора 20*4 */
	//#define LCDMODE_RDX0077	1	/* Индикатор 128*64 с контроллером UC1601.  */
	//#define LCDMODE_RDX0154	1	/* Индикатор 132*64 с контроллером UC1601.  */
	//#define LCDMODE_UC1601S_TOPDOWN	1	/* LCDMODE_RDX0154 - перевернуть изображение */
	//#define LCDMODE_UC1601S_EXTPOWER	1	/* LCDMODE_RDX0154 - без внутреннего преобразователя */
	//#define LCDMODE_RDX0120	1	/* Индикатор 64*32 с контроллером UC1601.  */
	//#define LCDMODE_TIC154	1	/* Индикатор 133*65 с контроллером PCF8535 */
	//#define LCDMODE_TIC218	1	/* Индикатор 133*65 с контроллером PCF8535 */
	//#define LCDMODE_PCF8535_TOPDOWN	1	/* Контроллер PCF8535 - перевернуть изображение */
	//#define LCDMODE_LS020 	1	/* Индикатор 176*132 Sharp LS020B8UD06 с контроллером LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - перевернуть изображение */
	//#define LCDMODE_LPH88		1	/* Индикатор 176*132 LPH8836-2 с контроллером Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - перевернуть изображение */
	#define LCDMODE_S1D13781	1	/* Инндикатор 480*272 с контроллером Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - перевернуть изображение */
	//#define LCDMODE_ILI9225	1	/* Индикатор 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 с контроллером ILI9225С */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - перевернуть изображение (для выводов слева от экрана) */
	//#define LCDMODE_UC1608	1		/* Индикатор 240*128 с контроллером UC1608.- монохромный */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ST7735	1	/* Индикатор 160*128 с контроллером Sitronix ST7735 */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - перевернуть изображение (для выводов справа) */
	//#define LCDMODE_ILI9341	1	/* 320*240 SF-TC240T-9370-T с контроллером ILI9341 - STM32F4DISCO */
	//#define LCDMODE_ILI9341_TOPDOWN	1	/* LCDMODE_ILI9341 - перевернуть изображение (для выводов справа) */
	//#define LCDMODE_ST7565S	1	/* Индикатор WO12864C2-TFH# 128*64 с контроллером Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ILI9163	1	/* Индикатор LPH9157-2 176*132 с контроллером ILITEK ILI9163 - Лента дисплея справа, а выводы слева. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - перевернуть изображение (для выводов справа, лента дисплея слева) */
	// --- Одна из этих строк определяет тип дисплея, для которого компилируется прошивка

	//#define ENCRES_DEFAULT ENCRES_128
	#define ENCRES_DEFAULT ENCRES_24
	#define WITHENCODER	1	/* для изменения частоты имеется енкодер */

	#define ENCODER_REVERSE	1	/* шагать в обртную сторону */

	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	#define WITHTX		1	/* включено управление передатчиком - сиквенсор, электронный ключ. */
	//#define WITHPBT		1	/* используется PBT */
	#define WITHIFSHIFT	1	/* используется IF SHIFT */
	#define WITHIFSHIFTOFFSET	(-250)	/* Начальное занчение IF SHIFT */
	#define WITHCAT		1	/* используется CAT */
	//#define WITHVOX		1	/* используется VOX */

	#define WITHSWRMTR	1	/* Измеритель КСВ */
	#define WITHPWRLIN	1	/* Индикатор выходной мощности показывает напряжение а не мощность */
	#define WITHSHOWSWRPWR 1	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */

	#define WITHBARS	1	/* отображение S-метра и SWR-метра */
	#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
	//#define WITHSWLMODE	1	/* поддержка запоминания множества частот в swl-mode */
	//#define WITHPOTWPM		1	/* используется регулировка скорости передачи в телеграфе потенциометром */
	//#define WITHVIBROPLEX	1	/* возможность эмуляции передачи виброплексом */

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */
	//#define WITHDEBUG		1	/* Отладочная печать через COM-порт. Без CAT (WITHCAT) */
	#define WITHSAMEBFO	1	/* использование общих настроек BFO для приёма и передачи */
	//#define WITHBCBANDS	1		/* в таблице диапазонов присутствуют вещательные диапазоны */
	#define WITHWARCBANDS	1	/* В таблице диапазонов присутствуют HF WARC диапазоны */
	//#define WITHONLYBANDS 1		/* Перестройка может быть ограничена любительскими диапазонами */

	//#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	//#define WITHWFM	1			/* используется WFM */
	//#define LO1PHASES	1		/* Прямой синтез первого гетеродина двумя DDS с програмимруемым сдвигом фазы */
	#define DEFPREAMPSTATE 	0	/* УВЧ по умолчанию включён (1) или выключен (0) */

	// Пользовательские варианты конфигурации.
	#define BANDSELSTYLERE_SW20XX 1
	//#define BANDSELSTYLERE_RX3QSP 1	/* Специальный вариант диапазонных фильтров по запросу RX3QSP */
	//#define BANDSELSTYLERE_R3PAV 1        /* 8 диапазонных ФНЧ где 12м и 10м совмещенны для дешифратора 74HCT238 ftp://shodtech.net/SW2012/%D0%A4%D0%9D%D0%A7/ */

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
	#define RTC1_TYPE RTC_TYPE_M41T81	/* ST M41T81M6/M41T81SM6F RTC clock chip with I2C interface */
	//#define RTC1_TYPE RTC_TYPE_DS1307	/* MAXIM DS1307/DS3231 RTC clock chip with I2C interface */

	/* Аппаратура контроллера SW2011 */
	//#define WITHLO1LEVELADJ		1	/* включено управление уровнем (амплитудой) LO1 */

	#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */

	// Тип синтезатора
	#if 1
		// Поздняя версия - всё на Si5351A
		#define WITHSI5351AREPLACE 1
	#else
		// Ранняя версия
		#define DDS1_TYPE DDS_TYPE_AD9951
		//#define PLL1_TYPE PLL_TYPE_SI570
		//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
		//#define DDS2_TYPE DDS_TYPE_AD9835
		#define DDS2_TYPE DDS_TYPE_AD9834
	#endif

	/* Board hardware configuration */

	//#define DDS1_CLK_DIV	7		/* Делитель опорной частоты перед подачей в DDS1 ATTINY2313 */
	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
	#define DDS2_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS2 */

	/* Назначение адресов на SPI шине */
	#define targetdds1 SPI_CSEL0	/* DDS1 - LO1 generator */
	#define targetdds2 SPI_CSEL1	/* DDS2 - LO3 generator */
	//#define targetpll1 SPI_CSEL2	/* ADF4001 after DDS1 - divide by r1 and scale to n1. Для двойной DDS первого гетеродина - вторая DDS */
	#define targetlcd SPI_CSEL2 	/* LCD over SPI line devices control */
	#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/
	//#define targetpll2 SPI_CSEL3	/* ADF4001 - fixed 2-nd LO generate or LO1 divider */ 
	#define targetctl1 SPI_CSEL4	/* control register */
	#define targetnvram SPI_CSEL5 	/* serial nvram */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define BOARD_FILTER_0P5		BOARD_FILTERCODE_1	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_1P8		BOARD_FILTERCODE_1	/* 1.8 kHz filter - установлен вместо фильтра 0.5 */
	#define BOARD_FILTER_2P7		BOARD_FILTERCODE_0	/* 3.1 or 2.75 kHz filter */
	//#define BOARD_FILTER_WFM		2	/* 3.1 or 2.75 kHz filter */

	#define WITHMODESETMIXONLY3 1
	//#define WITHMODESETMIXONLY3AM 1

	#define IF3_FMASK (IF3_FMASK_2P7 | /*IF3_FMASK_1P8 | */IF3_FMASK_0P5)
	#define IF3_FMASKTX (IF3_FMASK_2P7)
	#define IF3_FHAVE (IF3_FMASK_2P7 | IF3_FMASK_0P5)

#define WITHSPLIT	1	/* управление режимами расстройки одной кнопкой */
//#define WITHSPLITEX	1	/* Трехкнопочное управление режимами расстройки */
#define WITHCATEXT	1	/* Расширенный набор команд CAT */
#define WITHELKEY	1
//#define WITHKBDENCODER 1	// перестройка частоты кнопками
#define WITHKEYBOARD 1	/* в данном устройстве есть клавиатура */
#define KEYBOARD_USE_ADC	1	/* на одной линии установлено  четыре  клавиши. на vref - 6.8K, далее 2.2К, 4.7К и 13K. */

// Назначения входов АЦП процессора.
enum 
{ 
	VOLTSOURCE = 2, // Средняя точка делителя напряжения, для АКБ
#if WITHBARS
	SMETERIX = 1,	// S-meter
	PWRI = 0,		// PIN 37 Индикатор мощности передатчика
	FWD = 4, REF = 3,	// SWR-meter
#endif /* WITHBARS */

	XTHERMOMRRIX = BOARD_ADCMRRIN(0),	// кеш - индекc не должен повторяться в конфигурации
	PASENSEMRRIX = BOARD_ADCMRRIN(1),	// кеш - индекc не должен повторяться в конфигурации
	REFMRRIX = BOARD_ADCMRRIN(2),
	FWDMRRIX = BOARD_ADCMRRIN(3),
	PWRMRRIX = FWDMRRIX,
	VOLTMRRIX = BOARD_ADCMRRIN(4),	// кеш - индекc не должен повторяться в конфигурации

	KI0 = 5, KI1 = 6, KI2 = 7	// клавиатура
};

#define KI_COUNT 3	// количество используемых под клавиатуру входов АЦП
#define KI_LIST	KI1, KI2, KI0,	// инициализаторы для функции перекодировки

#define VOLTLEVEL_UPPER		43	// 4.3 kOhm - верхний резистор делителя датчика напряжения
#define VOLTLEVEL_LOWER		10	// 1.0 kOhm - нижний резистор

#define WITHMAXPWRCALI		100	/* калибровочный параметр PWR-метра */

#endif /* ATMEGA_CTLSTYLE_V9CN6_UY5UM_H_INCLUDED */
