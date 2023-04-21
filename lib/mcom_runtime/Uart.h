#ifndef ___UART_H___
#define ___UART_H___

//---------------------------------

#define UART0_BASE                        ( 0x38028000UL )                        /* MCom02 - UART 0 */
#define UART1_BASE                        ( 0x38029000UL )                        /* MCom02 - UART 1 */
#define UART2_BASE                        ( 0x3802a000UL )                        /* MCom02 - UART 2 */
#define UART3_BASE                        ( 0x3802b000UL )                        /* MCom02 - UART 3 */
/*
#define UART_RBR(x)        ( (unsigned long *)        ( (x) + 0x0000UL ) )
#define UART_THR(x)        ( (unsigned long *)        ( (x) + 0x0000UL ) )
#define UART_DLL(x)        ( (unsigned long *)        ( (x) + 0x0000UL ) )
#define UART_DLH(x)        ( (unsigned long *)        ( (x) + 0x0004UL ) )
#define UART_IER(x)        ( (unsigned long *)        ( (x) + 0x0004UL ) )
#define UART_IIR(x)        ( (unsigned long *)        ( (x) + 0x0008UL ) )
#define UART_FCR(x)        ( (unsigned long *)        ( (x) + 0x0008UL ) )
#define UART_LCR(x)        ( (unsigned long *)        ( (x) + 0x000CUL ) )
#define UART_MCR(x)        ( (unsigned long *)        ( (x) + 0x0010UL ) )
#define UART_LSR(x)        ( (unsigned long *)        ( (x) + 0x0014UL ) )
#define UART_MSR(x)        ( (unsigned long *)        ( (x) + 0x0018UL ) )
#define UART_SCR(x)        ( (unsigned long *)        ( (x) + 0x001CUL ) )
#define UART_SRBR(x)       ( (unsigned long *)        ( (x) + 0x0030UL ) )
#define UART_STHR(x)       ( (unsigned long *)        ( (x) + 0x0030UL ) )
#define UART_USR(x)        ( (unsigned long *)        ( (x) + 0x007CUL ) )
#define UART_TFL(x)        ( (unsigned long *)        ( (x) + 0x0080UL ) )
#define UART_RFL(x)        ( (unsigned long *)        ( (x) + 0x0084UL ) )
#define UART_SRR(x)        ( (unsigned long *)        ( (x) + 0x0088UL ) )
#define UART_SRTS(x)       ( (unsigned long *)        ( (x) + 0x008CUL ) )
#define UART_SBCR(x)       ( (unsigned long *)        ( (x) + 0x0080UL ) )
#define UART_SFE(x)        ( (unsigned long *)        ( (x) + 0x0098UL ) )
#define UART_SRT(x)        ( (unsigned long *)        ( (x) + 0x009CUL ) )
#define UART_STET(x)       ( (unsigned long *)        ( (x) + 0x00A0UL ) )
#define UART_HTX(x)        ( (unsigned long *)        ( (x) + 0x00A4UL ) )
*/
        /* IER register fields */
#define UART_IER_ERBFI_ENABLE       					( 1 << 0 )
#define UART_IER_ETBEI_ENABLE       					( 1 << 1 )
#define UART_IER_ELSI_ENABLE        					( 1 << 2 )
#define UART_IER_EDSSI_ENABLE       					( 1 << 3 )
#define UART_IER_PITME_ENABLE       					( 1 << 7 )

        /* IIR register fields */
#define UART_IIR_INTERRUPT_STATUS_MODEM                	(  0 << 0 )
#define UART_IIR_INTERRUPT_STATUS_EMPTYQ        		(  1 << 0 )
#define UART_IIR_INTERRUPT_STATUS_THR                	(  2 << 0 )
#define UART_IIR_INTERRUPT_STATUS_TX                	(  8 << 0 )
#define UART_IIR_TIMEOUT_STATUS_MASK                	( 12 << 0 )
#define UART_IIR_FIFO_ENABLE_STATUS_MASK        		(  3 << 6 )

        /* IIR register fields */
#define UART_IIR_INTERRUPT_STATUS_MODEM   (  0 << 0 )
#define UART_IIR_INTERRUPT_STATUS_EMPTYQ  (  1 << 0 )
#define UART_IIR_INTERRUPT_STATUS_THR     (  2 << 0 )
#define UART_IIR_INTERRUPT_STATUS_TX      (  8 << 0 )
#define UART_IIR_TIMEOUT_STATUS_MASK      ( 12 << 0 )
#define UART_IIR_FIFO_ENABLE_STATUS_MASK  (  3 << 6 )
//Лучше так:
//#define UART_IIR_FIFO_ENABLE_STATUS_MASK	( 3 << 6 )
//#define UART_IIR_TIMEOUT_STATUS_MASK		( 1 << 3 )
#define UART_IIR_INTERRUPT_STATUS_MASK		( 3 << 1 )
#define UART_IIR_INTERRUPT_STATUS_RX_ERROR	( 3 << 1 )
#define UART_IIR_INTERRUPT_STATUS_RX_DR		( 1 << 2 )
//#define UART_IIR_INTERRUPT_STATUS_TX		( 1 << 1 )
#define UART_IIR_INTERRUPT_PENDING_MASK		( 1 << 0 )

        /* FCR register fields */
#define UART_FCR_RFTL_STATE_IDX                			( 6 )
#define UART_FCR_RFTL_STATE                				( 3 << 6 )
#define UART_FCR_TET_STATE_IDX                			( 4 )
#define UART_FCR_TET_STATE                				( 3 << 4 )
#define UART_FCR_RESET_TX_FLAG                			( 1 << 2 )
#define UART_FCR_RESET_RX_FLAG                			( 1 << 1 )
#define UART_FCR_FEWO                        			( 1 << 0 )

        /* LCR register fields */
#define UART_LCR_DLAB                   				( 1 << 7 )
#define UART_LCR_BC                     				( 1 << 6 )
#define UART_LCR_STP                    				( 1 << 5 )
#define UART_LCR_EPS                    				( 1 << 4 )
#define UART_LCR_PEN                    				( 1 << 3 )
#define UART_LCR_STOP                   				( 1 << 2 )
#define UART_LCR_DLS                    				( 3 << 0 )

        /* MCR register fields */
#define UART_MCR_AFCE                        			( 1 << 5 )
#define UART_MCR_RTS                      				( 1 << 1 )

        /* LSR register fields */
#define UART_LSR_EIRF                   				( 1 << 7 )
#define UART_LSR_TEMT                   				( 1 << 6 )
#define UART_LSR_THRE                   				( 1 << 5 )
#define UART_LSR_BI                         			( 1 << 4 )
#define UART_LSR_FE                         			( 1 << 3 )
#define UART_LSR_PE                          			( 1 << 2 )
#define UART_LSR_OE                        				( 1 << 1 )
#define UART_LSR_RDR                       				( 1 << 0 )

        /* MSR register fields */
#define UART_MSR_CTS                    				( 1 << 4 )
#define UART_MSR_DCTS                   				( 1 << 0 )

        /* USR register fields */
#define UART_LSR_RFF                        			( 1 << 4 )
#define UART_LSR_RFNE                        			( 1 << 3 )
#define UART_LSR_TFE                         			( 1 << 2 )
#define UART_LSR_TFNF                   				( 1 << 1 )
#define UART_LSR_BUSY                   				( 1 << 0 )

        /* SRR register fields */
#define UART_SRR_XFR                         			( 1 << 2 )
#define UART_SRR_RFR                    				( 1 << 1 )
#define UART_SRR_UR                     				( 1 << 0 )

        /* SRTS register fields */
#define UART_SRTS_SRTS                  				( 1 << 0 )

        /* SBCR register fields */
#define UART_SBCR_SBCR                  				( 1 << 0 )

        /* SFE register fields */
#define UART_SFE_SBCR                   				( 1 << 0 )

         /* SRT register fields */
#define UART_SRT_SBCR                   				( 1 << 0 )

         /* STET register fields */
#define UART_STET_SBCR                  				( 1 << 0 )

         /* HTX register fields */
#define UART_HTX_HTX                    				( 1 << 0 )

#define UART_CLK_HZ                        				( 216000000UL )
#define UART0_VECTOR_ID                        			( 32 )

#define UART_FIFO_SIZE_BYTES        					( 32UL )
#define UART1_VECTOR_ID                        			( 33 )
#define UART2_VECTOR_ID                        			( 34 )
#define UART3_VECTOR_ID                        			( 35 )

#define TX_QUEUE                                		( 0 )
#define RX_QUEUE                                		( 1 )

#define min(a,b) ((a)<(b))?(a):(b)

//---------------------------------

#define TRANSMIT_LENGTH       20

#define UART_FIFO_LENGTH                128
    //Interrupts IDs
#define UART_INT_MODEM_STATUS                                   ((int)0)
#define UART_INT_NO_INTERRUPT                                   ((int)1)
#define UART_INT_THR_EMPTY                                      ((int)2)
#define UART_INT_DATA_RECIEVE                                   ((int)4)
#define UART_INT_RECEIVER_LINE_STATUS                           ((int)6)
#define UART_INT_BUSY_DETECT                                    ((int)7)
#define UART_INT_CHAR_TIMEOUT                                   ((int)12)
//??????????????
//#define UART_FLAG_TXFE		( 1 << 7 )
//#define UART_FLAG_TXFF		( 1 << 5 )
//#define UART_FLAG_RXFE		( 1 << 4 )

//---------------------------------
/*
#define UART_1_RX_PARAMSET1   22
#define UART_1_RX_PARAMSET2   65
#define UART_1_TX_PARAMSET1   23
#define UART_1_TX_PARAMSET2   66 //не используется
#define UART_2_RX_PARAMSET1   24
#define UART_2_RX_PARAMSET2   67
#define UART_2_TX_PARAMSET1   25
#define UART_2_TX_PARAMSET2   68 //не используется
*/

typedef enum
{
    BAUD9600	=	156,
    BAUD14400	=	104,
    BAUD19200	=	78,
    BAUD28800	=	52,
    BAUD38400	=	39,
    BAUD57600	=	26,
    BAUD115200	=	13,
    BAUD128000	=	12
}uart_baudrate_t;



#include "pdma.h"

struct UART_Job_struct;

typedef struct UART_Job_struct
{
      short                 sCurrentTaskState;     // 0 - Free
      short                 bMode;
      unsigned char        *ucBlockDataPtr;       // Указатель на выделенную область данных
      int                   uiSize;               // Размер выделенной области даанных
      int					TxSize;               //Уже передано данных
      void			       *UART_Job_next;        // Указатель на следующую структуру.
}_UART_Job;

typedef struct{
         unsigned char * pRXBuffPtr;                      //Указатель на RX буффер (циклический)
         unsigned int    RxBuffSize;                      // и его размер
         unsigned char * RxPrt;                           // для режима прерываний - указатель на последний принятый символ
         unsigned char * pTXTBuffPtr;                      //Указатель на  буффер
         unsigned int    localRP;                         //Указатель чтения из буфера обработки принятых данных по УАРТ 
         unsigned int    MaxAddress;                      //Чтоб не расчитывать каждый каз - сохранённое значение
         //unsigned char   rx_ps1;                          //Номера парамсетов для EDMA RX
         //unsigned char   rx_ps2;                          //RX-link
         //unsigned char   tx_ps1;                          //TX
         //unsigned char   tx_ps2;                          //TX-link
         pdma_mem_chain  dma_chain[4];
         unsigned int    UartReadIdx;                     //Указатель чтения из буфера приема по УАРТ
         unsigned int    localReadPtr;                    //Указатель чтения из буфера обработки принятых данных по УАРТ 
         unsigned int    uiUart_Mode_dma;
         unsigned int    uiDMA_id_tx;
         unsigned int    uiDMA_id_rx;
volatile unsigned int    usTotalReservd;                   //Очередь на передачу
volatile unsigned int    bInit;                   //Очередь на передачу
_UART_Job  *ujob;
}_UART_Nfo;

extern _UART_Nfo   unfo[4];


/*

Возвращяет 
uMode бит 0
   == 0 законченную строку, NULL иначе, - длинна в uReaded, pUserBufferPtr может передаваться NULL
      Пр: ReadUart(1, (unsigned char * )0, 0, tst, 0);
   == 1 pUserBufferPtr если данные приняты, NULL иначе, бинарные данные копируются в pUserBufferPtr - размер в uReaded
      Пр:ReadUart(0, tst_buff, 32, tst, 1);

uMode бит 1
   == 0 вызывается из основной программы
   == 1 вызывается из прерывания

*/
unsigned char * ReadUart(unsigned int uUartNum, unsigned char * pUserBufferPtr, unsigned int uUserBufferSize, unsigned int *uReaded, unsigned int uMode);
/*

uMode бит 0
   Не важен.

uMode бит 1
   == 0 вызывается из основной программы
   == 1 вызывается из прерывания

*/
//Ставить в очередь в уарт
void WriteUart(unsigned int uUartNum, unsigned char * dPtr, unsigned int uSize, unsigned int uMode);
//Инициализация UART
//bool Uart_Init(unsigned int uUartNum, unsigned int baudrate);
int Uart_Init(unsigned int uUartNum, unsigned int baudrate, unsigned int even, unsigned int mode_dma);
//
//   Внутренние функции:
//
//Инициирование передачи EDMA3
//void Edma3TXInit(unsigned int uUartNum, unsigned char * dPtr, unsigned int uSize);
//Вызывается из прерывания ДМА для запроса следующих данных для передачи
unsigned int GetNextQueueBlock(unsigned int uUartNum);
//
//	Примитивные функции
//
int uart_puts(unsigned int uart_id, const char* str);
unsigned short uart_getchar (unsigned int uUartNum);
void uart_putchar (unsigned int uUartNum, short c);
//
//
#endif // ___UART_H___
