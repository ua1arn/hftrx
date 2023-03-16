#pragma once
#ifndef HEADER_00003039_INCLUDED
#define HEADER_00003039_INCLUDED
#include <stdint.h>


/* IRQs */

typedef enum IRQn
{
    UART0_IRQn = 32,                                  /*!< UART  Interrupt */
    UART1_IRQn = 33,                                  /*!< UART  Interrupt */
    UART2_IRQn = 34,                                  /*!< UART  Interrupt */
    UART3_IRQn = 35,                                  /*!< UART  Interrupt */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define UART0_BASE ((uintptr_t) 0x38028000)           /*!< UART Base */
#define UART1_BASE ((uintptr_t) 0x38029000)           /*!< UART Base */
#define UART2_BASE ((uintptr_t) 0x3802A000)           /*!< UART Base */
#define UART3_BASE ((uintptr_t) 0x3802B000)           /*!< UART Base */

/*
 * @brief UART
 */
/*!< UART  */
typedef struct UART_Type
{
    volatile uint32_t DATA;                           /*!< Offset 0x000 UART Receive Buffer Register/Transmit Holding Register */
    volatile uint32_t UART_DLH;                       /*!< Offset 0x004  */
    volatile uint32_t UART_IIR;                       /*!< Offset 0x008 UART Interrupt Identity Register/UART FIFO Control Register */
    volatile uint32_t UART_LCR;                       /*!< Offset 0x00C UART Line Control Register */
    volatile uint32_t UART_MCR;                       /*!< Offset 0x010 UART Modem Control Register */
    volatile uint32_t UART_LSR;                       /*!< Offset 0x014 UART Line Status Register */
    volatile uint32_t UART_MSR;                       /*!< Offset 0x018 UART Modem Status Register */
    volatile uint32_t UART_SCR;                       /*!< Offset 0x01C UART Scratch Register */
             uint32_t reserved_0x020 [0x0004];
    volatile uint32_t UART_SRBR_STHR;                 /*!< Offset 0x030  */
             uint32_t reserved_0x034 [0x0012];
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

#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0  register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1  register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2  register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3  register set access pointer */


#endif /* HEADER_00003039_INCLUDED */
