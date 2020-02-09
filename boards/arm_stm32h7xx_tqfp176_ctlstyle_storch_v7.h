/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Трансивер с DSP обработкой "Аист" на процессоре
// rmainunit_v5km0.pcb, rmainunit_v5km1.pcb STM32H743IIT6, TFT 4.3", 2xUSB, SD-CARD, NAU8822L и FPGA EP4CE22E22I7N

#ifndef ARM_STM32H7XX_TQFP176_CTLSTYLE_STORCH_V6_H_INCLUDED
#define ARM_STM32H7XX_TQFP176_CTLSTYLE_STORCH_V6_H_INCLUDED 1

	#if ! defined(STM32H743xx)
		#error Wrong CPU selected. STM32H743xx expected
	#endif /* ! defined(STM32F767xx) */

	//#define WITHSAICLOCKFROMI2S 1	/* Блок SAI1 тактируется от PLL I2S */
	#define WITHI2SCLOCKFROMPIN 1	// тактовая частота на SPI2 (I2S) подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса
	#define WITHSAICLOCKFROMPIN 1	// тактовая частота на SAI1 подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса

	#define WITHUSEPLL		1	/* Главная PLL	*/
	#define WITHUSEPLL3		1	/* PLL3 - для LTDC на STM32H743xx	*/
	//#define WITHUSESAIPLL	1	/* SAI PLL	*/
	//#define WITHUSESAII2S	1	/* I2S PLL	*/
	//#define LTDC_DOTCLK	9000000UL	// частота пикселей при работе с интерфейсом RGB

	#if 1
		// при наличии внешнего кварцевого резонатора
		#define WITHCPUXTAL 12000000uL	/* На процессоре установлен кварц 12.000 МГц */
		#define REF1_DIV 6			// ref freq = 2.0000 MHz
		#define REF3_DIV 6			// ref freq = 2.0000 MHz

		#if defined(STM32F767xx)
			// normal operation frequency
			#define REF1_MUL 216		// 2*216.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_6WS
		#elif CPUSTYLE_STM32F7XX
			// normal operation frequency
			#define REF1_MUL 216		// 2*216.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS
		#elif CPUSTYLE_STM32H7XX
			// normal operation frequency
			#define REF1_MUL 384		// 2*384.000 MHz (192 <= PLLN <= 432)
			#define REF3_MUL 135		// 2*135.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_2WS
		#endif

	#else

		#if defined(STM32F767xx)
			// тактирование от внутреннего RC генератора 16 МГц
			#define REF1_DIV 8			// ref freq = 2.000 MHz
			// normal operation frequency
			#define REF1_MUL 216		// 2*216.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_6WS
		#elif CPUSTYLE_STM32F7XX
			// тактирование от внутреннего RC генератора 16 МГц
			#define REF1_DIV 8			// ref freq = 2.000 MHz
			// normal operation frequency
			#define REF1_MUL 216		// 2*216.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS
		#elif CPUSTYLE_STM32H7XX
			// тактирование от внутреннего RC генератора 64 МГц
			#define REF1_DIV 32			// ref freq = 2.000 MHz
			#define REF3_DIV 32			// ref freq = 2.000 MHz
			// normal operation frequency
			#define REF1_MUL 384		// 2*384.000 MHz (192 <= PLLN <= 432)
			#define REF3_MUL 135		// 2*135.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_2WS
		#endif

	#endif

	#if WITHI2SCLOCKFROMPIN
		#define FPGADECIMATION 2560
		#define FPGADIVIDERATIO 5
		#define EXTI2S_FREQ (REFERENCE_FREQ * DDS1_CLK_MUL / FPGADIVIDERATIO)
		#define EXTSAI_FREQ (REFERENCE_FREQ * DDS1_CLK_MUL / FPGADIVIDERATIO)

		#define ARMI2SMCLK	(REFERENCE_FREQ * DDS1_CLK_MUL / (FPGADECIMATION / 256))
		#define ARMSAIMCLK	(REFERENCE_FREQ * DDS1_CLK_MUL / (FPGADECIMATION / 256))
	#else /* WITHI2SCLOCKFROMPIN */
		#define PLLI2SN_MUL 172		// 344.064 (192 <= PLLI2SN <= 432)
		#define SAIREF1_MUL 172		// 245.76 / 1.024 = 240 (49 <= PLLSAIN <= 432)
		// Частота формируется процессором
		#define ARMI2SMCLK	12288000 //(PLLSAI_FREQ_OUT / 14)
		#define ARMSAIMCLK	12288000 //(PLLSAI_FREQ_OUT / 14)
	#endif /* WITHI2SCLOCKFROMPIN */

	/* модели синтезаторов - схемы частотообразования */

	/* Версии частотных схем - с преобразованием "наверх" */
	//#define FQMODEL_45_IF8868_UHF430	1	// SW2011
	//#define FQMODEL_73050		1	// 1-st IF=73.050, 2-nd IF=0.455 MHz
	//#define FQMODEL_73050_IF0P5		1	// 1-st IF=73.050, 2-nd IF=0.5 MHz
	//#define FQMODEL_80455		1	// 1-st IF=80.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455_IF0P5		1	// 1-st IF=64.455, 2-nd IF=0.5 MHz
	//#define FQMODEL_70455		1	// 1-st IF=70.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	//#define FQMODEL_60700_IF05	1	// 60.7 -> 10.7 -> 0.5
	//#define FQMODEL_60725_IF05	1	// 60.725 -> 10.725 -> 0.5
	//#define FQMODEL_60700_IF02	1	// 60.7 -> 10.7 -> 0.2
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz

	// +++ вариации прошивки, специфические для разных частот опорного генератора
	#if 0
		//#define DIRECT_50M0_X1		1	/* Тактовый генератор на плате 50.0 МГц */
		#define DIRECT_80M0_X1		1	/* Тактовый генератор на плате 80.0 МГц */
		//#define DIRECT_72M595_X1	1	/* Тактовый генератор 75.595 МГц */
		#define BANDSELSTYLERE_UPCONV56M_36M	1	/* Up-conversion with working band .030..36 MHz */
	#elif 0
		#define DIRECT_100M0_X1		1	/* Тактовый генератор на плате 100.0 МГц */
		#define BANDSELSTYLERE_UPCONV56M_45M	1	/* Up-conversion with working band .030..45 MHz */
	#else
		//#define DIRECT_125M0_X1		1	/* Тактовый генератор на плате 125.0 МГц */
		#define DIRECT_122M88_X1	1	/* Тактовый генератор 122.880 МГц */
		#define BANDSELSTYLERE_UPCONV56M	1	/* Up-conversion with working band .030..56 MHz */
	#endif
	#define FQMODEL_FPGA		1	// FPGA + IQ over I2S

	// --- вариации прошивки, специфические для разных частот

	#define CTLREGMODE_STORCH_V7	1	/* TFT 4.3" "Аист" с DSP и FPGA STM32H743IIT6 или R7S721020VCFP */

	#define WITHPOWERTRIM		1	// Имеется управление мощностью
	#define WITHPOWERTRIMMIN	5	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHPOWERTRIMMAX	100	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHPOWERTRIMATU	50	// Значение для работы автотюнера

	//#define WITHPABIASTRIM		1	// имеется управление током оконечного каскада усидителя мощности передатчика
	#define	WITHPABIASMIN		0
	#define	WITHPABIASMAX		255

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_SSB 	0		// Заглушка

	// +++ заглушки для плат с DSP обработкой
	#define	BOARD_AGCCODE_ON	0x00
	#define	BOARD_AGCCODE_OFF	0x01

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define BOARD_FILTER_0P5		1	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_3P1		0	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_6P0		0	/* 6.0 kHz filter */
	#define BOARD_FILTER_8P0		0	/* 6.0 kHz filter */
	// --- заглушки для плат с DSP обработкой

	#define WITHPREAMPATT2_6DB 1	// LTC2208 Управление УВЧ и двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB */
	//#define WITHATT2_6DB	1		// LTC2217 Управление двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB без УВЧ
	#define DEFPREAMPSTATE 	0	/* УВЧ по умолчанию включён (1) или выключен (0) */

	#define WITHAGCMODEONOFF	1	// АРУ вкл/выкл
	#define	WITHMIC1LEVEL		1	// установка усиления микрофона

	//#define DSTYLE_UR3LMZMOD	1	// Расположение элементов экрана в трансиверах UR3LMZ
	#define	FONTSTYLE_ITALIC	1	// Использовать альтернативный шрифт
	//#define COLORSTYLE_RED	1	// Цвета а-ля FT-1000

	// +++ Особые варианты расположения кнопок на клавиатуре
	//#define KEYB_RAVEN20_V5	1		/* 5 линий клавиатуры: расположение кнопок для Воробей с DSP обработкой */
	//#define KEYB_FPANEL20_V0A	1	/* 20 кнопок на 5 линий - плата rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении с новым расположением */
	#define KEYB_FPANEL20_V0A_PLAYFILE 1
	//#define KEYB_FPANEL20_V0A_RA1AGO	1	/* перевернутый */
	//#define KEYB_FPANEL30_V3	1	/* KEYBOARD_USE_ADC6_V1, 30 кнопок на 5 линий - плата rfrontpanel_v3 + LCDMODE_S1D13781 & LCDMODE_LQ043T3DX02K в нормальном расположении */
	//#define KEYBOARD_USE_ADC6_V1	1

	// --- Особые варианты расположения кнопок на клавиатуре
	#define WITHSPLIT	1	/* управление режимами расстройки одной кнопкой */
	//#define WITHSPLITEX	1	/* Трехкнопочное управление режимами расстройки */

	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
	//#define LCDMODE_HARD_SPI	1	/* LCD over SPI line */
	//#define LCDMODE_DUMMY	1
	#define LCDMODE_LTDC	1		/* Use STM32F4xxx with LCD-TFT Controller (LTDC), also need LCDMODE_ILI9341 */
	#define LCDMODE_LTDC_L8		1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит - RGB565. */
	#define LCDMODE_LTDC_PIP16	1	/* используется PIP с форматом 16 бит - RGB565 */
	//#define LCDMODE_WH2002	1	/* тип применяемого индикатора 20*2, возможно вместе с LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* тип применяемого индикатора 16*2 */
	//#define LCDMODE_WH1604	1	/* тип применяемого индикатора 16*4 */
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
	//#define LCDMODE_S1D13781_NHWACCEL 1	/* Неиспользоване аппаратных особенностей EPSON S1D13781 при выводе графики */
	//#define LCDMODE_S1D13781	1	/* Инндикатор 480*272 с контроллером Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - перевернуть изображение */
	//#define LCDMODE_S1D13781_REFOSC_MHZ	50	/* Частота генератора, установленного на контроллере дисплея */
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
	#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	//#define LCDMODE_AT070TN90 1	/* AT070TN90 panel (800*480) - 7" display */
	// --- Одна из этих строк определяет тип дисплея, для которого компилируется прошивка

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define	WITHENCODER	1	/* для изменения частоты имеется енкодер */
	//#define ENCODER_REVERSE	1	/* разводка на плате с перепутаными фазами от валкодера */
	//#define ENCODER2_REVERSE	1	/* разводка на плате с перепутаными фазами от валкодера */
	#define WITHENCODER2	1		/* есть второй валкодер */
	//#define WITHPWBUTTON	1	/* Наличие схемы электронного включения питания */
	/* Board hardware configuration */
	//#define CODEC1_TYPE CODEC_TYPE_TLV320AIC23B
	//#define CODEC_TYPE_TLV320AIC23B_USE_SPI	1
	//#define CODEC_TYPE_TLV320AIC23B_USE_8KS	1	/* кодек работает с sample rate 8 kHz */

	//#define CODEC_TYPE_WM8731_USE_SPI	1
	//#define CODEC_TYPE_WM8731_USE_8KS	1	/* кодек работает с sample rate 8 kHz */

	#define CODEC1_TYPE CODEC_TYPE_NAU8822L
	#define CODEC_TYPE_NAU8822_USE_SPI	1
	//#define CODEC_TYPE_NAU8822_USE_8KS	1	/* кодек работает с sample rate 8 kHz */

	//#define WITHDTMFPROCESSING 1
	//#define WITHBBOXMIKESRC BOARD_TXAUDIO_LINE

	#define CODEC2_TYPE	CODEC_TYPE_FPGAV1	/* квадратуры получаем от FPGA */
	//#define CODEC_TYPE_CS4272_USE_SPI	1		// codecboard v2.0
	//#define CODEC_TYPE_CS4272_STANDALONE	1		// codecboard v3.0

	#define WITHSAI1_FRAMEBITS 256	// Полный размер фрейма
	//#define WITHSAI2_FRAMEBITS 64	// Полный размер фрейма для двух квадратур по 24 бита - канал спектроанализатора
	#define WITHSAI1_FORMATI2S_PHILIPS 1	// требуется при получении данных от FPGA
	//#define WITHSAI2_FORMATI2S_PHILIPS 1	// требуется при получении данных от FPGA
	#define WITHI2S_FORMATI2S_PHILIPS 1	// Возможно использование при передаче данных в кодек, подключенный к наушникам и микрофону
	#define WITHI2SHWRXSLAVE	1		// Приёмный канал I2S (микрофон) используюся в SLAVE MODE
	#define WITHI2SHWTXSLAVE	1		// Передающий канал I2S (наушники) используюся в SLAVE MODE
	//#define WITHSAI1HWTXRXMASTER	1		// SAI1 work in MASTER mode
	#define WITHNESTEDINTERRUPTS	1	/* используется при наличии real-time части. */
	#define WITHINTEGRATEDDSP		1	/* в программу включена инициализация и запуск DSP части. */
	#define WITHIFDACWIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHIFADCWIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHAFADCWIDTH	16		// 1 бит знак и 15 бит значащих
	#define WITHAFDACWIDTH	16		// 1 бит знак и 15 бит значащих
	//#define WITHDACOUTDSPAGC		1	/* АРУ реализовано как выход ЦАП на аналоговую часть. */
	//#define WITHEXTERNALDDSP		1	/* имеется управление внешней DSP платой. */
	#define WITHDSPEXTDDC 1			/* Квадратуры получаются внешней аппаратурой */
	#define WITHDSPEXTFIR 1			/* Фильтрация квадратур осуществляется внешней аппаратурой */
	//#define WITHDSPLOCALFIR 1		/* test: Фильтрация квадратур осуществляется процессором */
	#define WITHIF4DSP	1			/*  "Дятел" */
	#define WITHDACSTRAIGHT 1		/* Требуется формирование кода для ЦАП в режиме беззнакового кода */

	// FPGA section
	//#define	WITHFPGAWAIT_AS	1	/* FPGA загружается из собственной микросхемы загрузчика - дождаться окончания загрузки перед инициализацией SPI в процессоре */
	#define	WITHFPGALOAD_PS	1	/* FPGA загружается процессором с помощью SPI */

	//#define WITHSKIPUSERMODE 1	// debug option: не отдавать в USER MODE блоки для фильтрации аудиосигнала
	#define BOARD_FFTZOOM_POW2MAX 2	// Возможные масштабы FFT x1, x2, x4, x8
	//#define WITHNOSPEEX	1	// Без шумоподавителя SPEEX
	//#define WITHUSEDUALWATCH	1	// Второй приемник
	//#define WITHLOOPBACKTEST	1	/* прослушивание микрофонного входа, генераторов */
	//#define WITHMODEMIQLOOPBACK	1	/* модем получает собственные передаваемые квадратуры */
	//#define WITHFASTWATERFLOW	1	/* быстрое отображение водопада (но требует больше памяти) */

	// выбор накопителя
	//#define WITHUSESDCARD		1	// Включение поддержки SD CARD
	#define WITHUSEUSBFLASH		1	// Включение поддержки USB memory stick

	// выбор функциональности
	#define WITHUSEAUDIOREC		1	// Запись звука
	//#define WITHUSEAUDIOREC2CH	1	// Запись звука в стерео формате
	//#define WITHUSEAUDIORECCLASSIC	1	// стандартный формат записи, без "дыр"

	#define WITHRTS96 1		/* Получение от FPGA квадратур, возможно передача по USB и отображение спектра/водопада. */
	#define WITHFQMETER	1	/* есть схема измерения опорной частоты, по внешнему PPS */
	//#define WITHOPERA4BEACON	1	/* работа маяком в OPERA */

	#if 0
		#define WITHWAVPLAYER 1	/* трансивер работает проигрывателем файлов с USB/SD накопителя */
		//#define WITHBBOX	1	// Black Box mode - устройство без органов управления
		//#define	WITHBBOXMIKESRC	BOARD_TXAUDIO_USB
	#elif 0
		#define WITHUSBHEADSET 1	/* трансивер работает USB гарнитурой для компьютера - режим тестирования */
		#define WITHBBOX	1	// Black Box mode - устройство без органов управления
		#define	WITHBBOXMIKESRC	BOARD_TXAUDIO_USB
	#elif 0
		#define WITHBBOX	1	// Black Box mode - устройство без органов управления
		#define	WITHBBOXFREQ	26985000L		// частота после включения
		//#define	WITHBBOXFREQ	(26985000L - 260)		// частота после включения - 135 коррекция частоты платы с  122.88 для попадания в приём платы с 100 МГц генератором без коррекции
		//#define	WITHBBOXFREQ	(26985000L - 1600)		// частота после включения
		//#define	WITHBBOXFREQ	(14070000L - 1000)		// прослушивание BPSK частот
		//#define	WITHBBOXFREQ	(14065000L - 135)		// частота после включения - 135 коррекция частоты платы с  122.88 для попадания в приём платы с 100 МГц генератором без коррекции
		//#define	WITHBBOXFREQ	14065000L		// частота после включения
		//#define	WITHBBOXFREQ	(14065000L - 1000)		// частота после включения
		//#define	WITHBBOXSUBMODE	SUBMODE_USB	// единственный режим работы
		#define	WITHBBOXSUBMODE	SUBMODE_BPSK	// единственный режим работы
		//#define	WITHBBOXFREQ	27100000L		// частота после включения
		//#define	WITHBBOXSUBMODE	SUBMODE_CW	// единственный режим работы
		//#define	WITHBBOXTX		1		// автоматический переход на передачу
		//#define	WITHBBOXMIKESRC	BOARD_TXAUDIO_2TONE
	#elif 0
		#define WITHBBOX	1	// Black Box mode - устройство без органов управления
		#define	WITHBBOXFREQ	136000L		// частота после включения
		#define	WITHBBOXSUBMODE	SUBMODE_USB	// единственный режим работы
		#define WITHBBOXREC	1		// автоматическое включение режима записи после подачи питания
	#elif 0
		#define WITHBBOX	1	// Black Box mode - устройство без органов управления
		#define	WITHBBOXFREQ	7030000L		// частота после включения
		#define	WITHBBOXSUBMODE	SUBMODE_LSB	// единственный режим работы
		#define	WITHBBOXTX		1		// автоматический переход на передачу
		#define	WITHBBOXMIKESRC	BOARD_TXAUDIO_2TONE
	#elif 0
		#define WITHBBOX	1	// Black Box mode - устройство без органов управления
		#define	WITHBBOXFREQ	7030000L		// частота после включения
		#define	WITHBBOXSUBMODE	SUBMODE_CWZ	// единственный режим работы
		#define	WITHBBOXTX		1		// автоматический переход на передачу
		//#define	WITHBBOXMIKESRC BOARD_TXAUDIO_2TONE
	#endif


	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	//#define WITHRFSG	1	/* включено управление ВЧ сигнал-генератором. */
	#define WITHTX		1	/* включено управление передатчиком - сиквенсор, электронный ключ. */
	#if 0
		/* TUNER & PA board 2*RD16 by avbelnn@yandex.ru */
		#define WITHAUTOTUNER	1	/* Есть функция автотюнера */
		#define SHORTSET8	1	
	#endif

	#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
	//#define WITHTHERMOLEVEL	1	/* отображение температуры */

	#define WITHIFSHIFT	1	/* используется IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* Начальное занчение IF SHIFT */
	//#define WITHPBT		1	/* используется PBT (если LO3 есть) */
	#define WITHCAT		1	/* используется CAT */
	//#define WITHDEBUG		1	/* Отладочная печать через COM-порт. Без CAT (WITHCAT) */
	//#define WITHMODEM		1	/* Устройство работает как радиомодем с последовательным интерфейсом */
	//#define WITHFREEDV	1	/* поддержка режима FreeDV - http://freedv.org/ */
	//#define WITHNMEA		1	/* используется NMEA parser */
	//#define WITHBEACON	1	/* Используется режим маяка */
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
	// Есть ли регулировка параметров потенциометрами
	////#define WITHPOTWPM		1	/* используется регулировка скорости передачи в телеграфе потенциометром */
	#define WITHPOTIFGAIN		1	/* регуляторы усиления ПЧ на потенциометрах */
	#define WITHPOTAFGAIN		1	/* регуляторы усиления НЧ на потенциометрах */
	//#define WITHPOTPOWER	1	/* регулятор мощности на потенциометре */
	//#define WITHANTSELECT	1	// Управление переключением антенн

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */

	//#define WITHONLYBANDS 1		/* Перестройка может быть ограничена любительскими диапазонами */
	//#define WITHBCBANDS		1		/* в таблице диапазонов присутствуют вещательные диапазоны */
	#define WITHWARCBANDS	1	/* В таблице диапазонов присутствуют HF WARC диапазоны */
	//#define WITHLO1LEVELADJ		1	/* включено управление уровнем (амплитудой) LO1 */
	//#define WITHLFM		1	/* LFM MODE */
	//#define WITHTEMPSENSOR	1	/* отображение данных с датчика температуры */
	#define WITHREFSENSOR	1		/* измерение по выделенному каналу АЦП опорного напряжения */
	#define WITHDIRECTBANDS 1	/* Прямой переход к диапазонам по нажатиям на клавиатуре */
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	//#define LO1PHASES	1		/* Прямой синтез первого гетеродина двумя DDS с програмимруемым сдвигом фазы */
	#define WITHFANTIMER	1	/* выключающийся по таймеру вентилятор в усилителе мощности */
	#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */
	#define WITHSENDWAV 1	/* трансивер может передавать записанные wav файлы */

	/* что за память настроек и частот используется в контроллере */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// Так же при использовании FM25040A - 5 вольт, 512 байт
	#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16		// demo board with atxmega128a4u
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A
	//#define NVRAM_TYPE NVRAM_TYPE_BKPSRAM	// Область памяти с батарейным питанием
	//#define HARDWARE_IGNORENONVRAM	1		// отладка на платах где нет никакого NVRAM

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */

	#define MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */
	/* Board hardware configuration */
	#define DDS1_TYPE DDS_TYPE_FPGAV1
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define PLL1_TYPE PLL_TYPE_ADF4001
	//#define DDS2_TYPE DDS_TYPE_AD9834
	//#define RTC1_TYPE RTC_TYPE_M41T81	/* ST M41T81M6 RTC clock chip with I2C interface */
	#define RTC1_TYPE RTC_TYPE_STM32F4xx	/* STM32F4xx/STM32F7xx internal RTC peripherial */
	//#define TSC1_TYPE TSC_TYPE_STMPE811	/* touch screen controller */
	//#define DAC1_TYPE	99999		/* наличие ЦАП для подстройки тактовой частоты */

	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */

	#define WITHMODESETFULLNFM 1

	//#define WITHWFM	1			/* используется WFM */
	/* все возможные в данной конфигурации фильтры */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1 /* | IF3_FMASK_6P0 | IF3_FMASK_8P0*/)
	/* все возможные в данной конфигурации фильтры для передачи */
	#define IF3_FMASKTX	(IF3_FMASK_3P1 /*| IF3_FMASK_6P0 */)
	/* фильтры, для которых стоит признак HAVE */
	#define IF3_FHAVE	( IF3_FMASK_0P5 | IF3_FMASK_3P1 /*| IF3_FMASK_6P0 | IF3_FMASK_8P0*/)

	#define WITHCATEXT	1	/* Расширенный набор команд CAT */
	#define WITHELKEY	1
	#define WITHKBDENCODER 1	// перестройка частоты кнопками
	#define WITHKEYBOARD 1	/* в данном устройстве есть клавиатура */
	#define KEYBOARD_USE_ADC	1	/* на одной линии установлено  четыре  клавиши. на vref - 6.8K, далее 2.2К, 4.7К и 13K. */

	#define VOLTLEVEL_UPPER		47	// 4.7 kOhm - верхний резистор делителя датчика напряжения
	#define VOLTLEVEL_LOWER		10	// 1 kOhm - нижний резистор

	#define THERMOSENSOR_UPPER		47	// 4.7 kOhm - верхний резистор делителя датчика температуры
	#define THERMOSENSOR_LOWER		10	// 1 kOhm - нижний резистор

	#define WITHDCDCFREQCTL	1		// Имеется управление частотой преобразователей блока питания и/или подсветки дисплея

	// Назначения входов АЦП процессора.
	enum 
	{ 
	#if WITHREFSENSOR
		VREFIX = 17,		// Reference voltage
	#endif /* WITHREFSENSOR */
	#if WITHTEMPSENSOR
		TEMPIX = 16,
	#endif /* WITHTEMPSENSOR */

	#if WITHPOTIFGAIN
		POTIFGAIN = 3,		// PA2 IF GAIN
	#endif /* WITHPOTIFGAIN */
	#if WITHPOTAFGAIN
		POTAFGAIN = 7,		// PA7 AF GAIN
	#endif /* WITHPOTAFGAIN */

	#if WITHPOTWPM
		POTWPM = 6,			// PA6 потенциометр управления скоростью передачи в телеграфе
	#endif /* WITHPOTWPM */
	#if WITHPOTPOWER
		POTPOWER = 6,			// регулировка мощности
	#endif /* WITHPOTPOWER */

	#if WITHTHERMOLEVEL
		XTHERMOIX = 9,		// PB1 Exernal thermo sensor ST LM235Z
	#endif /* WITHTHERMOLEVEL */

	#if 0
		// UA1CEI PA board: MCP3208 at targetext2 - P2_0 external SPI device (PA BOARD ADC)
		VOLTSOURCE = BOARD_ADCX2IN(4),		// MCP3208 CH7 Средняя точка делителя напряжения, для АКБ

		FWD = BOARD_ADCX2IN(3),
		REF = BOARD_ADCX2IN(2),
		PWRI = FWD,

		#define WITHCURRLEVEL2	1	/* отображение тока оконечного каскада */
		PASENSEIX2 = BOARD_ADCX2IN(0),	// DRAIN
		PAREFERIX2 = BOARD_ADCX2IN(1),	// reference (1/2 питания ACS712ELCTR-30B-T).

		#if WITHTHERMOLEVEL
			XTHERMOIX = BOARD_ADCXIN(6),		// MCP3208 CH6 Exernal thermo sensor ST LM235Z
		#endif /* WITHTHERMOLEVEL */
	#else
		// толькло основная плата - 5W усилитель

		#define WITHCURRLEVEL	1	/* отображение тока оконечного каскада */

		#if WITHCURRLEVEL
			//PASENSEIX = BOARD_ADCXIN(0),		// MCP3208 CH0 PA current sense - ACS712-30 chip
			PASENSEIX = 2,		// PA2 PA current sense - ACS712-05 chip
		#endif /* WITHCURRLEVEL */
		#if WITHVOLTLEVEL
			VOLTSOURCE = 8,		// PB0 Средняя точка делителя напряжения, для АКБ
		#endif /* WITHVOLTLEVEL */

		#if WITHSWRMTR
			//FWD = BOARD_ADCXIN(2), REF = BOARD_ADCXIN(3),		// MCP3208 CH2, CH3 Детектор прямой, отраженной волны
			FWD = 14, REF = 15,	// PC5	SWR-meter
			PWRI = FWD,			// PC4
		#endif /* WITHSWRMTR */
	#endif
		KI0 = 10, KI1 = 11, KI2 = 12, KI3 = 0, KI4 = 1	// клавиатура
	};

	#define KI_COUNT 5	// количество используемых под клавиатуру входов АЦП
	#define KI_LIST	KI4, KI3, KI2, KI1, KI0,	// инициализаторы для функции перекодировки

#endif /* ARM_STM32H7XX_TQFP176_CTLSTYLE_STORCH_V6_H_INCLUDED */
