#ifndef _G2D_ROTATE_TYPE_H
#define _G2D_ROTATE_TYPE_H

union g2d_rot_ctrl {
	unsigned int Reg;
	struct {
		unsigned int ModeSel:2;
		unsigned int res0:2;
		unsigned int Degree:2;
		unsigned int FlipV:1;
		unsigned int FlipH:1;
		unsigned int res1:22;
		unsigned int Bist_en:1;
		unsigned int Start:1;
	} Bits;
};

union g2d_rot_interrupt {
	unsigned int Reg;
	struct {
		unsigned int FinishIrq:1;
		unsigned int res0:15;
		unsigned int FinishJob:1;
		unsigned int res1:15;
	} Bits;
};

union g2d_rot_time_ctrl {
	unsigned int Reg;
	struct {
		unsigned int Timeout_st:1;
		unsigned int res0:29;
		unsigned int Timeout_rst_en:1;
		unsigned int Timeout_rst:1;
	} Bits;
};

union g2d_rot_in_fmt {
	unsigned int Reg;
	struct {
		unsigned int Fmt:6;
		unsigned int res0:26;
	} Bits;
};

union g2d_rot_size {
	unsigned int Reg;
	struct {
		unsigned int Width:13;
		unsigned int res0:3;
		unsigned int Height:13;
		unsigned int res1:3;
	} Bits;
};

union g2d_rot_rand_ctrl {
	unsigned int Reg;
	struct {
		unsigned int RandEn:1;
		unsigned int res0:3;
		unsigned int Mode:2;
		unsigned int res1:2;
		unsigned int Seed:24;
	} Bits;
};

union g2d_rot_rand_clk {
	unsigned int Reg;
	struct {
		unsigned int Neg_num:16;
		unsigned int Pos_num:16;
	};
};


struct g2d_rot_reg {
	/*0x00*/
	union g2d_rot_ctrl Ctrl;
	union g2d_rot_interrupt IRQ;
	union g2d_rot_time_ctrl TimeCtrl;
	unsigned int res0[5];
	/*0x20*/
	union g2d_rot_in_fmt InFmt;
	union g2d_rot_size InSize;
	unsigned int res1[2];
	/*0x30*/
	unsigned int InPitch[3];
	unsigned int res2;
	/*0x40*/
	unsigned int In_lAddr0;
	unsigned int In_hAddr0;
	unsigned int In_lAddr1;
	unsigned int In_hAddr1;
	/*0x50*/
	unsigned int In_lAddr2;
	unsigned int In_hAddr2;
	unsigned int res3[11];
	/*0x84*/
	union g2d_rot_size OutSize;
	unsigned int res4[2];
	/*0x90*/
	unsigned int OutPitch[3];
	unsigned int res5;
	/*0xa0*/
	unsigned int Out_lAddr0;
	unsigned int Out_hAddr0;
	unsigned int Out_lAddr1;
	unsigned int Out_hAddr1;
	/*0xb0*/
	unsigned int Out_lAddr2;
	unsigned int Out_hAddr2;
	union g2d_rot_rand_ctrl InRandCtrl;
	union g2d_rot_rand_clk InRandClk;
	/*0xc0*/
	union g2d_rot_rand_ctrl OutRandCtrl;
	union g2d_rot_rand_clk OutRandClk;
	unsigned int res6[2];
};

#endif /*End of file*/
