	cic64_3iq_v181 u0 (
		.in_error          (<connected-to-in_error>),          //  av_st_in.error
		.in_valid          (<connected-to-in_valid>),          //          .valid
		.in_ready          (<connected-to-in_ready>),          //          .ready
		.in0_data          (<connected-to-in0_data>),          //          .in0_data
		.in1_data          (<connected-to-in1_data>),          //          .in1_data
		.in2_data          (<connected-to-in2_data>),          //          .in2_data
		.in3_data          (<connected-to-in3_data>),          //          .in3_data
		.in4_data          (<connected-to-in4_data>),          //          .in4_data
		.in5_data          (<connected-to-in5_data>),          //          .in5_data
		.out_data          (<connected-to-out_data>),          // av_st_out.out_data
		.out_error         (<connected-to-out_error>),         //          .error
		.out_valid         (<connected-to-out_valid>),         //          .valid
		.out_ready         (<connected-to-out_ready>),         //          .ready
		.out_startofpacket (<connected-to-out_startofpacket>), //          .startofpacket
		.out_endofpacket   (<connected-to-out_endofpacket>),   //          .endofpacket
		.out_channel       (<connected-to-out_channel>),       //          .channel
		.clk               (<connected-to-clk>),               //     clock.clk
		.reset_n           (<connected-to-reset_n>)            //     reset.reset_n
	);

