-- ------------------------------------------------------------------------- 
-- High Level Design Compiler for Intel(R) FPGAs Version 18.1 (Release Build #625)
-- Quartus Prime development tool and MATLAB/Simulink Interface
-- 
-- Legal Notice: Copyright 2018 Intel Corporation.  All rights reserved.
-- Your use of  Intel Corporation's design tools,  logic functions and other
-- software and  tools, and its AMPP partner logic functions, and any output
-- files any  of the foregoing (including  device programming  or simulation
-- files), and  any associated  documentation  or information  are expressly
-- subject  to the terms and  conditions of the  Intel FPGA Software License
-- Agreement, Intel MegaCore Function License Agreement, or other applicable
-- license agreement,  including,  without limitation,  that your use is for
-- the  sole  purpose of  programming  logic devices  manufactured by  Intel
-- and  sold by Intel  or its authorized  distributors. Please refer  to the
-- applicable agreement for further details.
-- ---------------------------------------------------------------------------

-- VHDL created from fir_main_0002_rtl_core
-- VHDL created on Sun Mar 17 03:17:47 2019


library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
use IEEE.MATH_REAL.all;
use std.TextIO.all;
use work.dspba_library_package.all;

LIBRARY altera_mf;
USE altera_mf.altera_mf_components.all;
LIBRARY lpm;
USE lpm.lpm_components.all;

entity fir_main_0002_rtl_core is
    port (
        xIn_v : in std_logic_vector(0 downto 0);  -- sfix1
        xIn_c : in std_logic_vector(7 downto 0);  -- sfix8
        xIn_0 : in std_logic_vector(31 downto 0);  -- sfix32
        xOut_v : out std_logic_vector(0 downto 0);  -- ufix1
        xOut_c : out std_logic_vector(7 downto 0);  -- ufix8
        xOut_0 : out std_logic_vector(61 downto 0);  -- sfix62
        clk : in std_logic;
        areset : in std_logic
    );
end fir_main_0002_rtl_core;

architecture normal of fir_main_0002_rtl_core is

    attribute altera_attribute : string;
    attribute altera_attribute of normal : architecture is "-name AUTO_SHIFT_REGISTER_RECOGNITION OFF; -name PHYSICAL_SYNTHESIS_REGISTER_DUPLICATION ON; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 10037; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 15400; -name MESSAGE_DISABLE 14130; -name MESSAGE_DISABLE 10036; -name MESSAGE_DISABLE 12020; -name MESSAGE_DISABLE 12030; -name MESSAGE_DISABLE 12010; -name MESSAGE_DISABLE 12110; -name MESSAGE_DISABLE 14320; -name MESSAGE_DISABLE 13410; -name MESSAGE_DISABLE 113007";
    
    signal GND_q : STD_LOGIC_VECTOR (0 downto 0);
    signal VCC_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_xIn_0_15_q : STD_LOGIC_VECTOR (31 downto 0);
    signal d_in0_m0_wi0_wo0_assign_id1_q_11_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_in0_m0_wi0_wo0_assign_id1_q_15_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_inputframe_seq_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_inputframe_seq_eq : std_logic;
    signal u0_m0_wo0_run_count : STD_LOGIC_VECTOR (1 downto 0);
    signal u0_m0_wo0_run_preEnaQ : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_run_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_run_out : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_run_enableQ : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_run_ctrl : STD_LOGIC_VECTOR (2 downto 0);
    signal u0_m0_wo0_memread_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_14_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_25_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_26_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_27_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_28_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_29_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_30_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_31_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_32_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_33_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_34_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_memread_q_35_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_compute_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_24_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_25_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_26_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_27_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_28_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_29_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_30_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_31_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_32_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_33_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_34_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_compute_q_35_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count0_inner_q : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count0_inner_i : SIGNED (7 downto 0);
    attribute preserve : boolean;
    attribute preserve of u0_m0_wo0_wi0_r0_ra0_count0_inner_i : signal is true;
    signal u0_m0_wo0_wi0_r0_ra0_count0_q : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count0_i : UNSIGNED (6 downto 0);
    attribute preserve of u0_m0_wo0_wi0_r0_ra0_count0_i : signal is true;
    signal u0_m0_wo0_wi0_r0_ra0_count1_q : STD_LOGIC_VECTOR (1 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count1_i : UNSIGNED (0 downto 0);
    attribute preserve of u0_m0_wo0_wi0_r0_ra0_count1_i : signal is true;
    signal u0_m0_wo0_wi0_r0_ra0_count2_lutreg_q : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count2_inner_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count2_inner_i : SIGNED (0 downto 0);
    attribute preserve of u0_m0_wo0_wi0_r0_ra0_count2_inner_i : signal is true;
    signal u0_m0_wo0_wi0_r0_ra0_count2_inner_eq : std_logic;
    attribute preserve of u0_m0_wo0_wi0_r0_ra0_count2_inner_eq : signal is true;
    signal u0_m0_wo0_wi0_r0_ra0_count2_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count2_i : UNSIGNED (5 downto 0);
    attribute preserve of u0_m0_wo0_wi0_r0_ra0_count2_i : signal is true;
    signal u0_m0_wo0_wi0_r0_ra0_count2_eq : std_logic;
    attribute preserve of u0_m0_wo0_wi0_r0_ra0_count2_eq : signal is true;
    signal u0_m0_wo0_wi0_r0_wa0_q : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_wi0_r0_wa0_i : UNSIGNED (6 downto 0);
    attribute preserve of u0_m0_wo0_wi0_r0_wa0_i : signal is true;
    signal u0_m0_wo0_wi0_r0_memr0_reset0 : std_logic;
    signal u0_m0_wo0_wi0_r0_memr0_ia : STD_LOGIC_VECTOR (31 downto 0);
    signal u0_m0_wo0_wi0_r0_memr0_aa : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_wi0_r0_memr0_ab : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_wi0_r0_memr0_iq : STD_LOGIC_VECTOR (31 downto 0);
    signal u0_m0_wo0_wi0_r0_memr0_q : STD_LOGIC_VECTOR (31 downto 0);
    signal u0_m0_wo0_ca0_inner_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_ca0_inner_i : SIGNED (0 downto 0);
    attribute preserve of u0_m0_wo0_ca0_inner_i : signal is true;
    signal u0_m0_wo0_ca0_inner_eq : std_logic;
    attribute preserve of u0_m0_wo0_ca0_inner_eq : signal is true;
    signal u0_m0_wo0_ca0_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_ca0_i : UNSIGNED (5 downto 0);
    attribute preserve of u0_m0_wo0_ca0_i : signal is true;
    signal u0_m0_wo0_ca0_eq : std_logic;
    attribute preserve of u0_m0_wo0_ca0_eq : signal is true;
    signal u0_m0_wo0_cm0_q : STD_LOGIC_VECTOR (23 downto 0);
    signal u0_m0_wo0_aseq_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_aseq_eq : std_logic;
    signal d_u0_m0_wo0_aseq_q_26_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_aseq_q_27_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_aseq_q_28_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_aseq_q_29_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_aseq_q_30_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_aseq_q_31_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_aseq_q_32_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_aseq_q_33_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_aseq_q_34_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_u0_m0_wo0_aseq_q_35_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_oseq_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_oseq_eq : std_logic;
    signal u0_m0_wo0_oseq_gated_reg_q : STD_LOGIC_VECTOR (0 downto 0);
    signal d_out0_m0_wo0_assign_id3_q_36_q : STD_LOGIC_VECTOR (0 downto 0);
    signal outchan_q : STD_LOGIC_VECTOR (1 downto 0);
    signal outchan_i : UNSIGNED (0 downto 0);
    attribute preserve of outchan_i : signal is true;
    signal d_u0_m0_wo0_mtree_mult1_0_a_0_b_21_q : STD_LOGIC_VECTOR (16 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_a_1_b_22_q : STD_LOGIC_VECTOR (16 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_b_0_b_21_q : STD_LOGIC_VECTOR (16 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_b_1_b_22_q : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a0_b0_a0 : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a0_b0_b0 : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a0_b0_s1 : STD_LOGIC_VECTOR (33 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a0_b0_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_a0_b0_q : STD_LOGIC_VECTOR (33 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a1_b0_a0 : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a1_b0_b0 : STD_LOGIC_VECTOR (17 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a1_b0_s1 : STD_LOGIC_VECTOR (34 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a1_b0_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_a1_b0_q : STD_LOGIC_VECTOR (33 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a0_b1_a0 : STD_LOGIC_VECTOR (17 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a0_b1_b0 : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a0_b1_s1 : STD_LOGIC_VECTOR (34 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a0_b1_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_a0_b1_q : STD_LOGIC_VECTOR (33 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a1_b1_a0 : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a1_b1_b0 : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a1_b1_s1 : STD_LOGIC_VECTOR (33 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a1_b1_reset : std_logic;
    signal u0_m0_wo0_mtree_mult1_0_a1_b1_q : STD_LOGIC_VECTOR (33 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_UpperBits_for_b_q : STD_LOGIC_VECTOR (1 downto 0);
    signal d_u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_a_c_14_q : STD_LOGIC_VECTOR (3 downto 0);
    signal d_u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_b_c_14_q : STD_LOGIC_VECTOR (3 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_a : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_b : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_o : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_q_15_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_a : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_b : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_o : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_q : STD_LOGIC_VECTOR (3 downto 0);
    signal u0_m0_wo0_accum_p1_of_11_a : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_accum_p1_of_11_b : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_accum_p1_of_11_i : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_accum_p1_of_11_o : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_accum_p1_of_11_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p1_of_11_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_accum_p1_of_11_q_36_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_accum_p2_of_11_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p2_of_11_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p2_of_11_i : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p2_of_11_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p2_of_11_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p2_of_11_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p2_of_11_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_accum_p2_of_11_q_36_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_accum_p3_of_11_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p3_of_11_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p3_of_11_i : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p3_of_11_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p3_of_11_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p3_of_11_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p3_of_11_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_accum_p3_of_11_q_36_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_accum_p4_of_11_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p4_of_11_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p4_of_11_i : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p4_of_11_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p4_of_11_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p4_of_11_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p4_of_11_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_accum_p4_of_11_q_36_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_accum_p5_of_11_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p5_of_11_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p5_of_11_i : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p5_of_11_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p5_of_11_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p5_of_11_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p5_of_11_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_accum_p5_of_11_q_36_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_accum_p6_of_11_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p6_of_11_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p6_of_11_i : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p6_of_11_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p6_of_11_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p6_of_11_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p6_of_11_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_accum_p6_of_11_q_36_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_accum_p7_of_11_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p7_of_11_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p7_of_11_i : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p7_of_11_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p7_of_11_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p7_of_11_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p7_of_11_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_accum_p7_of_11_q_36_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_accum_p8_of_11_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p8_of_11_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p8_of_11_i : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p8_of_11_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p8_of_11_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p8_of_11_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p8_of_11_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_accum_p8_of_11_q_36_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_accum_p9_of_11_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p9_of_11_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p9_of_11_i : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p9_of_11_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p9_of_11_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p9_of_11_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p9_of_11_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_accum_p9_of_11_q_36_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_accum_p10_of_11_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p10_of_11_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p10_of_11_i : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p10_of_11_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_accum_p10_of_11_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p10_of_11_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p10_of_11_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_accum_p10_of_11_q_36_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_accum_p11_of_11_a : STD_LOGIC_VECTOR (3 downto 0);
    signal u0_m0_wo0_accum_p11_of_11_b : STD_LOGIC_VECTOR (3 downto 0);
    signal u0_m0_wo0_accum_p11_of_11_i : STD_LOGIC_VECTOR (3 downto 0);
    signal u0_m0_wo0_accum_p11_of_11_o : STD_LOGIC_VECTOR (3 downto 0);
    signal u0_m0_wo0_accum_p11_of_11_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_p11_of_11_q : STD_LOGIC_VECTOR (1 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_g_22_q : STD_LOGIC_VECTOR (4 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_f_21_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_e_20_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_d_19_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_c_18_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_g_22_q : STD_LOGIC_VECTOR (4 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_f_21_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_e_20_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_d_19_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_c_18_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_a : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_b : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_o : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_q_23_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_q_23_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_q_23_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_q_23_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_q_23_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_a : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_b : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_o : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_q : STD_LOGIC_VECTOR (4 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_UpperBits_for_a_q : STD_LOGIC_VECTOR (18 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_g_28_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_f_27_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_e_26_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_d_25_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_c_24_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_j_31_q : STD_LOGIC_VECTOR (4 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_i_30_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_h_29_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_g_28_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_f_27_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_e_26_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_d_25_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_c_24_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_a : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_b : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_o : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_a : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_b : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_o : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_q : STD_LOGIC_VECTOR (4 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_q_33_q : STD_LOGIC_VECTOR (4 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_k_33_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_j_32_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_i_31_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_h_30_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_g_29_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_f_28_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_e_27_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_d_26_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_c_25_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_a : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_b : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_o : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_c : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_a : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_b : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_o : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_cin : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_q : STD_LOGIC_VECTOR (5 downto 0);
    signal d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_q_35_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_adelay_p0_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_adelay_p1_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_adelay_p2_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_adelay_p3_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_adelay_p4_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_adelay_p5_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_adelay_p6_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_adelay_p7_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_adelay_p8_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_adelay_p9_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_adelay_p10_q : STD_LOGIC_VECTOR (1 downto 0);
    signal u0_m0_wo0_inputframe_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count0_run_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count2_run_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_ca0_run_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_oseq_gated_q : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_a_in : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_a_b : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_b_in : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_b_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_BitExpansion_for_b_q : STD_LOGIC_VECTOR (9 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_count2_lut_q : STD_LOGIC_VECTOR (7 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_b_0_in : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_b_0_b : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_b_1_in : STD_LOGIC_VECTOR (33 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_b_1_b : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a_0_in : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a_0_b : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a_1_in : STD_LOGIC_VECTOR (33 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_a_1_b : STD_LOGIC_VECTOR (16 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_0_q : STD_LOGIC_VECTOR (33 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_0_qint : STD_LOGIC_VECTOR (33 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_SignBit_for_a_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_a_q : STD_LOGIC_VECTOR (34 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_SignBit_for_b_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_b_q : STD_LOGIC_VECTOR (34 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_2_q : STD_LOGIC_VECTOR (67 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_2_qint : STD_LOGIC_VECTOR (67 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_BitJoin_for_q_q : STD_LOGIC_VECTOR (9 downto 0);
    signal u0_m0_wo0_accum_BitJoin_for_q_q : STD_LOGIC_VECTOR (61 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitJoin_for_q_q : STD_LOGIC_VECTOR (34 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_a_q : STD_LOGIC_VECTOR (52 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel6_0_merged_bit_select_b : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel6_0_merged_bit_select_c : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel6_0_merged_bit_select_d : STD_LOGIC_VECTOR (4 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel8_1_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_BitJoin_for_j_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel9_0_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_BitSelect_for_a_tessel9_0_b : STD_LOGIC_VECTOR (1 downto 0);
    signal u0_m0_wo0_accum_BitSelect_for_a_tessel9_1_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_accum_BitSelect_for_a_tessel10_0_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_join_q : STD_LOGIC_VECTOR (8 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_b_b : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_b_c : STD_LOGIC_VECTOR (3 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_b : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_c : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_d : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_e : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_f : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_g : STD_LOGIC_VECTOR (4 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_b : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_c : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_d : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_e : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_f : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_g : STD_LOGIC_VECTOR (4 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_SignBit_for_b_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitExpansion_for_b_q : STD_LOGIC_VECTOR (68 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_resize_in : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_resize_b : STD_LOGIC_VECTOR (6 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_1_q : STD_LOGIC_VECTOR (51 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_align_1_qint : STD_LOGIC_VECTOR (51 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_b : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_c : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_d : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_e : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_f : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_g : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_BitJoin_for_k_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_k_q : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_l_q : STD_LOGIC_VECTOR (1 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_BitExpansion_for_a_q : STD_LOGIC_VECTOR (9 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_b : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_c : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_d : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_e : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_f : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_g : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_h : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_i : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_j : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_k : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_SignBit_for_b_b : STD_LOGIC_VECTOR (0 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_b_q : STD_LOGIC_VECTOR (52 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_a_b : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_a_c : STD_LOGIC_VECTOR (3 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_b : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_c : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_d : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_e : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_f : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_g : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_h : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_i : STD_LOGIC_VECTOR (5 downto 0);
    signal u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_j : STD_LOGIC_VECTOR (4 downto 0);

begin


    -- GND(CONSTANT,0)@0
    GND_q <= "0";

    -- VCC(CONSTANT,1)@0
    VCC_q <= "1";

    -- d_in0_m0_wi0_wo0_assign_id1_q_11(DELAY,312)@10 + 1
    d_in0_m0_wi0_wo0_assign_id1_q_11 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => xIn_v, xout => d_in0_m0_wi0_wo0_assign_id1_q_11_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_inputframe_seq(SEQUENCE,13)@10 + 1
    u0_m0_wo0_inputframe_seq_clkproc: PROCESS (clk, areset)
        variable u0_m0_wo0_inputframe_seq_c : SIGNED(3 downto 0);
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_inputframe_seq_c := "0000";
            u0_m0_wo0_inputframe_seq_q <= "0";
            u0_m0_wo0_inputframe_seq_eq <= '0';
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (xIn_v = "1") THEN
                IF (u0_m0_wo0_inputframe_seq_c = "0000") THEN
                    u0_m0_wo0_inputframe_seq_eq <= '1';
                ELSE
                    u0_m0_wo0_inputframe_seq_eq <= '0';
                END IF;
                IF (u0_m0_wo0_inputframe_seq_eq = '1') THEN
                    u0_m0_wo0_inputframe_seq_c := u0_m0_wo0_inputframe_seq_c + 1;
                ELSE
                    u0_m0_wo0_inputframe_seq_c := u0_m0_wo0_inputframe_seq_c - 1;
                END IF;
                u0_m0_wo0_inputframe_seq_q <= STD_LOGIC_VECTOR(u0_m0_wo0_inputframe_seq_c(3 downto 3));
            END IF;
        END IF;
    END PROCESS;

    -- u0_m0_wo0_inputframe(LOGICAL,14)@11
    u0_m0_wo0_inputframe_q <= u0_m0_wo0_inputframe_seq_q and d_in0_m0_wi0_wo0_assign_id1_q_11_q;

    -- u0_m0_wo0_run(ENABLEGENERATOR,15)@11 + 2
    u0_m0_wo0_run_ctrl <= u0_m0_wo0_run_out & u0_m0_wo0_inputframe_q & u0_m0_wo0_run_enableQ;
    u0_m0_wo0_run_clkproc: PROCESS (clk, areset)
        variable u0_m0_wo0_run_enable_c : SIGNED(7 downto 0);
        variable u0_m0_wo0_run_inc : SIGNED(1 downto 0);
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_run_q <= "0";
            u0_m0_wo0_run_enable_c := TO_SIGNED(72, 8);
            u0_m0_wo0_run_enableQ <= "0";
            u0_m0_wo0_run_count <= "00";
            u0_m0_wo0_run_inc := (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (u0_m0_wo0_run_out = "1") THEN
                IF (u0_m0_wo0_run_enable_c(7) = '1') THEN
                    u0_m0_wo0_run_enable_c := u0_m0_wo0_run_enable_c - (-73);
                ELSE
                    u0_m0_wo0_run_enable_c := u0_m0_wo0_run_enable_c + (-1);
                END IF;
                u0_m0_wo0_run_enableQ <= STD_LOGIC_VECTOR(u0_m0_wo0_run_enable_c(7 downto 7));
            ELSE
                u0_m0_wo0_run_enableQ <= "0";
            END IF;
            CASE (u0_m0_wo0_run_ctrl) IS
                WHEN "000" | "001" => u0_m0_wo0_run_inc := "00";
                WHEN "010" | "011" => u0_m0_wo0_run_inc := "11";
                WHEN "100" => u0_m0_wo0_run_inc := "00";
                WHEN "101" => u0_m0_wo0_run_inc := "01";
                WHEN "110" => u0_m0_wo0_run_inc := "11";
                WHEN "111" => u0_m0_wo0_run_inc := "00";
                WHEN OTHERS => 
            END CASE;
            u0_m0_wo0_run_count <= STD_LOGIC_VECTOR(SIGNED(u0_m0_wo0_run_count) + SIGNED(u0_m0_wo0_run_inc));
            u0_m0_wo0_run_q <= u0_m0_wo0_run_out;
        END IF;
    END PROCESS;
    u0_m0_wo0_run_preEnaQ <= u0_m0_wo0_run_count(1 downto 1);
    u0_m0_wo0_run_out <= u0_m0_wo0_run_preEnaQ and VCC_q;

    -- u0_m0_wo0_memread(DELAY,16)@13
    u0_m0_wo0_memread : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_run_q, xout => u0_m0_wo0_memread_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_memread_q_14(DELAY,314)@13 + 1
    d_u0_m0_wo0_memread_q_14 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_memread_q, xout => d_u0_m0_wo0_memread_q_14_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_compute(DELAY,18)@14
    u0_m0_wo0_compute : dspba_delay
    GENERIC MAP ( width => 1, depth => 2, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_14_q, xout => u0_m0_wo0_compute_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_compute_q_24(DELAY,326)@14 + 10
    d_u0_m0_wo0_compute_q_24 : dspba_delay
    GENERIC MAP ( width => 1, depth => 10, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_compute_q, xout => d_u0_m0_wo0_compute_q_24_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_aseq(SEQUENCE,44)@24 + 1
    u0_m0_wo0_aseq_clkproc: PROCESS (clk, areset)
        variable u0_m0_wo0_aseq_c : SIGNED(8 downto 0);
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_aseq_c := "000000000";
            u0_m0_wo0_aseq_q <= "0";
            u0_m0_wo0_aseq_eq <= '0';
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_24_q = "1") THEN
                IF (u0_m0_wo0_aseq_c = "111111111") THEN
                    u0_m0_wo0_aseq_eq <= '1';
                ELSE
                    u0_m0_wo0_aseq_eq <= '0';
                END IF;
                IF (u0_m0_wo0_aseq_eq = '1') THEN
                    u0_m0_wo0_aseq_c := u0_m0_wo0_aseq_c + 73;
                ELSE
                    u0_m0_wo0_aseq_c := u0_m0_wo0_aseq_c - 1;
                END IF;
                u0_m0_wo0_aseq_q <= STD_LOGIC_VECTOR(u0_m0_wo0_aseq_c(8 downto 8));
            END IF;
        END IF;
    END PROCESS;

    -- d_u0_m0_wo0_compute_q_25(DELAY,327)@24 + 1
    d_u0_m0_wo0_compute_q_25 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_compute_q_24_q, xout => d_u0_m0_wo0_compute_q_25_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_memread_q_25(DELAY,315)@14 + 11
    d_u0_m0_wo0_memread_q_25 : dspba_delay
    GENERIC MAP ( width => 1, depth => 11, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_14_q, xout => d_u0_m0_wo0_memread_q_25_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_adelay_p0(DELAY,157)@25
    u0_m0_wo0_adelay_p0 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "NONE" )
    PORT MAP ( xin => u0_m0_wo0_accum_p1_of_11_q, xout => u0_m0_wo0_adelay_p0_q, ena => d_u0_m0_wo0_compute_q_25_q(0), clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_SignBit_for_b(BITSELECT,139)@24
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_SignBit_for_b_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_align_2_q(67 downto 67));

    -- u0_m0_wo0_wi0_r0_ra0_add_1_0_UpperBits_for_b(CONSTANT,76)@0
    u0_m0_wo0_wi0_r0_ra0_add_1_0_UpperBits_for_b_q <= "00";

    -- u0_m0_wo0_wi0_r0_ra0_count2_inner(COUNTER,27)@13
    -- low=-1, high=0, step=1, init=-1
    u0_m0_wo0_wi0_r0_ra0_count2_inner_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_count2_inner_i <= TO_SIGNED(-1, 1);
            u0_m0_wo0_wi0_r0_ra0_count2_inner_eq <= '0';
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (u0_m0_wo0_memread_q = "1") THEN
                IF (u0_m0_wo0_wi0_r0_ra0_count2_inner_eq = '0') THEN
                    u0_m0_wo0_wi0_r0_ra0_count2_inner_eq <= '1';
                ELSE
                    u0_m0_wo0_wi0_r0_ra0_count2_inner_eq <= '0';
                END IF;
                IF (u0_m0_wo0_wi0_r0_ra0_count2_inner_eq = '1') THEN
                    u0_m0_wo0_wi0_r0_ra0_count2_inner_i <= u0_m0_wo0_wi0_r0_ra0_count2_inner_i + 1;
                ELSE
                    u0_m0_wo0_wi0_r0_ra0_count2_inner_i <= u0_m0_wo0_wi0_r0_ra0_count2_inner_i + 1;
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_wi0_r0_ra0_count2_inner_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(u0_m0_wo0_wi0_r0_ra0_count2_inner_i, 1)));

    -- u0_m0_wo0_wi0_r0_ra0_count2_run(LOGICAL,28)@13
    u0_m0_wo0_wi0_r0_ra0_count2_run_q <= STD_LOGIC_VECTOR(u0_m0_wo0_wi0_r0_ra0_count2_inner_q(0 downto 0));

    -- u0_m0_wo0_wi0_r0_ra0_count2(COUNTER,29)@13
    -- low=0, high=36, step=1, init=0
    u0_m0_wo0_wi0_r0_ra0_count2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_count2_i <= TO_UNSIGNED(0, 6);
            u0_m0_wo0_wi0_r0_ra0_count2_eq <= '0';
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (u0_m0_wo0_memread_q = "1" and u0_m0_wo0_wi0_r0_ra0_count2_run_q = "1") THEN
                IF (u0_m0_wo0_wi0_r0_ra0_count2_i = TO_UNSIGNED(35, 6)) THEN
                    u0_m0_wo0_wi0_r0_ra0_count2_eq <= '1';
                ELSE
                    u0_m0_wo0_wi0_r0_ra0_count2_eq <= '0';
                END IF;
                IF (u0_m0_wo0_wi0_r0_ra0_count2_eq = '1') THEN
                    u0_m0_wo0_wi0_r0_ra0_count2_i <= u0_m0_wo0_wi0_r0_ra0_count2_i + 28;
                ELSE
                    u0_m0_wo0_wi0_r0_ra0_count2_i <= u0_m0_wo0_wi0_r0_ra0_count2_i + 1;
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_wi0_r0_ra0_count2_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(u0_m0_wo0_wi0_r0_ra0_count2_i, 6)));

    -- u0_m0_wo0_wi0_r0_ra0_count2_lut(LOOKUP,25)@13
    u0_m0_wo0_wi0_r0_ra0_count2_lut_combproc: PROCESS (u0_m0_wo0_wi0_r0_ra0_count2_q)
    BEGIN
        -- Begin reserved scope level
        CASE (u0_m0_wo0_wi0_r0_ra0_count2_q) IS
            WHEN "000000" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01001010";
            WHEN "000001" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01001100";
            WHEN "000010" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01001110";
            WHEN "000011" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01010000";
            WHEN "000100" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01010010";
            WHEN "000101" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01010100";
            WHEN "000110" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01010110";
            WHEN "000111" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01011000";
            WHEN "001000" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01011010";
            WHEN "001001" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01011100";
            WHEN "001010" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01011110";
            WHEN "001011" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01100000";
            WHEN "001100" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01100010";
            WHEN "001101" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01100100";
            WHEN "001110" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01100110";
            WHEN "001111" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01101000";
            WHEN "010000" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01101010";
            WHEN "010001" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01101100";
            WHEN "010010" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01101110";
            WHEN "010011" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01110000";
            WHEN "010100" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01110010";
            WHEN "010101" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01110100";
            WHEN "010110" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01110110";
            WHEN "010111" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01111000";
            WHEN "011000" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01111010";
            WHEN "011001" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01111100";
            WHEN "011010" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "01111110";
            WHEN "011011" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "00000000";
            WHEN "011100" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "00000010";
            WHEN "011101" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "00000100";
            WHEN "011110" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "00000110";
            WHEN "011111" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "00001000";
            WHEN "100000" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "00001010";
            WHEN "100001" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "00001100";
            WHEN "100010" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "00001110";
            WHEN "100011" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "00010000";
            WHEN "100100" => u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= "00010010";
            WHEN OTHERS => -- unreachable
                           u0_m0_wo0_wi0_r0_ra0_count2_lut_q <= (others => '-');
        END CASE;
        -- End reserved scope level
    END PROCESS;

    -- u0_m0_wo0_wi0_r0_ra0_count2_lutreg(REG,26)@13
    u0_m0_wo0_wi0_r0_ra0_count2_lutreg_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_count2_lutreg_q <= "01001010";
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (u0_m0_wo0_memread_q = "1") THEN
                u0_m0_wo0_wi0_r0_ra0_count2_lutreg_q <= STD_LOGIC_VECTOR(u0_m0_wo0_wi0_r0_ra0_count2_lut_q);
            END IF;
        END IF;
    END PROCESS;

    -- u0_m0_wo0_wi0_r0_ra0_add_1_0_BitExpansion_for_b(BITJOIN,75)@13
    u0_m0_wo0_wi0_r0_ra0_add_1_0_BitExpansion_for_b_q <= u0_m0_wo0_wi0_r0_ra0_add_1_0_UpperBits_for_b_q & u0_m0_wo0_wi0_r0_ra0_count2_lutreg_q;

    -- u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_b(BITSELECT,78)@13
    u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_b_b <= u0_m0_wo0_wi0_r0_ra0_add_1_0_BitExpansion_for_b_q(5 downto 0);
    u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_b_c <= u0_m0_wo0_wi0_r0_ra0_add_1_0_BitExpansion_for_b_q(9 downto 6);

    -- u0_m0_wo0_wi0_r0_ra0_count0_inner(COUNTER,21)@13
    -- low=-1, high=72, step=-1, init=72
    u0_m0_wo0_wi0_r0_ra0_count0_inner_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_count0_inner_i <= TO_SIGNED(72, 8);
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (u0_m0_wo0_memread_q = "1") THEN
                IF (u0_m0_wo0_wi0_r0_ra0_count0_inner_i(7 downto 7) = "1") THEN
                    u0_m0_wo0_wi0_r0_ra0_count0_inner_i <= u0_m0_wo0_wi0_r0_ra0_count0_inner_i - 183;
                ELSE
                    u0_m0_wo0_wi0_r0_ra0_count0_inner_i <= u0_m0_wo0_wi0_r0_ra0_count0_inner_i - 1;
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_wi0_r0_ra0_count0_inner_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(u0_m0_wo0_wi0_r0_ra0_count0_inner_i, 8)));

    -- u0_m0_wo0_wi0_r0_ra0_count0_run(LOGICAL,22)@13
    u0_m0_wo0_wi0_r0_ra0_count0_run_q <= STD_LOGIC_VECTOR(u0_m0_wo0_wi0_r0_ra0_count0_inner_q(7 downto 7));

    -- u0_m0_wo0_wi0_r0_ra0_count0(COUNTER,23)@13
    -- low=0, high=127, step=2, init=0
    u0_m0_wo0_wi0_r0_ra0_count0_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_count0_i <= TO_UNSIGNED(0, 7);
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (u0_m0_wo0_memread_q = "1" and u0_m0_wo0_wi0_r0_ra0_count0_run_q = "1") THEN
                u0_m0_wo0_wi0_r0_ra0_count0_i <= u0_m0_wo0_wi0_r0_ra0_count0_i + 2;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_wi0_r0_ra0_count0_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(u0_m0_wo0_wi0_r0_ra0_count0_i, 8)));

    -- u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_a(BITSELECT,70)@13
    u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_a_in <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((8 downto 8 => u0_m0_wo0_wi0_r0_ra0_count0_q(7)) & u0_m0_wo0_wi0_r0_ra0_count0_q));
    u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_a_b <= STD_LOGIC_VECTOR(u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_a_in(6 downto 1));

    -- u0_m0_wo0_wi0_r0_ra0_count1(COUNTER,24)@13
    -- low=0, high=1, step=1, init=0
    u0_m0_wo0_wi0_r0_ra0_count1_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_count1_i <= TO_UNSIGNED(0, 1);
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (u0_m0_wo0_memread_q = "1") THEN
                u0_m0_wo0_wi0_r0_ra0_count1_i <= u0_m0_wo0_wi0_r0_ra0_count1_i + 1;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_wi0_r0_ra0_count1_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(u0_m0_wo0_wi0_r0_ra0_count1_i, 2)));

    -- u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_b(BITSELECT,71)@13
    u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_b_in <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((8 downto 2 => u0_m0_wo0_wi0_r0_ra0_count1_q(1)) & u0_m0_wo0_wi0_r0_ra0_count1_q));
    u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_b_b <= STD_LOGIC_VECTOR(u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_b_in(0 downto 0));

    -- u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_join(BITJOIN,72)@13
    u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_join_q <= GND_q & GND_q & u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_a_b & u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_BitSelect_for_b_b;

    -- u0_m0_wo0_wi0_r0_ra0_add_1_0_BitExpansion_for_a(BITJOIN,73)@13
    u0_m0_wo0_wi0_r0_ra0_add_1_0_BitExpansion_for_a_q <= GND_q & u0_m0_wo0_wi0_r0_ra0_add_0_0_replace_or_join_q;

    -- u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_a(BITSELECT,77)@13
    u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_a_b <= u0_m0_wo0_wi0_r0_ra0_add_1_0_BitExpansion_for_a_q(5 downto 0);
    u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_a_c <= u0_m0_wo0_wi0_r0_ra0_add_1_0_BitExpansion_for_a_q(9 downto 6);

    -- u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2(ADD,79)@13 + 1
    u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_a_b);
    u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_b_b);
    u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_a) + UNSIGNED(u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_c(0) <= u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_o(6);
    u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_q <= u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_o(5 downto 0);

    -- d_u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_b_c_14(DELAY,354)@13 + 1
    d_u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_b_c_14 : dspba_delay
    GENERIC MAP ( width => 4, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_b_c, xout => d_u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_b_c_14_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_a_c_14(DELAY,353)@13 + 1
    d_u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_a_c_14 : dspba_delay
    GENERIC MAP ( width => 4, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_a_c, xout => d_u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_a_c_14_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2(ADD,80)@14 + 1
    u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_cin <= u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_c;
    u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_a <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_a_c_14_q) & '1';
    u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_wi0_r0_ra0_add_1_0_BitSelect_for_b_c_14_q) & u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_cin(0);
    u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_a) + UNSIGNED(u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_q <= u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_o(4 downto 1);

    -- d_u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_q_15(DELAY,355)@14 + 1
    d_u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_q_15 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_q, xout => d_u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_q_15_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_wi0_r0_ra0_add_1_0_BitJoin_for_q(BITJOIN,81)@15
    u0_m0_wo0_wi0_r0_ra0_add_1_0_BitJoin_for_q_q <= u0_m0_wo0_wi0_r0_ra0_add_1_0_p2_of_2_q & d_u0_m0_wo0_wi0_r0_ra0_add_1_0_p1_of_2_q_15_q;

    -- u0_m0_wo0_wi0_r0_ra0_resize(BITSELECT,32)@15
    u0_m0_wo0_wi0_r0_ra0_resize_in <= STD_LOGIC_VECTOR(u0_m0_wo0_wi0_r0_ra0_add_1_0_BitJoin_for_q_q(6 downto 0));
    u0_m0_wo0_wi0_r0_ra0_resize_b <= STD_LOGIC_VECTOR(u0_m0_wo0_wi0_r0_ra0_resize_in(6 downto 0));

    -- d_xIn_0_15(DELAY,311)@10 + 5
    d_xIn_0_15 : dspba_delay
    GENERIC MAP ( width => 32, depth => 5, reset_kind => "ASYNC" )
    PORT MAP ( xin => xIn_0, xout => d_xIn_0_15_q, clk => clk, aclr => areset );

    -- d_in0_m0_wi0_wo0_assign_id1_q_15(DELAY,313)@11 + 4
    d_in0_m0_wi0_wo0_assign_id1_q_15 : dspba_delay
    GENERIC MAP ( width => 1, depth => 4, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_in0_m0_wi0_wo0_assign_id1_q_11_q, xout => d_in0_m0_wi0_wo0_assign_id1_q_15_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_wi0_r0_wa0(COUNTER,33)@15
    -- low=0, high=127, step=1, init=18
    u0_m0_wo0_wi0_r0_wa0_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_wi0_r0_wa0_i <= TO_UNSIGNED(18, 7);
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_in0_m0_wi0_wo0_assign_id1_q_15_q = "1") THEN
                u0_m0_wo0_wi0_r0_wa0_i <= u0_m0_wo0_wi0_r0_wa0_i + 1;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_wi0_r0_wa0_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(u0_m0_wo0_wi0_r0_wa0_i, 7)));

    -- u0_m0_wo0_wi0_r0_memr0(DUALMEM,34)@15
    u0_m0_wo0_wi0_r0_memr0_ia <= STD_LOGIC_VECTOR(d_xIn_0_15_q);
    u0_m0_wo0_wi0_r0_memr0_aa <= u0_m0_wo0_wi0_r0_wa0_q;
    u0_m0_wo0_wi0_r0_memr0_ab <= u0_m0_wo0_wi0_r0_ra0_resize_b;
    u0_m0_wo0_wi0_r0_memr0_dmem : altsyncram
    GENERIC MAP (
        ram_block_type => "M9K",
        operation_mode => "DUAL_PORT",
        width_a => 32,
        widthad_a => 7,
        numwords_a => 128,
        width_b => 32,
        widthad_b => 7,
        numwords_b => 128,
        lpm_type => "altsyncram",
        width_byteena_a => 1,
        address_reg_b => "CLOCK0",
        indata_reg_b => "CLOCK0",
        wrcontrol_wraddress_reg_b => "CLOCK0",
        rdcontrol_reg_b => "CLOCK0",
        byteena_reg_b => "CLOCK0",
        outdata_reg_b => "CLOCK0",
        outdata_aclr_b => "NONE",
        clock_enable_input_a => "NORMAL",
        clock_enable_input_b => "NORMAL",
        clock_enable_output_b => "NORMAL",
        read_during_write_mode_mixed_ports => "DONT_CARE",
        power_up_uninitialized => "FALSE",
        init_file => "UNUSED",
        intended_device_family => "Cyclone IV E"
    )
    PORT MAP (
        clocken0 => '1',
        clock0 => clk,
        address_a => u0_m0_wo0_wi0_r0_memr0_aa,
        data_a => u0_m0_wo0_wi0_r0_memr0_ia,
        wren_a => d_in0_m0_wi0_wo0_assign_id1_q_15_q(0),
        address_b => u0_m0_wo0_wi0_r0_memr0_ab,
        q_b => u0_m0_wo0_wi0_r0_memr0_iq
    );
    u0_m0_wo0_wi0_r0_memr0_q <= u0_m0_wo0_wi0_r0_memr0_iq(31 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_b_1(BITSELECT,59)@15
    u0_m0_wo0_mtree_mult1_0_b_1_in <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((33 downto 32 => u0_m0_wo0_wi0_r0_memr0_q(31)) & u0_m0_wo0_wi0_r0_memr0_q));
    u0_m0_wo0_mtree_mult1_0_b_1_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_b_1_in(33 downto 17));

    -- d_u0_m0_wo0_mtree_mult1_0_b_1_b_22(DELAY,352)@15 + 7
    d_u0_m0_wo0_mtree_mult1_0_b_1_b_22 : dspba_delay
    GENERIC MAP ( width => 17, depth => 7, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_b_1_b, xout => d_u0_m0_wo0_mtree_mult1_0_b_1_b_22_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_ca0_inner(COUNTER,35)@14
    -- low=-1, high=0, step=1, init=0
    u0_m0_wo0_ca0_inner_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_ca0_inner_i <= TO_SIGNED(0, 1);
            u0_m0_wo0_ca0_inner_eq <= '1';
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (u0_m0_wo0_compute_q = "1") THEN
                IF (u0_m0_wo0_ca0_inner_eq = '0') THEN
                    u0_m0_wo0_ca0_inner_eq <= '1';
                ELSE
                    u0_m0_wo0_ca0_inner_eq <= '0';
                END IF;
                IF (u0_m0_wo0_ca0_inner_eq = '1') THEN
                    u0_m0_wo0_ca0_inner_i <= u0_m0_wo0_ca0_inner_i + 1;
                ELSE
                    u0_m0_wo0_ca0_inner_i <= u0_m0_wo0_ca0_inner_i + 1;
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_ca0_inner_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(u0_m0_wo0_ca0_inner_i, 1)));

    -- u0_m0_wo0_ca0_run(LOGICAL,36)@14
    u0_m0_wo0_ca0_run_q <= STD_LOGIC_VECTOR(u0_m0_wo0_ca0_inner_q(0 downto 0));

    -- u0_m0_wo0_ca0(COUNTER,37)@14
    -- low=0, high=36, step=1, init=0
    u0_m0_wo0_ca0_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_ca0_i <= TO_UNSIGNED(0, 6);
            u0_m0_wo0_ca0_eq <= '0';
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (u0_m0_wo0_compute_q = "1" and u0_m0_wo0_ca0_run_q = "1") THEN
                IF (u0_m0_wo0_ca0_i = TO_UNSIGNED(35, 6)) THEN
                    u0_m0_wo0_ca0_eq <= '1';
                ELSE
                    u0_m0_wo0_ca0_eq <= '0';
                END IF;
                IF (u0_m0_wo0_ca0_eq = '1') THEN
                    u0_m0_wo0_ca0_i <= u0_m0_wo0_ca0_i + 28;
                ELSE
                    u0_m0_wo0_ca0_i <= u0_m0_wo0_ca0_i + 1;
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_ca0_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(u0_m0_wo0_ca0_i, 6)));

    -- u0_m0_wo0_cm0(LOOKUP,41)@14 + 1
    u0_m0_wo0_cm0_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_cm0_q <= "000010010000110101010011";
        ELSIF (clk'EVENT AND clk = '1') THEN
            CASE (u0_m0_wo0_ca0_q) IS
                WHEN "000000" => u0_m0_wo0_cm0_q <= "000010010000110101010011";
                WHEN "000001" => u0_m0_wo0_cm0_q <= "000001101100011100000011";
                WHEN "000010" => u0_m0_wo0_cm0_q <= "000000000000000000000000";
                WHEN "000011" => u0_m0_wo0_cm0_q <= "111110000101000110010110";
                WHEN "000100" => u0_m0_wo0_cm0_q <= "111101000101110001100110";
                WHEN "000101" => u0_m0_wo0_cm0_q <= "111101110010001100001010";
                WHEN "000110" => u0_m0_wo0_cm0_q <= "000000000000000000000000";
                WHEN "000111" => u0_m0_wo0_cm0_q <= "000010100111100111001010";
                WHEN "001000" => u0_m0_wo0_cm0_q <= "000100000100101111100001";
                WHEN "001001" => u0_m0_wo0_cm0_q <= "000011001100110111000001";
                WHEN "001010" => u0_m0_wo0_cm0_q <= "000000000000000000000000";
                WHEN "001011" => u0_m0_wo0_cm0_q <= "111011111000100110101010";
                WHEN "001100" => u0_m0_wo0_cm0_q <= "111001001101011010101000";
                WHEN "001101" => u0_m0_wo0_cm0_q <= "111010001111001111100000";
                WHEN "001110" => u0_m0_wo0_cm0_q <= "000000000000000000000000";
                WHEN "001111" => u0_m0_wo0_cm0_q <= "001001100110100111100011";
                WHEN "010000" => u0_m0_wo0_cm0_q <= "010100010111110011010110";
                WHEN "010001" => u0_m0_wo0_cm0_q <= "011100110011110110001011";
                WHEN "010010" => u0_m0_wo0_cm0_q <= "011111111111111111111111";
                WHEN "010011" => u0_m0_wo0_cm0_q <= "011100110011110110001011";
                WHEN "010100" => u0_m0_wo0_cm0_q <= "010100010111110011010110";
                WHEN "010101" => u0_m0_wo0_cm0_q <= "001001100110100111100011";
                WHEN "010110" => u0_m0_wo0_cm0_q <= "000000000000000000000000";
                WHEN "010111" => u0_m0_wo0_cm0_q <= "111010001111001111100000";
                WHEN "011000" => u0_m0_wo0_cm0_q <= "111001001101011010101000";
                WHEN "011001" => u0_m0_wo0_cm0_q <= "111011111000100110101010";
                WHEN "011010" => u0_m0_wo0_cm0_q <= "000000000000000000000000";
                WHEN "011011" => u0_m0_wo0_cm0_q <= "000011001100110111000001";
                WHEN "011100" => u0_m0_wo0_cm0_q <= "000100000100101111100001";
                WHEN "011101" => u0_m0_wo0_cm0_q <= "000010100111100111001010";
                WHEN "011110" => u0_m0_wo0_cm0_q <= "000000000000000000000000";
                WHEN "011111" => u0_m0_wo0_cm0_q <= "111101110010001100001010";
                WHEN "100000" => u0_m0_wo0_cm0_q <= "111101000101110001100110";
                WHEN "100001" => u0_m0_wo0_cm0_q <= "111110000101000110010110";
                WHEN "100010" => u0_m0_wo0_cm0_q <= "000000000000000000000000";
                WHEN "100011" => u0_m0_wo0_cm0_q <= "000001101100011100000011";
                WHEN "100100" => u0_m0_wo0_cm0_q <= "000010010000110101010011";
                WHEN OTHERS => -- unreachable
                               u0_m0_wo0_cm0_q <= (others => '-');
            END CASE;
        END IF;
    END PROCESS;

    -- u0_m0_wo0_mtree_mult1_0_a_1(BITSELECT,57)@15
    u0_m0_wo0_mtree_mult1_0_a_1_in <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((33 downto 24 => u0_m0_wo0_cm0_q(23)) & u0_m0_wo0_cm0_q));
    u0_m0_wo0_mtree_mult1_0_a_1_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_a_1_in(33 downto 17));

    -- d_u0_m0_wo0_mtree_mult1_0_a_1_b_22(DELAY,350)@15 + 7
    d_u0_m0_wo0_mtree_mult1_0_a_1_b_22 : dspba_delay
    GENERIC MAP ( width => 17, depth => 7, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_a_1_b, xout => d_u0_m0_wo0_mtree_mult1_0_a_1_b_22_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_a1_b1(MULT,63)@22 + 2
    u0_m0_wo0_mtree_mult1_0_a1_b1_a0 <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_a_1_b_22_q);
    u0_m0_wo0_mtree_mult1_0_a1_b1_b0 <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_b_1_b_22_q);
    u0_m0_wo0_mtree_mult1_0_a1_b1_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_a1_b1_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 17,
        lpm_widthb => 17,
        lpm_widthp => 34,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "SIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_a1_b1_a0,
        datab => u0_m0_wo0_mtree_mult1_0_a1_b1_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_a1_b1_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_a1_b1_s1
    );
    u0_m0_wo0_mtree_mult1_0_a1_b1_q <= u0_m0_wo0_mtree_mult1_0_a1_b1_s1;

    -- u0_m0_wo0_mtree_mult1_0_align_2(BITSHIFT,67)@24
    u0_m0_wo0_mtree_mult1_0_align_2_qint <= u0_m0_wo0_mtree_mult1_0_a1_b1_q & "0000000000000000000000000000000000";
    u0_m0_wo0_mtree_mult1_0_align_2_q <= u0_m0_wo0_mtree_mult1_0_align_2_qint(67 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitExpansion_for_b(BITJOIN,138)@24
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitExpansion_for_b_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_SignBit_for_b_b & u0_m0_wo0_mtree_mult1_0_align_2_q;

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b(BITSELECT,142)@24
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitExpansion_for_b_q(5 downto 0));
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_c <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitExpansion_for_b_q(11 downto 6));
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_d <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitExpansion_for_b_q(17 downto 12));
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_e <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitExpansion_for_b_q(23 downto 18));
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_f <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitExpansion_for_b_q(29 downto 24));
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_g <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitExpansion_for_b_q(35 downto 30));
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_h <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitExpansion_for_b_q(41 downto 36));
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_i <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitExpansion_for_b_q(47 downto 42));
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_j <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitExpansion_for_b_q(53 downto 48));
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_k <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitExpansion_for_b_q(59 downto 54));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_SignBit_for_b(BITSELECT,121)@23
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_SignBit_for_b_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_align_1_q(51 downto 51));

    -- u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_SignBit_for_b(BITSELECT,107)@17
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_SignBit_for_b_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_a0_b1_q(33 downto 33));

    -- u0_m0_wo0_mtree_mult1_0_a_0(BITSELECT,56)@15
    u0_m0_wo0_mtree_mult1_0_a_0_in <= u0_m0_wo0_cm0_q(16 downto 0);
    u0_m0_wo0_mtree_mult1_0_a_0_b <= u0_m0_wo0_mtree_mult1_0_a_0_in(16 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_a0_b1(MULT,62)@15 + 2
    u0_m0_wo0_mtree_mult1_0_a0_b1_a0 <= '0' & u0_m0_wo0_mtree_mult1_0_a_0_b;
    u0_m0_wo0_mtree_mult1_0_a0_b1_b0 <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_b_1_b);
    u0_m0_wo0_mtree_mult1_0_a0_b1_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_a0_b1_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 18,
        lpm_widthb => 17,
        lpm_widthp => 35,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "SIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_a0_b1_a0,
        datab => u0_m0_wo0_mtree_mult1_0_a0_b1_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_a0_b1_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_a0_b1_s1
    );
    u0_m0_wo0_mtree_mult1_0_a0_b1_q <= u0_m0_wo0_mtree_mult1_0_a0_b1_s1(33 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_b(BITJOIN,106)@17
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_b_q <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_SignBit_for_b_b & u0_m0_wo0_mtree_mult1_0_a0_b1_q;

    -- u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b(BITSELECT,110)@17
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_b_q(5 downto 0));
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_c <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_b_q(11 downto 6));
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_d <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_b_q(17 downto 12));
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_e <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_b_q(23 downto 18));
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_f <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_b_q(29 downto 24));
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_g <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_b_q(34 downto 30));

    -- u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_SignBit_for_a(BITSELECT,104)@17
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_SignBit_for_a_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_a1_b0_q(33 downto 33));

    -- u0_m0_wo0_mtree_mult1_0_b_0(BITSELECT,58)@15
    u0_m0_wo0_mtree_mult1_0_b_0_in <= u0_m0_wo0_wi0_r0_memr0_q(16 downto 0);
    u0_m0_wo0_mtree_mult1_0_b_0_b <= u0_m0_wo0_mtree_mult1_0_b_0_in(16 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_a1_b0(MULT,61)@15 + 2
    u0_m0_wo0_mtree_mult1_0_a1_b0_a0 <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_a_1_b);
    u0_m0_wo0_mtree_mult1_0_a1_b0_b0 <= '0' & u0_m0_wo0_mtree_mult1_0_b_0_b;
    u0_m0_wo0_mtree_mult1_0_a1_b0_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_a1_b0_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 17,
        lpm_widthb => 18,
        lpm_widthp => 35,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "SIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_a1_b0_a0,
        datab => u0_m0_wo0_mtree_mult1_0_a1_b0_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_a1_b0_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_a1_b0_s1
    );
    u0_m0_wo0_mtree_mult1_0_a1_b0_q <= u0_m0_wo0_mtree_mult1_0_a1_b0_s1(33 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_a(BITJOIN,103)@17
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_a_q <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_SignBit_for_a_b & u0_m0_wo0_mtree_mult1_0_a1_b0_q;

    -- u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a(BITSELECT,109)@17
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_a_q(5 downto 0));
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_c <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_a_q(11 downto 6));
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_d <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_a_q(17 downto 12));
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_e <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_a_q(23 downto 18));
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_f <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_a_q(29 downto 24));
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_g <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitExpansion_for_a_q(34 downto 30));

    -- u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6(ADD,111)@17 + 1
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_b);
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_b);
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_c(0) <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_o(6);
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_q <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_o(5 downto 0);

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_c_18(DELAY,371)@17 + 1
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_c_18 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_c, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_c_18_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_c_18(DELAY,366)@17 + 1
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_c_18 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_c, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_c_18_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6(ADD,112)@18 + 1
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_cin <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_c;
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_a <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_c_18_q) & '1';
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_c_18_q) & u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_cin(0);
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_c(0) <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_o(7);
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_q <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_o(6 downto 1);

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_d_19(DELAY,372)@17 + 2
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_d_19 : dspba_delay
    GENERIC MAP ( width => 6, depth => 2, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_d, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_d_19_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_d_19(DELAY,367)@17 + 2
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_d_19 : dspba_delay
    GENERIC MAP ( width => 6, depth => 2, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_d, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_d_19_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6(ADD,113)@19 + 1
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_cin <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_c;
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_a <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_d_19_q) & '1';
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_d_19_q) & u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_cin(0);
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_c(0) <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_o(7);
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_q <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_o(6 downto 1);

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_e_20(DELAY,373)@17 + 3
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_e_20 : dspba_delay
    GENERIC MAP ( width => 6, depth => 3, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_e, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_e_20_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_e_20(DELAY,368)@17 + 3
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_e_20 : dspba_delay
    GENERIC MAP ( width => 6, depth => 3, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_e, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_e_20_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6(ADD,114)@20 + 1
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_cin <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_c;
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_a <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_e_20_q) & '1';
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_e_20_q) & u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_cin(0);
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_c(0) <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_o(7);
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_q <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_o(6 downto 1);

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_f_21(DELAY,374)@17 + 4
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_f_21 : dspba_delay
    GENERIC MAP ( width => 6, depth => 4, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_f, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_f_21_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_f_21(DELAY,369)@17 + 4
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_f_21 : dspba_delay
    GENERIC MAP ( width => 6, depth => 4, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_f, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_f_21_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6(ADD,115)@21 + 1
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_cin <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_c;
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_a <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_f_21_q) & '1';
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_f_21_q) & u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_cin(0);
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_c(0) <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_o(7);
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_q <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_o(6 downto 1);

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_g_22(DELAY,375)@17 + 5
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_g_22 : dspba_delay
    GENERIC MAP ( width => 5, depth => 5, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_g, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_g_22_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_g_22(DELAY,370)@17 + 5
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_g_22 : dspba_delay
    GENERIC MAP ( width => 5, depth => 5, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_g, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_g_22_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6(ADD,116)@22 + 1
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_cin <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_c;
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_a <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((5 downto 5 => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_g_22_q(4)) & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_a_g_22_q) & '1');
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_b <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((5 downto 5 => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_g_22_q(4)) & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitSelect_for_b_g_22_q) & u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_cin(0));
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_o <= STD_LOGIC_VECTOR(SIGNED(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_a) + SIGNED(u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_q <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_o(5 downto 1);

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_q_23(DELAY,380)@22 + 1
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_q_23 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_q, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_q_23_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_q_23(DELAY,379)@21 + 2
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_q_23 : dspba_delay
    GENERIC MAP ( width => 6, depth => 2, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_q, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_q_23_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_q_23(DELAY,378)@20 + 3
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_q_23 : dspba_delay
    GENERIC MAP ( width => 6, depth => 3, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_q, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_q_23_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_q_23(DELAY,377)@19 + 4
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_q_23 : dspba_delay
    GENERIC MAP ( width => 6, depth => 4, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_q, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_q_23_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_q_23(DELAY,376)@18 + 5
    d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_q_23 : dspba_delay
    GENERIC MAP ( width => 6, depth => 5, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_q, xout => d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_q_23_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitJoin_for_q(BITJOIN,117)@23
    u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitJoin_for_q_q <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p6_of_6_q & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p5_of_6_q_23_q & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p4_of_6_q_23_q & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p3_of_6_q_23_q & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p2_of_6_q_23_q & d_u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_p1_of_6_q_23_q;

    -- u0_m0_wo0_mtree_mult1_0_align_1(BITSHIFT,66)@23
    u0_m0_wo0_mtree_mult1_0_align_1_qint <= u0_m0_wo0_mtree_mult1_0_addcol_1_add_0_0_BitJoin_for_q_q & "00000000000000000";
    u0_m0_wo0_mtree_mult1_0_align_1_q <= u0_m0_wo0_mtree_mult1_0_align_1_qint(51 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_b(BITJOIN,120)@23
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_b_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_SignBit_for_b_b & u0_m0_wo0_mtree_mult1_0_align_1_q;

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b(BITSELECT,124)@23
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_b_q(5 downto 0));
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_c <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_b_q(11 downto 6));
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_d <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_b_q(17 downto 12));
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_e <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_b_q(23 downto 18));
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_f <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_b_q(29 downto 24));
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_g <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_b_q(35 downto 30));
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_h <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_b_q(41 downto 36));
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_i <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_b_q(47 downto 42));
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_j <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_b_q(52 downto 48));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_UpperBits_for_a(CONSTANT,119)@0
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_UpperBits_for_a_q <= "0000000000000000000";

    -- d_u0_m0_wo0_mtree_mult1_0_b_0_b_21(DELAY,351)@15 + 6
    d_u0_m0_wo0_mtree_mult1_0_b_0_b_21 : dspba_delay
    GENERIC MAP ( width => 17, depth => 6, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_b_0_b, xout => d_u0_m0_wo0_mtree_mult1_0_b_0_b_21_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_a_0_b_21(DELAY,349)@15 + 6
    d_u0_m0_wo0_mtree_mult1_0_a_0_b_21 : dspba_delay
    GENERIC MAP ( width => 17, depth => 6, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_a_0_b, xout => d_u0_m0_wo0_mtree_mult1_0_a_0_b_21_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_a0_b0(MULT,60)@21 + 2
    u0_m0_wo0_mtree_mult1_0_a0_b0_a0 <= d_u0_m0_wo0_mtree_mult1_0_a_0_b_21_q;
    u0_m0_wo0_mtree_mult1_0_a0_b0_b0 <= d_u0_m0_wo0_mtree_mult1_0_b_0_b_21_q;
    u0_m0_wo0_mtree_mult1_0_a0_b0_reset <= areset;
    u0_m0_wo0_mtree_mult1_0_a0_b0_component : lpm_mult
    GENERIC MAP (
        lpm_widtha => 17,
        lpm_widthb => 17,
        lpm_widthp => 34,
        lpm_widths => 1,
        lpm_type => "LPM_MULT",
        lpm_representation => "UNSIGNED",
        lpm_hint => "DEDICATED_MULTIPLIER_CIRCUITRY=YES, MAXIMIZE_SPEED=5",
        lpm_pipeline => 2
    )
    PORT MAP (
        dataa => u0_m0_wo0_mtree_mult1_0_a0_b0_a0,
        datab => u0_m0_wo0_mtree_mult1_0_a0_b0_b0,
        clken => VCC_q(0),
        aclr => u0_m0_wo0_mtree_mult1_0_a0_b0_reset,
        clock => clk,
        result => u0_m0_wo0_mtree_mult1_0_a0_b0_s1
    );
    u0_m0_wo0_mtree_mult1_0_a0_b0_q <= u0_m0_wo0_mtree_mult1_0_a0_b0_s1;

    -- u0_m0_wo0_mtree_mult1_0_align_0(BITSHIFT,65)@23
    u0_m0_wo0_mtree_mult1_0_align_0_qint <= u0_m0_wo0_mtree_mult1_0_a0_b0_q;
    u0_m0_wo0_mtree_mult1_0_align_0_q <= u0_m0_wo0_mtree_mult1_0_align_0_qint(33 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_a(BITJOIN,118)@23
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_a_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_UpperBits_for_a_q & u0_m0_wo0_mtree_mult1_0_align_0_q;

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a(BITSELECT,123)@23
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_b <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_a_q(5 downto 0);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_c <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_a_q(11 downto 6);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_d <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_a_q(17 downto 12);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_e <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_a_q(23 downto 18);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_f <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_a_q(29 downto 24);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_g <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitExpansion_for_a_q(35 downto 30);

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9(ADD,125)@23 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_b);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_b);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_o(6);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_o(5 downto 0);

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12(ADD,143)@24 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_q);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_b);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_o(6);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_o(5 downto 0);

    -- u0_m0_wo0_accum_p1_of_11(ADD,91)@25 + 1
    u0_m0_wo0_accum_p1_of_11_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_q);
    u0_m0_wo0_accum_p1_of_11_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_adelay_p0_q);
    u0_m0_wo0_accum_p1_of_11_i <= u0_m0_wo0_accum_p1_of_11_a;
    u0_m0_wo0_accum_p1_of_11_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_accum_p1_of_11_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_25_q = "1") THEN
                IF (u0_m0_wo0_aseq_q = "1") THEN
                    u0_m0_wo0_accum_p1_of_11_o <= u0_m0_wo0_accum_p1_of_11_i;
                ELSE
                    u0_m0_wo0_accum_p1_of_11_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_accum_p1_of_11_a) + UNSIGNED(u0_m0_wo0_accum_p1_of_11_b));
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_accum_p1_of_11_c(0) <= u0_m0_wo0_accum_p1_of_11_o(6);
    u0_m0_wo0_accum_p1_of_11_q <= u0_m0_wo0_accum_p1_of_11_o(5 downto 0);

    -- d_u0_m0_wo0_aseq_q_26(DELAY,338)@25 + 1
    d_u0_m0_wo0_aseq_q_26 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_aseq_q, xout => d_u0_m0_wo0_aseq_q_26_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_compute_q_26(DELAY,328)@25 + 1
    d_u0_m0_wo0_compute_q_26 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_compute_q_25_q, xout => d_u0_m0_wo0_compute_q_26_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_memread_q_26(DELAY,316)@25 + 1
    d_u0_m0_wo0_memread_q_26 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_25_q, xout => d_u0_m0_wo0_memread_q_26_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_adelay_p1(DELAY,158)@26
    u0_m0_wo0_adelay_p1 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "NONE" )
    PORT MAP ( xin => u0_m0_wo0_accum_p2_of_11_q, xout => u0_m0_wo0_adelay_p1_q, ena => d_u0_m0_wo0_compute_q_26_q(0), clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_c_25(DELAY,395)@24 + 1
    d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_c_25 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_c, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_c_25_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_c_24(DELAY,386)@23 + 1
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_c_24 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_c, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_c_24_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_c_24(DELAY,381)@23 + 1
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_c_24 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_c, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_c_24_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9(ADD,126)@24 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_cin <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p1_of_9_c;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_a <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_c_24_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_c_24_q) & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_o(6 downto 1);

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12(ADD,144)@25 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_cin <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p1_of_12_c;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_c_25_q) & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_o(6 downto 1);

    -- u0_m0_wo0_accum_p2_of_11(ADD,92)@26 + 1
    u0_m0_wo0_accum_p2_of_11_cin <= u0_m0_wo0_accum_p1_of_11_c;
    u0_m0_wo0_accum_p2_of_11_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_q) & '1';
    u0_m0_wo0_accum_p2_of_11_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_adelay_p1_q) & u0_m0_wo0_accum_p2_of_11_cin(0);
    u0_m0_wo0_accum_p2_of_11_i <= u0_m0_wo0_accum_p2_of_11_a;
    u0_m0_wo0_accum_p2_of_11_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_accum_p2_of_11_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_26_q = "1") THEN
                IF (d_u0_m0_wo0_aseq_q_26_q = "1") THEN
                    u0_m0_wo0_accum_p2_of_11_o <= u0_m0_wo0_accum_p2_of_11_i;
                ELSE
                    u0_m0_wo0_accum_p2_of_11_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_accum_p2_of_11_a) + UNSIGNED(u0_m0_wo0_accum_p2_of_11_b));
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_accum_p2_of_11_c(0) <= u0_m0_wo0_accum_p2_of_11_o(7);
    u0_m0_wo0_accum_p2_of_11_q <= u0_m0_wo0_accum_p2_of_11_o(6 downto 1);

    -- d_u0_m0_wo0_aseq_q_27(DELAY,339)@26 + 1
    d_u0_m0_wo0_aseq_q_27 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_aseq_q_26_q, xout => d_u0_m0_wo0_aseq_q_27_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_compute_q_27(DELAY,329)@26 + 1
    d_u0_m0_wo0_compute_q_27 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_compute_q_26_q, xout => d_u0_m0_wo0_compute_q_27_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_memread_q_27(DELAY,317)@26 + 1
    d_u0_m0_wo0_memread_q_27 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_26_q, xout => d_u0_m0_wo0_memread_q_27_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_adelay_p2(DELAY,159)@27
    u0_m0_wo0_adelay_p2 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "NONE" )
    PORT MAP ( xin => u0_m0_wo0_accum_p3_of_11_q, xout => u0_m0_wo0_adelay_p2_q, ena => d_u0_m0_wo0_compute_q_27_q(0), clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_d_26(DELAY,396)@24 + 2
    d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_d_26 : dspba_delay
    GENERIC MAP ( width => 6, depth => 2, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_d, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_d_26_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_d_25(DELAY,387)@23 + 2
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_d_25 : dspba_delay
    GENERIC MAP ( width => 6, depth => 2, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_d, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_d_25_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_d_25(DELAY,382)@23 + 2
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_d_25 : dspba_delay
    GENERIC MAP ( width => 6, depth => 2, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_d, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_d_25_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9(ADD,127)@25 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_cin <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p2_of_9_c;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_a <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_d_25_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_d_25_q) & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_o(6 downto 1);

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12(ADD,145)@26 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_cin <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p2_of_12_c;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_d_26_q) & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_o(6 downto 1);

    -- u0_m0_wo0_accum_p3_of_11(ADD,93)@27 + 1
    u0_m0_wo0_accum_p3_of_11_cin <= u0_m0_wo0_accum_p2_of_11_c;
    u0_m0_wo0_accum_p3_of_11_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_q) & '1';
    u0_m0_wo0_accum_p3_of_11_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_adelay_p2_q) & u0_m0_wo0_accum_p3_of_11_cin(0);
    u0_m0_wo0_accum_p3_of_11_i <= u0_m0_wo0_accum_p3_of_11_a;
    u0_m0_wo0_accum_p3_of_11_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_accum_p3_of_11_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_27_q = "1") THEN
                IF (d_u0_m0_wo0_aseq_q_27_q = "1") THEN
                    u0_m0_wo0_accum_p3_of_11_o <= u0_m0_wo0_accum_p3_of_11_i;
                ELSE
                    u0_m0_wo0_accum_p3_of_11_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_accum_p3_of_11_a) + UNSIGNED(u0_m0_wo0_accum_p3_of_11_b));
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_accum_p3_of_11_c(0) <= u0_m0_wo0_accum_p3_of_11_o(7);
    u0_m0_wo0_accum_p3_of_11_q <= u0_m0_wo0_accum_p3_of_11_o(6 downto 1);

    -- d_u0_m0_wo0_aseq_q_28(DELAY,340)@27 + 1
    d_u0_m0_wo0_aseq_q_28 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_aseq_q_27_q, xout => d_u0_m0_wo0_aseq_q_28_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_compute_q_28(DELAY,330)@27 + 1
    d_u0_m0_wo0_compute_q_28 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_compute_q_27_q, xout => d_u0_m0_wo0_compute_q_28_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_memread_q_28(DELAY,318)@27 + 1
    d_u0_m0_wo0_memread_q_28 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_27_q, xout => d_u0_m0_wo0_memread_q_28_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_adelay_p3(DELAY,160)@28
    u0_m0_wo0_adelay_p3 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "NONE" )
    PORT MAP ( xin => u0_m0_wo0_accum_p4_of_11_q, xout => u0_m0_wo0_adelay_p3_q, ena => d_u0_m0_wo0_compute_q_28_q(0), clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_e_27(DELAY,397)@24 + 3
    d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_e_27 : dspba_delay
    GENERIC MAP ( width => 6, depth => 3, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_e, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_e_27_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_e_26(DELAY,388)@23 + 3
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_e_26 : dspba_delay
    GENERIC MAP ( width => 6, depth => 3, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_e, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_e_26_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_e_26(DELAY,383)@23 + 3
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_e_26 : dspba_delay
    GENERIC MAP ( width => 6, depth => 3, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_e, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_e_26_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9(ADD,128)@26 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_cin <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p3_of_9_c;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_a <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_e_26_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_e_26_q) & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_o(6 downto 1);

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12(ADD,146)@27 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_cin <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p3_of_12_c;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_e_27_q) & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_o(6 downto 1);

    -- u0_m0_wo0_accum_p4_of_11(ADD,94)@28 + 1
    u0_m0_wo0_accum_p4_of_11_cin <= u0_m0_wo0_accum_p3_of_11_c;
    u0_m0_wo0_accum_p4_of_11_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_q) & '1';
    u0_m0_wo0_accum_p4_of_11_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_adelay_p3_q) & u0_m0_wo0_accum_p4_of_11_cin(0);
    u0_m0_wo0_accum_p4_of_11_i <= u0_m0_wo0_accum_p4_of_11_a;
    u0_m0_wo0_accum_p4_of_11_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_accum_p4_of_11_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_28_q = "1") THEN
                IF (d_u0_m0_wo0_aseq_q_28_q = "1") THEN
                    u0_m0_wo0_accum_p4_of_11_o <= u0_m0_wo0_accum_p4_of_11_i;
                ELSE
                    u0_m0_wo0_accum_p4_of_11_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_accum_p4_of_11_a) + UNSIGNED(u0_m0_wo0_accum_p4_of_11_b));
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_accum_p4_of_11_c(0) <= u0_m0_wo0_accum_p4_of_11_o(7);
    u0_m0_wo0_accum_p4_of_11_q <= u0_m0_wo0_accum_p4_of_11_o(6 downto 1);

    -- d_u0_m0_wo0_aseq_q_29(DELAY,341)@28 + 1
    d_u0_m0_wo0_aseq_q_29 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_aseq_q_28_q, xout => d_u0_m0_wo0_aseq_q_29_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_compute_q_29(DELAY,331)@28 + 1
    d_u0_m0_wo0_compute_q_29 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_compute_q_28_q, xout => d_u0_m0_wo0_compute_q_29_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_memread_q_29(DELAY,319)@28 + 1
    d_u0_m0_wo0_memread_q_29 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_28_q, xout => d_u0_m0_wo0_memread_q_29_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_adelay_p4(DELAY,161)@29
    u0_m0_wo0_adelay_p4 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "NONE" )
    PORT MAP ( xin => u0_m0_wo0_accum_p5_of_11_q, xout => u0_m0_wo0_adelay_p4_q, ena => d_u0_m0_wo0_compute_q_29_q(0), clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_f_28(DELAY,398)@24 + 4
    d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_f_28 : dspba_delay
    GENERIC MAP ( width => 6, depth => 4, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_f, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_f_28_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_f_27(DELAY,389)@23 + 4
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_f_27 : dspba_delay
    GENERIC MAP ( width => 6, depth => 4, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_f, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_f_27_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_f_27(DELAY,384)@23 + 4
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_f_27 : dspba_delay
    GENERIC MAP ( width => 6, depth => 4, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_f, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_f_27_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9(ADD,129)@27 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_cin <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p4_of_9_c;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_a <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_f_27_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_f_27_q) & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_o(6 downto 1);

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12(ADD,147)@28 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_cin <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p4_of_12_c;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_f_28_q) & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_o(6 downto 1);

    -- u0_m0_wo0_accum_p5_of_11(ADD,95)@29 + 1
    u0_m0_wo0_accum_p5_of_11_cin <= u0_m0_wo0_accum_p4_of_11_c;
    u0_m0_wo0_accum_p5_of_11_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_q) & '1';
    u0_m0_wo0_accum_p5_of_11_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_adelay_p4_q) & u0_m0_wo0_accum_p5_of_11_cin(0);
    u0_m0_wo0_accum_p5_of_11_i <= u0_m0_wo0_accum_p5_of_11_a;
    u0_m0_wo0_accum_p5_of_11_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_accum_p5_of_11_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_29_q = "1") THEN
                IF (d_u0_m0_wo0_aseq_q_29_q = "1") THEN
                    u0_m0_wo0_accum_p5_of_11_o <= u0_m0_wo0_accum_p5_of_11_i;
                ELSE
                    u0_m0_wo0_accum_p5_of_11_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_accum_p5_of_11_a) + UNSIGNED(u0_m0_wo0_accum_p5_of_11_b));
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_accum_p5_of_11_c(0) <= u0_m0_wo0_accum_p5_of_11_o(7);
    u0_m0_wo0_accum_p5_of_11_q <= u0_m0_wo0_accum_p5_of_11_o(6 downto 1);

    -- d_u0_m0_wo0_aseq_q_30(DELAY,342)@29 + 1
    d_u0_m0_wo0_aseq_q_30 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_aseq_q_29_q, xout => d_u0_m0_wo0_aseq_q_30_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_compute_q_30(DELAY,332)@29 + 1
    d_u0_m0_wo0_compute_q_30 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_compute_q_29_q, xout => d_u0_m0_wo0_compute_q_30_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_memread_q_30(DELAY,320)@29 + 1
    d_u0_m0_wo0_memread_q_30 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_29_q, xout => d_u0_m0_wo0_memread_q_30_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_adelay_p5(DELAY,162)@30
    u0_m0_wo0_adelay_p5 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "NONE" )
    PORT MAP ( xin => u0_m0_wo0_accum_p6_of_11_q, xout => u0_m0_wo0_adelay_p5_q, ena => d_u0_m0_wo0_compute_q_30_q(0), clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_g_29(DELAY,399)@24 + 5
    d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_g_29 : dspba_delay
    GENERIC MAP ( width => 6, depth => 5, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_g, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_g_29_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_g_28(DELAY,390)@23 + 5
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_g_28 : dspba_delay
    GENERIC MAP ( width => 6, depth => 5, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_g, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_g_28_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_g_28(DELAY,385)@23 + 5
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_g_28 : dspba_delay
    GENERIC MAP ( width => 6, depth => 5, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_g, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_g_28_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9(ADD,130)@28 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_cin <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p5_of_9_c;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_a <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_g_28_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_g_28_q) & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_o(6 downto 1);

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12(ADD,148)@29 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_cin <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p5_of_12_c;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_g_29_q) & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_o(6 downto 1);

    -- u0_m0_wo0_accum_p6_of_11(ADD,96)@30 + 1
    u0_m0_wo0_accum_p6_of_11_cin <= u0_m0_wo0_accum_p5_of_11_c;
    u0_m0_wo0_accum_p6_of_11_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_q) & '1';
    u0_m0_wo0_accum_p6_of_11_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_adelay_p5_q) & u0_m0_wo0_accum_p6_of_11_cin(0);
    u0_m0_wo0_accum_p6_of_11_i <= u0_m0_wo0_accum_p6_of_11_a;
    u0_m0_wo0_accum_p6_of_11_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_accum_p6_of_11_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_30_q = "1") THEN
                IF (d_u0_m0_wo0_aseq_q_30_q = "1") THEN
                    u0_m0_wo0_accum_p6_of_11_o <= u0_m0_wo0_accum_p6_of_11_i;
                ELSE
                    u0_m0_wo0_accum_p6_of_11_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_accum_p6_of_11_a) + UNSIGNED(u0_m0_wo0_accum_p6_of_11_b));
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_accum_p6_of_11_c(0) <= u0_m0_wo0_accum_p6_of_11_o(7);
    u0_m0_wo0_accum_p6_of_11_q <= u0_m0_wo0_accum_p6_of_11_o(6 downto 1);

    -- d_u0_m0_wo0_aseq_q_31(DELAY,343)@30 + 1
    d_u0_m0_wo0_aseq_q_31 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_aseq_q_30_q, xout => d_u0_m0_wo0_aseq_q_31_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_compute_q_31(DELAY,333)@30 + 1
    d_u0_m0_wo0_compute_q_31 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_compute_q_30_q, xout => d_u0_m0_wo0_compute_q_31_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_memread_q_31(DELAY,321)@30 + 1
    d_u0_m0_wo0_memread_q_31 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_30_q, xout => d_u0_m0_wo0_memread_q_31_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_adelay_p6(DELAY,163)@31
    u0_m0_wo0_adelay_p6 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "NONE" )
    PORT MAP ( xin => u0_m0_wo0_accum_p7_of_11_q, xout => u0_m0_wo0_adelay_p6_q, ena => d_u0_m0_wo0_compute_q_31_q(0), clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_h_30(DELAY,400)@24 + 6
    d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_h_30 : dspba_delay
    GENERIC MAP ( width => 6, depth => 6, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_h, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_h_30_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_h_29(DELAY,391)@23 + 6
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_h_29 : dspba_delay
    GENERIC MAP ( width => 6, depth => 6, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_h, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_h_29_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel6_0_merged_bit_select(BITSELECT,310)@29
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel6_0_merged_bit_select_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_UpperBits_for_a_q(7 downto 2));
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel6_0_merged_bit_select_c <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_UpperBits_for_a_q(13 downto 8));
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel6_0_merged_bit_select_d <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_UpperBits_for_a_q(18 downto 14));

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9(ADD,131)@29 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_cin <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p6_of_9_c;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel6_0_merged_bit_select_b) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_h_29_q) & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_o(6 downto 1);

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12(ADD,149)@30 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_cin <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p6_of_12_c;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_h_30_q) & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_o(6 downto 1);

    -- u0_m0_wo0_accum_p7_of_11(ADD,97)@31 + 1
    u0_m0_wo0_accum_p7_of_11_cin <= u0_m0_wo0_accum_p6_of_11_c;
    u0_m0_wo0_accum_p7_of_11_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_q) & '1';
    u0_m0_wo0_accum_p7_of_11_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_adelay_p6_q) & u0_m0_wo0_accum_p7_of_11_cin(0);
    u0_m0_wo0_accum_p7_of_11_i <= u0_m0_wo0_accum_p7_of_11_a;
    u0_m0_wo0_accum_p7_of_11_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_accum_p7_of_11_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_31_q = "1") THEN
                IF (d_u0_m0_wo0_aseq_q_31_q = "1") THEN
                    u0_m0_wo0_accum_p7_of_11_o <= u0_m0_wo0_accum_p7_of_11_i;
                ELSE
                    u0_m0_wo0_accum_p7_of_11_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_accum_p7_of_11_a) + UNSIGNED(u0_m0_wo0_accum_p7_of_11_b));
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_accum_p7_of_11_c(0) <= u0_m0_wo0_accum_p7_of_11_o(7);
    u0_m0_wo0_accum_p7_of_11_q <= u0_m0_wo0_accum_p7_of_11_o(6 downto 1);

    -- d_u0_m0_wo0_aseq_q_32(DELAY,344)@31 + 1
    d_u0_m0_wo0_aseq_q_32 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_aseq_q_31_q, xout => d_u0_m0_wo0_aseq_q_32_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_compute_q_32(DELAY,334)@31 + 1
    d_u0_m0_wo0_compute_q_32 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_compute_q_31_q, xout => d_u0_m0_wo0_compute_q_32_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_memread_q_32(DELAY,322)@31 + 1
    d_u0_m0_wo0_memread_q_32 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_31_q, xout => d_u0_m0_wo0_memread_q_32_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_adelay_p7(DELAY,164)@32
    u0_m0_wo0_adelay_p7 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "NONE" )
    PORT MAP ( xin => u0_m0_wo0_accum_p8_of_11_q, xout => u0_m0_wo0_adelay_p7_q, ena => d_u0_m0_wo0_compute_q_32_q(0), clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_i_31(DELAY,401)@24 + 7
    d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_i_31 : dspba_delay
    GENERIC MAP ( width => 6, depth => 7, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_i, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_i_31_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_i_30(DELAY,392)@23 + 7
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_i_30 : dspba_delay
    GENERIC MAP ( width => 6, depth => 7, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_i, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_i_30_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9(ADD,132)@30 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_cin <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p7_of_9_c;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel6_0_merged_bit_select_c) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_i_30_q) & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_o(6 downto 1);

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12(ADD,150)@31 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_cin <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p7_of_12_c;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_i_31_q) & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_o(6 downto 1);

    -- u0_m0_wo0_accum_p8_of_11(ADD,98)@32 + 1
    u0_m0_wo0_accum_p8_of_11_cin <= u0_m0_wo0_accum_p7_of_11_c;
    u0_m0_wo0_accum_p8_of_11_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_q) & '1';
    u0_m0_wo0_accum_p8_of_11_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_adelay_p7_q) & u0_m0_wo0_accum_p8_of_11_cin(0);
    u0_m0_wo0_accum_p8_of_11_i <= u0_m0_wo0_accum_p8_of_11_a;
    u0_m0_wo0_accum_p8_of_11_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_accum_p8_of_11_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_32_q = "1") THEN
                IF (d_u0_m0_wo0_aseq_q_32_q = "1") THEN
                    u0_m0_wo0_accum_p8_of_11_o <= u0_m0_wo0_accum_p8_of_11_i;
                ELSE
                    u0_m0_wo0_accum_p8_of_11_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_accum_p8_of_11_a) + UNSIGNED(u0_m0_wo0_accum_p8_of_11_b));
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_accum_p8_of_11_c(0) <= u0_m0_wo0_accum_p8_of_11_o(7);
    u0_m0_wo0_accum_p8_of_11_q <= u0_m0_wo0_accum_p8_of_11_o(6 downto 1);

    -- d_u0_m0_wo0_aseq_q_33(DELAY,345)@32 + 1
    d_u0_m0_wo0_aseq_q_33 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_aseq_q_32_q, xout => d_u0_m0_wo0_aseq_q_33_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_compute_q_33(DELAY,335)@32 + 1
    d_u0_m0_wo0_compute_q_33 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_compute_q_32_q, xout => d_u0_m0_wo0_compute_q_33_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_memread_q_33(DELAY,323)@32 + 1
    d_u0_m0_wo0_memread_q_33 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_32_q, xout => d_u0_m0_wo0_memread_q_33_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_adelay_p8(DELAY,165)@33
    u0_m0_wo0_adelay_p8 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "NONE" )
    PORT MAP ( xin => u0_m0_wo0_accum_p9_of_11_q, xout => u0_m0_wo0_adelay_p8_q, ena => d_u0_m0_wo0_compute_q_33_q(0), clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_j_32(DELAY,402)@24 + 8
    d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_j_32 : dspba_delay
    GENERIC MAP ( width => 6, depth => 8, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_j, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_j_32_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel8_1(BITSELECT,268)@32
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel8_1_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_q(4 downto 4));

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_j_31(DELAY,393)@23 + 8
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_j_31 : dspba_delay
    GENERIC MAP ( width => 5, depth => 8, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_j, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_j_31_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9(ADD,133)@31 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_cin <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p8_of_9_c;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_a <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_a_tessel6_0_merged_bit_select_d) & '1');
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_b <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((5 downto 5 => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_j_31_q(4)) & d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_BitSelect_for_b_j_31_q) & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_cin(0));
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_o <= STD_LOGIC_VECTOR(SIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_a) + SIGNED(u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_q <= u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_o(5 downto 1);

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_BitJoin_for_j(BITJOIN,269)@32
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_BitJoin_for_j_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel8_1_b & u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_q;

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12(ADD,151)@32 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_cin <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p8_of_12_c;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_BitJoin_for_j_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_j_32_q) & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_c(0) <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_o(7);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_o(6 downto 1);

    -- u0_m0_wo0_accum_p9_of_11(ADD,99)@33 + 1
    u0_m0_wo0_accum_p9_of_11_cin <= u0_m0_wo0_accum_p8_of_11_c;
    u0_m0_wo0_accum_p9_of_11_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_q) & '1';
    u0_m0_wo0_accum_p9_of_11_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_adelay_p8_q) & u0_m0_wo0_accum_p9_of_11_cin(0);
    u0_m0_wo0_accum_p9_of_11_i <= u0_m0_wo0_accum_p9_of_11_a;
    u0_m0_wo0_accum_p9_of_11_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_accum_p9_of_11_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_33_q = "1") THEN
                IF (d_u0_m0_wo0_aseq_q_33_q = "1") THEN
                    u0_m0_wo0_accum_p9_of_11_o <= u0_m0_wo0_accum_p9_of_11_i;
                ELSE
                    u0_m0_wo0_accum_p9_of_11_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_accum_p9_of_11_a) + UNSIGNED(u0_m0_wo0_accum_p9_of_11_b));
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_accum_p9_of_11_c(0) <= u0_m0_wo0_accum_p9_of_11_o(7);
    u0_m0_wo0_accum_p9_of_11_q <= u0_m0_wo0_accum_p9_of_11_o(6 downto 1);

    -- d_u0_m0_wo0_aseq_q_34(DELAY,346)@33 + 1
    d_u0_m0_wo0_aseq_q_34 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_aseq_q_33_q, xout => d_u0_m0_wo0_aseq_q_34_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_compute_q_34(DELAY,336)@33 + 1
    d_u0_m0_wo0_compute_q_34 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_compute_q_33_q, xout => d_u0_m0_wo0_compute_q_34_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_memread_q_34(DELAY,324)@33 + 1
    d_u0_m0_wo0_memread_q_34 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_33_q, xout => d_u0_m0_wo0_memread_q_34_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_adelay_p9(DELAY,166)@34
    u0_m0_wo0_adelay_p9 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "NONE" )
    PORT MAP ( xin => u0_m0_wo0_accum_p10_of_11_q, xout => u0_m0_wo0_adelay_p9_q, ena => d_u0_m0_wo0_compute_q_34_q(0), clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_k_33(DELAY,403)@24 + 9
    d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_k_33 : dspba_delay
    GENERIC MAP ( width => 6, depth => 9, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_k, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_k_33_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_q_33(DELAY,394)@32 + 1
    d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_q_33 : dspba_delay
    GENERIC MAP ( width => 5, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_q, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_q_33_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel9_0(BITSELECT,270)@33
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel9_0_b <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_result_add_0_0_p9_of_9_q_33_q(4 downto 4));

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_BitJoin_for_k(BITJOIN,276)@33
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_BitJoin_for_k_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel9_0_b & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel9_0_b & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel9_0_b & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel9_0_b & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel9_0_b & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_tessel9_0_b;

    -- u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12(ADD,152)@33 + 1
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_cin <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p9_of_12_c;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_a_BitJoin_for_k_q) & '1';
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_b <= STD_LOGIC_VECTOR("0" & d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_BitSelect_for_b_k_33_q) & u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_cin(0);
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_a) + UNSIGNED(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_b));
        END IF;
    END PROCESS;
    u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_q <= u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_o(6 downto 1);

    -- u0_m0_wo0_accum_BitSelect_for_a_tessel9_1(BITSELECT,188)@34
    u0_m0_wo0_accum_BitSelect_for_a_tessel9_1_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_q(1 downto 1));

    -- u0_m0_wo0_accum_BitSelect_for_a_tessel9_0(BITSELECT,187)@34
    u0_m0_wo0_accum_BitSelect_for_a_tessel9_0_b <= STD_LOGIC_VECTOR(u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_q(1 downto 0));

    -- u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_k(BITJOIN,192)@34
    u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_k_q <= u0_m0_wo0_accum_BitSelect_for_a_tessel9_1_b & u0_m0_wo0_accum_BitSelect_for_a_tessel9_1_b & u0_m0_wo0_accum_BitSelect_for_a_tessel9_1_b & u0_m0_wo0_accum_BitSelect_for_a_tessel9_1_b & u0_m0_wo0_accum_BitSelect_for_a_tessel9_0_b;

    -- u0_m0_wo0_accum_p10_of_11(ADD,100)@34 + 1
    u0_m0_wo0_accum_p10_of_11_cin <= u0_m0_wo0_accum_p9_of_11_c;
    u0_m0_wo0_accum_p10_of_11_a <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_k_q) & '1';
    u0_m0_wo0_accum_p10_of_11_b <= STD_LOGIC_VECTOR("0" & u0_m0_wo0_adelay_p9_q) & u0_m0_wo0_accum_p10_of_11_cin(0);
    u0_m0_wo0_accum_p10_of_11_i <= u0_m0_wo0_accum_p10_of_11_a;
    u0_m0_wo0_accum_p10_of_11_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_accum_p10_of_11_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_34_q = "1") THEN
                IF (d_u0_m0_wo0_aseq_q_34_q = "1") THEN
                    u0_m0_wo0_accum_p10_of_11_o <= u0_m0_wo0_accum_p10_of_11_i;
                ELSE
                    u0_m0_wo0_accum_p10_of_11_o <= STD_LOGIC_VECTOR(UNSIGNED(u0_m0_wo0_accum_p10_of_11_a) + UNSIGNED(u0_m0_wo0_accum_p10_of_11_b));
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_accum_p10_of_11_c(0) <= u0_m0_wo0_accum_p10_of_11_o(7);
    u0_m0_wo0_accum_p10_of_11_q <= u0_m0_wo0_accum_p10_of_11_o(6 downto 1);

    -- d_u0_m0_wo0_aseq_q_35(DELAY,347)@34 + 1
    d_u0_m0_wo0_aseq_q_35 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_aseq_q_34_q, xout => d_u0_m0_wo0_aseq_q_35_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_compute_q_35(DELAY,337)@34 + 1
    d_u0_m0_wo0_compute_q_35 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_compute_q_34_q, xout => d_u0_m0_wo0_compute_q_35_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_memread_q_35(DELAY,325)@34 + 1
    d_u0_m0_wo0_memread_q_35 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => d_u0_m0_wo0_memread_q_34_q, xout => d_u0_m0_wo0_memread_q_35_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_adelay_p10(DELAY,167)@35
    u0_m0_wo0_adelay_p10 : dspba_delay
    GENERIC MAP ( width => 2, depth => 1, reset_kind => "NONE" )
    PORT MAP ( xin => u0_m0_wo0_accum_p11_of_11_q, xout => u0_m0_wo0_adelay_p10_q, ena => d_u0_m0_wo0_compute_q_35_q(0), clk => clk, aclr => areset );

    -- d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_q_35(DELAY,404)@34 + 1
    d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_q_35 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_q, xout => d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_q_35_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_accum_BitSelect_for_a_tessel10_0(BITSELECT,193)@35
    u0_m0_wo0_accum_BitSelect_for_a_tessel10_0_b <= STD_LOGIC_VECTOR(d_u0_m0_wo0_mtree_mult1_0_result_add_1_0_p10_of_12_q_35_q(1 downto 1));

    -- u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_l(BITJOIN,195)@35
    u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_l_q <= u0_m0_wo0_accum_BitSelect_for_a_tessel10_0_b & u0_m0_wo0_accum_BitSelect_for_a_tessel10_0_b;

    -- u0_m0_wo0_accum_p11_of_11(ADD,101)@35 + 1
    u0_m0_wo0_accum_p11_of_11_cin <= u0_m0_wo0_accum_p10_of_11_c;
    u0_m0_wo0_accum_p11_of_11_a <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((2 downto 2 => u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_l_q(1)) & u0_m0_wo0_accum_BitSelect_for_a_BitJoin_for_l_q) & '1');
    u0_m0_wo0_accum_p11_of_11_b <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR((2 downto 2 => u0_m0_wo0_adelay_p10_q(1)) & u0_m0_wo0_adelay_p10_q) & u0_m0_wo0_accum_p11_of_11_cin(0));
    u0_m0_wo0_accum_p11_of_11_i <= u0_m0_wo0_accum_p11_of_11_a;
    u0_m0_wo0_accum_p11_of_11_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_accum_p11_of_11_o <= (others => '0');
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_35_q = "1") THEN
                IF (d_u0_m0_wo0_aseq_q_35_q = "1") THEN
                    u0_m0_wo0_accum_p11_of_11_o <= u0_m0_wo0_accum_p11_of_11_i;
                ELSE
                    u0_m0_wo0_accum_p11_of_11_o <= STD_LOGIC_VECTOR(SIGNED(u0_m0_wo0_accum_p11_of_11_a) + SIGNED(u0_m0_wo0_accum_p11_of_11_b));
                END IF;
            END IF;
        END IF;
    END PROCESS;
    u0_m0_wo0_accum_p11_of_11_q <= u0_m0_wo0_accum_p11_of_11_o(2 downto 1);

    -- d_u0_m0_wo0_accum_p10_of_11_q_36(DELAY,365)@35 + 1
    d_u0_m0_wo0_accum_p10_of_11_q_36 : dspba_delay
    GENERIC MAP ( width => 6, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_accum_p10_of_11_q, xout => d_u0_m0_wo0_accum_p10_of_11_q_36_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_accum_p9_of_11_q_36(DELAY,364)@34 + 2
    d_u0_m0_wo0_accum_p9_of_11_q_36 : dspba_delay
    GENERIC MAP ( width => 6, depth => 2, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_accum_p9_of_11_q, xout => d_u0_m0_wo0_accum_p9_of_11_q_36_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_accum_p8_of_11_q_36(DELAY,363)@33 + 3
    d_u0_m0_wo0_accum_p8_of_11_q_36 : dspba_delay
    GENERIC MAP ( width => 6, depth => 3, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_accum_p8_of_11_q, xout => d_u0_m0_wo0_accum_p8_of_11_q_36_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_accum_p7_of_11_q_36(DELAY,362)@32 + 4
    d_u0_m0_wo0_accum_p7_of_11_q_36 : dspba_delay
    GENERIC MAP ( width => 6, depth => 4, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_accum_p7_of_11_q, xout => d_u0_m0_wo0_accum_p7_of_11_q_36_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_accum_p6_of_11_q_36(DELAY,361)@31 + 5
    d_u0_m0_wo0_accum_p6_of_11_q_36 : dspba_delay
    GENERIC MAP ( width => 6, depth => 5, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_accum_p6_of_11_q, xout => d_u0_m0_wo0_accum_p6_of_11_q_36_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_accum_p5_of_11_q_36(DELAY,360)@30 + 6
    d_u0_m0_wo0_accum_p5_of_11_q_36 : dspba_delay
    GENERIC MAP ( width => 6, depth => 6, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_accum_p5_of_11_q, xout => d_u0_m0_wo0_accum_p5_of_11_q_36_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_accum_p4_of_11_q_36(DELAY,359)@29 + 7
    d_u0_m0_wo0_accum_p4_of_11_q_36 : dspba_delay
    GENERIC MAP ( width => 6, depth => 7, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_accum_p4_of_11_q, xout => d_u0_m0_wo0_accum_p4_of_11_q_36_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_accum_p3_of_11_q_36(DELAY,358)@28 + 8
    d_u0_m0_wo0_accum_p3_of_11_q_36 : dspba_delay
    GENERIC MAP ( width => 6, depth => 8, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_accum_p3_of_11_q, xout => d_u0_m0_wo0_accum_p3_of_11_q_36_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_accum_p2_of_11_q_36(DELAY,357)@27 + 9
    d_u0_m0_wo0_accum_p2_of_11_q_36 : dspba_delay
    GENERIC MAP ( width => 6, depth => 9, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_accum_p2_of_11_q, xout => d_u0_m0_wo0_accum_p2_of_11_q_36_q, clk => clk, aclr => areset );

    -- d_u0_m0_wo0_accum_p1_of_11_q_36(DELAY,356)@26 + 10
    d_u0_m0_wo0_accum_p1_of_11_q_36 : dspba_delay
    GENERIC MAP ( width => 6, depth => 10, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_accum_p1_of_11_q, xout => d_u0_m0_wo0_accum_p1_of_11_q_36_q, clk => clk, aclr => areset );

    -- u0_m0_wo0_accum_BitJoin_for_q(BITJOIN,102)@36
    u0_m0_wo0_accum_BitJoin_for_q_q <= u0_m0_wo0_accum_p11_of_11_q & d_u0_m0_wo0_accum_p10_of_11_q_36_q & d_u0_m0_wo0_accum_p9_of_11_q_36_q & d_u0_m0_wo0_accum_p8_of_11_q_36_q & d_u0_m0_wo0_accum_p7_of_11_q_36_q & d_u0_m0_wo0_accum_p6_of_11_q_36_q & d_u0_m0_wo0_accum_p5_of_11_q_36_q & d_u0_m0_wo0_accum_p4_of_11_q_36_q & d_u0_m0_wo0_accum_p3_of_11_q_36_q & d_u0_m0_wo0_accum_p2_of_11_q_36_q & d_u0_m0_wo0_accum_p1_of_11_q_36_q;

    -- u0_m0_wo0_oseq(SEQUENCE,46)@33 + 1
    u0_m0_wo0_oseq_clkproc: PROCESS (clk, areset)
        variable u0_m0_wo0_oseq_c : SIGNED(8 downto 0);
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_oseq_c := "001001000";
            u0_m0_wo0_oseq_q <= "0";
            u0_m0_wo0_oseq_eq <= '0';
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (d_u0_m0_wo0_compute_q_33_q = "1") THEN
                IF (u0_m0_wo0_oseq_c = "111111111") THEN
                    u0_m0_wo0_oseq_eq <= '1';
                ELSE
                    u0_m0_wo0_oseq_eq <= '0';
                END IF;
                IF (u0_m0_wo0_oseq_eq = '1') THEN
                    u0_m0_wo0_oseq_c := u0_m0_wo0_oseq_c + 73;
                ELSE
                    u0_m0_wo0_oseq_c := u0_m0_wo0_oseq_c - 1;
                END IF;
                u0_m0_wo0_oseq_q <= STD_LOGIC_VECTOR(u0_m0_wo0_oseq_c(8 downto 8));
            END IF;
        END IF;
    END PROCESS;

    -- u0_m0_wo0_oseq_gated(LOGICAL,47)@34
    u0_m0_wo0_oseq_gated_q <= u0_m0_wo0_oseq_q and d_u0_m0_wo0_compute_q_34_q;

    -- u0_m0_wo0_oseq_gated_reg(REG,48)@34 + 1
    u0_m0_wo0_oseq_gated_reg_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            u0_m0_wo0_oseq_gated_reg_q <= "0";
        ELSIF (clk'EVENT AND clk = '1') THEN
            u0_m0_wo0_oseq_gated_reg_q <= STD_LOGIC_VECTOR(u0_m0_wo0_oseq_gated_q);
        END IF;
    END PROCESS;

    -- outchan(COUNTER,53)@35 + 1
    -- low=0, high=1, step=1, init=1
    outchan_clkproc: PROCESS (clk, areset)
    BEGIN
        IF (areset = '1') THEN
            outchan_i <= TO_UNSIGNED(1, 1);
        ELSIF (clk'EVENT AND clk = '1') THEN
            IF (u0_m0_wo0_oseq_gated_reg_q = "1") THEN
                outchan_i <= outchan_i + 1;
            END IF;
        END IF;
    END PROCESS;
    outchan_q <= STD_LOGIC_VECTOR(STD_LOGIC_VECTOR(RESIZE(outchan_i, 2)));

    -- d_out0_m0_wo0_assign_id3_q_36(DELAY,348)@35 + 1
    d_out0_m0_wo0_assign_id3_q_36 : dspba_delay
    GENERIC MAP ( width => 1, depth => 1, reset_kind => "ASYNC" )
    PORT MAP ( xin => u0_m0_wo0_oseq_gated_reg_q, xout => d_out0_m0_wo0_assign_id3_q_36_q, clk => clk, aclr => areset );

    -- xOut(PORTOUT,54)@36 + 1
    xOut_v <= d_out0_m0_wo0_assign_id3_q_36_q;
    xOut_c <= STD_LOGIC_VECTOR("000000" & outchan_q);
    xOut_0 <= u0_m0_wo0_accum_BitJoin_for_q_q;

END normal;
