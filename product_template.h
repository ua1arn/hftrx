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

#define CMSIS_device_header "hardware.h"

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

// Современные конфигурации:
//#define CTLSTYLE_RAVENDSP_V1	1	// Трансивер Вороненок с 12 кГц IF DSP трактом
//#define CTLSTYLE_RAVENDSP_V3	1	// Трансивер Вороненок с DSP STM32F429ZIT6, кодеком TLV320AIC23B и FPGA EP4CE22E22I7N
//#define CTLSTYLE_RAVENDSP_V4	1	// Трансивер Вороненок с SD-CARD, DSP STM32F429ZIT6, кодеком TLV320AIC23B и FPGA EP4CE22E22I7N
//#define CTLSTYLE_RAVENDSP_V5	1	// Трансивер Вороненок с SD-CARD, DUAL WATCH, STM32F746ZET6, TLV320AIC23B и FPGA EP4CE22E22I7N
//#define CTLSTYLE_RAVENDSP_V6	1	// Трансивер Вороненок с SD-CARD, DUAL WATCH, STM32F746ZET6, NAU8822L и FPGA EP4CE22E22I7N
//#define CTLSTYLE_RAVENDSP_V7	1	// V6 fixed Rmainunit_v5bm.pcb STM32F746ZET6
//#define CTLSTYLE_STORCH_V1	1	// STM32F746ZET6, USB, SD-CARD, DUAL WATCH, NAU8822L и FPGA EP4CE22E22I7N
//#define CTLSTYLE_STORCH_V2	1	// wide LCD interface, STM32F746ZET6, 2xUSB, SD-CARD, DUAL WATCH, NAU8822L и FPGA EP4CE22E22I7N,
//#define CTLSTYLE_STORCH_V3	1	// Rmainunit_v5fm.pcb, Rmainunit_v5jm.pcb, Rmainunit_v5km.pcb STM32F767ZET6, 2xUSB, SD-CARD, 8-bit display interface, NAU8822L и FPGA EP4CE22E22I7N
//#define CTLSTYLE_STORCH_V5	1	// Rmainunit_v5l.pcb STM32F767ZET6, 2xmini-USB, mini SD-CARD, 8-bit display interface, NAU8822L и FPGA EP4CE22E22I7N

// Дисплей с фреймбуфером:
//#define CTLSTYLE_STORCH_V6	1	// Rmainunit_v5la.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, 8-bit display interface, NAU8822L и FPGA EP4CE22E22I7N
#define CTLSTYLE_STORCH_V7	1
//#define CTLSTYLE_V3D	1	// Плата STM32F746G-DISCO с процессором STM32F746NGH6

//
//#define CTLSTYLE_V8A 	1	// приёмник "Воронёнок" с первым гетеродином на AD9834+ADF4001 - ATMEGA644PA - raven_ctlstyle_v8a.h
//#define CTLSTYLE_V8B 	1	// приёмник "Воронёнок" с первым гетеродином на AD9951+ADF4001 - ATMEGA644PA или ARM - raven_ctlstyle_v8b.h
//#define CTLSTYLE_V1B	1	// Контроллер "Воробей" STM32F101RB или ATMega644PA - atmega644_ctlstyle_v1.h
//#define CTLSTYLE_V1C	1	// Контроллер "Воробей-2" на STM32F105RC с местом под кодек.
//#define CTLSTYLE_V1V	1	// Контроллер "Колибри" на ATMega328 - atmega328_ctlstyle_v1.h
//#define CTLSTYLE_V1K	1	// Контроллер на STM32F401RB - новый SW20xx
//#define CTLSTYLE_OLEG4Z_V1	1	// Rmainunit_v5fm.pcb STM32F767ZET6, 2xUSB, WFM, FPGA EP4CE22E22I7N, R820T2

// Модемы:
//#define CTLSTYLE_STORCH_V4	1	// Rmainunit_v5im.pcb UHF modem v2

// Конфигурации под трансиверы UR3LMZ (без внешнего дешифратора адреса).
// Полные:
//#define CTLSTYLE_SW2011 1		// ATMega: SW-2011-RDX - А.Шатун, UR3LMZ (atmega_ctlstyle_v9.h)
//#define CTLSTYLE_SW2012N 1	// ATMega: SW-2012-RDX 6 МГц ПЧ и встроенный преобразователь RDX0154 - А.Шатун, UR3LMZ (atmega_ctlstyle_v9n.h)
//#define CTLSTYLE_SW2013SF 1	// ATMega: SW-2012С - с ПЧ 6 МГЦ и цветным индикатором SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15, двухступенчатой регулировкой мощности, ...
//#define CTLSTYLE_SW2013RDX	1	// ATMega: SW2013_V3, SW2013_V5 (V5.1 - with VOLT METER) с индикатором RDX0154
//#define CTLSTYLE_SW2014FM	1	// ATMega: SW2013_V3, SW2013_V5 (V5.1 - with VOLT METER) с индикатором RDX0154
//#define CTLSTYLE_SW2015	1		// ATMega: SW2015 (LPF, no VHF, V5.1 - with VOLT METER) с индикатором RDX0154
//#define CTLSTYLE_SW2016	1		// ATMega: Si5351, SW20116 (LCD RDT065, Si5351 с ПЧ 6 МГц) - с ФНЧ, без УКВ
//#define CTLSTYLE_SW2016VHF	1	// ATMega: Si5351, SW20116 (LCD RDT065, Si5351 с ПЧ 6 МГц, FM)
//#define CTLSTYLE_SW2018XVR	1	// ATMega: Si5351, SW20116 (LCD RDT065, Si5351 с ПЧ 6 МГц, FM)

// mini:
//#define CTLSTYLE_SW2012_MINI 1	// ATMega: SW-2012-MINI 32*64 - А.Шатун, UR3LMZ (atmega_ctlstyle_v9m.h)
//#define CTLSTYLE_SW2012MC 1	// ATMega: SW-2012С LS020 - MINI с ПЧ 6 МГЦ и цветным индикатором LS020
//#define CTLSTYLE_SW2012CN 1	// ATMega: mini 2013 TFT1.8 - MINI с ПЧ 6 МГЦ и цветным индикатором 160*128 с контроллером Sitronix ST7735
//#define CTLSTYLE_SW2012CN5 1	// ATMega: mini 2013 TFT1.8 - SWR METER + VOLT METER - MINI с ПЧ 6 МГЦ и цветным индикатором 160*128 с контроллером Sitronix ST7735
//#define CTLSTYLE_SW2016MINI 1		// ATMega: mini 2013 TFT1.8 - SWR METER + VOLT METER - MINI с ПЧ 6 МГЦ и цветным индикатором 160*128 с контроллером Sitronix ST7735

// Простой синтезатор на AD9835/AD9834/AD9851/Si570:
//#define CTLSTYLE_V8S 1		// ATMega: "Простой синтезатор на AD9835" - без внешнего дешифратора адресов atmega_ctlstyle_v8s.h
//#define CTLSTYLE_V8S2X 1		// ATMega:Синтезатор на двух модулях AD9850 @ 125 MHz

// "Персональные" конфигурации:
//#define CTLSTYLE_V1A 1		// ATMega: DU VENT SI570+AD9834 (66MHz), LS020, ATMega32 - без внешнего дешифратора адресов - atmega_cpustyle_v1a.h, atmega_ctlstyle_v1a.h - похожа на sw2011
//#define STYLE_TS850_V1 	1	// ARM: Vorobey board for (rd3pq@rambler.ru - Si570), new call: R2PM
//#define CTLSTYLE_IGOR		1		// Версия для Игоря (45 МГц -> 128 kГц с фиксированными гетеродинами)
//#define CTLSTYLE_4Z5KY_V1	1		// Новая прошивка для синтезатора 4Z5KY с двухстрочником http://ur5yfv.ucoz.ua/forum/28-19-2
//#define CTLSTYLE_4Z5KY_V2	1		// Синтезатор 4Z5KY с двухстрочником http://www.cqham.ru/trx92_19.htm AD9951+AD9834
//#define CTLSTYLE_FMPLL_V1	1		// ATMega32, с ФАПЧ для приёма вещательного ЧМ
//#define CTLSTYLE_RK4CI	1		// Контроллер "воробей" ATMega644PA 128*240 - для RK4CI
//#define CTLSTYLE_UA3RNB		1	// Плата UA3RNB ATMega32: Si570+AD9834, WH1602 FM25L04B
//#define CTLSTYLE_UA3DKC		1	// Плата UA3DKC Сергей (ATMega32, FM25040, Si570+AD9834+WH2002) - ref 40 MHz
//#define CTLSTYLE_YUBOARD_V1 1		// ATMega32: Si570+AD9834, RDX0154 и FM25L16B
//#define CTLSTYLE_EW2DZ		1	// Плата UA3DKC Сергей (ATMega32, FM25040, Si570+AD9834+WH2002) - ref 40 MHz
//#define CTLSTYLE_RN4NAB	1	// Контроллер "воробей" ATMega644PA
//#define CTLSTYLE_RV9CQQ	1		// Р-399, собственый контроллер
//#define CTLSTYLE_DUMB	1	// Плата Простой синтезатор на Si570 и AD9834 http://www.cqham.ru/forum/showthread.php?23945-Простой-синтезатор-на-Si570-и-AD9834
//#define CTLSTYLE_V8S_UR3VBM 1		// ATMega: Ur3vbm for trx step killrill@inbox.ru "Простой синтезатор на AD9835" - без внешнего дешифратора адресов atmega_ctlstyle_v8s.h

//#define EXPLORER_V8 1		// ATMega: - rx24 - (Explorer OLD), с новым расположением портов и совмещёнными HD44780 и шиной адреса SPI.
//#define EXPLORER_V8A 		1	// ATMega: (Explorer NEW), с новым расположением портов и совмещёнными I2C, HD44780 и шиной адреса SPI.

// Эксперементальные конфигурации:
//#define CTLSTYLE_V1D	1	// Плата STM32F429I-DISCO с процессором STM32F429ZIT6
//#define CTLSTYLE_V2D	1	// Плата STM32F4DISCOVERY с процессором STM32F407VGT6
//#define CTLSTYLE_NUCLEO_V1	1	// Внешняя аудиоплата на базе NUCLEO-F767ZI с процессором STM32F767ZIT6  mb1137.pdf
//#define CTLSTYLE_V1E	1	// Плата с процессором STM32F030F4P (TSSOP-20)
//#define CTLSTYLE_V1F	1	// Плата с процессором STM32F051C6T (TQFP-48)
//#define CTLSTYLE_V1G	1	// Плата с процессором STM32L051K6T (TQFP-32)
//#define CTLSTYLE_V1X	1	// Плата с процессором ATXMega128A4U
//#define CTLSTYLE_V1M	1	// Плата с процессором ATmega64 (RA1AGO)

// Исторические конфигурации:
//#define CTLSTYLE_V7A 	1	// ARM: Модификация V7 с установленным USB мостом FT232RL. Atmel ARM CPU
//#define CTLSTYLE_V7 1		// ARM или ATMega: большая плата с шестью ГУН, двумя аттенюаторами, УВЧ, совмещённая с синтезатором (6 ГУН). Без FM
//#define CTLSTYLE_V5 1		// ATMega: (UA1ZH) Маленькая плата на ATMEGA, с двумя AD9834 и четырьмя ГУН (atmega_ctlstyle_v5.h)
#endif /* NIGHTBUILD */

#if CPUSTYLE_STM32F && CTLSTYLE_RAVENDSP_V1	// Трансивер Вороненок с IF DSP трактом
	#include "boards/arm_stm32f4xx_tqfp100_ctlstyle_raven_v1.h"	// ravendsp_v1
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp100_cpustyle_raven_v1.h"	// ravendsp_v1
#elif CPUSTYLE_STM32F && CTLSTYLE_RAVENDSP_V3	// STM32F429ZIT6 с кодеком TLV320AIC23B и FPGA EP4CE22E22I7N
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v3.h"	// Rmainunit_v2b.pcb
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_raven_v3.h"	// Rmainunit_v2b.pcb
#elif CPUSTYLE_STM32F && CTLSTYLE_RAVENDSP_V4	// SD-CARD. STM32F429ZIT6 or STM32F745ZGT6, TLV320AIC23B и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v4.h"	// Rmainunit_v2bm.pcb
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v4_RA1AGO.h"	// Rmainunit_v2bm.pcb
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_raven_v4.h"	// Rmainunit_v2bm.pcb
#elif CPUSTYLE_STM32F && CTLSTYLE_RAVENDSP_V5	// SD-CARD. DUAL WATCH, STM32F446ZET6, TLV320AIC23B и FPGA EP4CE22E22I7N
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v5.h"	// Rmainunit_v3bm.pcb
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v5_RU7L.h"	// RU7L Rmainunit_v3bm.pcb
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v5_vlad_vwws.h"	// Rmainunit_v3bm.pcb
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v5_YANTONOV.h"	// Rmainunit_v3bm.pcb
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v5_polyar.h"	// Rmainunit_v3bm.pcb
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v5_August.h"	// Rmainunit_v3bm.pcb
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v5_RA0CGS.h"	// Rmainunit_v3bm.pcb
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_raven_v5.h"	// Rmainunit_v3bm.pcb
#elif CPUSTYLE_STM32F && CTLSTYLE_RAVENDSP_V6	// SD-CARD. DUAL WATCH, STM32F446ZET6, TLV320AIC23B и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v6.h"	// Rmainunit_v4bm.pcb
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v6_mkryukov.h"	// Rmainunit_v4bm.pcb
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v6_RV1CB.h"	// Rmainunit_v4bm.pcb
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_raven_v6.h"	// Rmainunit_v3bm.pcb
#elif CPUSTYLE_STM32F && CTLSTYLE_RAVENDSP_V7	// SD-CARD. DUAL WATCH, STM32F446ZET6, TLV320AIC23B и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v7.h"	// Rmainunit_v5bm.pcb
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v7_yuri5555.h"	// Rmainunit_v5bm.pcb
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v7_RW1AM.h"	// Rmainunit_v5bm.pcb Пётр RW1AM
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_raven_v7_UY5UM.h"	// Rmainunit_v5bm.pcb Игорь Романович UY5UM
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_raven_v7.h"	// Rmainunit_v5bm.pcb
#elif CPUSTYLE_STM32F && CTLSTYLE_STORCH_V1	// SD-CARD. DUAL WATCH, STM32F446ZET6, TLV320AIC23B и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v1.h"	// Rmainunit_v5cm.pcb, Rmainunit_v5dm.pcb (USB)
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v1_penkevich.h"	// penkevich, Rmainunit_v5cm.pcb, Rmainunit_v5dm.pcb (USB)
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v1_RA9OH.h"	// Rmainunit_v5dm.pcb (USB)
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v1_R4DR.h"	// Rmainunit_v5dm.pcb (thermo stab) (USB)
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v1_RD1AD.h"	// Rmainunit_v5cm.pcb (USB)
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v1_OUSSOV.h"	// Rmainunit_v5cm.pcb (USB)
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_storch_v1.h"	// Rmainunit_v5cm.pcb, Rmainunit_v5dm.pcb
#elif CPUSTYLE_STM32F && CTLSTYLE_STORCH_V2	// wide LCD, SD-CARD. 2xUSB, DUAL WATCH, STM32F446ZET6, TLV320AIC23B и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v2.h"	// UA0YAS (RA0CGS) Rmainunit_v5em.pcb (USBx2)
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v2_RX3M.h"	// RX3M Rmainunit_v5em.pcb (USBx2)
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_storch_v2.h"	// Rmainunit_v5em.pcb (USBx2)
#elif CPUSTYLE_STM32F && CTLSTYLE_STORCH_V3	// SD-CARD. 2xUSB, DUAL WATCH, STM32F446ZET6, TLV320AIC23B и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v3.h"	// Rmainunit_v5fm.pcb (USBx2, wide display interface)
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v3_Piter_Kor.h"	// Piter_Kor Rmainunit_v5fm.pcb (USBx2, wide display interface)
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v3_R0CC.h"	// Rmainunit_v5fm.pcb (USBx2, wide display interface)
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v3_UT3NZ.h"	// stm32h743ziT6, EPSON 50 MHz Rmainunit_v5km.pcb (USBx2, wide display interface)
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v3_hansolo.h"	// Дачь gansolo@rambler.ru Rmainunit_v5jm.pcb (USBx2, wide display interface)
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v3_UA9WF.h"	// Rmainunit_v5fm.pcb LTC2217 (USBx2, wide display interface)
	//#include "boards/arm_stm32f7xx_tqfp144_ctlstyle_storch_v3_RA9W.h"	// Rmainunit_v5fm.pcb (USBx2, wide display interface)
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v3_headset.h"	// Rmainunit_v5fm.pcb (USBx2, wide display interface)
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_storch_v3.h"	// Rmainunit_v5fm.pcb (USBx2, wide display interface)
#elif CPUSTYLE_STM32F && CTLSTYLE_STORCH_V4	// модем v2
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v4.h"	// Rmainunit_v5fm.pcb (USBx2, wide display interface)
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_storch_v4.h"	// Rmainunit_v5fm.pcb (USBx2, wide display interface)
#elif CPUSTYLE_STM32F && CTLSTYLE_STORCH_V5 && 0	// SD-CARD. 2xUSB, DUAL WATCH, STM32F446ZET6, TLV320AIC23B и FPGA EP4CE22E22I7N
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v5_progdfu.h"	// Rmainunit_v5l.pcb (mini USBx2, wide display interface) - mini RX
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_storch_v5_progdfu.h"	// Rmainunit_v5l.pcb (mini USBx2, wide display interface) - mini RX
#elif CPUSTYLE_STM32F && CTLSTYLE_STORCH_V5	// SD-CARD. 2xUSB, DUAL WATCH, STM32F446ZET6, TLV320AIC23B и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v5.h"	// Rmainunit_v5l.pcb (mini USBx2, wide display interface) - mini RX
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v5_ua1cei.h"	// + tuner + PA Rmainunit_v5l.pcb (mini USBx2, wide display interface) - mini RX
	//#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_storch_v5_xwws.h"	// Rmainunit_v5l.pcb (mini USBx2, wide display interface) - mini RX
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_storch_v5.h"	// Rmainunit_v5l.pcb (mini USBx2, wide display interface) - mini RX
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
	#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c.h"
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c_falcon.h"
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c_lfm.h"
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c_qrp.h"
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c_ua1cei_noqrp.h"
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c_r1ab.h"
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9c_r1cbu.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32mp1xx_cpustyle_storch_v9c.h"	// rmainunit_sv9.pcb STM32MP157AAC - модуль MYC-YA157-V2, 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
#elif CPUSTYLE_STM32MP1 && CTLSTYLE_STORCH_V7	// rmainunit_v5km2.pcb STM32MP153DAB, TFT 4.3" or 7", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N, AD9246BCPZ-125
	#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9a.h"	// rmainunit_v5km7.pcb STM32MP153DAB, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N, LTC2208
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9a_RL1D.h"	// rmainunit_v5km7.pcb STM32MP153DAB, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N, LTC2208
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9a_r1yq.h"	// rmainunit_v5km7.pcb STM32MP153DAB, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9a_china_84748588.h"	// rmainunit_v5km7.pcb STM32MP153DAB, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9a_R3XBQ.h"	// rmainunit_v5km7.pcb STM32MP153DAB, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	//#include "boards/arm_stm32mp1xx_ctlstyle_storch_v9a_RK1AQ.h"	// rmainunit_v5km7.pcb STM32MP153DAB, TFT 4.3", 2xUSB, NAU8822L и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32mp1xx_cpustyle_storch_v9a.h"	// rmainunit_v5km7.pcb STM32MP153DAB
#elif (CPUSTYLE_A64) && CTLSTYLE_STORCH_V7 && 1
	#define WITHISBOOTLOADER_DDR	(1 && WITHISBOOTLOADER)	/* Allwinner A64-H (Banana Pi BPI-M64) - инициализатор DDR памяти на плате */
	#include "boards/arm_allwa64_ctlstyle_banana_pi_m64.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwa64_cpustyle_banana_pi_m64.h"
#elif (CPUSTYLE_T507) && CTLSTYLE_STORCH_V7 && 0
	#define CPUSTYLE_H616 1	/* Дополнительно к CPUSTYLE_T507 */
	#define WITHISBOOTLOADER_DDR	(1 && WITHISBOOTLOADER)	/* Orange Pi Zero 2 - инициализатор DDR памяти на плате */
	#include "boards/arm_allwh616_ctlstyle_orangepi_zero2.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwh616_cpustyle_orangepi_zero2.h"
#elif (CPUSTYLE_T507) && CTLSTYLE_STORCH_V7 && 1
	//#define WITHISBOOTLOADER_DDR	(1 && WITHISBOOTLOADER)	/* HelperBoard T507 Core Board - инициализатор LPDDR4 памяти на плате */
	#include "boards/arm_allwt507_ctlstyle_helperboard_t507.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt507_cpustyle_helperboard_t507.h"
#elif (CPUSTYLE_T507) && CTLSTYLE_STORCH_V7 && 1
	//#define WITHISBOOTLOADER_DDR	(1 && WITHISBOOTLOADER)	/* HelperBoard T507 Core Board - инициализатор LPDDR4 памяти на плате */
	#define WITHISBOOTLOADER 1
	#include "boards/arm_allwt507_ctlstyle_xhelperboard_t507.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt507_cpustyle_xhelperboard_t507.h"
#elif CPUSTYLE_T113 && CTLSTYLE_STORCH_V7 && 0
	#include "boards/arm_allwt113s3_ctlstyle_ra4asn.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_ra4asn.h"
#elif (CPUSTYLE_T113 || CPUSTYLE_F133) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwt113s3_ctlstyle_storch_v9a.h"	// rmainunit_sv9e.pcb Allwinner T113-s3, 2xUSB, NAU88C22 и FPGA EP4CE22E22I7N
	//#include "boards/arm_allwt113s3_ctlstyle_storch_v9a_ua1cei_mini100W.h"	// 4.3 inch 272*480, rmainunit_sv9e.pcb Allwinner T113-s3, 2xUSB, NAU88C22 и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_storch_v9a.h"	// rmainunit_sv9f.pcb Allwinner T113-s3, 2xUSB, NAU88C22 и FPGA EP4CE22E22I7N
#elif (CPUSTYLE_T113 || CPUSTYLE_F133) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwt113s3_ctlstyle_storch_v9a_v0_qrp.h"	// rmainunit_sv9e.pcb Allwinner T113-s3, 2xUSB, NAU88C22 и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_storch_v9a_v0.h"	// rmainunit_sv9e.pcb Allwinner T113-s3, 2xUSB, NAU88C22 и FPGA EP4CE22E22I7N
#elif (CPUSTYLE_T113 || CPUSTYLE_F133) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwt113s3_ctlstyle_mango_pi.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_mango_pi.h"
#elif (CPUSTYLE_T113 || CPUSTYLE_F133) && CTLSTYLE_STORCH_V7 && 1
	#include "boards/arm_allwt113s3_ctlstyle_tboard_v0.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_allwt113s3_cpustyle_tboard_v0.h"
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
#elif CPUSTYLE_STM32F && CTLSTYLE_OLEG4Z_V1	// 2xUSB, STM32F767ZIT6, FPGA EP4CE22E22I7N & R820T2
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_oleg4z_v1.h"	// Rmainunit_v5fm.pcb (USBx2, wide display interface)
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_oleg4z_v1.h"	// Rmainunit_v5fm.pcb (USBx2, wide display interface)
#elif CPUSTYLE_AT91SAM7S && CTLSTYLE_V7
	#include "boards/arm_ctlstyle_v7.h"		// up-conversion аппараты
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_at91sam7s_cpustyle_v7.h"
#elif CPUSTYLE_AT91SAM7S && CTLSTYLE_V7A
	#include "boards/arm_ctlstyle_v7a.h"	// up-conversion аппараты
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_at91sam7s_cpustyle_v7a.h"
#elif CPUSTYLE_ATMEGA && CTLSTYLE_V8A
	#include "boards/raven_ctlstyle_v8a.h"	// приёмник "Воронёнок" с первым гетеродином на AD9834+ADF4001
	//#include "boards/raven_ctlstyle_v8a_valery.h"	// приёмник "Воронёнок" с первым гетеродином на AD9834+ADF4001
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v8a.h"	// приёмник "Воронёнок", текущая версия для 40/44 ногих процессоров ATMega
#elif CPUSTYLE_ATMEGA && CTLSTYLE_V8B
	//#include "boards/raven_ctlstyle_v8b.h"	// приёмник "Воронёнок" с первым гетеродином на AD9951+ADF4001
	#include "boards/raven_ctlstyle_v8b_R3KBL.h"	// приёмник "Воронёнок" с первым гетеродином на AD9951+ADF4001
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v8a.h"	// приёмник "Воронёнок", текущая версия для 40/44 ногих процессоров ATMega
#elif CPUSTYLE_AT91SAM7S && CTLSTYLE_V8B
	#include "boards/raven_ctlstyle_v8b.h"	// приёмник "Воронёнок" с первым гетеродином на AD9951+ADF4001
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_at91sam7s_cpustyle_v8a.h"	/* AT91SAM7S64	*/
#elif (CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S) && CTLSTYLE_V8B
	#include "boards/raven_ctlstyle_v8b.h"	// приёмник "Воронёнок" с первым гетеродином на AD9951+ADF4001
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_atsam3s_cpustyle_v8a.h"	/* ATSAM3S4B	*/
#elif CPUSTYLE_ATMEGA328 && CTLSTYLE_4Z5KY_V1
	#include "boards/atmega328_ctl_4z5ky_v1.h"	/* Синтезатор 4Z5KY с двухстрочником http://ur5yfv.ucoz.ua/forum/28-19-2 */
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega328_cpu_4z5ky_v1.h"
#elif CPUSTYLE_ATMEGA328 && CTLSTYLE_4Z5KY_V2
	#include "boards/atmega328_ctl_4z5ky_v2.h"	/* Синтезатор 4Z5KY с двухстрочником http://www.cqham.ru/trx92_19.htm AD9951+AD9834 */
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega328_cpu_4z5ky_v2.h"
#elif CPUSTYLE_ATMEGA328 && CTLSTYLE_V1V
	#include "boards/atmega328_ctlstyle_v1.h"	// Синтезатор "Колибри"
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega328_cpustyle_v1.h"
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_FMPLL_V1
	#include "boards/atmega_fmradio_v1.h"	// Синтезатор "Воробей-2"
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v8a.h"
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_IGOR
	#include "boards/atmega644_ctlstyle_igor.h"	// Синтезатор "Воробей"
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v8a.h"
#elif CPUSTYLE_STM32F && CTLSTYLE_V1B
	#include "boards/atmega644_ctlstyle_v1.h"	// Синтезатор "Воробей"
//	#include "boards/arm_stm32f103c8_ctlstyle_test.h"
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32fxxx_tqfp64_cpustyle_v8a.h"	/* STM32F101RB	*/
#elif CPUSTYLE_STM32F && CTLSTYLE_V1K
	#include "boards/arm_stm32fxxx_tqfp64_ctlstyle_v1k.h"	// Синтезатор "Воробей" - новый SW20xx
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32fxxx_tqfp64_cpustyle_v1k.h"	/* STM32F401RB - новый SW20xx*/
#elif CPUSTYLE_STM32F && CTLSTYLE_V1E
	#include "boards/arm_stm32fxxx_tssop20_ctlstyle_v1.h"	// плата с процессором STM32F030F4P (TSSOP-20)
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32fxxx_tssop20_cpustyle_v1.h"	/* STM32F101RB	*/
#elif CPUSTYLE_STM32F && CTLSTYLE_V1F
	#include "boards/arm_stm32f051_tqfp48_ctlstyle_v1.h"	// Плата с процессором STM32F051C6T6 (TQFP-48)
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f051_tqfp48_cpustyle_v1.h"	/* STM32F101RB	*/
#elif CPUSTYLE_STM32F && CTLSTYLE_V1G
	#include "boards/arm_stm32l051_tqfp32_ctlstyle_v1.h"	// Плата с процессором STM32L051K6T (TQFP-32)
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32l051_tqfp32_cpustyle_v1.h"	/* STM32L051K6T	*/
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_RN4NAB
	#include "boards/atmega644_ctlstyle_RN4NAB.h"	// Синтезатор "Воробей"
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v8a.h"
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_RV9CQQ
	#include "boards/atmega_ctlstyle_RV9CQQ.h"	// Р-399, собственый контроллер
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_RV9CQQ.h"
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_V1B
	#include "boards/atmega644_ctlstyle_v1.h"	// Синтезатор "Воробей"
	//#include "boards/atmega644_ctlstyle_v1_gorlov.h"	// Синтезатор "Воробей" для RC3U hansgo_1969@mail.ru
	//#include "boards/atmega644_ctlstyle_LADVABEST.h"	// Синтезатор "Воробей" у ladvabest
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v8a.h"
#elif CPUSTYLE_ATMEGA128 && CTLSTYLE_V1M
	#include "boards/atmega64_ctlstyle_v1m.h"	// MG
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega64_cpustyle_v1m.h"	// MG
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_RK4CI
	#include "boards/atmega644_ctlstyle_rk4ci.h"	// Синтезатор "Воробей"
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v8a.h"
#elif CPUSTYLE_STM32F && CTLSTYLE_V1C
	#include "boards/arm_stm32fxxx_tqfp64_ctlstyle_v8b.h"	// "Воробей-2" с кодеком
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32fxxx_tqfp64_cpustyle_v8b.h"	// Синтезатор "Воробей-2" на процессоре STM32F105RC с кодеком TLV320AIC23B
#elif CPUSTYLE_STM32F && CTLSTYLE_V1D
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_32F429DISCO.h"	/* Плата STM32F429I-DISCO с процессором STM32F429ZIT6	*/
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_32F429DISCO.h"	/* плата STM32F429I-DISCO с процессором STM32F429ZIT6	*/
#elif CPUSTYLE_STM32F && CTLSTYLE_V2D
	#include "boards/arm_stm32fxxx_tqfp100_ctlstyle_v8c.h"	/* плата STM32F4DISCOVERY с процессором STM32F407VGT6	*/
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32fxxx_tqfp100_cpustyle_v8c.h"	/* плата STM32F4DISCOVERY с процессором STM32F407VGT6	*/
#elif CPUSTYLE_STM32F && CTLSTYLE_V3D
	#include "boards/arm_stm32f4xx_tqfp144_ctlstyle_32F746G.h"	/* Плата STM32F746G-DISCO с процессором STM32F746NGH6	*/
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f4xx_tqfp144_cpustyle_32F746G.h"	/* Плата STM32F746G-DISCO с процессором STM32F746NGH6	*/
#elif CPUSTYLE_STM32F && CTLSTYLE_NUCLEO_V1	// Внешняя аудиоплата на базе NUCLEO-F767ZI с процессором STM32F767ZIT6  mb1137.pdf
	#include "boards/arm_stm32f767_tqfp144_ctlstyle_nucleo.h"
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32f767_tqfp144_cpustyle_nucleo.h"
#elif CPUSTYLE_ATXMEGAXXXA4 && CTLSTYLE_V1X
	#include "boards/atxmega32a4_ctlstyle_v1.h"
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atxmega32a4_cpustyle_v1.h"
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_V1A
	#include "boards/atmega_ctlstyle_v1a.h"	// 
	//#include "boards/atmega_ctlstyle_v1a_ut4ua.h"	//
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v1a.h"	//
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_V5
	#include "boards/atmega_ctlstyle_v5.h"	
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v5.h"
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_V7
	#include "boards/atmega_ctlstyle_v7.h"	// up-conversion аппараты
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v7.h"
#elif CPUSTYLE_STM32F && STYLE_TS850_V1
	#include "boards/arm_ctlstyle_ts850_v1.h"	// up-conversion аппараты с ПЧ 73.050 МГц, 24 бита в управляющем регистре
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32fxxx_tqfp64_cpustyle_v8a.h"	/* STM32F101RB	*/
#elif CPUSTYLE_ATMEGA && CTLSTYLE_V8S
	#include "boards/atmega_ctlstyle_v8s.h"	// Down-conversion аппараты с ПЧ 4.0..10.7 МГц, 24/16  бита в управляющем регистре
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v8s.h"	// Версия без внешнего дешифратора ажресов на процессоре.
#elif CPUSTYLE_ATMEGA && CTLSTYLE_V8S_UR3VBM
	#include "boards/atmega_ctlstyle_v8s_ur3vbm.h"		// ATMega: Ur3vbm for trx step killrill@inbox.ru "Простой синтезатор на AD9835" - без внешнего дешифратора адресов atmega_ctlstyle_v8s.h
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v8s.h"	// Версия без внешнего дешифратора ажресов на процессоре.
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_V8S2X
	#include "boards/atmega_ctlstyle_v8s2x.h"	// Синтезатор на двух модулях AD9850 @ 125 MHz
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v8a.h"
#elif CPUSTYLE_ATMEGA && CTLSTYLE_YUBOARD_V1
	#include "boards/atmega_ctlstyle_yub1.h"	// ATMega32: Si570+AD9834, RDX0154 и FM25L16B
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v8s.h"	// Версия без внешнего дешифратора ажресов на процессоре.
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_DUMB
	#include "boards/atmega_ctl_dumb.h"	// Плата UA3DKC Сергей (Si570+AD9834+WH2002) - ref 40 MHz
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpu_dumb.h"	// новая плата Explorer, плата в единственном числе - эксперементальная плата Explorer
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_UA3DKC
	#include "boards/atmega_ctl_ua3dkc.h"	// Плата UA3DKC Сергей (Si570+AD9834+WH2002) - ref 40 MHz
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpu_ua3dkc.h"	// новая плата Explorer, плата в единственном числе - эксперементальная плата Explorer
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_UA3RNB
	#include "boards/atmega_ctl_ua3rnb.h"	// Плата UA3RNB ATMega32: Si570+AD9834, WH1602 FM25L04B ref 50 MHz
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpu_ua3dkc.h"	// новая плата Explorer, плата в единственном числе - эксперементальная плата Explorer
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_EW2DZ
	#include "boards/atmega_ctl_ew2dz.h"	// Плата UA3DKC Сергей (Si570+AD9834+WH2002) - ref 40 MHz
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpu_ua3dkc.h"	// новая плата Explorer, плата в единственном числе - эксперементальная плата Explorer
#elif CPUSTYLE_ATMEGA32 && EXPLORER_V8
	#include "boards/atmega_ctlstyle_rx24.h"	// Down-conversion аппараты с ПЧ 4.0..10.7 МГц, 24/16 бита в управляющем регистре
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v8.h"	// новая плата Explorer, плата в единственном числе - эксперементальная плата Explorer
#elif CPUSTYLE_ATMEGA32 && EXPLORER_V8A
	#include "boards/atmega_ctlstyle_rx24.h"	// Down-conversion аппараты с ПЧ 4.0..10.7 МГц, 24/16  бита в управляющем регистре
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v8a.h"	// новая плата Explorer, текущая версия для 40/44 ногих процессоров ATMega
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2011
	#include "boards/atmega_ctlstyle_v9.h"	// SW-2011-RDX
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9.h"	// SW-2011-RDX
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2012N
	#include "boards/atmega_ctlstyle_v9n.h"	// SW-2012-RDX с ПЧ 6 МГц
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9.h"	// SW-2011-RDX
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2012MC
	#include "boards/atmega_ctlstyle_v9c.h"	// SW-2011-MINI с ПЧ 6 МГц и цветным индикатором
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9.h"	// SW-2011-RDX
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2012CN
	#include "boards/atmega_ctlstyle_v9cn.h"	// SW-2011-MINI с ПЧ 6 МГц и цветным индикатором
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9.h"	// SW-2011-RDX
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2012CN5
	#include "boards/atmega_ctlstyle_v9cn5.h"	// SW-2011-MINI с ПЧ 6 МГц + SWR METER + VOLT METER и цветным индикатором
	//#include "boards/atmega_ctlstyle_v9cn5_RN3ZOB.h"	// ATMega644, tuner, цветной индикатор
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9.h"	// SW-2011-RDX
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2016MINI
	#include "boards/atmega_ctlstyle_v9cn6.h"	// SW-2016-MINI с ПЧ 6 МГц и цветным индикатором
	//#include "boards/atmega_ctlstyle_v9cn6_UY5UM.h"	// SW-2016-MINI с ПЧ 6 МГц и EPSON цветным индикатором
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9.h"	// SW-2011-RDX
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2013SF
	//#include "boards/atmega_ctlstyle_v9sf.h"	// SW-2012SFс ПЧ 6 МГц и цветным индикатором
	#include "boards/atmega_ctlstyle_v9sf_US2IT.h"	// US2IT/D0ITC (Si570), UT0IS/D0ISM (AD9951) SW-2012SFс ПЧ 6 МГц и цветным индикатором
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9sf.h"	// SW-2012SF-RDX
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2013RDX 
	#include "boards/atmega_ctlstyle_v9rdx.h"	// ATMega: SW2013_V3, SW2013_V5 (V5.1 - with VOLT METER) с индикатором RDX0154
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9sf.h"	// SW-2012SF-RDX
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2014FM 
	#include "boards/atmega_ctlstyle_v9fm.h"	// ATMega: SW2013_V3, SW2013_V5 (V5.1 - with VOLT METER) с индикатором RDX0154
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9sf.h"	// SW-2012SF-RDX
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2015
	#include "boards/atmega_ctlstyle_sw2015.h"	// SW-2015 с ПЧ 6 МГц и монохромным индикатором
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9sf.h"	// SW-2012SF-RDX
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2012_MINI
	#include "boards/atmega_ctlstyle_v9m.h"	// SW-2012-MINI
	//#include "boards/atmega_ctlstyle_v9sf_rx3qsp.h"	// плата SW-2012SF-MINI с ПЧ 6 МГц и цветным индикатором
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9.h"	// SW-2011-RDX
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2016
	#include "boards/atmega_ctlstyle_sw2016.h"	// atmega32, LCD RDT065, Si5351 с ПЧ 6 МГц и монохромным индикатором
	//#include "boards/atmega_ctlstyle_sw2016_UY5UM_WO240.h"	// atmega32, LCD RDT065, Si5351 с ПЧ 6 МГц и монохромным индикатором
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9sf.h"	// SW-2012SF-RDX
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2016VHF
	#include "boards/atmega_ctlstyle_sw2016vhf.h"	// atmega32, LCD RDT065, Si5351 с ПЧ 6 МГц и монохромным индикатором
	//#include "boards/atmega_ctlstyle_sw2016vhf_UY5UM.h"	// atmega32, LCD WO240128, Si5351 с ПЧ 6 МГц и монохромным индикатором
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9sf.h"	// SW-2012SF-RDX
#elif CPUSTYLE_ATMEGA32 && CTLSTYLE_SW2018XVR
	#include "boards/atmega_ctlstyle_sw2018xvr.h"	// atmega32, LCD RDX0154, Si5351 с ПЧ 6 МГц и монохромным индикатором
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#include "boards/atmega_cpustyle_v9sf.h"	// SW-2012SF-RDX
#else
	#define LCDMODE_RDX0154		1
	#define BANDSELSTYLERE_NOTHING	1
	#define	CTLREGMODE_NOCTLREG	1
	#include "paramdepend.h"				/* проверка зависимостей параметров конфигурации */
	#error No any suitable CPUSTYLE_xxx and CTLSTYLE_xxx combination found
#endif

#endif /* PRODUCT_H_INCLUDED */
