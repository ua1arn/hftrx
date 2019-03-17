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


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity auk_dspip_fastadd is
  generic (
    INWIDTH_g  :     natural := 18;
    LABWIDTH_g :     natural := 16);
    -- width of lab in selected device ( 10 or 16 in Cyclone,
    -- Cylone II, Stratix and Stratix II.  Don't know
    -- Stratix III yet.
  port (
    datain1    : in  std_logic_vector(INWIDTH_g-1 downto 0);
    datain2    : in  std_logic_vector(INWIDTH_g-1 downto 0);
    clk        : in  std_logic;
    enable     : in  std_logic;
    reset      : in  std_logic;
    dataout    : out std_logic_vector(INWIDTH_g downto 0));
end entity auk_dspip_fastadd;


architecture beh of auk_dspip_fastadd is

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

  attribute keep : boolean;
  attribute keep of result_high_cin0 : signal is true;
  attribute keep of result_high_cin1 : signal is true;

begin  -- architecture beh

  datain1_low  <= datain1(LOWWIDTH_c-1 downto 0);
  datain1_high <= datain1(HIGHWIDTH_c+LOWWIDTH_c-1 downto LOWWIDTH_c);
  datain2_low  <= datain2(LOWWIDTH_c-1 downto 0);
  datain2_high <= datain2(HIGHWIDTH_c+LOWWIDTH_c-1 downto LOWWIDTH_c);

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

  result_low       <= std_logic_vector(unsigned('0' & datain1_low) + unsigned('0' & datain2_low));
  result_high_cin0 <= std_logic_vector(unsigned(datain1_high(HIGHWIDTH_c-1) & datain1_high) +
                                       unsigned(datain2_high(HIGHWIDTH_c-1) & datain2_high));
  result_high_cin1 <= std_logic_vector(unsigned(datain1_high(HIGHWIDTH_c-1) & datain1_high )+
                                       unsigned(datain2_high(HIGHWIDTH_c-1) & datain2_high )+
                                       1);
  

end architecture beh;
