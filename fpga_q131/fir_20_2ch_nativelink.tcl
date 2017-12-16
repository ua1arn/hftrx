## ================================================================================
## Legal Notice: Copyright (C) 1991-2006 Altera Corporation
## Any megafunction design, and related net list (encrypted or decrypted),
## support information, device programming or simulation file, and any other
## associated documentation or information provided by Altera or a partner
## under Altera's Megafunction Partnership Program may be used only to
## program PLD devices (but not masked PLD devices) from Altera.  Any other
## use of such megafunction design, net list, support information, device
## programming or simulation file, or any other related documentation or
## information is prohibited for any other purpose, including, but not
## limited to modification, reverse engineering, de-compiling, or use with
## any other silicon devices, unless such use is explicitly licensed under
## a separate agreement with Altera or a megafunction partner.  Title to
## the intellectual property, including patents, copyrights, trademarks,
## trade secrets, or maskworks, embodied in any such megafunction design,
## net list, support information, device programming or simulation file, or
## any other related documentation or information provided by Altera or a
## megafunction partner, remains with Altera, the megafunction partner, or
## their respective licensors.  No other licenses, including any licenses
## needed under any third party's intellectual property, are provided herein.
## ================================================================================
##


catch {set testbench_files [glob -nocomplain *.hex]} error_msg
puts "$error_msg"
  
catch {set coef_files [glob -nocomplain fir_20_2ch_coef*.txt]} error_msg
puts "$error_msg"
  
# The top-level in HDL type "VERILOG"
set ipfs_ext vo
if {[file exists fir_20_2ch.vo]} {
	set hdl_ext v
} else {
	puts "Warning: Could not find fir_20_2ch.$ipfs_ext!"
}

set_global_assignment -name EDA_OUTPUT_DATA_FORMAT VERILOG -section_id eda_simulation

# Set test bench name
set_global_assignment -name EDA_TEST_BENCH_NAME tb -section_id eda_simulation


# test bench settings
set_global_assignment -name EDA_DESIGN_INSTANCE_NAME DUT -section_id tb
set_global_assignment -name EDA_TEST_BENCH_MODULE_NAME work.tb_fir_20_2ch -section_id tb
set_global_assignment -name EDA_TEST_BENCH_GATE_LEVEL_NETLIST_LIBRARY work -section_id tb

# IPFS file
set_global_assignment -name EDA_IPFS_FILE fir_20_2ch.$ipfs_ext -section_id eda_simulation -library work

# Add Testbench files
foreach i $testbench_files {
  set_global_assignment -name EDA_TEST_BENCH_FILE $i -section_id tb -library work
}
# Add coef files
foreach i $coef_files {
  set_global_assignment -name EDA_TEST_BENCH_FILE $i -section_id tb -library work
}
set_global_assignment -name EDA_TEST_BENCH_FILE fir_20_2ch_input.txt -section_id tb -library work

set_global_assignment -name EDA_TEST_BENCH_FILE tb_fir_20_2ch.vhd -section_id tb -library work

# Specify testbench mode for nativelink
set_global_assignment -name EDA_TEST_BENCH_ENABLE_STATUS TEST_BENCH_MODE -section_id eda_simulation

# Specify active testbench for nativelink
set_global_assignment -name EDA_NATIVELINK_SIMULATION_TEST_BENCH tb -section_id eda_simulation


