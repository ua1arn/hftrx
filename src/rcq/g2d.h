#ifndef G2DH
#define G2DH

#include "hardware.h"
#include "g2d_top_type.h"
#include "g2d_mixer_type.h"
#include "g2d_rotate_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define G2D_RCQ_TASKS_COUNT			128

#if (CORE_ARM)
	#define G2D_IRQ				121
#endif

#if (CORE_DSP)
	#define G2D_IRQ				55
#endif

//#define G2D_CLK_REG		    		(*(volatile uint32_t *)(0x02001000 + 0x0630))
//#define G2D_BGR_REG		    		(*(volatile uint32_t *)(0x02001000 + 0x063C))
//
//#define G2D_BASE 					(0x05410000)        //!< G2D_TOP Graphic 2D top Base
#define G2D_RCQ_BASE 				(0x05410020)        //!< G2D_TOP Graphic 2D top Base
//#define G2D_MIXER_BASE 				(0x05410100)        //
//#define G2D_BLD_BASE 				(0x05410400)        //!< G2D_BLD Graphic 2D (G2D) Engine Blender Base
#define G2D_CSC_BASE 				(0x05410500)        //!< G2D_BLD Graphic 2D (G2D) Engine Blender Base
//#define G2D_V0_BASE 				(0x05410800)        //!< G2D_VI Graphic 2D VI surface Base
//#define G2D_UI0_BASE 				(0x05411000)        //!< G2D_UI Graphic 2D UI surface Base
//#define G2D_UI1_BASE 				(0x05411800)        //!< G2D_UI Graphic 2D UI surface Base
//#define G2D_UI2_BASE 				(0x05412000)        //!< G2D_UI Graphic 2D UI surface Base
//#define G2D_WB_BASE 				(0x05413000)        //!< G2D_WB Graphic 2D (G2D) Engine Write Back Base
//#define G2D_VSU_BASE 				(0x05418000)        //!< G2D_VSU Graphic 2D Video Scaler Base
//#define G2D_ROT_BASE 				(0x05438000)

#define G2D_Top 	                ((volatile struct g2d_top_reg *) 					G2D_BASE)
#define G2D_RCQ 	                ((volatile struct g2d_rcq_reg *) 					G2D_RCQ_BASE)
#define G2D_Mixer 	                ((volatile struct g2d_mixer_glb_reg *) 				G2D_MIXER_BASE)
#define G2D_Blender                 ((volatile struct g2d_mixer_bld_reg *) 				G2D_BLD_BASE)
#define G2D_CSC		                ((volatile struct g2d_mixer_csc_reg *) 				G2D_CSC_BASE)
#define G2D_VY 		 	           	((volatile struct g2d_mixer_ovl_v_reg *) 			G2D_V0_BASE)
#define G2D_VU 		     	       	((volatile struct g2d_mixer_ovl_u_reg *) 			G2D_UI0_BASE)
#define G2D_VV 		        	    ((volatile struct g2d_mixer_ovl_u_reg *) 			G2D_UI1_BASE)
#define G2D_UI 		            	((volatile struct g2d_mixer_ovl_u_reg *) 			G2D_UI2_BASE)
#define G2D_WBack 	                ((volatile struct g2d_mixer_write_back_reg *) 		G2D_WB_BASE)
#define G2D_VScaler                 ((volatile struct g2d_mixer_video_scaler_reg *) 	G2D_VSU_BASE)
#define G2D_Rot 	                ((volatile struct g2d_rot_reg *) 					G2D_ROT_BASE)

typedef struct {

	struct g2d_rcq_head				Headers[G2D_RCQ_TASKS_COUNT];
	struct g2d_mixer_ovl_u_reg 		UI[G2D_RCQ_TASKS_COUNT];
	struct g2d_mixer_ovl_u_reg 		Canvas[1];
	struct g2d_mixer_bld_reg		 	Blender[2];
	struct g2d_mixer_write_back_reg 	WriteBack[1];
	struct g2d_rot_reg					CopyRotate[1];

	uint32_t 					UpdateHeader;
	uint32_t 					res[3];

} TG2D_RCQTask;

extern TG2D_RCQTask				G2D_RCQTask;

typedef enum
{
	G2D_ROT_0 = 0,
	G2D_ROT_90,
	G2D_ROT_180,
	G2D_ROT_270,

} EG2D_ROTANGLE;

typedef enum
{
	G2D_ROT_FLIP_NONE = 0,
	G2D_ROT_FLIP_V,
	G2D_ROT_FLIP_H,
	G2D_ROT_FLIP_VH,

} EG2D_ROTFLIP;

typedef struct{

	uint32_t 		Fmt;
	uint32_t 		Height;
	uint32_t 		Width;

	uint32_t 		srcAddr;
	uint32_t 		dstAddr;

	EG2D_ROTANGLE 	Angle;
	EG2D_ROTFLIP  	Flip;


} TG2D_ROT_Params;

typedef enum
{
	G2D_SRC_PERI2X = 0b00,
	G2D_SRC_VIDEO0x4 = 0b01,
	G2D_SRC_VIDEO1x4 = 0b10,
	G2D_SRC_AUDIO1div2 = 0b11,

} EG2DClkSrc;

typedef enum
{
	G2D_M_FACTOR1 = 0,
	G2D_M_FACTOR2,
	G2D_M_FACTOR3,
	G2D_M_FACTOR4,
	G2D_M_FACTOR5,
	G2D_M_FACTOR6,
	G2D_M_FACTOR7,
	G2D_M_FACTOR8,
	G2D_M_FACTOR9,
	G2D_M_FACTOR10,
	G2D_M_FACTOR11,
	G2D_M_FACTOR12,
	G2D_M_FACTOR13,
	G2D_M_FACTOR14,
	G2D_M_FACTOR15,
	G2D_M_FACTOR16,
	G2D_M_FACTOR17,
	G2D_M_FACTOR18,
	G2D_M_FACTOR19,
	G2D_M_FACTOR20,
	G2D_M_FACTOR21,
	G2D_M_FACTOR22,
	G2D_M_FACTOR23,
	G2D_M_FACTOR24,
	G2D_M_FACTOR25,
	G2D_M_FACTOR26,
	G2D_M_FACTOR27,
	G2D_M_FACTOR28,
	G2D_M_FACTOR29,
	G2D_M_FACTOR30,
	G2D_M_FACTOR31,
	G2D_M_FACTOR32,

} EG2DFACTORM;

void G2D_Setup(void);
void G2D_ROT_Copy_INT(TG2D_ROT_Params* cfg);
void G2D_Callback(uint32_t sts);

void G2D_Rcq_RunTask(TG2D_RCQTask* task);
//uint32_t G2D_Rcq_DrawFilledRectangle(G2D_RCQTaskA* task,
//								 uint32_t* canvas, uint32_t canvas_width, uint32_t canvas_height, uint32_t canvas_color_ARGB,
//								 uint32_t rec_ofstX, uint32_t rec_ofstY,
//								 uint32_t rec_width, uint32_t rec_height,
//								 uint32_t rec_color_ARGB);

#define __va_to_pa(_a) ((uint32_t) _a)
void G2D_IRQHandler(void);

#ifdef __cplusplus
}
#endif


#endif
