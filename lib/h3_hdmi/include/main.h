#ifndef MAIN_H_
#define MAIN_H_

#include "hardware.h"
#include "formats.h"

///#include <h3_watchdog.h>
#include "ccu.h"
//#include "system.h"
//#include "uart.h"
//#include "ports.h"
//#include "util.h"
//#include "h3_dma.h"
//#include "irq_timer.h"
//#include "interrupts.h"
#include "display.h"
#include "color.h"
#include "tve.h"
//#include "ILI9341.h"
#include <math.h>
#include "arm_math.h"
#include "arm_common_tables.h"

//#define NDEBUG 1

#define  lcdframebuffer1 0x43000000///0x4e000000 ///__attribute__ ((section ("UNCACHED")));
#define  lcdframebuffer2 (0x43000000+(512*512))
#define  lcdframebuffer3 (0x43000000+2*(512*512))

#define LCDX 512
#define LCDY 270
#define LCD_FRAME_OFFSET (LCDX*LCDY)

#define SOCID_A64	0x1689
#define SOCID_H3	0x1680
#define SOCID_V3S	0x1681
#define SOCID_H5	0x1718
#define SOCID_R40	0x1701


typedef struct UB_Image_t
{
  const uint16_t *table; // Tabelle mit den Daten
  uint16_t width;        // Breite des Bildes (in Pixel)
  uint16_t height;       // Hoehe des Bildes  (in Pixel)
}UB_Image;

typedef struct DMA2D_Koord_t {
  uint32_t source_xp; // Quelle X-Start
  uint32_t source_yp; // Quelle Y-Start
  uint32_t source_w;  // Quelle Breite
  uint32_t source_h;  // Quelle Hцhe
  uint32_t dest_xp;   // Ziel X-Start
  uint32_t dest_yp;   // Ziel Y-Start
}DMA2D_Koord;

extern UB_Image eesm;
extern UB_Image cab;

///volatile uint32_t framebuffer2[512*512] __attribute__ ((section ("UNCACHED")));
///volatile uint32_t framebuffer3[512*512] __attribute__ ((section ("UNCACHED")));

///#include "audio.h"
///#include "dma.h"
///#include "fs.h"
///#include "mmu.h"
///#include "network.h"
///#include "smp.h"
///#include "usb.h"
///#include <stdio.h>
///#include <stdlib.h>
///#include <h3_i2c.h>
///#include <h3_spi.h>

///#include "h3.h"!
///----------------------
#define u32_t uint32_t
#define read32(addr)      (*((volatile unsigned long*)(addr)))
#define write32(addr,v)   (*((volatile unsigned long*)(addr)) =(unsigned long)(v))
#define set_wbit(addr,v) (*((volatile unsigned long*)(addr))|=(unsigned long)(v))

//#define NULL 0
#define SPI0_BASE 0x01c68000

///#define H3_CCU				0x01C20000

typedef unsigned int			virtual_addr_t;
#define u8	uint8_t
#define u16	uint16_t
#define u32	uint32_t
#define u64	uint64_t
typedef unsigned char			u8_t;

///#define ROTATE_LCD 1

struct common_COMPLEX
{
 float re;
 float im;
};

struct common_COMPLEX_int
{
int re;
int im;
};

///---spi---
enum {
	SPI_GCR	= 0x04,
	SPI_TCR	= 0x08,
	SPI_IER	= 0x10,
	SPI_ISR	= 0x14,
	SPI_FCR	= 0x18,
	SPI_FSR	= 0x1c,
	SPI_WCR	= 0x20,
	SPI_CCR	= 0x24,
	SPI_MBC	= 0x30,
	SPI_MTC	= 0x34,
	SPI_BCC	= 0x38,
	SPI_TXD	= 0x200,
	SPI_RXD	= 0x300,
};


//============================================================================

//============================================================================
#define   TotalBend   25
#define   TotalFilter  8
#define   TotalMode   7

#define   LSB         0
#define   USB         1
#define   CW          2
#define   CWR         3
#define   AM          4
#define   FM          5
#define   FM2         6
//============================================================================

//============================================================================
//#define MIN(a,b)        (((a) < (b)) ? (a) : (b))
//#define MAX(a,b)        (((a) > (b)) ? (a) : (b))
#define min(a,b)        (((a) < (b)) ? (a) : (b))
#define max(a,b)        (((a) > (b)) ? (a) : (b))
//============================================================================
//============================================================================
#define SmetrX  44
#define SmetrY  153

#pragma pack (1)

#define FFT_FILTER_SIZE 1024///2048
#define Buf_float_tmp2_size     FFT_FILTER_SIZE*4

typedef struct _SDR_MODE{

  unsigned char         Band;

  unsigned long         Freq[TotalBend];
  unsigned char         Mode[TotalBend];
  unsigned char         Filter_SSB[TotalBend];
  unsigned char         Filter_AMFM[TotalBend];


  unsigned char         ANF;
  unsigned char         DNR;
  unsigned char         NB;
  unsigned char         NB_LEVEL;
  unsigned char         ATT[TotalBend];
  unsigned char         PRE[TotalBend];
  unsigned char         BYPS;
  unsigned char         Moni;
  unsigned char         Moni_vol;

  unsigned char         AGC_RX[TotalBend];
  unsigned char         AGC_TX;

  unsigned char         TX_PWR;
  unsigned char         TX_ECHO;
  signed long           Echo_del;
  float                 Echo_vol;
  float                 Echo_rpt;

  unsigned char         TX_EQ;
  unsigned char         RX_EQ;
  signed char           RX_SEQ[5];
  signed char           TX_SEQ[5];

  unsigned char         RTX_Mode;//0-RX 1-TX

  unsigned char         Menu_Sel;
  /*unsigned char         Tmp;// for debug
  float                 fTmp;// for debug
  unsigned char         ValMode;//0-freq 1-coint
  signed long           ValCount;*/

  unsigned char crc_static;
  unsigned char crc_sum;

}SDR_MODE;
//============================================================================

//============================================================================
#pragma pack (1)
typedef struct _SDR_SETUP{

  unsigned short        FFT_Size;
  float                 FFT_Spect_beta_up;
  float                 FFT_Spect_beta_dwn;
  unsigned char         FFT_Spect_ofset;
  unsigned long         Sample_Rate;
  unsigned char         SpecScale;

  unsigned char         DDS_type;//0-AD9851 1-AD9852 2-AD9951
  unsigned char         DDS_mul[4];
  unsigned long         DDS_mck[4];
  float                 koeff_DDS;

  unsigned short        RX_GAIN;
  unsigned short        MIC_GAIN;

  unsigned short        AMFM_Centr;
  unsigned short        FILTER_SSB[TotalFilter][2];
  unsigned short        FILTER_AMFM[TotalFilter];

  unsigned char         DNR_Ord;
  unsigned char         DNR_Del;
  float                 DNR_Rate;
  float                 DNR_Leak;

  float                 rx_mag[TotalBend];
  float                 rx_ph[TotalBend];

  float                 tx_mag[TotalBend];
  float                 tx_ph[TotalBend];

  unsigned char         tx_mix;//100%
  unsigned char         tx_fm_dev;//
  unsigned char         tx_am_mod;//

  unsigned char crc_static;
  unsigned char crc_sum;
}SDR_SETUP;
//============================================================================

//============================================================================
#pragma pack (1)
typedef struct _SDRSTR{

  SDR_MODE              SdrMode;
  SDR_SETUP             SdrSetup;

  unsigned char         Spectr[256];
  unsigned char         Spectr_avg[256];

  unsigned char         TX_Tone;
  float                 Smetr;

  unsigned char         FmPll_Init;

  unsigned char         ValMode;//0-freq 1-coint
  signed long           ValCount;

  unsigned char         Tmp;// for debug
  float                 fTmp;// for debug

}SDRSTR;
//============================================================================

static inline u8_t read8(virtual_addr_t addr)
{
	return( *((volatile u8_t *)(addr)) );
}

static inline void write8(virtual_addr_t addr, u8_t value)
{
	*((volatile u8_t *)(addr)) = value;
}


#ifdef GDBSTUB
///#include "gdb/gdbstub.h"
#endif

#endif
