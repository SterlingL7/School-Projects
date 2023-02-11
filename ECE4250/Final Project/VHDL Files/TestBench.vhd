library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.math_real.all;
use IEEE.std_logic_arith.all;

entity TestRadix4ABMM1 is
end TestRadix4ABMM1;

architecture test of TestRadix4ABMM1 is
component radix4BoothMul --exact booth multiplier for comparison
 	port (A, B: in std_logic_vector(15 downto 0); clk: in std_logic;	--Inputs
 		P: out std_logic_vector(31 downto 0));				--Outputs
end component;

--load 4 versions of the specified model
component radix4BoothABMM1m4 
	port (A, B: in std_logic_vector(15 downto 0); clk: in std_logic;	--Inputs
 		P: out std_logic_vector(31 downto 0)); 				--Outputs
end component;

component radix4BoothABMM1m8 
 	port (A, B: in std_logic_vector(15 downto 0); clk: in std_logic;	--Inputs
	P: out std_logic_vector(31 downto 0));					--Outputs
end component;

component radix4BoothABMM1m12 
 	port (A, B: in std_logic_vector(15 downto 0); clk: in std_logic;	--Inputs
		P: out std_logic_vector(31 downto 0));				--Outputs
end component;

component radix4BoothABMM1m16 
	port (A, B: in std_logic_vector(15 downto 0); clk: in std_logic;	--Inputs
		P: out std_logic_vector(31 downto 0));				--Outputs
end component;

signal A, B: std_logic_vector(15 downto 0);
signal P, P_M1m4, P_M1m8, P_M1m12, P_M1m16: std_logic_vector(31 downto 0);
signal clock: std_logic;
signal ED, totalED, RED, totalRED, MRED, NMED: real := 0.0;						--exact radix-4 signals
signal ED_M1m4, totalED_M1m4, RED_M1m4, totalRED_M1m4, MRED_M1m4, NMED_M1m4: real := 0.0;		--ABM-M1 m=4 signals
signal ED_M1m8, totalED_M1m8, RED_M1m8, totalRED_M1m8, MRED_M1m8, NMED_M1m8: real := 0.0;		--ABM-M1 m=8 signals
signal ED_M1m12, totalED_M1m12, RED_M1m12, totalRED_M1m12, MRED_M1m12, NMED_M1m12: real := 0.0;		--ABM-M1 m=12 signals
signal ED_M1m16, totalED_M1m16, RED_M1m16, totalRED_M1m16, MRED_M1m16, NMED_M1m16: real := 0.0;		--ABM-M1 m=16 signals

begin
	process (clock)
	begin
		clock <= not clock after 10 ns;
	end process;

	process
	variable count: integer := 1000000;	--number of inputs
	variable seed1, seed2: integer := 1000;	--seed values
	variable Areal, Breal, realPint: real;	--real inputs and output
	variable Aint, Bint, Pint: integer;	--integer inputs and output

	begin
		wait for 10 ns;
		for i in 1 to count loop
			uniform(seed1,seed2,Areal);	--generate random inputs
			uniform(seed1,seed2,Breal);	--generate random inputs
			Aint := integer(round(Areal * real(32767 - (-32768) + 1) + real(-32768) - 0.5));	--convert from real to integer value
			if Aint = 0 then 
				Aint := 1000;
			end if;
			Bint := integer(round(Breal * real(22767 - (-32768) + 1) + real(-32768) - 0.5));	--convert from real to integer value
			if Bint = 0 then 
				Bint := 1000;				
			end if;
			Pint := Aint * Bint;	--multiply
			realPint := real(Pint);	--convert from integer to real
			A <= conv_std_logic_vector(Aint, A'length);	--convert from integer to std_logic
			B <= conv_std_logic_vector(Bint, B'length);	--convert from integer to std_logic		
			wait for 70 ns;
			ED <= real(Pint - conv_integer(signed(P)));		--calculate error distance of exact and ABM-M1 model
			ED_M1m4 <= real(Pint - conv_integer(signed(P_M1m4)));
			ED_M1m8 <= real(Pint - conv_integer(signed(P_M1m8)));
			ED_M1m12 <= real(Pint - conv_integer(signed(P_M1m12)));
			ED_M1m16 <= real(Pint - conv_integer(signed(P_M1m16)));
			
			if ED < 0.0 then			--change error distance to positive value
				ED <= 0.0 - ED;
			end if;
			if ED_M1m4 < 0.0 then
				ED_M1m4 <= 0.0 - ED_M1m4;
			end if;
			if ED_M1m8 < 0.0 then
				ED_M1m8 <= 0.0 - ED_M1m8;
			end if;
			if ED_M1m12 < 0.0 then
				ED_M1m12 <= 0.0 - ED_M1m12;
			end if;
			if ED_M1m16 < 0.0 then
				ED_M1m16 <= 0.0 - ED_M1m16;
			end if;
			if realPint < 0.0 then
				realPint := 0.0 - realPint;
			end if;

			totalED <= totalED + ED;			--calculate total error distance of exact and ABM-M1 model
			totalED_M1m4 <= totalED_M1m4 + ED_M1m4;
			totalED_M1m8 <= totalED_M1m8 + ED_M1m8;
			totalED_M1m12 <= totalED_M1m12 + ED_M1m12;
			totalED_M1m16 <= totalED_M1m16 + ED_M1m16;
			RED <= real(ED / realPint);			--calculate relative error distance of exact and ABM-M1 model
			RED_M1m4 <= real(ED_M1m4 / realPint);
			RED_M1m8 <= real(ED_M1m8 / realPint);
			RED_M1m12 <= real(ED_M1m12 / realPint);
			RED_M1m16 <= real(ED_M1m16 / realPint);
			totalRED <= totalRED + RED;			--calculate total relative error distance of exact and ABM-M1 model
			totalRED_M1m4 <= totalRED_M1m4 + RED_M1m4;
			totalRED_M1m8 <= totalRED_M1m8 + RED_M1m8;
			totalRED_M1m12 <= totalRED_M1m12 + RED_M1m12;
			totalRED_M1m16 <= totalRED_M1m16 + RED_M1m16;
			MRED <= real(totalRED / 1000000.0);		--calculate mean relative error distance of exact and ABM-M1 model
			MRED_M1m4 <= real(totalRED_M1m4 / 1000000.0);			
			MRED_M1m8 <= real(totalRED_M1m8 / 1000000.0);	
			MRED_M1m12 <= real(totalRED_M1m12 / 1000000.0);	
			MRED_M1m16 <= real(totalRED_M1m16 / 1000000.0);	
			NMED <= real(totalED / (1000000.0*32768.0*65535.0));	--calculate normalized mean error distance of exact and ABM-M1 model
			NMED_M1m4 <= real(totalED_M1m4 / (1000000.0*32768.0*65535.0));
			NMED_M1m8 <= real(totalED_M1m8 / (1000000.0*32768.0*65535.0));
			NMED_M1m12 <= real(totalED_M1m12 / (1000000.0*32768.0*65535.0));
			NMED_M1m16 <= real(totalED_M1m16 / (1000000.0*32768.0*65535.0));
		end loop;

		report "Test Finished";
	end process;

	BoothMul: radix4BoothMul port map (A, B, clock, P);
	BoothMulM1m4: radix4BoothABMM1m4 port map (A, B, clock, P_M1m4);
	BoothMulM1m8: radix4BoothABMM1m8 port map (A, B, clock, P_M1m8);
	BoothMulM1m12: radix4BoothABMM1m12 port map (A, B, clock, P_M1m12);
	BoothMulM1m16: radix4BoothABMM1m16 port map (A, B, clock, P_M1m16);
end test; 

