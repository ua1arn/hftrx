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
    MFBSP0_DMAIRQ0_IRQn = 52,                         /*!< SWIC_MFBSP_DMA MFBSP0 */
    MFBSP0_DMAIRQ1_IRQn = 53,                         /*!< SWIC_MFBSP_DMA MFBSP0 */
    MFBSP1_DMAIRQ0_IRQn = 54,                         /*!< SWIC_MFBSP_DMA MFBSP1 */
    MFBSP1_DMAIRQ1_IRQn = 55,                         /*!< SWIC_MFBSP_DMA MFBSP0 */
    NFC_MPORTINT_IRQn = 71,                           /*!< NANDMPORT КОНТРОЛЛЕР ПАМЯТИ ТИПА NAND FLASH (NANDMPORT) */
    SWIC0DMA_IRQ0_IRQn = 80,                          /*!< SWIC_MFBSP_DMA SWIC0 */
    SWIC0DMA_IRQ1_IRQn = 81,                          /*!< SWIC_MFBSP_DMA SWIC0 */
    SWIC0DMA_IRQ2_IRQn = 82,                          /*!< SWIC_MFBSP_DMA SWIC0 */
    SWIC0DMA_IRQ3_IRQn = 83,                          /*!< SWIC_MFBSP_DMA SWIC0 */
    SWIC1DMA_IRQ0_IRQn = 84,                          /*!< SWIC_MFBSP_DMA SWIC1 */
    SWIC1DMA_IRQ1_IRQn = 85,                          /*!< SWIC_MFBSP_DMA SWIC1 */
    SWIC1DMA_IRQ2_IRQn = 86,                          /*!< SWIC_MFBSP_DMA SWIC1 */
    SWIC1DMA_IRQ3_IRQn = 87,                          /*!< SWIC_MFBSP_DMA SWIC1 */
    SWIC0_INT_IRQn = 88,                              /*!< SWIC КОНТРОЛЛЕР ИНТЕРФЕЙСА SPACEWIRE (SWIC) */
    SWIC1_INT_IRQn = 89,                              /*!< SWIC КОНТРОЛЛЕР ИНТЕРФЕЙСА SPACEWIRE (SWIC) */
    MFBSP0_RXIRQ_IRQn = 90,                           /*!< MFBSP МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  */
    MFBSP0_TXIRQ_IRQn = 91,                           /*!< MFBSP МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  */
    MFBSP0_SRQ_IRQn = 92,                             /*!< MFBSP МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  */
    MFBSP1_RXIRQ_IRQn = 93,                           /*!< MFBSP МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  */
    MFBSP1_TXIRQ_IRQn = 94,                           /*!< MFBSP МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  */
    MFBSP1_SRQ_IRQn = 95,                             /*!< MFBSP МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  */
    UART0_IRQn = 96,                                  /*!< UART Universal Asynchronous Receiver-Transmitter */
    UART1_IRQn = 97,                                  /*!< UART Universal Asynchronous Receiver-Transmitter */
    UART2_IRQn = 98,                                  /*!< UART Universal Asynchronous Receiver-Transmitter */
    UART3_IRQn = 99,                                  /*!< UART Universal Asynchronous Receiver-Transmitter */
    I2C0_IC_INTR_IRQn = 100,                          /*!< I2C  */
    I2C1_IC_INTR_IRQn = 101,                          /*!< I2C  */
    I2C2_IC_INTR_IRQn = 102,                          /*!< I2C  */
    I2S0_IRQn = 104,                                  /*!< I2S Прерывание от I2S0 */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define SDMA_BASE ((uintptr_t) 0x37220000)            /*!< SDMA Регистры блока управления SDMA Base */
#define NANDMPORT_BASE ((uintptr_t) 0x38007000)       /*!< NANDMPORT КОНТРОЛЛЕР ПАМЯТИ ТИПА NAND FLASH (NANDMPORT) Base */
#define NFCM_BASE ((uintptr_t) 0x38007000)            /*!< NANDMPORT КОНТРОЛЛЕР ПАМЯТИ ТИПА NAND FLASH (NANDMPORT) Base */
#define NORMPORT_BASE ((uintptr_t) 0x38008000)        /*!< NORMPORT контроллер внешней SRAM/NOR Flash памяти. Base */
#define UART0_BASE ((uintptr_t) 0x38028000)           /*!< UART Universal Asynchronous Receiver-Transmitter Base */
#define UART1_BASE ((uintptr_t) 0x38029000)           /*!< UART Universal Asynchronous Receiver-Transmitter Base */
#define UART2_BASE ((uintptr_t) 0x3802A000)           /*!< UART Universal Asynchronous Receiver-Transmitter Base */
#define UART3_BASE ((uintptr_t) 0x3802B000)           /*!< UART Universal Asynchronous Receiver-Transmitter Base */
#define I2C0_BASE ((uintptr_t) 0x3802C000)            /*!< I2C  Base */
#define I2C1_BASE ((uintptr_t) 0x3802D000)            /*!< I2C  Base */
#define I2C2_BASE ((uintptr_t) 0x3802E000)            /*!< I2C  Base */
#define I2S0_BASE ((uintptr_t) 0x38030000)            /*!< I2S Контроллер I2S предназначен для обмена аудио-данными в стерео-формате по последовательной шине I2S. Base */
#define GPIO0_BASE ((uintptr_t) 0x38034000)           /*!< GPIOBLOCK Регистры блока управления GPIO Base */
#define GPIOA_BASE ((uintptr_t) 0x38034000)           /*!< GPIO Регистры блока управления GPIO Base */
#define GPIOB_BASE ((uintptr_t) 0x3803400C)           /*!< GPIO Регистры блока управления GPIO Base */
#define GPIOC_BASE ((uintptr_t) 0x38034018)           /*!< GPIO Регистры блока управления GPIO Base */
#define GPIOD_BASE ((uintptr_t) 0x38034024)           /*!< GPIO Регистры блока управления GPIO Base */
#define SWIC0_BASE ((uintptr_t) 0x38082000)           /*!< SWIC КОНТРОЛЛЕР ИНТЕРФЕЙСА SPACEWIRE (SWIC) Base */
#define DMA_SWIC0_BASE ((uintptr_t) 0x38083000)       /*!< SWIC_MFBSP_DMA Регистры встроенных DMA портов SWIC и MFBSP Base */
#define SWIC1_BASE ((uintptr_t) 0x38084000)           /*!< SWIC КОНТРОЛЛЕР ИНТЕРФЕЙСА SPACEWIRE (SWIC) Base */
#define DMA_SWIC1_BASE ((uintptr_t) 0x38085000)       /*!< SWIC_MFBSP_DMA Регистры встроенных DMA портов SWIC и MFBSP Base */
#define MFBSP0_BASE ((uintptr_t) 0x38086000)          /*!< MFBSP МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  Base */
#define DMA_MFBSP0_BASE ((uintptr_t) 0x38087000)      /*!< SWIC_MFBSP_DMA Регистры встроенных DMA портов SWIC и MFBSP Base */
#define MFBSP1_BASE ((uintptr_t) 0x38088000)          /*!< MFBSP МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  Base */
#define DMA_MFBSP1_BASE ((uintptr_t) 0x38089000)      /*!< SWIC_MFBSP_DMA Регистры встроенных DMA портов SWIC и MFBSP Base */
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
 * @brief I2C
 */
/*!< I2C  */
typedef struct I2C_Type
{
    volatile uint32_t IC_CON;                         /*!< Offset 0x000 Регистр управления W/R (R для 4-ого бита) 0x7F 0x0 */
    volatile uint32_t IC_TAR;                         /*!< Offset 0x004 Регистр адреса абонента W/R 0x1055 0x4 */
    volatile uint32_t IC_SAR;                         /*!< Offset 0x008 Регистр slave-адреса. W/R 0x55 0x8 */
    volatile uint32_t IC_HS_MADD;                     /*!< Offset 0x00C Регистр кода адреса мастера для high speed режима. W/R 0x1 0xС */
    volatile uint32_t IC_DATA_CMD;                    /*!< Offset 0x010 Регистр управления передачей. W/R 0x0 0x10 */
    volatile uint32_t IC_SS_SCL_HCNT;                 /*!< Offset 0x014 Старший регистр счетчика предделителя частоты для standard.speed режима. W/R 0x190 0x14 */
    volatile uint32_t IC_SS_SCL_LCNT;                 /*!< Offset 0x018 Младший регистр счетчика предделителя частоты для standard.speed режима. W/R 0x1d6 0x18 */
    volatile uint32_t IC_FS_SCL_HCNT;                 /*!< Offset 0x01C Старший регистр счетчика предделителя частоты для fast-speed режима. W/R 0x3c 0x1C */
    volatile uint32_t IC_FS_SCL_LCNT;                 /*!< Offset 0x020 Младший регистр счетчика предделителя частоты для fast-speed режима. W/R 0x82 0x20 */
    volatile uint32_t IC_HS_SCL_HCNT;                 /*!< Offset 0x024 Старший регистр счетчика предделителя частоты для high-speed режима. W/R 0x6 0x24 */
    volatile uint32_t IC_HS_SCL_LCNT;                 /*!< Offset 0x028 Младший регистр счетчика предделителя частоты для high-speed режима. W/R 0x10 0x28 */
    volatile uint32_t IC_INTR_STAT;                   /*!< Offset 0x02C Регистр статуса прерывания. R 0x0 0x2C */
    volatile uint32_t IC_INTR_MASK;                   /*!< Offset 0x030 Регистр маски прерывания W/R 0x8ff 0X30 */
    volatile uint32_t IC_RAW_INTR_STAT;               /*!< Offset 0x034 Регистр статуса необработанного прерывания R 0x0 0x34 */
    volatile uint32_t IC_RX_TL;                       /*!< Offset 0x038 Регистр порога заполнения FIFO приемника. W/R 0x0 0x38 */
    volatile uint32_t IC_TX_TL;                       /*!< Offset 0x03C Регистр порога заполнения FIFO передатчика. W/R 0x0 0x3C */
    volatile uint32_t IC_CLT_INTR;                    /*!< Offset 0x040 Регистр сброса прерываний. R 0x0 0x40 */
    volatile uint32_t C_CLR_RX_UNDER;                 /*!< Offset 0x044 Регистр сброса прерывания RX_UNDER R 0x0 0x44 */
    volatile uint32_t IC_CLR_RX_OVER;                 /*!< Offset 0x048 Регистр сброса прерывания RX_OVER R 0x0 0x48 */
    volatile uint32_t IC_CLR_TX_OVER;                 /*!< Offset 0x04C Регистр сброса прерывания TX_OVER R 0x0 0x4C */
    volatile uint32_t IC_CLR_RD_REQ;                  /*!< Offset 0x050 Регистр сброса прерывания RD_REQ R 0x0 0x50 */
    volatile uint32_t IC_CLR_TX_ABR;                  /*!< Offset 0x054 Регистр сброса прерывания TX_ABR R 0x0 0x54 */
    volatile uint32_t IC_CLR_RX_DONE;                 /*!< Offset 0x058 Регистр сброса прерывания RX_DONE R 0x0 0x58 */
    volatile uint32_t IC_CLR_ACTIVITY;                /*!< Offset 0x05C Регистр сброса прерывания ACTIVITY R 0x0 0x5C */
    volatile uint32_t IC_CLR_STOP_DET;                /*!< Offset 0x060 Регистр сброса прерывания STOP_DET R 0x0 0x60 */
    volatile uint32_t IC_CLR_START_DET;               /*!< Offset 0x064 Регистр сброса прерывания START_DET R 0x0 0x64 */
    volatile uint32_t IC_CLR_GEN_CALL;                /*!< Offset 0x068 Регистр сброса прерывания GEN_CALL R 0x0 0x68 */
    volatile uint32_t IC_ENABLE;                      /*!< Offset 0x06C Регистр включения шины. W/R 0x0 0x6C */
    volatile uint32_t IC_STATUS;                      /*!< Offset 0x070 Регистр статуса шины. R 0x6 0x70 */
    volatile uint32_t IC_TXFLR;                       /*!< Offset 0x074 Регистр уровня FIFO передатчика R 0x0 0x74 */
    volatile uint32_t IC_RXFLR;                       /*!< Offset 0x078 Регистр уровня FIFO приемника R 0x0 0x78 */
    volatile uint32_t IC_SDA_HOLD;                    /*!< Offset 0x07C Регистр времени удержания SDA. W/R 0x1 0x7C */
    volatile uint32_t IC_TX_ABRT_SOURCE;              /*!< Offset 0x080 Регистр статуса обрыва передачи. R 0x0 0x80 */
    volatile uint32_t IC_SLV_DATA_NACK_ONLY;          /*!< Offset 0x084 Регистр генерирования SLV_DATA_NACK W/R 0x0 0x84 */
    volatile uint32_t IC_DMA_CR;                      /*!< Offset 0x088 Регистр контроля DMA интерфейса. W/R 0x0 0x88 */
    volatile uint32_t IC_DMA_TDLR;                    /*!< Offset 0x08C Регистр передачи данных через DMA интерфейс. W/R 0x0 0x8C */
    volatile uint32_t IC_DMA_RDLR;                    /*!< Offset 0x090 Регистр приема данных через DMA интерфейс. W/R 0x0 0x90 */
    volatile uint32_t IC_SDA_SETUP;                   /*!< Offset 0x094 Регистр установки SDA W/R 0x64 0x94 */
    volatile uint32_t IC_ACK_GENERAL_CALL;            /*!< Offset 0x098 Регистр вызова общего ACK W/R 0x1 0x98 */
    volatile uint32_t IC_ENABLE_STATUS;               /*!< Offset 0x09C Регистр статуса включения R 0x0 0x9C */
    volatile uint32_t IC_FS_SPKLEN;                   /*!< Offset 0x0A0 Регистр управления фильтрацией для standard-speed и fast.speed режимов W/R 0x5 0xA0 */
    volatile uint32_t IC_HS_SPKLEN;                   /*!< Offset 0x0A4 Регистр управления фильтрацией для high.speed режима. W/R 0x1 0xA4 */
} I2C_TypeDef; /* size of structure = 0x0A8 */
/*
 * @brief I2S
 */
/*!< I2S Контроллер I2S предназначен для обмена аудио-данными в стерео-формате по последовательной шине I2S. */
typedef struct I2S_Type
{
    volatile uint32_t IER;                            /*!< Offset 0x000 Регистр включения контроллера. W/R 0x0 0x0 */
    volatile uint32_t IRER;                           /*!< Offset 0x004 Регистр включения блока приема. W/R 0x0 0x4 */
    volatile uint32_t ITER;                           /*!< Offset 0x008 Регистр включения блока передачи. W/R 0x0 0x8 */
    volatile uint32_t CER;                            /*!< Offset 0x00C Регистр включения тактовой частоты. W/R 0x0 0xC */
    volatile uint32_t CCR;                            /*!< Offset 0x010 Регистр конфигурации тактовой частоты. W/R 0x10 0x10 */
    volatile uint32_t RXFFR;                          /*!< Offset 0x014 Регистр сброса FIFO буферов блока приема. W 0x0 0x14 */
    volatile uint32_t TXFFR;                          /*!< Offset 0x018 Регистр сброса FIFO буферов блока передачи. W 0x0 0x18 */
             uint32_t reserved_0x01C;
    volatile uint32_t LRBR0_LTHR0;                    /*!< Offset 0x020 Регистр «левого» слова приемника. R 0x0 0x20, Регистр «левого» слова нулевого передатчика. W 0x0 0x20 */
    volatile uint32_t RRBR0;                          /*!< Offset 0x024 Регистр «правого» слова приемника. R 0x0 0x24, Регистр «правого» слова нулевого передатчика. W 0x0 0x24 */
    volatile uint32_t RER0;                           /*!< Offset 0x028 Регистр включения приемника. W/R 0x1 0x28 */
    volatile uint32_t TER0;                           /*!< Offset 0x02C Регистр включения нулевого передатчика. W/R 0x1 0x2C */
    volatile uint32_t RCR0;                           /*!< Offset 0x030 Регистр конфигурации приемника. W/R 0x5 0x30 */
    volatile uint32_t TCR0;                           /*!< Offset 0x034 Регистр конфигурации нулевого передатчика. W/R 0x5 0x34 */
    volatile uint32_t ISR0;                           /*!< Offset 0x038 Регистр статуса прерывания нулевого канала. R 0x10 0x38 */
    volatile uint32_t IMR0;                           /*!< Offset 0x03C Регистр маски прерывания нулевого канала. W/R 0x33 0x3C */
    volatile uint32_t ROR0;                           /*!< Offset 0x040 Регистр переполнения FIFO приемника. R 0x0 0x40 */
    volatile uint32_t TOR0;                           /*!< Offset 0x044 Регистр переполнения FIFO нулевого передатчика. R 0x0 0x44 */
    volatile uint32_t RFCR0;                          /*!< Offset 0x048 Регистр конфигурации FIFO приемника. W/R 0x3 0x48 */
    volatile uint32_t TFCR0;                          /*!< Offset 0x04C Регистр конфигурации FIFO нулевого передатчика. W/R 0x3 0x4C */
    volatile uint32_t RFF0;                           /*!< Offset 0x050 Регистр сброса FIFO приемника. W 0x0 0x50 */
    volatile uint32_t TFF0;                           /*!< Offset 0x054 Регистр сброса FIFO нулевого передатчика. W 0x0 0x54 */
             uint32_t reserved_0x058 [0x0002];
    volatile uint32_t LTHR1;                          /*!< Offset 0x060 Регистр «левого» слова первого передатчика. W 0x0 0x60 */
    volatile uint32_t RTHR1;                          /*!< Offset 0x064 Регистр «правого» слова первого передатчика. W 0x0 0x64 */
             uint32_t reserved_0x068;
    volatile uint32_t TER1;                           /*!< Offset 0x06C Регистр включения первого передатчика W/R 0x1 0x6C */
             uint32_t reserved_0x070;
    volatile uint32_t TCR1;                           /*!< Offset 0x074 Регистр конфигурации первого передатчика. W/R 0x5 0x74 */
    volatile uint32_t ISR1;                           /*!< Offset 0x078 Регистр статуса прерывания первого канала. R 0x10 0x78 */
    volatile uint32_t IMR1;                           /*!< Offset 0x07C Регистр маски прерывания первого канала. W/R 0x33 0x7C */
             uint32_t reserved_0x080;
    volatile uint32_t TOR1;                           /*!< Offset 0x084 Регистр переполнения первого передатчика. R 0x0 0x84 */
             uint32_t reserved_0x088;
    volatile uint32_t TFCR1;                          /*!< Offset 0x08C Регистр конфигурации FIFO первого передатчика. W/R 0x3 0x8C */
             uint32_t reserved_0x090;
    volatile uint32_t TFF1;                           /*!< Offset 0x094 Регистр сброса FIFO первого передатчика. W 0x0 0x94 */
             uint32_t reserved_0x098 [0x0002];
    volatile uint32_t LTHR2;                          /*!< Offset 0x0A0 Регистр «левого» слова второго передатчика. W 0x0 0xA0 */
    volatile uint32_t RTHR2;                          /*!< Offset 0x0A4 Регистр «правого» слова второго передатчика. W 0x0 0xA4 */
             uint32_t reserved_0x0A8;
    volatile uint32_t TER2;                           /*!< Offset 0x0AC Регистр включения второго передатчика W/R 0x1 0xAC */
             uint32_t reserved_0x0B0;
    volatile uint32_t TCR2;                           /*!< Offset 0x0B4 Регистр конфигурации второго передатчика. W/R 0x5 0xB4 */
    volatile uint32_t ISR2;                           /*!< Offset 0x0B8 Регистр статуса прерывания второго канала. R 0x10 0xB8 */
    volatile uint32_t IMR2;                           /*!< Offset 0x0BC Регистр маски прерывания второго канала. W/R 0x33 0xBC */
             uint32_t reserved_0x0C0;
    volatile uint32_t TOR2;                           /*!< Offset 0x0C4 Регистр переполнения второго передатчика. R 0x0 0xC4 */
             uint32_t reserved_0x0C8;
    volatile uint32_t TFCR2;                          /*!< Offset 0x0CC Регистр конфигурации FIFO второго передатчика. W/R 0x3 0xCC */
             uint32_t reserved_0x0D0;
    volatile uint32_t TFF2;                           /*!< Offset 0x0D4 Регистр сброса FIFO второго передатчика. W 0x0 0xD4 */
             uint32_t reserved_0x0D8 [0x0002];
    volatile uint32_t LTHR3;                          /*!< Offset 0x0E0 Регистр «левого» слова третьего передатчика. W 0x0 0xE0 */
    volatile uint32_t RTHR3;                          /*!< Offset 0x0E4 Регистр «правого» слова третьего передатчика. W 0x0 0xE4 */
             uint32_t reserved_0x0E8;
    volatile uint32_t TER3;                           /*!< Offset 0x0EC Регистр включения третьего передатчика W/R 0x1 0xEC */
             uint32_t reserved_0x0F0;
    volatile uint32_t TCR3;                           /*!< Offset 0x0F4 Регистр конфигурации третьего передатчика. W/R 0x5 0xF4 */
    volatile uint32_t ISR3;                           /*!< Offset 0x0F8 Регистр статуса прерывания третьего канала. R 0x10 0xF8 */
    volatile uint32_t IMR3;                           /*!< Offset 0x0FC Регистр маски прерывания третьего канала. W/R 0x33 0xFC */
             uint32_t reserved_0x100;
    volatile uint32_t TOR3;                           /*!< Offset 0x104 Регистр переполнения третьего передатчика. R 0x0 0x104 */
             uint32_t reserved_0x108;
    volatile uint32_t TFCR3;                          /*!< Offset 0x10C Регистр конфигурации FIFO третьего передатчика. W/R 0x3 0x10C */
             uint32_t reserved_0x110;
    volatile uint32_t TFF3;                           /*!< Offset 0x114 Регистр сброса FIFO третьего передатчика. W 0x0 0x114 */
             uint32_t reserved_0x118 [0x002A];
    volatile uint32_t RXDMA;                          /*!< Offset 0x1C0 Регистр DMA приема. R 0x0 0x1C0 */
             uint32_t reserved_0x1C4;
    volatile uint32_t TXDMA;                          /*!< Offset 0x1C8 Регистр DMA передачи. W 0x0 0x1C8 */
    volatile uint32_t RTXDMA;                         /*!< Offset 0x1CC Регистр сброса DMA передачи. W 0x0 0x1CC */
             uint32_t reserved_0x1D0 [0x0008];
    volatile uint32_t I2S_COMP_PARAM_2;               /*!< Offset 0x1F0 Второй регистр параметров компонента. R 0x48c 0x1F0 */
    volatile uint32_t I2S_COMP_PARAM_1;               /*!< Offset 0x1F4 Первый регистр параметров компонента. R 0x24c067e 0x1F4 */
    volatile uint32_t I2S_COMP_VERSION;               /*!< Offset 0x1F8 Регистр ревизии компонента. R 0x3130362a 0x1F8 */
    volatile uint32_t I2S_COMP_TYPE;                  /*!< Offset 0x1FC Регистр типа компонента. R 0x445701a0 0x1FC */
} I2S_TypeDef; /* size of structure = 0x200 */
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
    volatile uint32_t PACKET;                         /*!< Offset 0x000 0x0 RW Регистр настройки пакетов */
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
    volatile uint32_t SYS_PWR_UP;                     /*!< Offset 0x000 undocumented */
    volatile uint32_t SYS_PWR_DOWN;                   /*!< Offset 0x004 undocumented */
             uint32_t reserved_0x008;
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
    volatile uint32_t FTR [0x008];                    /*!< Offset 0x040 RO Тип ошибки канала 0..7 */
             uint32_t reserved_0x060 [0x0028];
    struct
    {
        volatile uint32_t CSR;                        /*!< Offset 0x100 RO Статус канала 0..7 */
        volatile uint32_t CPC;                        /*!< Offset 0x104 RO PC канала 0..7 */
    } CSR_CPC [0x008];                                /*!< Offset 0x100 Статус и PC канала 0..7 */
             uint32_t reserved_0x140 [0x00B0];
    struct
    {
        volatile uint32_t SAR;                        /*!< Offset 0x400 RO Адрес Источника для канала 0..7 */
        volatile uint32_t DAR;                        /*!< Offset 0x404 RO Адрес Приёмника для канала 0..7 */
        volatile uint32_t CCR;                        /*!< Offset 0x408 RO Регистр управления для канала 0..7 */
        volatile uint32_t LC0;                        /*!< Offset 0x40C RO Счётчик циклов 0 для канала 0..7 */
        volatile uint32_t LC1;                        /*!< Offset 0x410 RO Счётчик циклов 1 для канала 0..7 */
                 uint32_t reserved_0x014 [0x0003];
    } CH [0x008];                                     /*!< Offset 0x400 Состояния каналов 0..7 */
             uint32_t reserved_0x500 [0x0200];
    volatile uint32_t DBGSTATUS;                      /*!< Offset 0xD00 RO Debug Status Register */
    volatile uint32_t DBGCMD;                         /*!< Offset 0xD04 WO Debug Command Register */
    volatile uint32_t DBGINST0;                       /*!< Offset 0xD08 WO 0-ой регистр отладки */
    volatile uint32_t DBGINST1;                       /*!< Offset 0xD0C WO 1-ый регистр отладки */
             uint32_t reserved_0xD10 [0x003C];
    volatile uint32_t CR [0x005];                     /*!< Offset 0xE00 RO Регистр конфигурации 0..4 */
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
 * @brief SWIC
 */
/*!< SWIC КОНТРОЛЛЕР ИНТЕРФЕЙСА SPACEWIRE (SWIC) */
typedef struct SWIC_Type
{
    volatile uint32_t HW_VER;                         /*!< Offset 0x000 Номер версии контроллера RD 0x00 */
    volatile uint32_t STATUS;                         /*!< Offset 0x004 Регистр состояния WR/RD 0x04 */
    volatile uint32_t RX_CODE;                        /*!< Offset 0x008 Регистр управляющего символа, принятого из сети (маркера времени, кода распределенного прерывания, кода подтверждения распределенного прерывания или кода CC11) RD 0x08 */
    volatile uint32_t MODE_CR;                        /*!< Offset 0x00C Регистр режима работы WR 0x0С */
    volatile uint32_t TX_SPEED;                       /*!< Offset 0x010 Регистр коэффициента скорости передачи WR 0x10 */
    volatile uint32_t TX_CODE;                        /*!< Offset 0x014 Регистр управляющего символа (маркера времени, кода распределенного прерывания, кода подтверждения, кода CC11) для передачи в сеть WR 0x14 */
    volatile uint32_t RX_SPEED;                       /*!< Offset 0x018 Регистр скорости приема данных в канале SpaceWire RD 0x18 */
             uint32_t reserved_0x01C;
    volatile uint32_t CNT_RX_PACK;                    /*!< Offset 0x020 Регистр счетчика принятых пакетов ненулевой длины RD/WR 0x20 */
    volatile uint32_t ISR_L;                          /*!< Offset 0x024 Младшие разряды регистра ISR RD/WR 0x24 */
    volatile uint32_t ISR_H;                          /*!< Offset 0x028 Старшие разряды регистра ISR RD/WR 0x28 */
    volatile uint32_t TRUE_TIME;                      /*!< Offset 0x02C Регистр, содержащий значение последнего правильного маркера времени и последнего принятого из сети маркера времени RD 0x2C */
    volatile uint32_t TOUT_CODE;                      /*!< Offset 0x030 Регистр размера таймаутов RD/WR 0x30 */
    volatile uint32_t ISR_tout_L;                     /*!< Offset 0x034 Младшие разряды регистра флагов таймаутов ISR RD/WR 0x34 */
    volatile uint32_t ISR_tout_H;                     /*!< Offset 0x038 Старшие разряды регистра флагов таймаутов ISR RD/WR 0x38 */
    volatile uint32_t LOG_ADDR;                       /*!< Offset 0x03C Регистр логического адреса RD/WR 0x3С */
    volatile uint32_t ack_nonack_regime;              /*!< Offset 0x040 Регистр управления режимом распределенных прерываний (с подтверждениями или без подтверждений) RD/WR 0x40 */
    volatile uint32_t isr_touts2;                     /*!< Offset 0x044 Регистр таймаутов кодов распределенных прерываний 2 RD/WR 0x44 */
    volatile uint32_t ISR_handler_TERM_FUNCT;         /*!< Offset 0x048 Регистр флагов функций терминального узла –обработчика RD/WR 0x48 */
    volatile uint32_t ISR_SPEC;                       /*!< Offset 0x04C Регистр рассылки управляющих кодов в специальный набор портов RD/WR 0x4C */
    volatile uint32_t ISR_1101;                       /*!< Offset 0x050 Регистр флагов приема управляющих кодов, назначение которых не определено в текущей версии стандарта RD/WR 0x50 */
    volatile uint32_t ISR_MACK_1101;                  /*!< Offset 0x054 Регистр маски портов, из которых не должны приниматься управляющие коды, назначение которых не определено в текущей версии стандарта RD/WR 0x54 */
    volatile uint32_t INT_RESET;                      /*!< Offset 0x058 Регистр параметров команды внешнего сброса WR/RD 0x58 */
    volatile uint32_t STATUS2;                        /*!< Offset 0x05C Регистр состояния 0x5C */
    volatile uint32_t MODE_CR2;                       /*!< Offset 0x060 Регистр режима работы 2 0x60 */
    volatile uint32_t Int_H_mack;                     /*!< Offset 0x064 Старшая половина регистра маски распределенных прерываний WR/RD 0x64 */
    volatile uint32_t Int_L_mack;                     /*!< Offset 0x068 Младшая половина регистра маски распределенных прерываний WR/RD 0x68 */
    volatile uint32_t Ack_H_mack;                     /*!< Offset 0x06C Старшая половина регистра маски кодов подтверждения WR/RD 0x6C */
    volatile uint32_t Ackl_L_mack;                    /*!< Offset 0x070 Младшая половина регистра маски кодов подтверждения WR/RD 0x70 */
    volatile uint32_t AUTO_SPEED_MANAGE;              /*!< Offset 0x074 Регистр параметров автоматической установки скорости передачи WR/RD 0x74 */
    volatile uint32_t ISR_source_TERM_FUNCT;          /*!< Offset 0x078 Регистр флагов функций терминального узла –источника 0x78 */
    volatile uint32_t ISR_spec_term_funct;            /*!< Offset 0x07C Регистр признака специальной функции для терминального узла обработчика 0x7C */
    volatile uint32_t ISR_L_reset;                    /*!< Offset 0x080 Младшая половина регистра глобального сброса ISR 0x80 */
    volatile uint32_t ISR_H_reset;                    /*!< Offset 0x084 Старшая половина регистра глобального сброса ISR 0x84 */
} SWIC_TypeDef; /* size of structure = 0x088 */
/*
 * @brief SWIC_MFBSP_DMA
 */
/*!< SWIC_MFBSP_DMA Регистры встроенных DMA портов SWIC и MFBSP */
typedef struct SWIC_MFBSP_DMA_Type
{
    volatile uint32_t CSR;                            /*!< Offset 0x000 Регистр управления и состояния (по чтению сброс битов «END» и «DONE») */
    volatile uint32_t CP;                             /*!< Offset 0x004 Регистр указателя цепочки  */
    volatile uint32_t IR;                             /*!< Offset 0x008 Регистр индекса */
    volatile uint32_t RUN;                            /*!< Offset 0x00C На запись: Псевдорегистр управления состоянием бита RUN регистра CSR На чтение: Регистр управления и состояния без сброса битов «END» и «DONE» */
} SWIC_MFBSP_DMA_TypeDef; /* size of structure = 0x010 */
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


/* Defines */



/* Access pointers */

#define SDMA ((SDMA_TypeDef *) SDMA_BASE)             /*!< SDMA Регистры блока управления SDMA register set access pointer */
#define NANDMPORT ((NANDMPORT_TypeDef *) NANDMPORT_BASE)/*!< NANDMPORT КОНТРОЛЛЕР ПАМЯТИ ТИПА NAND FLASH (NANDMPORT) register set access pointer */
#define NFCM ((NANDMPORT_TypeDef *) NFCM_BASE)        /*!< NFCM КОНТРОЛЛЕР ПАМЯТИ ТИПА NAND FLASH (NANDMPORT) register set access pointer */
#define NORMPORT ((NORMPORT_TypeDef *) NORMPORT_BASE) /*!< NORMPORT контроллер внешней SRAM/NOR Flash памяти. register set access pointer */
#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0 Universal Asynchronous Receiver-Transmitter register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1 Universal Asynchronous Receiver-Transmitter register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2 Universal Asynchronous Receiver-Transmitter register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3 Universal Asynchronous Receiver-Transmitter register set access pointer */
#define I2C0 ((I2C_TypeDef *) I2C0_BASE)              /*!< I2C0  register set access pointer */
#define I2C1 ((I2C_TypeDef *) I2C1_BASE)              /*!< I2C1  register set access pointer */
#define I2C2 ((I2C_TypeDef *) I2C2_BASE)              /*!< I2C2  register set access pointer */
#define I2S0 ((I2S_TypeDef *) I2S0_BASE)              /*!< I2S0 Контроллер I2S предназначен для обмена аудио-данными в стерео-формате по последовательной шине I2S. register set access pointer */
#define GPIO0 ((GPIOBLOCK_TypeDef *) GPIO0_BASE)      /*!< GPIO0 Регистры блока управления GPIO register set access pointer */
#define GPIOA ((GPIO_TypeDef *) GPIOA_BASE)           /*!< GPIOA Регистры блока управления GPIO register set access pointer */
#define GPIOB ((GPIO_TypeDef *) GPIOB_BASE)           /*!< GPIOB Регистры блока управления GPIO register set access pointer */
#define GPIOC ((GPIO_TypeDef *) GPIOC_BASE)           /*!< GPIOC Регистры блока управления GPIO register set access pointer */
#define GPIOD ((GPIO_TypeDef *) GPIOD_BASE)           /*!< GPIOD Регистры блока управления GPIO register set access pointer */
#define SWIC0 ((SWIC_TypeDef *) SWIC0_BASE)           /*!< SWIC0 КОНТРОЛЛЕР ИНТЕРФЕЙСА SPACEWIRE (SWIC) register set access pointer */
#define DMA_SWIC0 ((SWIC_MFBSP_DMA_TypeDef *) DMA_SWIC0_BASE)/*!< DMA_SWIC0 Регистры встроенных DMA портов SWIC и MFBSP register set access pointer */
#define SWIC1 ((SWIC_TypeDef *) SWIC1_BASE)           /*!< SWIC1 КОНТРОЛЛЕР ИНТЕРФЕЙСА SPACEWIRE (SWIC) register set access pointer */
#define DMA_SWIC1 ((SWIC_MFBSP_DMA_TypeDef *) DMA_SWIC1_BASE)/*!< DMA_SWIC1 Регистры встроенных DMA портов SWIC и MFBSP register set access pointer */
#define MFBSP0 ((MFBSP_TypeDef *) MFBSP0_BASE)        /*!< MFBSP0 МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  register set access pointer */
#define DMA_MFBSP0 ((SWIC_MFBSP_DMA_TypeDef *) DMA_MFBSP0_BASE)/*!< DMA_MFBSP0 Регистры встроенных DMA портов SWIC и MFBSP register set access pointer */
#define MFBSP1 ((MFBSP_TypeDef *) MFBSP1_BASE)        /*!< MFBSP1 МНОГОФУНКЦИОНАЛЬНЫЙ БУФЕРИЗИРОВАННЫЙ ПОСЛЕДОВАТЕЛЬНЫЙ ПОРТ (MFBSP)  register set access pointer */
#define DMA_MFBSP1 ((SWIC_MFBSP_DMA_TypeDef *) DMA_MFBSP1_BASE)/*!< DMA_MFBSP1 Регистры встроенных DMA портов SWIC и MFBSP register set access pointer */
#define CMCTR ((CMCTR_TypeDef *) CMCTR_BASE)          /*!< CMCTR Контроллер управления синхронизацией register set access pointer */
#define PMCTR ((PMCTR_TypeDef *) PMCTR_BASE)          /*!< PMCTR Контроллер управления энергопотреблением register set access pointer */
#define SMCTR ((SMCTR_TypeDef *) SMCTR_BASE)          /*!< SMCTR общиме системные настройки микросхемы register set access pointer */


#endif /* HEADER_00003039_INCLUDED */
