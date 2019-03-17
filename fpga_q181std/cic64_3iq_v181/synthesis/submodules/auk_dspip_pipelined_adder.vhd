-- (C) 2001-2018 Intel Corporation. All rights reserved.
-- Your use of Intel Corporation's design tools, logic functions and other 
-- software and tools, and its AMPP partner logic functions, and any output 
-- files from any of the foregoing (including device programming or simulation 
-- files), and any associated documentation or information are expressly subject 
-- to the terms and conditions of the Intel Program License Subscription 
-- Agreement, Intel FPGA IP License Agreement, or other applicable 
-- license agreement, including, without limitation, that your use is for the 
-- sole purpose of programming logic devices manufactured by Intel and sold by 
-- Intel or its authorized distributors.  Please refer to the applicable 
-- agreement for further details.


-------------------------------------------------------------------------
-------------------------------------------------------------------------
--
-- Revision Control Information
--
-- $RCSfile: auk_dspip_fast_accumulator.vhd,v $
-- $Source: /cvs/uksw/dsp_cores/lib/fu/fast_accum/rtl/auk_dspip_fast_accumulator.vhd,v $
--
-- $Revision: #1 $
-- $Date: 2018/07/18 $
-- Check in by     : $Author: psgswbuild $
-- Author   :  Alex Diaz-Manero
--
-- Project      :  FIR
--
-- Description : 
--
-- Fast pipelined Accumulator for fast filters 
-- 
--
-- $Log: auk_dspip_fast_accumulator.vhd,v $
-- Revision 1.2  2007/10/09 19:08:16  admanero
-- slight mod to avoid propagation of X through Sum at MSB bits
--
-- Revision 1.1  2007/09/24 16:59:34  admanero
-- first revision of fast (pipelined) accumulator.
--
--
-- ALTERA Confidential and Proprietary
-- Copyright 2007 (c) Altera Corporation
-- All rights reserved
--
-------------------------------------------------------------------------
-------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library work;
use work.auk_dspip_math_pkg.all;
--use auk_dspip_lib.auk_dspip_lib_pkg.all;


entity auk_dspip_pipelined_adder is
  generic (
	  -- This adder assumes that both operands  datain1 and datain2 are of the same data width
		-- if that is not the case and the shorter word is (sign) expanded to the size of the bigger then
		-- a penalty is incurred in the pipelined implementation.
		--  But for the time being I only need the same size  :-)  So don't make your life too difficult
    INWIDTH_g      : natural := 42;
		-- width of lab in selected device ( 10 or 16 in Cyclone,
    -- Cylone II, Stratix and Stratix II.  
		--  Alex :  should I use 19 bits for Stratix III? 
    -- The rational being 10 ALM (2 bits x ALM  + the carry chain inside the same LAB for efficiency.
		--  Shall I have family generic to set labwidth?
		LABWIDTH_g :     natural := 38);
  port (
		datain1    : in  std_logic_vector(INWIDTH_g-1 downto 0);
    datain2    : in  std_logic_vector(INWIDTH_g-1 downto 0);
    clk        : in  std_logic;
    enable     : in  std_logic;
    reset      : in  std_logic;
    dataout    : out std_logic_vector(INWIDTH_g downto 0));
end auk_dspip_pipelined_adder;


architecture rtl of auk_dspip_pipelined_adder is
  
	type NATURAL_ARRAY is array(NATURAL RANGE <>) of NATURAL;

	-- IN_OR_OUT  is  0 for IN and 1 for ADD (OUT)
  function calc_number_of_stages(FULL_WIDTH, LABWIDTH, IN_OR_OUT : natural) return natural is
		variable tmp : natural;
		variable result : natural;
	begin
		tmp := FULL_WIDTH;
		result := 0;
		while tmp > LABWIDTH+IN_OR_OUT loop
			tmp := tmp - LABWIDTH;
			result := result + 1;
		end loop;
		
		return result;
	end calc_number_of_stages;
	
	
	function calc_width_of_stages(FULL_WIDTH, LABWIDTH, IN_OR_OUT : natural) return natural_array is
		Constant STAGES_c : natural := calc_number_of_stages(FULL_WIDTH, LABWIDTH, IN_OR_OUT);
		variable tmp : natural;
		variable result : natural_array(stages_c downto 0);
	begin
		tmp := FULL_WIDTH;
		for I in 0 to STAGES_c loop
			if I=STAGES_c then
				result(I) := tmp;
			else
				result(I) := LABWIDTH;
				tmp := tmp - LABWIDTH;
			end if;
		end loop;
		return result;
	
	end calc_width_of_stages;
	
-- This stages_c  will be either 0, 1 or 2. Rarely it will be 3.
--  39 "divided" by 19 still should yield just 1 (2 stages). but for 40 it should 2 (3 stages)
-- So, it should be a function not a just a division.
-- For an adder, for simplicity, shouldn't I have the same stages for input and output?  Well, leave it like this anyways
	Constant STAGES_ADD_c : natural := calc_number_of_stages(INWIDTH_g+1, LABWIDTH_g, 1); --ACCUM_OUT_WIDTH_g / LABWIDTH_g;
	Constant WIDTH_STAGES_ADD_c : natural_array(STAGES_ADD_c downto 0) := calc_width_of_stages(INWIDTH_g+1, LABWIDTH_g, 1);
	Constant STAGES_IN_c : natural := calc_number_of_stages(INWIDTH_g, LABWIDTH_g, 0); 
	Constant WIDTH_STAGES_IN_c : natural_array(STAGES_IN_c downto 0) := calc_width_of_stages(INWIDTH_g, LABWIDTH_g, 0);
	
	type TRIANGLE_DELAY_t is array (natural range<>, natural range<>) of std_logic_vector(LABWIDTH_g-1 downto 0);
	type TRIANGLE_DELAY_P1_t is array (natural range<>, natural range<>) of std_logic_vector(LABWIDTH_g downto 0);
	type COLUMN_VECTOR_t is array (natural range<>) of std_logic_vector(LABWIDTH_g downto 0);
	
	signal  input1_side_data_scaled_delay_q : TRIANGLE_DELAY_t(STAGES_IN_c  downto 0, STAGES_IN_c  downto 0);
	signal  input1_side_data_scaled_delay_d : TRIANGLE_DELAY_t(STAGES_IN_c  downto 0, STAGES_IN_c  downto 0);
	signal  input2_side_data_scaled_delay_q : TRIANGLE_DELAY_t(STAGES_IN_c  downto 0, STAGES_IN_c  downto 0);
	signal  input2_side_data_scaled_delay_d : TRIANGLE_DELAY_t(STAGES_IN_c  downto 0, STAGES_IN_c  downto 0);
	signal output_side_data_scaled_delay_q : TRIANGLE_DELAY_P1_t(STAGES_ADD_c downto 0, STAGES_ADD_c downto 0);
	signal output_side_data_scaled_delay_d : TRIANGLE_DELAY_P1_t(STAGES_ADD_c downto 0, STAGES_ADD_c downto 0);
	signal datai1_int : COLUMN_VECTOR_t(STAGES_ADD_c downto 0);
	signal datai2_int : COLUMN_VECTOR_t(STAGES_ADD_c downto 0);
	signal datao_int  : COLUMN_VECTOR_t(STAGES_ADD_c downto 0);
	signal cout_cin_d       : std_logic_vector(STAGES_ADD_c+1 downto 0);
	signal cout_cin_q       : std_logic_vector(STAGES_ADD_c+1 downto 0);
	signal enb 							: std_logic;
  

begin

  enb <= enable;

  ifg4: if STAGES_ADD_c>0 generate
	 reg2 : process(clk, reset) is
	 begin  
		 if reset = '1' then
			 cout_cin_q(STAGES_ADD_c+1 downto 1) <= (others => '0');
		 elsif rising_edge(clk) then 
			 if enb = '1' then
				 cout_cin_q(STAGES_ADD_c+1 downto 1) <= cout_cin_d(STAGES_ADD_c+1 downto 1);
			 end if;
		 end if;
	 end process reg2;
	end generate ifg4;
	
	cout_cin_q(0) <= '0';
	
	fg1: for J in 0 to STAGES_IN_c generate
	   
		fg1c: for I in 0 to STAGES_IN_c generate
			ifg6b: if I <= 1 generate
				input1_side_data_scaled_delay_d(I, J)(WIDTH_STAGES_IN_c(J)-1 downto 0) <= datain1(WIDTH_STAGES_IN_c(J)-1 + J*LABWIDTH_g downto J*LABWIDTH_g);
				input2_side_data_scaled_delay_d(I, J)(WIDTH_STAGES_IN_c(J)-1 downto 0) <= datain2(WIDTH_STAGES_IN_c(J)-1 + J*LABWIDTH_g downto J*LABWIDTH_g);
			end generate ifg6b;
			ifg7b: if I > 1 generate
				input1_side_data_scaled_delay_d(I, J)(LABWIDTH_g-1 downto 0) <= input1_side_data_scaled_delay_q(I-1, J)(LABWIDTH_g-1 downto 0);
				input2_side_data_scaled_delay_d(I, J)(LABWIDTH_g-1 downto 0) <= input2_side_data_scaled_delay_q(I-1, J)(LABWIDTH_g-1 downto 0);
			end generate ifg7b;
		end generate fg1c;
		
	end generate fg1;
	
	reg : process(clk, reset) is
	 begin  
		 if reset = '1' then
		   loop1: for I in 1 to STAGES_IN_c loop
			 	loop2: for J in I to STAGES_IN_c loop
				input1_side_data_scaled_delay_q(I, J) <= (others => '0');
				input2_side_data_scaled_delay_q(I, J) <= (others => '0');
				end loop loop2;
			end loop loop1;
		 elsif rising_edge(clk) then 
			 if enb = '1' then
			 loop3: for I in 1 to STAGES_IN_c loop
				 loop4: for J in I to STAGES_IN_c loop
					input1_side_data_scaled_delay_q(I, J) <= input1_side_data_scaled_delay_d(I, J);
					input2_side_data_scaled_delay_q(I, J) <= input2_side_data_scaled_delay_d(I, J);
				 end loop loop4;
				end loop loop3;
			 end if;
		 end if;
	 end process reg;
	 
	 adding: process(cout_cin_q, datai1_int, datai2_int)
	    variable tmp_debug_s : signed(LABWIDTH_g downto 0);
			variable tmp_debug_u : unsigned(LABWIDTH_g downto 0);
		begin
		  --Only the upper most chunk sum is signed, the other chunks ought to be unsigned
		  for I in 0 to STAGES_ADD_c loop
				if I=STAGES_ADD_c then
					if cout_cin_q(I)='1' then
						--datao_int(I) <= signed(datai_int(I)) + signed(datas_int(I)) + natural(1);
						tmp_debug_s(WIDTH_STAGES_ADD_c(I)-1 downto 0) := signed(datai1_int(I)(WIDTH_STAGES_ADD_c(I)-1 downto 0)) + signed(datai2_int(I)(WIDTH_STAGES_ADD_c(I)-1 downto 0)) + natural(1);
					else
						--datao_int(I) <= signed(datai_int(I)) + signed(datas_int(I)) + natural(0);
						tmp_debug_s(WIDTH_STAGES_ADD_c(I)-1 downto 0) := signed(datai1_int(I)(WIDTH_STAGES_ADD_c(I)-1 downto 0)) + signed(datai2_int(I)(WIDTH_STAGES_ADD_c(I)-1 downto 0)) + natural(0);
					end if;
					
					datao_int(I) <= std_logic_vector(tmp_debug_s);
					cout_cin_d(I+1) <= tmp_debug_s(LABWIDTH_g);
				else
					if cout_cin_q(I)='1' then
						--datao_int(I) <= unsigned(datai_int(I)) + unsigned(datas_int(I)) + natural(1);
						tmp_debug_u := unsigned(datai1_int(I)) + unsigned(datai2_int(I)) + natural(1);
					else
						--datao_int(I) <= unsigned(datai_int(I)) + unsigned(datas_int(I)) + natural(0);
						tmp_debug_u := unsigned(datai1_int(I)) + unsigned(datai2_int(I)) + natural(0);
					end if;
					datao_int(I) <= std_logic_vector(tmp_debug_u);
					cout_cin_d(I+1) <= tmp_debug_u(LABWIDTH_g);
				end if;
			end loop;
		end process adding;
	
	
	 --Always  STAGES_ADD_c >= STAGES_IN_c
	 --when it is greater it needs special handling
	fg1b: for I in 0 to STAGES_ADD_c generate
		
		 --I need to split  STAGES_ADD_c  from STAGES_IN_c
		fg2: for J in 0 to I generate
		  ifg1a: if J=0 and I=0 generate
			   --In reality the chunks below the MSB are unsigned. Only the upper most (MSB) chunk is signed
				 -- 
				datai1_int(I)(WIDTH_STAGES_IN_c(I) downto 0) <= std_logic_vector('0' & input1_side_data_scaled_delay_d(0, 0)(WIDTH_STAGES_IN_c(I)-1 downto 0));
				datai2_int(I)(WIDTH_STAGES_IN_c(I) downto 0) <= std_logic_vector('0' & input2_side_data_scaled_delay_d(0, 0)(WIDTH_STAGES_IN_c(I)-1 downto 0));
			end generate ifg1a;
		 	ifg1: if J=I and I<STAGES_IN_c and I>0 and J>0 generate
			   --In reality the chunks below the MSB are unsigned. Only the upper most (MSB) chunk is signed
				datai1_int(I)(WIDTH_STAGES_IN_c(I) downto 0) <= std_logic_vector('0' & input1_side_data_scaled_delay_q(I, J)(WIDTH_STAGES_IN_c(I)-1 downto 0));
				datai2_int(I)(WIDTH_STAGES_IN_c(I) downto 0) <= std_logic_vector('0' & input2_side_data_scaled_delay_q(I, J)(WIDTH_STAGES_IN_c(I)-1 downto 0));
			end generate ifg1;
			--------
			--Could it be possible to have STAGES_ADD_c > STAGES_IN_c ? yes, it could  => sign extension stages needed herefor datai_int
			ifg2a: if J=I and I>=STAGES_IN_c and I<STAGES_ADD_c generate -- this only generated if (STAGES_ADD_c > STAGES_IN_c)
				datai1_int(I)(LABWIDTH_g downto 0) <= std_logic_vector(sign_extend(signed(input1_side_data_scaled_delay_q(STAGES_IN_c, J)(WIDTH_STAGES_IN_c(STAGES_IN_c)-1 downto 0)), LABWIDTH_g+1));
				datai2_int(I)(LABWIDTH_g downto 0) <= std_logic_vector(sign_extend(signed(input2_side_data_scaled_delay_q(STAGES_IN_c, J)(WIDTH_STAGES_IN_c(STAGES_IN_c)-1 downto 0)), LABWIDTH_g+1));
				--this last chunk doesn't need sign extension, it would need it if we wanted carry out (i.e. for overflow) 
			end generate ifg2a;
			--------
			-- Only the MSB chunk needs to be signed. Question: Is there a need for a pipelined accumulator with unsigned data?
			ifg2: if J=I and I=STAGES_ADD_c generate
				datai1_int(I)(LABWIDTH_g downto 0) <= std_logic_vector(sign_extend(signed(input1_side_data_scaled_delay_q(STAGES_IN_c, J)(WIDTH_STAGES_IN_c(STAGES_IN_c)-1 downto 0)), LABWIDTH_g+1));
				datai2_int(I)(LABWIDTH_g downto 0) <= std_logic_vector(sign_extend(signed(input2_side_data_scaled_delay_q(STAGES_IN_c, J)(WIDTH_STAGES_IN_c(STAGES_IN_c)-1 downto 0)), LABWIDTH_g+1));
				--this last chunk doesn't need sign extension, it would need it if we wanted carry out (i.e. for overflow) 
			end generate ifg2;
		end generate fg2;
		
			
		-- output connection, MSB chunk from _d  the remainder from _q
		ifg3: if I=STAGES_ADD_c generate
		   dataout(WIDTH_STAGES_ADD_c(I)-1+LABWIDTH_g*I downto LABWIDTH_g*I) <= output_side_data_scaled_delay_d(STAGES_ADD_c-I, I)(WIDTH_STAGES_ADD_c(I)-1 downto 0);
		end generate ifg3;
		ifg4: if I<STAGES_ADD_c generate
		   dataout(WIDTH_STAGES_ADD_c(I)-1+LABWIDTH_g*I downto LABWIDTH_g*I) <= output_side_data_scaled_delay_q(STAGES_ADD_c-I, I)(WIDTH_STAGES_ADD_c(I)-1 downto 0);
		end generate ifg4;
		
	end generate fg1b;
	
	
	fg3a: for J in 0 to STAGES_ADD_c generate
	  fg3b: for I in 0 to STAGES_ADD_c generate
			ifg6: if I <= 1 generate
		  	output_side_data_scaled_delay_d(I, J)(WIDTH_STAGES_ADD_c(J)-1 downto 0) <= datao_int(J)(WIDTH_STAGES_ADD_c(J)-1 downto 0);
			end generate ifg6;
			ifg7: if I > 1 generate
				output_side_data_scaled_delay_d(I, J)(LABWIDTH_g-1 downto 0) <= output_side_data_scaled_delay_q(I-1, J)(LABWIDTH_g-1 downto 0);
			end generate ifg7;
		end generate fg3b;
		
	end generate fg3a;
	-- 
	reg_o : process(clk, reset) is
	 begin  
		 if reset = '1' then
		   loop1: for I in 1 to STAGES_ADD_c loop
			 	loop2: for J in 0 to STAGES_ADD_c-I loop
				output_side_data_scaled_delay_q(I, J) <= (others => '0');
				end loop loop2;
			end loop loop1;
		 elsif rising_edge(clk) then 
			 if enb = '1' then
			 loop3: for I in 1 to STAGES_ADD_c loop
				 loop4: for J in 0 to STAGES_ADD_c-I loop
					output_side_data_scaled_delay_q(I, J) <= output_side_data_scaled_delay_d(I, J);
				 end loop loop4;
				end loop loop3;
			 end if;
		 end if;
	 end process reg_o;
	
end rtl;

