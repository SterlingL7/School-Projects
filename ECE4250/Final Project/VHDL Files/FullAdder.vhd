

library IEEE;
use IEEE.std_logic_1164.all;

entity FullAdder is
	port (X,Y,Ci: in std_logic;
		Co, Sum: out std_logic);
end FullAdder;

architecture adder of FullAdder is
	begin
		Sum <= x xor y xor Ci;
		Co <= (x and y) or (x and Ci) or (y and Ci);
end adder;
