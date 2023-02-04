connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {level==0 && jtag_device_ctx=="jsn-DLC10-13724327082a01-4ba00477-0"}
fpga -file bitstream_ebaz7020.bit
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source ps7_init_ebaz7020.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow tc1_xc7z020_app.elf
configparams force-mem-access 0
con