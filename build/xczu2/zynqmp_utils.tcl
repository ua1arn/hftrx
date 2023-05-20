#/******************************************************************************
#*
#* (c) Copyright 2010-2020 Xilinx, Inc. All rights reserved.
#*
#* This file contains confidential and proprietary information of Xilinx, Inc.
#* and is protected under U.S. and international copyright and other
#* intellectual property laws.
#*
#* DISCLAIMER
#* This disclaimer is not a license and does not grant any rights to the
#* materials distributed herewith. Except as otherwise provided in a valid
#* license issued to you by Xilinx, and to the maximum extent permitted by
#* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
#* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
#* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
#* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
#* and (2) Xilinx shall not be liable (whether in contract or tort, including
#* negligence, or under any other theory of liability) for any loss or damage
#* of any kind or nature related to, arising under or in connection with these
#* materials, including for any direct, or any indirect, special, incidental,
#* or consequential loss or damage (including loss of data, profits, goodwill,
#* or any type of loss or damage suffered as a result of any action brought by
#* a third party) even if such damage or loss was reasonably foreseeable or
#* Xilinx had been advised of the possibility of the same.
#*
#* CRITICAL APPLICATIONS
#* Xilinx products are not designed or intended to be fail-safe, or for use in
#* any application requiring fail-safe performance, such as life-support or
#* safety devices or systems, Class III medical devices, nuclear facilities,
#* applications related to the deployment of airbags, or any other applications
#* that could lead to death, personal injury, or severe property or
#* environmental damage (individually and collectively, "Critical
#* Applications"). Customer assumes the sole risk and liability of any use of
#* Xilinx products in Critical Applications, subject only to applicable laws
#* and regulations governing limitations on product liability.
#*
#* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
#* AT ALL TIMES.
#*
#******************************************************************************/


# ta = APU | A53
proc reset_apu { } {
    rst -cores
}

# ta = RPU | R5
proc clear_rpu_reset {} {
    rst -cores
}

# ta = RPU | R5
proc enable_split_mode {} {
    mwr 0xff9a0000 0x208
}

# ta = A53
proc enable_a32_mode { cpunr } {
    if { $cpunr > 3 } { error "invalid cpu number" }
    set val [mrd -force -value 0xfd5c0020]
    mwr -force 0xfd5c0020 [expr {$val & ~(1 << $cpunr)}]
}

proc psu_protection {} {
    # dummy proc for older designs
}

# ta = PSU | APU | RPU
proc disable_pmu_gate {} {
    set status [mrd -force -value 0xFFCA0038]
    set status [expr $status | 0x1C0]
    mwr -force 0xFFCA0038 $status
}

namespace eval zynqmp {
    proc get_pl_ranges {} {
	return [list {0x80000000 0xbfffffff} {0x400000000 0x5ffffffff} {0x1000000000 0x7fffffffff}]
    }
}
