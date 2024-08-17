#include "hardware.h"
#include "formats.h"

#include <stdint.h>
#include <string.h>

//#include "reg-ccu.h"
#include "reg-de.h"
#include "reg-tconlcd.h"

#include "fb-t113-rgb.h"

#ifdef TVE_MODE

#include "../TVE/tve.h"

#endif

//#include "io.h"
//
//#include "Lowlevel.h"

struct fb_t113_rgb_pdata_t
{
	uintptr_t virt_de;
	uintptr_t virt_tconlcd;

	unsigned int clk_tconlcd;
	int rst_de;
	int rst_tconlcd;
	int width;
	int height;
	int bits_per_pixel;
	int bytes_per_pixel;
	int pixlen;

	struct {
		int pixel_clock_hz;
		int h_front_porch;
		int h_back_porch;
		int h_sync_len;
		int v_front_porch;
		int v_back_porch;
		int v_sync_len;
		int h_sync_active;
		int v_sync_active;
		int den_active;
		int clk_active;
	} timing;
};

static struct fb_t113_rgb_pdata_t pdat;


static uint32_t read32(uintptr_t addr)
{
	return * (volatile uint32_t *) addr;
}

static void write32(uintptr_t addr, uint32_t value)
{
	* (volatile uint32_t *) addr = value;
}

static void t113_tvout_de_enable(struct fb_t113_rgb_pdata_t * pdat)
{
	struct de_glb_t * glb = (struct de_glb_t *)(pdat->virt_de + T113_DE_MUX_GLB);
	write32((uintptr_t)&glb->dbuff, 1);
}

static void t113_tvout_de_set_address(struct fb_t113_rgb_pdata_t * pdat, void * vram, uint8_t plane)
{
 if(plane)
 {
  struct de_ui_t * ui = (struct de_ui_t *)(pdat->virt_de + T113_DE_MUX_CHAN + 0x1000 * 1); //CH1 UI high priority
  write32((uintptr_t)&ui->cfg[0].top_laddr,(uint32_t)vram);                                //ARGB
 }
 else
 {
  struct de_vi_t * vi = (struct de_vi_t *)(pdat->virt_de + T113_DE_MUX_CHAN + 0x1000 * 0); //CH0 VI low priority
  write32((uintptr_t)&vi->cfg[0].top_laddr[0], (uint32_t)vram                        );    //Y
  write32((uintptr_t)&vi->cfg[0].top_laddr[1],((uint32_t)vram)+(TVD_WIDTH*TVD_HEIGHT));    //UV
 }
}

static void t113_tvout_de_set_mode(struct fb_t113_rgb_pdata_t * pdat)
{
	struct de_clk_t * clk = (struct de_clk_t *)(pdat->virt_de);
	struct de_glb_t * glb = (struct de_glb_t *)(pdat->virt_de + T113_DE_MUX_GLB);
	struct de_bld_t * bld = (struct de_bld_t *)(pdat->virt_de + T113_DE_MUX_BLD);

        struct de_csc_t * csc = (struct de_csc_t *)(pdat->virt_de + T113_DE_MUX_DCSC);

	struct de_vi_t * vi = (struct de_vi_t *)(pdat->virt_de + T113_DE_MUX_CHAN + 0x1000 * 0); //CH0 VI low  priority
	struct de_ui_t * ui = (struct de_ui_t *)(pdat->virt_de + T113_DE_MUX_CHAN + 0x1000 * 1); //CH1 UI high priority

	unsigned int size = ((( TVD_HEIGHT /*pdat->height*/ - 1) << 16) | (pdat->width - 1));
	unsigned int val;
	int i;

	val = read32((uintptr_t)&clk->rst_cfg);
	val |= 1 << 0;
	write32((uintptr_t)&clk->rst_cfg, val);

	val = read32((uintptr_t)&clk->gate_cfg);
	val |= 1 << 0;
	write32((uintptr_t)&clk->gate_cfg, val);

	val = read32((uintptr_t)&clk->bus_cfg);
	val |= 1 << 0;
	write32((uintptr_t)&clk->bus_cfg, val);

	val = read32((uintptr_t)&clk->sel_cfg);
	val &= ~(1 << 0);
	write32((uintptr_t)&clk->sel_cfg, val);

	write32((uintptr_t)&glb->ctl, (1 << 0));
	write32((uintptr_t)&glb->status, 0);
	write32((uintptr_t)&glb->dbuff, 1);
	write32((uintptr_t)&glb->size, size);

	for(i = 0; i < 4; i++)
	{
		void * chan = (void *)(pdat->virt_de + T113_DE_MUX_CHAN + 0x1000 * i);
		memset(chan, 0, i == 0 ? sizeof(struct de_vi_t) : sizeof(struct de_ui_t));
	}

	memset(bld,0,sizeof(struct de_bld_t));

	write32((uintptr_t)&bld->fcolor_ctl,0x00000303);              //enable pipe 0 & 1
	write32((uintptr_t)&bld->fcolor_ctl,0x00000101);              //enable pipe mgs

//                                           pipe3   pipe2  pipe1  pipe0
	write32((uintptr_t)&bld->route,(3<<12)|(2<<8)|(1<<4)|(0<<0)); //ch0=>pipe0, ch1=>pipe1, ch2=>pipe2, ch3=>pipe3

	write32((uintptr_t)&bld->premultiply,0);
	write32((uintptr_t)&bld->bkcolor,0xff0000FF);
	write32((uintptr_t)&bld->bkcolor,0xffFFFF00);	// mgs

	uint32_t bldmode = 0x03010301;	// default (mgs)
	//uint32_t bldmode = 0x03020302;
	write32((uintptr_t)&bld->bld_mode[0],bldmode);
	write32((uintptr_t)&bld->bld_mode[1],bldmode);             //Fs=Ad, Fd=1-As, Qs=Ad, Qd=1-As

	write32((uintptr_t)&bld->output_size, size);
	write32((uintptr_t)&bld->out_ctl, 0);
	write32((uintptr_t)&bld->ck_ctl, 0);

	for(i = 0; i < 4; i++)
	{
		write32((uintptr_t)&bld->attr[i].fcolor, 0xff000000);
		write32((uintptr_t)&bld->attr[i].insize, size);
	}

	write32(pdat->virt_de + T113_DE_MUX_VSU, 0);
	write32(pdat->virt_de + T113_DE_MUX_GSU1, 0);
	write32(pdat->virt_de + T113_DE_MUX_GSU2, 0);
	write32(pdat->virt_de + T113_DE_MUX_GSU3, 0);
	write32(pdat->virt_de + T113_DE_MUX_FCE, 0);
	write32(pdat->virt_de + T113_DE_MUX_BWS, 0);
	write32(pdat->virt_de + T113_DE_MUX_LTI, 0);
	write32(pdat->virt_de + T113_DE_MUX_PEAK, 0);
	write32(pdat->virt_de + T113_DE_MUX_ASE, 0);
	write32(pdat->virt_de + T113_DE_MUX_FCC, 0);
	write32(pdat->virt_de + T113_DE_MUX_DCSC, 0);

//https://elixir.bootlin.com/linux/latest/source/drivers/gpu/drm/sun4i/sun8i_csc.c
#ifndef TVE_MODE

#if is_composite
		/* set CSC coefficients */
		write32((uintptr_t)&csc->coef11,0x00000400 );
		write32((uintptr_t)&csc->coef12,0x00000000 );
		write32((uintptr_t)&csc->coef13,0x0000059B );
		write32((uintptr_t)&csc->coef14,0xFFFD322E );

		write32((uintptr_t)&csc->coef21,0x00000400 );
		write32((uintptr_t)&csc->coef22,0xFFFFFEA0 );
		write32((uintptr_t)&csc->coef23,0xFFFFFD25 );
		write32((uintptr_t)&csc->coef24,0x00021DD5 );

		write32((uintptr_t)&csc->coef31,0x00000400 );
		write32((uintptr_t)&csc->coef32,0x00000716 );
		write32((uintptr_t)&csc->coef33,0x00000000 );
		write32((uintptr_t)&csc->coef34,0xFFFC74BD );

		/* alpha for modes with alpha */
		write32((uintptr_t)&csc->globalpha,0xff << 24 );

		/* enable CSC unit */
		write32((uintptr_t)&csc->csc_ctl,1);
#else
		write32((uintptr_t)&csc->csc_ctl,0);
#endif

#endif

//https://elixir.bootlin.com/linux/latest/source/drivers/gpu/drm/sun4i/sun8i_vi_scaler.c

#if is_composite

 uint32_t src_w=TVD_WIDTH;
 uint32_t src_h=TVD_HEIGHT;

 uint32_t dst_w=LCD_PIXEL_WIDTH;
 uint32_t dst_h=LCD_PIXEL_HEIGHT;

 uint32_t hscale=(src_w<<16)/dst_w;
 uint32_t vscale=(src_h<<16)/dst_h;

 sun8i_vi_scaler_setup(src_w,src_h,dst_w,dst_h,hscale,vscale,0,0);
 sun8i_vi_scaler_enable(1);

#endif

//CH0 VI ----------------------------------------------------------------------------

	write32((uintptr_t)&vi->cfg[0].attr,(1<<0)|((is_composite?DE2_FORMAT_YUV420_V1U1V0U0:DE2_FORMAT)<<8)|((is_composite^1)<<15)); //VI
	//write32((uintptr_t)&vi->cfg[0].attr,0); //VI mgs
	write32((uintptr_t)&vi->cfg[0].size, size);
	write32((uintptr_t)&vi->cfg[0].coord, 0);

        write32((uintptr_t)&vi->cfg[0].pitch[0],pdat->width); //Y
        write32((uintptr_t)&vi->cfg[0].pitch[1],pdat->width); //UV

//	write32((uintptr_t)&vi->cfg[0].top_laddr[0],VIDEO_MEMORY0                       ); //Y
//	write32((uintptr_t)&vi->cfg[0].top_laddr[1],VIDEO_MEMORY0+(TVD_WIDTH*TVD_HEIGHT)); //UV

	write32((uintptr_t)&vi->ovl_size[0],size); //Y
	write32((uintptr_t)&vi->ovl_size[1],size); //UV
	write32((uintptr_t)&vi->fcolor [0], 0xFFFF0000);

/*
	write32((uintptr_t)&vi->hori[0], (dst_w<<16)|src_w); //Y
	write32((uintptr_t)&vi->hori[1], (dst_w<<16)|src_w); //UV

	write32((uintptr_t)&vi->vert[0], (dst_h<<16)|src_h); //Y
	write32((uintptr_t)&vi->vert[1], (dst_h<<16)|src_h); //UV
*/

//CH1 UI -----------------------------------------------------------------------------

	write32((uintptr_t)&ui->cfg[0].attr,(1<<0)|(DE2_FORMAT<<8)|(0xff<<24)|(UINT32_C(1) << 16));           //������� ���� � ���������� ������
	//write32((uintptr_t)&ui->cfg[0].attr, 0);
	write32((uintptr_t)&ui->cfg[0].size, size);
	write32((uintptr_t)&ui->cfg[0].coord, 0);
	write32((uintptr_t)&ui->cfg[0].pitch, BYTE_PER_PIXEL * pdat->width);
//	write32((uintptr_t)&ui->cfg[0].top_laddr,VIDEO_MEMORY1);                                  //VIDEO_MEMORY1
	write32((uintptr_t)&ui->ovl_size, size);
}

static void t113_tconlcd2_enable(struct fb_t113_rgb_pdata_t * pdat)
{
	struct t113_tconlcd_reg_t * tcon = (struct t113_tconlcd_reg_t *)pdat->virt_tconlcd;
	unsigned int val;

	val = read32((uintptr_t)&tcon->gctrl);
	val |= (1 << 31);
	write32((uintptr_t)&tcon->gctrl, val);
}

static void t113_tconlcd2_disable(struct fb_t113_rgb_pdata_t * pdat)
{
	struct t113_tconlcd_reg_t * tcon = (struct t113_tconlcd_reg_t *)pdat->virt_tconlcd;
	unsigned int val;
	val = read32((uintptr_t)&tcon->dclk);
	val &= ~(0xf << 28);
	write32((uintptr_t)&tcon->dclk, val);

	write32((uintptr_t)&tcon->gctrl, 0);
	write32((uintptr_t)&tcon->gint0, 0);
}

static void t113_tconlcd2_set_timing(struct fb_t113_rgb_pdata_t * pdat)
{
	struct t113_tconlcd_reg_t * tcon = (struct t113_tconlcd_reg_t *)pdat->virt_tconlcd;
	int bp, total;
	unsigned int val;

	val = (pdat->timing.v_front_porch + pdat->timing.v_back_porch + pdat->timing.v_sync_len) / 2;
	write32((uintptr_t)&tcon->ctrl, (1 << 31) | (0 << 24) | (0 << 23) | ((val & 0x1f) << 4) | (0 << 0) );

	val = pdat->clk_tconlcd / pdat->timing.pixel_clock_hz;

	write32((uintptr_t)&tcon->dclk, (0xf << 28) | ((val /* /2 */ ) << 0));                     //������ �� 2, ������� ������ �� ���������� � 2 ���� ��������

	write32((uintptr_t)&tcon->timing0, ((pdat->width - 1) << 16) | ((pdat->height - 1) << 0));

	bp = pdat->timing.h_sync_len + pdat->timing.h_back_porch;
	total = pdat->width + pdat->timing.h_front_porch + bp;
	write32((uintptr_t)&tcon->timing1, ((total - 1) << 16) | ((bp - 1) << 0));

	bp = pdat->timing.v_sync_len + pdat->timing.v_back_porch;
	total = pdat->height + pdat->timing.v_front_porch + bp;
	write32((uintptr_t)&tcon->timing2, ((total * 2) << 16) | ((bp - 1) << 0));

	write32((uintptr_t)&tcon->timing3, ((pdat->timing.h_sync_len - 1) << 16) | ((pdat->timing.v_sync_len - 1) << 0));

	val = (0 << 31) | (1 << 28);
	if(!pdat->timing.h_sync_active)
		val |= (1 << 25);
	if(!pdat->timing.v_sync_active)
		val |= (1 << 24);
	if(!pdat->timing.den_active)
		val |= (1 << 27);
	if(!pdat->timing.clk_active)
		val |= (1 << 26);
	write32((uintptr_t)&tcon->io_polarity, val);
	write32((uintptr_t)&tcon->io_tristate, 0);
}

static void t113_tconlcd2_set_dither(struct fb_t113_rgb_pdata_t * pdat)
{
	struct t113_tconlcd_reg_t * tcon = (struct t113_tconlcd_reg_t *)pdat->virt_tconlcd;

	if((pdat->bits_per_pixel == 16) || (pdat->bits_per_pixel == 18))
	{
		write32((uintptr_t)&tcon->frm_seed[0], 0x11111111);
		write32((uintptr_t)&tcon->frm_seed[1], 0x11111111);
		write32((uintptr_t)&tcon->frm_seed[2], 0x11111111);
		write32((uintptr_t)&tcon->frm_seed[3], 0x11111111);
		write32((uintptr_t)&tcon->frm_seed[4], 0x11111111);
		write32((uintptr_t)&tcon->frm_seed[5], 0x11111111);
		write32((uintptr_t)&tcon->frm_table[0], 0x01010000);
		write32((uintptr_t)&tcon->frm_table[1], 0x15151111);
		write32((uintptr_t)&tcon->frm_table[2], 0x57575555);
		write32((uintptr_t)&tcon->frm_table[3], 0x7f7f7777);

		if(pdat->bits_per_pixel == 16)
			write32((uintptr_t)&tcon->frm_ctrl, (1 << 31) | (1 << 6) | (0 << 5)| (1 << 4));
		else if(pdat->bits_per_pixel == 18)
			write32((uintptr_t)&tcon->frm_ctrl, (1 << 31) | (0 << 6) | (0 << 5)| (0 << 4));
	}
}

static void fb_t113_rgb_init(struct fb_t113_rgb_pdata_t *pdat)
{
//GPIO ���������������� � PIO.c

#ifdef TVE_MODE
	PRINTF("fb_t113_rgb_init for mode PAL\n");
    //TCONTVandTVE_Init(DISP_TV_MOD_NTSC);
    TCONTVandTVE_Init(DISP_TV_MOD_PAL);

#else
	t113_tconlcd2_disable(pdat);
	t113_tconlcd2_set_timing(pdat);
	t113_tconlcd2_set_dither(pdat);
	t113_tconlcd2_enable(pdat);
#endif

	t113_tvout_de_set_mode(pdat);
	t113_tvout_de_enable(pdat);
}

#if 0
static void lcd_backlight_init(void){
	GPIO_InitTypeDef InitGpio;
	InitGpio.GPIO_Mode = GPIO_Mode_AF5;
	InitGpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	InitGpio.GPIO_Pin = GPIO_Pin_22;
	GPIO_Init(GPIOD, &InitGpio);

	PWM_InitTypeDef PWM_InitStruct;
	PWM_InitStruct.chennal 		= PWM_Chennel_7;
	PWM_InitStruct.clock		= PWM_HOSC;
	PWM_InitStruct.clock_div	= PWM_DIV_8;
	PWM_InitStruct.prescal_k	= 20-1;
	PWM_InitStruct.active_state = PWM_High;
	PWM_InitStruct.period 		= 200-1;
	PWM_InitStruct.duty		= 50;

	pwm_init(&PWM_InitStruct);


	pwm_set_duty(7,100);
}

void lcd_set_pwm(unsigned char pwm){
	volatile unsigned int addr;
	unsigned int val=0;
	addr = (0x02000c00+0x0104+7*0x20);
	write32(addr, ((pwm<<0)|(100<<16)));			//(PWM_ACT_CYCLE<<0)|(PWM_ENTIRE_CYCLE<<16)
}
#endif

static void TCONLCD_Clock(void)
{
 CCU->TCONLCD_BGR_REG&=~(UINT32_C(1) << 16);                      //assert reset TCON_LCD
 CCU->TCONLCD_CLK_REG=(UINT32_C(1) << 31)|(1<<24)|(1<<8)|(2-1); //clock on, PLL_VIDEO0(4x), N=2, M=2 => 1188/2/2 = 297 MHz
 CCU->TCONLCD_BGR_REG|=1;                             //gate pass TCON_LCD
 CCU->TCONLCD_BGR_REG|=(UINT32_C(1) << 16);                       //de-assert reset TCON_LCD
}

void lcd_init4(void)
{
 TCONLCD_Clock();

	unsigned int val;

	//val = read32(T113_CCU_BASE + CCU_DE_CLK_REG);
	val = CCU->DE_CLK_REG;
	val |= (1 << 31) | (3 << 0);					//DE CLK: 300 MHz
	//write32((T113_CCU_BASE + CCU_DE_CLK_REG), val);
	CCU->DE_CLK_REG = val;
	//Open the clock gate
	//val = read32(T113_CCU_BASE + CCU_DE_BGR_REG);
	val = CCU->DE_BGR_REG;
	val |= (1 << 0);
	//write32((T113_CCU_BASE + CCU_DE_BGR_REG), val);
	CCU->DE_BGR_REG = val;
	//de-assert reset
	//val = read32(T113_CCU_BASE + CCU_DE_BGR_REG);
	val = CCU->DE_BGR_REG;
	val |= (1 << 16);
	//write32((T113_CCU_BASE + CCU_DE_BGR_REG), val);
	CCU->DE_BGR_REG = val;

/*
	val = read32(T113_CCU_BASE + CCU_TCONLCD_CLK_REG);
	val |= (1 << 31) | (1 << 24);					//TCON CLK: 24*99/2=1188 MHz
	write32((T113_CCU_BASE + CCU_TCONLCD_CLK_REG), val);
	//Open the clock gate
	val = read32(T113_CCU_BASE + CCU_TCONLCD_BGR_REG);
	val |= (1 << 0);
	write32((T113_CCU_BASE + CCU_TCONLCD_BGR_REG), val);
	//eassert reset
	val = read32(T113_CCU_BASE + CCU_TCONLCD_BGR_REG);
	val |= (1 << 16);
	write32((T113_CCU_BASE + CCU_TCONLCD_BGR_REG), val);
*/

	pdat.virt_tconlcd = T113_TCONLCD_BASE;
	pdat.virt_de = T113_DE_BASE;
	pdat.clk_tconlcd = 297000000; //1188000000;
	pdat.width = LCD_PIXEL_WIDTH;
	pdat.height = LCD_PIXEL_HEIGHT;
	pdat.bits_per_pixel  = BYTE_PER_PIXEL*8;
	pdat.bytes_per_pixel = BYTE_PER_PIXEL;
	pdat.pixlen = pdat.width * pdat.height * pdat.bytes_per_pixel;

	pdat.timing.pixel_clock_hz = 29232000; //50000000; // 29232000/(800+40+87+1)/(480+13+31+1) = 60 FPS
	pdat.timing.h_front_porch  = 40;       //160;
	pdat.timing.h_back_porch   = 87;       //140;
	pdat.timing.h_sync_len     = 1;        //20;
	pdat.timing.v_front_porch  = 13;       //13;
	pdat.timing.v_back_porch   = 31;       //31;
	pdat.timing.v_sync_len     = 1;        //2;
	pdat.timing.h_sync_active  = 0;
	pdat.timing.v_sync_active  = 0;
	pdat.timing.den_active     = 1;	       //!!!
	pdat.timing.clk_active     = 0;

	fb_t113_rgb_init(&pdat);

	//lcd_backlight_init();
}

void LCD_SwitchAddress(uint32_t address,uint8_t plane)
{
 t113_tvout_de_set_address(&pdat,(void*)address,plane);
 t113_tvout_de_enable(&pdat);
}
