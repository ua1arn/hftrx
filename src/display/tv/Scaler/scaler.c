#include "hardware.h"
#include "formats.h"

#include "scaler_coeff.h"

#include "../FB/reg-de.h"

#define IS_DE3 0
#define CHANNEL 0

#define HSUB 2
#define VSUB 2

#define DE2_VI_SCALER_UNIT_BASE 0x20000
#define DE2_VI_SCALER_UNIT_SIZE 0x20000

#define DE3_VI_SCALER_UNIT_BASE 0x20000
#define DE3_VI_SCALER_UNIT_SIZE 0x08000

#define BIT(x) (1U<<(x))

#define SUN8I_VI_SCALER_COEFF_COUNT		32

#define SUN8I_VI_SCALER_PHASE_FRAC		20
#define SUN8I_VI_SCALER_SCALE_FRAC		20

#define SUN8I_VI_SCALER_SIZE(w, h)		(((h) - 1) << 16 | ((w) - 1))

#define SUN50I_SCALER_VSU_SCALE_MODE_UI		0
#define SUN50I_SCALER_VSU_SCALE_MODE_NORMAL	1

#define SUN8I_SCALER_VSU_CTRL(base)		((base) + 0x0)
#define SUN50I_SCALER_VSU_SCALE_MODE(base)	((base) + 0x10)

#define SUN8I_SCALER_VSU_OUTSIZE(base)		((base) + 0x40)
#define SUN8I_SCALER_VSU_YINSIZE(base)		((base) + 0x80)
#define SUN8I_SCALER_VSU_YHSTEP(base)		((base) + 0x88)
#define SUN8I_SCALER_VSU_YVSTEP(base)		((base) + 0x8c)
#define SUN8I_SCALER_VSU_YHPHASE(base)		((base) + 0x90)
#define SUN8I_SCALER_VSU_YVPHASE(base)		((base) + 0x98)
#define SUN8I_SCALER_VSU_CINSIZE(base)		((base) + 0xc0)
#define SUN8I_SCALER_VSU_CHSTEP(base)		((base) + 0xc8)
#define SUN8I_SCALER_VSU_CVSTEP(base)		((base) + 0xcc)
#define SUN8I_SCALER_VSU_CHPHASE(base)		((base) + 0xd0)
#define SUN8I_SCALER_VSU_CVPHASE(base)		((base) + 0xd8)

#define SUN8I_SCALER_VSU_YHCOEFF0(base, i)	((base) + 0x200 + 0x4 * (i))
#define SUN8I_SCALER_VSU_YHCOEFF1(base, i)	((base) + 0x300 + 0x4 * (i))
#define SUN8I_SCALER_VSU_YVCOEFF(base, i)	((base) + 0x400 + 0x4 * (i))
#define SUN8I_SCALER_VSU_CHCOEFF0(base, i)	((base) + 0x600 + 0x4 * (i))
#define SUN8I_SCALER_VSU_CHCOEFF1(base, i)	((base) + 0x700 + 0x4 * (i))
#define SUN8I_SCALER_VSU_CVCOEFF(base, i)	((base) + 0x800 + 0x4 * (i))

#define SUN8I_SCALER_VSU_CTRL_EN		BIT(0)
#define SUN8I_SCALER_VSU_CTRL_COEFF_RDY		BIT(4)

//#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

#define regmap_write(x,y,z) (*(volatile uint32_t*)(T113_DE_BASE+T113_DE_MUX_VSU+(y)))=(z)

#define regmap_update_bits(x,y,z,t)                    \
{                                                      \
 (*(volatile uint32_t*)(T113_DE_BASE+T113_DE_MUX_VSU+(y)))&=~(z); \
 (*(volatile uint32_t*)(T113_DE_BASE+T113_DE_MUX_VSU+(y)))|=(t);  \
}                                                      \

static uint32_t sun8i_vi_scaler_base(int channel)
{
 return 0;

/*
	if (IS_DE3)
		return DE3_VI_SCALER_UNIT_BASE +
		       DE3_VI_SCALER_UNIT_SIZE * channel;
	else
		return DE2_VI_SCALER_UNIT_BASE +
		       DE2_VI_SCALER_UNIT_SIZE * channel;
*/

}

static int sun8i_vi_scaler_coef_index(unsigned int step)
{
	unsigned int scale, int_part, float_part;

	scale = step >> (SUN8I_VI_SCALER_SCALE_FRAC - 3);
	int_part = scale >> 3;
	float_part = scale & 0x7;

	switch (int_part) {
	case 0:
		return 0;
	case 1:
		return float_part;
	case 2:
		return 8 + (float_part >> 1);
	case 3:
		return 12;
	case 4:
		return 13;
	default:
		return 14;
	}
}

static void sun8i_vi_scaler_set_coeff(uint32_t base, uint32_t hstep, uint32_t vstep)
{
	const uint32_t *ch_left, *ch_right, *cy;
	int offset, i;

	if ((HSUB == 1) && (VSUB == 1)) {
		ch_left = lan3coefftab32_left;
		ch_right = lan3coefftab32_right;
		cy = lan2coefftab32;
	} else {
		ch_left = bicubic8coefftab32_left;
		ch_right = bicubic8coefftab32_right;
		cy = bicubic4coefftab32;
	}

	offset = sun8i_vi_scaler_coef_index(hstep) *
			SUN8I_VI_SCALER_COEFF_COUNT;
	for (i = 0; i < SUN8I_VI_SCALER_COEFF_COUNT; i++) {
		regmap_write(map, SUN8I_SCALER_VSU_YHCOEFF0(base, i),
			     lan3coefftab32_left[offset + i]);
		regmap_write(map, SUN8I_SCALER_VSU_YHCOEFF1(base, i),
			     lan3coefftab32_right[offset + i]);
		regmap_write(map, SUN8I_SCALER_VSU_CHCOEFF0(base, i),
			     ch_left[offset + i]);
		regmap_write(map, SUN8I_SCALER_VSU_CHCOEFF1(base, i),
			     ch_right[offset + i]);
	}

	offset = sun8i_vi_scaler_coef_index(hstep) *
			SUN8I_VI_SCALER_COEFF_COUNT;
	for (i = 0; i < SUN8I_VI_SCALER_COEFF_COUNT; i++) {
		regmap_write(map, SUN8I_SCALER_VSU_YVCOEFF(base, i),
			     lan2coefftab32[offset + i]);
		regmap_write(map, SUN8I_SCALER_VSU_CVCOEFF(base, i),
			     cy[offset + i]);
	}
}

void sun8i_vi_scaler_setup(uint32_t src_w, uint32_t src_h, uint32_t dst_w, uint32_t dst_h, uint32_t hscale, uint32_t vscale, uint32_t hphase, uint32_t vphase)
{
	uint32_t chphase, cvphase;
	uint32_t insize, outsize;
	uint32_t base;

	base = sun8i_vi_scaler_base(CHANNEL);

	hphase <<= SUN8I_VI_SCALER_PHASE_FRAC - 16;
	vphase <<= SUN8I_VI_SCALER_PHASE_FRAC - 16;
	hscale <<= SUN8I_VI_SCALER_SCALE_FRAC - 16;
	vscale <<= SUN8I_VI_SCALER_SCALE_FRAC - 16;

	insize = SUN8I_VI_SCALER_SIZE(src_w, src_h);
	outsize = SUN8I_VI_SCALER_SIZE(dst_w, dst_h);

	/*
	 * This is chroma V/H phase calculation as it appears in
	 * BSP driver. There is no detailed explanation. YUV 420
	 * chroma is threated specialy for some reason.
	 */
	if ((HSUB == 2) && (VSUB == 2)) {
		chphase = hphase >> 1;
		cvphase = (vphase >> 1) -
			(1UL << (SUN8I_VI_SCALER_SCALE_FRAC - 2));
	} else {
		chphase = hphase;
		cvphase = vphase;
	}

	if (IS_DE3) {
		uint32_t val;

		if ((HSUB == 1) && (VSUB) == 1)
			val = SUN50I_SCALER_VSU_SCALE_MODE_UI;
		else
			val = SUN50I_SCALER_VSU_SCALE_MODE_NORMAL;

		regmap_write(mixer->engine.regs,
			     SUN50I_SCALER_VSU_SCALE_MODE(base), val);
	}

	regmap_write(mixer->engine.regs,
		     SUN8I_SCALER_VSU_OUTSIZE(base), outsize);
	regmap_write(mixer->engine.regs,
		     SUN8I_SCALER_VSU_YINSIZE(base), insize);
	regmap_write(mixer->engine.regs,
		     SUN8I_SCALER_VSU_YHSTEP(base), hscale);
	regmap_write(mixer->engine.regs,
		     SUN8I_SCALER_VSU_YVSTEP(base), vscale);
	regmap_write(mixer->engine.regs,
		     SUN8I_SCALER_VSU_YHPHASE(base), hphase);
	regmap_write(mixer->engine.regs,
		     SUN8I_SCALER_VSU_YVPHASE(base), vphase);
	regmap_write(mixer->engine.regs,
		     SUN8I_SCALER_VSU_CINSIZE(base),
		     SUN8I_VI_SCALER_SIZE(src_w / HSUB,
					  src_h / VSUB));
	regmap_write(mixer->engine.regs,
		     SUN8I_SCALER_VSU_CHSTEP(base),
		     hscale / HSUB);
	regmap_write(mixer->engine.regs,
		     SUN8I_SCALER_VSU_CVSTEP(base),
		     vscale / VSUB);
	regmap_write(mixer->engine.regs,
		     SUN8I_SCALER_VSU_CHPHASE(base), chphase);
	regmap_write(mixer->engine.regs,
		     SUN8I_SCALER_VSU_CVPHASE(base), cvphase);

	sun8i_vi_scaler_set_coeff(base,
				  hscale, vscale);
}

void sun8i_vi_scaler_enable(uint8_t enable)
{
	uint32_t val, base;

	base = sun8i_vi_scaler_base(CHANNEL);

	if (enable)
		val = SUN8I_SCALER_VSU_CTRL_EN |
		      SUN8I_SCALER_VSU_CTRL_COEFF_RDY;
	else
		val = 0;

	regmap_write(mixer->engine.regs,
		     SUN8I_SCALER_VSU_CTRL(base), val);
}
