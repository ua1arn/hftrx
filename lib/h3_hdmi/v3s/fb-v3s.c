/*
 * driver/fb-v3s.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <812542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "main.h"
#include "reg-de.h"


struct fb_v3s_pdata_t
{
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


	struct led_t * backlight;
	int brightness;
};





 void  v3s_de_enable(void)
{
	struct de_glb_t * glb = (struct de_glb_t *)(V3S_DE_BASE + V3S_DE_MUX_GLB);
	write32((virtual_addr_t)&glb->dbuff, 1);
}

void v3s_de_set_address(void)
{
	//struct de_ui_t * ui = (struct de_ui_t *)(V3S_DE_BASE + V3S_DE_MUX_CHAN + 0x1000 * 2);
	struct de_ui_t * vi = (struct de_ui_t *)(V3S_DE_BASE + V3S_DE_MUX_CHAN + 0x1000 );
	//write32((virtual_addr_t)&ui->cfg[0].top_laddr, (u32_t)lcdframebuffer2);
    write32((virtual_addr_t)&vi->cfg[0].top_laddr, (u32_t)lcdframebuffer2);

}

#include <string.h>

void v3s_de_set_mode(void)
{
	struct de_clk_t * clk = (struct de_clk_t *)(V3S_DE_BASE);
	struct de_glb_t * glb = (struct de_glb_t *)(V3S_DE_BASE + V3S_DE_MUX_GLB);
	struct de_bld_t * bld = (struct de_bld_t *)(V3S_DE_BASE + V3S_DE_MUX_BLD);
	struct de_ui_t * ui = (struct de_ui_t *)(V3S_DE_BASE + V3S_DE_MUX_CHAN + 0x1000 * 2);

	u32_t size = (((LCDY - 1) << 16) | (LCDX - 1));

	u32_t val;
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
		void * chan = (void *)(V3S_DE_BASE + V3S_DE_MUX_CHAN + 0x1000 * i);
		memset(chan, 0, i == 0 ? sizeof(struct de_vi_t) : sizeof(struct de_ui_t));
	}

	memset(bld, 0, sizeof(struct de_bld_t));


	///write32((virtual_addr_t)&bld->fcolor_ctl, 0x00000101);
	write32((virtual_addr_t)&bld->fcolor_ctl,1<<8|1<<0);///*1<<9|1<<1/*|*/ 0x00000101);p1,p0
	write32((virtual_addr_t)&bld->route,2<<0); ///*2<<4/*|*/ 0-V 2-u 3ch->p1,2ch->p0
	write32((virtual_addr_t)&bld->premultiply, 0);
	write32((virtual_addr_t)&bld->bkcolor, 0xff000000);
	write32((virtual_addr_t)&bld->bld_mode[0], 0x03010301);
	write32((virtual_addr_t)&bld->bld_mode[1], 0x03010301);
	write32((virtual_addr_t)&bld->bld_mode[2], 0x03010301);
	write32((virtual_addr_t)&bld->bld_mode[3], 0x03010301);
	write32((virtual_addr_t)&bld->output_size, size);
	write32((virtual_addr_t)&bld->out_ctl, 0);
	write32((virtual_addr_t)&bld->ck_ctl, 0);

	for(i = 0; i < 4; i++)
	{
		write32((virtual_addr_t)&bld->attr[i].insize, size);

	write32((virtual_addr_t)&ui->cfg[i].size, size);
	write32((virtual_addr_t)&ui->cfg[i].coord, 0);
    write32((virtual_addr_t)&ui->cfg[i].pitch, 2 * LCDX);

	}
	///-------------p0-----------
	write32((virtual_addr_t)&bld->attr[0].fcolor, 0xff000000);
    write32((virtual_addr_t)&ui->cfg[0].attr,(1 << 0)  |(10 << 8) | (1 << 1) | (0xff << 24));/*10<<8-RGB_565*/
	write32((virtual_addr_t)&ui->cfg[0].top_laddr, (u32_t)lcdframebuffer1);
	///-------------p1-----------activ---
    write32((virtual_addr_t)&bld->attr[1].fcolor, 0xff000000);
    write32((virtual_addr_t)&ui->cfg[1].attr,(1 << 0)  |(10 << 8) | (1 << 1) | (0xff << 24));/*10<<8-RGB_565*/
    write32((virtual_addr_t)&ui->cfg[1].top_laddr, (u32_t)lcdframebuffer1);

	write32(V3S_DE_BASE + V3S_DE_MUX_VSU, 0);
	write32(V3S_DE_BASE + V3S_DE_MUX_GSU1, 0);
	write32(V3S_DE_BASE + V3S_DE_MUX_GSU2, 0);
	write32(V3S_DE_BASE + V3S_DE_MUX_GSU3, 0);
	write32(V3S_DE_BASE + V3S_DE_MUX_FCE, 0);
	write32(V3S_DE_BASE + V3S_DE_MUX_BWS, 0);
	write32(V3S_DE_BASE + V3S_DE_MUX_LTI, 0);
	write32(V3S_DE_BASE + V3S_DE_MUX_PEAK, 0);
	write32(V3S_DE_BASE + V3S_DE_MUX_ASE, 0);
	write32(V3S_DE_BASE + V3S_DE_MUX_FCC, 0);
	write32(V3S_DE_BASE + V3S_DE_MUX_DCSC, 0);


	///write32((virtual_addr_t)&ui->cfg[0].bot_laddr, (u32_t)framebuffer1);
///	write32((virtual_addr_t)&ui->top_haddr, (u32_t)framebuffer1>>29);
	write32((virtual_addr_t)&ui->cfg[0].top_laddr, (u32_t)lcdframebuffer2);
	///write32((virtual_addr_t)&ui->cfg[0].bot_laddr, (u32_t)framebuffer2);
	///write32(V3S_DE_BASE+V3S_DE_MUX_GSU1+UI_TOP_LADDR,(u32_t)framebuffer1);
	write32((virtual_addr_t)&ui->ovl_size, size);
	///write32(V3S_DE_BASE+V3S_DE_MUX_GSU1+UI_OVL_SIZE,size);
	///sprintf(buff,"%d\n",read32(&ui->cfg[0].attr));
    ///memcpy(framebuffer1,framebuffer2,pdat->pixlen);
	///dma_cache_sync(framebuffer1, pdat->pixlen, DMA_TO_DEVICE);
    struct de_vi_t * vi = (struct de_vi_t *)(V3S_DE_BASE + V3S_DE_MUX_CHAN );
    write32((virtual_addr_t)&vi->cfg[0].attr, (1 << 0) | (10 << 8) | (1 << 1) | (0xff << 24));/*10<<8-RGB_565*/
	write32((virtual_addr_t)&vi->cfg[0].size, size);
	write32((virtual_addr_t)&vi->cfg[0].coord, 0);
	write32((virtual_addr_t)&vi->cfg[0].pitch[0], 2 * LCDX);
	///write32((virtual_addr_t)&vi->cfg[0].pitch[1], 2 * LCDX);
	///write32((virtual_addr_t)&vi->cfg[0].pitch[2], 2 * LCDX);
	write32((virtual_addr_t)&vi->cfg[0].top_laddr[0], (u32_t)lcdframebuffer2);
	///write32((virtual_addr_t)&vi->cfg[0].top_laddr[1], (u32_t)framebuffer2);
	///write32((virtual_addr_t)&vi->cfg[0].top_laddr[2], (u32_t)framebuffer3);

	//write32((virtual_addr_t)&vi->top_haddr, (u32_t)framebuffer1>>29);
    write32((virtual_addr_t)&vi->ovl_size[0], size);

   // write32((virtual_addr_t)&vi->hori[1],1<<16|1<<0);
   // write32((virtual_addr_t)&vi->vert[1], 1<<16|1<<0);

}



