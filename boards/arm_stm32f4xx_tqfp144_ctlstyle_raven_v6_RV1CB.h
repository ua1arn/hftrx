/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Трансивер с DSP обработкой "Воронёнок-2" на процессоре
// STM32F446ZET6, STM32F746ZGT6 с кодеком NAU8822L и FPGA EP4CE22E22I7N
// с функцией DUAL WATCH
// Rmainunit_v4bm.pcb

// Custom configuration for Oleg Ignatiev, RV1CB

#ifndef ARM_STM32F4XX_TQFP144_CTLSTYLE_RAVEN_V6_RV1CB_H_INCLUDED
#define ARM_STM32F4XX_TQFP144_CTLSTYLE_RAVEN_V6_RV1CB_H_INCLUDED 1

	#if ! defined(STM32F746xx)
		#error Wrong CPU selected. STM32F746xx expected
	#endif /* ! defined(STM32F446xx) */

	//#define WITHSAICLOCKFROMI2S 1	/* Блок SAI1 тактируется от PLL I2S */
	#define WITHI2SCLOCKFROMPIN 1	// тактовая частота на SPI2 (I2S) подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса
	#define WITHSAICLOCKFROMPIN 1	// тактовая частота на SAI1 подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса

	#define WITHUSEPLL		1	/* Главная PLL	*/
	//#define WITHUSESAIPLL	1	/* SAI PLL	*/
	//#define WITHUSESAII2S	1	/* I2S PLL	*/

	#if 1
		// при наличии внешнего кварцевого резонатора
		#define WITHCPUXTAL 16000000uL	/* На процессоре установлен кварц 16.000 МГц */
		#define REF1_DIV 8			// ref freq = 2.0000 MHz

		#if defined(STM32F767xx)
			// normal operation frequency
			#define REF1_MUL 216		// 2*216.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_6WS	// overvlocking
		#elif CPUSTYLE_STM32F7XX
			// normal operation frequency
			#define REF1_MUL 216		// 2*216.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS	// overvlocking
		#elif CPUSTYLE_STM32F4XX
			// normal operation frequency
			#define REF1_MUL 192		// 2*192.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_5WS	// Задержка для работы с памятью 5 WS for 168 MHz at 3.3 volt
		#endif

	#elif 0
		// при наличии внешнего кварцевого резонатора
		#define WITHCPUXTAL 18432000uL	/* На процессоре установлен кварц 18.432 МГц */
		#define REF1_DIV 18			// ref freq = 1.024 MHz

		#if CPUSTYLE_STM32F7XX
			// normal operation frequency
			#define REF1_MUL 421		// 2*xxx.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS	// overvlocking
		#elif CPUSTYLE_STM32F4XX
			// normal operation frequency
			#define REF1_MUL 351		// 2*179.712 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_5WS	// Задержка для работы с памятью 5 WS for 168 MHz at 3.3 volt
		#endif

	#else
		// тактирование от внутреннего RC генератора 16 МГц
		#define REF1_DIV 8			// ref freq = 2.000 MHz

		#if defined(STM32F767xx)
			// normal operation frequency
			#define REF1_MUL 216		// 2*216.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_6WS	// overvlocking
		#elif CPUSTYLE_STM32F7XX
			// normal operation frequency
			#define REF1_MUL 216		// 2*216.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS	// overvlocking
		#elif CPUSTYLE_STM32F4XX
			// normal operation frequency
			#define REF1_MUL 192		// 2*192.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_5WS	// Задержка для работы с памятью 5 WS for 168 MHz at 3.3 volt
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
		#define PLLI2SN_MUL 336		// 344.064 (192 <= PLLI2SN <= 432)
		#define SAIREF1_MUL 240		// 245.76 / 1.024 = 240 (49 <= PLLSAIN <= 432)
		// Частота формируется процессором
		#define ARMI2SMCLK	(12288000UL)
		#define ARMSAIMCLK	(12288000UL)
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

	#define FQMODEL_FPGA		1	// FPGA + IQ over I2S

	#if 0
		#define DIRECT_80M0_X1		1	/* Тактовый генератор на плате 80.0 МГц */
		#define BANDSELSTYLERE_UPCONV56M_36M	1	/* Up-conversion with working band .030..36 MHz */
	#elif 0
		#define DIRECT_100M0_X1		1	/* Тактовый генератор на плате 100.0 МГц */
		#define BANDSELSTYLERE_UPCONV56M_45M	1	/* Up-conversion with working band .030..45 MHz */
	#else
		//#define DIRECT_125M0_X1		1	/* Тактовый генератор на плате 125.0 МГц */
		#define DIRECT_122M88_X1	1	/* Тактовый генератор 122.880 МГц */
		#define BANDSELSTYLERE_UPCONV56M	1	/* Up-conversion with working band .030..56 MHz */
	#endif
	// --- вариации прошивки, специфические для разных плат

	#define CTLREGMODE_RAVENDSP_V6	1	/* "Воронёнок" с DSP и FPGA, SD-CARD, dual watch & PA on board */

	#define WITHPOWERTRIM		1	// Имеется управление мощностью
	#define WITHPOWERTRIMMIN	5	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHPOWERTRIMMAX	100	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHPOWERTRIMATU	15	// Значение для работы автотюнера

	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея
	#define WITHLCDBACKLIGHTMIN	0	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры

	//#define WITHPABIASTRIM		1	// имеется управление током оконечного каскада усидителя мощности передатчика
	#define WITHPABIASMIN		0
	#define WITHPABIASMAX		255

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

	#define WITHAGCMODEONOFF	1	// АРУ вкл/выкл
	#define WITHMIC1LEVEL		1	// установка усиления микрофона

	#define DSTYLE_UR3LMZMOD	1	// Расположение элементов экрана в трансиверах UR3LMZ
	#define	FONTSTYLE_ITALIC	1	// Использовать альтернативный шрифт

	// +++ Особые варианты расположения кнопок на клавиатуре
	//#define KEYB_RAVEN20_V5	1		/* 5 линий клавиатуры: расположение кнопок для Воробей с DSP обработкой */
	//#define KEYB_VERTICAL_REV	1	/* расположение кнопок для плат "Воробей" и "Колибри" */
	//#define KEYB_VERTICAL_REV_TOPDOWN	1	/* расположение кнопок для ПЕРЕВЁРНУТЫХ плат "Воробей" и "Колибри" */
	//#define KEYB_VERTICAL	1	/* расположение кнопок для плат "Павлин" */
	//#define KEYB_V8S_DK1VS	1	/* расположение для контроллера DK1VS */
	//#define KEYB_VERTICAL_REV_RU6BK	1	/* расположение кнопок для плат "Воробей" и "Колибри" */
	//#define KEYBOARD_USE_ADC6	1	/* шесть кнопок на каждом входе ADCx */
	//#define KEYB_M0SERG	1	/* расположение кнопок для Serge Moisseyev */
	#define KEYB_FPANEL20_V0A	1	/* 20 кнопок на 5 линий - плата rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении с новым расположением */
	// --- Особые варианты расположения кнопок на клавиатуре
	#define WITHSPLIT	1	/* управление режимами расстройки одной кнопкой */
	//#define WITHSPLITEX	1	/* Трехкнопочное управление режимами расстройки */

	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
	//#define LCDMODE_HARD_SPI	1	/* LCD over SPI line */
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
	#define LCDMODE_S1D13781	1	/* Инндикатор 480*272 с контроллером Epson S1D13781 */
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
	//#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	// --- Одна из этих строк определяет тип дисплея, для которого компилируется прошивка

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	#define WITHENCODER	1	/* для изменения частоты имеется енкодер */

	/* Board hardware configuration */
	//#define	CODEC1_TYPE	CODEC_TYPE_TLV320AIC23B
	#define CODEC1_TYPE CODEC_TYPE_NAU8822L
	#define CODEC2_TYPE	CODEC_TYPE_FPGAV1	/* квадратуры получаем от FPGA */

	//#define CODEC_TYPE_CS4272_USE_SPI	1		// codecboard v2.0
	//#define CODEC_TYPE_CS4272_STANDALONE	1		// codecboard v3.0
	#define CODEC_TYPE_TLV320AIC23B_USE_SPI	1	// codecboard v2.0
	#define CODEC_TYPE_NAU8822_USE_SPI	1
	//#define CODEC_TYPE_NAU8822_USE_8KS	1	/* кодек работает с sample rate 8 kHz */

	#define WITHSAI1_FRAMEBITS 256	// Полный размер фрейма
	//#define WITHSAI2_FRAMEBITS 64	// Полный размер фрейма для двух квадратур по 24 бита - канал спектроанализатора
	#define WITHSAI1_FORMATI2S_PHILIPS 1	// требуется при получении данных от FPGA
	//#define WITHSAI2_FORMATI2S_PHILIPS 1	// требуется при получении данных от FPGA
	#define WITHI2S_FORMATI2S_PHILIPS 1	// Возможно использование при передаче данных в кодек, подключенный к наушникам и микрофону
	#define WITHI2S_FRAMEBITS 32	// Полный размер фрейма для двух каналов - канал кодека
	#define WITHI2SHWRXSLAVE	1		// Приёмный канал I2S (микрофон) используюся в SLAVE MODE
	#define WITHI2SHWTXSLAVE	1		// Передающий канал I2S (наушники) используюся в SLAVE MODE
	#define WITHNESTEDINTERRUPTS	1	/* используется при наличии real-time части. */
	#define WITHINTEGRATEDDSP		1	/* в программу включена инициализация и запуск DSP части. */
	#define WITHIFDACWIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHIFADCWIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHAFADCWIDTH	16		// 1 бит знак и 15 бит значащих
	#define WITHAFDACWIDTH	16		// 1 бит знак и 15 бит значащих
	//#define WITHDACOUTDSPAGC		1	/* АРУ реализовано как выход ЦАП на аналоговую часть. */
	//#define WITHEXTERNALDDSP		1	/* имеется управление внешней DSP платой. */
	//#define WITHLOOPBACKTEST	1	/* прослушивание микрофонного входа, генераторов */
	#define WITHDSPEXTDDC 1			/* Квадратуры получаются внешней аппаратурой */
	#define WITHDSPEXTFIR 1			/* Фильтрация квадратур осуществляется внешней аппаратурой */
	#define WITHIF4DSP	1	// "Дятел"
	//#define WITHUSESDCARD		1	// Включение поддержки SD CARD
	//#define WITHUSEAUDIOREC		1	// Запись звука на SD CARD
	//#define WITHUSEDUALWATCH	1	// Второй приемник
	//#define WITHREVERB	1	// ревербератор в обработке микрофонного сигнала
	#define WITHRTS96 1		/* Получение от FPGA квадратур, возможно передача по USB и отображение спектра/водопада. */
	#define BOARD_FFTZOOM_POW2MAX 1	/* x1, x2 */
	#define KEYB_FPANEL20_V0A_AVB	1	/* перевернутый */
	//#define WITHFQMETER	1	/* есть схема измерения опорной частоты, по внешнему PPS */

	#if 0
		#define WITHBBOX	1	// Black Box mode - устройство без органов управления
		#define WITHBBOXFREQ	26985000L		// частота после включения
		//#define WITHBBOXFREQ	(26985000L - 260)		// частота после включения - 135 коррекция частоты платы с  122.88 для попадания в приём платы с 100 МГц генератором без коррекции
		//#define WITHBBOXFREQ	(26985000L - 1600)		// частота после включения
		//#define WITHBBOXFREQ	(14070000L - 1000)		// прослушивание BPSK частот
		//#define WITHBBOXFREQ	(14065000L - 135)		// частота после включения - 135 коррекция частоты платы с  122.88 для попадания в приём платы с 100 МГц генератором без коррекции
		//#define WITHBBOXFREQ	14065000L		// частота после включения
		//#define WITHBBOXFREQ	(14065000L - 1000)		// частота после включения
		//#define WITHBBOXSUBMODE	SUBMODE_USB	// единственный режим работы
		#define WITHBBOXSUBMODE	SUBMODE_BPSK	// единственный режим работы
		//#define WITHBBOXFREQ	27100000L		// частота после включения
		//#define WITHBBOXSUBMODE	SUBMODE_CW	// единственный режим работы
		//#define WITHBBOXTX		1		// автоматический переход на передачу
	#elif 0
		#define WITHBBOX	1	// Black Box mode - устройство без органов управления
		#define WITHBBOXFREQ	136000L		// частота после включения
		#define WITHBBOXSUBMODE	SUBMODE_USB	// единственный режим работы
		#define WITHBBOXREC	1		// автоматическое включение режима записи после подачи питания
	#elif 0
		#define WITHBBOX	1	// Black Box mode - устройство без органов управления
		#define WITHBBOXFREQ	7030000L		// частота после включения
		#define WITHBBOXSUBMODE	SUBMODE_LSB	// единственный режим работы
		#define WITHBBOXTX		1		// автоматический переход на передачу
		#define WITHBBOXMIKESRC	BOARD_TXAUDIO_2TONE
	#endif


	// FPGA section
	//#define WITHFPGAWAIT_AS	1	/* FPGA загружается из собственной микросхемы загрузчика - дождаться окончания загрузки перед инициализацией SPI в процессоре */
	#define WITHFPGALOAD_PS	1	/* FPGA загружается процессором с помощью SPI */

	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	//#define WITHRFSG	1	/* включено управление ВЧ сигнал-генератором. */
	#define WITHTX		1	/* включено управление передатчиком - сиквенсор, электронный ключ. */
	//#define WITHAUTOTUNER	1	/* Есть функция автотюнера */
	#define WITHIFSHIFT	1	/* используется IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* Начальное занчение IF SHIFT */
	//#define WITHPBT		1	/* используется PBT (если LO3 есть) */
	#define WITHCAT		1	/* используется CAT */
	//#define WITHDEBUG		1	/* Отладочная печать через COM-порт. Без CAT (WITHCAT) */
	//#define WITHMODEM		1	/* Устройство работает как радиомодем с последовательным интерфейсом */
	//#define WITHFREEDV	1	/* поддержка режима FreeDV - http://freedv.org/ */
	//#define WITHNMEA		1	/* используется NMEA parser */
	//#define WITHBEACON	1	/* Используется режим маяка */
	#define WITHVOX			1	/* используется VOX */
	#define WITHSHOWSWRPWR 1	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
	#define WITHSWRMTR	1		/* Измеритель КСВ */
	//#define WITHPWRMTR	1	/* Индикатор выходной мощности или */
	//#define WITHPWRLIN	1	/* Индикатор выходной мощности показывает напряжение а не мощность */
	#define WITHBARS		1	/* отображение S-метра и SWR-метра */
	#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
	#define WITHCURRLEVEL	1	/* отображение тока оконечного каскада */
	//#define WITHSWLMODE	1	/* поддержка запоминания множества частот в swl-mode */
	#define WITHVIBROPLEX	1	/* возможность эмуляции передачи виброплексом */
	#define WITHSPKMUTE		1	/* управление выключением динамика */
	// Есть ли регулировка параметров потенциометрами
	#define WITHPOTWPM		1	/* используется регулировка скорости передачи в телеграфе потенциометром */
	#define WITHPOTGAIN		1	/* регуляторы усиления на потенциометрах */
	//#define WITHPOTPOWER	1	/* регулятор мощности на потенциометре */
	#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */

	//#define WITHONLYBANDS 1		/* Перестройка может быть ограничена любительскими диапазонами */
	//#define WITHBCBANDS	1		/* в таблице диапазонов присутствуют вещательные диапазоны */
	#define WITHWARCBANDS	1	/* В таблице диапазонов присутствуют HF WARC диапазоны */
	//#define WITHLO1LEVELADJ		1	/* включено управление уровнем (амплитудой) LO1 */
	//#define WITHLFM		1	/* LFM MODE */
	//#define WITHTEMPSENSOR	1	/* отображение данных с датчика температуры */
	#define WITHREFSENSOR	1		/* измерение по выделенному каналу АЦП опорного напряжения */
	#define WITHDIRECTBANDS 1	/* Прямой переход к диапазонам по нажатиям на клавиатуре */
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	//#define LO1PHASES	1		/* Прямой синтез первого гетеродина двумя DDS с програмимруемым сдвигом фазы */
	#define DEFPREAMPSTATE 	0	/* УВЧ по умолчанию включён (1) или выключен (0) */

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
	//#define RTC1_TYPE RTC_TYPE_STM32F4xx	/* STM32F4xx internal RTC peripherial */
	//#define TSC1_TYPE TSC_TYPE_STMPE811	/* touch screen controller */
	//#define DAC1_TYPE	99999		/* наличие ЦАП для подстройки тактовой частоты */

	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */

	/* Назначение адресов на SPI шине */
	#define targetdds1	SPI_CSEL_PG0 	/* FPGA NCO1 */
	#define targetext1	SPI_CSEL_PG15 	/* external devices control */
	#define targetctl1	SPI_CSEL_PG7 	/* control register as a chain of registers */
	#define targetfpga1	SPI_CSEL_PG1 	/* control register in FPGA */

	#define targetlcd	targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 targetext1	/* LCD with positive chip select signal	*/

	#define targetnvram SPI_CSEL_PG8  	/* serial nvram */
	#define targetcodec1 SPI_CSEL_PG6 	/* NAU8822L */

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
#define VOLTLEVEL_LOWER		10	// 1.0 kOhm - нижний резистор

#endif /* ARM_STM32F4XX_TQFP144_CTLSTYLE_RAVEN_V6_RV1CB_H_INCLUDED */
