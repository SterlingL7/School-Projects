---------------------------------------------------------------------------------
----  ECE 7250/4250 Lab
----  Digital Systems Design - VHDL and Programmable Logic Devices 
----  Instructor: Dr.Harry Tyrer
----  T.A.: Karthik Peddi
-------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity Adder4 is
  port (A, B: in std_logic_vector(3 downto 0); 
  	   Ci: in std_logic;  		-- Inputs
        S: out std_logic_vector(3 downto 0); 
	   Co: out std_logic);  		-- Outputs
end Adder4;

architecture Structure of Adder4 is
component FullAdder
  _______________________	-- Inputs	--Complete Component Declaration
  _______________________	-- Outputs
end component;

signal C: std_logic_vector(3 downto 1);

begin     --instantiate four copies of the FullAdder
  FA0: FullAdder port map (A(0), B(0), Ci, C(1), S(0));
  FA1: FullAdder port map (A(1), B(1), C(1), C(2), S(1));
  FA2: FullAdder port map (A(2), B(2), C(2), C(3), S(2));
  FA3: FullAdder port map (A(3), B(3), C(3), Co, S(3));
end Structure;									
