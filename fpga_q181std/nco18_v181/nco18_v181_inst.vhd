	component nco18_v181 is
		port (
			clk       : in  std_logic                     := 'X';             -- clk
			clken     : in  std_logic                     := 'X';             -- clken
			phi_inc_i : in  std_logic_vector(31 downto 0) := (others => 'X'); -- phi_inc_i
			fsin_o    : out std_logic_vector(16 downto 0);                    -- fsin_o
			fcos_o    : out std_logic_vector(16 downto 0);                    -- fcos_o
			out_valid : out std_logic;                                        -- out_valid
			reset_n   : in  std_logic                     := 'X'              -- reset_n
		);
	end component nco18_v181;

	u0 : component nco18_v181
		port map (
			clk       => CONNECTED_TO_clk,       -- clk.clk
			clken     => CONNECTED_TO_clken,     --  in.clken
			phi_inc_i => CONNECTED_TO_phi_inc_i, --    .phi_inc_i
			fsin_o    => CONNECTED_TO_fsin_o,    -- out.fsin_o
			fcos_o    => CONNECTED_TO_fcos_o,    --    .fcos_o
			out_valid => CONNECTED_TO_out_valid, --    .out_valid
			reset_n   => CONNECTED_TO_reset_n    -- rst.reset_n
		);

