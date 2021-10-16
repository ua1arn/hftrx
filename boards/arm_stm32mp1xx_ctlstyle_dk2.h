/* $Id$ */
/* board-specific CPU attached signals */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// https://www.st.com/en/evaluation-tools/stm32mp157c-dk2.html
// STM32MP157C-DK2 & 1024x600 HDMI


#ifndef ARM_STM32MP1_LFBGA354_CTLSTYLE_DK2_H_INCLUDED
#define ARM_STM32MP1_LFBGA354_CTLSTYLE_DK2_H_INCLUDED 1


	#if 0
		// Обновление загрузчика
		#define WITHHTTPUPDATE 1	// не совсестимо с работой с диском приложения
        #define ZAGRUPDATE
		// Адреса работы зщагрузчика
		#define SDCARDLOCATION FSBL1FIRSTSECTOR	// пришем FSBL #1
		//#define SDCARDLOCATION FSBL2FIRSTSECTOR	// пришем FSBL #2
	#else
		// нормальнная работа
		// Адреса работы зщагрузчика
		#define SDCARDLOCATION APPFIRSTSECTOR	// пришем приложение
	#endif

	#if ! defined(STM32MP157Axx) && ! defined(STM32MP153Dxx)
		#error Wrong CPU selected. STM32MP157Axx expected
	#endif /* ! defined(STM32MP157Axx) */

	#define WITHUSEPLL		1	/* Главная PLL	*/
	#define WITHUSEPLL1		1	/* PLL1 - MPU, AXI	*/
	#define WITHUSEPLL2		1	/* PLL2 - GPU, DDR	*/
	//#define WITHUSEPLL3		1	/* PLL3 - для LTDC на STM32H743xx	*/
	#define WITHUSEPLL4		1	/* PLL4 - для LTDC & USBPHY	*/
	//#define WITHUSESAIPLL	1	/* SAI PLL	*/
	//#define WITHUSESAII2S	1	/* I2S PLL	*/

	#define LSEFREQ 32768uL
	// Варианты конфигурации тактирования
	// ref1_ck, ref2_ck - 8..16 MHz
	// PLL1, PLL2 VCOs
	//#define WITHCPUXTAL 24000000uL	/* На процессоре установлен кварц 24.000 МГц */
	#define WITHCPUXOSC 24000000uL	/* На процессоре установлен генератор 24.000 МГц */

	//	In addition, if the USBO is used in full-speed mode only, the application can choose the
	//	48 MHz clock source to be provided to the USBO:
	// USBOSRC
	//	0: pll4_r_ck clock selected as kernel peripheral clock (default after reset)
	//	1: clock provided by the USB PHY (rcc_ck_usbo_48m) selected as kernel peripheral clock
	// USBPHYSRC
	//  0x0: hse_ker_ck clock selected as kernel peripheral clock (default after reset)
	//  0x1: pll4_r_ck clock selected as kernel peripheral clock
	//  0x2: hse_ker_ck/2 clock selected as kernel peripheral clock
	#define RCC_USBCKSELR_USBOSRC_VAL 1
	#define RCC_USBCKSELR_USBPHYSRC_VAL 0

	#if WITHISBOOTLOADER
		// Варианты конфигурации тактирования
		// ref1_ck, ref2_ck - 8..16 MHz
		// PLL1, PLL2 VCOs
		#if WITHCPUXTAL || WITHCPUXOSC

			// PLL1_1600
			#define PLL1DIVM	2	// ref1_ck = 12 MHz
			#define PLL1DIVP	1	// MPU
			#define PLL1DIVQ	2
			#define PLL1DIVR	2

			//#define PLL1DIVN	54	// 12*54 = 648 MHz
			//#define PLL1DIVN	66	// 12*66 = 792 MHz
			#define PLL1DIVN	(stm32mp1_overdrived() ? 66 : 54)	// Auto select

			// PLL2_1600
	#if 1
			#define PLL2DIVM	2	// ref2_ck = 12 MHz
			#define PLL2DIVN	44	// 528 MHz Valid division rations for DIVN: between 25 and 100
			#define PLL2DIVP	2	// AXISS_CK div2=minimum 528/2 = 264 MHz PLL2 selected as AXI sub-system clock (pll2_p_ck) - 266 MHz max for all CPU revisions
			#define PLL2DIVQ	1	// GPU clock divider = 528 MHz - 533 MHz max for all CPU revisions
			#define PLL2DIVR	1	// DDR clock divider = 528 MHz

			#include "src/sdram/stm32mp15-mx_4G.dtsi"		// 256k*16
	#elif 0
			#define PLL2DIVM	2	// ref2_ck = 12 MHz
			#define PLL2DIVN	66	// 528 MHz Valid division rations for DIVN: between 25 and 100
			#define PLL2DIVP	3	// AXISS_CK div2=minimum 528/2 = 264 MHz PLL2 selected as AXI sub-system clock (pll2_p_ck) - 266 MHz max for all CPU revisions
			#define PLL2DIVQ	2	// GPU clock divider = 528 MHz - 533 MHz max for all CPU revisions
			#define PLL2DIVR	2	// DDR clock divider = 528 MHz

			#include "src/sdram/stm32mp15-mx_4G.dtsi"		// 256k*16
	#else
			/* bad boards DDR3 clock = 300 MHz */
			#define PLL2DIVM	2	// ref2_ck = 12 MHz
			#define PLL2DIVN	50	// 600 MHz Valid division rations for DIVN: between 25 and 100
			#define PLL2DIVP	3	// AXISS_CK div2=minimum 1056/4 = 200 MHz PLL2 selected as AXI sub-system clock (pll2_p_ck) - 266 MHz max for all CPU revisions
			#define PLL2DIVQ	2	// GPU clock divider = 300 MHz - 533 MHz max for all CPU revisions
			#define PLL2DIVR	2	// DDR clock divider = 300 MHz

			#include "src/sdram/stm32mp15-mx_300MHz_4G.dtsi"	// 256k*16
	#endif

			// PLL3_800

			// PLL4_800
			#define PLL4DIVM	2	// ref2_ck = 12 MHz
			#define PLL4DIVN	64	// 768 MHz
			#define PLL4DIVP	2	// div2
			//#define PLL4DIVQ	19	// LTDC clock divider = 30.315 MHz
			//#define PLL4DIVR	20	// USBPHY clock divider = 38.4 MHz
			//#define PLL4DIVR	24	// USBPHY clock divider = 32 MHz
			//#define PLL4DIVR	32	// USBPHY clock divider = 24 MHz
			#define PLL4DIVR	16	// USBPHY clock divider = 48 MHz (для прямого тактирования USB_OTG FS)

		#else
			// HSI version (HSI=64 MHz)
			// PLL1_1600
			#define PLL1DIVM	5	// ref1_ck = 12.8 MHz
			#define PLL1DIVP	1	// MPU
			#define PLL1DIVQ	2
			#define PLL1DIVR	2
			//#define PLL1DIVN	50	// x25..x100: 12.8 * 50 = 640 MHz
			//#define PLL1DIVN	62	// x25..x100: 12.8 * 62 = 793.6 MHz
			#define PLL1DIVN	(stm32mp1_overdrived() ? 62 : 50)	// Auto select

	#if 1
			// PLL2_1600
			#define PLL2DIVM	5	// ref2_ck = 12.8 MHz
			#define PLL2DIVN	41	// 12.8 * 41 = 524.8 MHz
			#define PLL2DIVP	2	// div2=minimum PLL2 selected as AXI sub-system clock (pll2_p_ck)
			#define PLL2DIVQ	1	// GPU clock divider
			#define PLL2DIVR	1	// DDR clock divider

			#include "src/sdram/stm32mp15-mx_4G.dtsi"		// 256k*16
	#else
			// PLL2_1600
			#define PLL2DIVM	5	// ref2_ck = 12.8 MHz
			#define PLL2DIVN	61//41	// 12.8 * 41 = 524.8 MHz
			#define PLL2DIVP	3//2	// div2=minimum PLL2 selected as AXI sub-system clock (pll2_p_ck)
			#define PLL2DIVQ	2//1	// GPU clock divider
			#define PLL2DIVR	3//1	// DDR clock divider

			#include "src/sdram/stm32mp15-mx_300MHz_4G.dtsi"	// 256k*16
	#endif

			// PLL3_800
			// pll3_p_ck -> mcuss_ck - 209 MHz Max
			#define PLL3DIVM	5	// ref2_ck = 12.8 MHz

			// PLL4_800
			#define PLL4DIVM	5	// ref2_ck = 12.8 MHz
			#define PLL4DIVN	60	// 12.8 * 60 = 768 MHz
			#define PLL4DIVP	2	// div2
			//#define PLL4DIVR	20	// USBPHY clock divider = 38.4 MHz
			//#define PLL4DIVR	24	// USBPHY clock divider = 32 MHz
			//#define PLL4DIVR	32	// USBPHY clock divider = 24 MHz
			#define PLL4DIVR	16	// USBPHY clock divider = 48 MHz (для прямого тактирования USB_OTG FS)

		#endif
	#endif /* WITHISBOOTLOADER */

	#define CTLREGMODE_NOCTLREG	1

	//#define DSTYLE_UR3LMZMOD	1	// Расположение элементов экрана в трансиверах UR3LMZ
	#define	FONTSTYLE_ITALIC	1	// Использовать альтернативный шрифт

	#define WITHNETRESOURCE 1	// поддержка ресурсного протокола
	#define LCDMODE_V2A	1	/* только главный экран с тремя видеобуферами RGB565, без PIP */
	//#define LCDMODE_H497TLB01P4 1	/* 720xRGBx1280 - 5" AMOELD Panel H497TLB01.4 */
	//#define LCDMODETX_TC358778XBG 1	/* Toshiba TC358778XBG chip */
	#define LCDMODE_AT070TNA2 1	/* AT070TNA2 panel (1024*600) - 7" display */
	#define LCDMODEX_SII9022A	1 // siiI9022A Lattice Semiconductor Corp HDMI Transmitter

	//#define WITHLWIP 1
	#define RTC1_TYPE RTC_TYPE_STM32F4xx	/* STM32F4xx/STM32F7xx internal RTC peripherial */
	//#define TSC1_TYPE TSC_TYPE_S3402	/* Capasitive touch screen controller S3402 (on panel H497TLB01.4) */
	#define TSC1_TYPE TSC_TYPE_USBTOUCH
	#define WITHUSESDCARD		1	// Включение поддержки SD CARD
	//#define WITHUSEFATFS		1	// Запись звука на SD CARD

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
	#define NVRAM_TYPE NVRAM_TYPE_BKPSRAM	// Область памяти с батарейным питанием
	//#define NVRAM_TYPE NVRAM_TYPE_NOTHING	// нет NVRAM
	//#define HARDWARE_IGNORENONVRAM	1		// отладка на платах где нет никакого NVRAM

	#if LCDMODE_AT070TNA2 || LCDMODE_AT070TN90
		#if 0
			#define WITHTOUCHGUI		1
			#define WITHAFSPECTRE		1	/* показ спктра прослушиваемого НЧ сигнала. */
			#define WITHALPHA			64
			#define FORMATFROMLIBRARY 	1
			#define WITHUSEMALLOC	1	/* разрешение поддержки malloc/free/calloc/realloc */
		#endif
		//#define WITHAFSPECTRE		1	/* показ спктра прослушиваемого НЧ сигнала. */
	#endif /* LCDMODE_AT070TNA2 || LCDMODE_AT070TN90 */

	#define WITHUSEMALLOC	1	/* разрешение поддержки malloc/free/calloc/realloc */
	//#define FORMATFROMLIBRARY 	1

	#define WITHNOATTNOPREAMP 1
	#define WITHAGCMODEONOFF 1

#endif /* ARM_STM32MP1_LFBGA354_CTLSTYLE_DK2_H_INCLUDED */
