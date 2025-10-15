/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Трансивер с DSP обработкой "Аист" на процессоре STM32MP1xx
// rmainunit_v5km7.pcb STM32MP1xx, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N

#ifndef ARM_ALLWT128S3_CTLSTYLE_MANGO_PI_H_INCLUDED
#define ARM_ALLWT128S3_CTLSTYLE_MANGO_PI_H_INCLUDED 1

	#define WITHBRANDSTR "Falcon"

	//#define WITHSAICLOCKFROMI2S 1	/* Блок SAI1 тактируется от PLL I2S */
	// в данной конфигурации I2S и SAI - в режиме SLAVE
	#define WITHI2SCLOCKFROMPIN 1	// тактовая частота на SPI2 (I2S) подается с внешнего генератора, в процессор вводится через MCK сигнал интерфейса

	#define LSEFREQ 32768uL

	#define WITHCPUXTAL 24000000u	/* На процессоре установлен кварц 24.000 МГц */
	//#define WITHCPUXOSC 24000000u	/* На процессоре установлен генератор 24.000 МГц */

	//#define RV_PLL_CPU_N 42	/* 24 MHz * 42 = 1008 MHz */
	#define RV_PLL_CPU_N 50	/* 24 MHz * 50 = 1200 MHz */
	#define PLL_CPU_N 50	/* 24 MHz * 50 = 1200 MHz https://linux-sunxi.org/T113-s3 say about 1.2 GHz */

	#if 1//WITHISBOOTLOADER
		// Варианты конфигурации тактирования
		// ref1_ck, ref2_ck - 8..16 MHz
		// PLL1, PLL2 VCOs
		#if WITHCPUXTAL || WITHCPUXOSC


		#else

		#endif
	#endif /* WITHISBOOTLOADER */

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

	//#define CTLREGMODE_MANGO_PI	1	/* STM32MP157, дополнения для подключения трансвертора */
	#define CTLREGMODE_NOCTLREG 1

	
	
	

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
#if WITHISBOOTLOADER

	#define LCDMODE_DUMMY	1
	#define BOARD_DSTYLE "g_dummy.h"

#elif 0

	#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */

	#define LCDMODE_RGB565 1	/* Экран 16 бит */

	#define WITHFLATLINK 1	/* Работа с TFT панелью через LVDS интерфейс	*/
	
	//#define WITHMIPIDSISHW 1	/* MIPI-DSI display support */
	#define BOARD_DSTYLE "g_x480_y272_spectrum_notouch.h"

#elif 0

	#define LCDMODE_AT070TN90 1	/* AT070TN90 panel (800*480) - 7" display */

	//#define LCDMODE_PALETTE256 1	/* Экран 8 бит, L8 */
	//#define LCDMODE_RGB565 1	/* Экран 16 бит */
	#define LCDMODE_ARGB8888	1	/* Экран 32 бит ARGB8888 */

	#define WITHLCDDEMODE	1	/* DE MODE: MODE="1", VS and HS must pull high. */
	#define WITHFLATLINK 1	/* Работа с TFT панелью через LVDS интерфейс	*/
	//#define WITHMIPIDSISHW 1	/* MIPI-DSI display support */
	#define WITHMIPIDSISHW_LANES 2	/* mipi-dsi port lanes number */

	#define BOARD_DSTYLE "g_x800_y480.h"
	//#define BOARD_DSTYLE_LVGL "g_x800_y480_lvgl.h"
	//#define BOARD_DSTYLE "g_x800_y480_veloci_v0.h"

#elif 1

	#define LCDMODE_AT070TNA2 1	/* AT070TNA2 panel (1024*600) - 7" display */

	#define LCDMODE_RGB565 1	/* Экран 16 бит */
	//#define LCDMODE_ARGB8888	1	/* Экран 32 бит ARGB8888 */


	#define WITHLCDDEMODE	1	/* DE MODE: MODE="1", VS and HS must pull high. */
	#define WITHFLATLINK 1	/* Работа с TFT панелью через LVDS интерфейс	*/
	//#define WITHMIPIDSISHW 1	/* MIPI-DSI display support */
	#define WITHMIPIDSISHW_LANES 2	/* mipi-dsi port lanes number */
	#define BOARD_DSTYLE "g_x1024_y600.h"


#elif 1

	#define LCDMODE_LQ123K3LG01 1	/* LQ123K3LG01 panel (1280*480) - 12.3" display LVDS mode */

	//#define LCDMODE_PALETTE256 1	/* Экран 8 бит, L8 */
	#define LCDMODE_RGB565 1	/* Экран 16 бит */
	//#define LCDMODE_ARGB8888	1	/* Экран 32 бит ARGB8888 */

	#define WITHLCDDEMODE	1	/* DE MODE: MODE="1", VS and HS must pull high. */
	#define WITHFLATLINK 1	/* Работа с TFT панелью через LVDS интерфейс	*/
	//#define WITHMIPIDSISHW 1	/* MIPI-DSI display support */
	#define WITHMIPIDSISHW_LANES 2	/* mipi-dsi port lanes number */

	#define BOARD_DSTYLE "g_x800_y480.h"
	#define BOARD_DSTYLE_LVGL "g_x800_y480_lvgl.h"
	//#define BOARD_DSTYLE "g_x800_y480_veloci_v0.h"

#elif 1

	#define LCDMODE_TCG104XGLPAPNN 1	/* TCG104XGLPAPNN-AN30 panel (1024*768) - 10.4" display - DE mode required */
	#define LCDMODE_RGB565 1	/* Экран 16 бит */
	
	//#define WITHMIPIDSISHW 1	/* MIPI-DSI display support */
	#define WITHLCDDEMODE	1	/* DE MODE: MODE="1", VS and HS must pull high. */
	//#define WITHMIPIDSISHW 1	/* MIPI-DSI display support */
	#define WITHMIPIDSISHW_LANES 4	/* mipi-dsi port lanes number */
	#define BOARD_DSTYLE "g_x1024_y600.h"

#elif 0

	#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	#define LCDMODE_S1D13781	1	/* Инндикатор 480*272 с контроллером Epson S1D13781 */
	
	
	#define LCDMODE_S1D13781_REFOSC_MHZ	50	/* Частота генератора, установленного на контроллере дисплея */

#elif 0

	// not tested
	#define LCDMODE_ILI8961	1	/* 320 * 240 HHT270C-8961-6A6, RGB, ILI8961, use LCDMODE_LTDC_L24 */
	#define LCDMODE_V0	1	/* Обычная конфигурация без PIP с L8 на основном экране */

	//#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	//#define LCDMODE_LTDC_L24	1	/* 32-bit на пиксель в памяти (3 байта) */
	//#define LCDMODE_ILI8961_TOPDOWN	1

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
	//#define WITHWATCHDOG	1	/* разрешение сторожевого таймера в устройстве */
	//#define WITHSMPSYSTEM	1	/* разрешение поддержки SMP, Symmetric Multiprocessing */
	//#define WITHSPILOWSUPPORTT	1	/* Работа совместно с фоновым обменом SPI по прерываниям */
	
	//#define WITHSPILOWSUPPORTT	1	/* Работа совместно с фоновым обменом SPI по прерываниям */

	#define WITHUSESDCARD		1	// Включение поддержки SD CARD - загрузчик работает через eMMC
	#define WITHISBOOTLOADERRAWDISK	1	// чтение application с предопределённого смещения на накопителе
	#define WITHISBOOTLOADERRAWDISK_DEV 0	// device для FatFS diskio

#else /* WITHISBOOTLOADER */


	#define BOARD_AGCCODE_OFF 0
	#define BOARD_AGCCODE_ON 0
	
	/* Board hardware configuration */
	//#define CODEC1_TYPE CODEC_TYPE_AWHWCODEC
	//#define CODEC1_TYPE CODEC_TYPE_TLV320AIC23B
	//#define CODEC_TYPE_TLV320AIC23B_USE_SPI	1
	//#define CODEC_TYPE_TLV320AIC23B_USE_8KS	1	/* кодек работает с sample rate 8 kHz */
	//#define CODEC1_IFC_MASTER 1	// кодек формирует синхронизацию

	//#define CODEC_TYPE_WM8731_USE_SPI	1
	//#define CODEC_TYPE_WM8731_USE_8KS	1	/* кодек работает с sample rate 8 kHz */

	//#define CODEC1_TYPE CODEC_TYPE_NAU8822L
	//#define CODEC_TYPE_NAU8822_USE_SPI	1
	//#define CODEC_TYPE_NAU8822_USE_8KS	1	/* кодек работает с sample rate 8 kHz */
	//#define CODEC1_IFC_MASTER 1	// кодек формирует синхронизацию

	
	//#define WITHBBOXMIKESRC BOARD_TXAUDIO_LINE

	//#define CODEC2_TYPE	CODEC_TYPE_FPGAV1	/* квадратуры получаем от FPGA */
	//#define CODEC_TYPE_CS4272_USE_SPI	1		// codecboard v2.0
	//#define CODEC_TYPE_CS4272_STANDALONE	1		// codecboard v3.0

	#define WITHFPGAIF_FRAMEBITS 256	// Полный размер фрейма
	//#define WITHFPGARTS_FRAMEBITS 64	// Полный размер фрейма для двух квадратур по 24 бита - канал спектроанализатора
	#define WITHFPGAIF_FORMATI2S_PHILIPS 1	// требуется при получении данных от FPGA
	//#define WITHFPGARTS_FORMATI2S_PHILIPS 1	// требуется при получении данных от FPGA
	#define CODEC1_FORMATI2S_PHILIPS 1	// Возможно использование при передаче данных в кодек, подключенный к наушникам и микрофону
	#define CODEC1_FRAMEBITS 64		// Полный размер фрейма для двух каналов - канал кодека

	//#define WITHWATCHDOG	1	/* разрешение сторожевого таймера в устройстве */
	#if CPUSTYLE_T113
		#define WITHSMPSYSTEM	1	/* разрешение поддержки SMP, Symmetric Multiprocessing */
	#endif /* CPUSTYLE_T113 */
	//#define WITHSPILOWSUPPORTT	1	/* Работа совместно с фоновым обменом SPI по прерываниям */

	//#define WITHUSEUSBBT		1	// Включение поддержки USB BT stick
	#define WITHUSESDCARD		1	// Включение поддержки SD CARD/ eMMC
	//#define WITHUSEUSBFLASH		1	// Включение поддержки USB memory stick
	//#define WITHUSERAMDISK			1			// создание FATFS диска в озу
	//#define WITHUSERAMDISKSIZEKB	(16uL * 1024)	// размр в килобайтах FATFS диска в озу

	#define FORMATFROMLIBRARY 	1
	//#define WITHUSBHEADSET	1


	// +++ Эти строки можно отключать, уменьшая функциональность готового изделия
	
	// --- Эти строки можно отключать, уменьшая функциональность готового изделия

	#if 0
		#define WITHLWIP 1
	#endif
	#if 1
		#define WITHLVGL 1		/* bare-metal config of LVGL */
		//#define WITHLVGLINDEV 1	/* обработку событий от органов управления делает LVGL */
	#endif

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
	//#define RTC1_TYPE RTC_TYPE_STM32F4xx	/* STM32F4xx/STM32F7xx internal RTC peripherial */
	//#define WITHRTCLSI	1				/* тестирование без кварца 32.768 кГц */

	
	//#define TSC1_TYPE TSC_TYPE_STMPE811	/* touch screen controller */
	//#define TSC1_TYPE TSC_TYPE_AWTPADC	/* Allwinner F133/t113-s3 resistive touch screen controller */
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

	#if WITHPOTWPM
		POTWPM = 6,			// PA6 потенциометр управления скоростью передачи в телеграфе
	#endif /* WITHPOTWPM */
	#if WITHPOTPOWER
		POTPOWER = 6,			// регулировка мощности
	#endif /* WITHPOTPOWER */

	//#define WITHALTERNATIVEFONTS    1

	#if WITHTPA100W_UA1CEI_V2

		#define WITHCURRLEVEL	1	/* отображение тока оконечного каскада */
		#define WITHVOLTLEVEL	1	/* отображение напряжения АКБ */
		#define WITHPACLASSA	1	/* усилитель мощности поддерживает переключение в класс А */
		
		#define WITHTHERMOLEVEL	1	/* отображение данных с датчика температуры */
		#define WITHANTSELECTRX	1	/* Управление переключением антенн и приемной антенны */

		#define WITHAUTOTUNER_N7DDCALGO	1	/* Есть функция автотюнера по алгоритму N7DDC */
		#define FULLSET_7L8C	1	/* 7 indictors, 8 capacitors */

		#define WITHCURRLEVEL_ACS712_30A 1	// PA current sense - ACS712ELCTR-30B-T chip

		FWD = BOARD_ADCX2IN(0),
		REF = BOARD_ADCX2IN(1),
		

		#define WITHCURRLEVEL2	1	/* отображение тока оконечного каскада */
		PASENSEIX2 = BOARD_ADCX2IN(2),	// DRAIN
		PAREFERIX2 = BOARD_ADCX2IN(3),	// reference (1/2 питания ACS712ELCTR-30B-T).

		#if WITHTHERMOLEVEL
			XTHERMOIX = BOARD_ADCXIN(6),		// MCP3208 CH6 Exernal thermo sensor ST LM235Z
		#endif /* WITHTHERMOLEVEL */
		#if WITHVOLTLEVEL
			VOLTSOURCE = BOARD_ADCX1IN(7),		// Средняя точка делителя напряжения, для АКБ
		#endif /* WITHVOLTLEVEL */

	#elif WITHAUTOTUNER_AVBELNN

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
			
		#endif /* WITHSWRMTR */

		VOLTSOURCE = BOARD_ADCX1IN(7),		// MCP3208 CH7 Средняя точка делителя напряжения, для АКБ

	#elif 0
		// UA1CEI PA board: MCP3208 at targetext2 - P2_0 external SPI device (PA BOARD ADC)
		VOLTSOURCE = BOARD_ADCX2IN(4),		// MCP3208 CH7 Средняя точка делителя напряжения, для АКБ

		FWD = BOARD_ADCX2IN(3),
		REF = BOARD_ADCX2IN(2),
		

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
			
		#endif /* WITHSWRMTR */
	#endif

		XTHERMOMRRIX = BOARD_ADCMRRIN(0),	// кеш - индекc не должен повторяться в конфигурации
		PASENSEMRRIX = BOARD_ADCMRRIN(1),	// кеш - индекc не должен повторяться в конфигурации
		REFMRRIX = BOARD_ADCMRRIN(2),
		FWDMRRIX = BOARD_ADCMRRIN(3),
		PWRMRRIX = BOARD_ADCMRRIN(7),
		VOLTMRRIX = BOARD_ADCMRRIN(4),	// кеш - индекc не должен повторяться в конфигурации
		PASENSEMRRIX2 = BOARD_ADCMRRIN(5),		// кеш - индекc не должен повторяться в конфигурации
		PAREFERMRRIX2 = BOARD_ADCMRRIN(6),		// кеш - индекc не должен повторяться в конфигурации

		KI0 = 0, KI1 = 1, KI2 = 2, KI3 = 7, KI4 = 10	// клавиатура - PA0, PA1, PA2, PA7, PC0
	};

	#define KI_COUNT 5	// количество используемых под клавиатуру входов АЦП
	#define KI_LIST	KI4, KI3, KI2, KI1, KI0,	// инициализаторы для функции перекодировки

	#define BOARDPOWERMIN	0	// Нижний предел регулировки (показываемый на дисплее)
	#define BOARDPOWERMAX	100	// Верхний предел регулировки (показываемый на дисплее)

#endif /* ARM_ALLWT128S3_CTLSTYLE_MANGO_PI_H_INCLUDED */
