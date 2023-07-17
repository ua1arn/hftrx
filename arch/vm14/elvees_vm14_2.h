#pragma once
#ifndef HEADER_00003039_INCLUDED
#define HEADER_00003039_INCLUDED
#include <stdint.h>


/* IRQs */

typedef enum IRQn
{
    SGI0_IRQn = 0,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI1_IRQn = 1,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI2_IRQn = 2,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI3_IRQn = 3,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI4_IRQn = 4,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI5_IRQn = 5,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI6_IRQn = 6,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI7_IRQn = 7,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI8_IRQn = 8,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI9_IRQn = 9,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI10_IRQn = 10,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI11_IRQn = 11,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI12_IRQn = 12,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI13_IRQn = 13,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI14_IRQn = 14,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI15_IRQn = 15,                                  /*!< GIC_INTERFACE GIC CPU IF */
    VirtualMaintenanceInterrupt_IRQn = 25,            /*!< GIC_INTERFACE GIC CPU IF */
    HypervisorTimer_IRQn = 26,                        /*!< GIC_INTERFACE GIC CPU IF */
    VirtualTimer_IRQn = 27,                           /*!< GIC_INTERFACE GIC CPU IF */
    Legacy_nFIQ_IRQn = 28,                            /*!< GIC_INTERFACE GIC CPU IF */
    SecurePhysicalTimer_IRQn = 29,                    /*!< GIC_INTERFACE GIC CPU IF */
    NonSecurePhysicalTimer_IRQn = 30,                 /*!< GIC_INTERFACE GIC CPU IF */
    Legacy_nIRQ_IRQn = 31,                            /*!< GIC_INTERFACE GIC CPU IF */
    UART0_IRQn = 96,                                  /*!< UART Universal Asynchronous Receiver-Transmitter */
    UART1_IRQn = 97,                                  /*!< UART Universal Asynchronous Receiver-Transmitter */
    UART2_IRQn = 98,                                  /*!< UART Universal Asynchronous Receiver-Transmitter */
    UART3_IRQn = 99,                                  /*!< UART Universal Asynchronous Receiver-Transmitter */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define SDMA_BASE ((uintptr_t) 0x37220000)            /*!< SDMA Регистры блока управления SDMA Base */
#define NANDMPORT_BASE ((uintptr_t) 0x38007000)       /*!< NANDMPORT КОНТРОЛЛЕР ПАМЯТИ ТИПА NAND FLASH (NANDMPORT) Base */
#define NORMPORT_BASE ((uintptr_t) 0x38008000)        /*!< NORMPORT контроллер внешней SRAM/NOR Flash памяти. Base */
#define UART0_BASE ((uintptr_t) 0x38028000)           /*!< UART Universal Asynchronous Receiver-Transmitter Base */
#define UART1_BASE ((uintptr_t) 0x38029000)           /*!< UART Universal Asynchronous Receiver-Transmitter Base */
#define UART2_BASE ((uintptr_t) 0x3802A000)           /*!< UART Universal Asynchronous Receiver-Transmitter Base */
#define UART3_BASE ((uintptr_t) 0x3802B000)           /*!< UART Universal Asynchronous Receiver-Transmitter Base */
#define GPIO0_BASE ((uintptr_t) 0x38034000)           /*!< GPIOBLOCK Регистры блока управления GPIO Base */
#define GPIOA_BASE ((uintptr_t) 0x38034000)           /*!< GPIO Регистры блока управления GPIO Base */
#define GPIOB_BASE ((uintptr_t) 0x3803400C)           /*!< GPIO Регистры блока управления GPIO Base */
#define GPIOC_BASE ((uintptr_t) 0x38034018)           /*!< GPIO Регистры блока управления GPIO Base */
#define GPIOD_BASE ((uintptr_t) 0x38034024)           /*!< GPIO Регистры блока управления GPIO Base */
#define MFBSP0_BASE ((uintptr_t) 0x38086000)          /*!< MFBSP МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  Base */
#define DMA_MFBSP0_BASE ((uintptr_t) 0x38087000)      /*!< DMA_MFBSP  Base */
#define MFBSP1_BASE ((uintptr_t) 0x38088000)          /*!< MFBSP МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  Base */
#define DMA_MFBSP1_BASE ((uintptr_t) 0x38089000)      /*!< DMA_MFBSP  Base */
#define CMCTR_BASE ((uintptr_t) 0x38094000)           /*!< CMCTR Контроллер управления синхронизацией Base */
#define PMCTR_BASE ((uintptr_t) 0x38095000)           /*!< PMCTR Контроллер управления энергопотреблением Base */
#define SMCTR_BASE ((uintptr_t) 0x38096000)           /*!< SMCTR общиме системные настройки микросхемы Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0x39000100)   /*!< GIC_INTERFACE GIC CPU IF Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x39001000) /*!< GIC_DISTRIBUTOR  Base */

/*
 * @brief CMCTR
 */
/*!< CMCTR Контроллер управления синхронизацией */
typedef struct CMCTR_Type
{
             uint32_t reserved_0x000;
    volatile uint32_t DIV_MPU_CTR;                    /*!< Offset 0x004 Регистр делителя частоты MPUCLK */
    volatile uint32_t DIV_ATB_CTR;                    /*!< Offset 0x008 Регистр делителя частоты ATCLK */
    volatile uint32_t DIV_APB_CTR;                    /*!< Offset 0x00C Регистр делителя частоты APCLK */
    volatile uint32_t CHIP_REVISION;                  /*!< Offset 0x010 Регистр ревизии микросхемы */
    volatile uint32_t GATE_MPU_CTR;                   /*!< Offset 0x014 Регистр по управлению отключением частот в CMCTR_MPU */
             uint32_t reserved_0x018 [0x0005];
    volatile uint32_t DIV_GPU_CTR;                    /*!< Offset 0x02C Регистр делителя частоты GPU_ACLK */
    volatile uint32_t DIV_DDR0_CTR;                   /*!< Offset 0x030 Регистр делителя частоты DDR0_SCLK */
    volatile uint32_t DIV_DDR1_CTR;                   /*!< Offset 0x034 Регистр делителя частоты DDR1_SCLK */
    volatile uint32_t DIV_NFC_CTR;                    /*!< Offset 0x038 Регистр делителя частоты NFC_SCLK */
    volatile uint32_t DIV_NOR_CTR;                    /*!< Offset 0x03C Регистр делителя частоты NOR_SCLK */
    volatile uint32_t DIV_SYS0_CTR;                   /*!< Offset 0x040 Регистр делителя частоты L1_HCLK и связанных с ней частот */
    volatile uint32_t DIV_SYS1_CTR;                   /*!< Offset 0x044 Регистр делителя частоты L3_PCLK и связанных с ней частот */
    volatile uint32_t GATE_CORE_CTR;                  /*!< Offset 0x048 Регистр по управлению отключением частот в CMCTR_CORE */
    volatile uint32_t GATE_SYS_CTR;                   /*!< Offset 0x04C Регистр по управлению отключением частот в CMCTR_SYS */
             uint32_t reserved_0x050 [0x0006];
    volatile uint32_t GATE_DSP_CTR;                   /*!< Offset 0x068 Регистр по управлению отключением частот в CMCTR_DSP */
             uint32_t reserved_0x06C [0x0005];
    volatile uint32_t MUX_CLKOUT;                     /*!< Offset 0x080 Регистр выбора частоты для вывода CLKOUT */
    volatile uint32_t DIV_CLKOUT;                     /*!< Offset 0x084 Регистр делителя частоты для вывода CLKOUT */
    volatile uint32_t GATE_CLKOUT;                    /*!< Offset 0x088 Регистр управления отключением частоты для вывода CLKOUT */
             uint32_t reserved_0x08C;
    volatile uint32_t LS_ENABLE;                      /*!< Offset 0x090 Регистр контроля функции LightSleep памятей микросхемы */
             uint32_t reserved_0x094 [0x001B];
    volatile uint32_t SEL_APLL;                       /*!< Offset 0x100 Регистр контроля APLL */
    volatile uint32_t SEL_CPLL;                       /*!< Offset 0x104 Регистр контроля CPLL */
    volatile uint32_t SEL_DPLL;                       /*!< Offset 0x108 Регистр контроля DPLL */
    volatile uint32_t SEL_SPLL;                       /*!< Offset 0x10C Регистр контроля SPLL */
    volatile uint32_t SEL_VPLL;                       /*!< Offset 0x110 Регистр контроля VPLL */
    volatile uint32_t SEL_UPLL;                       /*!< Offset 0x114 Регистр контроля UPLL */
} CMCTR_TypeDef; /* size of structure = 0x118 */
/*
 * @brief GPIO
 */
/*!< GPIO Регистры блока управления GPIO */
typedef struct GPIO_Type
{
    volatile uint32_t gpio_swport_dr;                 /*!< Offset 0x000 Регистр данных порта А. W/R 0x0 0x00 */
    volatile uint32_t gpio_swport_ddr;                /*!< Offset 0x004 Регистр направления потока данных через порт А. W/R 0x0 0x04 */
    volatile uint32_t gpio_swport_ctl;                /*!< Offset 0x008 Регистр источника данных порта А. W/R 0x0 0x08 */
} GPIO_TypeDef; /* size of structure = 0x00C */
/*
 * @brief GPIOBLOCK
 */
/*!< GPIOBLOCK Регистры блока управления GPIO */
typedef struct GPIOBLOCK_Type
{
    volatile uint32_t gpio_swporta_dr;                /*!< Offset 0x000 Регистр данных порта А. W/R 0x0 0x00 */
    volatile uint32_t gpio_swporta_ddr;               /*!< Offset 0x004 Регистр направления потока данных через порт А. W/R 0x0 0x04 */
    volatile uint32_t gpio_swporta_ctl;               /*!< Offset 0x008 Регистр источника данных порта А. W/R 0x0 0x08 */
    volatile uint32_t gpio_swportb_dr;                /*!< Offset 0x00C Регистр данных порта B. W/R 0x0 0x0C  */
    volatile uint32_t gpio_swportb_ddr;               /*!< Offset 0x010 Регистр направления потока данных через порт B. W/R 0x0 0x10 */
    volatile uint32_t gpio_swportb_ctl;               /*!< Offset 0x014 Регистр источника данных порта B. W/R 0xFFFF 0x14 */
    volatile uint32_t gpio_swportc_dr;                /*!< Offset 0x018 Регистр данных порта C. W/R 0x0 0x18 */
    volatile uint32_t gpio_swportc_ddr;               /*!< Offset 0x01C Регистр направления потока данных через порт C. W/R 0x0 0x1C */
    volatile uint32_t gpio_swportc_ctl;               /*!< Offset 0x020 Регистр источника данных порта C. W/R 0x0 0x20 */
    volatile uint32_t gpio_swportd_dr;                /*!< Offset 0x024 Регистр данных порта D. W/R 0x0 0x24  */
    volatile uint32_t gpio_swportd_ddr;               /*!< Offset 0x028 Регистр направления потока данных через порт D. W/R 0x0 0x28 */
    volatile uint32_t gpio_swportd_ctl;               /*!< Offset 0x02C Регистр источника данных порта D. W/R 0x0 0x2C */
    volatile uint32_t gpio_inten;                     /*!< Offset 0x030 Регистр включения прерываний. W/R 0x0 0x30 */
    volatile uint32_t gpio_intmask;                   /*!< Offset 0x034 Регистр маскирования прерываний. W/R 0x0 0x34 */
    volatile uint32_t gpio_inttype_level;             /*!< Offset 0x038 Регистр уровня прерываний. W/R 0x0 0x38 */
    volatile uint32_t gpio_int_polarity;              /*!< Offset 0x03C Регистр полярности прерываний. W/R 0x0 0x3C */
    volatile uint32_t gpio_intstatus;                 /*!< Offset 0x040 Регистр статуса прерываний R 0x0 0x40 */
    volatile uint32_t gpio_raw_intstatus;             /*!< Offset 0x044 Регистр статуса немаскированных прерываний R 0x0 0x44 */
    volatile uint32_t gpio_debounce;                  /*!< Offset 0x048 Регистр включения фильтра дребезга. W/R 0x0 0x48 */
    volatile uint32_t gpio_porta_eoi;                 /*!< Offset 0x04C Регистр сброса прерываний. W 0x0 0x4C */
    volatile uint32_t gpio_ext_porta;                 /*!< Offset 0x050 Внешний регистр порта А. R 0x0 0x50 */
    volatile uint32_t gpio_ext_portb;                 /*!< Offset 0x054 Внешний регистр порта B. R 0x0 0x54 */
    volatile uint32_t gpio_ext_portc;                 /*!< Offset 0x058 Внешний регистр порта C. R 0x0 0x58 */
    volatile uint32_t gpio_ext_portd;                 /*!< Offset 0x05C Внешний регистр порта D. R 0x0 0x5C */
    volatile uint32_t gpio_ls_sync;                   /*!< Offset 0x060 Регистр включения синхронизации прерываний по уровню. W/R 0x0 0x60 */
} GPIOBLOCK_TypeDef; /* size of structure = 0x064 */
/*
 * @brief MFBSP
 */
/*!< MFBSP МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  */
typedef struct MFBSP_Type
{
    volatile uint32_t RX_TX_MFBSP;                    /*!< Offset 0x000 R Буфер приёма данных, W Буфер передачи данных */
    volatile uint32_t CSR_MFBSP;                      /*!< Offset 0x004 R/W Регистр управления и состояния */
    volatile uint32_t DIR_MFBSP;                      /*!< Offset 0x008 R/W Регистр управления направлением выводов порта ввода-вывода */
    volatile uint32_t GPIO_DR;                        /*!< Offset 0x00C R/W Регистр данных порта ввода-вывода */
    volatile uint32_t TCTR;                           /*!< Offset 0x010 R/W Регистр управления передатчиком */
    volatile uint32_t RCTR;                           /*!< Offset 0x014 R/W Регистр управления приёмником  */
    volatile uint32_t TSR;                            /*!< Offset 0x018 R/W Регистр состояния передатчика */
    volatile uint32_t RSR;                            /*!< Offset 0x01C R/W Регистр состояния приёмника */
    volatile uint32_t TCTR_RATE;                      /*!< Offset 0x020 (null) */
    volatile uint32_t RCTR_RATE;                      /*!< Offset 0x024 (null) */
    volatile uint32_t TSTART;                         /*!< Offset 0x028 R/W псевдорегистр ten – запуск/останов передатчика без изменения настроек передатчика */
    volatile uint32_t RSTART;                         /*!< Offset 0x02C R/W псевдорегистр ren – запуск/останов приемника без изменения настроек приемника */
    volatile uint32_t EMERG_MFBSP;                    /*!< Offset 0x030 R/W Регистр аварийного управления портом */
    volatile uint32_t IMASK_MFBSP;                    /*!< Offset 0x034 R/W Регистр маски прерываний от порта */
} MFBSP_TypeDef; /* size of structure = 0x038 */
/*
 * @brief NANDMPORT
 */
/*!< NANDMPORT КОНТРОЛЛЕР ПАМЯТИ ТИПА NAND FLASH (NANDMPORT) */
typedef struct NANDMPORT_Type
{
    volatile uint32_t PACKET;                         /*!< Offset 0x000 (null) */
    volatile uint32_t MEMADDR1;                       /*!< Offset 0x004 RW Регистр адреса 1 */
    volatile uint32_t MEMADDR2;                       /*!< Offset 0x008 RW Регистр адреса 2 */
    volatile uint32_t COMMAND;                        /*!< Offset 0x00C RW Регистр команды */
    volatile uint32_t PROGRAM;                        /*!< Offset 0x010 RW Регистр программы */
    volatile uint32_t INTERRUPT_STATUS_EN;            /*!< Offset 0x014 RW Регистр разрешения статуса прерываний */
    volatile uint32_t INTERRUPT_SIGNAL_EN;            /*!< Offset 0x018 RW Регистр разрешения сигнала прерываний */
    volatile uint32_t INTERRUPT_STATUS;               /*!< Offset 0x01C RW Регистр статуса прерываний */
    volatile uint32_t ID1;                            /*!< Offset 0x020 RO Регистр ID1 */
    volatile uint32_t ID2;                            /*!< Offset 0x024 RO Регистр ID2 */
    volatile uint32_t FLASH_STATUS;                   /*!< Offset 0x028 RO Регистр статуса флэш-памяти */
    volatile uint32_t TIMING;                         /*!< Offset 0x02C RW Регистр временных параметров */
    volatile uint32_t BUFFER_DATA;                    /*!< Offset 0x030 RW Регистр данных буфера */
    volatile uint32_t ECC;                            /*!< Offset 0x034 RW Регистр ECC */
    volatile uint32_t ECC_ERROR_CNT;                  /*!< Offset 0x038 RO Счётчик ошибок ECC */
    volatile uint32_t ECC_SPARE_CMD;                  /*!< Offset 0x03C RW Регистр команд ECC */
    volatile uint32_t ERROR_CNT_1BIT;                 /*!< Offset 0x040 RW Счётчик 1-битных ошибок */
    volatile uint32_t ERROR_CNT_2BIT;                 /*!< Offset 0x044 RW Счётчик 2-битных ошибок */
    volatile uint32_t ERROR_CNT_3BIT;                 /*!< Offset 0x048 RW Счётчик 3-битных ошибок */
    volatile uint32_t ERROR_CNT_4BIT;                 /*!< Offset 0x04C RW Счётчик 4-битных ошибок */
    volatile uint32_t DMA_SYS_ADDR;                   /*!< Offset 0x050 RW Системный адрес DMA */
    volatile uint32_t DMA_BUFFER_BOUNDARY;            /*!< Offset 0x054 RW Граница DMA пересылки */
    volatile uint32_t CPU_STATE;                      /*!< Offset 0x058 RW Регистр состояния CPU */
    volatile uint32_t ERROR_COUNT_5BIT;               /*!< Offset 0x05C RW Счётчик 5-битных ошибок */
    volatile uint32_t ERROR_COUNT_6BIT;               /*!< Offset 0x060 RW Счётчик 6-битных ошибок */
    volatile uint32_t ERROR_COUNT_7BIT;               /*!< Offset 0x064 RW Счётчик 7-битных ошибок */
    volatile uint32_t ERROR_COUNT_8BIT;               /*!< Offset 0x068 RW Счётчик 8-битных ошибок */
             uint32_t reserved_0x06C [0x0005];
    volatile uint32_t SLAVE_DMA_CFG;                  /*!< Offset 0x080 RW Регистр конфигурации Slave DMA */
} NANDMPORT_TypeDef; /* size of structure = 0x084 */
/*
 * @brief NORMPORT
 */
/*!< NORMPORT контроллер внешней SRAM/NOR Flash памяти. */
typedef struct NORMPORT_Type
{
             uint32_t reserved_0x000 [0x0400];
    volatile uint32_t SMC_MEMC_STATUS;                /*!< Offset 0x1000 RO Регистр состояния контроллера */
    volatile uint32_t SMC_MEMIF_CFG;                  /*!< Offset 0x1004 RO Регистр конфигурации интерфейса памяти */
    volatile uint32_t SMC_MEMC_CFG_SET;               /*!< Offset 0x1008 WO Регистр установки конфигурации контроллера */
    volatile uint32_t SMC_MEMC_CFG_CLR;               /*!< Offset 0x100C WO Регистр сброса конфигурации контроллера */
    volatile uint32_t SMC_DIRECT_CMD;                 /*!< Offset 0x1010 WO Регистр команд */
    volatile uint32_t SMC_SET_CYCLES;                 /*!< Offset 0x1014 WO Регистр настройки циклов доступа к памяти */
    volatile uint32_t SMC_SET_OPMODE;                 /*!< Offset 0x1018 WO Регистр настройки операции с памятью */
             uint32_t reserved_0x101C;
    volatile uint32_t SMC_REFRESH_PERIOD_0;           /*!< Offset 0x1020 RW Регистр периода обновления */
             uint32_t reserved_0x1024 [0x0037];
    volatile uint32_t SMC_SRAM_CYCLES0_0;             /*!< Offset 0x1100 RO Регистр параметров цикла доступа к памяти 0 */
    volatile uint32_t SMC_OPMODE0_0;                  /*!< Offset 0x1104 RO Регистр режима операции для памяти 0 */
             uint32_t reserved_0x1108 [0x0006];
    volatile uint32_t SMC_SRAM_CYCLES0_1;             /*!< Offset 0x1120 RO Регистр параметров цикла доступа к памяти 1 */
    volatile uint32_t SMC_OPMODE0_1;                  /*!< Offset 0x1124 RO Регистр режима операции для памяти 1 */
             uint32_t reserved_0x1128 [0x03AE];
    volatile uint32_t SMC_PERIPH_ID_0;                /*!< Offset 0x1FE0 RO ID регистр периферии 0 */
    volatile uint32_t SMC_PERIPH_ID_1;                /*!< Offset 0x1FE4 RO ID регистр периферии 1 */
    volatile uint32_t SMC_PERIPH_ID_2;                /*!< Offset 0x1FE8 RO ID регистр периферии 2 */
    volatile uint32_t SMC_PERIPH_ID_4;                /*!< Offset 0x1FEC RO ID регистр периферии 3 */
    volatile uint32_t SMC_PCELL_ID_0;                 /*!< Offset 0x1FF0 RO ID регистр компонента 0 */
    volatile uint32_t SMC_PCELL_ID_1;                 /*!< Offset 0x1FF4 RO ID регистр компонента 1 */
    volatile uint32_t SMC_PCELL_ID_2;                 /*!< Offset 0x1FF8 RO ID регистр компонента 2 */
    volatile uint32_t SMC_PCELL_ID_3;                 /*!< Offset 0x1FFC RO ID регистр компонента 3 */
             uint32_t reserved_0x2000 [0x0003];
    volatile uint32_t STATIC_MEM_CFG_0;               /*!< Offset 0x200C RW Регистр настройки внешней памяти */
             uint32_t reserved_0x2010;
    volatile uint32_t ADDR_CFG_0;                     /*!< Offset 0x2014 RW Регистр настройки адреса 0 */
    volatile uint32_t ADDR_CFG_1;                     /*!< Offset 0x2018 RW Регистр настройки адреса 1 */
             uint32_t reserved_0x201C [0x0002];
    volatile uint32_t BYTE_ORDER;                     /*!< Offset 0x2024 RW Регистр настройки порядка байтов */
} NORMPORT_TypeDef; /* size of structure = 0x2028 */
/*
 * @brief PMCTR
 */
/*!< PMCTR Контроллер управления энергопотреблением */
typedef struct PMCTR_Type
{
             uint32_t reserved_0x000 [0x0003];
    volatile uint32_t SYS_PWR_STATUS;                 /*!< Offset 0x00C Регистр статуса доменов */
    volatile uint32_t SYS_PWR_IMASK;                  /*!< Offset 0x010 Регистр маски прерывания SYS_PWR_INT */
    volatile uint32_t SYS_PWR_IRSTAT;                 /*!< Offset 0x014 Регистр наличного статуса прерывания SYS_PWR_ISTAT */
    volatile uint32_t SYS_PWR_INT;                    /*!< Offset 0x018 Регистр статуса прерывания SYS_PWR_INT */
    volatile uint32_t SYS_PWR_ICLR;                   /*!< Offset 0x01C Регистр очистки статуса прерывания SYS_PWR_INT */
    volatile uint32_t SYS_PWR_DELAY;                  /*!< Offset 0x020 Регистр задания задержек автоматов */
    volatile uint32_t DDR_PIN_RET;                    /*!< Offset 0x024 Регистр перевода выводов DDR в режим хранения состояния */
    volatile uint32_t DDR_INIT_END;                   /*!< Offset 0x028 Регистр вывода контроллеров DDRMC0, DDRMC1 из режима инициализации */
    volatile uint32_t WARM_RST_EN;                    /*!< Offset 0x02C Регистр разрешения «теплого» сброса */
             uint32_t reserved_0x030 [0x0004];
    volatile uint32_t SW_RST;                         /*!< Offset 0x040 Регистр программного «теплого» сброса */
    volatile uint32_t WARM_RST_STATUS;                /*!< Offset 0x044 Регистр статуса последнего сброса */
    volatile uint32_t PDM_RST_STATUS;                 /*!< Offset 0x048 Регистр статуса сброса доменов питания */
    volatile uint32_t NVMODE;                         /*!< Offset 0x04C Регистр управления сигналами NVMODE */
    volatile uint32_t CPU0_WKP_MASK [0x004];          /*!< Offset 0x050 Регистр маски включения домена CPU0 по прерываниям */
    volatile uint32_t CPU1_WKP_MASK [0x004];          /*!< Offset 0x060 Регистр маски включения домена CPU1 по прерываниям */
    volatile uint32_t ALWAYS_MISC0;                   /*!< Offset 0x070 Регистр общего назначения, не сбрасываемый при «теплом» сбросе */
    volatile uint32_t ALWAYS_MISC1;                   /*!< Offset 0x074 Регистр общего назначения, не сбрасываемый при «теплом» сбросе */
    volatile uint32_t WARM_BOOT_OVRD;                 /*!< Offset 0x078 Регистр управления загрузкой при «теплом» сбросе */
             uint32_t reserved_0x07C;
    volatile uint32_t CORE_PWR_UP;                    /*!< Offset 0x080 Регистр включения доменов */
    volatile uint32_t CORE_PWR_DOWN;                  /*!< Offset 0x084 Регистр выключения доменов */
             uint32_t reserved_0x088;
    volatile uint32_t CORE_PWR_STATUS;                /*!< Offset 0x08C Регистр статуса доменов */
    volatile uint32_t CORE_PWR_IMASK;                 /*!< Offset 0x090 Регистр маски прерывания CORE_PWR_INT */
    volatile uint32_t CORE_PWR_IRSTAT;                /*!< Offset 0x094 Регистр наличного статуса прерывания CORE_PWR_INT */
    volatile uint32_t CORE_PWR_ISTAT;                 /*!< Offset 0x098 Регистр статуса прерывания CORE_PWR_INT */
    volatile uint32_t CORE_PWR_ICLR;                  /*!< Offset 0x09C Регистр очистки статуса прерывания CORE_PWR_INT */
    volatile uint32_t CORE_PWR_DELAY;                 /*!< Offset 0x0A0 Регистр задания задержек автоматов доменов */
} PMCTR_TypeDef; /* size of structure = 0x0A4 */
/*
 * @brief SDMA
 */
/*!< SDMA Регистры блока управления SDMA */
typedef struct SDMA_Type
{
    volatile uint32_t DSR;                            /*!< Offset 0x000 RO Состояние потока управления */
    volatile uint32_t DPC;                            /*!< Offset 0x004 RO Счётчик команд потока управления */
             uint32_t reserved_0x008 [0x0006];
    volatile uint32_t INTEN;                          /*!< Offset 0x020 RW Разрешение прерываний */
    volatile uint32_t INT_EVENT_RIS;                  /*!< Offset 0x024 RO Статус прерываний/событий */
    volatile uint32_t INTMIS;                         /*!< Offset 0x028 RO Статус прерываний */
    volatile uint32_t INTCLR;                         /*!< Offset 0x02C WO Сброс прерываний */
    volatile uint32_t FSRD;                           /*!< Offset 0x030 RO Сбой потока управления */
    volatile uint32_t FSRC;                           /*!< Offset 0x034 RO Сбой потоков каналов */
    volatile uint32_t FTRD;                           /*!< Offset 0x038 RO Тип сбоя потока управления */
             uint32_t reserved_0x03C;
    volatile uint32_t FTR0;                           /*!< Offset 0x040 RO Тип ошибки канала 0 */
    volatile uint32_t FTR1;                           /*!< Offset 0x044 RO Тип ошибки канала 1 */
    volatile uint32_t FTR2;                           /*!< Offset 0x048 RO Тип ошибки канала 2 */
    volatile uint32_t FTR3;                           /*!< Offset 0x04C RO Тип ошибки канала 3 */
    volatile uint32_t FTR4;                           /*!< Offset 0x050 RO Тип ошибки канала 4 */
    volatile uint32_t FTR5;                           /*!< Offset 0x054 RO Тип ошибки канала 5 */
    volatile uint32_t FTR6;                           /*!< Offset 0x058 RO Тип ошибки канала 6 */
    volatile uint32_t FTR7;                           /*!< Offset 0x05C RO Тип ошибки канала 7 */
             uint32_t reserved_0x060 [0x0028];
    volatile uint32_t CSR0;                           /*!< Offset 0x100 RO Статус канала 0 */
    volatile uint32_t CPC0;                           /*!< Offset 0x104 RO PC канала 0 */
    volatile uint32_t CSR1;                           /*!< Offset 0x108 RO Статус канала 1 */
    volatile uint32_t CPC1;                           /*!< Offset 0x10C RO PC канала 1 */
    volatile uint32_t CSR2;                           /*!< Offset 0x110 RO Статус канала 2 */
    volatile uint32_t CPC2;                           /*!< Offset 0x114 RO PC канала 2 */
    volatile uint32_t CSR3;                           /*!< Offset 0x118 RO Статус канала 3 */
    volatile uint32_t CPC3;                           /*!< Offset 0x11C RO PC канала 3 */
    volatile uint32_t CSR4;                           /*!< Offset 0x120 RO Статус канала 4 */
    volatile uint32_t CPC4;                           /*!< Offset 0x124 RO PC канала 4 */
    volatile uint32_t CSR5;                           /*!< Offset 0x128 RO Статус канала 5 */
    volatile uint32_t CPC5;                           /*!< Offset 0x12C RO PC канала 5 */
    volatile uint32_t CSR6;                           /*!< Offset 0x130 RO Статус канала 6 */
    volatile uint32_t CPC6;                           /*!< Offset 0x134 RO PC канала 6 */
    volatile uint32_t CSR7;                           /*!< Offset 0x138 RO Статус канала 7 */
    volatile uint32_t CPC7;                           /*!< Offset 0x13C RO PC канала 7 */
             uint32_t reserved_0x140 [0x00B0];
    volatile uint32_t SAR0;                           /*!< Offset 0x400 RO Адрес Источника для канала 0 */
    volatile uint32_t DAR0;                           /*!< Offset 0x404 RO Адрес Приёмника для канала 0 */
    volatile uint32_t CCR0;                           /*!< Offset 0x408 RO Регистр управления для канала 0 */
    volatile uint32_t LC0_0;                          /*!< Offset 0x40C RO Счётчик циклов 0 для канала 0 */
    volatile uint32_t LC1_0;                          /*!< Offset 0x410 RO Счётчик циклов 1 для канала 0 */
             uint32_t reserved_0x414 [0x0003];
    volatile uint32_t SAR1;                           /*!< Offset 0x420 RO Адрес Источника для канала 1 */
    volatile uint32_t DAR1;                           /*!< Offset 0x424 RO Адрес Приёмника для канала 1 */
    volatile uint32_t CCR1;                           /*!< Offset 0x428 RO Регистр управления для канала 1 */
    volatile uint32_t LC0_1;                          /*!< Offset 0x42C RO Счётчик циклов 0 для канала 1 */
    volatile uint32_t LC1_1;                          /*!< Offset 0x430 RO Счётчик циклов 1 для канала 1 */
             uint32_t reserved_0x434 [0x0003];
    volatile uint32_t SAR2;                           /*!< Offset 0x440 RO Адрес Источника для канала 2 */
    volatile uint32_t DAR2;                           /*!< Offset 0x444 RO Адрес Приёмника для канала 2 */
    volatile uint32_t CCR2;                           /*!< Offset 0x448 RO Регистр управления для канала 2 */
    volatile uint32_t LC0_2;                          /*!< Offset 0x44C RO Счётчик циклов 0 для канала 2 */
    volatile uint32_t LC1_2;                          /*!< Offset 0x450 RO Счётчик циклов 1 для канала 2 */
             uint32_t reserved_0x454 [0x0003];
    volatile uint32_t SAR3;                           /*!< Offset 0x460 RO Адрес Источника для канала 3 */
    volatile uint32_t DAR3;                           /*!< Offset 0x464 RO Адрес Приёмника для канала 3 */
    volatile uint32_t CCR3;                           /*!< Offset 0x468 RO Регистр управления для канала 3 */
    volatile uint32_t LC0_3;                          /*!< Offset 0x46C RO Счётчик циклов 0 для канала 3 */
    volatile uint32_t LC1_3;                          /*!< Offset 0x470 RO Счётчик циклов 1 для канала 3 */
             uint32_t reserved_0x474 [0x0003];
    volatile uint32_t SAR4;                           /*!< Offset 0x480 RO Адрес Источника для канала 4 */
    volatile uint32_t DAR4;                           /*!< Offset 0x484 RO Адрес Приёмника для канала 4 */
    volatile uint32_t CCR4;                           /*!< Offset 0x488 RO Регистр управления для канала 4 */
    volatile uint32_t LC0_4;                          /*!< Offset 0x48C RO Счётчик циклов 0 для канала 4 */
    volatile uint32_t LC1_4;                          /*!< Offset 0x490 RO Счётчик циклов 1 для канала 4 */
             uint32_t reserved_0x494 [0x0003];
    volatile uint32_t SAR5;                           /*!< Offset 0x4A0 RO Адрес Источника для канала 5 */
    volatile uint32_t DAR5;                           /*!< Offset 0x4A4 RO Адрес Приёмника для канала 5 */
    volatile uint32_t CCR5;                           /*!< Offset 0x4A8 RO Регистр управления для канала 5 */
    volatile uint32_t LC0_5;                          /*!< Offset 0x4AC RO Счётчик циклов 0 для канала 5 */
    volatile uint32_t LC1_5;                          /*!< Offset 0x4B0 RO Счётчик циклов 1 для канала 5 */
             uint32_t reserved_0x4B4 [0x0003];
    volatile uint32_t SAR6;                           /*!< Offset 0x4C0 RO Адрес Источника для канала 6 */
    volatile uint32_t DAR6;                           /*!< Offset 0x4C4 RO Адрес Приёмника для канала 6 */
    volatile uint32_t CCR6;                           /*!< Offset 0x4C8 RO Регистр управления для канала 6 */
    volatile uint32_t LC0_6;                          /*!< Offset 0x4CC RO Счётчик циклов 0 для канала 6 */
    volatile uint32_t LC1_6;                          /*!< Offset 0x4D0 RO Счётчик циклов 1 для канала 6 */
             uint32_t reserved_0x4D4 [0x0003];
    volatile uint32_t SAR7;                           /*!< Offset 0x4E0 RO Адрес Источника для канала 7 */
    volatile uint32_t DAR7;                           /*!< Offset 0x4E4 RO Адрес Приёмника для канала 7 */
    volatile uint32_t CCR7;                           /*!< Offset 0x4E8 RO Регистр управления для канала 7 */
    volatile uint32_t LC0_7;                          /*!< Offset 0x4EC RO Счётчик циклов 0 для канала 7 */
    volatile uint32_t LC1_7;                          /*!< Offset 0x4F0 RO Счётчик циклов 1 для канала 7 */
             uint32_t reserved_0x4F4 [0x0203];
    volatile uint32_t DBGSTATUS;                      /*!< Offset 0xD00 RO Debug Status Register */
    volatile uint32_t DBGCMD;                         /*!< Offset 0xD04 WO Debug Command Register */
    volatile uint32_t DBGINST0;                       /*!< Offset 0xD08 WO 0-ой регистр отладки */
    volatile uint32_t DBGINST1;                       /*!< Offset 0xD0C WO 1-ый регистр отладки */
             uint32_t reserved_0xD10 [0x003C];
    volatile uint32_t CR0;                            /*!< Offset 0xE00 RO Регистр конфигурации 0 */
    volatile uint32_t CR1;                            /*!< Offset 0xE04 RO Регистр конфигурации 1 */
    volatile uint32_t CR2;                            /*!< Offset 0xE08 RO Регистр конфигурации 2 */
    volatile uint32_t CR3;                            /*!< Offset 0xE0C RO Регистр конфигурации 3 */
    volatile uint32_t CR4;                            /*!< Offset 0xE10 RO Регистр конфигурации 4 */
    volatile uint32_t CRD;                            /*!< Offset 0xE14 RO Конфигурация SDMA */
             uint32_t reserved_0xE18 [0x001A];
    volatile uint32_t WD;                             /*!< Offset 0xE80 RW Сторожевой регистр */
             uint32_t reserved_0xE84 [0x0057];
    volatile uint32_t periph_id_n;                    /*!< Offset 0xFE0 RO ID регистры периферии. */
             uint32_t reserved_0xFE4 [0x0003];
    volatile uint32_t pcell_id_n;                     /*!< Offset 0xFF0 RO ID регистры компонента. */
} SDMA_TypeDef; /* size of structure = 0xFF4 */
/*
 * @brief SMCTR
 */
/*!< SMCTR общиме системные настройки микросхемы */
typedef struct SMCTR_Type
{
    volatile uint32_t BOOT;                           /*!< Offset 0x000 Регистр отображения сигналов BOOT[1:0] микросхемы */
    volatile uint32_t BOOT_REMAP;                     /*!< Offset 0x004 Регистр управления картой памяти при начальной загрузке */
    volatile uint32_t MPU_CFGNMFI;                    /*!< Offset 0x008 Регистр разрешения немаскируемого FIQ в MPU */
    volatile uint32_t DDR_REMAP;                      /*!< Offset 0x00C Регистр управления картой памяти для DDR */
             uint32_t reserved_0x010 [0x0005];
    volatile uint32_t MIPI_MUX;                       /*!< Offset 0x024 Регистр мультиплексирования MIPI DSI */
    volatile uint32_t CHIP_ID;                        /*!< Offset 0x028 ID микросхемы */
    volatile uint32_t CHIP_CONFIG;                    /*!< Offset 0x02C Регистр конфигурации микросхемы */
    volatile uint32_t EMA_ARM;                        /*!< Offset 0x030 Регистр подстройки памятей в ARM MPU */
    volatile uint32_t EMA_L2;                         /*!< Offset 0x034 Регистр подстройки памятей в L2CACHE */
    volatile uint32_t EMA_DSP;                        /*!< Offset 0x038 Регистр подстройки памятей в DSP и VPU */
    volatile uint32_t EMA_CORE;                       /*!< Offset 0x03C Регистр подстройки памятей в CORE */
    volatile uint32_t IOPULL_CTR;                     /*!< Offset 0x040 Регистры управления подтягивающими резисторами контактных площадок микросхемы */
    volatile uint32_t COMM_DLOCK;                     /*!< Offset 0x044 Регистр индикации зависания коммутатора микросхемы */
} SMCTR_TypeDef; /* size of structure = 0x048 */
/*
 * @brief UART
 */
/*!< UART Universal Asynchronous Receiver-Transmitter */
typedef struct UART_Type
{
    volatile uint32_t UART_RBR_THR_DLL;               /*!< Offset 0x000 UART Receive Buffer Register/Transmit Holding Register */
    volatile uint32_t UART_DLH_IER;                   /*!< Offset 0x004  */
    volatile uint32_t UART_IIR_FCR;                   /*!< Offset 0x008 UART Interrupt Identity Register/UART FIFO Control Register */
    volatile uint32_t UART_LCR;                       /*!< Offset 0x00C UART Line Control Register */
    volatile uint32_t UART_MCR;                       /*!< Offset 0x010 UART Modem Control Register */
    volatile uint32_t UART_LSR;                       /*!< Offset 0x014 UART Line Status Register */
    volatile uint32_t UART_MSR;                       /*!< Offset 0x018 UART Modem Status Register */
    volatile uint32_t UART_SCR;                       /*!< Offset 0x01C UART Scratch Register */
             uint32_t reserved_0x020 [0x0004];
    volatile uint32_t UART_SRBR_STHR [0x010];         /*!< Offset 0x030  */
             uint32_t reserved_0x070 [0x0003];
    volatile uint32_t UART_USR;                       /*!< Offset 0x07C  */
    volatile uint32_t UART_TFL;                       /*!< Offset 0x080  */
    volatile uint32_t UART_RFL;                       /*!< Offset 0x084  */
    volatile uint32_t UART_SRR;                       /*!< Offset 0x088  */
    volatile uint32_t UART_SRTS;                      /*!< Offset 0x08C  */
    volatile uint32_t UART_SBCR;                      /*!< Offset 0x090 check address (was: 0x80)  */
             uint32_t reserved_0x094;
    volatile uint32_t UART_SFE;                       /*!< Offset 0x098  */
    volatile uint32_t UART_SRT;                       /*!< Offset 0x09C  */
    volatile uint32_t UART_STET;                      /*!< Offset 0x0A0  */
    volatile uint32_t UART_HTX;                       /*!< Offset 0x0A4  */
} UART_TypeDef; /* size of structure = 0x0A8 */


/* Access pointers */

#define SDMA ((SDMA_TypeDef *) SDMA_BASE)             /*!< SDMA Регистры блока управления SDMA register set access pointer */
#define NANDMPORT ((NANDMPORT_TypeDef *) NANDMPORT_BASE)/*!< NANDMPORT КОНТРОЛЛЕР ПАМЯТИ ТИПА NAND FLASH (NANDMPORT) register set access pointer */
#define NORMPORT ((NORMPORT_TypeDef *) NORMPORT_BASE) /*!< NORMPORT контроллер внешней SRAM/NOR Flash памяти. register set access pointer */
#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0 Universal Asynchronous Receiver-Transmitter register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1 Universal Asynchronous Receiver-Transmitter register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2 Universal Asynchronous Receiver-Transmitter register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3 Universal Asynchronous Receiver-Transmitter register set access pointer */
#define GPIO0 ((GPIOBLOCK_TypeDef *) GPIO0_BASE)      /*!< GPIO0 Регистры блока управления GPIO register set access pointer */
#define GPIOA ((GPIO_TypeDef *) GPIOA_BASE)           /*!< GPIOA Регистры блока управления GPIO register set access pointer */
#define GPIOB ((GPIO_TypeDef *) GPIOB_BASE)           /*!< GPIOB Регистры блока управления GPIO register set access pointer */
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)           /*!< GPIOC Регистры блока управления GPIO register set access pointer */
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE)           /*!< GPIOD Регистры блока управления GPIO register set access pointer */
#define MFBSP0 ((MFBSP_TypeDef *) MFBSP0_BASE)        /*!< MFBSP0 МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  register set access pointer */
#define MFBSP1 ((MFBSP_TypeDef *) MFBSP1_BASE)        /*!< MFBSP1 МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  register set access pointer */
#define CMCTR ((CMCTR_TypeDef *) CMCTR_BASE)          /*!< CMCTR Контроллер управления синхронизацией register set access pointer */
#define PMCTR ((PMCTR_TypeDef *) PMCTR_BASE)          /*!< PMCTR Контроллер управления энергопотреблением register set access pointer */
#define SMCTR ((SMCTR_TypeDef *) SMCTR_BASE)          /*!< SMCTR общиме системные настройки микросхемы register set access pointer */


#endif /* HEADER_00003039_INCLUDED */
