#ifndef _G2D_TOP_TYPE_H
#define _G2D_TOP_TYPE_H


union g2d_sclk_gate {
	unsigned int Reg;
	struct {
		unsigned int Mixer_Gate:1;
		unsigned int Rot_Gate:1;
		unsigned int res0:30;
	} Bits;
};

union g2d_hclk_gate {
	unsigned int Reg;
	struct {
		unsigned int Mixer_Gate:1;
		unsigned int Rot_Gate:1;
		unsigned int res0:30;
	} Bits;
};

union g2d_ahb_reset {
	unsigned int Reg;
	struct {
		unsigned int Mixer_Rst:1;
		unsigned int Rot_Rst:1;
		unsigned int res0:30;
	} Bits;
};

union g2d_sclk_div {
	unsigned int Reg;
	struct {
		unsigned int Mixer_SCLK_div:4;
		unsigned int Rot_SCLK_div:4;
		unsigned int res0:24;
	} Bits;
};

union g2d_version {
	unsigned int Reg;
	struct {
		unsigned int gsu_no:2;
		unsigned int vsu_no:2;
		unsigned int rtmx_no:1;
		unsigned int res0:3;
		unsigned int rot_no:1;
		unsigned int res1:7;
		unsigned int ip_version:16;
	} Bits;
};

union g2d_rcq_irq_ctl {
	unsigned int Reg;
	struct {
		unsigned int Sel:1;
		unsigned int res0:3;
		unsigned int TaskEnd_IRQ_En:1;
		unsigned int res1:1;
		unsigned int Finish_IRQ_En:1;
		unsigned int res2:25;
	} Bits;
};

union g2d_rcq_status {
	unsigned int Reg;
	struct {
		unsigned int TaskEnd_IRQ:1;
		unsigned int res0:1;
		unsigned int Finish_IRQ:1;
		unsigned int res1:5;
		unsigned int FrameCount:8;
		unsigned int res2:16;
	} Bits;
};

union g2d_rcq_ctrl {
	unsigned int Reg;
	struct {
		unsigned int Update:1;
		unsigned int res0:31;
	} Bits;
};

union g2d_rcq_header_len {
	unsigned int Reg;
	struct {
		unsigned int UpdateHeaderLen:16;
		unsigned int res0:16;
	} Bits;
};


struct g2d_top_reg {
	/*0x00*/
	union g2d_sclk_gate 		SCLK_Gate;
	union g2d_hclk_gate 		HCLK_Gate;
	union g2d_ahb_reset 		AHB_Rst;
	union g2d_sclk_div 			SCLK_div;
	/*0x10*/
	union g2d_version 			Version;
	unsigned int 				res0[3];
//	/*0x20*/
//	union g2d_rcq_irq_ctl 		RCQ_IRQ_Ctl;
//	union g2d_rcq_status 		RCQ_Status;
//	union g2d_rcq_ctrl 			RCQ_Ctrl;
//	unsigned int 				RCQ_Header_AddrL;
//	/*0x30*/
//	unsigned int 				RCQ_Header_AddrH;
//	union g2d_rcq_header_len 	RCQ_Header_Len;
};


struct g2d_rcq_reg {
	/*0x20*/
	union g2d_rcq_irq_ctl 		RCQ_IRQ_Ctl;
	union g2d_rcq_status 		RCQ_Status;
	union g2d_rcq_ctrl 			RCQ_Ctrl;
	unsigned int 				RCQ_Header_AddrL;
	/*0x30*/
	unsigned int 				RCQ_Header_AddrH;
	union g2d_rcq_header_len 	RCQ_Header_Len;
	unsigned int 				res[2];

};

/*mixer global register define start*/
union g2d_mxier_ctrl {
	unsigned int Reg;
	struct {
		unsigned int res0:4;
		unsigned int ScanOrder:2;
		unsigned int res1:2;
		unsigned int Bist_en:1;
		unsigned int res2:22;
		unsigned int Start:1;
	} Bits;
};

union g2d_mixer_interrupt {
	unsigned int Reg;
	struct {
		unsigned int MixerIrq:1;
		unsigned int res0:3;
		unsigned int FinishIrq:1;
		unsigned int res1:27;
	} Bits;
};

struct g2d_mixer_glb_reg {
	union g2d_mxier_ctrl Ctrl;
	union g2d_mixer_interrupt IRQ;
	unsigned int res[2];
};
/*mixer global register define end*/

// RCQ

union rcq_hd_dw0 {
	unsigned int  Reg;
	struct {
		unsigned int  Length:24;
		unsigned int  AddrH:8;
	} Bits;
};

union rcq_hd_dirty {
	unsigned int  Reg;
	struct {
		unsigned int  Active:1;
		unsigned int  res0:15;
		unsigned int  NextHeaderLength : 16; /* next frame header length */
	} Bits;
};

struct g2d_rcq_head {

	unsigned int  		AddrL; /* 32 bytes align */
	union rcq_hd_dw0 	Dw0;
	union rcq_hd_dirty 	Dirty;
	unsigned int  		Offset; /* offset_addr based on g2d_reg_base */

};


#endif /*End of file*/
