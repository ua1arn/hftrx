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
-- $RCSfile: auk_dspip_text_pkg.vhd,v $
-- $Source: /cvs/uksw/dsp_cores/lib/packages/auk_dspip_text_pkg.vhd,v $
--
-- $Revision: #1 $
-- $Date: 2018/07/18 $
-- Check in by     : $Author: psgswbuild $
-- Author   :  DSP_IP
--
-- Project      :  <project name>
--
-- Description : 
--
-- Common functions for DSP_IP cores. 
-- 
--
-- $Log: auk_dspip_text_pkg.vhd,v $
-- Revision 1.2  2007/05/04 15:33:11  sdemirso
-- merge from 7.1
--
-- Revision 1.1  2007/02/01 17:29:45  kmarks
-- Initial commit
--
-- Revision 1.5  2006/08/17 10:13:02  sdemirso
-- log2_ceil_one function added
--
-- Revision 1.4  2006/07/28 18:52:50  sdemirso
-- no compilation errors with the new directory structure
--
-- Revision 1.3  2006/07/28 10:27:30  sdemirso
-- Header updated
--
-- ALTERA Confidential and Proprietary
-- Copyright 2006 (c) Altera Corporation
-- All rights reserved
--
-------------------------------------------------------------------------
-------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.std_logic_1164.all;
use ieee.numeric_std.all;
PACKAGE auk_dspip_text_pkg IS


  -----------------------------------------------------------------------------
  -- NOTE that these log functions are not intended to synthesize directly
  -- into hardware, rather they are used to generate constants for
  -- synthesized hardware.
  -----------------------------------------------------------------------------
  type array_natural_t is array (natural range <>) of integer;

  function parse_string_array (str : string; size : natural) return array_natural_t;

  function str_to_int (str : string; base : string) return integer;

END PACKAGE auk_dspip_text_pkg;

package body auk_dspip_text_pkg is
  ---------------------------------------------------------------------------
  -- str_to_int Function. Only parses positive decimal values
  ---------------------------------------------------------------------------
  function str_to_int(str : string; base : string) return integer is
    variable res  : integer;
    variable base_cnt : integer;
  begin
    res := 0;
    base_cnt:=1;
    for i in str'length downto 1 loop
      if str(i) /= ' ' then
        res := res + (character'pos(str(i)) - character'pos('0'))*base_cnt;
        base_cnt:=base_cnt*10;
      end if;
    end loop;  -- i
    return res;
  end str_to_int;

  ---------------------------------------------------------------------------
  -- parse_string_array Function.
  ---------------------------------------------------------------------------
  function parse_string_array(str : string; size : natural) return array_natural_t is
    variable this_str : string(1 to 32);
    variable cnt_char : natural;         -- how many characters have we seen
    variable cnt_str : natural:=0;
    variable res : array_natural_t(0 to size-1);
  begin
    this_str := (others => ' ');
    cnt_char := 1;
    for i in str'left to str'right loop
      if str(i) = ','  then
        res(cnt_str) := str_to_int(this_str, "DEC");
        cnt_char := 1;
        cnt_str := cnt_str + 1;
        this_str := (others => ' ');
      else
        this_str(cnt_char) := str(i);
        cnt_char := cnt_char+1;
      end if;
    end loop;  -- i
     res(cnt_str) := str_to_int(this_str, "DEC");
    return res;
  end parse_string_array;

  end package body auk_dspip_text_pkg;
