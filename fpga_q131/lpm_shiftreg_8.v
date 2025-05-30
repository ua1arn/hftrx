// megafunction wizard: %LPM_SHIFTREG%
// GENERATION: STANDARD
// VERSION: WM1.0
// MODULE: LPM_SHIFTREG 

// ============================================================
// File Name: lpm_shiftreg_8.v
// Megafunction Name(s):
// 			LPM_SHIFTREG
//
// Simulation Library Files(s):
// 			lpm
// ============================================================
// ************************************************************
// THIS IS A WIZARD-GENERATED FILE. DO NOT EDIT THIS FILE!
//
// 13.1.4 Build 182 03/12/2014 SJ Full Version
// ************************************************************


//Copyright (C) 1991-2014 Altera Corporation
//Your use of Altera Corporation's design tools, logic functions 
//and other software and tools, and its AMPP partner logic 
//functions, and any output files from any of the foregoing 
//(including device programming or simulation files), and any 
//associated documentation or information are expressly subject 
//to the terms and conditions of the Altera Program License 
//Subscription Agreement, Altera MegaCore Function License 
//Agreement, or other applicable license agreement, including, 
//without limitation, that your use is for the sole purpose of 
//programming logic devices manufactured by Altera and sold by 
//Altera or its authorized distributors.  Please refer to the 
//applicable agreement for further details.


// synopsys translate_off
`timescale 1 ps / 1 ps
// synopsys translate_on
module lpm_shiftreg_8 (
	clock,
	data,
	enable,
	load,
	shiftin,
	q,
	shiftout);

	input	  clock;
	input	[7:0]  data;
	input	  enable;
	input	  load;
	input	  shiftin;
	output	[7:0]  q;
	output	  shiftout;

	wire  sub_wire0;
	wire [7:0] sub_wire1;
	wire  shiftout = sub_wire0;
	wire [7:0] q = sub_wire1[7:0];

	lpm_shiftreg	LPM_SHIFTREG_component (
				.clock (clock),
				.data (data),
				.load (load),
				.enable (enable),
				.shiftin (shiftin),
				.shiftout (sub_wire0),
				.q (sub_wire1)
				// synopsys translate_off
				,
				.aclr (),
				.aset (),
				.sclr (),
				.sset ()
				// synopsys translate_on
				);
	defparam
		LPM_SHIFTREG_component.lpm_direction = "LEFT",
		LPM_SHIFTREG_component.lpm_type = "LPM_SHIFTREG",
		LPM_SHIFTREG_component.lpm_width = 8;


endmodule

// ============================================================
// CNX file retrieval info
// ============================================================
// Retrieval info: PRIVATE: ACLR NUMERIC "0"
// Retrieval info: PRIVATE: ALOAD NUMERIC "0"
// Retrieval info: PRIVATE: ASET NUMERIC "0"
// Retrieval info: PRIVATE: ASET_ALL1 NUMERIC "1"
// Retrieval info: PRIVATE: CLK_EN NUMERIC "1"
// Retrieval info: PRIVATE: INTENDED_DEVICE_FAMILY STRING "Cyclone III"
// Retrieval info: PRIVATE: LeftShift NUMERIC "1"
// Retrieval info: PRIVATE: ParallelDataInput NUMERIC "1"
// Retrieval info: PRIVATE: Q_OUT NUMERIC "1"
// Retrieval info: PRIVATE: SCLR NUMERIC "0"
// Retrieval info: PRIVATE: SLOAD NUMERIC "1"
// Retrieval info: PRIVATE: SSET NUMERIC "0"
// Retrieval info: PRIVATE: SSET_ALL1 NUMERIC "1"
// Retrieval info: PRIVATE: SYNTH_WRAPPER_GEN_POSTFIX STRING "0"
// Retrieval info: PRIVATE: SerialShiftInput NUMERIC "1"
// Retrieval info: PRIVATE: SerialShiftOutput NUMERIC "1"
// Retrieval info: PRIVATE: nBit NUMERIC "8"
// Retrieval info: PRIVATE: new_diagram STRING "1"
// Retrieval info: LIBRARY: lpm lpm.lpm_components.all
// Retrieval info: CONSTANT: LPM_DIRECTION STRING "LEFT"
// Retrieval info: CONSTANT: LPM_TYPE STRING "LPM_SHIFTREG"
// Retrieval info: CONSTANT: LPM_WIDTH NUMERIC "8"
// Retrieval info: USED_PORT: clock 0 0 0 0 INPUT NODEFVAL "clock"
// Retrieval info: USED_PORT: data 0 0 8 0 INPUT NODEFVAL "data[7..0]"
// Retrieval info: USED_PORT: enable 0 0 0 0 INPUT NODEFVAL "enable"
// Retrieval info: USED_PORT: load 0 0 0 0 INPUT NODEFVAL "load"
// Retrieval info: USED_PORT: q 0 0 8 0 OUTPUT NODEFVAL "q[7..0]"
// Retrieval info: USED_PORT: shiftin 0 0 0 0 INPUT NODEFVAL "shiftin"
// Retrieval info: USED_PORT: shiftout 0 0 0 0 OUTPUT NODEFVAL "shiftout"
// Retrieval info: CONNECT: @clock 0 0 0 0 clock 0 0 0 0
// Retrieval info: CONNECT: @data 0 0 8 0 data 0 0 8 0
// Retrieval info: CONNECT: @enable 0 0 0 0 enable 0 0 0 0
// Retrieval info: CONNECT: @load 0 0 0 0 load 0 0 0 0
// Retrieval info: CONNECT: @shiftin 0 0 0 0 shiftin 0 0 0 0
// Retrieval info: CONNECT: q 0 0 8 0 @q 0 0 8 0
// Retrieval info: CONNECT: shiftout 0 0 0 0 @shiftout 0 0 0 0
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_shiftreg_8.v TRUE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_shiftreg_8.inc FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_shiftreg_8.cmp FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_shiftreg_8.bsf TRUE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_shiftreg_8_inst.v FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL lpm_shiftreg_8_bb.v FALSE
// Retrieval info: LIB_FILE: lpm
