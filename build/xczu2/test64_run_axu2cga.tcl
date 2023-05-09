connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {level==0 && jtag_device_ctx=="jsn-DLC10-13724327082a01-14711093-0"}
fpga -file bitstream_alinx_axu2cga.bit
targets -set -nocase -filter {name =~"APU*"}
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
set mode [expr [mrd -value 0xFF5E0200] & 0xf]
targets -set -nocase -filter {name =~ "*A53*#0"}
rst -processor
dow fsbl_a53_x64.elf
set bp_42_45_fsbl_bp [bpadd -addr &XFsbl_Exit]
con -block -timeout 60
bpremove $bp_42_45_fsbl_bp
targets -set -nocase -filter {name =~ "*A53*#0"}
rst -processor
dow helloworld_x64.elf
configparams force-mem-access 0
con