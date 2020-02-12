/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

/* Плата STM32F429I-DISCO с процессором STM32F429ZIT6	*/

#ifndef ARM_STM32F4XX_TQFP144_CPUSTYLE_32F429DISCO_H_INCLUDED
#define ARM_STM32F4XX_TQFP144_CPUSTYLE_32F429DISCO_H_INCLUDED 1

	#if ! defined(STM32F429xx)
		#error Wrong CPU selected. STM32F429xx expected
	#endif /* ! defined(STM32F429xx) */

	//#define WITHSAICLOCKFROMI2S	1	/* Блок SAI1 тактируется от PLL I2S */
	#define	WITHI2SCLOCKFROMPIN 1	// тактовая частота на SPI2 (I2S) подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса
	#define	WITHSAICLOCKFROMPIN 1	// тактовая частота на SAI1 подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса

	#define WITHUSEPLL		1	/* Главная PLL	*/
	#define WITHUSESAIPLL	1	/* SAI PLL - for LTDC	*/
	//#define WITHUSESAII2S	1	/* I2S PLL	*/

	#if 1
		// при наличии внешнего кварцевого резонатора
		#define WITHCPUXTAL 8000000UL	/* На процессоре установлен кварц 8.000 МГц */
		#define REF1_DIV 4			// ref freq = 2.000 MHz

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
		#define I2SCLOCKINFREQ (REFERENCE_FREQ / FPGADIVIDERATIO)

		#define ARMI2SMCLK	(REFERENCE_FREQ / FPGADIVIDERATIO / 2)
		#define ARMSAIMCLK	(REFERENCE_FREQ / FPGADIVIDERATIO / 2)
	#else /* WITHI2SCLOCKFROMPIN */
		// Частота формируется процессором
		#define ARMI2SMCLK	(12288000UL)
		#define ARMSAIMCLK	(12288000UL)
	#endif /* WITHI2SCLOCKFROMPIN */

	#define PLLI2SN_MUL 84		// 172032000UL
	#define SAIREF1_MUL 72		// 288 MHz

	/* модели синтезаторов - схемы частотообразования */

#if 1
	#define FQMODEL_FPGA		1	// FPGA + IQ over I2S
	//#define DIRECT_80M0_X1		1	/* Тактовый генератор на плате 80.0 МГц */
	//#define DIRECT_100M0_X1		1	/* Тактовый генератор на плате 100.0 МГц */
	//#define WITHDAC1VALDEF	178
	#define DIRECT_122M88_X1	1	/* Тактовый генератор 122.880 МГц */
	#define BANDSELSTYLERE_UPCONV56M	1	/* Up-conversion with working band .030..56 MHz */
#elif 0
	/* Версии частотных схем - с преобразованием "наверх" */
	//#define FQMODEL_45_IF8868_UHF430	1	// SW2011
	//#define FQMODEL_73050		1	// 1-st IF=73.050, 2-nd IF=0.455 MHz
	//#define FQMODEL_73050_IF0P5		1	// 1-st IF=73.050, 2-nd IF=0.5 MHz
	//#define FQMODEL_80455		1	// 1-st IF=80.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455		1	// 1-st IF=64.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_64455_IF0P5		1	// 1-st IF=64.455, 2-nd IF=0.5 MHz
	//#define FQMODEL_70455		1	// 1-st IF=70.455, 2-nd IF=0.455 MHz
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	#define FQMODEL_60700_IF05	1	// 60.7 -> 10.7 -> 0.5
	//#define FQMODEL_60725_IF05	1	// 60.725 -> 10.725 -> 0.5
	//#define FQMODEL_60700_IF02	1	// 60.7 -> 10.7 -> 0.2
	//#define FQMODEL_70200		1	// 1-st if = 70.2 MHz, 2-nd IF-200 kHz
	#define BANDSELSTYLERE_UPCONV56M	1	/* Up-conversion with working band .030..56 MHz */
	#define MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */

#elif 0
	/* Версии частотных схем - с преобразованием "наверх" */
	//#define FQMODEL_45_IF8868	1	// SW2011
	//#define FQMODEL_45_IF0P5	1	// 1-st IF=45MHz, 2-nd IF=500 kHz
	//#define FQMODEL_45_IF455	1	// 1-st IF=45MHz, 2-nd IF=455 kHz
	//#define FQMODEL_45_IF10700_IF200		1	// 1st IF=45.0 MHz, 2nd IF=10.7 MHz, 3rd IF=0.2 MHz
	//#define FQMODEL_45_IF8868_IF200	1	// RA6LPO version
	//#define FQMODEL_58M11_IF8868	1	// for gena, DL6RF
	//#define FQMODEL_45_IF6000	1	// up to 32 MHz
	//#define FQMODEL_45_IF8868_UHF144	1	// SW2011
	//#define FQMODEL_45_IF8868	1	// SW2011
	//#define FQMODEL_45_IF0P5	1	// 1-st IF=45MHz, 2-nd IF=500 kHz
	//#define FQMODEL_45_IF455	1	// 1-st IF=45MHz, 2-nd IF=455 kHz
	//#define FQMODEL_45_IF128	1	// 1-st IF=45MHz, 2-nd IF=128 kHz
	//#define BANDSELSTYLERE_UPCONV36M	1	/* Up-conversion with working band 0.1 MHz..36 MHz */
	#define BANDSELSTYLERE_UPCONV32M	1	/* Up-conversion with working band 0.1 MHz..32 MHz */
	#define MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */

#elif 0
	/* Версии частотных схем - с преобразованием "вниз" */
	#define FQMODEL_TRX500K		1	// Первая (и единственна) ПЧ 500 кГц
	//#define FQMODEL_TRX455K		1	// Первая (и единственная) ПЧ 455 кГц
	//#define WITHCLASSICSIDE	1	/* сторона первого гетеродина зависит от рабочей частоты */
	#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	#define BANDSELSTYLERE_LOCONV15M	1	/* Down-conversion with working band .030..15 MHz */
	#define MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */
#elif 1
	/* Версии частотных схем - с прямым преобразованием  */
	#define FQMODEL_DCTRX		1	// прямое преобразование
	//#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	#define DIRECT_100M0_X4		1	/* Board hardware configuration */
	#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	//#define WITHIF4DSP	1	// "Дятел"
	//#define DEFAULT_DSP_IF	12000
	//#define WITHEXTERNALDDSP		1	/* имеется управление внешней DSP платой. */
	#define MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */
#elif 0
	/* Версии частотных схем - с преобразованием "вниз" синтезатор MAXLAB */
	#define FQMODEL_UW3DI		1	// Первая (и единственна) ПЧ 500 кГц
	//#define DIRECT_100M0_X4		1	/* Board hardware configuration */
	#define HYBRID_DDS_ATTINY2313 1
	//#define MAXLAB_7M2			1	/* Board hardware configuration */
	#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	//#define MODEL_MAXLAB	1	/* PLL с уводом опорного генератора варикапом через ЦАП */
	//#define MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */
	#define MODEL_HYBRID 1	/* Гибридный явно тербуется */
#elif 1
	/* Версии частотных схем - с преобразованием "вниз" */
	#define FQMODEL_TRX8M		1	// Первая низкая ПЧ 8 МГц, 6 МГц, 5.5 МГц и другие
	//#define WITHCLASSICSIDE	1	/* сторона первого гетеродина зависит от рабочей частоты */
	// Выбор ПЧ
	//#define IF3_MODEL IF3_TYPE_9000
	//#define IF3_MODEL IF3_TYPE_8000
	#define IF3_MODEL IF3_TYPE_8868
	//#define IF3_MODEL IF3_TYPE_5645	// Drake R-4C and Drake T-4XC (Drake Twins) - 5645 kHz
	//#define IF3_MODEL IF3_TYPE_5500
	//#define IF3_MODEL IF3_TYPE_9045
	#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	//#define DIRECT_80M0_X5		1	/* Board hardware configuration */
	//#define DIRECT_DDS_ATTINY2313 1
	//#define HYBRID_DDS_ATTINY2313 1
	#define BANDSELSTYLERE_LOCONV32M	1	/* Down-conversion with working band .030..32 MHz */
	//#define BANDSELSTYLERE_LOCONV32M_NLB	1	/* Down-conversion with working band 1.6..32 MHz */
	#define MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */
#elif 0
	/* Специальные версии, возможно и не приёмники. */
	//#define FQMODEL_GEN500	1	// CML evaluation board with CMX992 chip, 1-st IF = 45 MHz
	//#define FQMODEL_RFSG_SI570	1	// Сигнал-генератор на Si570 "B" grade
	//#define MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */
#endif	
	// --- вариации прошивки, специфические для разных плат

	#define CTLREGSTYLE_DISCO32	1	// FPGA_V2
	//#define CTLREGSTYLE_NOCTLREG	1

	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define WITHPREAMPATT2_6DB		1	// Управление УВЧ и двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB */

	//#define DSTYLE_UR3LMZMOD	1	// Тестирование - расположение элементов экрана в трансиверах UR3LMZ
	//#define	FONTSTYLE_ITALIC	1	// Использовать альтернативный шрифт

	// +++ Особые варианты расположения кнопок на клавиатуре
	//#define KEYB_RA1AGO	1	/* расположение кнопок для RA1AGO (приёмник воронёнок с символьным дисплеем) */
	#define KEYB_VERTICAL_REV	1	/* расположение кнопок для плат "Воробей" и "Колибри" */
	//#define KEYB_VERTICAL_REV_TOPDOWN	1	/* расположение кнопок для ПЕРЕВЁРНУТЫХ плат "Воробей" и "Колибри" */
	//#define KEYB_VERTICAL	1	/* расположение кнопок для плат "Павлин" */
	//#define KEYB_V8S_DK1VS	1	/* расположение для контроллера DK1VS */
	//#define KEYB_VERTICAL_REV_RU6BK	1	/* расположение кнопок для плат "Воробей" и "Колибри" */
	//#define KEYBOARD_USE_ADC6	1	/* шесть кнопок на каждом входе ADCx */
	//#define KEYB_M0SERG	1	/* расположение кнопок для Serge Moisseyev */
	// --- Особые варианты расположения кнопок на клавиатуре

	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
	//#define LCDMODE_HARD_SPI	1	/* LCD over SPI line */
	#define LCDMODE_LTDC	1		/* Use STM32F4xxx with LCD-TFT Controller (LTDC) */
	#define LCDMODE_LTDC_L8	1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит. */
	//#define LCDMODE_LTDC_L24	1	/* 32-bit на пиксель в памяти (3 байта) */
	#define LCDMODE_LTDCSDRAMBUFF	1	/* используется область внешнего SDRAM для хранения framebuffer */
	#define SDRAM_BANK_ADDR     0xD0000000uL
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
	//#define LCDMODE_UC1608	1		/* Индикатор 240*128 с контроллером UC1608.- монохромный */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ST7735	1	/* Индикатор 160*128 с контроллером Sitronix ST7735 - TFT панель 160 * 128 HY-1.8-SPI */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - перевернуть изображение (для выводов справа) */
	//#define LCDMODE_ST7565S	1	/* Индикатор WO12864C2-TFH# 128*64 с контроллером Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ILI9320	1	/* Индикатор 248*320 с контроллером ILI9320 */
	#define LCDMODE_ILI9341	1	/* 320*240 SF-TC240T-9370-T с контроллером ILI9341 - STM32F4DISCO */
	//#define LCDMODE_ILI9341_TOPDOWN	1	/* LCDMODE_ILI9341 - перевернуть изображение (для выводов справа) */
	//#define LCDMODE_ILI8961	1	/* 320 * 240 HHT270C-8961-6A6, RGB, ILI8961, use LCDMODE_LTDC_L24 and LCDMODE_LTDCSDRAMBUFF */
	//#define LCDMODE_ILI8961_TOPDOWN	1
	//#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	// --- Одна из этих строк определяет тип дисплея, для которого компилируется прошивка

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	//#define WITHDIRECTFREQENER	1
	#define WITHSPLIT	1	/* управление режимами расстройки одной кнопкой */
	//#define WITHSPLITEX	1	/* Трехкнопочное управление режимами расстройки */
	#define WITHCATEXT	1	/* Расширенный набор команд CAT */
	#define WITHELKEY	(! WITHDEBUG)	// PA9 & PA10 - confilct with debug
	//#define WITHKBDENCODER 1	// перестройка частоты кнопками
	#define WITHKEYBOARD 1	/* в данном устройстве есть клавиатура */
	#define KEYBOARD_USE_ADC	1	/* на одной линии установлено  четыре  клавиши. на vref - 6.8K, далее 2.2К, 4.7К и 13K. */
	#define WITHENCODER	1	/* для изменения частоты имеется енкодер */
	#define WITHNESTEDINTERRUPTS	1	/* используется при наличии real-time части. */
	#define WITHINTEGRATEDDSP		1	/* в программу включена инициализация и запуск DSP части. */
	#define WITHIF4DSP	1	// "Дятел"
	#define WITHDSPLOCALFIR 1			/* Фильтрация квадратур осуществляется процессором */
	//#define WITHEXTERNALDDSP		1	/* имеется управление внешней DSP платой. */
	//#define WITHLOOPBACKTEST	1
	//#define DEFAULT_DSP_IF	12000
	#define WITHDSPEXTDDC 1			/* Квадратуры получаются внешней аппаратурой */
	//#define WITHDSPEXTFIR 1			/* Фильтрация квадратур осуществляется внешней аппаратурой */

	#if 1
		#define WITHTRANSPARENTIQ	1	/* процессор просто поддерживает двунаправленный обмен между USB и FPGA */
		#define WITHRTS96 1		/* Получение от FPGA квадратур, возможно передача по USB и отображение спектра/водопада. */
		#define	WITHBBOXMIKESRC	BOARD_TXAUDIO_USB
	#endif

	#define WITHSAICLOCKFROMPIN 1	// тактовая частота на SAI1 подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса
	//#define WITHSAI1_FRAMEBITS 256	// Полный размер фрейма
	#define WITHSAI1_FRAMEBITS 64	// Полный размер фрейма
	#define WITHSAI1_FORMATI2S_PHILIPS 1	// требуется при получении данных от FPGA
	#define FPGADIVIDERATIO 5
	#define EXTSAI_FREQ (REFERENCE_FREQ / FPGADIVIDERATIO)
	// DDC Module 1
	#define WITHIFDACWIDTH	16 //32		// 1 бит знак и 31 бит значащих
	#define WITHIFADCWIDTH	24 //32		// 1 бит знак и 31 бит значащих

	#define EXTI2S_FREQ (REFERENCE_FREQ / FPGADIVIDERATIO)
	#define WITHI2S_FORMATI2S_PHILIPS 1	// Возможно использование при передаче данных в кодек, подключенный к наушникам и микрофону
	#define WITHI2SHWRXSLAVE	1		// Приёмный канал I2S (микрофон) используюся в SLAVE MODE
	#define WITHI2SHWTXSLAVE	1		// Передающий канал I2S (наушники) используюся в SLAVE MODE
	// USB интерфейс и все кодеки
	#define WITHAFADCWIDTH	16		// 1 бит знак и 15 бит значащих
	#define WITHAFDACWIDTH	16		// 1 бит знак и 15 бит значащих

	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	//#define WITHRFSG	1	/* включено управление ВЧ сигнал-генератором. */
	#define WITHTX		1	/* включено управление передатчиком - сиквенсор, электронный ключ. */
	//#define WITHIFSHIFT	1	/* используется IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* Начальное занчение IF SHIFT */
	//#define WITHPBT		1	/* используется PBT (если LO3 есть) */
	//#define WITHMODEM		1	/* Устройство работает как радиомодем с последовательным интерфейсом */
	#define WITHCAT		1	/* используется CAT */
	//#define WITHDEBUG		1	/* Отладочная печать через COM-порт. Без CAT (WITHCAT) */
	//#define WITHBEACON	1	/* Используется режим маяка */
	//#define WITHVOX		1	/* используется VOX */
	#define WITHSHOWSWRPWR 1	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
	//#define WITHSWRMTR	1	/* Измеритель КСВ */
	//#define WITHPWRMTR	1	/* Индикатор выходной мощности или */
	//#define WITHPWRLIN	1	/* Индикатор выходной мощности показывает напряжение а не мощность */
	#define WITHBARS	1	/* отображение S-метра и SWR-метра */
	//#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
	//#define WITHSWLMODE	1	/* поддержка запоминания множества частот в swl-mode */
	//#define WITHPOTWPM		1	/* используется регулировка скорости передачи в телеграфе потенциометром */
	#define WITHVIBROPLEX	1	/* возможность эмуляции передачи виброплексом */
	#define WITHREFSENSOR	1	/* измерение по выделенному каналу АЦП опорного напряжения */

	#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */
	//#define WITHFIXEDBFO	1	/* Переворот боковых за счёт 1-го гетродина (особенно, если нет подстраиваемого BFO) */
	//#define WITHDUALFLTR	1	/* Переворот боковых за счёт переключения фильтра верхней или нижней боковой полосы */
	//#define WITHSAMEBFO	1	/* использование общих настроек BFO для приёма и передачи */
	//#define WITHONLYBANDS 1		/* Перестройка может быть ограничена любительскими диапазонами */
	#define WITHBCBANDS	1		/* в таблице диапазонов присутствуют вещательные диапазоны */
	#define WITHWARCBANDS	1	/* В таблице диапазонов присутствуют HF WARC диапазоны */
	//#define WITHLO1LEVELADJ		1	/* включено управление уровнем (амплитудой) LO1 */
	//#define WITHLFM		1	/* LFM MODE */
	//#define WITHTEMPSENSOR	1	/* отображение данных с датчика температуры */
	//#define WITHSLEEPTIMER	1	/* выключить индикатор и вывод звука по истечениии указанного времени */
	#define BOARD_FFTZOOM_POW2MAX 1	// Возможные масштабы FFT x1
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	//#define LO1PHASES	1		/* Прямой синтез первого гетеродина двумя DDS с програмимруемым сдвигом фазы */
	#define DEFPREAMPSTATE 	0	/* УВЧ по умолчанию включён (1) или выключен (0) */

	/* что за память настроек и частот используется в контроллере */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// Так же при использовании FM25040A - 5 вольт, 512 байт
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A
	#define NVRAM_TYPE NVRAM_TYPE_BKPSRAM	// Область памяти с батарейным питанием
	//#define HARDWARE_IGNORENONVRAM	1		// отладка на платах где нет никакого NVRAM

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */

	/* Board hardware configuration */
	#define MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */
	/* Board hardware configuration */
	//#define DDS1_TYPE DDS_TYPE_FPGAV1
	#define DDS1_TYPE DDS_TYPE_FPGAV2
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define DDS1_TYPE DDS_TYPE_AD9851
	//#define DDS2_TYPE DDS_TYPE_AD9834
	//#define DDS2_TYPE DDS_TYPE_AD9851
	//#define PLL2_TYPE PLL_TYPE_CMX992
	//#define DDS3_TYPE DDS_TYPE_AD9834
	//#define RTC1_TYPE RTC_TYPE_M41T81	/* ST M41T81M6 RTC clock chip with I2C interface */
	#define RTC1_TYPE RTC_TYPE_STM32F4xx	/* STM32F4xx/STM32F7xx internal RTC peripherial */
	#define WITHRTCLSI 1	/* запуск LSI (без кварца) */
	//#define TSC1_TYPE TSC_TYPE_STMPE811	/* touch screen controller */

	//#define DDS1_TYPE DDS_TYPE_ATTINY2313
	//#define PLL1_TYPE PLL_TYPE_LM7001
	//#define targetpll1 SPI_CSEL1

	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
	#define DDS2_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS2 */
	#define DDS3_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS3 */

	///#define DDS1_TYPE DDS_TYPE_AD9834
	///#define targetdds1 SPI_CSEL1 	/* DDS1 - LO1 output - работает AD9834 (Vasiliy, Ufa). */


	/* Назначение адресов на SPI шине */
	#define targetfpga1		SPI_CSEL2 	/* FPGA */
	#define targetlcd		SPI_CSEL1 	/* LCD over SPI line devices control */ 
	#define targetnvram		SPI_CSEL4	/* FM25L16B chip */
	#define targetcodec1	SPI_CSEL3 	/* CS4272 or different audio codec */

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	//#define BOARD_DETECTOR_MUTE 	(0x02)
	//#define BOARD_DETECTOR_SSB 	(0x00)
	//#define BOARD_DETECTOR_AM 	(0x01)
	//#define BOARD_DETECTOR_FM 	(0x03)
	//#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define BOARD_FILTER_0P5		0x00	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_1P8		0x01	/* 1.8 kHz filter - на очень старых платах */
	#define BOARD_FILTER_2P7		0x03	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_3P1		0x03	/* 3.1 or 2.75 kHz filter */

	//#define BOARD_AGCCODE_OFF 0
	#if 1
		// Вариант с фильтом 3.1 кГц
		//#define WITHMODESETSMART 1	/* в заваисмости от частоты меняется боковая, используется вместе с WITHFIXEDBFO */
		//#define WITHMODESETFULL 1
		#define WITHMODESETFULLNFM 1
		#define WITHWFM	1			/* используется WFM */
		/* все возможные в данной конфигурации фильтры */
		#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
		/* все возможные в данной конфигурации фильтры для передачи */
		#define IF3_FMASKTX	(IF3_FMASK_3P1)
		/* фильтры, для которых стоит признак HAVE */
		#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_3P1)
	#elif 1
		// Вариант с фильтом 2.7 кГц
		//#define WITHMODESETSMART 1	/* в заваисмости от частоты меняется боковая, используется вместе с WITHFIXEDBFO */
		#define WITHMODESETFULL 1
		//#define WITHMODESETFULLNFM 1
		//#define WITHWFM	1			/* используется WFM */
		/* все возможные в данной конфигурации фильтры */
		#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_2P7)
		/* все возможные в данной конфигурации фильтры для передачи */
		#define IF3_FMASKTX	(IF3_FMASK_2P7)
		/* фильтры, для которых стоит признак HAVE */
		#define IF3_FHAVE	(IF3_FMASK_0P5 | IF3_FMASK_2P7)
	#else
		//#define WITHMODESETSMART 1	/* в заваисмости от частоты меняется боковая, используется вместе с WITHFIXEDBFO */
		#define WITHMODESETFULL 1
		//#define WITHMODESETFULLNFM 1
		//#define WITHWFM	1			/* используется WFM */
		#define IF3_FMASK (IF3_FMASK_2P7 | IF3_FMASK_1P8 | IF3_FMASK_0P5)
		#define IF3_FMASKTX (IF3_FMASK_2P7)
		#define IF3_FHAVE (IF3_FMASK_2P7 | IF3_FMASK_0P5)
	#endif

#endif /* ARM_STM32F4XX_TQFP144_CPUSTYLE_32F429DISCO_H_INCLUDED */
