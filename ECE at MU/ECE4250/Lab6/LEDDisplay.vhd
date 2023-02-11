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

entity LEDDisplay is
  	_____________________________ 	--complete entity description
	_____________________________		
	_____________________________
	_____________________________	   
end LEDDisplay;

architecture Behav of LEDDisplay is

component dec_7seg 
	________________________________	 --complete component description
	________________________________
end component;

signal segment_a1, segment_a2, segment_b1, segment_b2, segment_c1, segment_c2, 
       segment_d1, segment_d2, segment_e1, segment_e2, segment_f1, segment_f2, segment_g1, segment_g2: std_logic;
signal Co :	std_logic_vector (3 downto 0);

begin

Co <=  "000" & Cout;

process (counter, segment_a1,segment_b1,segment_c1,segment_d1,segment_e1,segment_f1,segment_g1,segment_a2,segment_b2,segment_c2,segment_d2,segment_e2,segment_f2,segment_g2)
begin
	case counter is
--	when "1110" =>
	when "000" =>
		segment_a <= segment_a1;
		segment_b <= segment_b1;
		segment_c <= segment_c1;
		segment_d <= segment_d1;
		segment_e <= segment_e1;
		segment_f <= segment_f1;
		segment_g <= segment_g1;
--	when "1101" =>
	when "010" =>		--complete portion of code
		_____________________
		_____________________
		_____________________
		_____________________
		_____________________
		_____________________
		_____________________
--	when "1011" =>
	when "100" =>
		segment_a <= '1';
		segment_b <= '1';
		segment_c <= '1';
		segment_d <= '1';
		segment_e <= '1';
		segment_f <= '1';
		segment_g <= '1';
--	when "0111" =>
	when "110" =>
		segment_a <= '1';
		segment_b <= '1';
		segment_c <= '1';
		segment_d <= '1';
		segment_e <= '1';
		segment_f <= '1';
		segment_g <= '1';
	when others =>
		segment_a <= '1';
		segment_b <= '1';
		segment_c <= '1';
		segment_d <= '1';
		segment_e <= '1';
		segment_f <= '1';
		segment_g <= '1';
	end case;
end process;

LED_Display1: 	dec_7seg  port map (Sum, segment_a1, segment_b1, segment_c1, segment_d1, segment_e1, segment_f1, segment_g1);
LED_Display2: 	dec_7seg  port map (Co, segment_a2, segment_b2, segment_c2, segment_d2, segment_e2, segment_f2, segment_g2);

end Behav;
