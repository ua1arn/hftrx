#if (CPUSTYLE_T113 || CPUSTYLE_F133)
#include <stdint.h>
#include <string.h>

//#include "reg-ccu.h"
#include "reg-de.h"
#include "reg-tconlcd.h"

//#include "io.h"
//
//#include "Lowlevel.h"

#include "fb-t113-rgb.h"
#include "../../g2d_driver.h"

static uint32_t read32(uintptr_t a)
{
	return * (volatile uint32_t *) a;
}

static void write32(uintptr_t a, uint32_t v)
{
	* (volatile uint32_t *) a = v;
}

struct fb_t113_rgb_pdata_t
{
	virtual_addr_t virt_de;
	virtual_addr_t virt_tconlcd;

	unsigned int clk_tconlcd;
	int rst_de;
	int rst_tconlcd;
	int width;
	int height;
	int bits_per_pixel;
	int bytes_per_pixel;
	int pixlen;
	int index;
	void * vram[2];
	struct region_list_t * nrl, * orl;

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

static void inline t113_de_enable(struct fb_t113_rgb_pdata_t * pdat)
{
	struct de_glb_t * glb = (struct de_glb_t *)(pdat->virt_de + T113_DE_MUX_GLB);
	write32((virtual_addr_t)&glb->dbuff, 1);
}

/*
static inline void t113_de_set_address(struct fb_t113_rgb_pdata_t * pdat, void * vram)
{
	struct de_ui_t * ui = (struct de_ui_t *)(pdat->virt_de + T113_DE_MUX_CHAN + 0x1000 * 1);
	write32((virtual_addr_t)&ui->cfg[0].top_laddr, (unsigned int)(unsigned long)vram);
}
*/

static inline void t113_de_set_mode(struct fb_t113_rgb_pdata_t * pdat)
{
	struct de_clk_t * clk = (struct de_clk_t *)(pdat->virt_de);
	struct de_glb_t * glb = (struct de_glb_t *)(pdat->virt_de + T113_DE_MUX_GLB);
	struct de_bld_t * bld = (struct de_bld_t *)(pdat->virt_de + T113_DE_MUX_BLD);

	struct de_vi_t * vi = (struct de_vi_t *)(pdat->virt_de + T113_DE_MUX_CHAN + 0x1000 * 0); //CH0 VI low  priority
	struct de_ui_t * ui = (struct de_ui_t *)(pdat->virt_de + T113_DE_MUX_CHAN + 0x1000 * 1); //CH1 UI high priority

	unsigned int size = (((pdat->height - 1) << 16) | (pdat->width - 1));
	unsigned int val;
	int i;

	val = read32((virtual_addr_t)&clk->rst_cfg);
	val |= 1 << 0;
	write32((virtual_addr_t)&clk->rst_cfg, val);

	val = read32((virtual_addr_t)&clk->gate_cfg);
	val |= 1 << 0;
	write32((virtual_addr_t)&clk->gate_cfg, val);

	val = read32((virtual_addr_t)&clk->bus_cfg);
	val |= 1 << 0;
	write32((virtual_addr_t)&clk->bus_cfg, val);

	val = read32((virtual_addr_t)&clk->sel_cfg);
	val &= ~(1 << 0);
	write32((virtual_addr_t)&clk->sel_cfg, val);

	write32((virtual_addr_t)&glb->ctl, (1 << 0));
	write32((virtual_addr_t)&glb->status, 0);
	write32((virtual_addr_t)&glb->dbuff, 1);
	write32((virtual_addr_t)&glb->size, size);

	for(i = 0; i < 4; i++)
	{
		void * chan = (void *)(pdat->virt_de + T113_DE_MUX_CHAN + 0x1000 * i);
		memset(chan, 0, i == 0 ? sizeof(struct de_vi_t) : sizeof(struct de_ui_t));
	}

	memset(bld,0,sizeof(struct de_bld_t));

	write32((virtual_addr_t)&bld->fcolor_ctl,0x00000303);              //enable pipe 0 & 1

//                                           pipe3   pipe2  pipe1  pipe0
	write32((virtual_addr_t)&bld->route,(3<<12)|(2<<8)|(1<<4)|(0<<0)); //ch0=>pipe0, ch1=>pipe1, ch2=>pipe2, ch3=>pipe3

	write32((virtual_addr_t)&bld->premultiply,0);
	write32((virtual_addr_t)&bld->bkcolor,0xff0000FF);

	write32((virtual_addr_t)&bld->bld_mode[0],0x03020302);
	write32((virtual_addr_t)&bld->bld_mode[1],0x03020302);             //Fs=Ad, Fd=1-As, Qs=Ad, Qd=1-As

	write32((virtual_addr_t)&bld->output_size, size);
	write32((virtual_addr_t)&bld->out_ctl, 0);
	write32((virtual_addr_t)&bld->ck_ctl, 0);
	for(i = 0; i < 4; i++)
	{
		write32((virtual_addr_t)&bld->attr[i].fcolor, 0xff000000);
		write32((virtual_addr_t)&bld->attr[i].insize, size);
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

//CH0 VI ----------------------------------------------------------------------------

	write32((virtual_addr_t)&vi->cfg[0].attr,(1<<0)|(DE2_FORMAT_ABGR_8888<<8)|(1<<15));            //������ ����: 32 bit ABGR 8:8:8:8 ��� ���������� �����
	write32((virtual_addr_t)&vi->cfg[0].size, size);
	write32((virtual_addr_t)&vi->cfg[0].coord, 0);
        write32((virtual_addr_t)&vi->cfg[0].pitch[0], BYTE_PER_PIXEL * pdat->width);
	write32((virtual_addr_t)&vi->cfg[0].top_laddr[0],VIDEO_MEMORY0);                               //VIDEO_MEMORY0
	write32((virtual_addr_t)&vi->ovl_size[0], size);

//CH1 UI -----------------------------------------------------------------------------

	write32((virtual_addr_t)&ui->cfg[0].attr,(1<<0)|(DE2_FORMAT_ABGR_8888<<8)|(0xff<<24)|(1<<16)); //������� ����: 32 bit ABGR 8:8:8:8 � ���������� ������
	write32((virtual_addr_t)&ui->cfg[0].size, size);
	write32((virtual_addr_t)&ui->cfg[0].coord, 0);
	write32((virtual_addr_t)&ui->cfg[0].pitch, BYTE_PER_PIXEL * pdat->width);
	write32((virtual_addr_t)&ui->cfg[0].top_laddr,VIDEO_MEMORY1);                                  //VIDEO_MEMORY1
	write32((virtual_addr_t)&ui->ovl_size, size);
}

static void t113_tconlcd_enable(struct fb_t113_rgb_pdata_t * pdat)
{
	struct t113_tconlcd_reg_t * tcon = (struct t113_tconlcd_reg_t *)pdat->virt_tconlcd;
	unsigned int val;

	val = read32((virtual_addr_t)&tcon->gctrl);
	val |= (1 << 31);
	write32((virtual_addr_t)&tcon->gctrl, val);
}

static void t113_tconlcd_disable(struct fb_t113_rgb_pdata_t * pdat)
{
	struct t113_tconlcd_reg_t * tcon = (struct t113_tconlcd_reg_t *)pdat->virt_tconlcd;
	unsigned int val;
	val = read32((virtual_addr_t)&tcon->dclk);
	val &= ~(0xf << 28);
	write32((virtual_addr_t)&tcon->dclk, val);

	write32((virtual_addr_t)&tcon->gctrl, 0);
	write32((virtual_addr_t)&tcon->gint0, 0);
}

static void t113_tconlcd_set_timing(struct fb_t113_rgb_pdata_t * pdat)
{
	struct t113_tconlcd_reg_t * tcon = (struct t113_tconlcd_reg_t *)pdat->virt_tconlcd;
	int bp, total;
	unsigned int val;

	val = (pdat->timing.v_front_porch + pdat->timing.v_back_porch + pdat->timing.v_sync_len) / 2;
	write32((virtual_addr_t)&tcon->ctrl, (1 << 31) | (0 << 24) | (0 << 23) | ((val & 0x1f) << 4) | (0 << 0));
	val = pdat->clk_tconlcd / pdat->timing.pixel_clock_hz;
	write32((virtual_addr_t)&tcon->dclk, (0xf << 28) | ((val /* /2 */ ) << 0));                     //������ �� 2, ������� ������ �� ���������� � 2 ���� ��������
	write32((virtual_addr_t)&tcon->timing0, ((pdat->width - 1) << 16) | ((pdat->height - 1) << 0));
	bp = pdat->timing.h_sync_len + pdat->timing.h_back_porch;
	total = pdat->width + pdat->timing.h_front_porch + bp;
	write32((virtual_addr_t)&tcon->timing1, ((total - 1) << 16) | ((bp - 1) << 0));
	bp = pdat->timing.v_sync_len + pdat->timing.v_back_porch;
	total = pdat->height + pdat->timing.v_front_porch + bp;
	write32((virtual_addr_t)&tcon->timing2, ((total * 2) << 16) | ((bp - 1) << 0));
	write32((virtual_addr_t)&tcon->timing3, ((pdat->timing.h_sync_len - 1) << 16) | ((pdat->timing.v_sync_len - 1) << 0));

	val = (0 << 31) | (1 << 28);
	if(!pdat->timing.h_sync_active)
		val |= (1 << 25);
	if(!pdat->timing.v_sync_active)
		val |= (1 << 24);
	if(!pdat->timing.den_active)
		val |= (1 << 27);
	if(!pdat->timing.clk_active)
		val |= (1 << 26);
	write32((virtual_addr_t)&tcon->io_polarity, val);
	write32((virtual_addr_t)&tcon->io_tristate, 0);
}

static void t113_tconlcd_set_dither(struct fb_t113_rgb_pdata_t * pdat)
{
	struct t113_tconlcd_reg_t * tcon = (struct t113_tconlcd_reg_t *)pdat->virt_tconlcd;

	if((pdat->bits_per_pixel == 16) || (pdat->bits_per_pixel == 18))
	{
		write32((virtual_addr_t)&tcon->frm_seed[0], 0x11111111);
		write32((virtual_addr_t)&tcon->frm_seed[1], 0x11111111);
		write32((virtual_addr_t)&tcon->frm_seed[2], 0x11111111);
		write32((virtual_addr_t)&tcon->frm_seed[3], 0x11111111);
		write32((virtual_addr_t)&tcon->frm_seed[4], 0x11111111);
		write32((virtual_addr_t)&tcon->frm_seed[5], 0x11111111);
		write32((virtual_addr_t)&tcon->frm_table[0], 0x01010000);
		write32((virtual_addr_t)&tcon->frm_table[1], 0x15151111);
		write32((virtual_addr_t)&tcon->frm_table[2], 0x57575555);
		write32((virtual_addr_t)&tcon->frm_table[3], 0x7f7f7777);

		if(pdat->bits_per_pixel == 16)
			write32((virtual_addr_t)&tcon->frm_ctrl, (1 << 31) | (1 << 6) | (0 << 5)| (1 << 4));
		else if(pdat->bits_per_pixel == 18)
			write32((virtual_addr_t)&tcon->frm_ctrl, (1 << 31) | (0 << 6) | (0 << 5)| (0 << 4));
	}
}

static void fb_t113_rgb_init(struct fb_t113_rgb_pdata_t *pdat)
{
//GPIO ���������������� � PIO.c

/*
	GPIO_InitTypeDef InitGpio;
	InitGpio.GPIO_Mode = GPIO_Mode_AF2;
	InitGpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	InitGpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
			| GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8
			| GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13
			| GPIO_Pin_14 | GPIO_Pin_15 | GPIO_Pin_16 | GPIO_Pin_17
			| GPIO_Pin_18 | GPIO_Pin_19; //|GPIO_Pin_20|GPIO_Pin_21;
	GPIO_Init(GPIOD, &InitGpio);

	//MODE
	InitGpio.GPIO_Mode = GPIO_Mode_OUT;
	InitGpio.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &InitGpio);
	GPIO_SetBits(GPIOB, GPIO_Pin_7);
*/

//	t113_tconlcd_disable(pdat);
//	t113_tconlcd_set_timing(pdat);
//	t113_tconlcd_set_dither(pdat);
//	t113_tconlcd_enable(pdat);
	t113_de_set_mode(pdat);
	t113_de_enable(pdat);
	//t113_de_set_address(pdat, pdat->vram[pdat->index]);
	t113_de_enable(pdat);
}

#if 0
void lcd_backlight_init(void){
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

void tol_init(void){
	GPIO_InitTypeDef InitGpio;
	InitGpio.GPIO_Mode = GPIO_Mode_OUT;
	InitGpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	InitGpio.GPIO_Pin = GPIO_Pin_22|GPIO_Pin_0;
	GPIO_Init(GPIOD, &InitGpio);

	GPIO_ResetBits(GPIOD,GPIO_Pin_22);
	GPIO_SetBits(GPIOD,GPIO_Pin_0);
}
#endif
#endif
