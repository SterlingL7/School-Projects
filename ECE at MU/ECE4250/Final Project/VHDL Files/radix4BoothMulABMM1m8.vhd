------
-- 8-bit version of ABM-M1
-- bits below m significance are set to NOT a
-- if the encoding is negative or
-- set to a if the encoding is non-zero and non-negative
-- this is done along the bit plane of the partial product matrix,
-- not the individual partial products
-- correction terms are ORed with the approximated output
------

library IEEE; 
use IEEE.std_logic_1164.all;

entity radix4BoothABMM1m8 is 
 	port (A, B: in std_logic_vector(15 downto 0); clk: in std_logic;	--Inputs
 		P: out std_logic_vector(31 downto 0)); 				--Outputs
end radix4BoothABMM1m8;
 
Architecture Behavioral of radix4BoothABMM1m8 is 
signal C1, C2, C3, C4, C5, C6, C7: std_logic_vector(17 downto 0); 	--carry signals
signal S1, S2, S3, S4, S5, S6, S7: std_logic_vector(17 downto 0);	--partial sum signals
signal P0, P1, P2, P3, P4, P5, P6, P7: std_logic_vector(15 downto 0);	--partial product signals
signal neg, two, zero: std_logic_vector(7 downto 0);			--booth encoding signals
signal m0: std_logic_vector(15 downto 8);				--multiplexer output signals
signal m1: std_logic_vector(15 downto 6);
signal m2: std_logic_vector(15 downto 4);
signal m3: std_logic_vector(15 downto 2);
signal m4, m5, m6, m7: std_logic_vector(15 downto 0);
signal E, PS: std_logic_vector(7 downto 0);				--sign-extension signals
signal NE: std_logic;							--sign-extension signal
constant N: integer := 15;
constant K: integer := 7;

component FullAdder
	 port(X,Y,Ci: in std_logic; 	--Inputs
		Co,Sum: out std_logic); --Outputs
end component;

component HalfAdder
	 port(X,Y: in std_logic; 	--Inputs
		Co,Sum: out std_logic); --Outputs
end component;

begin 
 	process (clk)
	begin
		if clk'event and clk = '1' then
		case B(1) is	--first encoding appends a 0 to the LSB
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
			end case;
		when others => null;
		end case;

		--generate the first partial products row from the encoding bits
		for j in 0 to 7 loop
			P0(j) <= (not A(j) and neg(0)) or (A(j) and not neg(0) and not zero(0));
		end loop;
		for j in 8 to N loop
			m0(j) <= (A(j) and not two(0)) or (A(j-1) and two(0)); 
			P0(j) <= (not zero(0)) and (neg(0) xor m0(j));
		end loop;	

		for i in 1 to K loop	--loop to fill the 7 remaining encodings
			case B(2*i+1) is
			when '0' =>
				case B(2*i) is
				when '0' =>
					case B(2*i-1) is
					when '0' => neg(i) <= '0'; two(i) <= '0'; zero(i) <= '1';
					when '1' => neg(i) <= '0'; two(i) <= '0'; zero(i) <= '0';
					when others => null;
					end case;
				when '1' =>
					case B(2*i-1) is
					when '0' => neg(i) <= '0'; two(i) <= '0'; zero(i) <= '0';
					when '1' => neg(i) <= '0'; two(i) <= '1'; zero(i) <= '0';
					when others => null;
					end case;		
				when others => null;
				end case;
			when '1' =>
				case B(2*i) is
				when '0' =>
					case B(2*i-1) is
					when '0' => neg(i) <= '1'; two(i) <= '1'; zero(i) <= '0';
					when '1' => neg(i) <= '1'; two(i) <= '0'; zero(i) <= '0';
					when others => null;
					end case;
				when '1' =>
					case B(2*i-1) is
					when '0' => neg(i) <= '1'; two(i) <= '0'; zero(i) <= '0';
					when '1' => neg(i) <= '0'; two(i) <= '0'; zero(i) <= '1';
					when others => null;
					end case;		
				when others => null;
				end case;
			when others => null;
			end case;
		
			case i is	
			when 1 => 
				for j in 0 to 5 loop	--approximate partital product
					P1(j) <= (not A(j) and neg(i)) or (A(j) and not neg(i) and not zero(i));
				end loop;
				for j in 6 to N loop
					m1(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P1(j) <= (not zero(i)) and (neg(i) xor m1(j));
				end loop;	
			when 2 => 
				for j in 0 to 3 loop	--approximate partital product
					P2(j) <= (not A(j) and neg(i)) or (A(j) and not neg(i) and not zero(i));
				end loop;
				for j in 4 to N loop
					m2(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P2(j) <= (not zero(i)) and (neg(i) xor m2(j));
				end loop;
			when 3 => 
				for j in 0 to 1 loop	--approximate partital product
					P3(j) <= (not A(j) and neg(i)) or (A(j) and not neg(i) and not zero(i));
				end loop;
				for j in 2 to N loop
					m3(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P3(j) <= (not zero(i)) and (neg(i) xor m3(j));
				end loop;
			when 4 => 
				m4(0) <= (A(0) and not two(i)); P4(0) <= (not zero(i)) and (neg(i) xor m4(0)); 
				for j in 1 to N loop
					m4(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P4(j) <= (not zero(i)) and (neg(i) xor m4(j));
				end loop;
			when 5 => 
				m5(0) <= (A(0) and not two(i)); P5(0) <= (not zero(i)) and (neg(i) xor m5(0)); 
				for j in 1 to N loop
					m5(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P5(j) <= (not zero(i)) and (neg(i) xor m5(j));
				end loop;
			when 6 => 
				m6(0) <= (A(0) and not two(i)); P6(0) <= (not zero(i)) and (neg(i) xor m6(0)); 
				for j in 1 to N loop
					m6(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P6(j) <= (not zero(i)) and (neg(i) xor m6(j));
				end loop;
			when 7 => 
				m7(0) <= (A(0) and not two(i)); P7(0) <= (not zero(i)) and (neg(i) xor m7(0)); 
				for j in 1 to N loop
					m7(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); 
					P7(j) <= (not zero(i)) and (neg(i) xor m7(j));
				end loop;
			end case;
		end loop;
		end if;
	end process;

	--sign extension bitss
	E(0) <= zero(0) or (neg(0) xnor A(15)); PS(0) <= P0(0) or neg(0); NE <= not E(0);
	E(1) <= zero(1) or (neg(1) xnor A(15)); PS(1) <= P1(0) or neg(1);
	E(2) <= zero(2) or (neg(2) xnor A(15)); PS(2) <= P2(0) or neg(2);
	E(3) <= zero(3) or (neg(3) xnor A(15)); PS(3) <= P3(0) or neg(3);
	E(4) <= zero(4) or (neg(4) xnor A(15)); PS(4) <= P4(0) or neg(4);
	E(5) <= zero(5) or (neg(5) xnor A(15)); PS(5) <= P5(0) or neg(5);
	E(6) <= zero(6) or (neg(6) xnor A(15)); PS(6) <= P6(0) or neg(6); 
	E(7) <= zero(7) or (neg(7) xnor A(15)); PS(7) <= P7(0) or neg(7); 

	--generate 16 copies of the FullAdder FA1
	FullAdd1: for i in 0 to 15 generate
	begin
		Bit0_12: if i<13 generate
			FA1: FullAdder port map (P0(i+3), P1(i+1), C1(i), C1(i+1), S1(i+1));
		end generate Bit0_12;
		Bit13_14: if 12<i and i<15 generate
			FA1: FullAdder port map (NE, P1(i+1), C1(i), C1(i+1), S1(i+1));
		end generate Bit13_14;
		Bit15: if i=15 generate
			FA1: FullAdder port map (E(i-15), E(i-14), C1(i), C1(i+1), S1(i+1));
		end generate Bit15;
	end generate FullAdd1;

	--generate 16x6 copies of the FullAdder FA2, FA3, FA4, FA5, FA6, FA7
	FullAdd27: for i in 0 to 15 generate
	begin
		Bit0_14: if i<15 generate
			FA2: FullAdder port map (S1(i+3), P2(i+1), C2(i), C2(i+1), S2(i+1));
			FA3: FullAdder port map (S2(i+3), P3(i+1), C3(i), C3(i+1), S3(i+1));
			FA4: FullAdder port map (S3(i+3), P4(i+1), C4(i), C4(i+1), S4(i+1));
			FA5: FullAdder port map (S4(i+3), P5(i+1), C5(i), C5(i+1), S5(i+1));
			FA6: FullAdder port map (S5(i+3), P6(i+1), C6(i), C6(i+1), S6(i+1));
			FA7: FullAdder port map (S6(i+3), P7(i+1), C7(i), C7(i+1), S7(i+1));
		end generate Bit0_14;
		Bit15: if i=15 generate
			FA2: FullAdder port map (C1(i+2), E(i-13), C2(i), C2(i+1), S2(i+1));
			FA3: FullAdder port map (C2(i+2), E(i-12), C3(i), C3(i+1), S3(i+1));
			FA4: FullAdder port map (C3(i+2), E(i-11), C4(i), C4(i+1), S4(i+1));
			FA5: FullAdder port map (C4(i+2), E(i-10), C5(i), C5(i+1), S5(i+1));
			FA6: FullAdder port map (C5(i+2), E(i-9), C6(i), C6(i+1), S6(i+1));
			FA7: FullAdder port map (C6(i+2), E(i-8), C7(i), C7(i+1), S7(i+1));
		end generate Bit15;
	end generate FullAdd27;

	--half adders of the LSB of each partial product
	HA11: HalfAdder port map (P0(2), PS(1), C1(0), S1(0));
	HA21: HalfAdder port map (S1(2), PS(2), C2(0), S2(0));
	HA31: HalfAdder port map (S2(2), PS(3), C3(0), S3(0));
	HA41: HalfAdder port map (S3(2), PS(4), C4(0), S4(0));
	HA51: HalfAdder port map (S4(2), PS(5), C5(0), S5(0));
	HA61: HalfAdder port map (S5(2), PS(6), C6(0), S6(0));
	HA71: HalfAdder port map (S6(2), PS(7), C7(0), S7(0));

	--half adders for sign extension after the MSB of each partial product
	HA12: HalfAdder port map ('1', C1(16), C1(17), S1(17));
	HA22: HalfAdder port map ('1', C2(16), C2(17), S2(17));
	HA32: HalfAdder port map ('1', C3(16), C3(17), S3(17));
	HA42: HalfAdder port map ('1', C4(16), C4(17), S4(17));
	HA52: HalfAdder port map ('1', C5(16), C5(17), S5(17));
	HA62: HalfAdder port map ('1', C6(16), C6(17), S6(17));
	HA72: HalfAdder port map ('1', C7(16), C7(17), S7(17));

	P(0) <= PS(0); P(1) <= P0(1); P(2) <= S1(0); P(3) <= S1(1); P(4) <= S2(0); P(5) <= S2(1);
	P(6) <= S3(0); P(7) <= S3(1); P(8) <= S4(0); P(9) <= S4(1); P(10) <= S5(0); P(11) <= S5(1);
	P(12) <= S6(0); P(13) <= S6(1);
	P(31 downto 14) <= S7(17 downto 0);

end Behavioral;			
