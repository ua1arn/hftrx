/* $Id$ */
/* satmod9_v0 ARM board */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef ARM_CTLSTYLE_V9_H_INCLUDED
#define ARM_CTLSTYLE_V9_H_INCLUDED 1
	/* модели синтезаторов - схемы частотообразования */
	#define DIRECT_50M0_X1		1	/* Board hardware configuration */
	//#define DIRECT_125M0_X1		1	/* Board hardware configuration */
	#define FQMODEL_TRX8M		1	// Первая низкая ПЧ 8 МГц, 6 МГц, 5.5 МГц и другие
	//#define FQMODEL_DCTRX		1	// прямое преобразование
	#define WITHNESTEDINTERRUPTS	1	/* используется при наличии real-time части. */

	//#define BANDSELSTYLERE_NOTHING 1
	#define BANDSELSTYLERE_LOCONV32M_NLB 1
	// Выбор ПЧ
	//#define IF3_MODEL IF3_TYPE_9000
	#define IF3_MODEL IF3_TYPE_8868
	#define	MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */

	#define DEFPREAMPSTATE 	1	/* УВЧ по умолчанию включён (1) или выключен (0) */

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	//#define BOARD_DETECTOR_MUTE 0x02
	//#define BOARD_DETECTOR_SSB 0x00
	//#define BOARD_DETECTOR_AM 0x01
	//#define BOARD_DETECTOR_FM 0x03

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define BOARD_FILTER_0P5		0x00	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_1P8		0x01	/* 1.8 kHz filter - на очень старых платах */
	#define BOARD_FILTER_2P7		0x02	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_3P1		0x03	/* 3.1 or 2.75 kHz filter */

	#define WITHMODESETMIXONLY 1	/* CW/CWR/USB/LSB */

	#define ENCRES_DEFAULT ENCRES_128
	/* все возможные в данной конфигурации фильтры */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
	#define IF3_FHAVE 	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
	#define IF3_FMASKTX	(IF3_FMASK_3P1)

	// +++ вариации прошивки, специфические для плат на ATMega
	// --- вариации прошивки, специфические для плат на ATMega

	// +++ Особые варианты расположения кнопок на клавиатуре
	//#define KEYB_RA1AGG	1	/* расположение кнопок для корпуса, сделанного RA1AGG - три группы по четыре линейки кнопок друг за другом в один ряд. */
	//#define KEYB_UA1CEI	1	/* расположение кнопок для UA1CEI */
	// --- Особые варианты расположения кнопок на клавиатуре

	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
	//#define LCDMODE_S1D13781_NHWACCEL 1	/* Неиспользоване аппаратных особенностей EPSON S1D13781 при выводе графики */
	//#define LCDMODE_S1D13781	1	/* Инндикатор 480*272 с контроллером Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - перевернуть изображение */
	#define LCDMODE_UC1608	1		/* Индикатор 240*128 с контроллером UC1608.- монохромный */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - перевернуть изображение (для выводов сверху) */
	// --- Одна из этих строк определяет тип дисплея, для которого компилируется прошивка


	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */
	#define WITHDEBUG		1	/* Отладочная печать через COM-порт. Без CAT (WITHCAT) */
	//#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	#define CTLREGSTYLE_NOCTLREG 1
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0	// stub
	#define WITHPREAMPATT2_6DB		1	// Управление УВЧ и двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB */
	#define WITHUSESDCARD 1			// Включение поддержки SD CARD
	//#define WITHUSEAUDIOREC	1	// Запись звука на SD CARD

	/* что за память настроек и частот используется в контроллере */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// Так же при использовании FM25040A - 5 вольт, 512 байт
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_ATMEGA

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A
	#define NVRAM_TYPE NVRAM_TYPE_NOTHING	// нет NVRAM
	#define HARDWARE_IGNORENONVRAM	1		// отладка на платах где нет никакого NVRAM

	// End of NVRAM definitions section
	#define PLL1_TYPE PLL_TYPE_NONE		// ФАПЧ внешняя, никак не управляется. For zak user

/* Назначение адресов на SPI шине */
#define targetsdcard	SPI_CSEL0 	/* SD CARD */
#define targetnvram		SPI_CSEL1  	/* serial nvram */
#define targetlcd	SPI_CSEL2 	/* TFT over SPI line devices control */
#define targetctl1		SPI_CSEL6 	/* control register */
#define targetuc1608 0

#define WITHKEYBOARD 1	/* в данном устройстве есть клавиатура */
#define KEYBOARD_USE_ADC	1	/* на одной линии установлено  четыре  клавиши. на vref - 6.8K, далее 2.2К, 4.7К и 15К. */
// Назначения входов АЦП процессора.
enum 
{ 
	KI0 = 5, KI1 = 6, KI2 = 7, // клавиатура
};
#define	KI_COUNT 3
#define KI_LIST	KI2, KI1, KI0,	// инициализаторы для функции перекодировки

#endif /* ARM_CTLSTYLE_V9_H_INCLUDED */
