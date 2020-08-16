/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef ARM_CTLSTYLE_V7A_H_INCLUDED
#define ARM_CTLSTYLE_V7A_H_INCLUDED 1
	/* модели синтезаторов - схемы частотообразования */

#if 1
	/* Версии частотных схем - с преобразованием "наверх" */
	#define FQMODEL_73050		1	// 1-st IF=73.050, 2-nd IF=0.455 MHz
	//#define FQMODEL_73050_IF0P5		1	// 1-st IF=73.050, 2-nd IF=0.5 MHz
	//#define FQMODEL_80455		1	// 1-st IF=80.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455		1	// 1-st IF=64.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455_IF0P5		1	// 1-st IF=64.455, 2-nd IF=0.5 MHz
	//#define FQMODEL_70455		1	// 1-st IF=70.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	#define BANDSELSTYLERE_UPCONV56M	1	/* Up-conversion with working band .030..56 MHz */

#else
	/* Версии частотных схем - с преобразованием "наверх" */
	//#define FQMODEL_45_IF8868	1	// SW2011
	//#define FQMODEL_45_IF0P5	1	// 1-st IF=45MHz, 2-nd IF=500 kHz
	#define FQMODEL_45_IF455	1	// 1-st IF=45MHz, 2-nd IF=455 kHz
	#define BANDSELSTYLERE_UPCONV36M	1	/* Up-conversion with working band 0.1 MHz..36 MHz */
	//#define BANDSELSTYLERE_UPCONV32M	1	/* Up-conversion with working band 0.1 MHz..32 MHz */
#endif

	#define	FONTSTYLE_ITALIC	1	//

	//#define MODEL_DIRECT	1		// использовать DDS первого гетеродина как единственный
	// --- вариации прошивки, специфические для разных плат

	// +++ вариации прошивки, специфические для плат на ATMega
	// --- вариации прошивки, специфические для плат на ATMega

	// +++ Особые варианты расположения кнопок на клавиатуре
	#define KEYB_RA1AGG	1	/* расположение кнопок для корпуса, сделанного RA1AGG - три группы по четыре линейки кнопок друг за другом в один ряд. */
	//#define KEYB_UA1CEI	1	/* расположение кнопок для UA1CEI */
	// --- Особые варианты расположения кнопок на клавиатуре

	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
	//#define LCDMODE_HARD_SPI	1		/* LCD over SPI line */
	//#define LCDMODE_WH1602	1	/* тип применяемого индикатора 16*2 */
	#define LCDMODE_WH2002	1	/* тип применяемого индикатора 20*2, возможно вместе с LCDMODE_HARD_SPI */
	//#define LCDMODE_WH2004	1	/* тип применяемого индикатора 20*4 */
	// --- Одна из этих строк определяет тип дисплея, для которого компилируется прошивка

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_100
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define WITHENCODER	1	/* для изменения частоты имеется енкодер */
	#define WITHNESTEDINTERRUPTS	1	/* используется при наличии real-time части. */

	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	#define WITHTX		1	/* включено управление передатчиком - сиквенсор, электронный ключ. */
	//#define WITHPBT		1	/* используется PBT */
	#define WITHCAT		(1 && ! WITHDEBUG)	/* используется CAT */
	#define WITHVOX		1	/* используется VOX */
	#define WITHSWRMTR	1	/* Измеритель КСВ */
	//#define WITHPWRMTR	1	/* Индикатор выходной мощности или */
	#define WITHBARS	1	/* отображение S-метра и SWR-метра */
	//#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
	//#define WITHSWLMODE	1	/* поддержка запоминания множества частот в swl-mode */

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */
	#define WITHVIBROPLEX	1	/* возможность эмуляции передачи виброплексом */
	#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	//#define LO1PHASES	1		/* Прямой синтез первого гетеродина двумя DDS с програмимруемым сдвигом фазы */
	#define WITHMODESETFULLNFM 1	/* используется FM */
	//#define WITH2K4SSB	1		/* на месте 6 кГц фильтра стоит 2.7 кГц ЭМФ */
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

/* плата, на которой второй DDS - это AD9834,
   первый - AD9951, используется кварцевый генератор на 64 МГц */


#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */

/* Board hardware configuration */
#define DDS1_TYPE DDS_TYPE_AD9951
#define DDS2_TYPE DDS_TYPE_AD9834
#define PLL1_TYPE PLL_TYPE_ADF4001	// комметтировать, если первый гетеродин это только DDS без делителя.
#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
#define DDS2_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS2 */
#define HYBRID_NVFOS 6				/* количество ГУН первого гетеродина */
#define HYBRID_PLL1_POSITIVE 1		/* в петле ФАПЧ нет инвертирующего усилителя */
//#define HYBRID_PLL1_NEGATIVE 1		/* в петле ФАПЧ есть инвертирующий усилитель */


/* Назначение адресов на SPI шине */
#define targetdds1 SPI_CSEL0	/* DDS1 - AD9951 */
#define targetdds2 SPI_CSEL1	/* DDS2 - AD9834 LO3 output */
#define targetpll1 SPI_CSEL2	/* ADF001 after DDS1 - divide by r1 and scale to n1. Для двойной DDS первого гетеродина - вторая DDS */
#define targetrxc1 SPI_CSEL5	/* RX control register */
#define targetnvram SPI_CSEL6 	/* nvram serial nvram */
#define targetctldac1 SPI_CSEL7	/* control register and DAC */

/* коды входов коммутатора источников сигнала для УНЧ приёмника */
#define BOARD_DETECTOR_MUTE 0x02
#define BOARD_DETECTOR_SSB 0x00
#define BOARD_DETECTOR_AM 0x01
#define BOARD_DETECTOR_FM 0x03
#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
#define BOARD_FILTER_0P5	0x03	/* 0.5 or 0.3 kHz filter */
//#define BOARD_FILTER_WIDE9	0x00	/* 9 kHz filter - на очень старых платах */
//#define BOARD_FILTER_SEMINARROW	0x01	/* 1.8 kHz filter - на очень старых платах */
#define BOARD_FILTER_3P1		0x06	/* 3.1 or 2.75 kHz filter */
#define BOARD_FILTER_6P0	0x02	/* 6 kHz filter */
#define BOARD_FILTER_8P0	0x04	/* only 1-st IF filter used (6, 8 or 15 khz width) */

	//#define WITHMODESETSMART 1	/* в заваисмости от частоты меняется боковая, используется вместе с WITHFIXEDBFO */
	#if FQMODEL_64455_IF0P5 || FQMODEL_73050_IF0P5
		//#define WITHMODESETFULL 1
		#define WITHMODESETFULLNFM 1
		//#define WITHWFM	1			/* используется WFM */
		#define IF3_FMASK (IF3_FMASK_8P0 | IF3_FMASK_6P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FHAVE (IF3_FMASK_8P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
	#elif FQMODEL_64455 || FQMODEL_73050
		//#define WITHMODESETFULL 1
		#define WITHMODESETFULLNFM 1
		//#define WITHWFM	1			/* используется WFM */
		#define IF3_FMASK (IF3_FMASK_8P0 | IF3_FMASK_6P0  | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FHAVE (IF3_FMASK_8P0 | IF3_FMASK_6P0  | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
	#else
		//#define WITHMODESETFULL 1
		#define WITHMODESETFULLNFM 1
		//#define WITHWFM	1			/* используется WFM */
		#define IF3_FMASK (IF3_FMASK_15P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FHAVE (IF3_FMASK_15P0 | IF3_FMASK_3P1 | IF3_FMASK_0P5)
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
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
#if WITHVOX
	VOXIX = 2, AVOXIX = 1,	// VOX
#endif
	SMETERIX = 0,	// S-meter
	PWRI = 4,
	FWD = 4, REF = 3,	// SWR-meter
	KI0 = 5, KI1 = 6, KI2 = 7	// клавиатура
};
#define KI_COUNT 3	// количество используемых под клавиатуру входов АЦП
	#define KI_LIST	KI2, KI1, KI0,	// инициализаторы для функции перекодировки

//#define WITHPOWERTRIM		1	// Имеется управление мощностью
#define WITHPOWERTRIMMIN	5	// Нижний предел регулировки (показываемый на дисплее)
#define WITHPOWERTRIMMAX	100	// Верхний предел регулировки (показываемый на дисплее)
#define WITHPOWERTRIMATU	15	// Значение для работы автотюнера


#endif /* ARM_CTLSTYLE_V7A_H_INCLUDED */
