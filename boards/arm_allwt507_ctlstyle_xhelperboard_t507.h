/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Трансивер с DSP обработкой "Аист" на процессоре Allwinner T507
// v5km7_ddc_sv9k_a53_R3.0.pcb Allwinner T507, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
// HelperBoard T507 Core Board

#ifndef ARM_ALW_T507_CTL_XHELPERBOARD_H_INCLUDED
#define ARM_ALW_T507_CTL_XHELPERBOARD_H_INCLUDED 1

#define WITHCTRLBOARDT507 1

	#define WITHBRANDSTR "Falcon"

	//#define WITHSAICLOCKFROMI2S 1	/* Блок SAI1 тактируется от PLL I2S */
	// в данной конфигурации I2S и SAI - в режиме SLAVE
	#define WITHI2SCLOCKFROMPIN 1	// тактовая частота на SPI2 (I2S) подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса

	#define LSEFREQ 32768u

	#define WITHCPUXTAL 24000000u	/* На процессоре установлен кварц 24.000 МГц */
	//#define WITHCPUXOSC 24000000u	/* На процессоре установлен генератор 24.000 МГц */

	//  PLL_CPUX=24MHz*N/P
	#define PLL_CPU_N 	50
	#define PLL_CPU_P_POW 0

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
	//#define XVTR_NYQ1			1	// Support Nyquist-style frequency conversion
	//#define XVTR_R820T2 	1	/* R820T chip */

	// --- вариации прошивки, специфические для разных частот

	//#define CTLREGMODE_STORCH_V9A	1	/* STM32MP1, Allwinner t113-s3  */
	#define CTLREGMODE_NOCTLREG 1

	


	// +++ заглушки для плат с DSP обработкой
	#define BOARD_NOTCH_OFF		0
	#define BOARD_NOTCH_MANUAL	0
	#define BOARD_NOTCH_AUTO	0
	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
//	#define BOARD_DETECTOR_SSB 	0		// Заглушка
//	#define BOARD_DETECTOR_AM 	1		// Заглушка
//	#define BOARD_DETECTOR_FM 	2		// Заглушка
//	#define BOARD_DETECTOR_MUTE 	3		// Заглушка
//	#define BOARD_DETECTOR_TUNE 	4		// Заглушка

	// +++ заглушки для плат с DSP обработкой
	#define	BOARD_AGCCODE_ON	0x00
	#define	BOARD_AGCCODE_OFF	0x01

	#define WITHPABIASMIN		0
	#define WITHPABIASMAX		255

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
	#define WITHMIC1LEVEL		1	// установка усиления микрофона

	//#define DSTYLE_UR3LMZMOD	1	// Расположение элементов экрана в трансиверах UR3LMZ
	#define	FONTSTYLE_ITALIC	1	// Использовать альтернативный шрифт

	// +++ Особые варианты расположения кнопок на клавиатуре
	//#define KEYB_RAVEN20_V5	1		/* 5 линий клавиатуры: расположение кнопок для Воробей с DSP обработкой */
	#define KEYB_FPANEL20_V0A	1	/* 20 кнопок на 5 линий - плата rfrontpanel_v0 + LCDMODE_UC1608 в нормальном расположении с новым расположением */
	//#define KEYB_FPANEL20_V0A_RA1AGO	1	/* перевернутый */
	// --- Особые варианты расположения кнопок на клавиатуре
	#define WITHSPLIT	1	/* управление режимами расстройки одной кнопкой */
	//#define WITHSPLITEX	1	/* Трехкнопочное управление режимами расстройки */

	// +++ Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
#if WITHISBOOTLOADER || 1

	#define LCDMODE_DUMMY	1

#elif 0

	#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */

	//#define LCDMODE_V2A_2PAGE 1	/* только главный экран 16 бит (две страницы), без PIP */
	#define LCDMODE_V5A	1	/* только главный экран с двумя видеобуферами 32 бит ARGB8888, без PIP */

	//#define WITHFLATLINK 1	/* Работа с TFT панелью через SN75LVDS83B	*/

#elif 1

	#define LCDMODE_AT070TN90 1	/* AT070TN90 panel (800*480) - 7" display */

	//#define LCDMODE_V2 1	/* только главный экран 8 бит (две страницы), L8, без PIP */
	#define LCDMODE_V2A_2PAGE 1	/* только главный экран 16 бит (две страницы), без PIP */
	//#define LCDMODE_V5A	1	/* только главный экран с двумя видеобуферами 32 бит ARGB8888, без PIP */

	//#define WITHFLATLINK 1	/* Работа с TFT панелью через преобразователь RGB->FlatLink SN75LVDS83B	*/
	#define WITHLCDDEMODE	1	/* DE MODE: MODE="1", VS and HS must pull high. */

#elif 1

	#define LCDMODE_AT070TNA2 1	/* AT070TNA2 panel (1024*600) - 7" display */

	//#define LCDMODE_V2A_2PAGE 1	/* только главный экран 16 бит (две страницы), без PIP */
	#define LCDMODE_V5A	1	/* только главный экран с двумя видеобуферами 32 бит ARGB8888, без PIP */

	//#define WITHFLATLINK 1	/* Работа с TFT панелью через SN75LVDS83B	*/
	#define WITHLCDDEMODE	1	/* DE MODE: MODE="1", VS and HS must pull high. */

#elif 1

	#define LCDMODE_TCG104XGLPAPNN 1	/* TCG104XGLPAPNN-AN30 panel (1024*768) - 10.4" display - DE mode required */
	#define LCDMODE_V2A_2PAGE 1	/* только главный экран 16 бит (две страницы), без PIP */
	//#define WITHFLATLINK 1	/* Работа с TFT панелью через SN75LVDS83B	*/
	#define WITHLCDDEMODE	1	/* DE MODE: MODE="1", VS and HS must pull high. */

#elif 0

	#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	#define LCDMODE_S1D13781	1	/* Инндикатор 480*272 с контроллером Epson S1D13781 */
	//#define LCDMODE_S1D13781_NHWACCEL 1	/* Неиспользоване аппаратных особенностей EPSON S1D13781 при выводе графики */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - перевернуть изображение */
	#define LCDMODE_S1D13781_REFOSC_MHZ	50	/* Частота генератора, установленного на контроллере дисплея */

#elif 0

	// not tested
	#define LCDMODE_ILI8961	1	/* 320 * 240 HHT270C-8961-6A6, RGB, ILI8961, use LCDMODE_LTDC_L24 */
	#define LCDMODE_V0	1	/* Обычная конфигурация без PIP с L8 на основном экране */

	//#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	//#define LCDMODE_LTDC_L24	1	/* 32-bit на пиксель в памяти (3 байта) */
	//#define LCDMODE_ILI8961_TOPDOWN	1

#elif 0

	#define LCDMODE_ILI9341	1	/* 320*240 SF-TC240T-9370-T с контроллером ILI9341 - STM32F4DISCO */
	//#define LCDMODE_ILI9341_TOPDOWN	1	/* LCDMODE_ILI9341 - перевернуть изображение (для выводов справа) */

#else
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

	//#define LCDMODE_ILI9225	1	/* Индикатор 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 с контроллером ILI9225С */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - перевернуть изображение (для выводов слева от экрана) */
	//#define LCDMODE_UC1608	1		/* Индикатор 240*128 с контроллером UC1608.- монохромный */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ST7735	1	/* Индикатор 160*128 с контроллером Sitronix ST7735 - TFT панель 160 * 128 HY-1.8-SPI */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - перевернуть изображение (для выводов справа) */
	//#define LCDMODE_ST7565S	1	/* Индикатор WO12864C2-TFH# 128*64 с контроллером Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - перевернуть изображение (для выводов сверху) */
	//#define LCDMODE_ILI9320	1	/* Индикатор 248*320 с контроллером ILI9320 */
	//#define LCDMODE_LS020 	1	/* Индикатор 176*132 Sharp LS020B8UD06 с контроллером LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - перевернуть изображение */
	//#define LCDMODE_LPH88		1	/* Индикатор 176*132 LPH8836-2 с контроллером Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - перевернуть изображение */
	//#define LCDMODE_ILI9163	1	/* Индикатор LPH9157-2 176*132 с контроллером ILITEK ILI9163 - Лента дисплея справа, а выводы слева. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - перевернуть изображение (для выводов справа, лента дисплея слева) */
	//#define LCDMODE_L2F50	1	/* Индикатор 176*132 с контроллером Epson L2F50126 */
	//#define LCDMODE_L2F50_TOPDOWN	1	/* Переворот изображени я в случае LCDMODE_L2F50 */
	// --- Одна из этих строк определяет тип дисплея, для которого компилируется прошивка
#endif

#if WITHISBOOTLOADER

	#define NVRAM_TYPE NVRAM_TYPE_NOTHING	// нет NVRAM
	#define HARDWARE_IGNORENONVRAM	1		// отладка на платах где нет никакого NVRAM

	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */
	//#define WITHWATCHDOG	1	/* разрешение сторожевого таймера в устройстве */
	//#define WITHSMPSYSTEM	1	/* разрешение поддержки SMP, Symmetric Multiprocessing */
	#define WITHNESTEDINTERRUPTS	1	/* используется при наличии real-time части. */
	#define WITHSPILOWSUPPORTT	1	/* Работа совместно с фоновым обменом SPI по прерываниям */
	//#define WITHUSEMALLOC	1	/* разрешение поддержки malloc/free/calloc/realloc */
	//#define WITHSPILOWSUPPORTT	1	/* Работа совместно с фоновым обменом SPI по прерываниям */
//	#define WITHFPGALOAD_PS	1	/* FPGA загружается процессором с помощью SPI */
//	//#define BOARD_BITIMAGE_NAME "rbf/rbfimage_v9m_velo.h"
//	#define BOARD_BITIMAGE_NAME_ZIP "rbf/rbfimage_v9m_velo_zip.h"
//	#define BOARD_BITIMAGE_NAME_COMPRESSED "t1_v9m_velo.rbf"	/* требуемый файл в архиве */
//	#define WITHKEYBOARD 1	/* в данном устройстве есть клавиатура */
//	#define KEYBOARD_USE_ADC	1	/* на одной линии установлено  четыре  клавиши. на vref - 6.8K, далее 2.2К, 4.7К и 13K. */
	#define WITHUSEMALLOC	1	/* разрешение поддержки malloc/free/calloc/realloc */
	#define FORMATFROMLIBRARY 	1

#else /* WITHISBOOTLOADER */

	//#define WITHUSBHEADSET	1	/* Функциональность USB микрофона */
	#define WITHUSEMALLOC	1	/* разрешение поддержки malloc/free/calloc/realloc */
	#define FORMATFROMLIBRARY 	1
	#define WITHRTS96 1		/* Получение от FPGA квадратур, возможно передача по USB и отображение спектра/водопада. */
	//#define WITHRTS192 1		/* Получение от FPGA квадратур, возможно передача по USB и отображение спектра/водопада. */
	#define WITHFQMETER	1	/* есть схема измерения опорной частоты, по внешнему PPS */
	//#define WITHKEEPNVRAM (1 && ! WITHDEBUG)		/* ослабить проверку совпадения версий прошивок для стирания NVRAM */

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	//#define WITHDIRECTFREQENER	1
	//#define WITHENCODER	1	/* для изменения частоты имеется енкодер */
	//#define ENCODER_REVERSE	1	/* разводка на плате с перепутаными фазами от валкодера */
	//#define ENCODER2_REVERSE	1	/* разводка на плате с перепутаными фазами от валкодера */
	//#define WITHENCODER2	1		/* есть второй валкодер */
	#define BOARD_ENCODER2_DIVIDE 4		/* значение для валкодера PEC16-4220F-n0024 (с трещёткой") */
	/* Board hardware configuration */
	//#define CODEC1_TYPE CODEC_TYPE_TLV320AIC23B
	//#define CODEC_TYPE_TLV320AIC23B_USE_SPI	1
	//#define CODEC_TYPE_TLV320AIC23B_USE_8KS	1	/* кодек работает с sample rate 8 kHz */
	#define CODEC1_IFC_MASTER 1	// кодек формирует синхронизацию

	//#define CODEC_TYPE_WM8731_USE_SPI	1
	//#define CODEC_TYPE_WM8731_USE_8KS	1	/* кодек работает с sample rate 8 kHz */

//	#define CODEC1_TYPE CODEC_TYPE_NAU8822L
//	#define CODEC_TYPE_NAU8822_USE_SPI	1
	//#define CODEC_TYPE_NAU8822_USE_8KS	1	/* кодек работает с sample rate 8 kHz */
	//#define CODEC1_IFC_MASTER 1	// кодек формирует синхронизацию

	//#define WITHDTMFPROCESSING 1
	//#define WITHBBOXMIKESRC BOARD_TXAUDIO_LINE

	//#define CODEC2_TYPE	CODEC_TYPE_FPGAV1	/* квадратуры получаем от FPGA */
	//#define CODEC_TYPE_CS4272_USE_SPI	1		// codecboard v2.0
	//#define CODEC_TYPE_CS4272_STANDALONE	1		// codecboard v3.0

	#define WITHFPGAIF_FRAMEBITS 512	// Полный размер фрейма = 512 для WITHFPGAPIPE_CODEC1
	//#define WITHFPGAIF_FRAMEBITS 256
	//#define WITHFPGARTS_FRAMEBITS 64	// Полный размер фрейма для двух квадратур по 24 бита - канал спектроанализатора
	#define WITHFPGAIF_FORMATI2S_PHILIPS 1	// требуется при получении данных от FPGA
	//#define WITHFPGARTS_FORMATI2S_PHILIPS 1	// требуется при получении данных от FPGA
	#define CODEC1_FORMATI2S_PHILIPS 1	// Возможно использование при передаче данных в кодек, подключенный к наушникам и микрофону
	#define CODEC1_FRAMEBITS 64		// Полный размер фрейма для двух каналов - канал кодека

	//#define WITHWATCHDOG	1	/* разрешение сторожевого таймера в устройстве */
	#define WITHSMPSYSTEM	1	/* разрешение поддержки SMP, Symmetric Multiprocessing */
	#define WITHNESTEDINTERRUPTS	1	/* используется при наличии real-time части. */
	#define WITHSPILOWSUPPORTT	1	/* Работа совместно с фоновым обменом SPI по прерываниям */
	//#define WITHINTEGRATEDDSP		1	/* в программу включена инициализация и запуск DSP части. */
	//#define WITHRTOS	1	/* Использование RTOS */

	//#define WITHIF4DSP	1			/*  "Дятел" */
	//#define WITHDACOUTDSPAGC		1	/* АРУ реализовано как выход ЦАП на аналоговую часть. */
	//
//	#define WITHDSPEXTDDC 1			/* Квадратуры получаются внешней аппаратурой */
//	#define WITHDSPEXTFIR 1			/* Фильтрация квадратур осуществляется внешней аппаратурой */
//	#define WITHDSPLOCALFIR 1		/* test: Фильтрация квадратур осуществляется процессором */
	#define WITHDACSTRAIGHT 1		/* Требуется формирование кода для ЦАП в режиме беззнакового кода */
	#define WITHTXCWREDUCE	1	/* для получения сравнимой выходной мощности в SSB и CW уменьшен уровень CW и добавлено усиление аналоговой части. */
	#define WITHDEFDACSCALE 100	/* 0..100: настраивается под прегруз драйвера. (ADT1-6T, 200 Ohm feedbask) */

	// FPGA section
	//#define WITHFPGAWAIT_AS	1	/* FPGA загружается из собственной микросхемы загрузчика - дождаться окончания загрузки перед инициализацией SPI в процессоре */
	//#define WITHFPGALOAD_PS	1	/* FPGA загружается процессором с помощью SPI */

	//#define BOARD_BITIMAGE_NAME "rbf/rbfimage_v9m_velo.h"	// для WITHFPGAPIPE_CODEC1
	//#define BOARD_BITIMAGE_NAME "rbf/rbfimage_v9d_2ch.h"
	//#define BOARD_BITIMAGE_NAME "rbf/rbfimage_v9d_2ch_pattern256.h"

	//#define WITHSKIPUSERMODE 1	// debug option: не отдавать в USER MODE блоки для фильтрации аудиосигнала
	//#define WITHNOSPEEX	1	// Без шумоподавителя SPEEX
	//#define WITHUSEDUALWATCH	1	// Второй приемник
	//#define WITHREVERB	1	// ревербератор в обработке микрофонного сигнала
	//#define WITHLOOPBACKTEST	1	/* прослушивание микрофонного входа, генераторов */
	//#define WITHMODEMIQLOOPBACK	1	/* модем получает собственные передаваемые квадратуры */

	//#define WITHUSEUSBBT		1	// Включение поддержки USB BT stick
	//#define WITHUSESDCARD		1	// Включение поддержки SD CARD
	//#define WITHUSEUSBFLASH		1	// Включение поддержки USB memory stick
	//#define WITHUSERAMDISK			1			// создание FATFS диска в озу
	//#define WITHUSERAMDISKSIZEKB	(192uL * 1024)	// размр в килобайтах FATFS диска в озу

	//#define WITHUSEAUDIOREC		1	// Запись звука на SD CARD
	//#define WITHUSEAUDIOREC2CH	1	// Запись звука на SD CARD в стерео
	//#define WITHUSEAUDIORECCLASSIC	1	// стандартный формат записи, без "дыр"
	//#define WITHDISPLAYSNAPSHOT 1	/* запись видимого изображения */

	//#define WITHFT8	1	/* Поддержка протокола FT8. Для фонового декодирования требуется минимум двухъядерный процессор и внешняя оперативная память */
	//#define WITHRTTY 1	/* подержка демодулятора RTTY */

	#if LCDMODE_AT070TNA2 || LCDMODE_AT070TN90
		#define BOARD_FFTZOOM_POW2MAX 3	// Возможные масштабы FFT x1, x2, x4, x8
		#define WITHFFTSIZEWIDE 1024		/* Отображение спектра и волопада */
//		#define WITHVIEW_3DSS		1
//		#define WITHVIEW_3DSS_MARK	1
		#define WITHSPECBETA_DEFAULT	30
//		#define WITHAFSPECTRE		1		/* показ спктра прослушиваемого НЧ сигнала. */
//		#define WITHFFTSIZEAF 		512		/* Отображение спектра НЧ сигнвлв */
		#if 0
			#define WITHTOUCHGUI		1
			#define WITHGUIDEBUG		1	/* вывод отладочной информации на экран по удержанию Break-In */
			#define WITHAFSPECTRE		1	/* показ спктра прослушиваемого НЧ сигнала. */
			#define WITHDISPLAY_FPS		30
			#define WITHDISPLAYSWR_FPS	30
			#define WITHALPHA			24
			#define FORMATFROMLIBRARY 	1
			#define WITHUSEMALLOC		1	/* разрешение поддержки malloc/free/calloc/realloc */
			#define WITHAFGAINDEFAULT	(150 * BOARD_AFGAIN_MAX / 255)
			//#define WITHTHERMOLEVEL	1
			#define WITHALTERNATIVEFONTS	1
			#define WITHAFEQUALIZER		1
			#define WITHALTERNATIVELAYOUT	1
			#define WITHRLEDECOMPRESS	1	/* поддержка вывода сжатых RLE изображений, пока что только для ARGB8888 видеобуфера */
			#define WITHDEFAULTVIEW		VIEW_3DSS
			#define WITHFT8					1
		#else
			#define WITHDISPLAY_FPS		15
			#define WITHDISPLAYSWR_FPS	15
		#endif
	#elif LCDMODE_LQ043T3DX02K
		#define BOARD_FFTZOOM_POW2MAX 3	// Возможные масштабы FFT x1, x2, x4, x8
		#define WITHFFTSIZEWIDE 512		/* Отображение спектра и волопада */
		#define WITHDISPLAYSWR_FPS 15
		#define WITHAFSPECTRE		1		/* показ спктра прослушиваемого НЧ сигнала. */
		#define WITHFFTSIZEAF 		512		/* Отображение спектра НЧ сигнвлв */
	#else
		#define BOARD_FFTZOOM_POW2MAX 3	// Возможные масштабы FFT x1, x2, x4, x8
		#define WITHFFTSIZEWIDE 1024		/* Отображение спектра и волопада */
		#define WITHDISPLAYSWR_FPS 15
		//#define WITHAFSPECTRE		1		/* показ спктра прослушиваемого НЧ сигнала. */
		#define WITHFFTSIZEAF 		512		/* Отображение спектра НЧ сигнвлв */
	#endif /* LCDMODE_AT070TNA2 || LCDMODE_AT070TN90 */


	#if 0
		#define WITHUSBHEADSET 1	/* трансивер работает USB гарнитурой для компьютера - режим тестирования */
		#define WITHBBOX	1	// Black Box mode - устройство без органов управления
		#define WITHBBOXMIKESRC	BOARD_TXAUDIO_USB
	#elif 0
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
		//#define WITHBBOXMIKESRC	BOARD_TXAUDIO_2TONE
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
	#elif 0
		#define WITHBBOX	1	// Black Box mode - устройство без органов управления
		#define WITHBBOXFREQ	7030000L		// частота после включения
		#define WITHBBOXSUBMODE	SUBMODE_CWZ	// единственный режим работы
		#define WITHBBOXTX		1		// автоматический переход на передачу
		//#define WITHBBOXMIKESRC BOARD_TXAUDIO_2TONE
	#endif


	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	//#define WITHRFSG	1	/* включено управление ВЧ сигнал-генератором. */
	//#define WITHTX		1	/* включено управление передатчиком - сиквенсор, электронный ключ. */
	#if 0
		#define WITHAUTOTUNER_UA1CEI_V2 1	/* Есть функция автотюнера */
	#elif 0
		/* TUNER & PA board 2*RD16 by avbelnn@yandex.ru */
		#define WITHAUTOTUNER	1	/* Есть функция автотюнера */
		#define SHORTSET8	1
		#define WITHAUTOTUNER_AVBELNN	1	/* Плата управления LPF и тюнером от avbelnn */
		#define WITHANTSELECT	1	/* Управление переключением антенн */
	#elif 0
		/* TUNER by R3KBL */
		#define WITHAUTOTUNER	1	/* Есть функция автотюнера */
		#define SHORTSET7	1
	#else
		#define WITHSWRPROT 0	/* отключаем защиту по КСВ */
	#endif
	#define WITHNOTXDACCONTROL	1	/* в этой версии нет ЦАП управления смещением TXDAC передатчика */


	//#define WITHIFSHIFT	1	/* используется IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* Начальное занчение IF SHIFT */
	//#define WITHPBT		1	/* используется PBT (если LO3 есть) */
	//#define WITHCAT		1	/* используется CAT */
	//#define WITHMODEM		1	/* Устройство работает как радиомодем с последовательным интерфейсом */
	//#define WITHFREEDV	1	/* поддержка режима FreeDV - http://freedv.org/ */
	//
	//#define WITHBEACON	1	/* Используется режим маяка */
	#if WITHTX
		#define WITHVOX			1	/* используется VOX */
		#define WITHSHOWSWRPWR 1	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		#define WITHSWRMTR	1		/* Измеритель КСВ */
	#endif /* WITHTX */
	//#define WITHPWRMTR	1	/* Индикатор выходной мощности или */
	//#define WITHPWRLIN	1	/* Индикатор выходной мощности показывает напряжение а не мощность */
	//#define WITHBARS		1	/* отображение S-метра и SWR-метра */
	//#define WITHSWLMODE	1	/* поддержка запоминания множества частот в swl-mode */
	#define WITHVIBROPLEX	1	/* возможность эмуляции передачи виброплексом */
	#define WITHSPKMUTE		1	/* управление выключением динамика */
	#define WITHDATAMODE	1	/* управление с клавиатуры передачей с USB AUDIO канала */
	// Есть ли регулировка параметров потенциометрами
	////#define WITHPOTWPM		1	/* используется регулировка скорости передачи в телеграфе потенциометром */
//	#define WITHPOTIFGAIN		1	/* регуляторы усиления ПЧ на потенциометрах */
//	#define WITHPOTAFGAIN		1	/* регуляторы усиления НЧ на потенциометрах */
	//#define WITHPOTPOWER	1	/* регулятор мощности на потенциометре */
	//#define WITHPOTNFMSQL 1		/* NFM SQUELCH */
	//#define WITHANTSELECT	1	// Управление переключением антенн

	//#define WITHMENU 	1	/* функциональность меню может быть отключена - если настраивать нечего */

	//#define WITHONLYBANDS 1		/* Перестройка может быть ограничена любительскими диапазонами */
	//#define WITHBCBANDS		1		/* в таблице диапазонов присутствуют вещательные диапазоны */
	#define WITHWARCBANDS	1	/* В таблице диапазонов присутствуют HF WARC диапазоны */
	//#define WITHLO1LEVELADJ		1	/* включено управление уровнем (амплитудой) LO1 */
	//#define WITHLFM		1	/* LFM MODE */
	//#define LFMTICKSFREQ ARMI2SRATE
	//#define WITHTEMPSENSOR	1	/* отображение данных с датчика температуры */
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

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* разрядность FTW выбранного DDS */

	#define MODEL_DIRECT	1	/* использовать прямой синтез, а не гибридный */
	/* Board hardware configuration */
	//#define DDS1_TYPE DDS_TYPE_FPGAV1
	//#define DDS1_TYPE DDS_TYPE_FPGA_NONE
	//#define RTC1_TYPE RTC_TYPE_DS1305	/* MAXIM DS1305EN RTC clock chip with SPI interface */
	//#define RTC1_TYPE RTC_TYPE_M41T81	/* ST M41T81M6 RTC clock chip with I2C interface */
	//#define RTC1_TYPE RTC_TYPE_STM32F4xx	/* STM32F4xx/STM32F7xx internal RTC peripherial */
	//#define WITHRTCLSI	1				/* тестирование без кварца 32.768 кГц */

	//#define TSC1_TYPE TSC_TYPE_TSC2046	/* Resistive touch screen controller TI TSC2046 */
	//#define TSC1_TYPE TSC_TYPE_STMPE811	/* touch screen controller */
	//#define TSC_TYPE_STMPE811_USE_SPI	1
	//#define WITH_STMPE811_INTERRUPTS	1
	//#define TSC1_TYPE	TSC_TYPE_GT911		/* Capacitive touch screen with controller Goodix GT911 */
	//#define WITH_GT911_INTERRUPTS	1
	//#define TSC1_TYPE TSC_TYPE_XPT2046	/* touch screen controller XPTEK XPT2046 */
	//#define DAC1_TYPE	99999		/* наличие ЦАП для подстройки тактовой частоты */

	//#define BOARD_TSC1_XMIRROR 1	// Зеркалируем тачскрин по горизонтали.
	//#define BOARD_TSC1_YMIRROR 1	// Зеркалируем тачскрин по вертикали.

	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */

	//#define WITHWFM	1			/* используется WFM */

	#define WITHCATEXT	1	/* Расширенный набор команд CAT */
	//#define WITHELKEY	1
	//#define WITHKBDENCODER 1	// перестройка частоты кнопками

	//#define WITHKEYBOARD 1	/* в данном устройстве есть клавиатура */
	#define KEYBOARD_USE_ADC	1	/* на одной линии установлено  четыре  клавиши. на vref - 6.8K, далее 2.2К, 4.7К и 13K. */

#endif /* WITHISBOOTLOADER */

	#define WITHMODESETFULLNFM 1

	/* все возможные в данной конфигурации фильтры */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1 /* | IF3_FMASK_6P0 | IF3_FMASK_8P0*/)
	/* все возможные в данной конфигурации фильтры для передачи */
	#define IF3_FMASKTX	(IF3_FMASK_3P1 /*| IF3_FMASK_6P0 */)
	/* фильтры, для которых стоит признак HAVE */
	#define IF3_FHAVE	( IF3_FMASK_0P5 | IF3_FMASK_3P1 /*| IF3_FMASK_6P0 | IF3_FMASK_8P0*/)

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
	#if WITHPOTNFMSQL
		POTNFMSQL = IFGAIN_IXI,
	#endif /* WITHPOTNFMSQL */

	#if WITHREFSENSOR
		VREFIX = 17,		// Reference voltage
	#endif /* WITHREFSENSOR */
	#if WITHTEMPSENSOR
		TEMPIX = BOARD_ADCX1IN(6),
	#endif /* WITHTEMPSENSOR */

	#if WITHPOTWPM
		POTWPM = WPM_POTIX,			// PA6 потенциометр управления скоростью передачи в телеграфе
	#endif /* WITHPOTWPM */
	#if WITHPOTPOWER
		POTPOWER = WPM_POTIX,			// регулировка мощности
	#endif /* WITHPOTPOWER */

	//#define WITHALTERNATIVEFONTS    1

	#if WITHAUTOTUNER_UA1CEI_V2
		#define WITHAUTOTUNER	1	/* Есть функция автотюнера */
		// UA1CEI PA board: MCP3208 at targetext2 - P2_0 external SPI device (PA BOARD ADC)
		// UA1CEI PA board: MCP3208 at targetext2 - P2_0 external SPI device (PA BOARD ADC)
		//#define WITHTXCWREDUCE	1	/* для получения сравнимой выходной мощности в SSB и CW уменьшен уровень CW и добавлено усиление аналоговой части. */
		#define WITHCURRLEVEL	1	/* отображение тока оконечного каскада */
		#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
		#define WITHPACLASSA	1	/* усилитель мощности поддерживает переключение в класс А */
		#define WITHPOWERTRIMCLASSA 100	// Значение для работы в классе A
		#define WITHTHERMOLEVEL	1	/* отображение данных с датчика температуры */
		#define WITHANTSELECTRX	1	/* Управление переключением антенн и приемной антенны */

		#define SHORTSET_7L8C	1	/* 7 indictors, 8 capacitors */
		//#define FULLSET_7L8C	1	/* 7 indictors, 8 capacitors */

		#define WITHCURRLEVEL_ACS712_30A 1	// PA current sense - ACS712ELCTR-30B-T chip

		FWD = BOARD_ADCX2IN(0),
		REF = BOARD_ADCX2IN(1),
		PWRI = FWD,

		#define WITHCURRLEVEL2	1	/* отображение тока оконечного каскада */
		PASENSEIX2 = BOARD_ADCX2IN(2),	// DRAIN
		PAREFERIX2 = BOARD_ADCX2IN(3),	// reference (1/2 питания ACS712ELCTR-30B-T).

		#if WITHTHERMOLEVEL
			XTHERMOIX = BOARD_ADCX1IN(6),		// MCP3208 CH6 Exernal thermo sensor ST LM235Z
		#endif /* WITHTHERMOLEVEL */
		#if WITHVOLTLEVEL
			VOLTSOURCE = BOARD_ADCXKINxxx(7),		// Средняя точка делителя напряжения, для АКБ
		#endif /* WITHVOLTLEVEL */

		// ST LM235Z
		#define THERMOSENSOR_UPPER		0	// 4.7 kOhm - верхний резистор делителя датчика температуры
		#define THERMOSENSOR_LOWER		10	// 1 kOhm - нижний резистор
		#define THERMOSENSOR_OFFSET 	(- 2730)		// 2.98 volt = 25 Celsius, 10 mV/C
		#define THERMOSENSOR_DENOM	 	1			// миливольты к десятым долям градуса 2.98 volt = 25 Celsius

	#elif WITHAUTOTUNER_AVBELNN

		XTHERMOIX = BOARD_ADCX1IN(6),		// MCP3208 CH6 Exernal thermo sensor ST LM235Z

		#define WITHVOLTLEVEL	1	/* отображение напряжения питания */
		#define WITHCURRLEVEL	1	/* отображение тока оконечного каскада */

		#define WITHCURRLEVEL_ACS712_30A 1	// PA current sense - ACS712ELCTR-30B-T chip
		//#define WITHCURRLEVEL_ACS712_20A 1	// PA current sense - ACS712ELCTR-20B-T chip
		PASENSEIX = WPM_POTIX,		// PA1 PA current sense - ACS712-05 chip
		//PASENSEIX = 2,		// PA1 PA current sense - ACS712-05 chip

		#if WITHSWRMTR
			FWD = BOARD_ADCX1IN(3), REF = BOARD_ADCX1IN(4),	// MCP3208 CH5, CH4 Детектор прямой, отраженной волны
			PWRI = FWD,
		#endif /* WITHSWRMTR */

		VOLTSOURCE = BOARD_ADCX1IN(7),		// MCP3208 CH7 Средняя точка делителя напряжения, для АКБ

	#elif 0
		// UA1CEI PA board: MCP3208 at targetext2 - P2_0 external SPI device (PA BOARD ADC)
		VOLTSOURCE = BOARD_ADCX2INxx(4),		// MCP3208 CH7 Средняя точка делителя напряжения, для АКБ

		FWD = BOARD_ADCX2INxx(3),
		REF = BOARD_ADCX2INxx(2),
		PWRI = FWD,

		#define WITHCURRLEVEL2	1	/* отображение тока оконечного каскада */
		PASENSEIX2 = BOARD_ADCX2INxx(0),	// DRAIN
		PAREFERIX2 = BOARD_ADCX2INxx(1),	// reference (1/2 питания ACS712ELCTR-30B-T).
	#else
		// толькло основная плата - 5W усилитель

		#define WITHCURRLEVEL	1	/* отображение тока оконечного каскада */
		#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
		//#define WITHTHERMOLEVEL	1	/* отображение температуры */

		#if WITHCURRLEVEL
			PASENSEIX = BOARD_ADCX1IN(6),		// MCP3208 CH6 PA current sense - ACS712-05 chip
		#endif /* WITHCURRLEVEL */
		#if WITHVOLTLEVEL
			VOLTSOURCE = BOARD_ADCX1IN(7),		// Средняя точка делителя напряжения, для АКБ
		#endif /* WITHVOLTLEVEL */

		#if WITHTHERMOLEVEL
			XTHERMOIX = BOARD_ADCX1IN(6),		// Exernal thermo sensor ST LM235Z
		#endif /* WITHTHERMOLEVEL */

		#if WITHSWRMTR
			FWD = BOARD_ADCX1IN(4), REF = BOARD_ADCX1IN(5),	// MCP3208 CH5, CH4 Детектор прямой, отраженной волны
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

		KI0 = BOARD_ADCXKIN(0), 	// клавиатура на АЦП MCP3208
		KI1 = BOARD_ADCXKIN(1),
		KI2 = BOARD_ADCXKIN(2),
		KI3 = BOARD_ADCXKIN(3),
		KI4 = BOARD_ADCXKIN(4)
	};

	#define KI_COUNT 5	// количество используемых под клавиатуру входов АЦП
	#define KI_LIST	KI4, KI3, KI2, KI1, KI0,	// инициализаторы для функции перекодировки

	#define BOARDPOWERMIN	0	// Нижний предел регулировки (показываемый на дисплее)
	#define BOARDPOWERMAX	100	// Верхний предел регулировки (показываемый на дисплее)

#endif /* ARM_ALW_T507_CTL_XHELPERBOARD_H_INCLUDED */
