
------
-- 16-bit version of ABM-M3
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

entity radix4BoothABMM3m16 is 
 	port (A, B: in std_logic_vector(15 downto 0); clk: in std_logic;
 		P: out std_logic_vector(31 downto 0)); 
end radix4BoothABMM3m16;
 
Architecture Behavioral of radix4BoothABMM3m16 is 
constant m: integer := 16; --number of bits to approximate
--each vector is width reduced 
--ex. (15 downto 13) for P1 in this 16-bit approximation
signal C1: std_logic_vector(17 downto m-3);
signal C2: std_logic_vector(17 downto m-5);
signal C3: std_logic_vector(17 downto m-7);
signal C4: std_logic_vector(17 downto m-9);
signal C5: std_logic_vector(17 downto m-11);
signal C6: std_logic_vector(17 downto m-13); 
signal C7: std_logic_vector(17 downto m-15); 
--partial sum signals
signal S1: std_logic_vector(17 downto m-3);
signal S2: std_logic_vector(17 downto m-5);
signal S3: std_logic_vector(17 downto m-7);
signal S4: std_logic_vector(17 downto m-9);
signal S5: std_logic_vector(17 downto m-11);
signal S6: std_logic_vector(17 downto m-13);
signal S7: std_logic_vector(17 downto m-15);
--partial product signals
signal P0: std_logic_vector(15 downto m-1);
signal P1: std_logic_vector(15 downto m-3); 
signal P2: std_logic_vector(15 downto m-5);
signal P3: std_logic_vector(15 downto m-7); 
signal P4: std_logic_vector(15 downto m-9);
signal P5: std_logic_vector(15 downto m-11);
signal P6: std_logic_vector(15 downto m-13);
signal P7: std_logic_vector(15 downto m-15);
--booth encoding signals
signal neg, two, zero: std_logic_vector(7 downto 0);

--signal m0: std_logic_vector(15 downto m); removed since no exact partial products
signal m1: std_logic_vector(15 downto m-2);
signal m2: std_logic_vector(15 downto m-4);
signal m3: std_logic_vector(15 downto m-6);
signal m4: std_logic_vector(15 downto m-8);
signal m5: std_logic_vector(15 downto m-10);
signal m6: std_logic_vector(15 downto m-12); 
signal m7: std_logic_vector(15 downto m-14);

-- E is sign extensions, PS is adding 1 for 2's complement
signal E, PS: std_logic_vector(7 downto 0);
signal NE: std_logic;
constant N: integer := 15;
constant K: integer := 7;

--since our code uses std_logic_vectors I defined an OR-reduction 
--that performs a bitwise or on an input std_logic_vector
-- function or_reduce( in_vec: std_logic_vector)
	-- return std_logic is
	-- variable orreduce: std_logic:='0';
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
		--P0 is reduced to a single bit in this model
		
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
				
				--first the approximate bits
				P1(m-3) <= or_reduce(A(m-3 downto 0)) and not zero(1);
				--then the exact bits
				for j in m-2 to N loop
					m1(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P1(j) <= (not zero(i)) and (neg(i) xor m1(j));
				end loop;	
			when 2 => 
				
				P2(m-5) <= or_reduce(A(m-5 downto 0)) and not zero(2);
				
				for j in m-4 to N loop
					m2(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P2(j) <= (not zero(i)) and (neg(i) xor m2(j));
				end loop;
			when 3 => 
				
				P3(m-7) <= or_reduce(A(m-7 downto 0)) and not zero(3);
				
				for j in m-6 to N loop
					m3(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P3(j) <= (not zero(i)) and (neg(i) xor m3(j));
				end loop;
			when 4 => 
				P4(m-9) <= or_reduce(A(m-9 downto 0)) and not zero(4);
				for j in m-8 to N loop
					m4(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P4(j) <= (not zero(i)) and (neg(i) xor m4(j));
				end loop;
			when 5 => 
				P5(m-11) <= or_reduce(A(m-11 downto 0)) and not zero(5);
				for j in m-10 to N loop
					m5(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P5(j) <= (not zero(i)) and (neg(i) xor m5(j));
				end loop;
			when 6 => 
				P6(m-13) <= or_reduce(A(m-13 downto 0)) and not zero(6);
				for j in m-12 to N loop
					m6(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P6(j) <= (not zero(i)) and (neg(i) xor m6(j));
				end loop;
			when 7 =>  
				P7(m-15) <= or_reduce(A(m-15 downto 0)) and not zero(7);
				for j in m-14 to N loop
					m7(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P7(j) <= (not zero(i)) and (neg(i) xor m7(j));
				end loop;
			when others => null;
			end case;
		end loop;
		end if;
	end process;

	--sign extension bits
	E(0) <= zero(0) or (neg(0) xnor A(15));  NE <= not E(0);
	E(1) <= zero(1) or (neg(1) xnor A(15)); 
	E(2) <= zero(2) or (neg(2) xnor A(15)); 
	E(3) <= zero(3) or (neg(3) xnor A(15)); 
	E(4) <= zero(4) or (neg(4) xnor A(15)); 
	E(5) <= zero(5) or (neg(5) xnor A(15)); 
	E(6) <= zero(6) or (neg(6) xnor A(15)); 
	E(7) <= zero(7) or (neg(7) xnor A(15)); 

--	half adders to combine all the approximate partial products 
	HA113: HalfAdder port map(P0(m-1), P1(m-3), C1(m-3), S1(m-3));
	HA211: HalfAdder port map(S1(m-3), P2(m-5), C2(m-5), S2(m-5));
	HA39: HalfAdder port map(S2(m-5), P3(m-7), C3(m-7), S3(m-7));
	HA47: HalfAdder port map(S3(m-7), P4(m-9), C4(m-9), S4(m-9));
	HA55: HalfAdder port map(S4(m-9), P5(m-11), C5(m-11), S5(m-11));
	HA63: HalfAdder port map(S5(m-11), P6(m-13), C6(m-13), S6(m-13));
	HA71: HalfAdder port map(S6(m-13), P7(m-15), C7(m-15), S7(m-15));
	
	--all but last full adder row are width reduced by the approximation
	FA114: FullAdder port map (NE, P1(14), C1(13), C1(14), S1(14));
	FA115: FullAdder port map (NE, P1(15), C1(14), C1(15), S1(15));
	FA116: FullAdder port map (E(0), E(1), C1(15), C1(16), S1(16));


	FA212: FullAdder port map (S1(14), P2(12), C2(11), C2(12), S2(12));
	FA213: FullAdder port map (S1(15), P2(13), C2(12), C2(13), S2(13));
	FA214: FullAdder port map (S1(16), P2(14), C2(13), C2(14), S2(14));
	FA215: FullAdder port map (S1(17), P2(15), C2(14), C2(15), S2(15));
	FA216: FullAdder port map (C1(17), E(2), C2(15), C2(16), S2(16));
	

	FA310: FullAdder port map (S2(12), P3(10), C3(9), C3(10), S3(10));
	FA311: FullAdder port map (S2(13), P3(11), C3(10), C3(11), S3(11));
	FA312: FullAdder port map (S2(14), P3(12), C3(11), C3(12), S3(12));
	FA313: FullAdder port map (S2(15), P3(13), C3(12), C3(13), S3(13));
	FA314: FullAdder port map (S2(16), P3(14), C3(13), C3(14), S3(14));
	FA315: FullAdder port map (S2(17), P3(15), C3(14), C3(15), S3(15));
	FA316: FullAdder port map (C2(17), E(3), C3(15), C3(16), S3(16));


	FA48: FullAdder port map (S3(10), P4(8), C4(7), C4(8), S4(8));
	FA49: FullAdder port map (S3(11), P4(9), C4(8), C4(9), S4(9));
	FA410: FullAdder port map (S3(12), P4(10), C4(9), C4(10), S4(10));
	FA411: FullAdder port map (S3(13), P4(11), C4(10), C4(11), S4(11));
	FA412: FullAdder port map (S3(14), P4(12), C4(11), C4(12), S4(12));
	FA413: FullAdder port map (S3(15), P4(13), C4(12), C4(13), S4(13));
	FA414: FullAdder port map (S3(16), P4(14), C4(13), C4(14), S4(14));
	FA415: FullAdder port map (S3(17), P4(15), C4(14), C4(15), S4(15));
	FA416: FullAdder port map (C3(17), E(4), C4(15), C4(16), S4(16));


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
	
--	half adders for sign extension after the MSB of each partial product
	HA12: HalfAdder port map ('1', C1(16), C1(17), S1(17));
	HA22: HalfAdder port map ('1', C2(16), C2(17), S2(17));
	HA32: HalfAdder port map ('1', C3(16), C3(17), S3(17));
	HA42: HalfAdder port map ('1', C4(16), C4(17), S4(17));
	HA52: HalfAdder port map ('1', C5(16), C5(17), S5(17));
	HA62: HalfAdder port map ('1', C6(16), C6(17), S6(17));
	HA72: HalfAdder port map ('1', C7(16), C7(17), S7(17));

	P(m-2 downto 0) <= "000000000000000"; --set the lowest bits m-1 to zeros since the matrix is reduced
	P(31 downto 15) <= S7(17 downto 1);

end Behavioral;			
