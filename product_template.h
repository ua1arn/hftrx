/* template for product.h */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Для выбора конфигурации целевого трансивера/приёмника раскомментировать в данном файле одну из строк.
// В зависимости от выбранной конфигурации, в проект включаются пары файлов из каталога ./boards
//
// В проекте используется файл product.h - данный файл (product.h.prototype) копируется и переименовывается в product.h. Для компиляции требуется выбрать одну из конфигураций.

#ifndef PRODUCT_H_INCLUDED
#define PRODUCT_H_INCLUDED

#ifndef HARDWARE_H_INCLUDED
	#error PLEASE, DO NOT USE THIS FILE DIRECTLY. USE FILE "hardware.h" INSTEAD.
#endif

#ifndef NIGHTBUILD

//#define WITHDEBUG		1	/* Отладочная печать через COM-порт. */
//#define DEBUGSPEED 500000
#define DEBUGSPEED 115200
//#define DEFAULTDIALFREQ	18112000
//#define DEFAULTDIALFREQ	225000
//#define DEFAULTDIALFREQ	14021000


// Renesas specific
//
// Сперва выполняем make lib при закомментированном WITHISBOOTLOADER0
// Далее открыть WITHISBOOTLOADER0 и выполнить make bootloader

//#define WITHISBOOTLOADER	1	/* выполняем make bootloader */
//#define WITHISBOOTLOADER0	(WITHISBOOTLOADER && CPUSTYLE_R7S721)	/* Renesas specific option - FSBL. Открыть эту строку и запустить make bootloader */
//#define WITHISBOOTLOADER_DDR	(WITHISBOOTLOADER && 1)	/* for xfel: T507, H616, A64, T113-s4 - инициализатор LPDDR4 памяти на плате - set RAM base in allwnr_t507_boot.ld */

// Дисплей с фреймбуфером:
//#define CTLSTYLE_STORCH_V6	1	// Rmainunit_v5la.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, 8-bit display interface, NAU8822L и FPGA EP4CE22E22I7N
#define CTLSTYLE_STORCH_V7	1
//#define CTLSTYLE_V3D	1	// Плата STM32F746G-DISCO с процессором STM32F746NGH6

// Модемы:
//#define CTLSTYLE_STORCH_V4	1	// Rmainunit_v5im.pcb UHF modem v2
#endif /* NIGHTBUILD */

#if CPUSTYLE_STM32F && CTLSTYLE_STORCH_V5 && 0	// SD-CARD. 2xUSB, DUAL WATCH, STM32F446ZET6, TLV320AIC23B и FPGA EP4CE22E22I7N
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v5_progdfu.h"	// Rmainunit_v5l.pcb (mini USBx2, wide display interface) - mini RX
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_storch_v5_progdfu.h"	// Rmainunit_v5l.pcb (mini USBx2, wide display interface) - mini RX
#elif CPUSTYLE_STM32H7XX && CTLSTYLE_STORCH_V6	// Rmainunit_v5la.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	#include "boards/arm_stm32h7xx_tqfp176_ctlstyle_storch_v6.h"	// Rmainunit_v5la.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32h7xx_tqfp176_ctlstyle_storch_v6_nikulski.h"	// Rmainunit_v5la.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32h7xx_tqfp176_ctlstyle_storch_v6_idenis.h"	// Rmainunit_v5la.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32h7xx_tqfp176_ctlstyle_storch_v6_nodisplay.h".h"	// Rmainunit_v5la.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32h7xx_tqfp176_ctlstyle_storch_v6_radiolav.h"	// Rmainunit_v5la.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32h7xx_tqfp176_ctlstyle_storch_v6_UA1M.h"	// Rmainunit_v5la.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32h7xx_tqfp176_ctlstyle_storch_v6_RA1AGO.h"	// Rmainunit_v5la.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32h7xx_tqfp176_cpustyle_storch_v6.h"	// Rmainunit_v5l.pcb (mini USBx2, wide display interface) - mini RX
#elif CPUSTYLE_STM32H7XX && CTLSTYLE_STORCH_V7 && 0	// rmainunit_v5km0.pcb STM32H743IIT6, TFT 4.3", 2xUSB, SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	#include "boards/arm_stm32h7xx_tqfp100_ctlstyle_storch_v7z_vt.h"	// rmainunit_v5km0.pcb, rmainunit_v5km1.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32h7xx_tqfp100_cpustyle_storch_v7z_vt.h"	// Rmainunit_v5l.pcb (mini USBx2, wide display interface) - mini RX
#elif CPUSTYLE_STM32H7XX && CTLSTYLE_STORCH_V7 && 0	// rmainunit_v5km0.pcb STM32H743IIT6, TFT 4.3", 2xUSB, SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	#include "boards/arm_stm32h7xx_tqfp176_ctlstyle_storch_v7z_no_radio.h"	// rmainunit_v5km0.pcb, rmainunit_v5km1.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32h7xx_tqfp176_cpustyle_storch_v7z.h"	// Rmainunit_v5l.pcb (mini USBx2, wide display interface) - mini RX
#elif CPUSTYLE_STM32H7XX && CTLSTYLE_STORCH_V7 && 1	// rmainunit_v5km0.pcb STM32H743IIT6, TFT 4.3", 2xUSB, SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	#include "boards/arm_stm32h7xx_tqfp176_ctlstyle_storch_v7.h"	// rmainunit_v5km0.pcb, rmainunit_v5km1.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32h7xx_tqfp176_ctlstyle_storch_v7_no_radio.h"	// Rmainunit_v5la.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32h7xx_tqfp176_cpustyle_storch_v7.h"	// Rmainunit_v5l.pcb (mini USBx2, wide display interface) - mini RX
#elif CPUSTYLE_R7S721 && CTLSTYLE_STORCH_V7 && 1		// rmainunit_v5km2.pcb R7S721020VCFP, TFT 4.3" or 7", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N, AD9246BCPZ-125
	#include "boards/arm_r7s72_tqfp176_ctlstyle_storch_v8.h"	// rmainunit_v5km2.pcb R7S721020VCFP, TFT 7", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_r7s72_tqfp176_ctlstyle_storch_v8_rd3tcd.h"	// rmainunit_v5km2.pcb R7S721020VCFP, TFT 7", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_r7s72_tqfp176_ctlstyle_storch_v8_ua6ljx.h"	// rmainunit_v5km2.pcb R7S721020VCFP, TFT 7", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_r7s72_tqfp176_ctlstyle_storch_v8_us4ijr.h"	// us4ijr@gmail.com rmainunit_v5km2.pcb R7S721020VCFP, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_r7s72_tqfp176_ctlstyle_storch_v8_ua3reo.h"	// rmainunit_v5km2.pcb R7S721020VCFP, TFT 7", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_r7s72_tqfp176_cpustyle_storch_v8.h"	// rmainunit_v5km2.pcb (mini USBx2, wide display interface) - mini RX
#elif CPUSTYLE_R7S721 && CTLSTYLE_STORCH_V7 && 0		// rmainunit_v5km5.pcb R7S721020VCFP, TFT 4.3" or 7", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N, AD9246BCPZ-125
	#include "boards/arm_r7s72_tqfp176_ctlstyle_storch_v9.h"	// rmainunit_v5km5.pcb R7S721020VCFP, TFT 7", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_r7s72_tqfp176_ctlstyle_storch_v9_us4ijr.h"	// us4ijr@gmail.com rmainunit_v5km5.pcb R7S721020VCFP, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_r7s72_tqfp176_cpustyle_storch_v9.h"	// rmainunit_v5km5.pcb (mini USBx2, wide display interface) - mini RX
#elif CPUSTYLE_STM32MP1 && CTLSTYLE_STORCH_V7 && 0	// // https://www.st.com/en/evaluation-tools/stm32mp157c-dk2.html STM32MP157C-DK2 & 1024x600 HDMI
	#include "boards/arm_stm32mp1xx_ctlstyle_dk2.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32mp1xx_cpustyle_dk2.h"
#elif CPUSTYLE_STM32MP1 && CTLSTYLE_STORCH_V7 && 0	// PanGu board, STM32MP157AAC, USB device, USB HOST+HUB, Ethernet
	#include "boards/arm_stm32mp1xx_ctlstyle_PanGuBoard_v1p2.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32mp1xx_cpustyle_PanGuBoard_v1p2.h"
#elif CPUSTYLE_STM32MP1 && CTLSTYLE_STORCH_V7 && 1	// rmainunit_sv9.pcb STM32MP157AAC - модуль MYC-YA157-V2, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c.h"
	#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c_falcon.h"
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c_lfm.h"
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c_qrp.h"
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c_r1ab.h"
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c_r1cbu.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32mp1xx_cpustyle_storch_v9c.h"	// rmainunit_sv9.pcb STM32MP157AAC - модуль MYC-YA157-V2, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
#elif CPUSTYLE_STM32MP1 && CTLSTYLE_STORCH_V7 && 0	// rmainunit_sv9.pcb STM32MP157AAC - модуль MYC-YA157-V2, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c_bb.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32mp1xx_cpustyle_storch_v9c_bb.h"	// rmainunit_sv9.pcb STM32MP157AAC - модуль MYC-YA157-V2, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
#elif CPUSTYLE_STM32MP1 && CTLSTYLE_STORCH_V7	// rmainunit_v5km2.pcb STM32MP153DAB, TFT 4.3" or 7", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N, AD9246BCPZ-125
	#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9a.h"	// rmainunit_v5km7.pcb STM32MP153DAB, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N, LTC2208
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9a_RL1D.h"	// RU3FW rmainunit_v5km7.pcb STM32MP153DAB, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N, LTC2208
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9a_r1yq.h"	// rmainunit_v5km7.pcb STM32MP153DAB, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9a_china_84748588.h"	// rmainunit_v5km7.pcb STM32MP153DAB, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9a_R3XBQ.h"	// rmainunit_v5km7.pcb STM32MP153DAB, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9a_RK1AQ.h"	// rmainunit_v5km7.pcb STM32MP153DAB, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32mp1xx_cpustyle_storch_v9a.h"	// rmainunit_v5km7.pcb STM32MP153DAB
#elif (CPUSTYLE_A64) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwa64_ctlstyle_banana_pi_m64.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwa64_cpustyle_banana_pi_m64.h"
#elif (CPUSTYLE_H3) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwh3_ctlstyle_orange_pi_pc.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwh3_cpustyle_orange_pi_pc.h"
#elif (CPUSTYLE_T507) && CTLSTYLE_STORCH_V7 && 0
	#define CPUSTYLE_H616 1	/* Дополнительно к CPUSTYLE_T507 */
	#include "boards/arm_allwh616_ctlstyle_orangepi_zero2.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwh616_cpustyle_orangepi_zero2.h"
#elif (CPUSTYLE_T507) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwt507_ctlstyle_veloci_v0.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt507_cpustyle_veloci_v0.h"
#elif (CPUSTYLE_T507) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwt507_ctlstyle_helperboard_t507_ra4asn.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt507_cpustyle_helperboard_t507_ra4asn.h"
#elif (CPUSTYLE_T507) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwt507_ctlstyle_xhelperboard_t507.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt507_cpustyle_xhelperboard_t507.h"
#elif (CPUSTYLE_T507) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwt507_ctlstyle_video2.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt507_cpustyle_video2.h"
#elif CPUSTYLE_T113 && CTLSTYLE_STORCH_V7 && 0
	#include "boards/arm_allwt113s3_ctlstyle_ra4asn.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_ra4asn.h"
#elif (CPUSTYLE_T113 || CPUSTYLE_F133) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwt113s3_ctlstyle_storch_v9a.h"	// rmainunit_sv9f.pcb, rmainunit_sv9u.pcb Allwinner T113-s3, 2xUSB, NAU88C22 и FPGA EP4CE22E22I7N
	//#include "boards/arm_allwt113s3_ctlstyle_storch_v9a_R3XBQ.h"	// rmainunit_sv9f.pcb, rmainunit_sv9u.pcb Allwinner T113-s3, 2xUSB, NAU88C22 и FPGA EP4CE22E22I7N
	//#include "boards/arm_allwt113s3_ctlstyle_storch_v9a_weather.h"	// rmainunit_sv9f.pcb, rmainunit_sv9u.pcb Allwinner T113-s3, 2xUSB, NAU88C22 и FPGA EP4CE22E22I7N
	//#include "boards/arm_allwt113s3_ctlstyle_storch_v9a_ua1cei_mini100W.h"	// 4.3 inch 272*480, rmainunit_sv9e.pcb Allwinner T113-s3, 2xUSB, NAU88C22 и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_storch_v9a.h"	// rmainunit_sv9f.pcb, rmainunit_sv9u.pcb Allwinner T113-s3, 2xUSB, NAU88C22 и FPGA EP4CE22E22I7N
#elif (CPUSTYLE_T113 || CPUSTYLE_F133) && CTLSTYLE_STORCH_V7 && 0
	#include "boards/arm_allwt113s3_ctlstyle_storch_v9a_v0_qrp.h"	// rmainunit_sv9e.pcb Allwinner T113-s3, 2xUSB, NAU88C22 и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_storch_v9a_v0.h"	// rmainunit_sv9e.pcb Allwinner T113-s3, 2xUSB, NAU88C22 и FPGA EP4CE22E22I7N
#elif (CPUSTYLE_T113 || CPUSTYLE_F133) && CTLSTYLE_STORCH_V7 && 0
	#include "boards/arm_allwt113s3_ctlstyle_storch_v9w.h"	// rmainunit_sv9w.pcb, rmainunit_sv9x.pcb  Allwinner t133-S3, USB HUB, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_allwt113s3_ctlstyle_storch_v9w_ua1cei.h"	// rmainunit_sv9w.pcb, rmainunit_sv9x.pcb  Allwinner t133-S3, USB HUB, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_storch_v9w.h"	// rmainunit_sv9w.pcb, rmainunit_sv9x.pcb Allwinner t133-S3, USB HUB, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
#elif (CPUSTYLE_T113 || CPUSTYLE_F133) && CTLSTYLE_STORCH_V7 && 0
	#include "boards/arm_allwt113s3_ctlstyle_storch_v9x2.h"	// rmainunit_sv9x1.pcb, rmainunit_sv9x2.pcb, rmainunit_sv9x3.pcb Allwinner t133-S3, USB HUB, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_storch_v9x2.h"	// rmainunit_sv9x1.pcb, rmainunit_sv9x2.pcb, rmainunit_sv9x3.pcb Allwinner t133-S3, USB HUB, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
#elif (CPUSTYLE_T113 || CPUSTYLE_F133) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwt113s3_ctlstyle_mango_pi.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_mango_pi.h"
#elif (CPUSTYLE_T113 || CPUSTYLE_F133) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwt113s3_ctlstyle_mango_pi_dctrx.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_mango_pi_dctrx.h"
#elif (CPUSTYLE_T113 || CPUSTYLE_F133) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwt113s3_ctlstyle_tboard_v0.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_tboard_v0.h"
#elif (CPUSTYLE_T113 || CPUSTYLE_F133) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwt113s3_ctlstyle_yo6puc.h"	// sabi project
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_yo6puc.h"	// sabi project
#elif (CPUSTYLE_V3S) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_ctlstyle_allw_v3s_lichee0.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_cpustyle_allw_v3s_lichee0.h"
#elif CPUSTYLE_XCZU && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_xczuxx_ctlstyle_axu2cga_ad9640.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_xczuxx_cpustyle_axu2cga_ad9640.h"
#elif CPUSTYLE_XCZU && CTLSTYLE_STORCH_V7
	#include "boards/arm_xczu2_ctlstyle_alinx_axu2cga.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_xczu2_cpustyle_alinx_axu2cga.h"
#elif CPUSTYLE_XC7Z && CTLSTYLE_STORCH_V7 && 0
	#include "boards/arm_x7c70xx_ctlstyle_antminer_20_v1p1.h"	// 7020
	//#include "boards/arm_x7c70xx_ctlstyle_antminer_10_v1p1.h"	// 7020
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_x7c70xx_cpustyle_antminer_v1p1.h"
#elif CPUSTYLE_XC7Z && CTLSTYLE_STORCH_V7 && 0
	#include "boards/arm_x7c70xx_ctlstyle_zinc20.h"	// 7020 Плата Цник20 от НПК ООО "АТРИ" http://www.a3.spb.ru/
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_x7c70xx_cpustyle_zinc20.h"	// 7020 Плата Цник20 от НПК ООО "АТРИ" http://www.a3.spb.ru/
#elif CPUSTYLE_XC7Z && CTLSTYLE_STORCH_V7 && 0
	#include "boards/arm_x7c70xx_ctlstyle_zm10.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_x7c70xx_cpustyle_zm10.h"
#elif CPUSTYLE_XC7Z && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_x7c70xx_ctlstyle_storch_sv9.h"	// 7020
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_x7c70xx_cpustyle_storch_sv9.h"
#elif CPUSTYLE_XC7Z && CTLSTYLE_STORCH_V7 && 0
	#include "boards/arm_x7c70xx_ctlstyle_ebaz4205_10_v1.h"	// плата EBAZ4205
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_x7c70xx_cpustyle_ebaz4205_v1.h"	// плата EBAZ4205
#elif CPUSTYLE_XC7Z && CTLSTYLE_STORCH_V7
	#include "boards/arm_x7c70xx_ctlstyle_ebaz4205_v2.h"	// плата EBAZ4205 с xc7z020 и 512 Мб DDR3
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_x7c70xx_cpustyle_ebaz4205_v2.h"	// плата EBAZ4205 с xc7z020 и 512 Мб DDR3
#elif CPUSTYLE_VM14 && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_vm14_ctlstyle_v0.h"	// 1892ВМ14Я ELVEES multicore.ru
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_vm14_cpustyle_v0.h"
#else
	#define LCDMODE_DUMMY	1
	#define BANDSELSTYLERE_NOTHING	1
	#define	CTLREGMODE_NOCTLREG	1
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#error No any suitable CPUSTYLE_xxx and CTLSTYLE_xxx combination found
#endif

#endif /* PRODUCT_H_INCLUDED */
