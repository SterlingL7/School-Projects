------
-- exact booth multiplier implementation
-- each bit is found using radix-4 encoding,
-- which can be +/-2, +/-1, or 0
-- encoding is generated from 3 bits of the multiplier
-- using the equation E = -2B(i+1) + B(i) + B(i-1)
-- this is represented as a 3-bit ecoding, 1 bit for each -,2,0
-- first, if 2 is set, the partial product will be A(i-1) else A(i)
-- then, if negative bit is set, A is negated, finally, if the zero
-- bit is set, Pi is set to 0
------

library IEEE; 
USE IEEE.numeric_bit.ALL; 
use IEEE.std_logic_1164.all;

entity radix4BoothMul is 
 	port (A, B: in std_logic_vector(15 downto 0); clk: in std_logic; 	--Inputs
 		P: out std_logic_vector(31 downto 0)); 				--Outputs
end radix4BoothMul;
 
Architecture Behavioral of radix4BoothMul is 
signal C1, C2, C3, C4, C5, C6, C7: std_logic_vector(19 downto 0); 	--carry signals
signal S1, S2, S3, S4, S5, S6, S7: std_logic_vector(19 downto 0);	--partial sum signals
signal C8: std_logic_vector(17 downto 0);				--carry signals
signal P0, P1, P2, P3, P4, P5, P6, P7: std_logic_vector(15 downto 0);	--partial product signals
signal neg, two, zero: std_logic_vector(7 downto 0);			--booth encoding signals
signal m0, m1, m2, m3, m4, m5, m6, m7: std_logic_vector(15 downto 0);	--multiplexer output signals
signal E: std_logic_vector(7 downto 0);					--sign-extension signals
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
		m0(0) <= (A(0) and not two(0));
		P0(0) <= (not zero(0)) and (neg(0) xor m0(0));
		for i in 1 to N loop
			m0(i) <= (A(i) and not two(0)) or (A(i-1) and two(0));
			P0(i) <= (not zero(0)) and (neg(0) xor m0(i));
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

			for j in 1 to N loop	--each encoding is used to generate the partial products
				case i is
				when 1 => m1(0) <= (A(0) and not two(1)); P1(0) <= (not zero(1)) and (neg(1) xor m1(0)); 
					m1(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); P1(j) <= (not zero(i)) and (neg(i) xor m1(j));
				when 2 => m2(0) <= (A(0) and not two(2)); P2(0) <= (not zero(2)) and (neg(2) xor m2(0)); 
					m2(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); P2(j) <= (not zero(i)) and (neg(i) xor m2(j));
				when 3 =>  m3(0) <= (A(0) and not two(3)); P3(0) <= (not zero(3)) and (neg(3) xor m3(0)); 
					m3(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); P3(j) <= (not zero(i)) and (neg(i) xor m3(j));
				when 4 => m4(0) <= (A(0) and not two(4)); P4(0) <= (not zero(4)) and (neg(4) xor m4(0)); 
					m4(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); P4(j) <= (not zero(i)) and (neg(i) xor m4(j));
				when 5 => m5(0) <= (A(0) and not two(5)); P5(0) <= (not zero(5)) and (neg(5) xor m5(0)); 
					m5(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); P5(j) <= (not zero(i)) and (neg(i) xor m5(j));
				when 6 => m6(0) <= (A(0) and not two(6)); P6(0) <= (not zero(6)) and (neg(6) xor m6(0)); 
					m6(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); P6(j) <= (not zero(i)) and (neg(i) xor m6(j));
				when 7 => m7(0) <= (A(0) and not two(7)); P7(0) <= (not zero(7)) and (neg(7) xor m7(0)); 
					m7(j) <= (A(j) and not two(i)) or (A(j-1) and two(i)); P7(j) <= (not zero(i)) and (neg(i) xor m7(j)); 
				end case;
			end loop;
		end loop;
		end if;
	end process;

	--sign extension bitss
	E(0) <= zero(0) or (neg(0) xnor A(15)); NE <= not E(0);
	E(1) <= zero(1) or (neg(1) xnor A(15));
	E(2) <= zero(2) or (neg(2) xnor A(15));
	E(3) <= zero(3) or (neg(3) xnor A(15));
	E(4) <= zero(4) or (neg(4) xnor A(15));
	E(5) <= zero(5) or (neg(5) xnor A(15));
	E(6) <= zero(6) or (neg(6) xnor A(15));
	E(7) <= zero(7) or (neg(7) xnor A(15));

	--generate 17 copies of the FullAdder FA1
	FullAdd1: for i in 0 to 16 generate
	begin
		Bit0_13: if i<14 generate
			FA1: FullAdder port map (P0(i+2), P1(i), C1(i+1), C1(i+2), S1(i+2));
		end generate Bit0_13;
		Bit14_15: if 13<i and i<16 generate
			FA1: FullAdder port map (NE, P1(i), C1(i+1), C1(i+2), S1(i+2));
		end generate Bit14_15;
		Bit16: if i=16 generate
			FA1: FullAdder port map (E(i-16), E(i-15), C1(i+1), C1(i+2), S1(i+2));
		end generate Bit16;
	end generate FullAdd1;

	--generate 17x6 copies of the FullAdder FA2, FA3, FA4, FA5, FA6, FA7
	FullAdd27: for i in 0 to 16 generate
	begin
		Bit0_15: if i<16 generate
			FA2: FullAdder port map (S1(i+4), P2(i), C2(i+1), C2(i+2), S2(i+2));
			FA3: FullAdder port map (S2(i+4), P3(i), C3(i+1), C3(i+2), S3(i+2));
			FA4: FullAdder port map (S3(i+4), P4(i), C4(i+1), C4(i+2), S4(i+2));
			FA5: FullAdder port map (S4(i+4), P5(i), C5(i+1), C5(i+2), S5(i+2));
			FA6: FullAdder port map (S5(i+4), P6(i), C6(i+1), C6(i+2), S6(i+2));
			FA7: FullAdder port map (S6(i+4), P7(i), C7(i+1), C7(i+2), S7(i+2));
		end generate Bit0_15;
		Bit16: if i=16 generate
			FA2: FullAdder port map (C1(i+3), E(i-14), C2(i+1), C2(i+2), S2(i+2));
			FA3: FullAdder port map (C2(i+3), E(i-13), C3(i+1), C3(i+2), S3(i+2));
			FA4: FullAdder port map (C3(i+3), E(i-12), C4(i+1), C4(i+2), S4(i+2));
			FA5: FullAdder port map (C4(i+3), E(i-11), C5(i+1), C5(i+2), S5(i+2));
			FA6: FullAdder port map (C5(i+3), E(i-10), C6(i+1), C6(i+2), S6(i+2));
			FA7: FullAdder port map (C6(i+3), E(i-9), C7(i+1), C7(i+2), S7(i+2));
		end generate Bit16;
	end generate FullAdd27;
	
	--generate 3x7 copies of the HalfAdder HA1, HA2, HA3, HA4, HA5, HA6, HA7
	HalfAdd27: for i in 0 to 2 generate
	begin
		std_logic0: if i=0 generate
			HA1: HalfAdder port map (P0(i), neg(i), C1(i), S1(i));
			HA2: HalfAdder port map (S1(i+2), neg(i+1), C2(i), S2(i));
			HA3: HalfAdder port map (S2(i+2), neg(i+2), C3(i), S3(i));			
			HA4: HalfAdder port map (S3(i+2), neg(i+3), C4(i), S4(i));
			HA5: HalfAdder port map (S4(i+2), neg(i+4), C5(i), S5(i));
			HA6: HalfAdder port map (S5(i+2), neg(i+5), C6(i), S6(i));
			HA7: HalfAdder port map (S6(i+2), neg(i+6), C7(i), S7(i));
		end generate std_logic0;
		Bit1: if i=1 generate
			HA1: HalfAdder port map (P0(i), C1(i-1), C1(i), S1(i));
			HA2: HalfAdder port map (S1(i+2), C2(i-1), C2(i), S2(i));
			HA3: HalfAdder port map (S2(i+2), C3(i-1), C3(i), S3(i));
			HA4: HalfAdder port map (S3(i+2), C4(i-1), C4(i), S4(i));
			HA5: HalfAdder port map (S4(i+2), C5(i-1), C5(i), S5(i));
			HA6: HalfAdder port map (S5(i+2), C6(i-1), C6(i), S6(i));
			HA7: HalfAdder port map (S6(i+2), C7(i-1), C7(i), S7(i));
		end generate Bit1;
		Bit2: if i=2 generate
			HA1: HalfAdder port map ('1', C1(i+16), C1(i+17), S1(i+17));
			HA2: HalfAdder port map ('1', C2(i+16), C2(i+17), S2(i+17));
			HA3: HalfAdder port map ('1', C3(i+16), C3(i+17), S3(i+17));
			HA4: HalfAdder port map ('1', C4(i+16), C4(i+17), S4(i+17));
			HA5: HalfAdder port map ('1', C5(i+16), C5(i+17), S5(i+17));
			HA6: HalfAdder port map ('1', C6(i+16), C6(i+17), S6(i+17));
			HA7: HalfAdder port map ('1', C7(i+16), C7(i+17), S7(i+17));
		end generate Bit2;
	end generate HalfAdd27;

	P(0) <= S1(0); P(1) <= S1(1); P(2) <= S2(0); P(3) <= S2(1); 
	P(4) <= S3(0); P(5) <= S3(1); P(6) <= S4(0); P(7) <= S4(1); 
	P(8) <= S5(0); P(9) <= S5(1); P(10) <= S6(0); P(11) <= S6(1);
	P(12) <= S7(0); P(13) <= S7(1); 

	--generate 18 copies of the HalfAdder HA8
	HalfAdd8: for i in 0 to 17 generate
	begin
		Bit0: if i=0 generate
			HA8: HalfAdder port map (S7(i+2), neg(i+7), C8(i), P(i+14));
		end generate Bit0;
		Bit1_17: if i>0 generate
			HA8: HalfAdder port map (S7(i+2), C8(i-1), C8(i), P(i+14));
		end generate Bit1_17;
	end generate HalfAdd8;

end Behavioral;
