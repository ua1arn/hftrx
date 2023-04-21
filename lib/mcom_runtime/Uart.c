#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "SWItypedef.h"
#include "uart.h"
#include "dma_alias.h"
#include "pdma.h"
#include "gic.h" 
#include "pll.h" 

//#pragma -Wincompatible-pointer-types
#define UARTS_NUM 			    4
/***************************System Registers*******************************/
#define CMCTR_BASE                                              0x38094000
#define DIV_SYS1_CTR            (*(volatile unsigned int*)(CMCTR_BASE + 0x040))
#define DIV_SYS2_CTR            (*(volatile unsigned int*)(CMCTR_BASE + 0x044))
#define GATE_SYS_CTR            (*(volatile unsigned int*)(CMCTR_BASE + 0x04c))
#define CLK_UART3_EN            (1 << 15)
#define CLK_UART2_EN            (1 << 14)
#define CLK_UART1_EN            (1 << 13)
#define CLK_UART0_EN            (1 << 12)
#define SEL_SPLL				(*(volatile unsigned int*)(CMCTR_BASE + 0x10c))

#define GPIO0_BASE              0x38034000
#define GPIO0(a)                (*(volatile unsigned int*)(GPIO0_BASE + (a)))
#define SWPORTA_DR						0x00
#define SWPORTA_DDR						0x04
#define SWPORTA_CTL						0x08
#define SWPORTB_DR						0x0c
#define SWPORTB_DDR						0x10
#define SWPORTB_CTL						0x14
#define SWPORTC_DR						0x18
#define SWPORTC_DDR						0x1c
#define SWPORTC_CTL						0x20
#define SWPORTD_DR						0x24
#define SWPORTD_DDR						0x28
#define SWPORTD_CTL						0x2c
#define GPIOA18_UART0_SIN                                       (1 << 18)
#define GPIOA19_UART0_SOUT                                      (1 << 19)
#define GPIOA20_UART0_CTSn                                      (1 << 20)
#define GPIOA21_UART0_RTSn                                      (1 << 21)
#define GPIOD0_UART1_SIN                                        (1 << 0)
#define GPIOD1_UART1_SOUT                                       (1 << 1)
#define GPIOD2_UART1_CTSn                                       (1 << 2)
#define GPIOD3_UART1_RTSn                                       (1 << 3)
#define GPIOD4_UART2_SIN                                        (1 << 4)
#define GPIOD5_UART2_SOUT                                       (1 << 5)
#define GPIOD6_UART3_SIN                                        (1 << 6)
#define GPIOD7_UART3_SOUT                                       (1 << 7)

#define GIC_UART0_INTR                                          (96)
#define GIC_UART1_INTR                                          (97)
#define GIC_UART2_INTR                                          (98)
#define GIC_UART3_INTR                                          (99)

#define FCR_FEWO                                                (1 << 0)
#define FCR_RCVR                                                (1 << 1)
#define FCR_XFIFOR                                              (1 << 2)

#define MCR_RTS                                                 (1 << 1)
#define MCR_AFCE                                                (1 << 5)

#define LSR_THRE                                                (1 << 5)
#define LSR_RDR                                                 (1 << 0)

#define IER_ERBFI                                               (1 << 0)
#define IER_ETBEI                                               (1 << 1)
#define IER_PTIME                                               (1 << 7)

#define UART_SET_TET(val,tet)                                    (((val) & ~(3U << 4)) | (tet << 4))

#define LINE_FEED                                               0xA
#define CARET_RETURN                                            0xD

const unsigned int UART_GPIO[4][5] __attribute__ ((aligned(8))) = {
    { SWPORTA_CTL, GPIOA18_UART0_SIN, GPIOA19_UART0_SOUT, GPIOA20_UART0_CTSn, GPIOA21_UART0_RTSn},
    { SWPORTD_CTL, GPIOD0_UART1_SIN, GPIOD1_UART1_SOUT, GPIOD2_UART1_CTSn, GPIOD3_UART1_RTSn},
    { SWPORTD_CTL, GPIOD4_UART2_SIN, GPIOD5_UART2_SOUT, 0, 0},
    { SWPORTD_CTL, GPIOD6_UART3_SIN, GPIOD7_UART3_SOUT, 0, 0},
};
typedef struct
{
    volatile unsigned int RBR_THR_DLL;
    volatile unsigned int DLH_IER;
    union
    {
        volatile unsigned int IIR;
        volatile unsigned int FCR;
    };
    union
    {
        volatile unsigned int value;
        struct
        {
            volatile unsigned int DLS : 2;
            volatile unsigned int STOP : 1;
            volatile unsigned int PEN : 1;
            volatile unsigned int PARITY : 1;
            volatile unsigned int STP : 1;
            volatile unsigned int BC : 1;
            volatile unsigned int DLAB : 1;
            volatile unsigned int reserve : 24;
        };
    }LCR;
    volatile unsigned int MCR;
    volatile unsigned int LSR;
    volatile unsigned int MSR;
    volatile unsigned int SCR;
    volatile unsigned int reserve0[4];
    volatile unsigned int SRBR_STHR[16];
    volatile unsigned int reserve1[3];
    volatile unsigned int USR;
    volatile unsigned int TFL;
    volatile unsigned int RFL;
    volatile unsigned int SRR;
    volatile unsigned int SRTS;
    volatile unsigned int SBCR;
    volatile unsigned int reserve2[1];
    volatile unsigned int SFE;
    volatile unsigned int SRT;
    volatile unsigned int STET;
    volatile unsigned int HTX;
}uart_reg_t;

//приёмные буфера
//unsigned int UART_RX_BUFF_SIZE[UARTS_NUM]={2048, 2048, 2048, 2048};
#define UART_1_RX_BUFF_SIZE 16
unsigned int UART_RX_BUFF_SIZE[UARTS_NUM]={UART_1_RX_BUFF_SIZE, UART_1_RX_BUFF_SIZE, UART_1_RX_BUFF_SIZE, UART_1_RX_BUFF_SIZE};
//#pragma DATA_ALIGN(128);
unsigned char  UartRxBuffer[UARTS_NUM][UART_1_RX_BUFF_SIZE] __attribute__((aligned(8)));
//структура с информацией
_UART_Nfo   unfo[UARTS_NUM];
pdma_mem_chain  UartRXmem_chain[UARTS_NUM][2] __attribute__((aligned(8)));

//Буфер для обработки принятых ТЕКСТОВЫХ данных по УАРТ
unsigned char UartRead_Buff[UARTS_NUM][TRANSMIT_LENGTH];

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

static void uart_dma_interrupt_handler(int id)
{
register int status, uart_num;
register int dma_id;
register int dma_id_mask = 1;
    register _UART_Nfo        *punfo, *punfo_tmp;
    pdma_mem_channel* dma_ch;
    //
        punfo     = &unfo[0];
        punfo_tmp = punfo; 
    //
	dma_irq irq;
	volatile unsigned long long *status_reg, *clear_reg;

	switch (id)
	{
	case PDMA_INT_FLAG0:
		irq = IntTfr;
		status_reg = &(pdma_interrupt_regs->StatusTfr);
		clear_reg  = &(pdma_interrupt_regs->ClearTfr);
		break;
	case PDMA_INT_FLAG1:
		irq = IntBlock;
		status_reg = &(pdma_interrupt_regs->StatusBlock);
		clear_reg  = &(pdma_interrupt_regs->ClearBlock);
		break;
	case PDMA_INT_FLAG3:
		irq = IntDstTran;
		status_reg = &(pdma_interrupt_regs->StatusDstTran);
		clear_reg  = &(pdma_interrupt_regs->ClearDstTran);
		break;
	case PDMA_INT_FLAG2:
		irq = IntSrcTran;
		status_reg = &(pdma_interrupt_regs->StatusSrcTran);
		clear_reg  = &(pdma_interrupt_regs->ClearSrcTran);
		break;
	case PDMA_INT_FLAG4:
		irq = IntError;
		status_reg = &(pdma_interrupt_regs->StatusErr);
		clear_reg  = &(pdma_interrupt_regs->ClearErr);
		break;
	default:
		break;
	}

    /*
    status = pdma_interrupt_regs->RawTfr;//StatusTfr;//RawTfr; //IntTfr
    //
    for(dma_id = 0; dma_id < 8 ; dma_id++)
    {
    	if( status & dma_id_mask )
    	{
    	    punfo = punfo_tmp; 
    		for(uart_num = 0; uart_num < 4; uart_num++)
    		{
    			if(punfo->bInit == 1)
    			{
    				if(punfo->uiDMA_id_tx == dma_id)
        			{
        				GetNextQueueBlock(uart_num);
        				pdma_interrupt_regs->ClearTfr = dma_id_mask;
        			}
        			if(punfo->uiDMA_id_rx == dma_id)
        			{
        				//GetNextQueueBlock(uart_num);
        				dma_ch = get_pdma_dev(dma_id);
        				//MaskBlock, MaskDstTran, MaskErr, MaskSrcTran, MaskTfr
        				pdma_interrupt_regs->ClearTfr = dma_id_mask;
        			}
    			}
    		punfo++;
    		}
    	}
    	dma_id_mask <<=1;
    }*/
    //
	for (int i = 0; i < 8; i++) 
	{
		if ((*status_reg >> i) & 0x1) 
		{
			*clear_reg = (1 << i);
			if(irq == IntTfr)
			{
				if(i & 0x1)
				{//RX
					int test;
					test = 1;
				}
				else
				{//TX
					GetNextQueueBlock(i>>1);
				}
			}
			//if (callback != 0) {callback(irq, i);}
		}
	}

    //pdma_interrupt_regs->ClearTfr = 1;
}

/*
//int i_dma_test = 0;

void pdma0_interrupt_handler(  int _value  )
{
	uart_dma_interrupt_handler(_value);
	///i_dma_test = pdma_interrupt_regs->RawTfr;;
}
void pdma1_interrupt_handler(  int _value  )
{
	uart_dma_interrupt_handler(_value);
//	i_dma_test = pdma_interrupt_regs->RawTfr;;
}
void pdma2_interrupt_handler(  int _value  )
{
	uart_dma_interrupt_handler(_value);
//	i_dma_test = pdma_interrupt_regs->RawTfr;;
}
void pdma3_interrupt_handler(  int _value  )
{
	uart_dma_interrupt_handler(_value);
//	i_dma_test = pdma_interrupt_regs->RawTfr;;
}
void pdma4_interrupt_handler(  int _value  )
{
	uart_dma_interrupt_handler(_value);
//	i_dma_test = pdma_interrupt_regs->RawTfr;;
}
void pdma5_interrupt_handler(  int _value  )
{
	uart_dma_interrupt_handler(_value);
//	i_dma_test = pdma_interrupt_regs->RawTfr;;
}
*/

//прототип uart_interrupt_handler
void uart_interrupt_handler( int uartNum );

void uart0_interrupt_handler( int _value )
{
	uart_interrupt_handler(0);
}
void uart1_interrupt_handler( int _value )
{
	uart_interrupt_handler(1);
}
void uart2_interrupt_handler( int _value )
{
	uart_interrupt_handler(2);
}
void uart3_interrupt_handler( int _value )
{
	uart_interrupt_handler(3);
}


void uart_data_read(unsigned int uartNum);
void uart_data_transfer(unsigned int uartNum);
//void pTXInit(unsigned int uUartNum, unsigned char * dPtr, unsigned int uSize);
//void dmaTXInit(unsigned int uUartNum, unsigned char * dPtr, unsigned int uSize);

uart_reg_t* getUartHandler(unsigned int uartNum)
{
    uartNum &= 0x3;
    return (uart_reg_t*)(UART0_BASE + 0x1000 * uartNum);
}

unsigned long uart_base(unsigned int idUart)
{
        unsigned long ulBase = 0;
    idUart &= 0x3;
	switch ( idUart )
	{
	case 0:
		ulBase = UART0_BASE;
		break;
	case 1:
		ulBase = UART1_BASE;
		break;
	case 2:
		ulBase = UART2_BASE;
		break;
	case 3:
		ulBase = UART3_BASE;
		break;
	}
   return  ulBase;
}

/*
 * uMode == 1 бинарный режим. Иначе текст - разбив на строки
 * uMode == 0 текст - разбив на строки
 * */

unsigned char * ReadUart(unsigned int uUartNum, unsigned char * pUserBufferPtr, unsigned int uUserBufferSize, unsigned int *uReaded, unsigned int uMode)
{
    register _UART_Nfo        *punfo;

        punfo     = &unfo[uUartNum];
		pdma_mem_channel * pdma;
    ////сохряняем указатель на позицию посл. записанного в массив символа
    unsigned int savedUartWP;// = (unsigned int)Edma3ccRegsOvly->PARAMSET[punfo->rx_ps1].DST - (unsigned int)punfo->pRXBuffPtr;
    unsigned char currentChar = 0;
    unsigned int size = 0;
    unsigned int address;
    unsigned int clearRemainder;
    unsigned int clearSize;

    if(punfo->bInit == 0) return NULL;

    //сохряняем указатель на позицию посл. записанного в массив символа

    if( uUartNum < 3 && punfo -> uiUart_Mode_dma == 1)
    {
        pdma   = get_pdma_dev( punfo -> uiDMA_id_rx );
    	savedUartWP = (unsigned int)pdma->DAR - (unsigned int)punfo->pRXBuffPtr;
    }
    else
    {
    	savedUartWP = (unsigned int)punfo->RxPrt - (unsigned int)punfo->pRXBuffPtr;
    }

    //определяем размер записанных, но ещё не считанных данных по УАРТу через ЕДМА
    //вычисляем размер записанных, но ещё не считанных данных по УАРТу через ЕДМА если не было переполнения массива
    if (punfo->UartReadIdx < savedUartWP)
    {
        size = savedUartWP - punfo->UartReadIdx;
    }
    //вычисляем размер записанных, но ещё не считанных данных по УАРТу через ЕДМА 
    //если циклический массив успел заполнится и начал писаться дальше
    else if (punfo->UartReadIdx > savedUartWP)
    {
        size = punfo->RxBuffSize - punfo->UartReadIdx + savedUartWP;
    }
    //если размер записанных данных превышает заданный пользователем размер для чтения
         if( uMode & 1 )
         {
           if(size > uUserBufferSize) size = uUserBufferSize;
           *uReaded = 0;
         }
    //Вычисление адреса последнего считанного из массива Uart1RxBuffer элемента
    address = (unsigned int)punfo->pRXBuffPtr + punfo->UartReadIdx;
    clearRemainder = 0;
    clearSize = 0;
    //Если количество записанных через ЕДМА данных выходит за границы
    //адреса массива Uart1RxBuffer - разбить
    if ((address + size) > punfo->MaxAddress)
    {
        clearRemainder = (address + size) - punfo->MaxAddress;
        clearSize = size - clearRemainder;
    }
    else if(size)
    {
        clearSize = size;
    }

    if ( clearSize )
    {
      if( uMode & 1 )
      {
        memcpy(pUserBufferPtr, (unsigned int*)address, clearSize);
        if(clearRemainder) memcpy(pUserBufferPtr+clearSize, punfo->pRXBuffPtr, clearRemainder);
        *uReaded=size;
        punfo->UartReadIdx+=size;
        punfo->UartReadIdx &= (punfo->RxBuffSize-1);
        return pUserBufferPtr;
      }
    }
    //Считываем все данные из буфера
    while ( size )
    {
        currentChar = punfo->pRXBuffPtr[ punfo->UartReadIdx ++ ];
        punfo->UartReadIdx &= ( punfo->RxBuffSize - 1 );
        punfo->pTXTBuffPtr[ punfo->localRP ++ ] = currentChar;
        if (currentChar == '\n' || punfo->localRP > TRANSMIT_LENGTH - 1)
        {
            punfo->pTXTBuffPtr[ punfo->localRP ]=0;
            punfo->localRP = 0;
            return punfo->pTXTBuffPtr;
        }
        size --;
    }
return NULL; 
}

void Uart_EDMA_RX_Init(unsigned int uUartNum);

/*unsigned short CalcDelitel(unsigned int baudrate)
{
    unsigned int cur, low, high, delitel, err_cur, err_low, err_high;
    cur = (OSCILLATOR/16)/baudrate;
    high = cur + 1; low = cur - 1;
    err_cur  = _abs(baudrate-OSCILLATOR/(cur*16));
    err_high = _abs(baudrate-OSCILLATOR/(high*16));
    err_low  = _abs(baudrate-OSCILLATOR/(low*16));
    if(err_cur>err_high){if(err_high>err_low){delitel=low;}else{delitel=high;}}else{if(err_cur>err_low){delitel=low;}else{delitel=cur;}}
return (unsigned short)delitel;
}*/

unsigned get_l3comm_freq(void)
{
       return (((SEL_SPLL & 0xff) + 1)*DEFAULT_XTI_CLOCK >> (DIV_SYS1_CTR&0x1)) >> (DIV_SYS2_CTR&0x1);
}

int Uart_Init(unsigned int uUartNum, unsigned int baudrate, unsigned int even, unsigned int mode_dma)
{
    register _UART_Nfo        *punfo;
    //
    int dma_id, l3_freq = 0;
    int divisor = 0;
        punfo     = &unfo[uUartNum];
        uart_reg_t* uart;
    //
     memset(punfo, 0, sizeof(_UART_Nfo));
     memset(&UartRxBuffer[uUartNum],0, UART_RX_BUFF_SIZE[uUartNum]);
    //
    punfo->ujob=NULL;
    l3_freq = get_l3comm_freq();
    if(baudrate >= 9600)
    {
    	divisor  =  l3_freq;
    	divisor  /= 16;
    	divisor  /= baudrate;
    }
    else
    {
    	divisor = ((baudrate * (1 + (SEL_SPLL & 0xFF))) >> (DIV_SYS1_CTR&0x1)) >> (DIV_SYS2_CTR&0x1);
    }
    //
    GATE_SYS_CTR |= (CLK_UART0_EN << uUartNum); //Enable CLK
    GPIO0(UART_GPIO[uUartNum][0]) |= UART_GPIO[uUartNum][1] | UART_GPIO[uUartNum][2] | UART_GPIO[uUartNum][3] | UART_GPIO[uUartNum][4]; //Set pins
    //
    uart = getUartHandler(uUartNum);
    //
    punfo->uiUart_Mode_dma = mode_dma;
    //Check busy
    if( uart->USR & 0x1 )
    {
        uart->SRR = 7;
        while( uart->USR & 0x1 );
    }
	// Disable
	uart->IIR = 1; //1 = нет прерываний в очереди (изначально = 0)
	uart->DLH_IER = 0;
    //Disable receiving
    uart->MCR = 0;
    //
    //Disable interrupts
    uart->MSR = 0;
    //
    //Clear errors
    uart->LSR = 0;
    uart->SCR = 0;
    //
    //Setup UART
    uart->LCR.DLS = 3; // only 8 bit == 3 !!!!!
    uart->LCR.STOP = 0;// 1 stopBit;
    //
    if(even != 0)
      uart->LCR.PEN = 1;
    else
      uart->LCR.PEN = 0;
    //-----------------------
    if(even == 1)
    uart->LCR.PARITY = 0; // 0 - нечетность
    if(even == 2)
    uart->LCR.PARITY = 1; // 1 - четность

    //Setup divisor
    uart->LCR.DLAB = 1;
    uart->DLH_IER = divisor >> 8;
    uart->RBR_THR_DLL = divisor & 0xFF;
    uart->LCR.DLAB = 0;

    //Configure FIFO
    uart->FCR = FCR_FEWO | FCR_RCVR | FCR_XFIFOR | (0<<UART_FCR_TET_STATE_IDX) | (0<<UART_FCR_RFTL_STATE_IDX); // Режим FIFO + сброс фифо приемника и передатчика (Уровень в фифо 1 символ)
    //Disable auto RTS signal
    uart->MCR = MCR_RTS;
    //return 0;
    // При этм прерывание должно срабатывать когда в фифо есть хотя бы один символ на чтение и очередь на предачу 0
/*  0<<4
Порог заполнения FIFO передатчика, по которому устанавливается
прерывание THRE при работе контроллера в режиме по этому
прерыванию: 00 - FIFO пуст. 01 - 2 символа в FIFO 10 - FIFO заполнен на 1/4 11 - FIFO заполнен на 1/2
    0<<6
Порог заполнения FIFO приемника, при котором формируется
прерывание по наличию данных для чтения:
00 – 1 символ в FIFO 01 – FIFO заполнено на 1/4 10 – FIFO заполнено на 1/2 11 – FIFO содержит на 2 слова меньше, чем возможно.
*/
     punfo->pRXBuffPtr = (unsigned char *)&(UartRxBuffer[uUartNum]);
     punfo->RxPrt 	   = punfo->pRXBuffPtr;
     punfo->pTXTBuffPtr= (unsigned char *)&(UartRead_Buff[uUartNum]);
     punfo->RxBuffSize = UART_RX_BUFF_SIZE[uUartNum];
     punfo->MaxAddress = (unsigned int)UartRxBuffer[uUartNum] + UART_RX_BUFF_SIZE[uUartNum];
     //punfo->rx_ps1     = UART_1_RX_PARAMSET1;
     //punfo->rx_ps2     = UART_1_RX_PARAMSET2;
     //punfo->tx_ps1     = UART_1_TX_PARAMSET1;
     //punfo->tx_ps2     = UART_1_TX_PARAMSET2;
     memset(punfo->pRXBuffPtr,0,punfo->RxBuffSize);
     //
    if( uUartNum < 3 && punfo -> uiUart_Mode_dma == 1 )
    {
    	//uart->LCR.DLAB = 0; // доступ к THR
    	////uart->RBR_THR_DLL = IER_PTIME; // Для контроля через pdma
    	//uart->DLH_IER = IER_PTIME; // Для контроля через pdma . В документации на страницы 1560 ошибка Регистр IER ошибочно назван THR
    	//
        //Конфигурация дма на прием RX переписывает одну и ту же область памяти (для каждого уарта - свою)
        dma_id = uUartNum * 2;
        punfo->uiDMA_id_tx = dma_id;     //TX
        punfo->uiDMA_id_rx = dma_id + 1; //RX
        //
    	pdma_fill_link((pdma_mem_chain *)&UartRXmem_chain[uUartNum][0], punfo->pRXBuffPtr, (void *)&uart->RBR_THR_DLL, UART_RX_BUFF_SIZE[uUartNum], (pdma_mem_chain *)0, 2);//1
    	//pdma_fill_link((pdma_mem_chain *)&UartRXmem_chain[uUartNum][0], punfo->pRXBuffPtr, (void *)&uart->RBR_THR_DLL, UART_RX_BUFF_SIZE[uUartNum], (pdma_mem_chain *)&UartRXmem_chain[uUartNum][1], 2);//1
    	//pdma_fill_link((pdma_mem_chain *)&UartRXmem_chain[uUartNum][1], punfo->pRXBuffPtr, (void *)&uart->RBR_THR_DLL, UART_RX_BUFF_SIZE[uUartNum], (pdma_mem_chain *)&UartRXmem_chain[uUartNum][0], 2);//1
    	//
    	//risc_enable_interrupt(PDMA_INT_FLAG0, GIC_PL390_TARGET_CPU0, 0xF1);
    	//risc_register_interrupt(&uart_dma_interrupt_handler, PDMA_INT_FLAG0, 0);
    	//
		pdma_enable_interrupt(punfo->uiDMA_id_rx, IntTfr, uart_dma_interrupt_handler);
		pdma_enable_interrupt(punfo->uiDMA_id_rx, IntSrcTran, uart_dma_interrupt_handler);
		pdma_enable_interrupt(punfo->uiDMA_id_rx, IntDstTran, uart_dma_interrupt_handler);
		pdma_enable_interrupt(punfo->uiDMA_id_rx, IntBlock, uart_dma_interrupt_handler);
		pdma_enable_interrupt(punfo->uiDMA_id_rx, IntError, uart_dma_interrupt_handler);
    	//
    	pdma_start_chain(punfo->uiDMA_id_rx, &UartRXmem_chain[uUartNum][0], &UartRXmem_chain[uUartNum][1]);
    }
    else
    {
        punfo->uiDMA_id_tx = (1<<8);
        punfo->uiDMA_id_rx = (1<<8);
        switch( uUartNum )
        {
        	case 0:
        		risc_enable_interrupt(UART0_INTR, GIC_PL390_TARGET_CPU0, 0xF1);
        		risc_register_interrupt(&uart0_interrupt_handler, UART0_INTR, 0);
        	break;
        	case 1:
        		risc_enable_interrupt(UART1_INTR, GIC_PL390_TARGET_CPU0, 0xF1);
        		risc_register_interrupt(&uart1_interrupt_handler, UART1_INTR, 0);
        	break;
        	case 2:
        		risc_enable_interrupt(UART2_INTR, GIC_PL390_TARGET_CPU0, 0xF1);
        		risc_register_interrupt(&uart2_interrupt_handler, UART2_INTR, 0);
        	break;
        	case 3:
        		risc_enable_interrupt(UART3_INTR, GIC_PL390_TARGET_CPU0, 0xF1);
        		risc_register_interrupt(&uart3_interrupt_handler, UART3_INTR, 0);
        	break;
        }
    	uart->DLH_IER = IER_ERBFI       //Receive interrupt
            //| IER_ETBEI             //Empty THR
            //| IER_PTIME            //Custom size THR
            ;
    }
    //
    punfo->bInit = 1;
    return 1;

}

void uart_disable(unsigned int uUartNum)
{
    uUartNum &= 0x3;
    uart_reg_t* uart = getUartHandler(uUartNum);
    register _UART_Nfo        *punfo;
           punfo     = &unfo[uUartNum];
    //
    punfo->bInit = 0;
    //Disable interrupts
    uart->DLH_IER = 0;
    //
    GPIO0(UART_GPIO[uUartNum][0]) &= ~(UART_GPIO[uUartNum][1] | UART_GPIO[uUartNum][2] | UART_GPIO[uUartNum][3] | UART_GPIO[uUartNum][4]);  //Unset pins
    GATE_SYS_CTR &= ~(CLK_UART0_EN << uUartNum); //Disable CLK
    //
    punfo     = &unfo[uUartNum];
    //
    if(punfo -> uiUart_Mode_dma == 1)
    {
    	pdma_stop(punfo->uiDMA_id_tx);
    	pdma_stop(punfo->uiDMA_id_rx);
    }
}


void uart_interrupt_handler( int uartNum )
{
    //Get UART number
    //punfo = &unfo[uartNum];
    //int uartNum = (ulICCIAR - GIC_UART0_INTR) & 0x3;
    uart_reg_t* uart = getUartHandler(uartNum);

    //Get ID interrupt
    int uart_status = uart->IIR & 0xF;
    volatile int status = 0;

        if(uart_status & UART_INT_MODEM_STATUS)
        {
            status = uart->MSR;
        }
        //if(uart_status & UART_INT_NO_INTERRUPT){}
        if(uart_status & UART_INT_THR_EMPTY)
        {
			// Завершена передача? if ( UART_IIR_INTERRUPT_STATUS_TX == ( uart_status & UART_IIR_INTERRUPT_STATUS_MASK ) )
            uart_data_transfer(uartNum);
        }
        if(uart_status & UART_INT_DATA_RECIEVE)
        {
            uart_data_read(uartNum);
        }
        if(uart_status & UART_INT_RECEIVER_LINE_STATUS)
        {
            status = uart->LSR;
        }
        if(uart_status & UART_INT_BUSY_DETECT)
        {
            status = uart->USR;
        }
    //if(callback != 0) callback(uartNum, int_id, status);
}

/*

uMode бит 0
   не используется
uMode = 1 / бит 1
   == 0 вызывается из основной программы
   == 1 вызывается из прерывания
uMode = 4 / бит 2
   == 0 Буфер на передачу будет удалён после вызова ф-ции
   == 1 Данные передаются со статического буфера и не изменятся
*/

void WriteUart(unsigned int uUartNum, unsigned char * dPtr, unsigned int uSize, unsigned int uMode)
{
    register _UART_Job  *ujob_ptr, *ujob_ptr_tmp;
    register _UART_Nfo  *punfo;
    //
        punfo     = &unfo[uUartNum];
        uart_reg_t * uart = getUartHandler(uUartNum);
    //
    if(punfo->bInit == 0) return;

     if(!(uMode&2))
     {
	    //TSR &= 0xFFFFFFFE; //Clear GIE
       //asm("           DINT    ");
     }
     //
     //Если ЕДМА свободен для передачи новой порции данных - инициализировать передачу
     //
     if(punfo->usTotalReservd==0)
     {
        punfo->ujob = malloc ( sizeof(_UART_Job) );
        ujob_ptr=punfo->ujob;
        ujob_ptr->uiSize = uSize;
        ujob_ptr->TxSize = 0;
        ujob_ptr->UART_Job_next=NULL;
        if(uMode&4)
        {
            ujob_ptr->ucBlockDataPtr=dPtr;
            ujob_ptr->bMode=0;
        }
        else
        {
            ujob_ptr->ucBlockDataPtr = malloc (ujob_ptr->uiSize);
            memcpy(ujob_ptr->ucBlockDataPtr, dPtr, ujob_ptr->uiSize);
            ujob_ptr->bMode=1;
        }
        punfo->usTotalReservd++;
        //Прерывания запрещяются - CACHE_wbL2i - нормально
        //Запускаем передачу по УАРТ через ЕДМА, указывая адрес массива для чтения 
        //и кол-во передаваемой информации
        if(uUartNum < 3 && punfo -> uiUart_Mode_dma == 1)
        {
        //Прерывания запрещяются - CACHE_wbL2i - нормально
        //Запускаем передачу по УАРТ через ЕДМА, указывая адрес массива для чтения 
        //и кол-во передаваемой информации
        //dmaTXInit(uUartNum, ujob_ptr->ucBlockDataPtr, ujob_ptr->uiSize);
        	pdma_hw_configure(punfo->uiDMA_id_tx, (void *)&uart->RBR_THR_DLL, ujob_ptr->ucBlockDataPtr, ujob_ptr->uiSize, NULL, 2);// TX
        	pdma_run(punfo->uiDMA_id_tx);
        }
        else // передаем через прерывания
        {
            ujob_ptr_tmp = punfo->ujob;
			//while ( uart->FCR & UART_FLAG_TXFF ) //? забиваем буфер фифо
			while ( uart->TFL < UART_FIFO_LENGTH && punfo->ujob != NULL/* && punfo->ujob->TxSize < punfo->ujob->uiSize*/) //? забиваем буфер фифо
			{
    
                if(punfo->ujob->TxSize >= punfo->ujob->uiSize)
                {
                    ujob_ptr_tmp = (_UART_Job *)((punfo->ujob)->UART_Job_next);//сохраняем на следующий
                    //Текущую структуру уже передали
                    if(punfo->ujob->bMode)
                    {
                      free (punfo->ujob->ucBlockDataPtr);//Убиваем буффер
                    }
                    free (punfo->ujob);//и структуру
                    punfo->usTotalReservd--;
                    //ставим новую на первое место.
                    punfo->ujob=ujob_ptr_tmp;
                }
                if(ujob_ptr_tmp!=NULL)
                {
                  uart->RBR_THR_DLL = ujob_ptr_tmp->ucBlockDataPtr[punfo->ujob->TxSize++];
                }
		    }
			if(uart->TFL > 0 && punfo->ujob != NULL) // буфер заполнили - взводим прерывание
			{
		    	uart->DLH_IER = IER_ERBFI       //Receive interrupt
		            | IER_ETBEI             //Empty THR
					;
				asm volatile("dsb; isb;");
			}
        }
     }
     //иначе - поставить в очередь
     else
     {
           ujob_ptr=punfo->ujob;
           while(ujob_ptr->UART_Job_next != NULL)
           {
                 ujob_ptr = ujob_ptr->UART_Job_next;
           }
           //--
           ujob_ptr->UART_Job_next = malloc(sizeof( _UART_Job));
           ujob_ptr=ujob_ptr->UART_Job_next;
           ujob_ptr->uiSize=uSize;
           if(uMode&4)
           {
            ujob_ptr->ucBlockDataPtr=dPtr;
            ujob_ptr->bMode=0;
           }
           else
           {
            ujob_ptr->ucBlockDataPtr=malloc (ujob_ptr->uiSize);
            memcpy(ujob_ptr->ucBlockDataPtr, dPtr, ujob_ptr->uiSize);
            ujob_ptr->bMode=1;
           }
           ujob_ptr->UART_Job_next=NULL;
           punfo->usTotalReservd++;
           //
           if(punfo -> uiUart_Mode_dma == 0)
           {
        	   uart->DLH_IER = IER_ERBFI       //Receive interrupt
	            | IER_ETBEI             //Empty THR
	            | IER_PTIME;            //Custom size THR
           }
     }
      if(!(uMode&2))
      {
	    //TSR |= 0x1; //Set GIE 
       //asm("           RINT    ");
      }
}


//Вызывается из прерывания ДМА для запроса следующих данных для передачи
unsigned int GetNextQueueBlock(unsigned int uUartNum)
{
register _UART_Nfo  *punfo=&unfo[uUartNum];
uart_reg_t* uart = getUartHandler(uUartNum);
         _UART_Job  *ujob_ptr_tmp;

    if(punfo->usTotalReservd==0)
    {
         return 0;//нет данных
    }
    ujob_ptr_tmp=(punfo->ujob)->UART_Job_next;//сохраняем на следующий
    //Текущую структуру уже передали
    if(punfo->ujob->bMode)
    {
      free  (punfo->ujob->ucBlockDataPtr);//Убиваем буффер
    }
    free  (punfo->ujob);//и структуру
    punfo->usTotalReservd--;
    //ставим новую на первое место.
    punfo->ujob=ujob_ptr_tmp;
    if(ujob_ptr_tmp!=NULL)
    {
        //CACHE_wbL2i((unsigned int*)ujob_ptr_tmp->ucBlockDataPtr,ujob_ptr_tmp->uiSize,CACHE_WAIT);
        //ДМА
        //pdma_configure(dma_id, (void *)&uart->RBR_THR_DLL, ujob_ptr_tmp->ucBlockDataPtr, ujob_ptr_tmp->uiSize, 0, 6);// TX
        pdma_hw_configure(punfo->uiDMA_id_tx, (void *)&uart->RBR_THR_DLL, ujob_ptr_tmp->ucBlockDataPtr, ujob_ptr_tmp->uiSize, NULL, 2);// TX
        pdma_run(punfo->uiDMA_id_tx);
        //
      return 1;
    }
  return 0;
}

void uart_data_transfer(unsigned int uUartNum)
{
    uart_reg_t* uart = getUartHandler(uUartNum);
    register _UART_Job  *ujob_ptr_tmp; // *ujob_ptr,
    register _UART_Nfo  *punfo;
    //
        punfo     = &unfo[uUartNum];
    //--------------------
		if(punfo->usTotalReservd > 0)
		{
	        uart->DLH_IER = IER_ERBFI       //Receive interrupt
	            //| IER_ETBEI             //Empty THR
	            //| IER_PTIME            //Custom size THR
	                ;
			//---------------------------------------
				ujob_ptr_tmp = punfo->ujob;
				//Заполняем фифо
				//while ( uart->FCR & UART_FLAG_TXFF ) //?
				while (uart->TFL < UART_FIFO_LENGTH && punfo->ujob != NULL/* && ujob_ptr_tmp != NULL*/)
				{
                    if(punfo->ujob->TxSize >= punfo->ujob->uiSize) // Если данные по текущей задаче переданы
                    {
                        ujob_ptr_tmp = (_UART_Job *)((punfo->ujob)->UART_Job_next);//сохраняем на следующий
                        //Текущую структуру уже передали
                        if(punfo->ujob->bMode)
                        {
                          free (punfo->ujob -> ucBlockDataPtr);//Убиваем буффер
                        }
                        free (punfo -> ujob);//и структуру
                        punfo->usTotalReservd--;
                        //ставим новую на первое место.
                        punfo->ujob = ujob_ptr_tmp;
                    }
                    if(ujob_ptr_tmp != NULL)
                    {
                    	uart->RBR_THR_DLL = ujob_ptr_tmp -> ucBlockDataPtr[punfo->ujob->TxSize++];
                    }
			    }
			    //----------------
			    if(punfo->ujob != NULL && uart->TFL > 0 /*punfo->ujob -> TxSize < punfo->ujob -> uiSize*/) // У нас еще есть что передавать // (uart->TFL == UART_FIFO_LENGTH)
			    {
			        uart->DLH_IER = IER_ERBFI       //Receive interrupt
			            | IER_ETBEI             //Empty THR
			            | IER_PTIME;            //Custom size THR
			    }
			    else
			    {
			        uart->DLH_IER = IER_ERBFI       //Receive interrupt
			            //| IER_ETBEI             //Empty THR
			            //| IER_PTIME            //Custom size THR
			                ;
			    }
			//---------------------------------------
			    asm volatile("dsb; isb;");
        }
}



void uart_data_read(unsigned int uUartNum)
{
    register _UART_Nfo  *punfo;
    uart_reg_t* uart = getUartHandler(uUartNum);
    punfo       = &unfo[uUartNum];
    int rx_size = UART_RX_BUFF_SIZE[uUartNum];
    //
    while(uart->RFL > 0)
    {
			//Передаем на верхний уровень
			*(punfo->RxPrt++) = uart->RBR_THR_DLL;
			//punfo->RxPrt &= rx_size;
    	    if( ((unsigned int)punfo->RxPrt - (unsigned int)punfo->pRXBuffPtr) >= rx_size) punfo->RxPrt -= rx_size;
    }
}

/*
int is_tx_empty(unsigned int uartNum)
{
    uart_reg_t* uart = getUartHandler(uartNum);
    return !(uart_get_available_read(&tx_buffer[uartNum]) > 0 || uart->TFL > 0);
}
int is_rx_empty(unsigned int uartNum)
{
    uart_reg_t* uart = getUartHandler(uartNum);
    return !(uart_get_available_read(&rx_buffer[uartNum]) > 0 || uart->RFL > 0);
}

void uart_putchar (unsigned int idUart, short c)
{
        unsigned long ulBase = uart_base(idUart);

	if ( 0 != ulBase )
	{
		while ( ( *UART_LSR(ulBase) & UART_LSR_THRE ) == 0 );
		*UART_THR(ulBase) = c;
	}
}

unsigned short uart_getchar (unsigned int idUart)
{
    unsigned short c = 0;
    unsigned long ulBase = uart_base(idUart);

	if ( 0 != ulBase )
	{
		while ( ( *UART_LSR(ulBase) & UART_LSR_RDR ) == 0 ){continue;};
                c = ((unsigned short) *UART_RBR(ulBase));
    }
        return c;
}

int uart_puts(unsigned int uart_id, const char* str)
{
    do{
        uart_putchar(uart_id, *str);
    } while(*str++);
    return 0;
}
*/
unsigned short uart_getchar (unsigned int uUartNum)
{
    unsigned short c = 0;
    uart_reg_t* uart = getUartHandler(uUartNum);

	if ( 0 != uart )
	{
		while ( ( uart->LSR & UART_LSR_RDR ) == 0 ){continue;};
                c = ((unsigned short) uart->RBR_THR_DLL);
    }
        return c;
}

void uart_putchar (unsigned int uUartNum, short c)
{
    uart_reg_t* uart = getUartHandler(uUartNum);

	if ( 0 != uart )
	{
		while ( ( uart->LSR & UART_LSR_THRE ) == 0 );
		uart->RBR_THR_DLL = c;
	}
}
int uart_puts(unsigned int uart_id, const char* str)
{
    do{
        uart_putchar(uart_id, *str);
    } while(*str++);
    return 0;
}

