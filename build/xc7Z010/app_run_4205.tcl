connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Xilinx Virtual Cable localhost:2542" && level==0 && jtag_device_ctx=="jsn-XVC-localhost:2542-13722093-0"}
fpga -file bitstream_4205.bit
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source ps7_init_4205.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow tc1_xc7z010_app.elf
configparams force-mem-access 0
con