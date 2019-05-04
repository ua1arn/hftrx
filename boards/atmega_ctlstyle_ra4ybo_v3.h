/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// аппаратута трансивера RA4YBO V3
// "Стационарный с Павлином" STM32F103C8T6, 2*HMC830, up-conversion to 64455 для RA4YBO
//

#ifndef ATMEGA_CTLSTYLE_RA4YBO_V3_H_INCLUDED
#define ATMEGA_CTLSTYLE_RA4YBO_V3_H_INCLUDED 1

	#define WITHUSEPLL		1	/* Главная PLL	*/

	// STM32F103C8T6 Medium-density performance line
	#if WITHCPUXTAL
		#define	REFINFREQ WITHCPUXTAL
		#define REF1_DIV 1
		#define REF1_MUL 9	// Up to 16 supported
	#else
		#define	REFINFREQ 8000000UL
		#define REF1_DIV 2
		#define REF1_MUL 8	// Up to 16 supported
	#endif

	/* модели синтезаторов - схемы частотообразования */

	// --- вариации прошивки, специфические для разных плат
	//#define CTLREGMODE_RA4YBO_V3	1
	#define CTLREGMODE_RA4YBO_V3A	1

	#define WITHPREAMPATT2_10DB		1	// Управление УВЧ и двухкаскадным аттенюатором

	#define	BOARD_AGCCODE_0		0x00
	#define	BOARD_AGCCODE_1		0x01
	#define	BOARD_AGCCODE_2		0x02
	#define	BOARD_AGCCODE_3		0x03

	#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_0
	#define	BOARD_AGCCODE_MED	BOARD_AGCCODE_1
	#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_2
	#define	BOARD_AGCCODE_LONG	BOARD_AGCCODE_3
	#define	BOARD_AGCCODE_OFF	0xFF

	#define WITHAGCMODE4STAGES	1	// 4 скорости. выключенно не бывает

	#define	FONTSTYLE_ITALIC	1	//
	#define DSTYLE_UR3LMZMOD	1


	// +++ вариации прошивки, специфические для плат на ATMega
	// --- вариации прошивки, специфические для плат на ATMega

	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	//#define LCDMODE_WH2002	1	/* тип применяемого индикатора 20*2, возможно вместе с LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* тип применяемого индикатора 16*2 */
	//#define LCDMODE_WH2004	1	/* тип применяемого индикатора 20*4 */
	//#define LCDMODE_RDX0077	1	/* Индикатор 128*64 с контроллером UC1601.  */
	//#define LCDMODE_RDX0154	1	/* Индикатор 132*64 с контроллером UC1601. */
	//#define LCDMODE_RDX0120	1	/* Индикатор 64*32 с контроллером UC1601.  */
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
	//#define LCDMODE_L2F50_TOPDOWN	1	/* Переворот изображени я в случае LCDMODE_L2F50 */
	#define LCDMODE_S1D13781	1	/* Инндикатор 480*272 с контроллером Epson S1D13781 */
	#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - перевернуть изображение */
	//#define LCDMODE_ILI9225	1	/* Индикатор 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 с контроллером ILI9225С */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - перевернуть изображение (для выводов слева от экрана) */
	//#define LCDMODE_UC1608	1		/* Индикатор 240*128 с контроллером UC1608.- монохромный */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ST7735	1	/* Индикатор 160*128 с контроллером Sitronix ST7735 */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - перевернуть изображение (для выводов справа) */
	//#define LCDMODE_ST7565S	1	/* Индикатор WO12864C2-TFH# 128*64 с контроллером Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ILI9320	1	/* Индикатор 248*320 с контроллером ILI9320 */
	// --- Одна из этих строк определяет тип дисплея, для которого компилируется прошивка

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_100
	//#define ENCRES_DEFAULT ENCRES_24
	#define	WITHENCODER	1	/* для изменения частоты имеется енкодер */
	#define ENCODER_REVERSE	1	/* шагать в обртную сторону */

	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	#define WITHTX		1	/* включено управление передатчиком - сиквенсор, электронный ключ. */
	//#define WITHCAT		1	/* используется CAT */
	//#define WITHDEBUG		1	/* Отладочная печать через COM-порт. Без CAT (WITHCAT) */
	#define WITHVOX		1	/* используется VOX */
	#define WITHBARS	1	/* отображение S-метра и SWR-метра */
	#define WITHSHOWSWRPWR 1	/* на дисплее одновременно отображаются SWR-meter и PWR-meter - Требует WITHSWRMTR */
	#define WITHSWRMTR	1	/* Измеритель КСВ или */
	//#define WITHPWRMTR	1	/* Индикатор выходной мощности  */
	//#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
	//#define WITHSWLMODE	1	/* поддержка запоминания множества частот в swl-mode */
	//#define WITHPBT		1	/* используется PBT */
	#define WITHIFSHIFT	1	/* используется IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* Начальное занчение IF SHIFT */
	#define WITHMODESETFULLNFM 1
	#define WITHWFM	1			/* используется WFM */
	#define WITHCURRLEVEL	1

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */
	#define WITHVIBROPLEX	1	/* возможность эмуляции передачи виброплексом */

	#define	WITHAUTOTUNER	1	/* Есть функция автотюнера */
	//#define FULLSET8 1
	#define SHORTSET8 1
	#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	//#define LO1PHASES	1		/* Прямой синтез первого гетеродина двумя DDS с програмимруемым сдвигом фазы */
	#define DEFPREAMPSTATE 	1	/* УВЧ по умолчанию включён (1) или выключен (0) */
	
	/* что за память настроек и частот используется в контроллере */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// Так же при использовании FM25040A - 5 вольт, 512 байт
	#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A

	// End of NVRAM definitions section

	//#define FTW_RESOLUTION 28	/* разрядность FTW выбранного DDS */
	#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */
	//#define FTW_RESOLUTION 31	/* разрядность FTW выбранного DDS - ATINY2313 */

	#define LO1MODE_DIRECT		1

#if CTLREGMODE_RA4YBO_V3

	#define PLL1_TYPE PLL_TYPE_HMC830
	#define PLL1_FRACTIONAL_LENGTH	24	/* количество бит в дробной части делителя */
	#define PLL2_TYPE PLL_TYPE_LMX2306
	//#define PLL2_TYPE PLL_TYPE_HMC830
	//#define PLL2_FRACTIONAL_LENGTH	24	/* количество бит в дробной части делителя */

	#define REFERENCE_FREQ	50000000L
	#define	SYNTH_R1		1


#elif CTLREGMODE_RA4YBO_V3A

	#define PLL1_TYPE PLL_TYPE_SI570
	#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	#define DDS2_TYPE DDS_TYPE_AD9834
	#define PLL2_TYPE PLL_TYPE_LMX2306	/* Делитель октавного гетеродина для получения сквозного диапазона */
	#define REFERENCE_FREQ	50000000UL	/* LO2 = 72595 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */


#if 1
	// Частота сравнения около 25 кГц
	// Версия под фильтр 1-й ПЧ 80.455
	// IF2=500 kHz
	// LO2=79995000
	#define LO2_PLL_N500	3203	// 79,955,067 Hz 
	#define LO2_PLL_R500	2003
	// IF2=455 kHz
	// LO2=80000000
	#define LO2_PLL_N455	3200
	#define LO2_PLL_R455	2000
#else
	// Частота сравнения около 50 кГц
	// Версия под фильтр 1-й ПЧ 80.455
	// IF2=500 kHz
	#define LO2_PLL_N500	1593
	#define LO2_PLL_R500	998
	// IF2=455 kHz
	#define LO2_PLL_N455	1600
	#define LO2_PLL_R455	1000
#endif
	/* биты в регистрах управления ADG714 */
	#define ADG714_OFF	0x00u	/* все выключены */
	#define ADG714_S1D1 (0x01u << 0)
	#define ADG714_S2D2 (0x01u << 1)
	#define ADG714_S3D3 (0x01u << 2)
	#define ADG714_S4D4 (0x01u << 3)
	#define ADG714_S5D5 (0x01u << 4)
	#define ADG714_S6D6 (0x01u << 5)
	#define ADG714_S7D7 (0x01u << 6)
	#define ADG714_S8D8 (0x01u << 7)

	#define MAKEFLTCODE(in, out) ((in) * 256 + (out))	// in: Коммутатор IC1, out: коммутатор IC2

	#define BOARD_FILTER_RX_WFM		MAKEFLTCODE(ADG714_OFF, ADG714_OFF)

	#define BOARD_FILTER500_RX_0P5	MAKEFLTCODE(ADG714_S1D1, ADG714_S1D1)		// emf-500-0.6-S

	#define BOARD_FILTER500_RX_3P1	MAKEFLTCODE(ADG714_S3D3, ADG714_S3D3)		// emf-500-3.1-N
	#define BOARD_FILTER500_TX_3P1	MAKEFLTCODE(ADG714_S4D4, ADG714_S4D4)		// emf-500-3.1-N

	#define BOARD_FILTER455_RX_3P0	MAKEFLTCODE(ADG714_S2D2, ADG714_S2D2)		// collins

	#define BOARD_FILTER455_RX_10P0	MAKEFLTCODE(ADG714_S6D6, ADG714_S6D6)		// piezo
	#define BOARD_FILTER455_TX_10P0	MAKEFLTCODE(ADG714_S5D5, ADG714_S5D5)		// piezo

	#define BOARD_FILTER455_RX_6P0	MAKEFLTCODE(ADG714_S7D7, ADG714_S7D7)		// piezo

	#define BOARD_FILTER455_RX_2P1	MAKEFLTCODE(ADG714_S8D8, ADG714_S8D8)		// piezo


	#define BOARD_FILTER_OFF MAKEFLTCODE(ADG714_OFF, ADG714_OFF)

#endif

	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
	#define DDS1_CLK_MUL	1		/* Умножитель опорной частоты перед подачей в DDS1 */
	#define DDS2_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель опорной частоты перед подачей в DDS1 */
	#define DDS3_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
	#define DDS3_CLK_MUL	1		/* Умножитель опорной частоты перед подачей в DDS1 */

	//#define DDS1_TYPE DDS_TYPE_AD9834

	//#define WITHFIXEDBFO	1	/* Переворот боковых за счёт 1-го гетродина (особенно, если нет подстраиваемого BFO) */
	//#define WITHDUALFLTR	1	/* Переворот боковых за счёт переключения фильтра верхней или нижней боковой полосы */
	#define WITHSAMEBFO	1		/* использование общих настроек BFO для приёма и передачи */
	#define DDS2_TYPE DDS_TYPE_AD9834


	#define RTC1_TYPE RTC_TYPE_M41T81	/* ST M41T81M6 RTC clock chip with I2C interface */

	/* Назначение адресов на SPI шине */
	#define targetdds2 SPI_CSEL_PA9 	/* DDS2 - LO3 output */
	#define targetpll1 SPI_CSEL_PA11 	/* PLL1 - HMC830 */ 
	#define targetpll2 SPI_CSEL_PA10 	/* PLL2 - HMC830 */ 
	#define targetctl1 SPI_CSEL_PA8 	/* control register */
	#define targetnvram SPI_CSEL_PA15  	/* serial nvram */
	#define targetlcd SPI_CSEL_PA12 	/* LCD over SPI line devices control */ 
	

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_SSB 0x03
	#define BOARD_DETECTOR_AM 0x02
	#define BOARD_DETECTOR_FM 0x01
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x01	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	#define BOARD_DETECTOR_MUTE 4

	//#define IF3_FMASK (IF3_FMASK_17P0 | IF3_FMASK_7P8 | IF3_FMASK_3P1 | IF3_FMASK_2P4 | IF3_FMASK_1P5 | IF3_FMASK_0P5 | IF3_FMASK_0P3)
	#define IF3_FMASK (IF3_FMASK_3P1 | IF3_FMASK_0P5 | IF3_FMASK_6P0)
	#define IF3_FHAVE IF3_FMASK
	#define IF3_FMASKTX	(IF3_FMASK_6P0)

	#define WITHSPLIT	1	/* управление режимами расстройки одной кнопкой */
	//#define WITHSPLITEX	1	/* Трехкнопочное управление режимами расстройки */
	#define WITHCATEXT	1	/* Расширенный набор команд CAT */
	#define WITHELKEY	1
	#define WITHKBDENCODER 1	// перестройка частоты кнопками
	#define WITHKEYBOARD 1	/* в данном устройстве есть клавиатура */
	#define KEYBOARD_USE_ADC	1	/* на одной линии установлено  четыре  клавиши. на vref - 6.8K, далее 2.2К, 4.7К и 13K. */
	#define KEYBOARD_USE_ADC6_V1	1
	//#define KEYBOARD_USE_ADC6	1
	#define WITHDIRECTFREQENER	1	/* прямой ввод частоты с клавиатуры */

	#define WITHSPKMUTE		1	/* управление выключением динамика */
	#define WITHANTSELECT	1
	// +++ Особые варианты расположения кнопок на клавиатуре
	#define KEYB_RA4YBO_V3	1	/*  */
	// --- Особые варианты расположения кнопок на клавиатуре
	// Назначения входов АЦП процессора.
	enum 
	{ 
	#if WITHBARS
		SMETERIX = 0,	// S-meter
	#endif
	#if WITHVOX
		VOXIX = 1, AVOXIX = 2,	// VOX
	#endif
	#if WITHSWRMTR
		FWD = 4, REF = 3,	// SWR-meter
		PWRI = 4, 
	#endif
	#if WITHCURRLEVEL
		PASENSEIX = 5,		// PA1 PA current sense - ACS712-30 chip
	#endif /* WITHCURRLEVEL */
		KI0 = 6, KI1 = 7, KI2 = 8, KI3 = 9	// клавиатура
	};
	#define KI_COUNT 4	// количество используемых под клавиатуру входов АЦП


	#define VOLTLEVEL_UPPER		47	// 4.7 kOhm - верхний резистор делителя датчика напряжения
	#define VOLTLEVEL_LOWER		10	// 1.0 kOhm - нижний резистор


	/* Первая ПЧ - 64455, вторая - 500/455 кГц */

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	//#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO1_SIDE_F(freq) (((freq) < 64455000UL) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#if WITHIF4DSP
		// архитектура для DSP на последней ПЧ
		#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#else /* WITHIF4DSP */
		// обычная архитектура для слухового приёмв
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
	#endif /* WITHIF4DSP */

	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 - "затычка" */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 - AD9834 */

	#define	WFM_IF1	10700000ul

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */
	#define LO4_POWER2 0

	#define TUNE_BOTTOM 30000L		/* нижняя частота настройки */
	#define TUNE_TOP 180000000L		/* верхняя частота настройки */


#endif /* ATMEGA_CTLSTYLE_RA4YBO_V3_H_INCLUDED */
