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

entity FullAdder is
	port (X, Y, Cin:	in std_logic;
	Cout, Sum:	out std_logic );
end FullAdder;

architecture Equations of FullAdder is
begin
	Sum  <= ___________________		--complete the expressions for Sum and Cout
	Cout <= ________________________  
end Equations;
