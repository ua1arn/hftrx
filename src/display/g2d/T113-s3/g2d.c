#if (CPUSTYLE_T113 || CPUSTYLE_F133)
#include "hardware.h"
#include "formats.h"

#include "g2d.h"

#include "Gate.h"
//#include "delay.h"

#include "g2d_regs_v2.h"

int g2d_ext_hd_finish_flag=0;

void G2D_Clk(void)
{
 G2D_BGR_REG&=~(1<<16);               //assert Reset

 G2D_CLK_REG=(1UL<<31)|(1<<24)|(3-1); //enable clk, PLLVIDEO0(4x), div 3

 G2D_BGR_REG|=1;                      //pass G2D clock

 MBUS_MAT_CLK_GATING_REG|=(1<<10)|1;  //MBUS gating for G2D & DMA

 G2D_BGR_REG|=(1<<16);                //de-assert Reset
}

int g2d_init(g2d_init_para *para)
{
 mixer_set_reg_base(para->g2d_base);
 return 0;
}

static const g2d_init_para G2D_INIT=
{
 .g2d_base=G2D_BASE
};

void G2D_Init(void)
{
 G2D_Clk();                  //клоки, гейты, сбросы...
 g2d_init((void*)&G2D_INIT); //настройка базового адреса

 g2d_bsp_open();

 PRINTF("\nG2D Open!\n");
}

/* Запуск и ожидание завершения работы G2D */
/* 0 - timeout. 1 - OK */
//int xhwacc_waitdone(void)
//{
////	unsigned n = 0x20000000;
////	xG2D_MIXER->G2D_MIXER_CTL |= (1u << 31);	/* start the module */
//	for (;;)
//	{
//		const uint_fast32_t MASK = (1u << 0);	/* FINISH_IRQ */
//		const uint_fast32_t sts = xG2D_MIXER->G2D_MIXER_INT;
//		if (((sts & MASK) != 0))
//		{
//			xG2D_MIXER->G2D_MIXER_INT = MASK;
//			break;
//		}
//		hardware_nonguiyield();
//		if (-- n == 0)
//		{
//			PRINTF("xG2D_MIXER->G2D_MIXER_CTL=%08X, xG2D_MIXER->G2D_MIXER_INT=%08X\n", xG2D_MIXER->G2D_MIXER_CTL, xG2D_MIXER->G2D_MIXER_INT);
//			return 0;
//		}
//	}
//	ASSERT((xG2D_MIXER->G2D_MIXER_CTL & (1u << 31)) == 0);
//	return 1;
//}

int g2d_wait_cmd_finish(void)
{
 u32 mixer_irq_flag,rot_irq_flag;

 Loop:

 mixer_irq_flag=mixer_irq_query();
 rot_irq_flag=rot_irq_query();

 if(mixer_irq_flag==0)
 {
  g2d_mixer_reset();
  return 1;
 }
 else if(rot_irq_flag==0)
 {
  g2d_rot_reset();
  return 2;
 }

 goto Loop;
}

int g2d_fill(g2d_fillrect *para)
{
	__s32 err = 0;

	/* check the parameter valid */
	if (((para->dst_rect.x < 0) &&
	     ((-para->dst_rect.x) > para->dst_rect.w)) ||
	    ((para->dst_rect.y < 0) &&
	     ((-para->dst_rect.y) > para->dst_rect.h)) ||
	    ((para->dst_rect.x > 0) &&
	     (para->dst_rect.x > para->dst_image.w - 1)) ||
	    ((para->dst_rect.y > 0) &&
	     (para->dst_rect.y > para->dst_image.h - 1))) {
		pr_warn("invalid fillrect parameter setting");
		return -EINVAL;
	}
	if (((para->dst_rect.x < 0) &&
				((-para->dst_rect.x) < para->dst_rect.w))) {
		para->dst_rect.w = para->dst_rect.w + para->dst_rect.x;
		para->dst_rect.x = 0;
	} else if ((para->dst_rect.x + para->dst_rect.w)
			> para->dst_image.w) {
		para->dst_rect.w = para->dst_image.w - para->dst_rect.x;
	}
	if (((para->dst_rect.y < 0) &&
				((-para->dst_rect.y) < para->dst_rect.h))) {
		para->dst_rect.h = para->dst_rect.h + para->dst_rect.y;
		para->dst_rect.y = 0;
	} else if ((para->dst_rect.y + para->dst_rect.h)
			> para->dst_image.h)
		para->dst_rect.h = para->dst_image.h - para->dst_rect.y;

	g2d_ext_hd_finish_flag = 0;

	err = mixer_fillrectangle(para);

	return err;
}

enum g2d_scan_order scan_order=G2D_SM_TDLR;

int g2d_blit(g2d_blt *para)
{
	__s32 err = 0;
	__u32 tmp_w, tmp_h;

	if ((para->flag & G2D_BLT_ROTATE90) ||
			(para->flag & G2D_BLT_ROTATE270)) {
		tmp_w = para->src_rect.h;
		tmp_h = para->src_rect.w;
	} else {
		tmp_w = para->src_rect.w;
		tmp_h = para->src_rect.h;
	}


#if 1
	/* check the parameter valid */
	if (((para->src_rect.x < 0) &&
	     ((-para->src_rect.x) > para->src_rect.w)) ||
	    ((para->src_rect.y < 0) &&
	     ((-para->src_rect.y) > para->src_rect.h)) ||
	    ((para->dst_x < 0) &&
	     ((-para->dst_x) > tmp_w)) ||
	    ((para->dst_y < 0) &&
	     ((-para->dst_y) > tmp_h)) ||
	    ((para->src_rect.x > 0) &&
	     (para->src_rect.x > para->src_image.w - 1)) ||
	    ((para->src_rect.y > 0) &&
	     (para->src_rect.y > para->src_image.h - 1)) ||
	    ((para->dst_x > 0) &&
	     (para->dst_x > para->dst_image.w - 1)) ||
	    ((para->dst_y > 0) && (para->dst_y > para->dst_image.h - 1))) {
		pr_warn("invalid blit parameter setting");
		return -EINVAL;
	}
#endif

	if (((para->src_rect.x < 0) &&
				((-para->src_rect.x) < para->src_rect.w))) {
		para->src_rect.w = para->src_rect.w + para->src_rect.x;
		para->src_rect.x = 0;
	} else if ((para->src_rect.x + para->src_rect.w)
			> para->src_image.w) {
		para->src_rect.w = para->src_image.w - para->src_rect.x;
	}
	if (((para->src_rect.y < 0) &&
				((-para->src_rect.y) < para->src_rect.h))) {
		para->src_rect.h = para->src_rect.h + para->src_rect.y;
		para->src_rect.y = 0;
	} else if ((para->src_rect.y + para->src_rect.h)
			> para->src_image.h) {
		para->src_rect.h = para->src_image.h - para->src_rect.y;
	}

	if (((para->dst_x < 0) && ((-para->dst_x) < tmp_w))) {
		para->src_rect.w = tmp_w + para->dst_x;
		para->src_rect.x = (-para->dst_x);
		para->dst_x = 0;
	} else if ((para->dst_x + tmp_w) > para->dst_image.w) {
		para->src_rect.w = para->dst_image.w - para->dst_x;
	}
	if (((para->dst_y < 0) && ((-para->dst_y) < tmp_h))) {
		para->src_rect.h = tmp_h + para->dst_y;
		para->src_rect.y = (-para->dst_y);
		para->dst_y = 0;
	} else if ((para->dst_y + tmp_h) > para->dst_image.h)
		para->src_rect.h = para->dst_image.h - para->dst_y;

	g2d_ext_hd_finish_flag = 0;

	/* Add support inverted order copy, however,
	 * hardware have a bug when reciving y coordinate,
	 * it use (y + height) rather than (y) on inverted
	 * order mode, so here adjust it before pass it to hardware.
	 */
//	mutex_lock(&global_lock);
	if (scan_order > G2D_SM_TDRL)
		para->dst_y += para->src_rect.h;
//	mutex_unlock(&global_lock);

	err = mixer_blt(para, scan_order);

	return err;
}

int g2d_stretchblit(g2d_stretchblt *para)
{
	__s32 err = 0;

	/* check the parameter valid */
	if (((para->src_rect.x < 0) &&
	     ((-para->src_rect.x) > para->src_rect.w)) ||
	    ((para->src_rect.y < 0) &&
	     ((-para->src_rect.y) > para->src_rect.h)) ||
	    ((para->dst_rect.x < 0) &&
	     ((-para->dst_rect.x) > para->dst_rect.w)) ||
	    ((para->dst_rect.y < 0) &&
	     ((-para->dst_rect.y) > para->dst_rect.h)) ||
	    ((para->src_rect.x > 0) &&
	     (para->src_rect.x > para->src_image.w - 1)) ||
	    ((para->src_rect.y > 0) &&
	     (para->src_rect.y > para->src_image.h - 1)) ||
	    ((para->dst_rect.x > 0) &&
	     (para->dst_rect.x > para->dst_image.w - 1)) ||
	    ((para->dst_rect.y > 0) &&
	     (para->dst_rect.y > para->dst_image.h - 1))) {
		pr_warn("invalid stretchblit parameter setting");
		return -EINVAL;
	}
	if (((para->src_rect.x < 0) &&
				((-para->src_rect.x) < para->src_rect.w))) {
		para->src_rect.w = para->src_rect.w + para->src_rect.x;
		para->src_rect.x = 0;
	} else if ((para->src_rect.x + para->src_rect.w)
			> para->src_image.w) {
		para->src_rect.w = para->src_image.w - para->src_rect.x;
	}
	if (((para->src_rect.y < 0) &&
				((-para->src_rect.y) < para->src_rect.h))) {
		para->src_rect.h = para->src_rect.h + para->src_rect.y;
		para->src_rect.y = 0;
	} else if ((para->src_rect.y + para->src_rect.h)
			> para->src_image.h) {
		para->src_rect.h = para->src_image.h - para->src_rect.y;
	}

	if (((para->dst_rect.x < 0) &&
				((-para->dst_rect.x) < para->dst_rect.w))) {
		para->dst_rect.w = para->dst_rect.w + para->dst_rect.x;
		para->dst_rect.x = 0;
	} else if ((para->dst_rect.x + para->dst_rect.w)
			> para->dst_image.w) {
		para->dst_rect.w = para->dst_image.w - para->dst_rect.x;
	}
	if (((para->dst_rect.y < 0) &&
				((-para->dst_rect.y) < para->dst_rect.h))) {
		para->dst_rect.h = para->dst_rect.h + para->dst_rect.y;
		para->dst_rect.y = 0;
	} else if ((para->dst_rect.y + para->dst_rect.h)
			> para->dst_image.h) {
		para->dst_rect.h = para->dst_image.h - para->dst_rect.y;
	}

	g2d_ext_hd_finish_flag = 0;

	/* Add support inverted order copy, however,
	 * hardware have a bug when reciving y coordinate,
	 * it use (y + height) rather than (y) on inverted
	 * order mode, so here adjust it before pass it to hardware.
	 */

//	mutex_lock(&global_lock);
	if (scan_order > G2D_SM_TDRL)
		para->dst_rect.y += para->src_rect.h;
//	mutex_unlock(&global_lock);

	err = mixer_stretchblt(para, scan_order);

	return err;
}
#endif
