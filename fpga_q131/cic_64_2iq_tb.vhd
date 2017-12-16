-- ================================================================================
-- Legal Notice: Copyright (C) 1991-2008 Altera Corporation
-- Any megafunction design, and related net list (encrypted or decrypted),
-- support information, device programming or simulation file, and any other
-- associated documentation or information provided by Altera or a partner
-- under Altera's Megafunction Partnership Program may be used only to
-- program PLD devices (but not masked PLD devices) from Altera.  Any other
-- use of such megafunction design, net list, support information, device
-- programming or simulation file, or any other related documentation or
-- information is prohibited for any other purpose, including, but not
-- limited to modification, reverse engineering, de-compiling, or use with
-- any other silicon devices, unless such use is explicitly licensed under
-- a separate agreement with Altera or a megafunction partner.  Title to
-- the intellectual property, including patents, copyrights, trademarks,
-- trade secrets, or maskworks, embodied in any such megafunction design,
-- net list, support information, device programming or simulation file, or
-- any other related documentation or information provided by Altera or a
-- megafunction partner, remains with Altera, the megafunction partner, or
-- their respective licensors.  No other licenses, including any licenses
-- needed under any third party's intellectual property, are provided herein.
-- ================================================================================
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use std.textio.all;
entity cic_64_2iq_tb is
end cic_64_2iq_tb;


architecture tb of cic_64_2iq_tb is
  
  constant clk_time_step : time := 1 ns;

  signal clk     : std_logic;
  signal clk_cnt : integer 	:=0;
  signal reset_n : std_logic;
  signal clken : std_logic;
  signal in0_data : std_logic_vector (31 downto 0);
  signal in1_data : std_logic_vector (31 downto 0);
  signal in2_data : std_logic_vector (31 downto 0);
  signal in3_data : std_logic_vector (31 downto 0);
  signal out_data : std_logic_vector (31 downto 0);
  signal out_channel : std_logic_vector (1 downto 0);
  signal out_startofpacket : std_logic;
  signal out_endofpacket   : std_logic;
  signal in_error  : std_logic_vector (1 downto 0);
  signal out_error : std_logic_vector (1 downto 0);
  signal in_valid  : std_logic;
  signal out_ready : std_logic;
  signal in_ready  : std_logic;
  signal out_valid : std_logic;
  signal start     : std_logic;
  signal end_test  : std_logic;

--------------------------------------------------------------------------------------------                                      
-- CIC Component Declaration
--------------------------------------------------------------------------------------------                                      
  component cic_64_2iq is
    port (
      clk               : in  std_logic;
      clken             : in  std_logic;
      reset_n           : in  std_logic;
      in_ready          : out std_logic;
      in_valid          : in  std_logic;
      in0_data          : in  std_logic_vector (31 downto 0);
      in1_data          : in  std_logic_vector (31 downto 0);
      in2_data          : in  std_logic_vector (31 downto 0);
      in3_data          : in  std_logic_vector (31 downto 0);
      out_data          : out std_logic_vector (31 downto 0);
      out_channel       : out std_logic_vector (1 downto 0);
      out_startofpacket : out std_logic;
      out_endofpacket   : out std_logic;
      in_error          : in  std_logic_vector (1 downto 0);
	  out_error         : out std_logic_vector (1 downto 0);
      out_ready         : in  std_logic;
      out_valid         : out std_logic
      );
  end component cic_64_2iq;

begin

  -----------------------------------------------------------------------------------------------
  -- Reset Generation                                                                          
  -----------------------------------------------------------------------------------------------
  reset_n <= '0', '1' after 92*clk_time_step;
  -----------------------------------------------------------------------------------------------
  -- Clock Generation                                                                         
  -----------------------------------------------------------------------------------------------
  clkgen : process
  begin
    if end_test = '1' then
      clk <= '0';
      wait;
    else
      clk <= '0';
      wait for 5*clk_time_step;
      clk <= '1';
      wait for 5*clk_time_step;
    end if;
  end process clkgen;

  -----------------------------------------------------------------------------------------------
  -- Clock Counter
  -----------------------------------------------------------------------------------------------
  clk_cnt_proc : process(clk, reset_n)
  begin
    if reset_n = '0' then
      clk_cnt <= 0;
    elsif rising_edge(clk) then
	  clk_cnt <= clk_cnt + 1;
	end if ;
  end process clk_cnt_proc;

  -- clock enable
  -- always enabled
  clken <= '1';

  -- for example purposes, the ready signal is always asserted.
  out_ready <= '1';

  -- no input error
  in_error <= (others => '0');

  -- start valid for first cycle to indicate that the file reading should start.
  start_p : process (clk, reset_n)
  begin
    if reset_n = '0' then
      start <= '1';
    elsif rising_edge(clk) then
      if in_valid = '1' and in_ready = '1' then
        start <= '0';
      end if;
    end if;
  end process start_p;


  -----------------------------------------------------------------------------------------------
  -- Read input data from files                                                                  
  -----------------------------------------------------------------------------------------------
  testbench_i : process(clk) is
    file r_file     : text open read_mode is "cic_64_2iq_tb_input.txt";
    variable data_r : integer;
    variable rdata  : line;
  begin
      if(reset_n = '0') then
        in0_data  <= std_logic_vector(to_signed(0, 32));
        in1_data  <= std_logic_vector(to_signed(0, 32));
        in2_data  <= std_logic_vector(to_signed(0, 32));
        in3_data  <= std_logic_vector(to_signed(0, 32));
        in_valid <= '0';
        end_test <= '0';
      elsif rising_edge(clk) then
        if not endfile(r_file) then
          if((in_valid = '1' and in_ready = '1') or
             (start = '1'and not (in_valid = '1' and in_ready = '0'))) then
            readline(r_file, rdata);
            read(rdata, data_r);
            in_valid <= '1';
              in0_data  <= std_logic_vector(to_signed(data_r, 32));
              in1_data  <= std_logic_vector(to_signed(2, 32));
              in2_data  <= std_logic_vector(to_signed(4, 32));
              in3_data  <= std_logic_vector(to_signed(8, 32));
          else
            in_valid <= '1';
            in0_data  <= in0_data;
            in1_data  <= in1_data;
            in2_data  <= in2_data;
            in3_data  <= in3_data;
          end if;
        else
          if (end_test = '0') then
            if (in_valid = '1' and in_ready = '1') then
              end_test <= '1';
              in_valid <= '0';
              in0_data  <= std_logic_vector(to_signed(0, 32));
              in1_data  <= std_logic_vector(to_signed(0, 32));
              in2_data  <= std_logic_vector(to_signed(0, 32));
              in3_data  <= std_logic_vector(to_signed(0, 32));
            else
              in_valid <= '1';
              in0_data  <= in0_data;
              in1_data  <= in1_data;
              in2_data  <= in2_data;
              in3_data  <= in3_data;
            end if;
          end if;
        end if;
      end if;
  end process testbench_i;

  ---------------------------------------------------------------------------------------------
  -- Write data output to Files                                               
  ---------------------------------------------------------------------------------------------

  testbench_o : process(clk, reset_n) is
    file ro_file   : text open write_mode is "cic_64_2iq_tb_output.txt";
    variable rdata : line;
    variable data_r : integer;
  begin
    if rising_edge(clk) and reset_n = '1' then
      if(out_valid = '1' and out_ready = '1') then
        data_r := to_integer(signed(out_data));
        write(rdata, data_r);
        writeline(ro_file, rdata);
      end if;
    end if;
  end process testbench_o;

  ---------------------------------------------------------------------------------------------
  -- CIC Component Instantiation                                                               
  ---------------------------------------------------------------------------------------------
  cic_64_2iq_inst : cic_64_2iq
    port map (
      clk               => clk,
      clken             => clken,
      reset_n           => reset_n,
      in_ready          => in_ready,
      in_valid          => in_valid,
      in0_data          => in0_data,
      in1_data          => in1_data,
      in2_data          => in2_data,
      in3_data          => in3_data,
      out_data          => out_data,
      out_channel       => out_channel,
      out_startofpacket => out_startofpacket,
      out_endofpacket   => out_endofpacket,
      in_error          => in_error,
      out_error         => out_error,
      out_ready         => out_ready,
      out_valid         => out_valid
      );

end tb;
