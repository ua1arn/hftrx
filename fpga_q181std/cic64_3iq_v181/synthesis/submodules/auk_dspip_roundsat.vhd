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
-- $RCSfile: auk_dspip_roundsat.vhd,v $
-- $Source: /cvs/uksw/dsp_cores/lib/fu/roundsat/rtl/auk_dspip_roundsat.vhd,v $
--
-- $Revision: #1 $
-- $Date: 2018/07/18 $
-- Check in by     : $Author: psgswbuild $
-- Author   :  Volker Mauer
--
-- Project      :  common FU library
--
-- Description : 
--
-- This functional unit can be used to implement various forms of
-- rounding, saturation and truncation.
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


entity auk_dspip_roundsat is
  generic (
    IN_WIDTH_g      : natural := 8;     -- data width
    OUT_WIDTH_g     : natural := 8;     -- data width
    ROUNDING_TYPE_g : string  := "TRUNCATE_LOW";
    LATENCY : natural := 1
    );

  port (
    clk     : in  std_logic;
    reset   : in  std_logic;
    enable  : in  std_logic;            -- global clock enable
    datain  : in  std_logic_vector(IN_WIDTH_g-1 downto 0);
    dataout : out std_logic_vector(OUT_WIDTH_g-1 downto 0));
end entity auk_dspip_roundsat;

architecture beh of auk_dspip_roundsat is

begin  -- architecture beh

  -----------------------------------------------------------------------------
  -- truncate low
  -----------------------------------------------------------------------------
  trunc_low: if ROUNDING_TYPE_g = "TRUNCATE_LOW" generate
  begin  -- generate trunc_low
    trunc_low_p: process (clk) is
    begin  -- process trunc_low_p
      if rising_edge(clk) then         -- rising clock edge
        if reset = '1' then                 -- asynchronous reset (active high)
          dataout <= (others => '0');
        else
          if enable = '1' then
            dataout <= datain(IN_WIDTH_g-1 downto IN_WIDTH_g-OUT_WIDTH_g);
          end if;
        end if;
      end if;
    end process trunc_low_p;
  end generate trunc_low;

  -----------------------------------------------------------------------------
  -- truncate high
  -----------------------------------------------------------------------------
  trunc_high: if ROUNDING_TYPE_g = "TRUNCATE_HIGH" generate
  begin  -- generate trunc_high
    trunc_high_p: process (clk) is
    begin  -- process trunc_high_p
      if rising_edge(clk) then         -- rising clock edge
        if reset = '1' then                 -- asynchronous reset (active high)
          dataout <= (others => '0');
        else
          if enable = '1' then
            dataout <= datain(OUT_WIDTH_g-1 downto 0);          
          end if;
        end if;
      end if;
    end process trunc_high_p;
  end generate trunc_high;

  -----------------------------------------------------------------------------
  -- saturation
  -----------------------------------------------------------------------------
  sat     : if ROUNDING_TYPE_g = "SATURATE" generate
  begin  -- generate sat
    sat_p : process (clk) is
    begin  -- process sat_p
      if rising_edge(clk) then       -- rising clock edge
        if reset = '1' then               -- synchronous reset (active high)
          dataout   <= (others => '0');
        else
          if enable = '1' then          
            if signed(datain) > 2**(OUT_WIDTH_g-1)-1 then
              dataout <= std_logic_vector(to_signed(2**(OUT_WIDTH_g-1)-1, OUT_WIDTH_g));
            elsif signed(datain) < -2**(OUT_WIDTH_g-1) then
              dataout <= std_logic_vector(to_signed(-2**(OUT_WIDTH_g-1), OUT_WIDTH_g));
            else
              dataout <= datain(OUT_WIDTH_g-1 downto 0);
            end if;
          end if;
        end if;
      end if;
    end process sat_p;
  end generate sat;

  -----------------------------------------------------------------------------
  -- symmetrical saturation
  -----------------------------------------------------------------------------
  satsym     : if ROUNDING_TYPE_g = "SATURATE_SYM" generate
  begin  -- generate satsym
    satsym_p : process (clk) is
    begin  -- process satsym_p
      if rising_edge(clk) then       -- rising clock edge
        if reset = '1' then               -- asynchronous reset (active high)
          dataout   <= (others => '0');
        else
          if enable = '1' then         
            if signed(datain) > 2**(OUT_WIDTH_g-1)-1 then
              dataout <= std_logic_vector(to_signed(2**(OUT_WIDTH_g-1)-1, OUT_WIDTH_g));
            elsif signed(datain) < -2**(OUT_WIDTH_g-1)+1 then
              dataout <= std_logic_vector(to_signed(-2**(OUT_WIDTH_g-1)+1, OUT_WIDTH_g));
            else
              dataout <= datain(OUT_WIDTH_g-1 downto 0);
            end if;
          end if;
        end if;
      end if;
    end process satsym_p;
  end generate satsym;

  -----------------------------------------------------------------------------
  -- simple rounding (always rounds up)
  -----------------------------------------------------------------------------
  round     : if ROUNDING_TYPE_g = "ROUND_UP" generate

  signal RB  : std_logic;               -- rounding bit (MSB of discarded bit)

  begin  -- generate round
    -----------------------------------------------------------------------------
    -- get relevant bits
    -----------------------------------------------------------------------------
    RB  <= datain(IN_WIDTH_g-OUT_WIDTH_g-1);
   
    round_p : process (clk) is
    begin  -- process round_p
      if rising_edge(clk) then       -- rising clock edge
        if reset = '1' then               -- asynchronous reset (active high)
          dataout   <= (others => '0');
        else
          if enable = '1' then          
            if RB = '1' then
              dataout <= std_logic_vector(signed(datain(IN_WIDTH_g-1 downto IN_WIDTH_g-OUT_WIDTH_g))+1);
            else
              dataout <= datain(IN_WIDTH_g-1 downto IN_WIDTH_g-OUT_WIDTH_g);
            end if;
          end if;
        end if;
      end if;
    end process round_p;
  end generate round;

  -----------------------------------------------------------------------------
  -- round towards 0
  -----------------------------------------------------------------------------
  round0 : if ROUNDING_TYPE_g = "ROUND0" generate
  signal SB  : std_logic;               -- sign bit
  begin  -- generate round0

  SB  <= datain(IN_WIDTH_g-1);

    round0_p : process (clk) is
      variable OR_accu : std_logic := '0';
    begin  -- process round0_p
      if rising_edge(clk) then       -- rising clock edge
        if reset = '1' then               -- asynchronous reset (active high)
          dataout   <= (others => '0');
        else
          if enable = '1' then
            OR_accu := '0';
            for i in 0 to IN_WIDTH_g-OUT_WIDTH_g-1 loop
              OR_accu := OR_accu or datain(i);
            end loop;  -- i
            if SB = '1' and OR_accu = '1' then
              dataout <= std_logic_vector(signed(datain(IN_WIDTH_g-1 downto IN_WIDTH_g-OUT_WIDTH_g))+1);
            else
              dataout <= datain(IN_WIDTH_g-1 downto IN_WIDTH_g-OUT_WIDTH_g);
            end if;
          end if;
        end if;
      end if;
    end process round0_p;
  end generate round0;

  -----------------------------------------------------------------------------
  -- round away from 0
  -----------------------------------------------------------------------------
  round_up_sym : if ROUNDING_TYPE_g = "ROUND_UP_SYM" generate
  signal SB  : std_logic;               -- sign bit
  signal SB_delayed  : std_logic;       -- sign bit, delayed
  signal RB  : std_logic;               -- rounding bit (MSB of discarded bit)
  signal dataout_temp : std_logic_vector(OUT_WIDTH_g-1 downto 0);  
                                        -- internal, readable version of dataoout

  begin  -- generate round_up_sym

  SB  <= datain(IN_WIDTH_g-1);
    RB  <= datain(IN_WIDTH_g-OUT_WIDTH_g-1);

    round_up_sym_p : process (clk) is
      variable OR_accu : std_logic := '0';
    begin  -- process round_up_sym_p
      if rising_edge(clk) then       -- rising clock edge
        if reset = '1' then               -- asynchronous reset (active high)
          dataout_temp   <= (others => '0');
          SB_delayed <= '0';
        else
          if enable = '1' then
            SB_delayed <= SB;
            OR_accu := '0';
            for i in 0 to IN_WIDTH_g-OUT_WIDTH_g-1 loop
              OR_accu := OR_accu or datain(i);
            end loop;  -- i
            if (SB = '0' and RB = '1') or (SB = '1' and RB='1' and OR_accu='1') then
              dataout_temp <= std_logic_vector(signed(datain(IN_WIDTH_g-1 downto IN_WIDTH_g-OUT_WIDTH_g))+1);
            else
              dataout_temp <= datain(IN_WIDTH_g-1 downto IN_WIDTH_g-OUT_WIDTH_g);
            end if;
          end if;
        end if;
      end if;
    end process round_up_sym_p;

    dataout <= dataout_temp;

    assert not(SB_delayed = '0' and dataout_temp(OUT_WIDTH_g-1 downto OUT_WIDTH_g-2)="10") report "Overflow during rounding, dataout invalid.  This condition occurs when ROUND_UP_SYM is selected for rounding, and a large integer appears at the input.  Please consider changing rounding mode to CONVERGENT rounding, where overflows cannot happen, or apply SATURATION first." severity warning;
    
  end generate round_up_sym;

  -----------------------------------------------------------------------------
  -- convergent rounding
  -----------------------------------------------------------------------------
  conv_round_1 : if ROUNDING_TYPE_g = "CONV_ROUND" and LATENCY = 1 generate
  signal LSB : std_logic;               -- least significant retained bit
  signal RB  : std_logic;               -- rounding bit (MSB of discarded bit)

  begin  -- generate conv_round

    -----------------------------------------------------------------------------
    -- get relevant bits
    -----------------------------------------------------------------------------
    RB  <= datain(IN_WIDTH_g-OUT_WIDTH_g-1);
    LSB <= datain(IN_WIDTH_g-OUT_WIDTH_g);
    
    conv_round_p1 : process (clk) is
       variable OR_accu : std_logic := '0';
    begin  -- process conv_round_p
      if rising_edge(clk) then       -- rising clock edge
        if reset = '1' then               -- asynchronous reset (active high)
          dataout     <= (others => '0');
        else
          if enable = '1' then
            OR_accu := '0';
            for i in 0 to IN_WIDTH_g-OUT_WIDTH_g-2 loop
              OR_accu := OR_accu or datain(i);
            end loop;  -- i
            if RB = '1' and (LSB = '1' or OR_accu = '1') then
              dataout <= std_logic_vector(signed(datain(IN_WIDTH_g-1 downto IN_WIDTH_g-OUT_WIDTH_g))+1);
            else
              dataout   <= datain(IN_WIDTH_g-1 downto IN_WIDTH_g-OUT_WIDTH_g);          
            end if;
          end if;
       end if;
     end if;
    end process conv_round_p1;
  end generate conv_round_1;


  conv_round_2 : if ROUNDING_TYPE_g = "CONV_ROUND" and LATENCY = 2 generate
  signal LSB : std_logic;               -- least significant retained bit
  signal RB  : std_logic;               -- rounding bit (MSB of discarded bit)
  signal datareg : std_logic_vector(IN_WIDTH_g-1 downto 0);
  signal OR_accu : std_logic;

  begin  -- generate conv_round

    -----------------------------------------------------------------------------
    -- get relevant bits
    -----------------------------------------------------------------------------

    conv_round_p : process(clk) is
    variable OR_Temp : std_logic := '0';
    begin
      if rising_edge(clk) then       -- rising clock edge
        if reset = '1' then               -- asynchronous reset (active high)
           RB  <= '0';
           LSB <= '0';
           OR_accu <= '0';
           datareg <= (others =>'0');
        else
          if enable = '1' then
           OR_Temp := '0';
            for i in 0 to IN_WIDTH_g-OUT_WIDTH_g-2 loop
              OR_Temp := OR_Temp or datain(i);
            end loop;  --
           RB  <= datain(IN_WIDTH_g-OUT_WIDTH_g-1);
           LSB <= datain(IN_WIDTH_g-OUT_WIDTH_g);
           OR_accu <= OR_Temp;
           datareg <= datain;
          end if;
        end if;
      end if; 
    end process conv_round_p;

    conv_round_p2 : process (clk) is
    begin  -- process conv_round_p
      if rising_edge(clk) then       -- rising clock edge
        if reset = '1' then               -- asynchronous reset (active high)
          dataout     <= (others => '0');
        else
          if enable = '1' then
            if RB = '1' and (LSB = '1' or OR_accu = '1') then
              dataout <= std_logic_vector(signed(datareg(IN_WIDTH_g-1 downto IN_WIDTH_g-OUT_WIDTH_g))+1);
            else
              dataout   <= datareg(IN_WIDTH_g-1 downto IN_WIDTH_g-OUT_WIDTH_g);          
            end if;
          end if;
       end if;
     end if;
    end process conv_round_p2;
  end generate conv_round_2;


conv_round_3 : if ROUNDING_TYPE_g = "CONV_ROUND" and LATENCY = 3 generate
  signal LSB,LSB_R : std_logic;               -- least significant retained bit
  signal RB,RB_R  : std_logic;               -- rounding bit (MSB of discarded bit)
  signal datareg, datareg_2 : std_logic_vector(IN_WIDTH_g-1 downto 0);
  signal OR_accu_1, OR_accu_2 : std_logic;
  signal OR_accu : std_logic;

  begin  -- generate conv_round

    -----------------------------------------------------------------------------
    -- get relevant bits
    -----------------------------------------------------------------------------

    conv_round_p : process(clk) is
    variable OR_Temp_2,OR_Temp_1 : std_logic := '0';
    begin
      if rising_edge(clk) then       -- rising clock edge
        if reset = '1' then               -- asynchronous reset (active high)
           RB  <= '0';
           LSB <= '0';
           RB_R  <= '0';
           LSB_R <= '0';
           OR_accu_1 <= '0';
           OR_accu_2 <= '0';
           datareg <= (others =>'0');
        else
          if enable = '1' then
           OR_Temp_1 := '0';
           OR_Temp_2 := '0';
            for i in 0 to (IN_WIDTH_g-OUT_WIDTH_g-2)/2 loop
              OR_Temp_1 := OR_Temp_1 or datain(i);
            end loop;  --
            for i in ((IN_WIDTH_g-OUT_WIDTH_g-2)/2)+1 to (IN_WIDTH_g-OUT_WIDTH_g-2) loop
              OR_Temp_2 := OR_Temp_2 or datain(i);
            end loop;  --           RB  <= datain(IN_WIDTH_g-OUT_WIDTH_g-1);
           LSB <= datain(IN_WIDTH_g-OUT_WIDTH_g);
           LSB_R <= LSB;
           RB  <= datain(IN_WIDTH_g-OUT_WIDTH_g-1);
           RB_R <= RB;
           OR_accu_1 <= OR_Temp_1;
           OR_accu_2 <= OR_Temp_2;
           OR_accu <= OR_accu_1 or OR_accu_2;
           datareg <= datain;
           datareg_2 <= datareg;
          end if;
        end if;
      end if; 
    end process conv_round_p;

    conv_round_p2 : process (clk) is
    begin  -- process conv_round_p
      if rising_edge(clk) then       -- rising clock edge
        if reset = '1' then               -- asynchronous reset (active high)
          dataout     <= (others => '0');
        else
          if enable = '1' then
            if RB_R = '1' and (LSB_R = '1' or OR_accu = '1') then
              dataout <= std_logic_vector(signed(datareg_2(IN_WIDTH_g-1 downto IN_WIDTH_g-OUT_WIDTH_g))+1);
            else
              dataout   <= datareg_2(IN_WIDTH_g-1 downto IN_WIDTH_g-OUT_WIDTH_g);          
            end if;
          end if;
       end if;
     end if;
    end process conv_round_p2;
  end generate conv_round_3;
  -----------------------------------------------------------------------------
  -- error checking:
  --   Have we got a valid rounding mode?
  --   Is the input greater than the output?
  -----------------------------------------------------------------------------
  assert    (ROUNDING_TYPE_g = "SATURATE" or
             ROUNDING_TYPE_g = "SATURATE_SYM" or
             ROUNDING_TYPE_g = "ROUND_UP" or
             ROUNDING_TYPE_g = "ROUND0" or
             ROUNDING_TYPE_g = "ROUND_UP_SYM" or
             ROUNDING_TYPE_g = "CONV_ROUND" or
             ROUNDING_TYPE_g = "TRUNCATE_LOW" or
             ROUNDING_TYPE_g = "TRUNCATE_HIGH"
             ) report "Please check your rounding type and its spelling.  Currently, we only support SATURATE, SATURATE_SYM, ROUND_UP, ROUND0, CONV_ROUND, TRUNCATE_LOW, TRUNCATE_HIGH" severity error;
  assert    (((LATENCY = 1) or (ROUNDING_TYPE_g = "CONV_ROUND" and (LATENCY = 3 or LATENCY = 2)))) report "Please check your Latency. Currently we only support latency for all modes and 2 latency for convergent round" severity error; 


end architecture beh;
