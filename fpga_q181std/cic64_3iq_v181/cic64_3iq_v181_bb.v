
module cic64_3iq_v181 (
	in_error,
	in_valid,
	in_ready,
	in0_data,
	in1_data,
	in2_data,
	in3_data,
	in4_data,
	in5_data,
	out_data,
	out_error,
	out_valid,
	out_ready,
	out_startofpacket,
	out_endofpacket,
	out_channel,
	clk,
	reset_n);	

	input	[1:0]	in_error;
	input		in_valid;
	output		in_ready;
	input	[31:0]	in0_data;
	input	[31:0]	in1_data;
	input	[31:0]	in2_data;
	input	[31:0]	in3_data;
	input	[31:0]	in4_data;
	input	[31:0]	in5_data;
	output	[31:0]	out_data;
	output	[1:0]	out_error;
	output		out_valid;
	input		out_ready;
	output		out_startofpacket;
	output		out_endofpacket;
	output	[2:0]	out_channel;
	input		clk;
	input		reset_n;
endmodule
