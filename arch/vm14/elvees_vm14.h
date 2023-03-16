/*
 * elvees_vm14.h
 *
 *  Created on: 16 мар. 2023 г.
 *      Author: User
 */

#ifndef ARCH_VM14_ELVEES_VM14_H_
#define ARCH_VM14_ELVEES_VM14_H_

typedef enum xIRQn
{

  /******  VM14  specific Interrupt Numbers ****************************************************************************/

	/* interrupts */
  MPU_L2CCINTR = 32,   // Прерывание контроллера кэша второго уровня MPU
  DSP_INT_DSP,    // Прерывание от DSP
  GPU_IRQPPMMU0,  // Прерывание от MMU пиксельного процессора GPU
  GPU_IRQPP0,     // Прерывание от пиксельного процессора GPU
  GPU_IRQPMU,     // Прерывание от PMU GPU
  GPU_IRQGPMMU,   // Прерывание от MMU геометрического процессора GPU
  GPU_IRQGP,      // Прерывание от геометрического процессора GPU
  VPU_INT,        // Прерывание от VPU
  SDMA_IRQ0,      // Прерывание 0 от SDMA
  SDMA_IRQ1,      // Прерывание 1 от SDMA
  SDMA_IRQ2,      // Прерывание 2 от SDMA
  SDMA_IRQ3,      // Прерывание 3 от SDMA
  SDMA_IRQ4,      // Прерывание 4 от SDMA
  SDMA_IRQ5,      // Прерывание 5 от SDMA
  SDMA_IRQ6,      // Прерывание 6 от SDMA
  SDMA_IRQ7,      // Прерывание 7 от SDMA
  SDMA_IRQ8,      // Прерывание 8 от SDMA
  SDMA_IRQ9,      // Прерывание 9 от SDMA
  SDMA_IRQ10,     // Прерывание 10 от SDMA
  SDMA_IRQ11,     // Прерывание 11 от SDMA
  SDMA_IRQ12,     // Прерывание 12 от SDMA
  SDMA_IRQ13,     // Прерывание 13 от SDMA
  SDMA_IRQ14,     // Прерывание 14 от SDMA
  SDMA_IRQ15,     // Прерывание 15 от SDMA
  PDMA_INT_FLAG0, //
  PDMA_INT_FLAG1, //
  PDMA_INT_FLAG2, //
  PDMA_INT_FLAG3, //
  PDMA_INT_FLAG4, //
  RTC_TICK_INT,   //
  RTC_PMWKP_INT,  //
  RTC_ALARM_INT,  //
  USBIC_INT,      //
  USBIC_ENDP_INT, //
  VPIN_VIOINT,    //
  VPIN_STR0INT,   //
  VPIN_STR1INT,   //
  VPOUT_LCDINT,   //
  VPOUT_DSIINT,   //
  NFC_MPORTINT,   //
  MFBSP0_DMAIRQ0, //Прерывание от канала DMA порта МFBSP0 при передаче данных
  MFBSP0_DMAIRQ1, //Прерывание от канала DMA порта МFBSP0 при приеме данных
  MFBSP1_DMAIRQ0, //Прерывание от канала DMA порта МFBSP1 при передаче данных
  MFBSP1_DMAIRQ1, //Прерывание от канала DMA порта МFBSP1 при приеме данных
  MPU_IRQS44,     //Резерв
  MPU_IRQS45,     //Резерв
  CPU0_PMUIRQ,    //Прерывание от монитора производительности CPU0
  CPU1_PMUIRQ,    //Прерывание от монитора производительности CPU1
  SWIC0DMA_IRQ0,  //
  SWIC0DMA_IRQ1,  //
  SWIC0DMA_IRQ2,  //
  SWIC0DMA_IRQ3,  //
  SWIC1DMA_IRQ0,  //
  SWIC1DMA_IRQ1,  //
  SWIC1DMA_IRQ2,  //
  SWIC1DMA_IRQ3,  //
  SWIC0_INT,      //
  SWIC1_INT,      //
  MFBSP0_RXIRQ,   //Формируется, если порт включен на прием данных (в одном из
                //режимов), а число 64-х разрядных слов в буфере приёма больше
                //чем RLEV (RLEV устанавливается в регистре состояния
                //приёмника RSR)
  MFBSP0_TXIRQ, //Формируется, если порт включен на передачу данных (в одном
                //из режимов), а число 64-х разрядных слов, находящихся в
                //буфере передачи меньше, либо равно TLEV (TLEV
                //устанавливается в регистре состояния передатчика TSR)
  MFBSP0_SRQ,   //Запрос обслуживания от порта MFBSP0. Формируется, если порт
                //выключен (LEN=0, SPI_I2S_EN=0), а на выводах LACK или LCLK
                //присутствует сигнал высокого уровня
  MFBSP1_RXIRQ, //Формируется, если порт включен на прием данных (в одном из
                //режимов), а число 64-х разрядных слов в буфере приёма больше
                //чем RLEV (RLEV устанавливается в регистре состояния
                //приёмника RSR)
  MFBSP1_TXIRQ, //Формируется, если порт включен на передачу данных (в одном
                //из режимов), а число 64-х разрядных слов, находящихся в
                //буфере передачи меньше, либо равно TLEV (TLEV
                //устанавливается в регистре состояния передатчика TSR)
  MFBSP1_SRQ,   //Запрос обслуживания от порта MFBSP0. Формируется, если порт
                //выключен (LEN=0, SPI_I2S_EN=0), а на выводах LACK или LCLK
                //присутствует сигнал высокого уровня
  UART0_INTR,   //Прерывание от UART0
  UART1_INTR,   //Прерывание от UART1
  UART2_INTR,   //Прерывание от UART2
  UART3_INTR,   //Прерывание от UART3
  I2C0_IC_INTR, //Прерывание от I2C0
  I2C1_IC_INTR, //Прерывание от I2C1
  I2C2_IC_INTR, //Прерывание от I2C2
  MCC_INT,      //
  I2S0_INTR,    //Прерывание от I2S0
  EMAC_INT,     //Прерывание от EMAC
  SSI0_SSI_INTR, //Прерывание от SSI0
  SSI1_SSI_INTR, //Прерывание от SSI1
  PWM_INT,       //
  PWM_INTU,      //
  SDMMC0_INT,    //Прерывание от SDMMC0
  SDMMC1_INT,    //Прерывание от SDMMC1
  TIMER_INTR0,   //Прерывание 0 от TIMER
  TIMER_INTR1,   //Прерывание 1 от TIMER
  TIMER_INTR2,   //Прерывание 2 от TIMER
  TIMER_INTR3,   //Прерывание 3 от TIMER
  TIMER_INTR4,   //Прерывание 4 от TIMER
  TIMER_INTR5,   //Прерывание 5 от TIMER
  TIMER_INTR6,   //Прерывание 6 от TIMER
  TIMER_INTR7,   //Прерывание 7 от TIMER
  GPIO_INTR0,    //Прерывание 0 от GPIO
  GPIO_INTR1,    //Прерывание 1 от GPIO
  GPIO_INTR2,    //Прерывание 2 от GPIO
  GPIO_INTR3,    //Прерывание 3 от GPIO
  GPIO_INTR4,    //Прерывание 4 от GPIO
  GPIO_INTR5,    //Прерывание 5 от GPIO
  GPIO_INTR6,    //Прерывание 6 от GPIO
  GPIO_INTR7,    //Прерывание 7 от GPIO
  GPIO_INTR8,    //Прерывание 8 от GPIO
  GPIO_INTR9,    //Прерывание 9 от GPIO
  GPIO_INTR10,   //Прерывание 10 от GPIO
  GPIO_INTR11,   //Прерывание 11 от GPIO
  GPIO_INTR12,   //Прерывание 12 от GPIO
  GPIO_INTR13,   //Прерывание 13 от GPIO
  GPIO_INTR14,   //Прерывание 14 от GPIO
  GPIO_INTR15,   //Прерывание 15 от GPIO
  GPIO_INTR16,   //Прерывание 16 от GPIO
  GPIO_INTR17,   //Прерывание 17 от GPIO
  GPIO_INTR18,   //Прерывание 18 от GPIO
  GPIO_INTR19,   //Прерывание 19 от GPIO
  GPIO_INTR20,   //Прерывание 20 от GPIO
  GPIO_INTR21,   //Прерывание 21 от GPIO
  GPIO_INTR22,   //Прерывание 22 от GPIO
  GPIO_INTR23,   //Прерывание 23 от GPIO
  GPIO_INTR24,   //Прерывание 24 от GPIO
  GPIO_INTR25,   //Прерывание 25 от GPIO
  GPIO_INTR26,   //Прерывание 26 от GPIO
  GPIO_INTR27,   //Прерывание 27 от GPIO
  GPIO_INTR28,   //Прерывание 28 от GPIO
  GPIO_INTR29,   //Прерывание 29 от GPIO
  GPIO_INTR30,   //Прерывание 30 от GPIO
  GPIO_INTR31,   //Прерывание 31 от GPIO
  MAILBOX_IRQR,  //
  MAILBOX_IRQW,  //
  IRQ_PMU0,      //
  IRQ_PMU1,      //
  CPU0_CTIIRQ,   //
  CPU1_CTIIRQ,   //
  DLOCK_IRQ,     //Прерывание от коммутатора микросхемы
  WDT_IRQ,       //Прерывание от сторожевого таймера

} xIRQn_Type;


/* auto-generated header */
#include "elvees_vm14_2.h"


/* configuration for the PL310 L2 cache controller */
#define PL310_BASE L2CACHE_BASE
#define PL310_TAG_RAM_LATENCY ((1u << 8) | (1u << 4) | (1u << 0))
#define PL310_DATA_RAM_LATENCY ((1u << 8) | (2u << 4) | (1u << 0))

//#define GIC_DISTRIBUTOR_BASE  	((uintptr_t) 0x39001000)                        /*!< (GIC DIST  ) Base Address */
//#define GIC_INTERFACE_BASE    	((uintptr_t) 0x39000100)                        /*!< (GIC CPU IF) Base Address */
#define L2C_310_BASE          	((uintptr_t) 0x39004000)                        /*!< (PL310     ) Base Address */
#define TIMER_BASE				((uintptr_t) 0x39000600)

/* --------  Configuration of the Cortex-A9 Processor and Core Peripherals  ------- */
#define __CA_REV         		    0x0000    /*!< Core revision r0p0 */

#define __CORTEX_A                    9U      /*!< Cortex-A# Core */
#define __FPU_PRESENT                 1U      /*!< Set to 1 if FPU is present */
#define __GIC_PRESENT                 1U      /*!< Set to 1 if GIC is present */
#define __TIM_PRESENT                 1U      /*!< Set to 1 if TIM is present */
#define __L2C_PRESENT                 1U      /*!< Set to 1 if L2C is present */

#include "core_ca.h"

#include <arch/vm14/system_vm14.h>

#endif /* ARCH_VM14_ELVEES_VM14_H_ */
