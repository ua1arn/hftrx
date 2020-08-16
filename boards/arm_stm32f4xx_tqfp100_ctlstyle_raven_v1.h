/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Трансивер с 12-кГц DSP обработкой "Воронёнок-2" на процессоре STM32F429 с кодеками TLV320AIC23B и CS4272
//

#ifndef ARM_STM32F4XX_TQFP100_CTLSTYLE_RAVEN_V1_H_INCLUDED
#define ARM_STM32F4XX_TQFP100_CTLSTYLE_RAVEN_V1_H_INCLUDED 1

	#define WITHSAICLOCKFROMI2S 1	/* Блок SAI1 тактируется от PLL I2S */
	//#define WITHI2SCLOCKFROMPIN 1	// тактовая частота на SPI2 (I2S) подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса
	//#define WITHSAICLOCKFROMPIN 1	// тактовая частота на SAI1 подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса

	#define WITHUSEPLL		1	/* Главная PLL	*/
	#define WITHUSESAIPLL	1	/* SAI PLL	*/
	//#define WITHUSESAII2S	1	/* I2S PLL	*/

	#if 0
		// при наличии внешнего кварцевого резонатора
		#define WITHCPUXTAL 16000000uL	/* На процессоре установлен кварц 16.000 МГц */
		#define REF1_DIV 8			// ref freq = 2.0000 MHz

		#if CPUSTYLE_STM32F7XX
			// normal operation frequency
			#define REF1_MUL 216		// 2*192.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS	// overvlocking
		#elif CPUSTYLE_STM32F4XX
			// normal operation frequency
			#define REF1_MUL 180		// 2*180.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_5WS	// Задержка для работы с памятью 5 WS for 168 MHz at 3.3 volt
		#endif

	#elif 1
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

		#if CPUSTYLE_STM32F7XX
			// normal operation frequency
			#define REF1_MUL 216		// 2*192.000 MHz (192 <= PLLN <= 432) - overclocking, but USB 48 MHz clock generation
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS	// overvlocking
		#elif CPUSTYLE_STM32F4XX
			// normal operation frequency
			#define REF1_MUL 180		// 2*180.000 MHz (192 <= PLLN <= 432)
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
	#define FQMODEL_80455		1	// 1-st IF=80.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455		1	// 1-st IF=64.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455_IF0P5		1	// 1-st IF=64.455, 2-nd IF=0.5 MHz
	//#define FQMODEL_70455		1	// 1-st IF=70.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	//#define FQMODEL_60700_IF05	1	// 60.7 -> 10.7 -> 0.5
	//#define FQMODEL_60725_IF05	1	// 60.725 -> 10.725 -> 0.5
	//#define FQMODEL_60700_IF02	1	// 60.7 -> 10.7 -> 0.2
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	#define BANDSELSTYLERE_UPCONV56M	1	/* Up-conversion with working band .030..56 MHz */
	// --- вариации прошивки, специфические для разных плат

	#define CTLREGMODE_RAVENDSP_V1	1	/* "Воронёнок" с DSP */

	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея
	#define WITHLCDBACKLIGHTMIN	0	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_SSB 	0		// Заглушка

	/* Назначение биов управления коммутаторов фильтров */
	#define BOARD_FMUX_BYP_RX	0x80	// включён bypass фильтр ПЧ
	#define BOARD_FMUX_BYP_OFF	0x01	// выключен bypass фильтр ПЧ
	#define BOARD_FMUX_6P0_RX	0x40	// включён 6 кГц фильтр ПЧ
	#define BOARD_FMUX_6P0_TX	0x02	// выключен 6 кГц фильтр ПЧ
	#define BOARD_FMUX_3P1_RX	0x20	// включён 3.1 кГц фильтр ПЧ
	#define BOARD_FMUX_3P1_TX	0x04	// выключен 3.1 кГц фильтр ПЧ
	#define BOARD_FMUX_0P5_RX	0x10	// включён 0.5 кГц фильтр ПЧ
	#define BOARD_FMUX_0P5_OFF	0x08	// выключен 0.5 кГц фильтр ПЧ
	/* коды фильтров второй ПЧ, выдаваемые на мультиплексоры ADG714 */
	/* Включить один из фильтров в тракт приёма */
	#define	BOARD_FILTERCODE_BYPRX	(BOARD_FMUX_BYP_RX | BOARD_FMUX_6P0_TX | BOARD_FMUX_3P1_TX | BOARD_FMUX_0P5_OFF)
	#define	BOARD_FILTERCODE_6P0RX	(BOARD_FMUX_BYP_OFF | BOARD_FMUX_6P0_RX | BOARD_FMUX_3P1_TX | BOARD_FMUX_0P5_OFF)
	#define	BOARD_FILTERCODE_3P1RX	(BOARD_FMUX_BYP_OFF | BOARD_FMUX_6P0_TX | BOARD_FMUX_3P1_RX | BOARD_FMUX_0P5_OFF)
	#define	BOARD_FILTERCODE_0P5RX	(BOARD_FMUX_BYP_OFF | BOARD_FMUX_6P0_TX * 0| BOARD_FMUX_3P1_TX * 1 | BOARD_FMUX_0P5_RX)
	/* Включить один из фильтров в тракт передачи */
	#define	BOARD_FILTERCODE_6P0TX	(BOARD_FMUX_BYP_OFF | BOARD_FMUX_6P0_TX * 1 | BOARD_FMUX_3P1_TX * 0 | BOARD_FMUX_0P5_OFF)
	#define	BOARD_FILTERCODE_3P1TX	(BOARD_FMUX_BYP_OFF | BOARD_FMUX_6P0_TX * 0 | BOARD_FMUX_3P1_TX * 1 | BOARD_FMUX_0P5_OFF)

	// +++ заглушки для плат с DSP обработкой
	#define	BOARD_AGCCODE_ON	0x00
	#define	BOARD_AGCCODE_OFF	0x01
	// --- заглушки для плат с DSP обработкой

	#define WITHPREAMPATT2_6DB		1	// Управление УВЧ и двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB */

	#define WITHAGCMODEONOFF	1	// АРУ вкл/выкл
	//#define WITHMIC1LEVEL		1	// установка усиления микрофона

	//#define DSTYLE_UR3LMZMOD	1	// Расположение элементов экрана в трансиверах UR3LMZ
	#define	FONTSTYLE_ITALIC	1	// Использовать альтернативный шрифт

	// +++ Особые варианты расположения кнопок на клавиатуре
	// для пяти входов АЦП с клавиатуры
	#define KEYB_FPANEL20_V0A	1	/* 20 кнопок на 5 линий - плата rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении с новым расположением */
	//#define KEYB_RAVEN20_V5	1		/* 5 линий клавиатуры: расположение кнопок для Воробей с DSP обработкой */

	// для шести входов АЦП с клавиатуры
	//#define KEYB_RAVEN24	1	/* расположение кнопок для Воробей с DSP обработкой */
	//#define KEYB_VERTICAL_REV	1	/* расположение кнопок для плат "Воробей" и "Колибри" */
	//#define KEYB_VERTICAL_REV_TOPDOWN	1	/* расположение кнопок для ПЕРЕВЁРНУТЫХ плат "Воробей" и "Колибри" */
	//#define KEYB_VERTICAL	1	/* расположение кнопок для плат "Павлин" */
	//#define KEYB_V8S_DK1VS	1	/* расположение для контроллера DK1VS */
	//#define KEYB_VERTICAL_REV_RU6BK	1	/* расположение кнопок для плат "Воробей" и "Колибри" */
	//#define KEYBOARD_USE_ADC6	1	/* шесть кнопок на каждом входе ADCx */
	//#define KEYB_M0SERG	1	/* расположение кнопок для Serge Moisseyev */
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
	//#define LCDMODE_ILI9225	1	/* Индикатор 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 с контроллером ILI9225С */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - перевернуть изображение (для выводов слева от экрана) */
	#define LCDMODE_UC1608	1		/* Индикатор 240*128 с контроллером UC1608.- монохромный */
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
	#define	CODEC1_TYPE	CODEC_TYPE_TLV320AIC23B
	#define CODEC2_TYPE CODEC_TYPE_CS4272
	#define WITHSAI1HWTXRXMASTER	1		// SAI1 work in MASTER mode

	//#define CODEC_TYPE_CS4272_USE_SPI	1		// codecboard v2.0
	#define CODEC_TYPE_CS4272_STANDALONE	1		// codecboard v3.0
	#define CODEC_TYPE_TLV320AIC23B_USE_SPI	1	// codecboard v2.0

	#if defined (STM32F446xx) || CPUSTYLE_STM32F7XX
		// Инициализируются I2S2 и I2S3
		// На некоторых процессорах I2S не может работать в full duplex mode,
		// для этого используютя I2S2 как передатчик и I2S3 как приёмник с внешними соединениями
		#define WITHI2SHWRXSLAVE	1		// Приёмный канал I2S (микрофон) используюся в SLAVE MODE
	#else /* defined (STM32F446xx) */
		#define WITHI2SI2S2EXTFULLDUPLEXHW 1
	#endif /* defined (STM32F446xx) */

	//#define WITHI2S_FORMATI2S_PHILIPS 1	// Возможно использование при передаче данных в кодек, подключенный к наушникам и микрофону
	#define WITHSAI1_FRAMEBITS	64	// Полный размер фрейма
	#define WITHSAICLOCKFROMI2S	1	/* Блок SAI1 тактируется от PLL I2S */
	//#define WITHI2SCLOCKFROMPIN 1	// тактовая частота на SPI2 (I2S) подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса
	//#define WITHSAICLOCKFROMPIN 1	// тактовая частота на SAI1 подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса

	#define WITHNESTEDINTERRUPTS	1	/* используется при наличии real-time части. */
	#define WITHINTEGRATEDDSP		1	/* в программу включена инициализация и запуск DSP части. */
	//#define WITHDACOUTDSPAGC		1	/* АРУ реализовано как выход ЦАП на аналоговую часть. */
	//#define WITHDSPEXTDDC 1			/* Квадратуры получаются внешней аппаратурой */

	#define WITHIFDACWIDTH	24		// 1 бит знак и 23 бит значащих
	#define WITHIFADCWIDTH	24		// 1 бит знак и 23 бит значащих
	#define WITHAFADCWIDTH	16		// 1 бит знак и 15 бит значащих
	#define WITHAFDACWIDTH	16		// 1 бит знак и 15 бит значащих
	//#define WITHEXTERNALDDSP		1	/* имеется управление внешней DSP платой. */
	//#define WITHLOOPBACKTEST	1
	#define WITHIF4DSP	1	// "Дятел"
	#define WITHDSPLOCALFIR 1			/* Фильтрация квадратур осуществляется процессором */
	//#define WITHMODEM 1		// тестирование приёма/передачи BPSK31
	//#define WITHFREEDV	1	/* поддержка режима FreeDV - http://freedv.org/ */
	//#define WITHUSESDCARD 1			// Включение поддержки SD CARD
	//#define WITHUSEAUDIOREC	1	// Запись звука на SD CARD

	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	//#define WITHRFSG	1	/* включено управление ВЧ сигнал-генератором. */
	#define WITHTX		1	/* включено управление передатчиком - сиквенсор, электронный ключ. */
	//#define WITHAUTOTUNER	1	/* Есть функция автотюнера */
	#define WITHIFSHIFT	1	/* используется IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* Начальное занчение IF SHIFT */
	//#define WITHPBT		1	/* используется PBT (если LO3 есть) */
	#define WITHCAT		1	/* используется CAT */
	//#define WITHDEBUG		1	/* Отладочная печать через COM-порт. Без CAT (WITHCAT) */
	//#define WITHNMEA		1	/* используется NMEA parser */
	//#define WITHBEACON	1	/* Используется режим маяка */
	#define WITHVOX 1		/* используется VOX */
	#define WITHSHOWSWRPWR 1	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
	#define WITHSWRMTR	1	/* Измеритель КСВ */
	//#define WITHPWRMTR	1	/* Индикатор выходной мощности или */
	//#define WITHPWRLIN	1	/* Индикатор выходной мощности показывает напряжение а не мощность */
	#define WITHBARS	1	/* отображение S-метра и SWR-метра */
	//#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
	//#define WITHSWLMODE	1	/* поддержка запоминания множества частот в swl-mode */
	////#define WITHPOTWPM		1	/* используется регулировка скорости передачи в телеграфе потенциометром */
	#define WITHPOTGAIN		1	/* регуляторы усиления на потенциометрах */
	#define WITHVIBROPLEX	1	/* возможность эмуляции передачи виброплексом */
	#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */
	//#define WITHFIXEDBFO	1	/* Переворот боковых за счёт 1-го гетродина (особенно, если нет подстраиваемого BFO) */
	//#define WITHDUALBFO		1	/* Переворот боковых за счёт переключения частоты BFO внешним сигналом */
	//#define WITHDUALFLTR	1	/* Переворот боковых за счёт переключения фильтра верхней или нижней боковой полосы */


	#define WITHSAMEBFO	1	/* использование общих настроек BFO для приёма и передачи */
	//#define WITHONLYBANDS 1		/* Перестройка может быть ограничена любительскими диапазонами */
	#define WITHBCBANDS	1		/* в таблице диапазонов присутствуют вещательные диапазоны */
	#define WITHWARCBANDS	1	/* В таблице диапазонов присутствуют HF WARC диапазоны */
	//#define WITHLO1LEVELADJ		1	/* включено управление уровнем (амплитудой) LO1 */
	//#define WITHLFM		1	/* LFM MODE */
	#define WITHTEMPSENSOR	1	/* отображение данных с датчика температуры */
	#define WITHDIRECTBANDS 1	/* Прямой переход к диапазонам по наэатиям на клавиатуре */
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
	//#define HARDWARE_IGNORENONVRAM	1		// отладка на платах где нет никакого NVRAM

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */

	#define MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */
	/* Board hardware configuration */
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	#define DDS1_TYPE DDS_TYPE_AD9951
	//#define PLL1_TYPE PLL_TYPE_ADF4001
	#define DDS2_TYPE DDS_TYPE_AD9834
	//#define RTC1_TYPE RTC_TYPE_STM32F4xx	/* STM32F4xx internal RTC peripherial */

	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
	#if FQMODEL_80455
		#define DDS2_CLK_DIV	2		/* Делитель опорной частоты перед подачей в DDS2 */
	#else
		#define DDS2_CLK_DIV	2		/* Делитель опорной частоты перед подачей в DDS2 */
	#endif
	////#define HYBRID_NVFOS 4				/* количество ГУН первого гетеродина */
	////#define HYBRID_PLL1_POSITIVE 1		/* в петле ФАПЧ нет инвертирующего усилителя */
	//#define HYBRID_PLL1_NEGATIVE 1		/* в петле ФАПЧ есть инвертирующий усилитель */

	/* Назначение адресов на SPI шине */
	#define targetdds1 SPI_CSEL0 	/* DDS1 */
	#define targetdds2 SPI_CSEL1 	/* DDS2 - LO3 output */
	#define targetpll1 SPI_CSEL2 	/* ADF4001 after DDS1 - divide by r1 and scale to n1. Для двойной DDS первого гетеродина - вторая DDS */
	#define targetext1 SPI_CSEL4 	/* external devices control */
	#define targetext2 SPI_CSEL5 	/* external devices control */
	#define targetctl1 SPI_CSEL6 	/* control register */

	#define targetlcd targetext1 	/* LCD over SPI line devices control */ 
	#define targetsdcard targetext2	/* Запись звука на SD CARD */

	//#define targetuc1608 SPI_CSEL255	/* LCD with positive chip select signal	*/
	#define targetuc1608 targetext1	/* LCD with positive chip select signal	*/

	#define targetnvram SPI_CSEL8  	/* serial nvram */
	#define targetcodec1 SPI_CSEL9 	/* PD3 TLV320AIC23B */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define BOARD_FILTER_0P5		BOARD_FILTERCODE_0P5RX	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_3P1		BOARD_FILTERCODE_3P1RX	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_6P0		BOARD_FILTERCODE_6P0RX	/* 6.0 kHz filter */
	#define BOARD_FILTER_8P0		BOARD_FILTERCODE_BYPRX	/* 6.0 kHz filter */

	#define WITHMODESETFULLNFM 1
	//#define WITHWFM	1			/* используется WFM */
	/* все возможные в данной конфигурации фильтры */
	#define IF3_FMASK	(/*IF3_FMASK_0P5 | IF3_FMASK_3P1 | */ IF3_FMASK_6P0 | IF3_FMASK_8P0)
	/* все возможные в данной конфигурации фильтры для передачи */
	#define IF3_FMASKTX	(/*IF3_FMASK_3P1 | */ IF3_FMASK_6P0)
	/* фильтры, для которых стоит признак HAVE */
	#define IF3_FHAVE	(/* IF3_FMASK_0P5 | IF3_FMASK_3P1 | */ IF3_FMASK_6P0 | IF3_FMASK_8P0)

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
#if WITHTEMPSENSOR
	TEMPIX = 16,		// ADC1->CR2 |= ADC_CR2_TSVREFE;	// для тестов
#endif

#if WITHPOTGAIN
	POTIFGAIN = 10,		// PC0 IF GAIN
	POTAFGAIN = 11,		// PC1 AF GAIN
#endif /* WITHPOTGAIN */

#if WITHPOTWPM
	POTWPM = 12,		// PC2 потенциометр управления скоростью передачи в телеграфе
#endif /* WITHPOTWPM */

	POTAUX1 = 13,		// PC3 AUX1
	POTAUX2 = 14,		// PC4 AUX2
	POTAUX3 = 15,		// PC5 AUX3

#if WITHSWRMTR
	PWRI = 9,			// PB1
	FWD = 9, REF = 8,	// PB0	SWR-meter
#endif /* WITHSWRMTR */
	// для шести входов АЦП с клавиатуры
	//KI0 = 0, KI1 = 1, KI2 = 2, KI3 = 3, KI4 = 6, KI5 = 7	// клавиатура
	// для пяти входов АЦП с клавиатуры
	KI0 = 0, KI1 = 1, KI2 = 2, KI3 = 3, KI4 = 6	// клавиатура
};

#define KI_COUNT 5	// количество используемых под клавиатуру входов АЦП
#define KI_LIST	KI4, KI3, KI2, KI1, KI0,	// инициализаторы для функции перекодировки
//#define KI_COUNT 6	// количество используемых под клавиатуру входов АЦП
//#define KI_LIST	KI5, KI4, KI3, KI2, KI1, KI0,	// инициализаторы для функции перекодировки

#define WITHNOTXDACCONTROL	1	/* в этой версии нет ЦАП управления смещением TXDAC передатчика */
#define WITHPOWERTRIM		1	// Имеется управление мощностью
#define WITHPOWERTRIMMIN	5	// Нижний предел регулировки (показываемый на дисплее)
#define WITHPOWERTRIMMAX	100	// Верхний предел регулировки (показываемый на дисплее)
#define WITHPOWERTRIMATU	15	// Значение для работы автотюнера

#endif /* ARM_STM32F4XX_TQFP100_CTLSTYLE_RAVEN_V1_H_INCLUDED */
