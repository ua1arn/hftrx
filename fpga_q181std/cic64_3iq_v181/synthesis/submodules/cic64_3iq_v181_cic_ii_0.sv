// (C) 2001-2018 Intel Corporation. All rights reserved.
// Your use of Intel Corporation's design tools, logic functions and other 
// software and tools, and its AMPP partner logic functions, and any output 
// files from any of the foregoing (including device programming or simulation 
// files), and any associated documentation or information are expressly subject 
// to the terms and conditions of the Intel Program License Subscription 
// Agreement, Intel FPGA IP License Agreement, or other applicable 
// license agreement, including, without limitation, that your use is for the 
// sole purpose of programming logic devices manufactured by Intel and sold by 
// Intel or its authorized distributors.  Please refer to the applicable 
// agreement for further details.



module cic64_3iq_v181_cic_ii_0 (
    in0_data,
    in1_data,
    in2_data,
    in3_data,
    in4_data,
    in5_data,
	out_ready,
	in_valid,
    clk,
    clken,
    reset_n,
    in_ready,
	out_channel,
	out_startofpacket,
	out_endofpacket,
	in_error,
	out_error,
    out_data,
    out_valid
        );

    parameter DEVICE_FAMILY  = "Cyclone IV E";



    parameter FILTER_TYPE             =  "decimator";
    parameter STAGES        =  5;
    parameter D_DELAY              =  1;
    parameter VRC_EN   =  0;
    parameter RCF_MAX         =  64;
    parameter RCF_MIN         =  64;
    parameter INTERFACES    =  6;
    parameter CH_PER_INT  =  1;
    parameter INT_USE_MEM      =  "false";
    parameter INT_MEM     =  "auto";
    parameter DIF_USE_MEM  =  "false";
    parameter DIF_MEM =  "auto";
    parameter IN_WIDTH             =  32;
    parameter OUT_WIDTH            =  32;
    parameter ROUND_TYPE         =  "H_PRUNE";
    parameter PIPELINING              =  0;


    parameter C_STAGE_0_WIDTH         = 38;
    parameter C_STAGE_1_WIDTH         = 37;
    parameter C_STAGE_2_WIDTH         = 36;
    parameter C_STAGE_3_WIDTH         = 35;
    parameter C_STAGE_4_WIDTH         = 35;
    parameter C_STAGE_5_WIDTH         = 0;
    parameter C_STAGE_6_WIDTH         = 0;
    parameter C_STAGE_7_WIDTH         = 0;
    parameter C_STAGE_8_WIDTH         = 0;
    parameter C_STAGE_9_WIDTH         = 0;
    parameter C_STAGE_10_WIDTH        = 0;
    parameter C_STAGE_11_WIDTH        = 0;
    parameter MAX_C_STAGE_WIDTH       = 62;
 
    parameter I_STAGE_0_WIDTH         = 61;
    parameter I_STAGE_1_WIDTH         = 55;
    parameter I_STAGE_2_WIDTH         = 49;
    parameter I_STAGE_3_WIDTH         = 45;
    parameter I_STAGE_4_WIDTH         = 40;
    parameter I_STAGE_5_WIDTH         = 0;
    parameter I_STAGE_6_WIDTH         = 0;
    parameter I_STAGE_7_WIDTH         = 0;
    parameter I_STAGE_8_WIDTH         = 0;
    parameter I_STAGE_9_WIDTH         = 0;
    parameter I_STAGE_10_WIDTH        = 0;
    parameter I_STAGE_11_WIDTH        = 0;
    parameter MAX_I_STAGE_WIDTH       = 62;

    localparam TOTAL_CHANNELS = CH_PER_INT*INTERFACES;

    localparam INTERFACES_IN  = (FILTER_TYPE=="decimator" & INTERFACES > 1)  ?    INTERFACES : 1  ;
    localparam INTERFACES_OUT = (FILTER_TYPE=="interpolator" & INTERFACES > 1)  ?    INTERFACES : 1  ;
    localparam CHANNEL_SIZE_OUT = (FILTER_TYPE=="interpolator" & INTERFACES > 1) ? CH_PER_INT : TOTAL_CHANNELS ;
    localparam CHANNEL_SIZE_IN = (FILTER_TYPE=="decimator" & INTERFACES > 1) ? CH_PER_INT : TOTAL_CHANNELS ;
    localparam CHANNEL_OUT_WIDTH = (CHANNEL_SIZE_OUT > 1) ? $clog2(CHANNEL_SIZE_OUT) : 1;
    localparam NUMBER_OF_CHANNELS = INTERFACES*CH_PER_INT;
    localparam RATE_FACTOR_WIDTH = $clog2(RCF_MAX+1);
    localparam CHANNEL_WIDTH = $clog2(TOTAL_CHANNELS);
    localparam COUNTER_FS_MAX = RCF_MAX*NUMBER_OF_CHANNELS;

    //latency calculations
    //localparam COMB_STAGE_LATENCY = 1;
    //localparam COMB_SECTION_LATENCY = COMB_STAGE_LATENCY*STAGES;
    //localparam INT_SECTION_LATENCY = (~INT_USE_MEM & PIPELINING > 1)  ? PIPELINING*STAGES:  STAGES*NUMBER_OF_CHANNELS;
    //localparam MUX_LATENCY = NUMBER_OF_CHANNELS*(INTERFACES-1)+1;
    //localparam DEC_SISO_LATENCY =  COMB_SECTION_LATENCY + INT_SECTION_LATENCY;
    //localparam INT_SISO_LATENCY =  COMB_SECTION_LATENCY + INT_SECTION_LATENCY;
    //localparam DEC_MISO_LATENCY = COMB_SECTION_LATENCY+INT_SECTION_LATENCY+2;
    //localparam INT_SIMO_LATENCY = COMB_SECTION_LATENCY+INT_SECTION_LATENCY+MUX_LATENCY ;
    //localparam S_LATENCY = (FILTER_TYPE == "decimator") ? DEC_SISO_LATENCY : INT_SISO_LATENCY;
    //localparam M_LATENCY = (FILTER_TYPE == "decimator") ? DEC_MISO_LATENCY : INT_SIMO_LATENCY;
    //localparam TOTAL_LATENCY = (INTERFACES > 1) ? M_LATENCY : S_LATENCY;
    //localparam CLOCKS_PER_SAMPLE = RCF_MIN/INTERFACES;











input       clk;
input       clken;
input       reset_n;
logic       [RATE_FACTOR_WIDTH-1:0]  rate;  
logic       in_startofpacket;
logic       in_endofpacket;
output      in_ready;
input       in_valid;
output      [CHANNEL_OUT_WIDTH-1:0] out_channel;
output      out_startofpacket;
output      out_endofpacket;
input       [1:0]	  in_error;
output      [1:0]	  out_error;
input       out_ready;
output      out_valid;
 input [IN_WIDTH-1:0]    in0_data;
 input [IN_WIDTH-1:0]    in1_data;
 input [IN_WIDTH-1:0]    in2_data;
 input [IN_WIDTH-1:0]    in3_data;
 input [IN_WIDTH-1:0]    in4_data;
 input [IN_WIDTH-1:0]    in5_data;
output [OUT_WIDTH-1:0]    out_data;

wire [IN_WIDTH-1:0]  din     [INTERFACES_IN-1:0];
wire [OUT_WIDTH-1:0] dout    [INTERFACES_OUT-1:0];





assign din[0] = in0_data;
assign din[1] = in1_data;
assign din[2] = in2_data;
assign din[3] = in3_data;
assign din[4] = in4_data;
assign din[5] = in5_data;

assign out_data = dout[0];
assign       in_startofpacket = 1'b1;
assign       in_endofpacket = 1'b1;
assign   rate = '0;



        alt_cic_core #(
            .DEVICE_FAMILY (DEVICE_FAMILY),
            .FILTER_TYPE (FILTER_TYPE),
            .STAGES (STAGES),
            .D_DELAY (D_DELAY),
            .VRC_EN (VRC_EN),
            .RCF_MAX (RCF_MAX),
            .RCF_MIN (RCF_MIN),
            .INTERFACES (INTERFACES),
            .CH_PER_INT (CH_PER_INT),
            .INT_USE_MEM (INT_USE_MEM),
            .INT_MEM (INT_MEM),
            .DIF_USE_MEM (DIF_USE_MEM),
            .DIF_MEM (DIF_MEM),
            .IN_WIDTH (IN_WIDTH),
            .OUT_WIDTH (OUT_WIDTH),
            .ROUND_TYPE (ROUND_TYPE),
            .PIPELINING (PIPELINING),

            .C_STAGE_0_WIDTH(C_STAGE_0_WIDTH),
            .C_STAGE_1_WIDTH(C_STAGE_1_WIDTH),
            .C_STAGE_2_WIDTH(C_STAGE_2_WIDTH),
            .C_STAGE_3_WIDTH(C_STAGE_3_WIDTH),
            .C_STAGE_4_WIDTH(C_STAGE_4_WIDTH),
            .C_STAGE_5_WIDTH(C_STAGE_5_WIDTH),
            .C_STAGE_6_WIDTH(C_STAGE_6_WIDTH),
            .C_STAGE_7_WIDTH(C_STAGE_7_WIDTH),
            .C_STAGE_8_WIDTH(C_STAGE_8_WIDTH),
            .C_STAGE_9_WIDTH(C_STAGE_9_WIDTH),
            .C_STAGE_10_WIDTH(C_STAGE_10_WIDTH),
            .C_STAGE_11_WIDTH(C_STAGE_11_WIDTH),
            .MAX_C_STAGE_WIDTH(MAX_C_STAGE_WIDTH),

            .I_STAGE_0_WIDTH(I_STAGE_0_WIDTH),
            .I_STAGE_1_WIDTH(I_STAGE_1_WIDTH),
            .I_STAGE_2_WIDTH(I_STAGE_2_WIDTH),
            .I_STAGE_3_WIDTH(I_STAGE_3_WIDTH),
            .I_STAGE_4_WIDTH(I_STAGE_4_WIDTH),
            .I_STAGE_5_WIDTH(I_STAGE_5_WIDTH),
            .I_STAGE_6_WIDTH(I_STAGE_6_WIDTH),
            .I_STAGE_7_WIDTH(I_STAGE_7_WIDTH),
            .I_STAGE_8_WIDTH(I_STAGE_8_WIDTH),
            .I_STAGE_9_WIDTH(I_STAGE_9_WIDTH),
            .I_STAGE_10_WIDTH(I_STAGE_10_WIDTH),
            .I_STAGE_11_WIDTH(I_STAGE_11_WIDTH),
            .MAX_I_STAGE_WIDTH(MAX_I_STAGE_WIDTH)
        ) core (
            .clk(clk),
            .clken(clken),
            .reset_n(reset_n),
            .rate(rate),
            .in_startofpacket(in_startofpacket),
            .in_endofpacket(in_endofpacket),
            .in_data(din),
            .in_ready(in_ready),
            .in_valid(in_valid),
            .out_channel(out_channel),
            .out_startofpacket(out_startofpacket),
            .out_endofpacket(out_endofpacket),
            .in_error(in_error),
            .out_error(out_error),
            .out_ready(out_ready),
            .out_data(dout),
            .out_valid(out_valid)
        );
        defparam core.HYPER_PIPELINE = 0;


endmodule
