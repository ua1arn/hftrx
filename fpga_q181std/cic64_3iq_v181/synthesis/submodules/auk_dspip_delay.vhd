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
-- $RCSfile: auk_dspip_delay.vhd,v $
-- $Source: /cvs/uksw/dsp_cores/lib/fu/delay/rtl/auk_dspip_delay.vhd,v $
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
-- This functional unit can be used to insert a delay of specified length.
-- The output data at time T will be equivalent to the input data at time
-- T-DELAY. The user can select to implement the delay using either LE's
-- (registers) or Memory. The type of memory to use is selected by the
-- user. 
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
library altera_mf;
use altera_mf.altera_mf_components.all;
library work;
use work.auk_dspip_math_pkg.all;


entity auk_dspip_delay is
  generic (
    WIDTH_g          :     natural := 8;  -- data width
    DELAY_g          :     natural := 8;
                                        -- number of clock cycles the input
                                        -- will be delayed by
    MEMORY_TYPE_g    :     string  := "AUTO";
                                        -- possible values are  "m4k", "m512",
                                        -- "register", "mram", "auto", 
                                        -- "lutram", "M9K", "M144K".
                                        -- Any other string will be interpreted
                                        -- as "auto"
    REGISTER_FIRST_g :     natural := 1;
                                        -- if "1", the first delay is guaranteed
                                        -- to be in registers
    REGISTER_LAST_g  :     natural := 1);  -- if "1", the last delay is guaranteed
                                        -- to be in registers
  port (
    clk              : in  std_logic;
    reset            : in  std_logic;
    enable           : in  std_logic;   -- global clock enable
    datain           : in  std_logic_vector(WIDTH_g-1 downto 0);
    dataout          : out std_logic_vector(WIDTH_g-1 downto 0)
    );
end entity auk_dspip_delay;


architecture rtl of auk_dspip_delay is
begin  -- architecture rtl


  ------------------------------------------------------------------------------
  -- array of registers
  ------------------------------------------------------------------------------
  register_fifo      : if MEMORY_TYPE_g = "register" or MEMORY_TYPE_g = "REGISTER" or DELAY_g-REGISTER_FIRST_g-REGISTER_LAST_g < 3 generate
    type tFIFO_DATA is array (0 to DELAY_g-1) of std_logic_vector(WIDTH_g-1 downto 0);
    signal fifo_data : tFIFO_DATA;
  begin  -- generate register_fifo

    -- purpose: array of registers
    -- type   : sequential
    -- inputs : clk, reset, data_in
    -- outputs: data_out
    array_of_reg  : process (clk, reset) is
    begin  -- process array_of_reg
      if reset = '1' then               -- asynchronous reset (active high)
        reset_all : for i in 0 to DELAY_g-1 loop
          fifo_data(i) <= (others => '0');
        end loop;
      elsif rising_edge(clk) then       -- rising clock edge

        -------------------------------------------------------------------
        -- memory array
        -------------------------------------------------------------------

        if enable = '1' then
          fifo_data(0)   <= datain;
    if DELAY_g > 1 then
            shift_up : for i in 1 to DELAY_g-1 loop
              fifo_data(i) <= fifo_data(i-1);
            end loop;
    end if;
        end if;

      end if;
    end process array_of_reg;
    dataout <= fifo_data(DELAY_g-1);


  end generate register_fifo;

  -----------------------------------------------------------------------------
  -- memory based delay using altshift_tap
  -----------------------------------------------------------------------------
  memory_fifo                   :     if DELAY_g-REGISTER_FIRST_g-REGISTER_LAST_g >= 3 and not(MEMORY_TYPE_g = "register" or MEMORY_TYPE_g = "REGISTER") generate
component altshift_taps
      generic (
        lpm_hint                :     string;
        lpm_type                :     string;
        number_of_taps          :     natural;
        tap_distance            :     natural;
        width                   :     natural
        );
      port (
        taps                    : out std_logic_vector (WIDTH_g-1 downto 0);
        clken                   : in  std_logic;
        clock                   : in  std_logic;
        aclr                    : in  std_logic := '0';        
        sclr                    : in  std_logic := '0';     
        shiftout                : out std_logic_vector (WIDTH_g-1 downto 0);
        shiftin                 : in  std_logic_vector (WIDTH_g-1 downto 0)
        );
    end component;
    component scfifo
      generic (
        add_ram_output_register :     string;
        intended_device_family  :     string;
        lpm_numwords            :     natural;
        lpm_showahead           :     string;
        lpm_type                :     string;
        lpm_width               :     natural;
        lpm_widthu              :     natural;
        overflow_checking       :     string;
        underflow_checking      :     string;
        use_eab                 :     string
        );
      port (
        rdreq                   : in  std_logic;
        clock                   : in  std_logic;
        q                       : out std_logic_vector (WIDTH_g-1 downto 0);
        wrreq                   : in  std_logic;
        data                    : in  std_logic_vector (WIDTH_g-1 downto 0)
        );
    end component;

    constant mem_depth : natural := DELAY_g - REGISTER_FIRST_g - REGISTER_LAST_g;
    constant lpm_hint  : string  := "RAM_BLOCK_TYPE = " & MEMORY_TYPE_g;


    signal mem_in            : std_logic_vector(WIDTH_g-1 downto 0);  -- memory input
    signal mem_out           : std_logic_vector(WIDTH_g-1 downto 0);  -- memory_output
    signal count_after_reset : unsigned(log2_ceil(mem_depth) downto 0);
    signal extended_reset    : std_logic;

  begin  -- generate memory_fifo



    input_stage : if REGISTER_FIRST_g = 1 and (REGISTER_LAST_g = 0 or DELAY_g > 1) generate
    begin  -- generate input_stage
      -- purpose: registers first stage
      -- type   : sequential
      -- inputs : clk, reset, data_in
      -- outputs: mem_in

      reg_input : process (clk, reset) is
      begin  -- process reg_input
        if reset = '1' then             -- asynchronous reset (active high)
          mem_in   <= (others => '0');
        elsif rising_edge(clk) then     -- rising clock edge
          if enable = '1' then
            mem_in <= datain;
          end if;
        end if;
      end process reg_input;
    end generate input_stage;

    bypass_input_stage : if ((REGISTER_FIRST_g = 0) or
                           ((REGISTER_FIRST_g = 1) and (REGISTER_LAST_g = 1) and (DELAY_g = 1))) generate
    -- captures the case, where the total delay is 1, and input and output
    -- register is requested.  In this case, the output register is also the
    -- input register.  Bypassing input register, leaving only output register.

    begin  -- generate bypass_input_stage
      mem_in <= datain;
    end generate bypass_input_stage;

    check_illegal_generic                                 : if REGISTER_FIRST_g > 1 generate
    begin  -- generate check_illegal_generic
      assert false report "Generic REGISTER_FIRST_g in FU : DELAY must be 0 or 1" severity failure;
    end generate check_illegal_generic;

    check_illegal_generic_3                                 : if DELAY_g < 1 generate
    begin  -- generate check_illegal_generic
      assert false report "Generic DELAY_g in FU : DELAY must be at least 1" severity failure;
    end generate check_illegal_generic_3;

    ---------------------------------------------------------------------------
    -- only instantiate memory if the length of the shift reg is greater than
    -- 0. This is to capture the case where the length is 1, and input or
    -- output registers have been used. 
    ---------------------------------------------------------------------------
    memory_i : if mem_depth > 0 generate
    begin  -- generate memory_i
      altshift_taps_component : altshift_taps
        generic map (
          lpm_hint       => lpm_hint,
          lpm_type       => "altshift_taps",
          number_of_taps => 1,
          tap_distance   => mem_depth,
          width          => WIDTH_g
          )
        port map (
          clken          => enable,
          clock          => clk,
          shiftin        => mem_in,
          taps           => mem_out,
          shiftout       => open
          );
    end generate memory_i;
    
    no_memory_i : if mem_depth < 1 generate
    begin  -- generate no_memory_i
      mem_out <= mem_in;                -- bypass memory
    end generate no_memory_i;

    

    output_stage : if REGISTER_LAST_g = 1 generate
    begin  -- generate output_stage
      -- purpose: registers first stage
      -- type   : sequential
      -- inputs : clk, reset, data_in
      -- outputs: mem_in
      reg_output : process (clk, reset) is
      begin  -- process reg_output
        if reset = '1' then             -- asynchronous reset (active high)
          dataout     <= (others => '0');
        elsif rising_edge(clk) then     -- rising clock edge
          if enable = '1' then
            if extended_reset = '1' then
              dataout <= (others => '0');
            else
              dataout <= mem_out;
            end if;
          end if;
        end if;
      end process reg_output;
    end generate output_stage;

    bypass_output_stage : if REGISTER_LAST_g = 0 generate
    begin  -- generate bypass_output_stage
      dataout <= mem_out when extended_reset = '0' else
                 (others => '0');
    end generate bypass_output_stage;

    check_illegal_generic2                               : if REGISTER_LAST_g > 1 generate
    begin  -- generate check_illegal_generic2
      assert false report "Generic REGISTER_LAST_g in FU : DELAY must be 0 or 1" severity failure;
    end generate check_illegal_generic2;

    -- purpose: creates an extended reset, that is high for MEM_DEPTH_c clock cycles.
    -- This signal is used to 0 out the first MEM_DEPTH values from the delay register.
    -- type   : sequential
    -- inputs : clk, resenable
    -- outputs: extended_reset
    extend_reset : process (clk, reset) is
    begin  -- process extended_reset
      if reset = '1' then               -- asynchronous reset (active high)
        extended_reset        <= '1';
        count_after_reset     <= (others => '0');
      elsif rising_edge(clk) then       -- rising clock edge
        if enable = '1' then
          if count_after_reset < mem_depth then
            count_after_reset <= count_after_reset + 1;
            extended_reset    <= '1';
          else
            extended_reset    <= '0';
          end if;
        end if;
      end if;
    end process extend_reset;

  end generate memory_fifo;


end architecture rtl;
