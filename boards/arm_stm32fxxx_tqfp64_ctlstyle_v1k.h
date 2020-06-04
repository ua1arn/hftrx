/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Синтезатор с процессором STM32F401RB
// новый SW20xx
//

#ifndef ARM_STM32FXXX_TQFP64_CTLSTYLE_V1K_H_INCLUDED
#define ARM_STM32FXXX_TQFP64_CTLSTYLE_V1K_H_INCLUDED 1

	#define CTLSTYLE_SW2011ALL	1

	#if 0
		#define WITHAUTOTUNER	1	/* Есть функция автотюнера */
		#define SHORTSET7 1
		//#define FULLSET7 1
	#endif

	#define WITHUSEPLL		1	/* Главная PLL	*/
	//#define WITHUSESAIPLL	1	/* SAI PLL	*/
	//#define WITHUSESAII2S	1	/* I2S PLL	*/

	#if 1
		// при наличии внешнего кварцевого резонатора
		#define WITHCPUXTAL 8000000UL	/* На процессоре установлен кварц 8.000 МГц */
		#define REF1_DIV 4			// ref freq = 2.000 MHz

		#if 1
			// normal operation frequency
			#define REF1_MUL 168	// N умножитель в PLL1
			//#define PLL1_P 4	// делитель перед SYSTEM CLOCK MUX в PLL1
			//#define PLL1_Q 7	// делитель на выход 48 МГц
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_2WS
		#endif

	#else
		// тактирование от внутреннего RC генератора 16 МГц
		#define REF1_DIV 8			// ref freq = 2.000 MHz

		#if 1
			// normal operation frequency
			#define REF1_MUL 84	// N умножитель в PLL1
			//#define PLL1_P 4	// делитель перед SYSTEM CLOCK MUX в PLL1
			//#define PLL1_Q 7	// делитель на выход 48 МГц
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_2WS
		#endif

	#endif

	/* модели синтезаторов - схемы частотообразования */
	#if 1

		#define FQMODEL_45_IF6000_UHF144	1
		//#define FQMODEL_45_IF8868_UHF144	1
		// Пользовательские варианты конфигурации.
		#define BANDSELSTYLERE_SW20XX_UHF 1
		//#define BANDSELSTYLERE_RX3QSP 1	/* Специальный вариант диапазонных фильтров по запросу RX3QSP */
		//#define BANDSELSTYLERE_R3PAV 1        /* 8 диапазонных ФНЧ где 12м и 10м совмещенны для дешифратора 74HCT238 ftp://shodtech.net/SW2012/%D0%A4%D0%9D%D0%A7/ */

	#else
		// Для случаев использования платы синтезатора в трансиверах с одним преобразованием.
		#define FQMODEL_TRX8M		1	// Первая низкая ПЧ 8 МГц, 6 МГц, 5.5 МГц и другие
		#define DIRECT_27M0_X1		1	/* Board hardware configuration */
		//#define DIRECT_25M0_X1		1	/* Board hardware configuration */
		//#define DIRECT_50M0_X8		1	/* Board hardware configuration */
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
	#define CTLREGSTYLE_SW2016VHF	1

	#define DSTYLE_UR3LMZMOD	1
	//#define	FONTSTYLE_ITALIC	1	//
	// --- вариации прошивки, специфические для разных плат

	// +++ вариации прошивки, специфические для плат на ATMega
	// --- вариации прошивки, специфические для плат на ATMega

	// +++ Особые варианты расположения кнопок на клавиатуре
	//#define KEYBSTYLE_SW2013RDX	1	// 8-ми кнопочная клавиатура
	#define KEYBSTYLE_SW2016RDX	1	// 8-ми кнопочная клавиатура
	//#define KEYBSTYLE_SW2013RDX_LTIYUR	1		// 12-ти кнопочная клавиатура LTIYUR (check KI_COUNT)
	//#define KEYBSTYLE_SW2013RDX_UT7LC	1	// 12-ти кнопочная клавиатура UT7LC (check KI_COUNT)
	// --- Особые варианты расположения кнопок на клавиатуре

	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
	//#define LCDMODE_HARD_SPI	1	/* LCD over SPI line */
	//#define LCDMODE_WH2002	1	/* тип применяемого индикатора 20*2, возможно вместе с LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* тип применяемого индикатора 16*2 */
	//#define LCDMODE_WH1604	1	/* тип применяемого индикатора 16*4 */
	//#define LCDMODE_WH2004	1	/* тип применяемого индикатора 20*4 */
	//#define LCDMODE_RDX0077	1	/* Индикатор 128*64 с контроллером UC1601.  */
	//#define LCDMODE_RDX0154	1	/* Индикатор 132*64 с контроллером UC1601. */
	//#define LCDMODE_RDX0120	1	/* Индикатор 64*32 с контроллером UC1601.  */
	#define LCDMODE_RDT065	1	/* Индикатор 132*64 с контроллером UC1601, XMIRROR & TOPDOWN */
	//#define LCDMODE_UC1601S_XMIRROR	1	/* Индикатор 132*64 с контроллером UC1601.  */
	//#define LCDMODE_UC1601S_TOPDOWN	1	/* LCDMODE_RDX0154 - перевернуть изображение */
	//#define LCDMODE_UC1601S_EXTPOWER	1	/* LCDMODE_RDX0154 - без внутреннего преобразователя */
	//#define LCDMODE_TIC154	1	/* Индикатор 133*65 с контроллером PCF8535 */
	//#define LCDMODE_TIC218	1	/* Индикатор 133*65 с контроллером PCF8535 */
	//#define LCDMODE_PCF8535_TOPDOWN	1	/* Контроллер PCF8535 - перевернуть изображение */
	//#define LCDMODE_LS020 	1	/* Индикатор 176*132 Sharp LS020B8UD06 с контроллером LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - перевернуть изображение */
	//#define LCDMODE_LPH88		1	/* Индикатор 176*132 LPH8836-2 с контроллером Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - перевернуть изображение */
	//#define LCDMODE_ILI9163	1	/* Индикатор LPH9157-2 176*132 с контроллером ILITEK ILI9163 - Лента дисплея справа, а выводы слева. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - перевернуть изображение (для выводов справа, лента дисплея слева) */
	//#define LCDMODE_L2F50	1	/* Индикатор 176*132 с контроллером Epson L2F50126 */
	//#define LCDMODE_L2F50_TOPDOWN	1	/* Переворот изображения в случае LCDMODE_L2F50 */
	//#define LCDMODE_S1D13781_NHWACCEL 1	/* Неиспользоване аппаратных особенностей EPSON S1D13781 при выводе графики */
	//#define LCDMODE_S1D13781	1	/* Инндикатор 480*272 с контроллером Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - перевернуть изображение */
	//#define LCDMODE_ILI9225	1	/* Индикатор 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 с контроллером ILI9225С */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - перевернуть изображение (для выводов слева от экрана) */
	//#define LCDMODE_UC1608	1		/* Индикатор 240*128 с контроллером UC1608.- монохромный */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ST7735	1	/* Индикатор 160*128 с контроллером Sitronix ST7735 - TFT панель 160 * 128 HY-1.8-SPI */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - перевернуть изображение (для выводов справа) */
	//#define LCDMODE_ST7565S	1	/* Индикатор WO12864C2-TFH# 128*64 с контроллером Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ILI9320	1	/* Индикатор 248*320 с контроллером ILI9320 */
	//#define LCDMODE_ILI9341	1	/* 320*240 SF-TC240T-9370-T с контроллером ILI9341 - STM32F4DISCO */
	//#define LCDMODE_ILI9341_TOPDOWN	1	/* LCDMODE_ILI9341 - перевернуть изображение (для выводов справа) */
	//#define LCDMODE_ILI8961	1	/* 320 * 240, RGB, ILI8961 */
	//#define LCDMODE_ILI8961_TOPDOWN	1
	//#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	// --- Одна из этих строк определяет тип дисплея, для которого компилируется прошивка

	//#define ENCRES_DEFAULT ENCRES_128
	#define ENCRES_DEFAULT ENCRES_64
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHENCODER	1	/* для изменения частоты имеется енкодер */

	#define ENCODER_REVERSE	1	/* шагать в обртную сторону */

	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	#define WITHCAT		1	/* используется CAT */
	//#define WITHDEBUG		1	/* Отладочная печать через COM-порт. Без CAT (WITHCAT) */

	#define WITHTX		1	/* включено управление передатчиком - сиквенсор, электронный ключ. */
	//#define WITHPBT		1	/* используется PBT */
	#define WITHIFSHIFT	1	/* используется IF SHIFT */
	#define WITHIFSHIFTOFFSET	(-250)	/* Начальное занчение IF SHIFT */
	#define WITHSWRMTR	1	/* Измеритель КСВ */
	//#define WITHPWRMTR	1	/* Индикатор выходной мощности или */
	#define WITHPWRLIN	1	/* Индикатор выходной мощности показывает напряжение а не мощность */
	#define WITHBARS	1	/* отображение S-метра и SWR-метра */
	#define WITHSHOWSWRPWR 1	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
	#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
	//#define WITHSWLMODE	1	/* поддержка запоминания множества частот в swl-mode */
	//#define WITHVIBROPLEX	1	/* возможность эмуляции передачи виброплексом */
	#define WITHSUBTONES	1	/* выполняется формирование субтона при передаче NFM */
	//#define WITHPOTWPM		1	/* for UY5UM используется регулировка скорости передачи в телеграфе потенциометром */
	//#define WITHANTSELECT	1	// for UY5UM Управление переключением антенн

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */
	#define WITHSAMEBFO	1	/* использование общих настроек BFO для приёма и передачи */
	#define WITHWARCBANDS	1	/* В таблице диапазонов присутствуют HF WARC диапазоны */
	//#define WITHONLYBANDS 1		/* Перестройка может быть ограничена любительскими диапазонами */
	#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	//#define WITHWFM	1			/* используется WFM */
	//#define LO1PHASES	1		/* Прямой синтез первого гетеродина двумя DDS с програмимруемым сдвигом фазы */
	#define DEFPREAMPSTATE 	0	/* УВЧ по умолчанию включён (1) или выключен (0) */

	//#define HARDWARE_IGNORENONVRAM	1		// отладка на платах где нет никакого NVRAM
	/* что за память настроек и частот используется в контроллере */
	#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// Так же при использовании FM25040A - 5 вольт, 512 байт
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM
	//#define NVRAM_TYPE NVRAM_TYPE_BKPSRAM	// Область памяти с батарейным питанием
	//#define HARDWARE_IGNORENONVRAM	1		// отладка на платах где нет никакого NVRAM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16		// demo board with atxmega128a4u - 25LC160A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */

	/* Board hardware configuration */
	//#define PLL1_TYPE PLL_TYPE_SI5351A
	//#define PLL1_FRACTIONAL_LENGTH	10	/* Si5351a: lower 10 bits is a fractional part */
	//#define PLL2_TYPE PLL_TYPE_SI5351A
	//#define PLL2_FRACTIONAL_LENGTH	10	/* Si5351a: lower 10 bits is a fractional part */
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9852
	//#define DDS1_TYPE DDS_TYPE_AD9851
	//#define DDS2_TYPE DDS_TYPE_AD9834
	//#define DDS2_TYPE DDS_TYPE_AD9851
	//#define PLL2_TYPE PLL_TYPE_CMX992
	//#define DDS3_TYPE DDS_TYPE_AD9834
	//#define RTC1_TYPE RTC_TYPE_STM32F4xx	/* STM32F4xx/STM32F7xx internal RTC peripherial */
	#define WITHSI5351AREPLACE 1

	//#define DDS1_TYPE DDS_TYPE_ATTINY2313
	//#define PLL1_TYPE PLL_TYPE_LM7001
	//#define targetpll1 SPI_CSEL1

	//#define WITHSI5351AREPLACE 1

	//#define RTC1_TYPE RTC_TYPE_M41T81	/* ST M41T81M6 RTC clock chip with I2C interface */

	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
	#define DDS2_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS2 */
	#define DDS3_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS3 */

	///#define DDS1_TYPE DDS_TYPE_AD9834
	///#define targetdds1 SPI_CSEL1 	/* DDS1 - LO1 output - работает AD9834 (Vasiliy, Ufa). */
	/* Назначение адресов на SPI шине */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	//#define targetdds3 SPI_CSEL1 	/* DDS2 - LO2 output - RU6NFK with FQMODEL_10M7_500K */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	#define targetext1 SPI_CSEL2 	/* external devices control */
	#define targetctl1 SPI_CSEL3 	/* control register */
	#define targetnvram SPI_CSEL4  	/* serial nvram */
	#define targetlcd targetext1
	#define targetuc1608 targetext1

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define BOARD_FILTER_0P5		BOARD_FILTERCODE_1	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_2P7		BOARD_FILTERCODE_0	/* 3.1 or 2.75 kHz filter */

	#define BOARD_FILTER_15P0 BOARD_FILTER_2P7
	#define BOARD_FILTER_15P0_NFM BOARD_FILTER_2P7

	#define WITHMODESETMIXONLY3NFM 1	/* К режимам работы только смесительного детектора добавляется NFM */
	#define WITHDEDICATEDNFM 1		/* Для NFM отдельный тракт со своим фильтром */

	#define IF3_FMASK (IF3_FMASK_2P7 | IF3_FMASK_0P5 | IF3_FMASK_15P0)
	#define IF3_FMASKTX (IF3_FMASK_2P7 | IF3_FMASK_15P0)
	#define IF3_FHAVE (IF3_FMASK_2P7 | IF3_FMASK_0P5 | IF3_FMASK_15P0)

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
#if WITHTEMPSENSOR
	TEMPIX = 16,		// ADC1->CR2 |= ADC_CR2_TSVREFE;	// для тестов
#endif
#if WITHVOLTLEVEL 
	VOLTSOURCE = 4,		// Средняя точка делителя напряжения, для АКБ
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
#if WITHSWRMTR
	PWRI = 4,
	FWD = 4, REF = 3,	// SWR-meter
#endif /* WITHSWRMTR */
	//KI0 = 5, KI1 = 6, KI2 = 7	// клавиатура
	KI0 = 6, KI1 = 7	// клавиатура
};

#define VOLTLEVEL_UPPER		43	// 4.3 kOhm - верхний резистор делителя датчика напряжения
#define VOLTLEVEL_LOWER		10	// 1.0 kOhm - нижний резистор

//#define KI_COUNT 3	// количество используемых под клавиатуру входов АЦП
//#define KI_LIST	KI2, KI1, KI0,	// инициализаторы для функции перекодировки
#define KI_COUNT 2	// количество используемых под клавиатуру входов АЦП
#define KI_LIST	KI1, KI0,	// инициализаторы для функции перекодировки

#endif /* ARM_STM32FXXX_TQFP64_CTLSTYLE_V1K_H_INCLUDED */
