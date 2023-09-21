connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Platform Cable USB II 13724327082a01" && level==0 && jtag_device_ctx=="jsn-DLC10-13724327082a01-13722093-0"}
fpga -file bitstream_zm_10.bit
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source ps7_init_zm.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow tc1_xc7z010_app.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con