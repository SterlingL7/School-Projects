----------
-- 8-bit version of ABM-M2
--each partial product is width reduced by 
--summing the LSBs of A to get bit Aj, then passing
--Aj to the approximate partial product generator
----------

library IEEE; 
USE IEEE.numeric_bit.ALL; 
use IEEE.std_logic_1164.all;

entity radix4BoothABMM2m8 is 
 	port (A, B: in std_logic_vector(15 downto 0); clk: in std_logic;
 		P: out std_logic_vector(31 downto 0)); 
end radix4BoothABMM2m8;

Architecture Behavioral of radix4BoothABMM2m8 is 
constant m: integer := 8; --number of bits to approximate
signal Aj: std_logic := '0'; --bit used in approximation
signal Asum: integer := 0; --sum of LSBs used to determine Aj value
signal C1, C2, C3, C4, C5, C6, C7: std_logic_vector(17 downto m-1); 
signal S1, S2, S3, S4, S5, S6, S7: std_logic_vector(17 downto m-1);

--partial products
signal P0, P1, P2, P3, P4, P5, P6, P7: std_logic_vector(15 downto m-1);
--encoding bits
signal neg, two, zero: std_logic_vector(7 downto 0);
--intermediate value for exact booth multiplier
signal m0, m1, m2, m3, m4, m5, m6, m7: std_logic_vector(15 downto m);

--sign extension values
signal E: std_logic_vector(7 downto 0);
signal NE: std_logic;

constant N: integer := 15;
constant K: integer := 7;

--function to do bitwise sum on a vector
--used to determine the value of the Aj bit 
--which is used as input to the partial product generator
function bit_sum( in_vec: std_logic_vector)
	return integer is
	variable sum: integer:=0;
	begin
	for i in 0 to in_vec'length-1 loop
		if in_vec(i) = '1' then
			sum := sum + 1;
		end if;
	end loop;
	return sum;
end function bit_sum;

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
		case B(1) is
		when '0' => 
			case B(0) is 
			when '0' => neg(0) <= '0'; two(0) <= '0'; zero(0) <= '1';
			when '1' => neg(0) <= '0'; two(0) <= '0'; zero(0) <= '0'; when others => null;
			end case;
		when '1' => 
			case B(0) is 
			when '0' => neg(0) <= '1'; two(0) <= '1'; zero(0) <= '0';
			when '1' => neg(0) <= '1'; two(0) <= '0'; zero(0) <= '0'; when others => null;
			end case; 
		when others => null;
		end case;

		--determine the Aj value for the approximation as
		-- 0 if sum of m LSBs is < m/2, else 1;
		Asum <= bit_sum(A(m-1 downto 0));
		if Asum >= m/2 then
			Aj <= '1';
		else
			Aj <= '0';
		end if;

		--generate LSB of P0 using the Aj value
		P0(m-1) <= (not Aj and neg(0)) or (Aj and not neg(0) and not zero(0));

		--generate the higher bits with exact PPG
		for i in m to N loop
			m0(i) <= (A(i) and not two(0)) or (A(i-1) and two(0));
			P0(i) <= (not zero(0)) and (neg(0) xor m0(i));
		end loop;

		for i in 1 to K loop --encode the other 7 Partial Products
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
				--first element is the approximation using Aj 
				P1(m-1) <= (not Aj and neg(i)) or (Aj and not neg(i) and not zero(i));
				--exact partial products for the remainder
				for j in m to N loop
					m1(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P1(j) <= (not zero(i)) and (neg(i) xor m1(j));
				end loop;	
			when 2 => 
				
				P2(m-1) <= (not Aj and neg(i)) or (Aj and not neg(i) and not zero(i));
				
				for j in m to N loop
					m2(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P2(j) <= (not zero(i)) and (neg(i) xor m2(j));
				end loop;
			when 3 => 
				
				P3(m-1) <= (not Aj and neg(i)) or (Aj and not neg(i) and not zero(i));
				
				for j in m to N loop
					m3(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P3(j) <= (not zero(i)) and (neg(i) xor m3(j));
				end loop;
			when 4 => 
				P4(m-1) <= (not Aj and neg(i)) or (Aj and not neg(i) and not zero(i));
				for j in m to N loop
					m4(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P4(j) <= (not zero(i)) and (neg(i) xor m4(j));
				end loop;
			when 5 => 

				P5(m-1) <= (not Aj and neg(i)) or (Aj and not neg(i) and not zero(i));
				for j in m to N loop
					m5(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P5(j) <= (not zero(i)) and (neg(i) xor m5(j));
				end loop;
			when 6 => 
	
				P6(m-1) <= (not Aj and neg(i)) or (Aj and not neg(i) and not zero(i));
				for j in m to N loop
					m6(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P6(j) <= (not zero(i)) and (neg(i) xor m6(j));
				end loop;
			when 7 =>  
				P7(m-1) <= (not Aj and neg(i)) or (Aj and not neg(i) and not zero(i));
				for j in m to N loop
					m7(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P7(j) <= (not zero(i)) and (neg(i) xor m7(j));
				end loop;
			when others => null;
			end case;
		end loop;

		end if;
	end process;

	--set sign extension bits
	E(0) <= zero(0) or (neg(0) xnor A(15)); NE <= not E(0);
	E(1) <= zero(1) or (neg(1) xnor A(15));
	E(2) <= zero(2) or (neg(2) xnor A(15));
	E(3) <= zero(3) or (neg(3) xnor A(15));
	E(4) <= zero(4) or (neg(4) xnor A(15));
	E(5) <= zero(5) or (neg(5) xnor A(15));
	E(6) <= zero(6) or (neg(6) xnor A(15));
	E(7) <= zero(7) or (neg(7) xnor A(15));

	--each row of adders width reduced by m-1 
	HA17: HalfAdder port map (P0(9), P1(7), C1(7), S1(7));
	FA18: FullAdder port map (P0(10), P1(8), C1(7), C1(8), S1(8));
	FA19: FullAdder port map (P0(11), P1(9), C1(8), C1(9), S1(9));
	FA110: FullAdder port map (P0(12), P1(10), C1(9), C1(10), S1(10));
	FA111: FullAdder port map (P0(13), P1(11), C1(10), C1(11), S1(11));
	FA112: FullAdder port map (P0(14), P1(12), C1(11), C1(12), S1(12));
	FA113: FullAdder port map (P0(15), P1(13), C1(12), C1(13), S1(13));
	FA114: FullAdder port map (NE, P1(14), C1(13), C1(14), S1(14));
	FA115: FullAdder port map (NE, P1(15), C1(14), C1(15), S1(15));
	FA116: FullAdder port map (E(0), E(1), C1(15), C1(16), S1(16));


	HA27: HalfAdder port map (S1(9), P2(7), C2(7), S2(7));
	FA28: FullAdder port map (S1(10), P2(8), C2(7), C2(8), S2(8));
	FA29: FullAdder port map (S1(11), P2(9), C2(8), C2(9), S2(9));
	FA210: FullAdder port map (S1(12), P2(10), C2(9), C2(10), S2(10));
	FA211: FullAdder port map (S1(13), P2(11), C2(10), C2(11), S2(11));
	FA212: FullAdder port map (S1(14), P2(12), C2(11), C2(12), S2(12));
	FA213: FullAdder port map (S1(15), P2(13), C2(12), C2(13), S2(13));
	FA214: FullAdder port map (S1(16), P2(14), C2(13), C2(14), S2(14));
	FA215: FullAdder port map (S1(17), P2(15), C2(14), C2(15), S2(15));
	FA216: FullAdder port map (C1(17), E(2), C2(15), C2(16), S2(16));

	HA37: HalfAdder port map (S2(9), P3(7), C3(7), S3(7));
	FA38: FullAdder port map (S2(10), P3(8), C3(7), C3(8), S3(8));
	FA39: FullAdder port map (S2(11), P3(9), C3(8), C3(9), S3(9));
	FA310: FullAdder port map (S2(12), P3(10), C3(9), C3(10), S3(10));
	FA311: FullAdder port map (S2(13), P3(11), C3(10), C3(11), S3(11));
	FA312: FullAdder port map (S2(14), P3(12), C3(11), C3(12), S3(12));
	FA313: FullAdder port map (S2(15), P3(13), C3(12), C3(13), S3(13));
	FA314: FullAdder port map (S2(16), P3(14), C3(13), C3(14), S3(14));
	FA315: FullAdder port map (S2(17), P3(15), C3(14), C3(15), S3(15));
	FA316: FullAdder port map (C2(17), E(3), C3(15), C3(16), S3(16));

	HA47: HalfAdder port map (S3(9), P4(7), C4(7), S4(7));
	FA48: FullAdder port map (S3(10), P4(8), C4(7), C4(8), S4(8));
	FA49: FullAdder port map (S3(11), P4(9), C4(8), C4(9), S4(9));
	FA410: FullAdder port map (S3(12), P4(10), C4(9), C4(10), S4(10));
	FA411: FullAdder port map (S3(13), P4(11), C4(10), C4(11), S4(11));
	FA412: FullAdder port map (S3(14), P4(12), C4(11), C4(12), S4(12));
	FA413: FullAdder port map (S3(15), P4(13), C4(12), C4(13), S4(13));
	FA414: FullAdder port map (S3(16), P4(14), C4(13), C4(14), S4(14));
	FA415: FullAdder port map (S3(17), P4(15), C4(14), C4(15), S4(15));
	FA416: FullAdder port map (C3(17), E(4), C4(15), C4(16), S4(16));

	HA57: HalfAdder port map (S4(9), P5(7), C5(7), S5(7));
	FA58: FullAdder port map (S4(10), P5(8), C5(7), C5(8), S5(8));
	FA59: FullAdder port map (S4(11), P5(9), C5(8), C5(9), S5(9));
	FA510: FullAdder port map (S4(12), P5(10), C5(9), C5(10), S5(10));
	FA511: FullAdder port map (S4(13), P5(11), C5(10), C5(11), S5(11));
	FA512: FullAdder port map (S4(14), P5(12), C5(11), C5(12), S5(12));
	FA513: FullAdder port map (S4(15), P5(13), C5(12), C5(13), S5(13));
	FA514: FullAdder port map (S4(16), P5(14), C5(13), C5(14), S5(14));
	FA515: FullAdder port map (S4(17), P5(15), C5(14), C5(15), S5(15));
	FA516: FullAdder port map (C4(17), E(5), C5(15), C5(16), S5(16));

	HA67: HalfAdder port map (S5(9), P6(7), C6(7), S6(7));
	FA68: FullAdder port map (S5(10), P6(8), C6(7), C6(8), S6(8));
	FA69: FullAdder port map (S5(11), P6(9), C6(8), C6(9), S6(9));
	FA610: FullAdder port map (S5(12), P6(10), C6(9), C6(10), S6(10));
	FA611: FullAdder port map (S5(13), P6(11), C6(10), C6(11), S6(11));
	FA612: FullAdder port map (S5(14), P6(12), C6(11), C6(12), S6(12));
	FA613: FullAdder port map (S5(15), P6(13), C6(12), C6(13), S6(13));
	FA614: FullAdder port map (S5(16), P6(14), C6(13), C6(14), S6(14));
	FA615: FullAdder port map (S5(17), P6(15), C6(14), C6(15), S6(15));
	FA616: FullAdder port map (C5(17), E(6), C6(15), C6(16), S6(16));

	HA77: HalfAdder port map (S6(9), P7(7), C7(7), S7(7));
	FA78: FullAdder port map (S6(10), P7(8), C7(7), C7(8), S7(8));
	FA79: FullAdder port map (S6(11), P7(9), C7(8), C7(9), S7(9));
	FA710: FullAdder port map (S6(12), P7(10), C7(9), C7(10), S7(10));
	FA711: FullAdder port map (S6(13), P7(11), C7(10), C7(11), S7(11));
	FA712: FullAdder port map (S6(14), P7(12), C7(11), C7(12), S7(12));
	FA713: FullAdder port map (S6(15), P7(13), C7(12), C7(13), S7(13));
	FA714: FullAdder port map (S6(16), P7(14), C7(13), C7(14), S7(14));
	FA715: FullAdder port map (S6(17), P7(15), C7(14), C7(15), S7(15));
	FA716: FullAdder port map (C6(17), E(7), C7(15), C7(16), S7(16));
	
	--these half adders are adding 1 to the MSB of a partial product for sign extensions
	HA12: HalfAdder port map ('1', C1(16), C1(17), S1(17));
	HA22: HalfAdder port map ('1', C2(16), C2(17), S2(17));
	HA32: HalfAdder port map ('1', C3(16), C3(17), S3(17));
	HA42: HalfAdder port map ('1', C4(16), C4(17), S4(17));
	HA52: HalfAdder port map ('1', C5(16), C5(17), S5(17));
	HA62: HalfAdder port map ('1', C6(16), C6(17), S6(17));
	HA72: HalfAdder port map ('1', C7(16), C7(17), S7(17));

	--fill in final product
	P(m-2 downto 0) <= "0000000"; P(7) <= P0(7); 
	P(8) <= P0(8); P(9) <= S1(7); P(10) <= S1(8); P(11) <= S2(7);
	P(12) <= S2(8); P(13) <= S3(7); P(14) <= S3(8); P(15) <= S4(7);
	P(16) <= S4(8); P(17) <= S5(7); P(18) <= S5(8); P(19) <= S6(7); 
	P(20) <= S6(8); 
	P(31 downto 21) <= S7(17 downto m-1);

	
end Behavioral;