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
	//#define WITHCPUXTAL 24000000u	/* На процессоре установлен кварц 24.000 МГц */
	#define WITHCPUXOSC 24000000u	/* На процессоре установлен генератор 24.000 МГц */

	#if 1//WITHISBOOTLOADER
		// Варианты конфигурации тактирования
		// ref1_ck, ref2_ck - 8..16 MHz
		// PLL1, PLL2 VCOs
		#if WITHCPUXTAL || WITHCPUXOSC

			// PLL1_1600
			#define PLL1DIVM	2	// ref1_ck = 12 MHz (8..16 MHz valid)
			#define PLL1DIVP	1	// MPU
			#define PLL1DIVQ	2
			#define PLL1DIVR	2

			//#define PLL1DIVN	54	// 12*54 = 648 MHz
			//#define PLL1DIVN	66	// 12*66 = 792 MHz
			#define PLL1DIVN	(stm32mp1_overdrived() ? 66 : 54)	// Auto select

			// PLL2_1600
			#if 1
				#define PLL2DIVM	2	// ref2_ck = 12 MHz (8..16 MHz valid)
				#define PLL2DIVN	44	// 528 MHz Valid division rations for DIVN: between 25 and 100
				#define PLL2DIVP	2	// AXISS_CK div2=minimum 528/2 = 264 MHz PLL2 selected as AXI sub-system clock (pll2_p_ck) - 266 MHz max for all CPU revisions
				#define PLL2DIVQ	1	// GPU clock divider = 528 MHz - 533 MHz max for all CPU revisions
				#define PLL2DIVR	1	// DDR clock divider = 528 MHz
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_1G.dtsi"	// 64k*16
				#define BOARD_DDR_CONFIG "stm32mp15-mx_2G.dtsi"	// 128k*16
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_4G.dtsi"		// 256k*16
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_8G.dtsi"	// 512k*16
			#elif 0
				// PLL2_1600
				#define PLL2DIVM	2	// ref2_ck = 12 MHz (8..16 MHz valid)
				#define PLL2DIVN	66	// 528 MHz Valid division rations for DIVN: between 25 and 100
				#define PLL2DIVP	3	// AXISS_CK div2=minimum 528/2 = 264 MHz PLL2 selected as AXI sub-system clock (pll2_p_ck) - 266 MHz max for all CPU revisions
				#define PLL2DIVQ	2	// GPU clock divider = 528 MHz - 533 MHz max for all CPU revisions
				#define PLL2DIVR	2	// DDR clock divider = 528 MHz
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_1G.dtsi"	// 64k*16
				#define BOARD_DDR_CONFIG "stm32mp15-mx_2G.dtsi"	// 128k*16
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_4G.dtsi"		// 256k*16
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_8G.dtsi"	// 512k*16
			#else
				// PLL2_1600
				/* bad boards DDR3 clock = 300 MHz */
				#define PLL2DIVM	2	// ref2_ck = 12 MHz (8..16 MHz valid)
				#define PLL2DIVN	50	// 600 MHz Valid division rations for DIVN: between 25 and 100
				#define PLL2DIVP	3	// AXISS_CK div2=minimum 1056/4 = 200 MHz PLL2 selected as AXI sub-system clock (pll2_p_ck) - 266 MHz max for all CPU revisions
				#define PLL2DIVQ	2	// GPU clock divider = 300 MHz - 533 MHz max for all CPU revisions
				#define PLL2DIVR	2	// DDR clock divider = 300 MHz
				// less or equal 300 MHz
				// DDR3 timings only 6-6-6 (in  according AN5168
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_300MHz_1G.dtsi"	// 64k*16
				#define BOARD_DDR_CONFIG "stm32mp15-mx_300MHz_2G.dtsi"	// 128k*16
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_300MHz_4G.dtsi"	// 256k*16
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_300MHz_8G.dtsi"	// 512k*16
			#endif

			// PLL3_800
			#define PLL3DIVM	2	// ref3_ck = 12 MHz (4..16 MHz valid)
			#define PLL3DIVN	64	// 768 MHz
			#define PLL3DIVQ	125	// I2S, SAI clock divider: 12/2*64 = 768 MHz. 768/125 = 6.144 MHz. 48 kHz * 64 = 3.072 MHz

			// PLL4_800
			#define PLL4DIVM	2	// ref2_ck = 12 MHz (4..16 MHz valid)
			#define PLL4DIVN	64	// 768 MHz
			#define PLL4DIVP	2	// div2
			//#define PLL4DIVQ	19	// LTDC clock divider = 30.315 MHz
			//#define PLL4DIVR	20	// USBPHY clock divider = 38.4 MHz
			//#define PLL4DIVR	24	// USBPHY clock divider = 32 MHz
			//#define PLL4DIVR	32	// USBPHY clock divider = 24 MHz
			#define PLL4DIVR	16	// USBPHY clock divider = 48 MHz (для прямого тактирования USB_OTG FS)

			//	In addition, if the USBO is used in full-speed mode only, the application can choose the
			//	48 MHz clock source to be provided to the USBO:
			// USBOSRC
			//	0: pll4_r_ck clock selected as kernel peripheral clock (default after reset)
			//	1: clock provided by the USB PHY (rcc_ck_usbo_48m) selected as kernel peripheral clock
			// USBPHYSRC
			//  0x0: hse_ker_ck clock selected as kernel peripheral clock (default after reset)
			//  0x1: pll4_r_ck clock selected as kernel peripheral clock
			//  0x2: hse_ker_ck/2 clock selected as kernel peripheral clock
			#define RCC_USBCKSELR_USBOSRC_VAL 0x01
			#define RCC_USBCKSELR_USBPHYSRC_VAL 0x00


		#else
			// HSI version (HSI=64 MHz)
			// PLL1_1600
			#define PLL1DIVM	5	// ref1_ck = 12.8 MHz (8..16 MHz valid)
			#define PLL1DIVP	1	// MPU
			#define PLL1DIVQ	2
			#define PLL1DIVR	2
			//#define PLL1DIVN	50	// x25..x100: 12.8 * 50 = 640 MHz
			//#define PLL1DIVN	62	// x25..x100: 12.8 * 62 = 793.6 MHz
			#define PLL1DIVN	(stm32mp1_overdrived() ? 62 : 50)	// Auto select

			#if 1
				// PLL2_1600
				#define PLL2DIVM	5	// ref2_ck = 12.8 MHz (8..16 MHz valid)
				#define PLL2DIVN	41	// 12.8 * 41 = 524.8 MHz
				#define PLL2DIVP	2	// div2=minimum PLL2 selected as AXI sub-system clock (pll2_p_ck)
				#define PLL2DIVQ	1	// GPU clock divider
				#define PLL2DIVR	1	// DDR clock divider
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_1G.dtsi"	// 64k*16
				#define BOARD_DDR_CONFIG "stm32mp15-mx_2G.dtsi"	// 128k*16
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_4G.dtsi"		// 256k*16
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_8G.dtsi"	// 512k*16
			#else
				// PLL2_1600
				#define PLL2DIVM	5	// ref2_ck = 12.8 MHz (8..16 MHz valid)
				#define PLL2DIVN	61//41	// 12.8 * 41 = 524.8 MHz
				#define PLL2DIVP	3//2	// div2=minimum PLL2 selected as AXI sub-system clock (pll2_p_ck)
				#define PLL2DIVQ	2//1	// GPU clock divider
				#define PLL2DIVR	3//1	// DDR clock divider
				// less or equal 300 MHz
				// DDR3 timings only 6-6-6 (in  according AN5168
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_300MHz_1G.dtsi"	// 64k*16
				#define BOARD_DDR_CONFIG "stm32mp15-mx_300MHz_2G.dtsi"	// 128k*16
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_300MHz_4G.dtsi"	// 256k*16
				//#define BOARD_DDR_CONFIG "stm32mp15-mx_300MHz_8G.dtsi"	// 512k*16
			#endif

			// PLL3_800
			// pll3_p_ck -> mcuss_ck - 209 MHz Max
			#define PLL3DIVM	5	// ref3_ck = 12.8 MHz (4..16 MHz valid)
			#define PLL3DIVN	60	// 768 MHz
			#define PLL3DIVQ	125	// I2S, SAI clock divider: 12/2*64 = 768 MHz. 768/125 = 6.144 MHz. 48 kHz * 64 = 3.072 MHz

			// PLL4_800
			#define PLL4DIVM	5	// ref2_ck = 12.8 MHz  (4..16 MHz valid)
			#define PLL4DIVN	60	// 12.8 * 60 = 768 MHz
			#define PLL4DIVP	2	// div2
			//#define PLL4DIVR	20	// USBPHY clock divider = 38.4 MHz
			//#define PLL4DIVR	24	// USBPHY clock divider = 32 MHz
			//#define PLL4DIVR	32	// USBPHY clock divider = 24 MHz
			#define PLL4DIVR	16	// USBPHY clock divider = 48 MHz (для прямого тактирования USB_OTG FS)

			//	In addition, if the USBO is used in full-speed mode only, the application can choose the
			//	48 MHz clock source to be provided to the USBO:
			// USBOSRC
			//	0: pll4_r_ck clock selected as kernel peripheral clock (default after reset)
			//	1: clock provided by the USB PHY (rcc_ck_usbo_48m) selected as kernel peripheral clock
			// USBPHYSRC
			//  0x0: hse_ker_ck clock selected as kernel peripheral clock (default after reset)
			//  0x1: pll4_r_ck clock selected as kernel peripheral clock
			//  0x2: hse_ker_ck/2 clock selected as kernel peripheral clock
			#define RCC_USBCKSELR_USBOSRC_VAL 0x01
			#define RCC_USBCKSELR_USBPHYSRC_VAL 0x01

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
	//#define WITHFT8	1	/* Поддержка протокола FT8. Для фонового декодирования требуется минимум двухъядерный процессор и внешняя оперативная память */

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
//			#define WITHALTERNATIVEFONTS    1
//			#define WITHALTERNATIVELAYOUT    1
		#endif
		//#define WITHAFSPECTRE		1	/* показ спктра прослушиваемого НЧ сигнала. */
	#endif /* LCDMODE_AT070TNA2 || LCDMODE_AT070TN90 */

	#define WITHUSEMALLOC	1	/* разрешение поддержки malloc/free/calloc/realloc */
	//#define FORMATFROMLIBRARY 	1

	// +++ вариации прошивки, специфические для разных частот опорного генератора
	//#define DIRECT_125M0_X1		1	/* Тактовый генератор на плате 125.0 МГц */
	#define DIRECT_122M88_X1	1	/* Тактовый генератор 122.880 МГц */
	#define BANDSELSTYLERE_UPCONV56M	1	/* Up-conversion with working band .030..56 MHz */
	#define FQMODEL_FPGA		1	// FPGA + IQ over I2S
	#define XVTR_NYQ1			1	// Support Nyquist-style frequency conversion

	// --- вариации прошивки, специфические для разных частот

	#define WITHPREAMPATT2_6DB 1	/* LTC2208 Управление УВЧ и двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB */
	//#define WITHATT2_6DB	1		// LTC2217 Управление двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB без УВЧ
	#define DEFPREAMPSTATE 	0	/* УВЧ по умолчанию включён (1) или выключен (0) */

	#define WITHAGCMODEONOFF	1	// АРУ вкл/выкл
	#define WITHMIC1LEVEL		1	// установка усиления микрофона

	#define CTLREGMODE_NOCTLREG	1
	
	
	

	#define WITHPABIASMIN		0
	#define WITHPABIASMAX		255

	#define WITHNOATTNOPREAMP 1
	#define WITHAGCMODEONOFF 1
	#define WITHMODESETFULLNFM 1
	//#define WITHRTTY 1	/* подержка демодулятора RTTY */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define BOARD_FILTER_0P5		1	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_3P1		0	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_6P0		0	/* 6.0 kHz filter */
	#define BOARD_FILTER_8P0		0	/* 6.0 kHz filter */

	/* все возможные в данной конфигурации фильтры */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1 /* | IF3_FMASK_6P0 | IF3_FMASK_8P0*/)
	/* все возможные в данной конфигурации фильтры для передачи */
	#define IF3_FMASKTX	(IF3_FMASK_3P1 /*| IF3_FMASK_6P0 */)
	/* фильтры, для которых стоит признак HAVE */
	#define IF3_FHAVE	( IF3_FMASK_0P5 | IF3_FMASK_3P1 /*| IF3_FMASK_6P0 | IF3_FMASK_8P0*/)

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

//	#define NVRAM_TYPE NVRAM_TYPE_NOTHING	// нет NVRAM
//	#define HARDWARE_IGNORENONVRAM	1		// отладка на платах где нет никакого NVRAM

	#define DDS1_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS1 */

	#define BOARDPOWERMIN	0	// Нижний предел регулировки (показываемый на дисплее)
	#define BOARDPOWERMAX	100	// Верхний предел регулировки (показываемый на дисплее)

#endif /* ARM_STM32MP1_LFBGA354_CTLSTYLE_DK2_H_INCLUDED */
