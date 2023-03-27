#pragma once
#ifndef HEADER_00003039_INCLUDED
#define HEADER_00003039_INCLUDED
#include <stdint.h>


/* IRQs */

typedef enum IRQn
{
    SGI0_IRQn = 0,                                    /*!<   Interrupt */
    SGI1_IRQn = 1,                                    /*!<   Interrupt */
    SGI2_IRQn = 2,                                    /*!<   Interrupt */
    SGI3_IRQn = 3,                                    /*!<   Interrupt */
    SGI4_IRQn = 4,                                    /*!<   Interrupt */
    SGI5_IRQn = 5,                                    /*!<   Interrupt */
    SGI6_IRQn = 6,                                    /*!<   Interrupt */
    SGI7_IRQn = 7,                                    /*!<   Interrupt */
    SGI8_IRQn = 8,                                    /*!<   Interrupt */
    SGI9_IRQn = 9,                                    /*!<   Interrupt */
    SGI10_IRQn = 10,                                  /*!<   Interrupt */
    SGI11_IRQn = 11,                                  /*!<   Interrupt */
    SGI12_IRQn = 12,                                  /*!<   Interrupt */
    SGI13_IRQn = 13,                                  /*!<   Interrupt */
    SGI14_IRQn = 14,                                  /*!<   Interrupt */
    SGI15_IRQn = 15,                                  /*!<   Interrupt */
    VirtualMaintenanceInterrupt_IRQn = 25,            /*!<   Interrupt */
    HypervisorTimer_IRQn = 26,                        /*!<   Interrupt */
    VirtualTimer_IRQn = 27,                           /*!<   Interrupt */
    Legacy_nFIQ_IRQn = 28,                            /*!<   Interrupt */
    SecurePhysicalTimer_IRQn = 29,                    /*!<   Interrupt */
    NonSecurePhysicalTimer_IRQn = 30,                 /*!<   Interrupt */
    Legacy_nIRQ_IRQn = 31,                            /*!<   Interrupt */
    UART0_IRQn = 96,                                  /*!< UART Universal Asynchronous Receiver-Transmitter Interrupt */
    UART1_IRQn = 97,                                  /*!< UART Universal Asynchronous Receiver-Transmitter Interrupt */
    UART2_IRQn = 98,                                  /*!< UART Universal Asynchronous Receiver-Transmitter Interrupt */
    UART3_IRQn = 99,                                  /*!< UART Universal Asynchronous Receiver-Transmitter Interrupt */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define UART0_BASE ((uintptr_t) 0x38028000)           /*!< UART Base */
#define UART1_BASE ((uintptr_t) 0x38029000)           /*!< UART Base */
#define UART2_BASE ((uintptr_t) 0x3802A000)           /*!< UART Base */
#define UART3_BASE ((uintptr_t) 0x3802B000)           /*!< UART Base */
#define PMCTR_BASE ((uintptr_t) 0x38095000)           /*!< PMCTR Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0x39000100)   /*!<  Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0x39001000) /*!<  Base */

/*
 * @brief PMCTR
 */
/*!< PMCTR Контроллер управления питанием */
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

#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0 Universal Asynchronous Receiver-Transmitter register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1 Universal Asynchronous Receiver-Transmitter register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2 Universal Asynchronous Receiver-Transmitter register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3 Universal Asynchronous Receiver-Transmitter register set access pointer */
#define PMCTR ((PMCTR_TypeDef *) PMCTR_BASE)          /*!< PMCTR Контроллер управления питанием register set access pointer */
#define GIC_INTERFACE ((_TypeDef *) GIC_INTERFACE_BASE)/*!< GIC_INTERFACE  register set access pointer */
#define GIC_DISTRIBUTOR ((_TypeDef *) GIC_DISTRIBUTOR_BASE)/*!< GIC_DISTRIBUTOR  register set access pointer */


#endif /* HEADER_00003039_INCLUDED */
