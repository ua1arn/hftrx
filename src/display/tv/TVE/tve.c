#include "hardware.h"
#include "formats.h"
#include "tve.h"

//#include "Gate.h"
//#include "delay.h"

typedef struct
{
 volatile uint32_t GCTL_REG;           // !!! 0x00 TCON Control
 volatile uint32_t GINT0_REG;          // !!! 0x04 Interrupt_0
 volatile uint32_t GINT1_REG;          // !!! 0x08 Interrupt_1

 volatile uint32_t RES0[13];

 volatile uint32_t SRC_CTL_REG;        // !!! 0x40 TCON0 Control

 volatile uint32_t RES1[17];

 volatile uint32_t IO_POL_REG;         // !!! 0x88 TV SYNC Signal Polarity Register
 volatile uint32_t IO_TRI_REG;         // !!! 0x8C TV SYNC Signal volatile Control Register

 volatile uint32_t CTL_REG;            // !!! 0x90 TCON1 Control
 volatile uint32_t BASIC_REG[6];       // !!! 0x94..0xA8 TCON1 Basic Timing 0..5

 volatile uint32_t RES2[20];

 volatile uint32_t DEBUG_REG;          // !!! 0xFC TCON Debug Information
}
tcon_tv0;

#define TCON_TV0_local2 ((tcon_tv0*)TCON_TV0_BASE)

void VIDEO1_PLL(void)
{
 uint32_t v=CCU->PLL_VIDEO1_CTRL_REG;

 v&=~(0xFF<<8);
 v|=(1UL<<31)|(1<<30)|((72-1)<<8);      //N=72 => PLL_VIDEO1(4x) = 24*N/M = 24*72/2 = 864 MHz

 CCU->PLL_VIDEO1_CTRL_REG=v;

 CCU->PLL_VIDEO1_CTRL_REG|=(1<<29);          //Lock enable

 while(!(CCU->PLL_VIDEO1_CTRL_REG&(1<<28))); //Wait pll stable
 local_delay_ms(20);

 CCU->PLL_VIDEO1_CTRL_REG&=~(1<<29);         //Lock disable
}

void TVE_Clock(void)
{
	CCU->TVE_BGR_REG&=~((1<<17)|(1<<16));                 //assert reset for TVE & TVE_TOP
	CCU->TVE_CLK_REG=(1UL<<31)|(3<<24)|(1<<8)|(2-1);      //clock on, PLL_VIDEO1(4x), N=2, M=2 => 864/2/2 = 216 MHz
	CCU->TVE_BGR_REG|=(1<<1)|1;                           //gate pass for TVE & TVE_TOP
	CCU->TVE_BGR_REG|=(1<<17)|(1<<16);                    //de-assert reset for TVE & TVE_TOP
}

void DPSS_Clock(void)
{
	CCU->DPSS_TOP_BGR_REG&=~(1<<16);                      //assert reset DPSS_TOP
	CCU->DPSS_TOP_BGR_REG|=1;                             //gate pass DPSS_TOP
	CCU->DPSS_TOP_BGR_REG|=(1<<16);                       //de-assert reset DPSS_TOP
}

void TCONTV_Clock(void)
{
 CCU->TCONTV_BGR_REG&=~(1<<16);                        //assert reset TCON_TV
 CCU->TCONTV_CLK_REG=(1UL<<31)|(1<<24)|(2<<8)|(11-1);  //clock on, PLL_VIDEO0(4x), N=4, M=11 => 1188/4/11 = 27 MHz
 CCU->TCONTV_BGR_REG|=1;                               //gate pass TCON_TV
 CCU->TCONTV_BGR_REG|=(1<<16);                         //de-assert reset TCON_TV
}

void TCONTV_Init(uint32_t mode)
{
 if(mode==DISP_TV_MOD_NTSC)
 {
  TCON_TV0_local2->CTL_REG = 0x80000000 | ((525 - 480 ) << 4);   //VT-V
  TCON_TV0_local2->BASIC_REG[0] = ((720 - 1) << 16) | (480 - 1); //H,V
  TCON_TV0_local2->BASIC_REG[1] = ((720 - 1) << 16) | (480 - 1);
  TCON_TV0_local2->BASIC_REG[2] = ((720 - 1) << 16) | (480 - 1);
  TCON_TV0_local2->BASIC_REG[3] = ((858 - 1) << 16) | ( 60 - 1); //HT, HBP
  TCON_TV0_local2->BASIC_REG[4] = ((525 * 2) << 16) | ( 30 - 1); //VT*2, VBP
  TCON_TV0_local2->BASIC_REG[5] = (62 << 16) | 6;                //HS, VS
 }
 else //PAL
 {
  TCON_TV0_local2->CTL_REG = 0x80000000 | ((625 - 576 ) << 4);   //VT-V
  TCON_TV0_local2->BASIC_REG[0] = ((720 - 1) << 16) | (576 - 1); //H,V
  TCON_TV0_local2->BASIC_REG[1] = ((720 - 1) << 16) | (576 - 1);
  TCON_TV0_local2->BASIC_REG[2] = ((720 - 1) << 16) | (576 - 1);
  TCON_TV0_local2->BASIC_REG[3] = ((864 - 1) << 16) | ( 68 - 1); //HT, HBP
  TCON_TV0_local2->BASIC_REG[4] = ((625 * 2) << 16) | ( 39 - 1); //VT*2, VBP
  TCON_TV0_local2->BASIC_REG[5] = (64 << 16) | 5;                //HS, VS
 }

 TCON_TV0_local2->IO_POL_REG = 0;
 TCON_TV0_local2->IO_TRI_REG = 0x0FFFFFFF;

 TCON_TV0_local2->SRC_CTL_REG=0;             //0 - DE, 1..7 - test
 TCON_TV0_local2->GINT0_REG=(1<<30);         //enable Vblank int
 TCON_TV0_local2->GCTL_REG=(1UL<<31)|(1<<1); //enable TCONTV
}

void TV_VSync(void)
{
 TCON_TV0_local2->GINT0_REG&=~(1<<14);
 while(!(TCON_TV0_local2->GINT0_REG&(1<<14)));
}

void TVE_Init(uint32_t mode)
{
 tve_low_set_top_reg_base((void __iomem*)TVE_TOP_BASE);
 tve_low_set_reg_base(0,(void __iomem*)TVE_BASE);

 tve_low_init(0);

 tve_low_dac_autocheck_disable(0);
// tve_low_dac_autocheck_enable(0);

 tve_low_set_tv_mode(0,mode,0);

 tve_low_dac_enable(0);

 tve_low_open(0);

 tve_low_enhance(0,0); //0,1,2

 printf("TVE Open...\n");

// if(tve_low_get_dac_status(0))printf("DAC connected!\n");
// else                         printf("DAC NOT connected!\n");
}

void TCONTVandTVE_Init(unsigned int mode)
{
 TCONTV_Clock();
 DPSS_Clock();

 VIDEO1_PLL();
 TVE_Clock();

// Undocumented registers ---------------------------------------------------------------

 (*(volatile uint32_t*)(DISPLAY_TOP_BASE+0x00))&=~1;      //0 - CCU clock, 1 - TVE clock
 (*(volatile uint32_t*)(DISPLAY_TOP_BASE+0x20))|=(1<<20); //enable clk for TCON_TV0_local2

 uint32_t v=(*(volatile uint32_t*)(DISPLAY_TOP_BASE+0x1C));
 v&=0xFFFFFFF0;
 v|=0x00000002;
 (*(volatile uint32_t*)(DISPLAY_TOP_BASE+0x1C))=v;        //0 - DE to TCON_LCD, 2 - DE to TCON_TV

// --------------------------------------------------------------------------------------

 TCONTV_Init(mode);
 TVE_Init(mode);
}
