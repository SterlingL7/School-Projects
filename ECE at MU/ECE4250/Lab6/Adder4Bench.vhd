---------------------------------------------------------------------------------
----  ECE 7250/4250 Lab
----  Digital Systems Design - VHDL and Programmable Logic Devices 
----  Instructor: Dr.Harry Tyrer
----  T.A.: Karthik Peddi
---------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity Adder4Bench is
  port (A, B	: 	in std_logic_vector(3 downto 0); 
  	   Ci	: 	in std_logic;  			-- Inputs
	   clk	:	in std_logic;
	   AN0, AN1, AN2, AN3	:	out std_logic;
        segment_a, segment_b, segment_c, segment_d, segment_e, segment_f, segment_g, segment_dp :	out std_logic);
end Adder4Bench;

architecture _______________________     -- Complete syntax here    			            and don’t forget to end architecture
component AnodeControl 
port (clk	: in _________;  		       --Complete syntax
  	   counter_out	:	out std_logic_vector (2 downto 0);
	   Anode	:	out std_logic_vector (3 downto 0));
end component;

component Adder4
  port (A, B: in std_logic_vector(3 downto 0); 
  	   Ci: in std_logic;  			-- Inputs
        S: out std_logic_vector(3 downto 0); 
	   Co: out std_logic);  			-- Outputs
end component;

component LEDDisplay is
  port (Sum	: 	in std_logic_vector(3 downto 0); 
  	   Cout	: 	in std_logic;  -- Inputs
    	   counter:	in std_logic_vector (2 downto 0);
--	   Anode	:	in std_logic_vector(3 downto 0);
        segment_a, segment_b, segment_c, segment_d, segment_e, segment_f, segment_g:	out std_logic);
end component;


signal Co: std_logic;
signal S:  std_logic_vector (3 downto 0);
signal counter:	________________________	   -- Complete syntax
signal Anode:	________________________      -- Complete syntax

begin

AN0 <= Anode(0);
AN1 <= Anode(1);
AN2 <= Anode(2);
AN3 <= Anode(3);

segment_dp <= '1';

adder:	Adder4 port map (A, B, Ci, S, Co);
LEDDisplay0: 	LEDDisplay  port map (S, Co, counter, segment_a, segment_b, segment_c, segment_d, segment_e, segment_f, segment_g);
ANDisplay:	AnodeControl port map (clk, counter, Anode);

____________________;                         -- Complete syntax
