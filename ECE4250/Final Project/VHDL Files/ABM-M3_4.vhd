
------
-- 4-bit version of ABM-M3
-- matrix width reduction by ORing the LSBs of each partial product
--the ORed value is ANDed with NOT zero to get the final
--partial product bit
--this is done along the bit plane of the partial product matrix,
--not the individual partial products
------

library IEEE; 
USE IEEE.numeric_bit.ALL; 
use IEEE.std_logic_1164.all;
use IEEE.std_logic_misc.all; --for or_reduce function

entity radix4BoothABMM3m4 is 
 	port (A, B: in std_logic_vector(15 downto 0); clk: in std_logic;
 		P: out std_logic_vector(31 downto 0)); 
end radix4BoothABMM3m4;
 
Architecture Behavioral of radix4BoothABMM3m4 is 
constant m: integer := 4; --number of bits to approximate
--carry signals
signal C1: std_logic_vector(17 downto m-3);
signal C2, C3, C4, C5, C6, C7: std_logic_vector(17 downto 0); 
--partial sum signals
signal S1: std_logic_vector(17 downto m-3);
signal S2, S3, S4, S5, S6, S7: std_logic_vector(17 downto 0);
--each partial product vector is width reduced 
--ex. (15 downto 3) for P0 in this 4-bit approximation
signal P0: std_logic_vector(15 downto m-1);
signal P1: std_logic_vector(15 downto m-3); 
signal P2,P3,P4,P5,P6,P7: std_logic_vector(15 downto 0);
--booth encoding signals
signal neg, two, zero: std_logic_vector(7 downto 0);
--m0-m7 are intermediate values for the exact booth partial
--product generator
signal m0: std_logic_vector(15 downto m);
signal m1: std_logic_vector(15 downto m-2);
signal m2, m3, m4, m5, m6, m7: std_logic_vector(15 downto 0);
-- E is sign extensions, PS is adding 1 for 2's complement
signal E, PS: std_logic_vector(7 downto 0);
signal NE: std_logic;
constant N: integer := 15;
constant K: integer := 7;

--since our code uses std_logic_vectors I defined an OR-reduction 
--that performs a bitwise or on an input std_logic_vector
-- function or_reduce( in_vec: std_logic_vector)
	-- return bit is
	-- variable orreduce: bit:='0';
	-- begin
	-- for i in 0 to in_vec'length-1 loop
		-- orreduce := orreduce or in_vec(i);
	-- end loop;
	-- return orreduce;
-- end function or_reduce;


component FullAdder
	 port(X,Y,Ci: in std_logic; --Inputs
		Co,Sum: out std_logic); --Outputs
end component;

component HalfAdder
	 port(X,Y: in std_logic; --Inputs
		Co,Sum: out std_logic); --Outputs
end component;

begin 
 	process (clk)
	begin
		if clk'event and clk = '1' then
		case B(1) is --first encoding appends a 0 to the LSB
		when '0' => 
			case B(0) is 
			when '0' => neg(0) <= '0'; two(0) <= '0'; zero(0) <= '1';
			when '1' => neg(0) <= '0'; two(0) <= '0'; zero(0) <= '0';
			when others => null;
			end case;
		when '1' => 
			case B(0) is 
			when '0' => neg(0) <= '1'; two(0) <= '1'; zero(0) <= '0';
			when '1' => neg(0) <= '1'; two(0) <= '0'; zero(0) <= '0';
			when others => null;
			end case; when others => null;
		end case;

		--OR lowest m bits of A, AND with not zero of partial product 0 to produce value for LSB of P0
		P0(m-1) <= or_reduce(A(m-1 downto 0)) and not zero(0);
		--remaining bits produced via standard partial product generator
		for i in m to N loop
			m0(i) <= (A(i) and not two(0)) or (A(i-1) and two(0));
			P0(i) <= (not zero(0)) and (neg(0) xor m0(i));
		end loop;

		for i in 1 to K loop --loop to fill the 7 remaining encodings
			case B(2*i+1) is
			when '0' =>
				case B(2*i) is
				when '0' =>
					case B(2*i-1) is
					when '0' => neg(i) <= '0'; two(i) <= '0'; zero(i) <= '1';
					when '1' => neg(i) <= '0'; two(i) <= '0'; zero(i) <= '0'; when others => null;
					end case;
				when '1' =>
					case B(2*i-1) is
					when '0' => neg(i) <= '0'; two(i) <= '0'; zero(i) <= '0';
					when '1' => neg(i) <= '0'; two(i) <= '1'; zero(i) <= '0'; when others => null;
					end case; when others => null;		
				end case;
			when '1' =>
				case B(2*i) is
				when '0' =>
					case B(2*i-1) is
					when '0' => neg(i) <= '1'; two(i) <= '1'; zero(i) <= '0';
					when '1' => neg(i) <= '1'; two(i) <= '0'; zero(i) <= '0'; when others => null;
					end case;
				when '1' =>
					case B(2*i-1) is
					when '0' => neg(i) <= '1'; two(i) <= '0'; zero(i) <= '0';
					when '1' => neg(i) <= '0'; two(i) <= '0'; zero(i) <= '1'; when others => null;
					end case; when others => null;	
				end case; when others => null;
			end case;
		
			case i is --each encoding is used to generate the partial products
			when 1 => 
				--approximation for the lowest bits if their absolute placement 
				--is lower than the threshold
				P1(m-3) <= or_reduce(A(m-3 downto 0)) and not zero(1);
				--exact partial products for the remainder
				for j in m-2 to N loop
					m1(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P1(j) <= (not zero(i)) and (neg(i) xor m1(j));
				end loop;	
			when 2 => 
				--the 0th element is a separate case since A(-1) does not exist
				m2(0) <= (A(0) and not two(i));
				P2(0) <= (not zero(i)) and (neg(i) xor m2(0));
				
				for j in 1 to N loop
					m2(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P2(j) <= (not zero(i)) and (neg(i) xor m2(j));
				end loop;
			when 3 => 
				
				m3(0) <= (A(0) and not two(i));
				P3(0) <= (not zero(i)) and (neg(i) xor m3(0));
				
				for j in 1 to N loop
					m3(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P3(j) <= (not zero(i)) and (neg(i) xor m3(j));
				end loop;
			when 4 => 
				m4(0) <= (A(0) and not two(i));
				P4(0) <= (not zero(i)) and (neg(i) xor m4(0));
				for j in 1 to N loop
					m4(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P4(j) <= (not zero(i)) and (neg(i) xor m4(j));
				end loop;
			when 5 => 
				m5(0) <= (A(0) and not two(i));
				P5(0) <= (not zero(i)) and (neg(i) xor m5(0));
				for j in 1 to N loop
					m5(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P5(j) <= (not zero(i)) and (neg(i) xor m5(j));
				end loop;
			when 6 => 
				m6(0) <= (A(0) and not two(i));
				P6(0) <= (not zero(i)) and (neg(i) xor m6(0));
				for j in 1 to N loop
					m6(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P6(j) <= (not zero(i)) and (neg(i) xor m6(j));
				end loop;
			when 7 =>  
				m7(0) <= (A(0) and not two(i));
				P7(0) <= (not zero(i)) and (neg(i) xor m7(0));
				for j in 1 to N loop
					m7(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P7(j) <= (not zero(i)) and (neg(i) xor m7(j));
				end loop;
			when others => null;
			end case;
		end loop;
		end if;
	end process;
	--calculate sign extension bits 
	E(0) <= zero(0) or (neg(0) xnor A(15));  NE <= not E(0);
	E(1) <= zero(1) or (neg(1) xnor A(15)); 
	E(2) <= zero(2) or (neg(2) xnor A(15)); PS(2) <= P2(0) or neg(2);
	E(3) <= zero(3) or (neg(3) xnor A(15)); PS(3) <= P3(0) or neg(3);
	E(4) <= zero(4) or (neg(4) xnor A(15)); PS(4) <= P4(0) or neg(4);
	E(5) <= zero(5) or (neg(5) xnor A(15)); PS(5) <= P5(0) or neg(5);
	E(6) <= zero(6) or (neg(6) xnor A(15)); PS(6) <= P6(0) or neg(6);
	E(7) <= zero(7) or (neg(7) xnor A(15)); PS(7) <= P7(0) or neg(7);

	--half adder for the approximate bits since they are first ones in each row
	HA11: HalfAdder port map(P0(m-1), P1(m-3), C1(m-3), S1(m-3));

	--first full adder row is width reduced
	FA12: FullAdder port map (P0(4), P1(2), C1(1), C1(2), S1(2));
	FA13: FullAdder port map (P0(5), P1(3), C1(2), C1(3), S1(3));
	FA14: FullAdder port map (P0(6), P1(4), C1(3), C1(4), S1(4));
	FA15: FullAdder port map (P0(7), P1(5), C1(4), C1(5), S1(5));
	FA16: FullAdder port map (P0(8), P1(6), C1(5), C1(6), S1(6));
	FA17: FullAdder port map (P0(9), P1(7), C1(6), C1(7), S1(7));
	FA18: FullAdder port map (P0(10), P1(8), C1(7), C1(8), S1(8));
	FA19: FullAdder port map (P0(11), P1(9), C1(8), C1(9), S1(9));
	FA110: FullAdder port map (P0(12), P1(10), C1(9), C1(10), S1(10));
	FA111: FullAdder port map (P0(13), P1(11), C1(10), C1(11), S1(11));
	FA112: FullAdder port map (P0(14), P1(12), C1(11), C1(12), S1(12));
	FA113: FullAdder port map (P0(15), P1(13), C1(12), C1(13), S1(13));
	FA114: FullAdder port map (NE, P1(14), C1(13), C1(14), S1(14));
	FA115: FullAdder port map (NE, P1(15), C1(14), C1(15), S1(15));
	FA116: FullAdder port map (E(0), E(1), C1(15), C1(16), S1(16));

	FA21: FullAdder port map (S1(3), P2(1), C2(0), C2(1), S2(1));
	FA22: FullAdder port map (S1(4), P2(2), C2(1), C2(2), S2(2));
	FA23: FullAdder port map (S1(5), P2(3), C2(2), C2(3), S2(3));
	FA24: FullAdder port map (S1(6), P2(4), C2(3), C2(4), S2(4));
	FA25: FullAdder port map (S1(7), P2(5), C2(4), C2(5), S2(5));
	FA26: FullAdder port map (S1(8), P2(6), C2(5), C2(6), S2(6));
	FA27: FullAdder port map (S1(9), P2(7), C2(6), C2(7), S2(7));
	FA28: FullAdder port map (S1(10), P2(8), C2(7), C2(8), S2(8));
	FA29: FullAdder port map (S1(11), P2(9), C2(8), C2(9), S2(9));
	FA210: FullAdder port map (S1(12), P2(10), C2(9), C2(10), S2(10));
	FA211: FullAdder port map (S1(13), P2(11), C2(10), C2(11), S2(11));
	FA212: FullAdder port map (S1(14), P2(12), C2(11), C2(12), S2(12));
	FA213: FullAdder port map (S1(15), P2(13), C2(12), C2(13), S2(13));
	FA214: FullAdder port map (S1(16), P2(14), C2(13), C2(14), S2(14));
	FA215: FullAdder port map (S1(17), P2(15), C2(14), C2(15), S2(15));
	FA216: FullAdder port map (C1(17), E(2), C2(15), C2(16), S2(16));
	
	FA31: FullAdder port map (S2(3), P3(1), C3(0), C3(1), S3(1));
	FA32: FullAdder port map (S2(4), P3(2), C3(1), C3(2), S3(2));
	FA33: FullAdder port map (S2(5), P3(3), C3(2), C3(3), S3(3));
	FA34: FullAdder port map (S2(6), P3(4), C3(3), C3(4), S3(4));
	FA35: FullAdder port map (S2(7), P3(5), C3(4), C3(5), S3(5));
	FA36: FullAdder port map (S2(8), P3(6), C3(5), C3(6), S3(6));
	FA37: FullAdder port map (S2(9), P3(7), C3(6), C3(7), S3(7));
	FA38: FullAdder port map (S2(10), P3(8), C3(7), C3(8), S3(8));
	FA39: FullAdder port map (S2(11), P3(9), C3(8), C3(9), S3(9));
	FA310: FullAdder port map (S2(12), P3(10), C3(9), C3(10), S3(10));
	FA311: FullAdder port map (S2(13), P3(11), C3(10), C3(11), S3(11));
	FA312: FullAdder port map (S2(14), P3(12), C3(11), C3(12), S3(12));
	FA313: FullAdder port map (S2(15), P3(13), C3(12), C3(13), S3(13));
	FA314: FullAdder port map (S2(16), P3(14), C3(13), C3(14), S3(14));
	FA315: FullAdder port map (S2(17), P3(15), C3(14), C3(15), S3(15));
	FA316: FullAdder port map (C2(17), E(3), C3(15), C3(16), S3(16));

	FA41: FullAdder port map (S3(3), P4(1), C4(0), C4(1), S4(1));
	FA42: FullAdder port map (S3(4), P4(2), C4(1), C4(2), S4(2));
	FA43: FullAdder port map (S3(5), P4(3), C4(2), C4(3), S4(3));
	FA44: FullAdder port map (S3(6), P4(4), C4(3), C4(4), S4(4));
	FA45: FullAdder port map (S3(7), P4(5), C4(4), C4(5), S4(5));
	FA46: FullAdder port map (S3(8), P4(6), C4(5), C4(6), S4(6));
	FA47: FullAdder port map (S3(9), P4(7), C4(6), C4(7), S4(7));
	FA48: FullAdder port map (S3(10), P4(8), C4(7), C4(8), S4(8));
	FA49: FullAdder port map (S3(11), P4(9), C4(8), C4(9), S4(9));
	FA410: FullAdder port map (S3(12), P4(10), C4(9), C4(10), S4(10));
	FA411: FullAdder port map (S3(13), P4(11), C4(10), C4(11), S4(11));
	FA412: FullAdder port map (S3(14), P4(12), C4(11), C4(12), S4(12));
	FA413: FullAdder port map (S3(15), P4(13), C4(12), C4(13), S4(13));
	FA414: FullAdder port map (S3(16), P4(14), C4(13), C4(14), S4(14));
	FA415: FullAdder port map (S3(17), P4(15), C4(14), C4(15), S4(15));
	FA416: FullAdder port map (C3(17), E(4), C4(15), C4(16), S4(16));

	FA51: FullAdder port map (S4(3), P5(1), C5(0), C5(1), S5(1));
	FA52: FullAdder port map (S4(4), P5(2), C5(1), C5(2), S5(2));
	FA53: FullAdder port map (S4(5), P5(3), C5(2), C5(3), S5(3));
	FA54: FullAdder port map (S4(6), P5(4), C5(3), C5(4), S5(4));
	FA55: FullAdder port map (S4(7), P5(5), C5(4), C5(5), S5(5));
	FA56: FullAdder port map (S4(8), P5(6), C5(5), C5(6), S5(6));
	FA57: FullAdder port map (S4(9), P5(7), C5(6), C5(7), S5(7));
	FA58: FullAdder port map (S4(10), P5(8), C5(7), C5(8), S5(8));
	FA59: FullAdder port map (S4(11), P5(9), C5(8), C5(9), S5(9));
	FA510: FullAdder port map (S4(12), P5(10), C5(9), C5(10), S5(10));
	FA511: FullAdder port map (S4(13), P5(11), C5(10), C5(11), S5(11));
	FA512: FullAdder port map (S4(14), P5(12), C5(11), C5(12), S5(12));
	FA513: FullAdder port map (S4(15), P5(13), C5(12), C5(13), S5(13));
	FA514: FullAdder port map (S4(16), P5(14), C5(13), C5(14), S5(14));
	FA515: FullAdder port map (S4(17), P5(15), C5(14), C5(15), S5(15));
	FA516: FullAdder port map (C4(17), E(5), C5(15), C5(16), S5(16));

	FA61: FullAdder port map (S5(3), P6(1), C6(0), C6(1), S6(1));
	FA62: FullAdder port map (S5(4), P6(2), C6(1), C6(2), S6(2));
	FA63: FullAdder port map (S5(5), P6(3), C6(2), C6(3), S6(3));
	FA64: FullAdder port map (S5(6), P6(4), C6(3), C6(4), S6(4));
	FA65: FullAdder port map (S5(7), P6(5), C6(4), C6(5), S6(5));
	FA66: FullAdder port map (S5(8), P6(6), C6(5), C6(6), S6(6));
	FA67: FullAdder port map (S5(9), P6(7), C6(6), C6(7), S6(7));
	FA68: FullAdder port map (S5(10), P6(8), C6(7), C6(8), S6(8));
	FA69: FullAdder port map (S5(11), P6(9), C6(8), C6(9), S6(9));
	FA610: FullAdder port map (S5(12), P6(10), C6(9), C6(10), S6(10));
	FA611: FullAdder port map (S5(13), P6(11), C6(10), C6(11), S6(11));
	FA612: FullAdder port map (S5(14), P6(12), C6(11), C6(12), S6(12));
	FA613: FullAdder port map (S5(15), P6(13), C6(12), C6(13), S6(13));
	FA614: FullAdder port map (S5(16), P6(14), C6(13), C6(14), S6(14));
	FA615: FullAdder port map (S5(17), P6(15), C6(14), C6(15), S6(15));
	FA616: FullAdder port map (C5(17), E(6), C6(15), C6(16), S6(16));

	FA71: FullAdder port map (S6(3), P7(1), C7(0), C7(1), S7(1));
	FA72: FullAdder port map (S6(4), P7(2), C7(1), C7(2), S7(2));
	FA73: FullAdder port map (S6(5), P7(3), C7(2), C7(3), S7(3));
	FA74: FullAdder port map (S6(6), P7(4), C7(3), C7(4), S7(4));
	FA75: FullAdder port map (S6(7), P7(5), C7(4), C7(5), S7(5));
	FA76: FullAdder port map (S6(8), P7(6), C7(5), C7(6), S7(6));
	FA77: FullAdder port map (S6(9), P7(7), C7(6), C7(7), S7(7));
	FA78: FullAdder port map (S6(10), P7(8), C7(7), C7(8), S7(8));
	FA79: FullAdder port map (S6(11), P7(9), C7(8), C7(9), S7(9));
	FA710: FullAdder port map (S6(12), P7(10), C7(9), C7(10), S7(10));
	FA711: FullAdder port map (S6(13), P7(11), C7(10), C7(11), S7(11));
	FA712: FullAdder port map (S6(14), P7(12), C7(11), C7(12), S7(12));
	FA713: FullAdder port map (S6(15), P7(13), C7(12), C7(13), S7(13));
	FA714: FullAdder port map (S6(16), P7(14), C7(13), C7(14), S7(14));
	FA715: FullAdder port map (S6(17), P7(15), C7(14), C7(15), S7(15));
	FA716: FullAdder port map (C6(17), E(7), C7(15), C7(16), S7(16));
	
	--some of these half adders add 1 for 2's complement on partial products that are not reduced 
	--and the others are adding 1 to the MSB of a partial product for sign extensions
	HA12: HalfAdder port map ('1', C1(16), C1(17), S1(17));
	HA21: HalfAdder port map (S1(2), PS(2), C2(0), S2(0));
	HA22: HalfAdder port map ('1', C2(16), C2(17), S2(17));
	HA31: HalfAdder port map (S2(2), PS(3), C3(0), S3(0));
	HA32: HalfAdder port map ('1', C3(16), C3(17), S3(17));
	HA41: HalfAdder port map (S3(2), PS(4), C4(0), S4(0));
	HA42: HalfAdder port map ('1', C4(16), C4(17), S4(17));
	HA51: HalfAdder port map (S4(2), PS(5), C5(0), S5(0));
	HA52: HalfAdder port map ('1', C5(16), C5(17), S5(17));
	HA61: HalfAdder port map (S5(2), PS(6), C6(0), S6(0));
	HA62: HalfAdder port map ('1', C6(16), C6(17), S6(17));
	HA71: HalfAdder port map (S6(2), PS(7), C7(0), S7(0));
	HA72: HalfAdder port map ('1', C7(16), C7(17), S7(17));

	P(m-2 downto 0) <= "000"; --set the lowest bits to zeros since the matrix is reduced
	P(3) <= P0(3); P(4) <= S3(0); P(5) <= S3(1); P(6) <= S4(0);
	P(7) <= S3(1); P(8) <= S4(0); P(9) <= S4(1); P(10) <= S5(0); P(11) <= S5(1);
	P(12) <= S6(0); P(13) <= S6(1); 
	P(31 downto 14) <= S7(17 downto 0);
end Behavioral;			