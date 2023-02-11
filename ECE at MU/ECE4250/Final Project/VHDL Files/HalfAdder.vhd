
library IEEE;
use IEEE.std_logic_1164.all;

entity HalfAdder is
	port (X,Y: in std_logic;
		Co,Sum: out std_logic);
end HalfAdder;

architecture adder of HalfAdder is
	begin
		Sum <= x xor y;
		Co <= (x and y);
end adder;