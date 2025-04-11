/* $Id$ */
/* board-specific CPU attached signals */
/*
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
 *
 * Плата на основе вычислительного модуля Orange Pi CM4 (Rockchip RK3566 + 2Гб DDR4)
 * и FPGA Artix-7 XC7A50T, интерфейс связи - PCI Express x1, LTC2208 и DAC904E, by RA4ASN
 *
 */

#ifndef LINUX_ROCKCHIP_CTLSTYLE_OPICM4_XC7A50T_PCIE_FULL_H_INCLUDED
#define LINUX_ROCKCHIP_CTLSTYLE_OPICM4_XC7A50T_PCIE_FULL_H_INCLUDED 1

	#if ! defined(RK3566)
		#error Wrong CPU selected. RK3566 expected
	#endif /* ! defined(RK3566) */

	#define DIRECT_122M88_X1			1	/* Тактовый генератор 122.880 МГц */
	#define BANDSELSTYLERE_UPCONV56M	1

	#define FQMODEL_FPGA		1	// FPGA + IQ over I2S
	#define XVTR_R820T2			1	// ad936x
	#define	WITHAD936XIIO		1	// External AD936x board
//todo: объединить эти 2 дефайна

	// --- вариации прошивки, специфические для разных частот

//	#define CTLREGMODE_NOCTLREG	1
	#define CTLREGMODE_AXU2CGA_FULL		1

	#define WITHPABIASMIN		0
	#define WITHPABIASMAX		255

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_SSB 	0		// Заглушка

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define BOARD_FILTER_0P5		1	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_3P1		0	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_6P0		0	/* 6.0 kHz filter */
	#define BOARD_FILTER_8P0		0	/* 6.0 kHz filter */
	// --- заглушки для плат с DSP обработкой

	#define WITHPREAMPATT2_6DB 1	/* LTC2208 Управление УВЧ и двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB */
	//#define WITHATT2_6DB	1		// LTC2217 Управление двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB без УВЧ
	#define DEFPREAMPSTATE 	0	/* УВЧ по умолчанию включён (1) или выключен (0) */
	#define WITHAGCMODEONOFF	1	// АРУ вкл/выкл

	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
	#define LCDMODE_V5A	1	/* только главный экран с двумя видеобуферами 32 бит ARGB8888, без PIP */
	#define LCDMODE_AT070TN90 1	/* AT070TN90 panel (800*480) - 7" display */
	#define WITHTFT_OVER_LVDS	1	// LVDS receiver THC63LVDF84B


	#define ENCRES_DEFAULT ENCRES_128
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	
	#define WITHENCODER2	1		/* есть второй валкодер */
	#define ENCODER2_EVDEV	1		// Linux Input device
	#define BOARD_ENCODER2_DIVIDE 2		/* значение для валкодера PEC16-4220F-n0024 (с трещёткой") */
	/* Board hardware configuration */
	#define CODEC1_TYPE CODEC_TYPE_ALSA
	#define WITHFPGAIF_FRAMEBITS 256	// Полный размер фрейма
	#define CODEC1_FORMATI2S_PHILIPS 1	// Возможно использование при передаче данных в кодек, подключенный к наушникам и микрофону
	#define CODEC1_FRAMEBITS 64		// Полный размер фрейма для двух каналов - канал кодека

	#define WITHSMPSYSTEM	1	/* разрешение поддержки SMP, Symmetric Multiprocessing */
	#define WITHINTEGRATEDDSP		1	/* в программу включена инициализация и запуск DSP части. */
	#define WITHIF4DSP	1			/*  "Дятел" */
	#define WITHDSPEXTDDC 1			/* Квадратуры получаются внешней аппаратурой */
	//#define WITHDSPEXTFIR 1			/* Фильтрация квадратур осуществляется внешней аппаратурой */
	#define WITHDSPLOCALFIR 1		/* test: Фильтрация квадратур осуществляется процессором */
	#define WITHDSPLOCALTXFIR 1
	#define WITHDACSTRAIGHT 1		/* Требуется формирование кода для ЦАП в режиме беззнакового кода */
	#define WITHTXCWREDUCE	1	/* для получения сравнимой выходной мощности в SSB и CW уменьшен уровень CW и добавлено усиление аналоговой части. */
	#define WITHDEFDACSCALE 100	/* 0..100: настраивается под прегруз драйвера. (ADT1-6T, 200 Ohm feedbask) */
	#define BOARD_FFTZOOM_POW2MAX 3	// Возможные масштабы FFT x1, x2, x4, x8

	#define WITHRTS96 				1		/* Получение от FPGA квадратур, возможно передача по USB и отображение спектра/водопада. */
	#define WITHFFTSIZEWIDE 		1024		/* Отображение спектра и волопада */
	#define WITHVIEW_3DSS			1
	#define WITHDEFAULTVIEW			VIEW_3DSS
	#define WITHVIEW_3DSS_MARK		1
	#define WITHDISPLAY_FPS			30
	#define WITHDISPLAYSWR_FPS		30
	#define WITHSPECBETA_DEFAULT	30
	#define WITHAFSPECTRE			1		/* показ спктра прослушиваемого НЧ сигнала. */
	#define WITHFFTSIZEAF 			512		/* Отображение спектра НЧ сигнвлв */
//	#define WITHLVGL				1
	#define WITHTOUCHGUI			1
//	#define WITHGUIDEBUG			1	/* вывод отладочной информации на экран по удержанию Break-In */
	#define WITHAFSPECTRE			1	/* показ спктра прослушиваемого НЧ сигнала. */
	#define WITHALPHA				24
	#define FORMATFROMLIBRARY 		1
	#define WITHAFGAINDEFAULT		14
	#define WITHALTERNATIVEFONTS	1
//	#define WITHAFEQUALIZER			1
	#define WITHALTERNATIVELAYOUT	1
	#define WITHRLEDECOMPRESS		1	/* поддержка вывода сжатых RLE изображений, пока что только для ARGB8888 видеобуфера */
	#define WITHFT8					1	/* Поддержка протокола FT8. Для фонового декодирования требуется минимум двухъядерный процессор и внешняя оперативная память */
//	#define WITHGNSS				1
//	#define WITHNMEA				1
//	#define WITHLFM					1
	#define WITHLFMTOFFSET			15
	#define DMABUFCLUSTER			32
	#define DMABUFSCALE				1
	#define WITHIQSHIFT				1
//	#define DEFAULTDIALFREQ			12289000uL
//	#define WITHCPUFANPWM			1
	#define WITHCPUTEMPERATURE		1
//	#define WITHEXTIO_LAN			1 	// ExtIO_Storch.dll for SDRSharper
	#define WITHWNB					1	// Simple noise blanker
	#define WITHAUDIOSAMPLESREC		1	// запись и воспроизведение коротких фрагментов звука из эфира
//	#define REALREFERENCE_FREQ 		61443000L
	#define MOUSE_EVDEV				1
	#define MOUSE_CURSOR_PATH		"./arrow.png"
	#define BLUETOOTH_ALSA			1

	#define WITHNOTXDACCONTROL	1	/* в этой версии нет ЦАП управления смещением TXDAC передатчика */

	#define WITHTX	1
	#if WITHTX
		#define WITHSHOWSWRPWR 1	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		#define WITHSWRMTR	1		/* Измеритель КСВ */
	#endif /* WITHTX */
	#define WITHBARS		1	/* отображение S-метра и SWR-метра */
	#define WITHSPKMUTE		1	/* управление выключением динамика */

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */

	//#define WITHONLYBANDS 1		/* Перестройка может быть ограничена любительскими диапазонами */
	//#define WITHBCBANDS		1		/* в таблице диапазонов присутствуют вещательные диапазоны */
	//#define WITHWARCBANDS	1	/* В таблице диапазонов присутствуют HF WARC диапазоны */
	
	#define WITHDIRECTBANDS 1	/* Прямой переход к диапазонам по нажатиям на клавиатуре */
	#define WITHPOWERTRIM		1	// Имеется управление мощностью

	/* что за память настроек и частот используется в контроллере */
	#define NVRAM_TYPE NVRAM_TYPE_LINUX
	#define RTC1_TYPE  RTC_TYPE_LINUX
	#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */
	#define MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */
	/* Board hardware configuration */
	#define DDS1_TYPE 		DDS_TYPE_XDMA
	#define TSC1_TYPE 		TSC_TYPE_EVDEV

	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */

	#define WITHKEYBOARD 1	/* в данном устройстве есть клавиатура */
	#define KEYBOARD_SINGLE	1
	#define KEYBOARD_EVDEV	1		// Linux Input device

	#define WITHMODESETFULLNFM 1
	/* все возможные в данной конфигурации фильтры */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1 /* | IF3_FMASK_6P0 | IF3_FMASK_8P0*/)
	/* все возможные в данной конфигурации фильтры для передачи */
	#define IF3_FMASKTX	(IF3_FMASK_3P1 /*| IF3_FMASK_6P0 */)
	/* фильтры, для которых стоит признак HAVE */
	#define IF3_FHAVE	( IF3_FMASK_0P5 | IF3_FMASK_3P1 /*| IF3_FMASK_6P0 | IF3_FMASK_8P0*/)

	//#define WITHDCDCFREQCTL	1		// Имеется управление частотой преобразователей блока питания и/или подсветки дисплея

	#define VOLTLEVEL_UPPER		47	// 4.7 kOhm - верхний резистор делителя датчика напряжения
	#define VOLTLEVEL_LOWER		10	// 1 kOhm - нижний резистор

	// Назначения входов АЦП процессора.
	enum
	{
		WPM_POTIX = BOARD_ADCX1IN(2),			// MCP3208 CH2 потенциометр управления скоростью передачи в телеграфе
		IFGAIN_IXI = BOARD_ADCX1IN(0),			// MCP3208 CH0 IF GAIN
		AFGAIN_IXI = BOARD_ADCX1IN(1),			// MCP3208 CH1 AF GAIN

		// толькло основная плата - 5W усилитель

		//#define WITHCURRLEVEL	1	/* отображение тока оконечного каскада */
		#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
		//#define WITHTHERMOLEVEL	1	/* отображение температуры */

		#if WITHCURRLEVEL
			//PASENSEIX = BOARD_ADCXIN(0),		// MCP3208 CH0 PA current sense - ACS712-30 chip
			PASENSEIX = 12,		// PC2 PA current sense - ACS712-05 chip
		#endif /* WITHCURRLEVEL */
		#if WITHVOLTLEVEL
			VOLTSOURCE = BOARD_ADCX1IN(7),		// Средняя точка делителя напряжения, для АКБ
		#endif /* WITHVOLTLEVEL */

		#if WITHTHERMOLEVEL
			XTHERMOIX = BOARD_ADCX1IN(6),		// Exernal thermo sensor ST LM235Z
		#endif /* WITHTHERMOLEVEL */

		#if WITHSWRMTR
			//FWD = BOARD_ADCXIN(2), REF = BOARD_ADCXIN(3),		// MCP3208 CH2, CH3 Детектор прямой, отраженной волны
			FWD = 14, REF = 15,	// PC4, PC5	SWR-meter
			PWRI = FWD,
		#endif /* WITHSWRMTR */


		XTHERMOMRRIX = BOARD_ADCMRRIN(0),	// кеш - индекc не должен повторяться в конфигурации
		PASENSEMRRIX = BOARD_ADCMRRIN(1),	// кеш - индекc не должен повторяться в конфигурации
		REFMRRIX = BOARD_ADCMRRIN(2),
		FWDMRRIX = BOARD_ADCMRRIN(3),
		PWRMRRIX = FWDMRRIX,
		VOLTMRRIX = BOARD_ADCMRRIN(4),	// кеш - индекc не должен повторяться в конфигурации
		PASENSEMRRIX2 = BOARD_ADCMRRIN(5),		// кеш - индекc не должен повторяться в конфигурации
		PAREFERMRRIX2 = BOARD_ADCMRRIN(6),		// кеш - индекc не должен повторяться в конфигурации

		KI0 = 0, KI1 = 1, KI2 = 2, KI3 = 7, KI4 = 10	// клавиатура - PA0, PA1, PA2, PA7, PC0
	};

	#define KI_COUNT 5	// количество используемых под клавиатуру входов АЦП
	#define KI_LIST	KI4, KI3, KI2, KI1, KI0,	// инициализаторы для функции перекодировки

	#define BOARDPOWERMIN	0	// Нижний предел регулировки (показываемый на дисплее)
	#define BOARDPOWERMAX	100	// Верхний предел регулировки (показываемый на дисплее)

#endif /* LINUX_ROCKCHIP_CTLSTYLE_OPICM4_XC7A50T_PCIE_FULL_H_INCLUDED */
