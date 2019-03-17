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
-- $RCSfile: $
-- $Source: $
--
-- $Revision: #1 $
-- $Date: 2018/07/18 $
-- Check in by     : $Author: psgswbuild $
-- Author   :   Volker Mauer
--
-- Project      :  auk_dspip_fastaddsub
--
-- Description : 
--
-- This functional unit can be used to perform an addition or subtraction
-- in a single clock cycle faster than in a standard adder, by means of a
-- carry select structure.
--
-- This means that the adder is split into a lower part and an upper part.
-- The upper part is implemented twice:  Once with the carry_in fixed to '0'
-- and once with the carry_in fixed to '1'.  The carry_out of the lower part
-- is used to control a mux selecting the two possible upper results. 
--
-- This structucture is useful where the width is so great that the standard
-- adder would be slower than the DSP block, and a pipelined adder cannot be
-- used, like in accumulators.
--
-- There is a device dependency:  The width of the lower part, where 1 LE per
-- bit is required, should be greater than half the overall width, and
-- ideally be selected to fit into labs of the target device.  The upper
-- half, where 3 LEs per bit are needed, should add the remaining bits. 
--
-- Given the width and the labwidth, the function will automatically calculate
-- the width of lower and upper parts using the following formula:
--
-- Wlow = Wlab * ceil(Win / (2*Wlab))
-- Whigh = Win-Wlow
--
-- Example:  If the adder is 48 bits in, the lab width is 10, we would
--           get Wlow = 30, Whigh=18.
-- 
-- ALTERA Confidential and Proprietary
-- Copyright 2006 (c) Altera Corporation
-- All rights reserved
--
-------------------------------------------------------------------------
-------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity auk_dspip_fastaddsub is
  generic (
    INWIDTH_g  :     natural := 18;
    LABWIDTH_g :     natural := 16);
    -- width of lab in selected device ( 10 or 16 in Cyclone,
    -- Cylone II, Stratix and Stratix II.  Don't know
    -- Stratix III yet.
  port (
    datain1    : in  std_logic_vector(INWIDTH_g-1 downto 0);
    datain2    : in  std_logic_vector(INWIDTH_g-1 downto 0);
    add_nsub   : in std_logic;
    clk        : in  std_logic;
    enable     : in  std_logic;
    reset      : in  std_logic;
    dataout    : out std_logic_vector(INWIDTH_g downto 0));
end entity auk_dspip_fastaddsub;


architecture beh of auk_dspip_fastaddsub is

  constant LOWWIDTH_c  : natural := LABWIDTH_g * (1 + integer(INWIDTH_g/(LABWIDTH_g*2)));
  constant asdf : natural := integer(INWIDTH_g/(LABWIDTH_g*2));
  constant asdf2 : natural := 1+integer(INWIDTH_g/(LABWIDTH_g*2));
  constant asdf3 : natural := LABWIDTH_g * (1+integer(INWIDTH_g/(LABWIDTH_g*2)));
  
  constant HIGHWIDTH_c : natural := INWIDTH_g-LOWWIDTH_c;


  signal datain1_low      : std_logic_vector(LOWWIDTH_c-1 downto 0);
  signal datain1_high     : std_logic_vector(HIGHWIDTH_c-1 downto 0);
  signal datain2_low      : std_logic_vector(LOWWIDTH_c-1 downto 0);
  signal datain2_high     : std_logic_vector(HIGHWIDTH_c-1 downto 0);
  signal result_low       : std_logic_vector(LOWWIDTH_c downto 0);
  signal result_high_cin0 : std_logic_vector(HIGHWIDTH_c downto 0);
  signal result_high_cin1 : std_logic_vector(HIGHWIDTH_c downto 0);

  -----------------------------------------------------------------------------
  -- Synthesis tools have got clever.  Need to set "keep" attribute, or the
  -- synthesis tool will recognise the structure as a standard adder, and
  -- optimise it accordingly.
  -----------------------------------------------------------------------------
  attribute keep : boolean;
  attribute keep of result_high_cin0 : signal is true;
  attribute keep of result_high_cin1 : signal is true;

begin  -- architecture beh

  -----------------------------------------------------------------------------
  -- split inputs into upper and lower halves
  -----------------------------------------------------------------------------
  datain1_low  <= datain1(LOWWIDTH_c-1 downto 0);
  datain1_high <= datain1(HIGHWIDTH_c+LOWWIDTH_c-1 downto LOWWIDTH_c);
  datain2_low  <= datain2(LOWWIDTH_c-1 downto 0);
  datain2_high <= datain2(HIGHWIDTH_c+LOWWIDTH_c-1 downto LOWWIDTH_c);

  -----------------------------------------------------------------------------
  -- perform separate additions/subtractions for upper and lower halves
  -----------------------------------------------------------------------------
  result_low       <= std_logic_vector(unsigned('0' & datain1_low) + unsigned('0' & datain2_low)) when add_nsub='1' else
                      std_logic_vector(unsigned('0' & datain1_low) - unsigned('0' & datain2_low));
  result_high_cin0 <= std_logic_vector(unsigned(datain1_high(HIGHWIDTH_c-1) & datain1_high) +
                                       unsigned(datain2_high(HIGHWIDTH_c-1) & datain2_high)) when add_nsub='1' else
                      std_logic_vector(unsigned(datain1_high(HIGHWIDTH_c-1) & datain1_high) -
                                       unsigned(datain2_high(HIGHWIDTH_c-1) & datain2_high));
  result_high_cin1 <= std_logic_vector(unsigned(datain1_high(HIGHWIDTH_c-1) & datain1_high )+
                                       unsigned(datain2_high(HIGHWIDTH_c-1) & datain2_high )+
                                       1) when add_nsub = '1' else
                      std_logic_vector(unsigned(datain1_high(HIGHWIDTH_c-1) & datain1_high )-
                                       unsigned(datain2_high(HIGHWIDTH_c-1) & datain2_high )-
                                       1);

  -----------------------------------------------------------------------------
  -- multiplex (select) the upper half, register selected upper half and lower
  -- half.
  -----------------------------------------------------------------------------
  adder_process : process (clk, reset) is
  begin  -- process adder_process
    if reset = '1' then                 -- asynchronous reset (active high)
      dataout     <= (others => '0');
    elsif rising_edge(clk) then         -- rising clock edge
      if enable = '1' then
        if result_low(LOWWIDTH_c) = '1' then
          dataout <= result_high_cin1 & result_low(LOWWIDTH_c-1 downto 0);
        else
          dataout <= result_high_cin0 & result_low(LOWWIDTH_c-1 downto 0);
        end if;
      end if;
    end if;
  end process adder_process;


end architecture beh;
