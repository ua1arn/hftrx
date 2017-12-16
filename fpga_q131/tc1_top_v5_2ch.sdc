#************************************************************
# THIS IS A WIZARD-GENERATED FILE.                           
#
# Version 13.1.4 Build 182 03/12/2014 SJ Full Version
#
#************************************************************

# Copyright (C) 1991-2014 Altera Corporation
# Your use of Altera Corporation's design tools, logic functions 
# and other software and tools, and its AMPP partner logic 
# functions, and any output files from any of the foregoing 
# (including device programming or simulation files), and any 
# associated documentation or information are expressly subject 
# to the terms and conditions of the Altera Program License 
# Subscription Agreement, Altera MegaCore Function License 
# Agreement, or other applicable license agreement, including, 
# without limitation, that your use is for the sole purpose of 
# programming logic devices manufactured by Altera and sold by 
#************************************************************
# THIS IS A WIZARD-GENERATED FILE.                           
#
# Version 11.1 Build 259 01/25/2012 Service Pack 2 SJ Full Version
#
#************************************************************
create_clock -name "clockfromadc" -period 125MHz [get_ports {adc_clka}]
create_clock -name "ref122880" -period 125MHz [get_ports {refclk_in}]

create_clock -name "sclk_clock" -period 25MHz [get_ports {cpu_sclk}]
create_clock -name "fpga_ctl_cs_clock" -period 25MHz [get_ports {fpga_ctl_cs}]
create_clock -name "fpga_fir_clk_clock" -period 25MHz [get_ports {fpga_fir_clk_n}]
#create_clock -name "sai1_sck_a_clock" -period 13MHz [get_ports {sai1_sck_a}]

create_generated_clock -name "sai1_sck_a_clock" -source [get_clocks {ref122880}] -divide_by 10 -duty_cycle 20.0

set_clock_groups -asynchronous \
	-group { [get_clocks {clockfromadc}] [get_clocks {ref122880}] } \
	-group { [get_clocks {sclk_clock}] } \
	-group { [get_clocks {sai1_sck_a_clock}] } \
	-group { [get_clocks {fpga_ctl_cs_clock}] } \
	-group { [get_clocks {fpga_fir_clk_clock}] }

set_false_path -from [get_clocks {clockfromadc ref122880 sclk_clock sai1_sck_a_clock fpga_ctl_cs_clock fpga_fir_clk_clock}] -to [get_ports {adc_dith adc_rand adc_shdn adc_pga dac_sleep}]
set_false_path -from [get_clocks {clockfromadc ref122880 sclk_clock sai1_sck_a_clock fpga_ctl_cs_clock fpga_fir_clk_clock}] -to [get_ports {led0 led1 led2 led3}]
set_false_path -from [get_clocks {clockfromadc ref122880 sclk_clock sai1_sck_a_clock fpga_ctl_cs_clock fpga_fir_clk_clock}] -to [get_ports {sai1_sd_b}]

# Automatically constrain PLL and other generated clocks
derive_pll_clocks -create_base_clocks

# Automatically calculate clock uncertainty to jitter and other effects.
derive_clock_uncertainty

# tsu/th constraints

# tco constraints

# tpd constraints
#set_input_delay -clock  "clockfromadc"  -min [expr { - 0.8}] [get_ports {adc_data[*] adc_ovfl}]
#set_input_delay -clock  "clockfromadc"  -max [expr { + 0.8}] [get_ports {adc_data[*] adc_ovfl}]

#**************************************************************
# Set Clock Latency
#**************************************************************
set_output_delay -clock [get_clocks {ref122880}] -max 36ps [get_ports {dac_d[*]}]
set_output_delay -clock [get_clocks {ref122880}] -min 30ps [get_ports {dac_d[*]}]

#**************************************************************
# Set Input Delay
#**************************************************************
set_input_delay -clock clockfromadc -max 0ps [get_ports adc_data[*]]
set_input_delay -clock clockfromadc -min -66ps [get_ports adc_data[*]]
set_input_delay -clock clockfromadc -max 0ps [get_ports adc_ovfl]
set_input_delay -clock clockfromadc -min -66ps [get_ports adc_ovfl]

set_false_path -from {t1_2ch_rts96:inst|spislave_sync:inst12|lpm_ff:inst5|dffs[6]} -to {t1_2ch_rts96:inst|cicdec0:inst54|cicdec0_ast:cicdec0_ast_inst|cicdec0_st:fircore|sadd_cen:U_2_sym_add|res[32]}
set_false_path -from {t1_2ch_rts96:inst|spislave_sync:inst12|lpm_ff:inst5|dffs[6]} -to {t1_2ch_rts96:inst|cicdec0:inst54|cicdec0_ast:cicdec0_ast_inst|cicdec0_st:fircore|sadd_cen:U_2_sym_add|res[30]}
set_false_path -from {t1_2ch_rts96:inst|cic_64_3iq:inst33|cic_64_3iq_cic:cic_64_3iq_cic_inst|auk_dspip_avalon_streaming_sink_cic_131:aii_sink|scfifo:\normal_fifo:fifo_eab_on:in_fifo|scfifo_5jh1:auto_generated|a_dpfifo_ut81:dpfifo|altsyncram_a0g1:FIFOram|q_b[130]} -to {t1_2ch_rts96:inst|cic_64_3iq:inst33|cic_64_3iq_cic:cic_64_3iq_cic_inst|cic_64_3iq_cic_core:cic_core|auk_dspip_integrator_cic_131:auk_dspip_integrator_1_0|auk_dspip_delay_cic_131:\glogic:integrator_pipeline_0_generate:u1|\register_fifo:fifo_data[0][84]}
