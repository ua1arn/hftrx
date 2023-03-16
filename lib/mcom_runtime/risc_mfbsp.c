// ***************** simple_time.� ***********************************
//
//  MFBSP
//
//
// *******************************************************************
#include "risc_mfbsp.h"
#include "gic.h" 
#include "pll.h"
#include <string.h>

//MFBSP0_DMAIRQ0
//MFBSP0_DMAIRQ1
//MFBSP1_DMAIRQ0
//MFBSP1_DMAIRQ1

void IFRNS_INT_PROC(void);
void FRNS_INT_PROC(void);

//#pragma DATA_SECTION(".myfastdata")
//#pragma DATA_ALIGN(MCBSP0_FRNS_BUFF_LEN);
int McBSP0Ping[MCBSP0_BUFF_LEN*2] __attribute__ ((aligned (8)));

//#pragma DATA_SECTION(".myfastdata")
//#pragma DATA_ALIGN(MCBSP0_FRNS_BUFF_LEN);
int McBSP0Pong[MCBSP0_BUFF_LEN*2] __attribute__ ((aligned (8)));

//#pragma DATA_SECTION(".myfastdata")
//#pragma DATA_ALIGN(MCBSP1_IFRNS_BUFF_LEN);
int McBSP1Ping[MCBSP1_BUFF_LEN*2] __attribute__ ((aligned (8)));

//#pragma DATA_SECTION(".myfastdata")
//#pragma DATA_ALIGN(MCBSP1_IFRNS_BUFF_LEN);
int McBSP1Pong[MCBSP1_BUFF_LEN*2] __attribute__ ((aligned (8)));



dma_mfbsp_chain	dma0_mfbsp_rx_chain[2] __attribute__ ((aligned (8)));
dma_mfbsp_chain dma1_mfbsp_rx_chain[2] __attribute__ ((aligned (8)));

int i = 0;
int b = 0;
void mfbsp0_rx_dma_interrupt(int id)
{
    int a;
    mfbsp_dma_ch* DMAch =  get_mfbsp_dma_dev(0, 0);
    a = DMAch->CSR;
    IFRNS_INT_PROC();
    //i++;
}

void mfbsp0_tx_dma_interrupt(int id)
{
    //i++;
}

void mfbsp1_rx_dma_interrupt(int id)
{
    int a;
    mfbsp_dma_ch* DMAch =  get_mfbsp_dma_dev(1, 0);
    a = DMAch->CSR;
    //IFRNS_INT_PROC(); // Пока отрубим второй канал
    b++;
}

void mfbsp1_tx_dma_interrupt(int id)
{
    //i++;
}

/*
MFBSP0_RXIRQ
MFBSP0_TXIRQ
MFBSP0_SRQ
MFBSP1_RXIRQ
MFBSP1_TXIRQ
MFBSP1_SRQ
*/

void mfbsp_init_dma()
{
	memset(McBSP0Ping, 0, sizeof(McBSP0Ping));
	memset(McBSP1Ping, 0, sizeof(McBSP1Ping));
	memset(McBSP0Pong, 0, sizeof(McBSP0Pong));
	memset(McBSP1Pong, 0, sizeof(McBSP1Pong));
    		risc_enable_interrupt(MFBSP0_DMAIRQ1, GIC_PL390_TARGET_CPU0, 0xF1);
    		risc_register_interrupt(&mfbsp0_rx_dma_interrupt, MFBSP0_DMAIRQ1, 0);
    		risc_enable_interrupt(MFBSP1_DMAIRQ1, GIC_PL390_TARGET_CPU0, 0xF1);
    		risc_register_interrupt(&mfbsp1_rx_dma_interrupt, MFBSP1_DMAIRQ1, 0);
    		//4test
    		risc_enable_interrupt(MFBSP0_DMAIRQ0, GIC_PL390_TARGET_CPU0, 0xF1);
    		risc_register_interrupt(&mfbsp0_tx_dma_interrupt, MFBSP0_DMAIRQ0, 0);
    		risc_enable_interrupt(MFBSP1_DMAIRQ0, GIC_PL390_TARGET_CPU0, 0xF1);
    		risc_register_interrupt(&mfbsp1_tx_dma_interrupt, MFBSP1_DMAIRQ0, 0);

    		//risc_enable_interrupt(MFBSP0_DMAIRQ0, GIC_PL390_TARGET_CPU0, 0xF1);
    		//risc_register_interrupt(&mfbsp0_tx_dma_interrupt, MFBSP0_DMAIRQ0, 0);
    		//risc_enable_interrupt(MFBSP1_DMAIRQ0, GIC_PL390_TARGET_CPU0, 0xF1);
    		//risc_register_interrupt(&mfbsp1_tx_dma_interrupt, MFBSP1_DMAIRQ0, 0);
    		spi_receiver_configure(0, 16, 1);
    		spi_receiver_configure(1, 16, 1);
//dma_mfbsp_chain * mfbsp_fill_link(dma_mfbsp_chain *link, void *addr, unsigned int size, unsigned int flags)
            //link->CSR=(size-1)<<16|0x3C|flags;
            //
            //   111100b 111100b
            //
    		mfbsp_fill_link2(&dma0_mfbsp_rx_chain[0], &dma0_mfbsp_rx_chain[1], &McBSP0Ping[0], MCBSP0_BUFF_LEN/2, 1);
    		mfbsp_fill_link2(&dma0_mfbsp_rx_chain[1], &dma0_mfbsp_rx_chain[0], &McBSP0Pong[0], MCBSP0_BUFF_LEN/2, 1);
    		//                                                                  
    		mfbsp_fill_link2(&dma1_mfbsp_rx_chain[0], &dma1_mfbsp_rx_chain[1], &McBSP1Ping[0], MCBSP1_BUFF_LEN/2, 1);
    		mfbsp_fill_link2(&dma1_mfbsp_rx_chain[1], &dma1_mfbsp_rx_chain[0], &McBSP1Pong[0], MCBSP1_BUFF_LEN/2, 1);
    		//
    		mfbsp_start_chain(0, 0, &dma0_mfbsp_rx_chain[0]);
    		mfbsp_start_chain(1, 0, &dma1_mfbsp_rx_chain[0]);
    		//mfbsp_dma_run(0, 0);
			//mfbsp_dma_run(1, 0);
}


mfbsp_port* get_mfbsp_dev(int num)
{
	if (num == 0) return _MFBSPport0;
	else if (num == 1) return _MFBSPport1;
	else return 0;
}

mfbsp_dma_ch* get_mfbsp_dma_dev(int num, int istx)
{
	if (istx)
	{
		if (num == 0) return _MFBSPdmaTXch0;
		else if (num == 1) return _MFBSPdmaTXch1;
		else return 0;
	}
	else
	{
		if (num == 0) return _MFBSPdmaRXch0;
		else if (num == 1) return _MFBSPdmaRXch1;
		else return 0;
	}
	return 0;
}


// Parameters:
//      id      - number of requested MFBSP port
//	len	- lenght of transmitted word in bits
//	cnt	- number of words in frame
//	clk     - clock frequency of transmitter
enum ERL_ERROR spi_transmitter_configure(int id, int len, int cnt, unsigned clk)
{
  mfbsp_port* mfport = get_mfbsp_dev(id);
  if(!mfport)
  {
        return ERL_SYSTEM_ERROR;
  }

  if(len>32 || len<1) return ERL_PROGRAM_ERROR;
  if(cnt>64 || cnt<1) return ERL_PROGRAM_ERROR;

  MFBSP_CSR_REG mfbsp_csr;
  mfbsp_csr._val = 0;
  mfbsp_csr.ltran = 1;
  mfbsp_csr.spi_i2s_en = 1;

  MFBSP_DIR_REG mfbsp_dir;
  mfbsp_dir._val = 0;
  mfbsp_dir.td_dir = 1;
  mfbsp_dir.tcs_dir = 1;
  mfbsp_dir.rcs_dir = 1;
  mfbsp_dir.tclk_dir = 1;
  mfbsp_dir.rclk_dir = 1;

  MFBSP_TCTR_REG tctr;
  tctr._val = 0;
  tctr.ten = 1;
  tctr.tmode = 1;
  tctr.tneg = 1;
  tctr.tdel = 1;
  tctr.tmbf = 1;
  tctr.twordlen = len-1;  // TWORDLEN=31, 
  tctr.twordcnt = cnt-1;   // TWORDCNT=0 
  tctr.ss1 = 1; //

  MFBSP_TCTR_RATE_REG tr_rate;
  tr_rate._val = 0;
  unsigned cpu_clk = getCurrentCPUFreq();
  tr_rate.tclk_rate = (cpu_clk/(clk*2))-1;

  mfport->CSR = mfbsp_csr._val;
  mfport->DIR = mfbsp_dir._val;
  mfport->TCTR = tctr._val;
  mfport->TCTR_RATE = tr_rate._val;

  return ERL_NO_ERROR;
}

// Parameters:
//      id      - number of requested MFBSP port
//	len	- lenght of transmitted word in bits
//	cnt	- number of words in frame
enum ERL_ERROR spi_receiver_configure(int id, int len, int cnt)
{
  mfbsp_port* mfport = get_mfbsp_dev(id);
  if(!mfport)
  {
        return ERL_SYSTEM_ERROR;
  }

  if(len>32 || len<1) return ERL_PROGRAM_ERROR;
  if(cnt>64 || cnt<1) return ERL_PROGRAM_ERROR;

  MFBSP_CSR_REG mfbsp_csr;
  mfbsp_csr._val = 0;
  mfbsp_csr.spi_i2s_en = 1;

  MFBSP_DIR_REG mfbsp_dir;
  mfbsp_dir._val = 0;
  mfbsp_dir.rd_dir = 0;

  MFBSP_RCTR_REG rctr;
  rctr._val = 0;
  rctr.ren = 1;           // 1 - приемник включен
  rctr.rmode = 1;         //spi
  rctr.rneg = 1;          //Полярность тактового сигнала приёмника
  rctr.rdel = 0;          //Задержка начала приёма данных на пол такта .. 1      0 - gj pflytve
  rctr.rmbf = 1;          // 1 - старшим битом вперед
  rctr.rsign = 1;          // 1 - расширять знак
  //rctr.rcs_cp = 0;      // - сигнал ss с внешнего выыода
  //rctr.rclk_cp = 0;
  rctr.rwordlen = len-1;  // RWORDLEN
  rctr.rwordcnt = cnt-1;   // RWORDCNT 

  MFBSP_RCTR_RATE_REG rc_rate;
  rc_rate._val = 0;

  mfport->CSR = mfbsp_csr._val;
  mfport->DIR = mfbsp_dir._val;
  mfport->RCTR = rctr._val;
  mfport->RCTR_RATE = rc_rate._val;

  return ERL_NO_ERROR;
}

enum ERL_ERROR spi_duplex_configure(int id, int len, int cnt, unsigned clk, int ismaster)
{
  mfbsp_port* mfport = get_mfbsp_dev(id);
  if(!mfport)
  {
        return ERL_SYSTEM_ERROR;
  }

  if(len>32 || len<1) return ERL_PROGRAM_ERROR;
  if(cnt>64 || cnt<1) return ERL_PROGRAM_ERROR;

  MFBSP_CSR_REG mfbsp_csr;
  mfbsp_csr._val = 0;
  mfbsp_csr.spi_i2s_en = 1;

  MFBSP_DIR_REG mfbsp_dir;
  mfbsp_dir._val = 0;
  MFBSP_TCTR_RATE_REG tc_rate;
  tc_rate._val = 0;
  MFBSP_RCTR_RATE_REG rc_rate;
  rc_rate._val = 0;
  if (ismaster)
  {
    mfbsp_dir.rclk_dir = 1;
    mfbsp_dir.tclk_dir = 1;
    mfbsp_dir.rcs_dir = 1;
    mfbsp_dir.tcs_dir = 1;
    mfbsp_dir.td_dir = 1;
    unsigned cpu_clk = getCurrentCPUFreq();
    tc_rate.tclk_rate = (cpu_clk/(clk*2))-1;
    rc_rate.rclk_rate = (cpu_clk/(clk*2))-1;
  }
  else mfbsp_dir.rd_dir = 1;

  MFBSP_RCTR_REG rctr;
  rctr._val = 0;
  rctr.ren = 1;
  rctr.rmode = 1;
  rctr.rclk_cp = 1;
  rctr.rcs_cp = 1;
  rctr.rneg = 1;
  rctr.rdel = 1;
  rctr.rmbf = 1;
  rctr.rwordlen = len-1;  // RWORDLEN
  rctr.rwordcnt = cnt-1;   // RWORDCNT

  MFBSP_TCTR_REG tctr;
  tctr._val = 0;
  tctr.ten = 1;
  tctr.tmode = 1;
  tctr.tneg = 1;
  tctr.tdel = 1;
  tctr.tmbf = 1;
  tctr.twordlen = len-1;  // TWORDLEN=31
  tctr.twordcnt = cnt-1;   // TWORDCNT=0
  tctr.ss0 = 1; //

  mfport->CSR = mfbsp_csr._val;
  mfport->DIR = mfbsp_dir._val;
  mfport->RCTR = rctr._val;
  mfport->TCTR = tctr._val;
  mfport->RCTR_RATE = rc_rate._val;
  mfport->TCTR_RATE = tc_rate._val;

  return ERL_NO_ERROR;
}

enum ERL_ERROR mfbsp_dma_configure(int id, void * array, int size, int istx)
{
    mfbsp_dma_ch* DMAch =  get_mfbsp_dma_dev(id, istx);
    if(!DMAch)
    {
        return ERL_SYSTEM_ERROR;
    }

    MFBSP_DMA_CSR_REG mfbsp_dmach;
    mfbsp_dmach._val = 0;
    mfbsp_dmach.wn = 0xf;
    mfbsp_dmach.wcx = size - 1;
    DMAch->CSR = mfbsp_dmach._val;
    DMAch->IR = (unsigned) array;
    return ERL_NO_ERROR;
}

enum ERL_ERROR mfbsp_dma_run(int id, int istx)
{
  mfbsp_dma_ch* DMAch =  get_mfbsp_dma_dev(id, istx);
  if(!DMAch)
  {
    return ERL_SYSTEM_ERROR;
  }
  MFBSP_DMA_CSR_REG csr_reg;
  csr_reg._val = DMAch->CSR;
  csr_reg.run = 1;
  DMAch->CSR = csr_reg._val;
  return ERL_NO_ERROR;
}

enum ERL_ERROR mfbsp_dma_wait(int id, int istx)
{
  mfbsp_dma_ch* DMAch =  get_mfbsp_dma_dev(id, istx);
  if(!DMAch)
  {
    return ERL_SYSTEM_ERROR;
  }
  while(1)
  {
    int is_stop = DMAch->RUN&0x8000;
    if (is_stop) break;
  }
  return ERL_NO_ERROR;
}

dma_mfbsp_chain * mfbsp_fill_link(dma_mfbsp_chain *link, void *addr, unsigned int size, unsigned int flags)
{
    link->IR=(unsigned int)addr;
    link->CP=(unsigned int)(link+1);
    link->CSR=(size-1)<<16|0x3C|flags;
    return link+1;
}

void mfbsp_fill_link2(dma_mfbsp_chain *link, dma_mfbsp_chain *link2, void *addr, unsigned int size, unsigned int flags)
{
    link->IR=(unsigned int)addr;
    link->CP=(unsigned int)(link2);
    // 12 - САМОИНИЦИАЛИЗАЦИЯ 13 IM -ВЫДАВАТЬ ПРЕРЫВАНИЕ
    link->CSR=(size-1)<<16|0x3C|1<<12|1<<13|flags;
}

enum ERL_ERROR mfbsp_start_chain(int id, int istx, dma_mfbsp_chain *link)
{
  mfbsp_dma_ch* DMAch =  get_mfbsp_dma_dev(id, istx);
  if(!DMAch)
  {
    return ERL_SYSTEM_ERROR;
  }
  DMAch->CP=((unsigned int)link)|1;
  return ERL_NO_ERROR;
}

enum ERL_ERROR mfbsp_link_wait(int id, int istx)
{
  mfbsp_dma_ch* DMAch =  get_mfbsp_dma_dev(id, istx);
  if(!DMAch)
  {
    return ERL_SYSTEM_ERROR;
  }
  while(1)
  {
    int is_run = DMAch->RUN&0x1;
    if (!is_run) break;
  }
  return ERL_NO_ERROR;
}
