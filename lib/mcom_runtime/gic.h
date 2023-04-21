// ********************** gic.h ***************************************
//                 Arm-Dsp runtime library
//
//  Module for interrupt handling. Warning: use only with custom statrup
//  file. Example - "reset.s".
//
// (c) multicore.ru
//
// *******************************************************************

#ifndef __ARM_GIC__
#define __ARM_GIC__

#ifdef __cplusplus
extern "C" {
#endif

#include "erlcommon.h"

#define CORE_CNT 2
#define INTERRUPTS_NUM 160

typedef enum {
    GIC_PL390_PPI0,
    GIC_PL390_PPI1,
    GIC_PL390_PPI2,
    GIC_PL390_PPI3,
    GIC_PL390_PPI4,
    GIC_PL390_PPI5,
    GIC_PL390_PPI6,
    GIC_PL390_PPI7,
    GIC_PL390_PPI8,
    GIC_PL390_PPI9,
    GIC_PL390_PPI10,
    GIC_PL390_PPI11,
    GIC_PL390_PPI12,
    GIC_PL390_PPI13,
    GIC_PL390_PPI14,
    GIC_PL390_PPI15,
    GIC_PL390_PPI16,
    GIC_PL390_PPI17,
    GIC_PL390_PPI18,
    GIC_PL390_PPI19,
    GIC_PL390_PPI20,
    GIC_PL390_PPI21,
    GIC_PL390_PPI22,
    GIC_PL390_PPI23,
    GIC_PL390_PPI24,
    GIC_PL390_PPI25,
    GIC_PL390_PPI26,
    GIC_PL390_PPI27, // Global Timer
    GIC_PL390_PPI28,
    GIC_PL390_PPI29, // Private Timer
    GIC_PL390_PPI30, // Private WDT
    GIC_PL390_PPI31,

    MPU_L2CCINTR,   // Прерывание контроллера кэша второго уровня MPU - 32
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
} INTERRUPT_TYPE;

struct pl390_gic_dist_if {
    volatile unsigned int ICDDCR; 				// +0x000 - RW - Distributor Control Register
    const volatile unsigned int ICDICTR; 		// +0x004 - RO - Interrupt Controller Type Register
    const volatile unsigned int ICDIIDR; 		// +0x008 - RO - Distributor
    const volatile unsigned int padding0[29]; 	// Implementer Identification Register
    	  volatile unsigned int ICDISR[32]; 	// +0x080 - RW - Interrupt Security Registers
    	  volatile unsigned int ICDISER[32]; 	// +0x100 - RW - Interrupt Set-Enable Registers
    	  volatile unsigned int ICDICER[32]; 	// +0x180 - RW - Interrupt Clear-Enable Registers
    	  volatile unsigned int ICDISPR[32]; 	// +0x200 - RW - Interrupt Set-Pending Registers
    	  volatile unsigned int ICDICPR[32]; 	// +0x280 - RW - Interrupt Clear-Pending Registers
    const volatile unsigned int ICDABR[32]; 	// +0x300 - RO - Active Bit Registers
    const volatile unsigned int padding1[32];
    	  volatile unsigned int ICDIPR[256]; 	// +0x400 - RW - Interrupt Priority Registers
    	  volatile unsigned int ICDIPTR[256]; 	// +0x800 - RW - Interrupt Processor Targets Registers
     	  volatile unsigned int ICDICFR[64]; 	// +0xC00 - RW - Interrupt Configuration Registers
    const volatile unsigned int padding2[128];
    	  volatile unsigned int ICDSGIR; 		// +0xF00 - WO - Software Generated Interrupt Register
};

struct pl390_gic_cpu_if {
    volatile unsigned int
        ICCICR; // +0x000 - RW - CPU Interface Control Register
    volatile unsigned int
        ICCPMR; // +0x004 - RW - Interrupt Priority Mask Register
    volatile unsigned int ICCBPR; // +0x008 - RW - Binary Point Register
    const volatile unsigned int
        ICCIAR; // +0x00C - RO - Interrupt Acknowledge Register
    volatile unsigned int ICCEOIR; // +0x010 - WO - End of Interrupt Register
    const volatile unsigned int
        ICCRPR; // +0x014 - RO - Running Priority Register
    const volatile unsigned int
        ICCHPIR; // +0x018 - RO - Highest Pending Interrupt Register
    volatile unsigned int
        ICCABPR; // +0x01C - RW - Aliased Binary Point Register

    const volatile unsigned int padding0[55];

    const volatile unsigned int
        ICCIIDR; // +0x0FC - RO - CPU Interface Identification Register
};

// extern volatile struct pl390_gic_dist_if* gic_dist;
// extern volatile struct pl390_gic_cpu_if* gic_cpu;

// ----------------------------------------------------------

// Sets the address of the distributor and cpu interfaces
/*!
 * @ingroup Group_SYSTEM
 * Setup base addresses distributor and cpu interface arrays
 * @param dist - distributor address
 * @param cpu - cpu interface address
 */
void initGICPointers(unsigned int dist, unsigned int cpu);

// ----------------------------------------------------------

//  Global enable of the Interrupt Distributor
/*!
 * @ingroup Group_SYSTEM
 * Enable GIC unit
 */
void enableGIC(void);

// Global disable of the Interrupt Distributor
/*!
 * @ingroup Group_SYSTEM
 * Disable GIC unit
 */
void disableGIC(void);

// ----------------------------------------------------------

// Enables the interrupt source number ID
/*!
 * @ingroup Group_SYSTEM
 * Enable interrupt specified by ID
 * @param ID - IRQ identificator
 */
void enableIntID(unsigned int ID);

// Disables the interrupt source number ID
/*!
 * @ingroup Group_SYSTEM
 * Disables the interrupt source number ID
 * @param ID - IRQ identificator
 */
void disableIntID(unsigned int ID);

// ----------------------------------------------------------

// Sets the priority of the specified ID
/*!
 * @ingroup Group_SYSTEM
 * Sets the priority of the specified ID
 * @param ID - IRQ identificator
 * @param priority - IRQ priority
 */
void setIntPriority(unsigned int ID, unsigned int priority);

// Returns the priority of the specified ID
/*!
 * @ingroup Group_SYSTEM
 * Returns the priority of the specified ID
 * @param ID - IRQ identificator
 * @return priority
 */
unsigned int getIntPriority(unsigned int ID);

// ----------------------------------------------------------

#define GIC_PL390_TARGET_CPU0 (0x01)
#define GIC_PL390_TARGET_CPU1 (0x02)
#define GIC_PL390_TARGET_CPU2 (0x04)
#define GIC_PL390_TARGET_CPU3 (0x08)
#define GIC_PL390_TARGET_CPU4 (0x10)
#define GIC_PL390_TARGET_CPU5 (0x20)
#define GIC_PL390_TARGET_CPU6 (0x40)
#define GIC_PL390_TARGET_CPU7 (0x80)

// Sets the target CPUs of the specified ID
// For 'target' use one of the above defines
/*!
 * @ingroup Group_SYSTEM
 * Sets the target CPUs of the specified ID
 * @param ID - IRQ identificator
 * @param target - target CPU (1, 2, 4, 8, 10, 20, 40, 80)
 */
void setIntTarget(unsigned int ID, unsigned int target);

// Returns the target CPUs of the specified ID
/*!
 * @ingroup Group_SYSTEM
 * Returns the target CPUs of the specified ID
 * @param ID - IRQ identificator
 * @return target (1, 2, 4, 8, 10, 20, 40, 80)
 */
unsigned int getIntTarget(unsigned int ID);

// ----------------------------------------------------------

#define GIC_PL390_CONFIG_LEVEL (0)
#define GIC_PL390_CONFIG_EDGE (2)

// Configures the specified ID as being level or edge triggered
/*!
 * @ingroup Group_SYSTEM
 * Configures the specified ID as being level or edge triggered
 * @param ID - IRQ identificator
 * @param conf - configuration ( 0 - level, 2 - edge)
 */
void configureID(unsigned int ID, unsigned int conf);

// ----------------------------------------------------------

// Sets the pending bit of the specified ID
/*!
 * @ingroup Group_SYSTEM
 * Sets the pending bit of the specified ID
 * @param ID - IRQ identificator
 */
void setIntPending(unsigned int ID);

// Clears the pending bit of the specified ID
/*!
 * @ingroup Group_SYSTEM
 * Clears the pending bit of the specified ID
 * @param ID - IRQ identificator
 */
void clearIntPending(unsigned int ID);

#define GIC_PL390_PENDING_SET (1)
#define GIC_PL390_PENDING_CLEAR (0)

// Returns the value of the status bit of the specified ID
/*!
 * @ingroup Group_SYSTEM
 * Returns the value of the status bit of the specified ID
 * @param ID - IRQ identificator
 * @return pending status (1 - set, 0 - clear)
 */
unsigned int getIntPending(unsigned int ID);

// ----------------------------------------------------------

#define GIC_PL390_SGI_SECURE (0)
#define GIC_PL390_SGI_NONSECURE (1)
#define GIC_PL390_SGI_FILTER_USE_LIST (0)
#define GIC_PL390_SGI_FILTER_NOT_THIS_CPU (1)
#define GIC_PL390_SGI_FILTER_THIS_CPU (2)
#define GIC_PL390_SGI_CPU0 (0x01)
#define GIC_PL390_SGI_CPU1 (0x02)
#define GIC_PL390_SGI_CPU2 (0x04)
#define GIC_PL390_SGI_CPU3 (0x08)
#define GIC_PL390_SGI_CPU4 (0x10)
#define GIC_PL390_SGI_CPU5 (0x20)
#define GIC_PL390_SGI_CPU6 (0x40)
#define GIC_PL390_SGI_CPU7 (0x80)

// Send a software generate interrupt
void sendSGI(unsigned int ID, unsigned int cpu_list, unsigned int filter_list,
             unsigned int SATT);

// ----------------------------------------------------------

// Sets the specified ID as secure
void makeIntSecure(unsigned int ID);

// Set the specified ID as non-secure
void makeIntNonSecure(unsigned int ID);

// Returns the security of the specified ID
unsigned int getIntSecurity(unsigned int ID);

// ------------------------------------------------------------
// CPU Interface functions
// ------------------------------------------------------------

// Enables the processor interface
// Must been done one each core seperately
void enableCPUInterface(void);

// Disables the processor interface
void disableCPUInterface(void);

// Enables the sending of secure interrupts as FIQs
void enableSecureFIQs(void);

// Disables the sending of secure interrupts as FIQs
void disableSecureFIQs(void);

//  Returns the value of the Interrupt Acknowledge Register
unsigned int readIntAck(void);

// Writes ID to the End Of Interrupt register
void writeEOI(unsigned int ID);

// Sets the Priority mask register for the core run on
// The reset value masks ALL interrupts!
void setPriorityMask(unsigned int priority);

// Sets the Binary Point Register for the core run on
void setBinaryPoint(unsigned int priority);

// interrupt handler
typedef void (*handler_funct_ptr)(int);
extern handler_funct_ptr funct_handlers[INTERRUPTS_NUM];
extern handler_funct_ptr funct_handlers1[INTERRUPTS_NUM];
enum EXCEPTIONS {
    RESET,
    UNDEFINED_INSTRUCTION,
    SOFTWARE_INTERRUPT,
    PREFETCH_ABORT,
    DATA_ABORT,
    RESERVED0,
    RESERVED1,
    FIQ_PROCESSING,
    SIZE
};
/*!
 * @ingroup Group_SYSTEM
 * Enable IRQ interrupts
 */
void enable_IRQ(void);
/*!
 * @ingroup Group_SYSTEM
 * Disable IRQ interrupts
 */
void disable_IRQ(void);
/*!
 * @ingroup Group_SYSTEM
 * Enable FIQ interrupts
 */
void enable_FIQ(void);
/*!
 * @ingroup Group_SYSTEM
 * Disable FIQ interrupts
 */
void disable_FIQ(void);
/*!
 * @ingroup Group_SYSTEM
 * UNUSED (System handler called from assembler boot code)
 */
void C_int_handler(unsigned int ulICCIAR);

void C_exception_handler(unsigned int exception_id);
/*!
 * @ingroup Group_SYSTEM
 * Attach custom IRQ handler to specified ID
 * @param ID - IRQ identifier
 * @param funct_pointer - custom handler pointer
 */
void attach_irq_handler(unsigned int ID, handler_funct_ptr funct_pointer,
                        unsigned int core);
/*!
 * @ingroup Group_SYSTEM
 * Attach custom exception handler to specified ID
 * @param ID - exception identifier
 * @param funct_pointer - custom handler pointer
 */
void attach_exception_handler(enum EXCEPTIONS ID,
                              handler_funct_ptr funct_pointer,
                              unsigned int core);
/*!
 * @ingroup Group_SYSTEM
 * UNUSED (Dummy handler)
 * @param n - IRQ bit number
 */
void reset_irq_handlers(void);
/*!
 * @ingroup Group_SYSTEM
 * Reset exception handlers array by default function
 */
void reset_exception_handlers(void);

void dummy_exception_handler(int id);
void dummy_prefetch_abort_handler(int id);
void dummy_data_abort_handler(int id);
/****************************************************
 Group of procedures for old tests compatibility
 Do not use it in new tests!!!!
 ******************************************************/
/*!
 * @ingroup Group_SYSTEM
 * UNUSED (only for old tests support)
 */
void InterruptEnable(void);
/*!
 * @ingroup Group_SYSTEM
 * UNUSED (only for old tests support)
 */
void InterruptDisable(void);
/*!
 * @ingroup Group_SYSTEM
 * UNUSED (only for old tests support)
 * @param ID - interrupt ID
 */
void openIM_mask(int ID);
/*!
 * @ingroup Group_SYSTEM
 * UNUSED (only for old tests support)
 * @param ID - interrupt ID
 */
void closeIM_mask(int ID);
/*!
 * @ingroup Group_SYSTEM
 * UNUSED  (only for old tests support)
 * @param n_bit - bit number
 */
void open_mask(int n_bit);
/*!
 * @ingroup Group_SYSTEM
 * UNUSED  (only for old tests support)
 * @param n_bit - bit number
 */
void close_mask(int n_bit);

// ----------------------------------------------------------
// End of arm_gic.hpp
// ----------------------------------------------------------

void prefetchAbortInterrupt(int adr);
void dataAbortInterrupt(int adr);
void FIQInterrupt();
void vApplicationIRQHandler(unsigned int ulICCIAR);

// *****************************************************

// Section: Functions
//
// Function: risc_initialize_gic
//
// Initialize GIC for IRQ handling
//
void risc_initialize_gic();

// Function: risc_enable_interrupt
//
// Enable ARM IRQ for target CPU
//
// Parameters:
//      ID - number of interrupt (from enum INTERRUPT_TYPE)
//      target - target CPU number
//      priority - priority of IRQ
//
// Examples:
//  > risc_enable_interrupt(TIMER_INTR2, GIC_PL390_TARGET_CPU0, 0xF1);
//
// See also:
//
//      risc_disable_interrupt
//
void risc_enable_interrupt(INTERRUPT_TYPE ID, unsigned int target,
                           unsigned int priority);

// Function: risc_disable_interrupt
//
// Disable interrupt by IRQ number
//
// Parameters:
//      ID - number of interrupt
//
// Examples:
//  > risc_disable_interrupt(TIMER_INTR2);
//
// See also:
//
//      risc_enable_interrupt
//
void risc_disable_interrupt(INTERRUPT_TYPE ID);

// Function: risc_disable_interrupts
//
// Disable all IRQ interrupts by setting priority mask
//
// Examples:
//  > risc_disable_interrupts();
//
// See also:
//
//      risc_enable_interrupt
//      risc_disable_interrupt
//
void risc_disable_interrupts();

// Function: risc_disable_interrupts
//
// Enable interrupts by setting priority mask
//
// Examples:
//  > risc_anable_interrupts();
//
// See also:
//
//      risc_enable_interrupt
//      risc_disable_interrupt
//
void risc_enable_interrupts();

// Function: risc_register_interrupt
//
// Register IRQ handler function in handlers vector
//
// Parameters:
//      funct_pointer - your handler function
//      ID - type of interrupt, for which you set handler
//      core - CPU number
//
// Returns:
//      ERL_ERROR - status of error (from enum ERL_ERROR)
//
// Examples:
//  > risc_register_interrupt(risc_tmr_hnlr, RISC_INT_IT);
//
// See also:
//
//      risc_enable_interrupt
//
enum ERL_ERROR risc_register_interrupt(handler_funct_ptr funct_pointer,
                                       INTERRUPT_TYPE ID, unsigned int core);

#ifdef __cplusplus
}
#endif

#endif // __ARM_GIC__
