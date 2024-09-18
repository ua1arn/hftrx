/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Трансивер с DSP обработкой "Аист" на процессоре ZYNQ 7000

#ifndef ARM_XC7ZXX_BGAXXX_CTLSTYLE_EBAZ4205_V2_H_INCLUDED
#define ARM_XC7ZXX_BGAXXX_CTLSTYLE_EBAZ4205_V2_H_INCLUDED 1

	#if ! defined(XC7Z020)
		#error Wrong CPU selected. XC7Z020 expected
	#endif /* ! defined(XC7Z020) */

	//#define WITHSAICLOCKFROMI2S 1	/* Блок SAI1 тактируется от PLL I2S */
	// в данной конфигурации I2S и SAI - в режиме SLAVE
	#define WITHI2SCLOCKFROMPIN 1	// тактовая частота на SPI2 (I2S) подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса

	// Варианты конфигурации тактирования
	#define WITHCPUXOSC 33333000uL	/* На процессоре установлен генератор 33.333 МГц */
	#define ARM_PLL_MUL	40
	#define ARM_PLL_DIV	2
	#define IO_PLL_MUL 48	// IO_PLL_CTRL.PLL_FDIV value

	// PL Clock 0 Output control
	// ~50 MHz
	#define SCLR_FPGA0_CLK_CTRL_DIVISOR0 8
	#define SCLR_FPGA0_CLK_CTRL_DIVISOR1 4

	// PL Clock 1 Output control
	// ~50 MHz
	#define SCLR_FPGA1_CLK_CTRL_DIVISOR0 8
	#define SCLR_FPGA1_CLK_CTRL_DIVISOR1 4

	// PL Clock 2 Output control
	// ~50 MHz
	#define SCLR_FPGA2_CLK_CTRL_DIVISOR0 8
	#define SCLR_FPGA2_CLK_CTRL_DIVISOR1 4

	// PL Clock 3 Output control
	// ~50 MHz
	#define SCLR_FPGA3_CLK_CTRL_DIVISOR0 8
	#define SCLR_FPGA3_CLK_CTRL_DIVISOR1 4

	#define DDR_PLL_MUL 32	// DDR_PLL_CTRL.PLL_FDIV value - 1066.656 MHz
	#define DDR_2XCLK_DIVISOR 3	// DDR_CLK_CTRL.DDR_2XCLK_DIVISOR value 355 MHz
	#define DDR_3XCLK_DIVISOR 2	// DDR_CLK_CTRL.DDR_3XCLK_DIVISOR value (only even) 533 MHz

	#define SCLR_UART_CLK_CTRL_DIVISOR_VALUE 16
	#define SCLR_SDIO_CLK_CTRL_DIVISOR_VALUE 16

	/* модели синтезаторов - схемы частотообразования */

	// +++ вариации прошивки, специфические для разных частот опорного генератора

	//#define DIRECT_122M88_X1	1	/* Тактовый генератор 122.880 МГц */
	#define DIRECT_61M440_X1	1
	#define BANDSELSTYLERE_UPCONV56M_36M	1	/* Up-conversion with working band .030..56 MHz */

	#define FQMODEL_FPGA		1	// FPGA + IQ over I2S
	//#define XVTR_NYQ1			1	// Support Nyquist-style frequency conversion

	// --- вариации прошивки, специфические для разных частот

	//#define CTLREGMODE_NOCTLREG	1
	#define CTLREGMODE_ZYNQ_4205	1

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
	#define WITHMIC1LEVEL		1	// установка усиления микрофона

	#define	FONTSTYLE_ITALIC	1	// Использовать альтернативный шрифт

	// +++ Особые варианты расположения кнопок на клавиатуре
	#define KEYBOARD_SINGLE	1
	//#define KEYBOARD_MATRIX_4x4	1

	// --- Особые варианты расположения кнопок на клавиатуре
	#define WITHSPLIT	1	/* управление режимами расстройки одной кнопкой */
	//#define WITHSPLITEX	1	/* Трехкнопочное управление режимами расстройки */

#if WITHISBOOTLOADER
	#define LCDMODE_DUMMY	1

#else /* WITHISBOOTLOADER */
	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
	#define LCDMODE_V5A	1	/* только главный экран с двумя видеобуферами 32 бит ARGB8888, без PIP */
	#define LCDMODE_AT070TN90 1	/* AT070TN90 panel (800*480) - 7" display */
	//#define LCDMODE_AT070TNA2 1	/* AT070TNA2 panel (1024*600) - 7" display */
	#define WITHTFT_OVER_LVDS	1	// LVDS receiver THC63LVDF84B
#endif /* WITHISBOOTLOADER */

#if WITHISBOOTLOADER

	// +++ заглушки для плат с DSP обработкой
	#define	BOARD_AGCCODE_ON	0x00
	#define	BOARD_AGCCODE_OFF	0x01

	#define BOARD_DETECTOR_AM 	0		// Заглушка
	#define BOARD_DETECTOR_FM 	0		// Заглушка
	#define BOARD_DETECTOR_MUTE 	0		// Заглушка
	#define BOARD_DETECTOR_TUNE 	0		// Заглушка

	// +++ заглушки для плат с DSP обработкой
	#define BOARD_NOTCH_OFF		0
	#define BOARD_NOTCH_MANUAL	0
	#define BOARD_NOTCH_AUTO	0

	#define NVRAM_TYPE NVRAM_TYPE_NOTHING	// нет NVRAM
	#define HARDWARE_IGNORENONVRAM	1		// отладка на платах где нет никакого NVRAM

	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
	//#define WITHSMPSYSTEM	1	/* разрешение поддержки SMP, Symmetric Multiprocessing */
	//
	#define WITHUSEMALLOC	1	/* разрешение поддержки malloc/free/calloc/realloc */
	#define WITHUSESDCARD		1	// Включение поддержки SD CARD
	#define WITHISBOOTLOADERFATFS 1
	#define WITHISBOOTLOADERIMAGE "tc1_xc7z010_app_xyl32.bin"

#else /* WITHISBOOTLOADER */

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define WITHENCODER	1	/* для изменения частоты имеется енкодер */
	//#define ENCODER_REVERSE	1	/* разводка на плате с перепутаными фазами от валкодера */
	//#define ENCODER2_REVERSE	1	/* разводка на плате с перепутаными фазами от валкодера */
	#define WITHENCODER2	1		/* есть второй валкодер */
	#define BOARD_ENCODER2_DIVIDE 2		/* значение для валкодера PEC16-4220F-n0024 (с трещёткой") */
	/* Board hardware configuration */
	#define CODEC1_TYPE CODEC_TYPE_TLV320AIC23B
	//#define CODEC_TYPE_TLV320AIC23B_USE_SPI	1
	//#define CODEC_TYPE_TLV320AIC23B_USE_8KS	1	/* кодек работает с sample rate 8 kHz */

	//#define CODEC_TYPE_WM8731_USE_SPI	1
	//#define CODEC_TYPE_WM8731_USE_8KS	1	/* кодек работает с sample rate 8 kHz */

	//#define CODEC1_TYPE CODEC_TYPE_NAU8822L
	//#define CODEC_TYPE_NAU8822_USE_SPI	1
	//#define CODEC_TYPE_NAU8822_USE_8KS	1	/* кодек работает с sample rate 8 kHz */

	//#define WITHDTMFPROCESSING 1
	//#define WITHBBOXMIKESRC BOARD_TXAUDIO_LINE

	#define CODEC2_TYPE	CODEC_TYPE_FPGAV1	/* квадратуры получаем от FPGA */
	//#define CODEC_TYPE_CS4272_USE_SPI	1		// codecboard v2.0
	//#define CODEC_TYPE_CS4272_STANDALONE	1		// codecboard v3.0

	//#define WITHFPGAIF_FRAMEBITS 64	// Полный размер фрейма - только один приемник без спектра.
	#define WITHFPGAIF_FRAMEBITS 256	// Полный размер фрейма
	//#define WITHFPGARTS_FRAMEBITS 64	// Полный размер фрейма для двух квадратур по 24 бита - канал спектроанализатора
	//#define WITHFPGAIF_FORMATI2S_PHILIPS 1	// требуется при получении данных от FPGA
	//#define WITHFPGARTS_FORMATI2S_PHILIPS 1	// требуется при получении данных от FPGA
	#define CODEC1_FORMATI2S_PHILIPS 1	// Возможно использование при передаче данных в кодек, подключенный к наушникам и микрофону
	#define CODEC1_FRAMEBITS 64		// Полный размер фрейма для двух каналов - канал кодека
	//#define CODEC1_IFC_MASTER 1	// кодек формирует синхронизацию

	#define WITHSMPSYSTEM	1	/* разрешение поддержки SMP, Symmetric Multiprocessing */
	//
	#define WITHINTEGRATEDDSP		1	/* в программу включена инициализация и запуск DSP части. */

	#define WITHIF4DSP	1			/*  "Дятел" */
	//#define WITHDACOUTDSPAGC		1	/* АРУ реализовано как выход ЦАП на аналоговую часть. */
	//
	#define WITHDSPEXTDDC 1			/* Квадратуры получаются внешней аппаратурой */
	#define WITHDSPEXTFIR 1			/* Фильтрация квадратур осуществляется внешней аппаратурой */
	//#define WITHDSPLOCALFIR 1		/* test: Фильтрация квадратур осуществляется процессором */
	#define WITHDSPLOCALTXFIR	1
	#define WITHDACSTRAIGHT 1		/* Требуется формирование кода для ЦАП в режиме беззнакового кода */
	#define WITHTXCWREDUCE	1	/* для получения сравнимой выходной мощности в SSB и CW уменьшен уровень CW и добавлено усиление аналоговой части. */
	#define WITHDEFDACSCALE 100	/* 0..100: настраивается под прегруз драйвера. (ADT1-6T, 200 Ohm feedbask) */

	// FPGA section
	//#define WITHFPGAWAIT_AS	1	/* FPGA загружается из собственной микросхемы загрузчика - дождаться окончания загрузки перед инициализацией SPI в процессоре */
	//#define WITHFPGALOAD_PS	1	/* FPGA загружается процессором с помощью SPI */
	//#define WITHFPGALOAD_DCFG	1	/* FPGA загружается процессором через интерфейс XDCFG (ZYNQ7000) */
	//#define BOARD_BITIMAGE_NAME "build/xc7Z010/bitstream_4205_10.h"

	//#define WITHSKIPUSERMODE 1	// debug option: не отдавать в USER MODE блоки для фильтрации аудиосигнала
	#define BOARD_FFTZOOM_POW2MAX 3	// Возможные масштабы FFT x1, x2, x4, x8
	//#define WITHNOSPEEX	1	// Без шумоподавителя SPEEX
	//#define WITHUSEDUALWATCH	1	// Второй приемник
	#define WITHREVERB	1	// ревербератор в обработке микрофонного сигнала
	//#define WITHLOOPBACKTEST	1	/* прослушивание микрофонного входа, генераторов */
	//#define WITHMODEMIQLOOPBACK	1	/* модем получает собственные передаваемые квадратуры */

	//#define WITHUSESDCARD		1	// Включение поддержки SD CARD
	//#define WITHUSEUSBFLASH		1	// Включение поддержки USB memory stick
	//#define WITHUSERAMDISK			1			// создание FATFS диска в озу
	#define WITHUSERAMDISKSIZEKB	(192uL * 1024)	// размр в килобайтах FATFS диска в озу

	//#define WITHUSEAUDIOREC		1	// Запись звука на SD CARD
	//#define WITHUSEAUDIOREC2CH	1	// Запись звука на SD CARD в стерео
	//#define WITHUSEAUDIORECCLASSIC	1	// стандартный формат записи, без "дыр"

	#define WITHRTS96 1		/* Получение от FPGA квадратур, возможно передача по USB и отображение спектра/водопада. */
	#if LCDMODE_AT070TNA2 || LCDMODE_AT070TN90
		#define WITHFFTSIZEWIDE 1024		/* Отображение спектра и волопада */
		#define WITHVIEW_3DSS		1
		#define WITHDEFAULTVIEW		VIEW_3DSS
		#define WITHVIEW_3DSS_MARK	1
		#define WITHDISPLAY_FPS		30
		#define WITHDISPLAYSWR_FPS	30
		#define WITHSPECBETA_DEFAULT	30
		#define WITHAFSPECTRE		1		/* показ спктра прослушиваемого НЧ сигнала. */
		#define WITHFFTSIZEAF 		512		/* Отображение спектра НЧ сигнвлв */
		#if 1
			#define REALREFERENCE_FREQ 		61439600L
			#define WITHTOUCHGUI			1
			#define WITHGUIDEBUG			1	/* вывод отладочной информации на экран по удержанию Break-In */
			#define WITHAFSPECTRE			1	/* показ спктра прослушиваемого НЧ сигнала. */
			#define WITHALPHA				24
			#define FORMATFROMLIBRARY 		1
			#define WITHUSEMALLOC			1	/* разрешение поддержки malloc/free/calloc/realloc */
			#define WITHAFGAINDEFAULT		(0 * BOARD_AFGAIN_MAX / 255)
			#define WITHTHERMOLEVEL			1
			#define WITHALTERNATIVEFONTS	1
//			#define WITHAFEQUALIZER			1
			#define WITHALTERNATIVELAYOUT	1
			#define WITHRLEDECOMPRESS		1	/* поддержка вывода сжатых RLE изображений, пока что только для ARGB8888 видеобуфера */
			#define WITHFT8					1	/* Поддержка протокола FT8. Для фонового декодирования требуется минимум двухъядерный процессор и внешняя оперативная память */
			#define WITHLFM					1
			#define WITHNMEA				1
			#define WITHGNSS				1	/* В конфигурации трбуется разбирать поток NMEA от навигационного модуля */
//			#define WITHEXTRFBOARDTEST		1
			#define DMABUFCLUSTER			32
			#define WITHIQSHIFT				1
		#endif
	#elif LCDMODE_LQ043T3DX02K
		#define WITHFFTSIZEWIDE 512		/* Отображение спектра и волопада */
		#define WITHDISPLAYSWR_FPS 15
		#define WITHAFSPECTRE		1		/* показ спктра прослушиваемого НЧ сигнала. */
		#define WITHFFTSIZEAF 		512		/* Отображение спектра НЧ сигнвлв */
	#else
		#define WITHFFTSIZEWIDE 1024		/* Отображение спектра и волопада */
		#define WITHDISPLAYSWR_FPS 15
		#define WITHAFSPECTRE		1		/* показ спктра прослушиваемого НЧ сигнала. */
		#define WITHFFTSIZEAF 		512		/* Отображение спектра НЧ сигнвлв */
	#endif /* LCDMODE_AT070TNA2 || LCDMODE_AT070TN90 */

	//#define WITHVIEW_3DSS		1

	////*#define WITHRTS192 1		/* Получение от FPGA квадратур, возможно передача по USB и отображение спектра/водопада. */
	#define WITHFQMETER	1	/* есть схема измерения опорной частоты, по внешнему PPS */

	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия

	#define WITHNOTXDACCONTROL	1	/* в этой версии нет ЦАП управления смещением TXDAC передатчика */

	//#define WITHIFSHIFT	1	/* используется IF SHIFT */
	////#define WITHCAT		1	/* используется CAT */
	#define WITHTX	1
	#if WITHTX
		#define WITHVOX			1	/* используется VOX */
		#define WITHSHOWSWRPWR 1	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		#define WITHSWRMTR	1		/* Измеритель КСВ */
	#endif /* WITHTX */
	//#define WITHPWRMTR	1	/* Индикатор выходной мощности или */
	//#define WITHPWRLIN	1	/* Индикатор выходной мощности показывает напряжение а не мощность */
	#define WITHBARS		1	/* отображение S-метра и SWR-метра */
	//#define WITHSWLMODE	1	/* поддержка запоминания множества частот в swl-mode */
	#define WITHVIBROPLEX	1	/* возможность эмуляции передачи виброплексом */
	#define WITHSPKMUTE		1	/* управление выключением динамика */
	#define WITHDATAMODE	1	/* управление с клавиатуры передачей с USB AUDIO канала */

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */

	//#define WITHONLYBANDS 1		/* Перестройка может быть ограничена любительскими диапазонами */
	//#define WITHBCBANDS		1		/* в таблице диапазонов присутствуют вещательные диапазоны */
	#define WITHWARCBANDS	1	/* В таблице диапазонов присутствуют HF WARC диапазоны */
	
	////*#define WITHREFSENSOR	1		/* измерение по выделенному каналу АЦП опорного напряжения */
	#define WITHDIRECTBANDS 1	/* Прямой переход к диапазонам по нажатиям на клавиатуре */
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	#if 0
		#define WITHOPENVG	1		/* Использоывние OpenVG (khronos.org) - -fexceptions required */
		#define WITHUSEMALLOC	1	/* разрешение поддержки malloc/free/calloc/realloc */
		#define FORMATFROMLIBRARY 	1	/* поддержка печати плавающей точки */
	#endif
	#if 0
		#define WITHLWIP 1
		#define WITHUSEMALLOC	1	/* разрешение поддержки malloc/free/calloc/realloc */
		#define FORMATFROMLIBRARY 	1	/* поддержка печати плавающей точки */
	#endif
	//#define LO1PHASES	1		/* Прямой синтез первого гетеродина двумя DDS с програмимруемым сдвигом фазы */
	#define WITHFANTIMER	1	/* выключающийся по таймеру вентилятор в усилителе мощности */
	//#define WITHFANPWM		1	/* есть управление скоростью вентилятора */
	#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */

	#define WITHPOWERTRIM		1	// Имеется управление мощностью
	//#define WITHPABIASTRIM		1	// имеется управление током оконечного каскада усидителя мощности передатчика

	/* что за память настроек и частот используется в контроллере */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// Так же при использовании FM25040A - 5 вольт, 512 байт
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM
	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16		// demo board with atxmega128a4u
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A
	//#define NVRAM_TYPE NVRAM_TYPE_BKPSRAM	// Область памяти с батарейным питанием
	#define NVRAM_TYPE NVRAM_TYPE_NOTHING	// нет NVRAM
	#define HARDWARE_IGNORENONVRAM	1		// отладка на платах где нет никакого NVRAM

	#define RTC1_TYPE RTC_TYPE_DS1307

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */

	#define MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */
	/* Board hardware configuration */
	#define DDS1_TYPE DDS_TYPE_ZYNQ_PL
	//#define TSC1_TYPE TSC_TYPE_STMPE811	/* touch screen controller */
	#define TSC1_TYPE TSC_TYPE_GT911
	//#define TSC1_TYPE TSC_TYPE_ILI2102

	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */

	#define WITHCATEXT	1	/* Расширенный набор команд CAT */
	//#define WITHELKEY	1
	//#define WITHKBDENCODER 1	// перестройка частоты кнопками
	#define WITHKEYBOARD 1	/* в данном устройстве есть клавиатура */
	//#define KEYBOARD_USE_ADC	1	/* на одной линии установлено  четыре  клавиши. на vref - 6.8K, далее 2.2К, 4.7К и 13K. */

	// ST LM235Z
	#define THERMOSENSOR_UPPER		47	// 4.7 kOhm - верхний резистор делителя датчика температуры
	#define THERMOSENSOR_LOWER		10	// 1 kOhm - нижний резистор
	#define THERMOSENSOR_OFFSET 	(- 2730)		// 2.98 volt = 25 Celsius, 10 mV/C
	#define THERMOSENSOR_DENOM	 	10			// миливольты к десятым долям градуса 2.98 volt = 25 Celsius

#endif /* WITHISBOOTLOADER */

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

	#if WITHPOTIFGAIN
		POTIFGAIN = IFGAIN_IXI,
	#endif /* WITHPOTIFGAIN */
	#if WITHPOTAFGAIN
		POTAFGAIN = AFGAIN_IXI,
	#endif /* WITHPOTAFGAIN */

	#if WITHREFSENSOR
		VREFIX = 17,		// Reference voltage
	#endif /* WITHREFSENSOR */
	#if WITHTEMPSENSOR
		TEMPIX = 16,
	#endif /* WITHTEMPSENSOR */

	#if WITHPOTWPM
		POTWPM = 6,			// PA6 потенциометр управления скоростью передачи в телеграфе
	#endif /* WITHPOTWPM */
	#if WITHPOTPOWER
		POTPOWER = 6,			// регулировка мощности
	#endif /* WITHPOTPOWER */


	#if WITHAUTOTUNER_AVBELNN

		XTHERMOIX = BOARD_ADCX1IN(6),		// MCP3208 CH6 Exernal thermo sensor ST LM235Z

		#define WITHVOLTLEVEL	1	/* отображение напряжения питания */
		#define WITHCURRLEVEL	1	/* отображение тока оконечного каскада */

		#define WITHCURRLEVEL_ACS712_30A 1	// PA current sense - ACS712ELCTR-30B-T chip
		//#define WITHCURRLEVEL_ACS712_20A 1	// PA current sense - ACS712ELCTR-20B-T chip
		PASENSEIX = WPM_POTIX,		// PA1 PA current sense - ACS712-05 chip
		//PASENSEIX = 2,		// PA1 PA current sense - ACS712-05 chip

		#if WITHSWRMTR
			//FWD = BOARD_ADCXIN(2), REF = BOARD_ADCXIN(3),		// MCP3208 CH2, CH3 Детектор прямой, отраженной волны
			FWD = 14, REF = 15,	// PC4, PC5	SWR-meter
			PWRI = FWD,
		#endif /* WITHSWRMTR */

		VOLTSOURCE = BOARD_ADCX1IN(7),		// MCP3208 CH7 Средняя точка делителя напряжения, для АКБ

	#elif 0
		// UA1CEI PA board: MCP3208 at targetext2 - P2_0 external SPI device (PA BOARD ADC)
		VOLTSOURCE = BOARD_ADCX2IN(4),		// MCP3208 CH7 Средняя точка делителя напряжения, для АКБ

		FWD = BOARD_ADCX2IN(3),
		REF = BOARD_ADCX2IN(2),
		PWRI = FWD,

		#define WITHCURRLEVEL2	1	/* отображение тока оконечного каскада */
		PASENSEIX2 = BOARD_ADCX2IN(0),	// DRAIN
		PAREFERIX2 = BOARD_ADCX2IN(1),	// reference (1/2 питания ACS712ELCTR-30B-T).
	#else
		// толькло основная плата - 5W усилитель

		//#define WITHCURRLEVEL	1	/* отображение тока оконечного каскада */
		//#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
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
	#endif

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

	#define BOARDPOWERMIN    0    // Нижний предел регулировки (показываемый на дисплее)
	#define BOARDPOWERMAX    100    // Верхний предел регулировки (показываемый на дисплее)

#endif /* ARM_XC7ZXX_BGAXXX_CTLSTYLE_EBAZ4205_V2_H_INCLUDED */
