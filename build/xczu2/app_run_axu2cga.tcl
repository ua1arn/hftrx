connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -nocase -filter {name =~"APU*"}
rst -cores 
targets -set -filter {jtag_cable_name =~ "Platform Cable USB II 13724327082a01" && level==0 && jtag_device_ctx=="jsn-DLC10-13724327082a01-14711093-0"}
fpga -file bitstream_alinx_axu2cga.bit
targets -set -nocase -filter {name =~"APU*"}
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source psu_init_axu2cga.tcl
psu_init
after 1000
psu_ps_pl_reset_config
catch {psu_protection}
targets -set -nocase -filter {name =~ "*A53*#0"}
rst -processor
dow tc1_xczu2_app.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A53*#0"}
con
