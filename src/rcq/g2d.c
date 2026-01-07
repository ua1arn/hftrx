#include "g2d.h"
#include "formats.h"

#if RTMIXIDLCD && LCDMODE_ARGB8888 && (1 && (CPUSTYLE_T113 || CPUSTYLE_F133))

__ALIGNED(32) TG2D_RCQTask					G2D_RCQTask;


void G2D_DisableAll(void){

	G2D_VY->Attr.Bits.LayEn = 0;
	G2D_VU->Attr.Bits.LayEn = 0;
	G2D_VV->Attr.Bits.LayEn = 0;
	G2D_UI->Attr.Bits.LayEn = 0;
	G2D_CSC->CscCtrl.Reg = 0;
	G2D_VScaler->Ctrl.Bits.Enable = 0;
	G2D_Rot->Ctrl.Bits.Start = 0;
}

void G2D_Rcq_RunTask(TG2D_RCQTask* task){

	dcache_clean((uintptr_t) task, sizeof(TG2D_RCQTask));

	G2D_RCQ->RCQ_Header_AddrL = (uint32_t)__va_to_pa((uint32_t) &task->Headers[0]);
	G2D_RCQ->RCQ_Header_AddrH = 0;
	G2D_RCQ->RCQ_Header_Len.Bits.UpdateHeaderLen = task->UpdateHeader * sizeof(struct g2d_rcq_head); //
	G2D_RCQ->RCQ_IRQ_Ctl.Bits.Finish_IRQ_En = 0;
	G2D_RCQ->RCQ_IRQ_Ctl.Bits.TaskEnd_IRQ_En = 1;
	G2D_RCQ->RCQ_Ctrl.Bits.Update = 1;
}

__weak void G2D_Callback(uint32_t sts){
	//DBG("\n\r ####################### G2D_IRQHandler = "); DBG32(reg);
}

void G2D_IRQHandler(void){

	uint32_t reg = G2D_Rot->IRQ.Reg; //G2D_ROT_INT;
	G2D_Rot->IRQ.Reg = reg;

	uint32_t mix = G2D_Mixer->IRQ.Reg; //G2D_MIXER_INTERRUPT;
	G2D_Mixer->IRQ.Reg = mix;

	uint32_t rcqs = G2D_RCQ->RCQ_Status.Reg;
	G2D_RCQ->RCQ_Status.Reg = rcqs;

//	uint32_t rcq = G2D_RCQ->RCQ_IRQ_Ctl.Reg;
//	G2D_RCQ->RCQ_IRQ_Ctl.Reg = rcq;

//	DBG("\n\r ####################### G2D_IRQHandler Rot = "); DBG32(reg);
//	DBG("\n\r ####################### G2D_IRQHandler Mix = "); DBG32(mix);
//	DBG("\n\r ####################### G2D_IRQHandler RCQ Status = "); DBG32(rcqs);
//	DBG("\n\r ####################### G2D_IRQHandler RCQ IRQ = "); DBG32(rcq);

	//BIT_CLEAR(G2D_Top->RCQ_Status.Reg, 2);
	//G2D_Top->RCQ_IRQ_Ctl.Bits.Finish_IRQ_En = 0;
//	 DBG("\n\r *-*-*-*-*-*-** ");
//	 DBG("\n\r G2D_UIx_ATTR = "); DBG32(G2D_UIx_ATTR(1));
//	 DBG("\n\r G2D_UIx_MBSIZE = "); DBG32(G2D_UIx_MBSIZE(1));
//	 DBG("\n\r G2D_UIx_COOR = "); DBG32(G2D_UIx_COOR(1));
//	 DBG("\n\r G2D_UIx_PITCH = "); DBG32(G2D_UIx_PITCH(1));
//	 DBG("\n\r G2D_UIx_LADD = "); DBG32(G2D_UIx_LADD(1));
//	 DBG("\n\r G2D_UIx_HADD = "); DBG32(G2D_UIx_HADD(1));
//	 DBG("\n\r G2D_UIx_FILLC = "); DBG32(G2D_UIx_FILLC(1));
//	 DBG("\n\r G2D_UIx_SIZE = "); DBG32(G2D_UIx_SIZE(1));
//
//	 DBG("\n\r *-*-*-*-*-*-** ");
//	 DBG("\n\r G2D_UIx_ATTR = "); DBG32(G2D_UIx_ATTR(2));
//	 DBG("\n\r G2D_UIx_MBSIZE = "); DBG32(G2D_UIx_MBSIZE(2));
//	 DBG("\n\r G2D_UIx_COOR = "); DBG32(G2D_UIx_COOR(2));
//	 DBG("\n\r G2D_UIx_PITCH = "); DBG32(G2D_UIx_PITCH(2));
//	 DBG("\n\r G2D_UIx_LADD = "); DBG32(G2D_UIx_LADD(2));
//	 DBG("\n\r G2D_UIx_HADD = "); DBG32(G2D_UIx_HADD(2));
//	 DBG("\n\r G2D_UIx_FILLC = "); DBG32(G2D_UIx_FILLC(2));
//	 DBG("\n\r G2D_UIx_SIZE = "); DBG32(G2D_UIx_SIZE(2));


	G2D_Callback(reg);
}

//void G2D_Init(EG2DClkSrc src, EG2DFACTORM m ){
//
//	BIT_SET(CCU->MBUS_CLK_REG, 30);
//	BIT_SET(CCU->MBUS_MAT_CLK_GATING_REG, 10); // G2D_MCLK_EN
//
//	G2D_CLK_REG =  (1 << 31) |
//				 ((src & 7)  << 24) |
//				 ((m & 0x1F) <<  0) |
//				  0;
//
//
//	BIT_SET(G2D_BGR_REG, 0);
//	BIT_SET(G2D_BGR_REG, 16);
//}
//
//void G2D_Setup(void){
//
//	G2D_Init(G2D_SRC_PERI2X, G2D_M_FACTOR4); // 1200/4 = 300 MHz
//
//	const uint32_t modules = (1 << 1) | // ROT
//							 (1 << 0) | // MIXER
//							 0;
//
//	G2D_Top->SCLK_Gate.Reg = modules; // G2D_SCLK_GATE = modules;
//	G2D_Top->HCLK_Gate.Reg = modules; // G2D_HCLK_GATE = modules;
//	G2D_Top->SCLK_div.Reg = 0;		// G2D_SCLK_DIV = 0;
//	G2D_Top->AHB_Rst.Reg = 0; // G2D_AHB_RST = 0;
//	G2D_Top->AHB_Rst.Reg = modules; // G2D_AHB_RST = modules;
//
//	IRQSetHandler(G2D_IRQ, G2D_IRQHandler);
//}

void G2D_ROT_Copy_INT(TG2D_ROT_Params* cfg){

	// LBC compression TODO

	G2D_Rot->Ctrl.Reg = 0;

	G2D_Rot->InFmt.Reg = 0;

	G2D_Rot->InSize.Bits.Height = (((cfg->Height - 1) & 0x1FFF));
	G2D_Rot->InSize.Bits.Width = (((cfg->Width  - 1) & 0x1FFF));
	G2D_Rot->InPitch[0] = cfg->Width * 4; // x length * fmt size. Should be 128bit aligned.

	G2D_Rot->In_lAddr0 = (uint32_t)__va_to_pa((uint32_t)cfg->srcAddr);
	G2D_Rot->In_hAddr0 = 0;

	if ((cfg->Angle == G2D_ROT_90) || (cfg->Angle == G2D_ROT_270)){

		G2D_Rot->OutSize.Bits.Width = (((cfg->Height - 1) & 0x1FFF));
		G2D_Rot->OutSize.Bits.Height = (((cfg->Width  - 1) & 0x1FFF));
		G2D_Rot->OutPitch[0] = cfg->Height * 4;	// Should be 128bit aligned.
	}
	else {

		G2D_Rot->OutSize.Bits.Height = (((cfg->Height - 1) & 0x1FFF));
		G2D_Rot->OutSize.Bits.Width = (((cfg->Width  - 1) & 0x1FFF));
		G2D_Rot->OutPitch[0] = cfg->Width * 4;	// Should be 128bit aligned.
	}

	G2D_Rot->Out_lAddr0 = (uint32_t)__va_to_pa((uint32_t)cfg->dstAddr);
	G2D_Rot->Out_hAddr0 = 0;

	G2D_Rot->IRQ.Bits.FinishIrq = 1;
	G2D_Rot->IRQ.Bits.FinishJob = 1;

	G2D_Rot->Ctrl.Bits.ModeSel = 1;
	G2D_Rot->Ctrl.Bits.Degree = cfg->Angle;
	G2D_Rot->Ctrl.Bits.FlipH = cfg->Flip >> 1;
	G2D_Rot->Ctrl.Bits.FlipV = cfg->Flip & 1;
	G2D_Rot->Ctrl.Bits.Start = 1;
}
#endif

