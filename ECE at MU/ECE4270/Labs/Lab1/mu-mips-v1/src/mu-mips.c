#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "mu-mips.h"

/***************************************************************/
/* Print out a list of commands available                                                                  */
/***************************************************************/
void help() {        
	printf("------------------------------------------------------------------\n\n");
	printf("\t**********MU-MIPS Help MENU**********\n\n");
	printf("sim\t-- simulate program to completion \n");
	printf("run <n>\t-- simulate program for <n> instructions\n");
	printf("rdump\t-- dump register values\n");
	printf("reset\t-- clears all registers/memory and re-loads the program\n");
	printf("input <reg> <val>\t-- set GPR <reg> to <val>\n");
	printf("mdump <start> <stop>\t-- dump memory from <start> to <stop> address\n");
	printf("high <val>\t-- set the HI register to <val>\n");
	printf("low <val>\t-- set the LO register to <val>\n");
	printf("print\t-- print the program loaded into memory\n");
	printf("?\t-- display help menu\n");
	printf("quit\t-- exit the simulator\n\n");
	printf("------------------------------------------------------------------\n\n");
}

/***************************************************************/
/* Read a 32-bit word from memory                                                                            */
/***************************************************************/
uint32_t mem_read_32(uint32_t address)
{
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) &&  ( address <= MEM_REGIONS[i].end) ) {
			uint32_t offset = address - MEM_REGIONS[i].begin;
			return (MEM_REGIONS[i].mem[offset+3] << 24) |
					(MEM_REGIONS[i].mem[offset+2] << 16) |
					(MEM_REGIONS[i].mem[offset+1] <<  8) |
					(MEM_REGIONS[i].mem[offset+0] <<  0);
		}
	}
	return 0;
}

/***************************************************************/
/* Write a 32-bit word to memory                                                                                */
/***************************************************************/
void mem_write_32(uint32_t address, uint32_t value)
{
	int i;
	uint32_t offset;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) && (address <= MEM_REGIONS[i].end) ) {
			offset = address - MEM_REGIONS[i].begin;

			MEM_REGIONS[i].mem[offset+3] = (value >> 24) & 0xFF;
			MEM_REGIONS[i].mem[offset+2] = (value >> 16) & 0xFF;
			MEM_REGIONS[i].mem[offset+1] = (value >>  8) & 0xFF;
			MEM_REGIONS[i].mem[offset+0] = (value >>  0) & 0xFF;
		}
	}
}

/***************************************************************/
/* Execute one cycle                                                                                                              */
/***************************************************************/
void cycle() {                                                
	handle_instruction();
	CURRENT_STATE = NEXT_STATE;
	INSTRUCTION_COUNT++;
}

/***************************************************************/
/* Simulate MIPS for n cycles                                                                                       */
/***************************************************************/
void run(int num_cycles) {                                      
	
	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped\n\n");
		return;
	}

	printf("Running simulator for %d cycles...\n\n", num_cycles);
	int i;
	for (i = 0; i < num_cycles; i++) {
		if (RUN_FLAG == FALSE) {
			printf("Simulation Stopped.\n\n");
			break;
		}
		cycle();
	}
}

/***************************************************************/
/* simulate to completion                                                                                               */
/***************************************************************/
void runAll() {                                                     
	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped.\n\n");
		return;
	}

	printf("Simulation Started...\n\n");
	while (RUN_FLAG){
		cycle();
	}
	printf("Simulation Finished.\n\n");
}

/***************************************************************/ 
/* Dump a word-aligned region of memory to the terminal                              */
/***************************************************************/
void mdump(uint32_t start, uint32_t stop) {          
	uint32_t address;

	printf("-------------------------------------------------------------\n");
	printf("Memory content [0x%08x..0x%08x] :\n", start, stop);
	printf("-------------------------------------------------------------\n");
	printf("\t[Address in Hex (Dec) ]\t[Value]\n");
	for (address = start; address <= stop; address += 4){
		printf("\t0x%08x (%d) :\t0x%08x\n", address, address, mem_read_32(address));
	}
	printf("\n");
}

/***************************************************************/
/* Dump current values of registers to the teminal                                              */   
/***************************************************************/
void rdump() {                               
	int i; 
	printf("-------------------------------------\n");
	printf("Dumping Register Content\n");
	printf("-------------------------------------\n");
	printf("# Instructions Executed\t: %u\n", INSTRUCTION_COUNT);
	printf("PC\t: 0x%08x\n", CURRENT_STATE.PC);
	printf("-------------------------------------\n");
	printf("[Register]\t[Value]\n");
	printf("-------------------------------------\n");
	for (i = 0; i < MIPS_REGS; i++){
		printf("[R%d]\t: 0x%08x\n", i, CURRENT_STATE.REGS[i]);
	}
	printf("-------------------------------------\n");
	printf("[HI]\t: 0x%08x\n", CURRENT_STATE.HI);
	printf("[LO]\t: 0x%08x\n", CURRENT_STATE.LO);
	printf("-------------------------------------\n");
}

/***************************************************************/
/* Read a command from standard input.                                                               */  
/***************************************************************/
void handle_command() {                         
	char buffer[20];
	uint32_t start, stop, cycles;
	uint32_t register_no;
	int register_value;
	int hi_reg_value, lo_reg_value;

	printf("MU-MIPS SIM:> ");

	if (scanf("%s", buffer) == EOF){
		exit(0);
	}

	switch(buffer[0]) {
		case 'S':
		case 's':
			runAll(); 
			break;
		case 'M':
		case 'm':
			if (scanf("%x %x", &start, &stop) != 2){
				break;
			}
			mdump(start, stop);
			break;
		case '?':
			help();
			break;
		case 'Q':
		case 'q':
			printf("**************************\n");
			printf("Exiting MU-MIPS! Good Bye...\n");
			printf("**************************\n");
			exit(0);
		case 'R':
		case 'r':
			if (buffer[1] == 'd' || buffer[1] == 'D'){
				rdump();
			}else if(buffer[1] == 'e' || buffer[1] == 'E'){
				reset();
			}
			else {
				if (scanf("%d", &cycles) != 1) {
					break;
				}
				run(cycles);
			}
			break;
		case 'I':
		case 'i':
			if (scanf("%u %i", &register_no, &register_value) != 2){
				break;
			}
			CURRENT_STATE.REGS[register_no] = register_value;
			NEXT_STATE.REGS[register_no] = register_value;
			break;
		case 'H':
		case 'h':
			if (scanf("%i", &hi_reg_value) != 1){
				break;
			}
			CURRENT_STATE.HI = hi_reg_value; 
			NEXT_STATE.HI = hi_reg_value; 
			break;
		case 'L':
		case 'l':
			if (scanf("%i", &lo_reg_value) != 1){
				break;
			}
			CURRENT_STATE.LO = lo_reg_value;
			NEXT_STATE.LO = lo_reg_value;
			break;
		case 'P':
		case 'p':
			print_program(); 
			break;
		default:
			printf("Invalid Command.\n");
			break;
	}
}

/***************************************************************/
/* reset registers/memory and reload program                                                    */
/***************************************************************/
void reset() {   
	int i;
	/*reset registers*/
	for (i = 0; i < MIPS_REGS; i++){
		CURRENT_STATE.REGS[i] = 0;
	}
	CURRENT_STATE.HI = 0;
	CURRENT_STATE.LO = 0;
	
	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}
	
	/*load program*/
	load_program();
	
	/*reset PC*/
	INSTRUCTION_COUNT = 0;
	CURRENT_STATE.PC =  MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	RUN_FLAG = TRUE;
}

/***************************************************************/
/* Allocate and set memory to zero                                                                            */
/***************************************************************/
void init_memory() {                                           
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		MEM_REGIONS[i].mem = malloc(region_size);
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}
}

/**************************************************************/
/* load program into memory                                                                                      */
/**************************************************************/
void load_program() {                   
	FILE * fp;
	int i, word;
	uint32_t address;

	/* Open program file. */
	fp = fopen(prog_file, "r");
	if (fp == NULL) {
		printf("Error: Can't open program file %s\n", prog_file);
		exit(-1);
	}

	/* Read in the program. */

	i = 0;
	while( fscanf(fp, "%x\n", &word) != EOF ) {
		address = MEM_TEXT_BEGIN + i;
		mem_write_32(address, word);
		printf("writing 0x%08x into address 0x%08x (%d)\n", word, address, address);
		i += 4;
	}
	PROGRAM_SIZE = i/4;
	printf("Program loaded into memory.\n%d words written into memory.\n\n", PROGRAM_SIZE);
	fclose(fp);
}

/************************************************************/
/* decode and execute instruction                                                                     */ 
/************************************************************/
void handle_instruction()
{
	/*IMPLEMENT THIS*/
	/* execute one instruction at a time. Use/update CURRENT_STATE and and NEXT_STATE, as necessary.*/

	uint32_t instruction, opcode, function, rs, rt, rd, sa, immediate, target;
	uint64_t product, p1, p2;
	uint32_t temp;
	
	printf("[0x%x]\t", CURRENT_STATE.PC);

	instruction = mem_read_32(CURRENT_STATE.PC);
	//printf("instruction:0x%x \n", instruction);

	opcode = (instruction & 0xfc000000) >> 26;
	function = (instruction & 0x3f);
	rs = (instruction & 0x03e00000) >> 21;
	rt = (instruction & 0x001f0000) >> 16;
	rd = (instruction & 0x0000f800) >> 11;
	sa = (instruction & 0x000007c0) >> 6;
	immediate = (instruction & 0x0000ffff);
	target = (instruction & 0x03ffffff);
	

	if(opcode == 0x00){
		switch(function){
			case 0x20: // ADD
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x21: // ADDU
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x22: //SUB
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x23: //SUBU
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x18: //MULT

				p1 = (uint64_t)CURRENT_STATE.REGS[rs];
				p2 = (uint64_t)CURRENT_STATE.REGS[rt];

				product = p1 * p2;

				NEXT_STATE.LO = (uint32_t)(product & 0xffffffff);
				NEXT_STATE.HI = (uint32_t)((product & 0xffffffff00000000) >> 32);
				print_instruction(CURRENT_STATE.PC);
				break;

			case 0x19: //MULTU
				p1 = (uint64_t)(0x00000000 | CURRENT_STATE.REGS[rs]);
				p2 = (uint64_t)(0x00000000 | CURRENT_STATE.REGS[rt]);

				product = p1 * p2;

				NEXT_STATE.LO = (uint32_t)(product & 0xffffffff);
				NEXT_STATE.HI = (uint32_t)((product & 0xffffffff00000000) >> 32);
				print_instruction(CURRENT_STATE.PC);
				break;

			case 0x1A: //DIV
				NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
				NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rs];
				print_instruction(CURRENT_STATE.PC);
				break;

			case 0x1B: //DIVU
				NEXT_STATE.LO = (0x00000000 | CURRENT_STATE.REGS[rs]) / (0x00000000 | CURRENT_STATE.REGS[rt]);
				NEXT_STATE.HI = (0x00000000 | CURRENT_STATE.REGS[rs]) % (0x00000000 | CURRENT_STATE.REGS[rt]);
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x14: //AND
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x25: //OR
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break; 
			case 0x26: //XOR
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x27: //NOR
				NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
				print_instruction(CURRENT_STATE.PC);
				break;

			case 0x2A: //SLT
				if(CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt]) NEXT_STATE.REGS[rd] = 0x00000001;
				else NEXT_STATE.REGS[rd] = 0x00000000;
				print_instruction(CURRENT_STATE.PC);
				break;

			case 0x00: //SLL
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << sa;
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x02: //SRL
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> sa;
				print_instruction(CURRENT_STATE.PC);
				break;

			case 0x03: //SRA
				if((CURRENT_STATE.REGS[rt] & 0x10) == 0x10){
					for(int i = 0; i < sa; ++i){
						CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rt] >> 1;
						CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rt] | 0x10; 
					}
				}

				else{
					for(int i = 0; i < sa; ++i){
						CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rt] >> 1;
					}	
				}
				NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt];
				print_instruction(CURRENT_STATE.PC);
				break;
			case(0x10): //MFHI-
				NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
				print_instruction(CURRENT_STATE.PC);
				break;         
			case(0x12): //MFLO-
				NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
				print_instruction(CURRENT_STATE.PC);
				break;     
			case(0x11): //MTHI-
				NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
				print_instruction(CURRENT_STATE.PC);
				break;     
			case(0x13): //MTLO-
				NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
				print_instruction(CURRENT_STATE.PC);
				break;     	
			case(0x08): //JR-
				print_instruction(CURRENT_STATE.PC);
				CURRENT_STATE.PC = CURRENT_STATE.REGS[rs] - 4;
				break;    
		
			case(0x09): //JALR-
				if( rd == 0x0){
					temp = CURRENT_STATE.REGS[rs];
					NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
					CURRENT_STATE.PC = temp - 4;
				}
				else{
					temp = CURRENT_STATE.REGS[rs];
					NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 4;
					CURRENT_STATE.PC = temp - 4;
				}
				print_instruction(CURRENT_STATE.PC);     
				break; 
            
			case 0x0C: //SYSCALL
				//if(CURRENT_STATE.REGS[2] == 0xa){
					RUN_FLAG = FALSE;
				print_instruction(CURRENT_STATE.PC);
				//}
				break;	
		}
	}

	else{
		switch(opcode){
			case 0x08: // ADDI
				if ((immediate & 0x8000) == 0x8000) immediate = immediate | 0xffff0000;
				NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + immediate;
				print_instruction(CURRENT_STATE.PC);
				break;

			case 0x09: // ADDIU
				if ((immediate & 0x8000) == 0x8000) immediate = immediate | 0xffff0000;
				NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + immediate;
				print_instruction(CURRENT_STATE.PC);
				break;
			case 0x0C: //ANDI
				temp = immediate & 0x0000FFFF;
				NEXT_STATE.REGS[rt] = (temp & CURRENT_STATE.REGS[rs]);
				print_instruction(CURRENT_STATE.PC);
				break;

			case 0x0D: //ORI

				NEXT_STATE.REGS[rt] = ((CURRENT_STATE.REGS[rs] & 0xffff0000) | ((immediate | (CURRENT_STATE.REGS[rs] & 0xffff))));
				print_instruction(CURRENT_STATE.PC);
				break;

			case 0x0E: //XORI
				temp = immediate & 0x0000FFFF;
				NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ temp;
				print_instruction(CURRENT_STATE.PC);
				break;

			case 0x0A: //SLTI
				if ((immediate & 0x8000) == 0x8000) immediate = immediate | 0xffff0000;

				if(CURRENT_STATE.REGS[rs] < immediate) NEXT_STATE.REGS[rt] = 0x0001;
				else NEXT_STATE.REGS[rt] = 0x0000;
				print_instruction(CURRENT_STATE.PC);
				break;
				
            case(0x23): //LW- load word
				if ((immediate & 0x8000) == 0x8000) immediate = immediate | 0xffff0000;
				NEXT_STATE.REGS[rt] = mem_read_32(CURRENT_STATE.REGS[rs] + immediate);
				print_instruction(CURRENT_STATE.PC);
				break;            
			case(0x20): //LB- load byte
				if ((immediate & 0x8000) == 0x8000) immediate = immediate | 0xffff0000;
				NEXT_STATE.REGS[rt] = mem_read_32(CURRENT_STATE.REGS[rs] + immediate) & 0x000000FF;
				print_instruction(CURRENT_STATE.PC);
				break;            
			case(0x21): //LH- load half-word
				if ((immediate & 0x8000) == 0x8000) immediate = immediate | 0xffff0000;

				temp = mem_read_32(CURRENT_STATE.REGS[rs] + immediate) & 0x0000FFFF;
				if((temp & 0x8000) == 0x8000) temp = temp | 0xffff0000;
				NEXT_STATE.REGS[rt] = temp;
				print_instruction(CURRENT_STATE.PC);
				break;            
			case(0x0F): //LUI-
				NEXT_STATE.REGS[rt] = ((immediate << 16) & 0xFFFF0000);
				print_instruction(CURRENT_STATE.PC);
				break;            
			case(0x2B): //SW- store word
				if ((immediate & 0x8000) == 0x8000) immediate = immediate | 0xffff0000;
				mem_write_32(CURRENT_STATE.REGS[rs] + immediate, CURRENT_STATE.REGS[rt]);
				print_instruction(CURRENT_STATE.PC);
				break;     
			case(0x28): //SB- store byte
				if ((immediate & 0x8000) == 0x8000) immediate = immediate | 0xffff0000;
				mem_write_32(CURRENT_STATE.REGS[rs] + immediate, CURRENT_STATE.REGS[rt] & 0x000000FF);
				print_instruction(CURRENT_STATE.PC);
				break;
			case(0x29): //SH- store half-word
				if ((immediate & 0x8000) == 0x8000) immediate = immediate | 0xffff0000;
				mem_write_32(CURRENT_STATE.REGS[rs] + immediate, CURRENT_STATE.REGS[rt] & 0x0000FFFF);
				print_instruction(CURRENT_STATE.PC);
				break;     
			
			case(0x04): //BEQ-
				if(CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]){
					temp = immediate << 2;
					if((0x00008000 & immediate) == 0x8000){//1000 0000 0000 0000
						temp = temp | 0xFFFC0000;
					}
					print_instruction(CURRENT_STATE.PC);
					CURRENT_STATE.PC = CURRENT_STATE.PC + temp - 4;
				}
				break;    
			case(0x05): //BNE-
				if(CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]){
					temp = immediate << 2;
					if((0x00008000 & immediate) == 0x8000){//1000 0000 0000 0000
						temp = temp | 0xFFFC0000;
					}
					print_instruction(CURRENT_STATE.PC);
					CURRENT_STATE.PC = CURRENT_STATE.PC + temp - 4;
				}
				
				break;             
			case(0x06): //BLEZ-
				if(((CURRENT_STATE.REGS[rs] & 0x80000000) == 0x80000000) || (CURRENT_STATE.REGS[rs] == 0)){
					temp = immediate << 2;
					if((0x00008000 & immediate) == 0x8000){//1000 0000 0000 0000
						temp = temp | 0xFFFC0000;
					}
					print_instruction(CURRENT_STATE.PC);
					CURRENT_STATE.PC = CURRENT_STATE.PC + temp - 4;
				}
				break;     
			case(0x01): //BLTZ and BGEZ- have same opcode, rt section is different
				
				if(rt == 0){ //BLTZ rt == 0
					if((CURRENT_STATE.REGS[rs] & 0x80000000) == 0x80000000){
						temp = immediate << 2;
						if((0x00008000 & immediate) == 0x8000){//1000 0000 0000 0000
							temp = temp | 0xFFFC0000;
						}
						print_instruction(CURRENT_STATE.PC);
						CURRENT_STATE.PC = CURRENT_STATE.PC + temp - 4;
					}
				}
				if(rt == 1){ //BGEZ rt == 1
					if((CURRENT_STATE.REGS[rs] & 0x80000000) == 0){
						temp = immediate << 2;
						if((0x00008000 & immediate) == 0x8000){//1000 0000 0000 0000
							temp = temp | 0xFFFC0000;
						}
						print_instruction(CURRENT_STATE.PC);
						CURRENT_STATE.PC = CURRENT_STATE.PC + temp - 4;
					}
				}
				
				break;     
			case(0x07): //BGTZ-
				if(((CURRENT_STATE.REGS[rs] & 0x80000000) == 0) && (CURRENT_STATE.REGS[rs] != 0)){
					if(CURRENT_STATE.REGS[rs] != 0){
						temp = immediate << 2;
						if((0x00008000 & immediate) == 0x8000){//1000 0000 0000 0000
							temp = temp | 0xFFFC0000;
						}
						print_instruction(CURRENT_STATE.PC);
						CURRENT_STATE.PC = CURRENT_STATE.PC + temp - 4;
					}
				}
				break;   
			case(0x02): //J-
				temp = target << 2;
				temp = temp | (CURRENT_STATE.PC & 0xF0000000);
				print_instruction(CURRENT_STATE.PC);
				CURRENT_STATE.PC = temp - 4;
				
				break;   
			case(0x03): //JAL-
				NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
				temp = target << 2;
				temp = temp | (CURRENT_STATE.PC & 0xF0000000);
				print_instruction(CURRENT_STATE.PC);
				CURRENT_STATE.PC = temp - 4;
				
				
				break;
		}
	}
	NEXT_STATE.PC = CURRENT_STATE.PC + 4;
}


/************************************************************/
/* Initialize Memory                                                                                                    */ 
/************************************************************/
void initialize() { 
	init_memory();
	CURRENT_STATE.PC = MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	RUN_FLAG = TRUE;
}

/************************************************************/
/* Print the program loaded into memory (in MIPS assembly format)    */ 
/************************************************************/
void print_program(){
	int i;
	uint32_t addr;
	
	for(i=0; i<PROGRAM_SIZE; i++){
		addr = MEM_TEXT_BEGIN + (i*4);
		printf("[0x%x]\t", addr);
		print_instruction(addr);
	}
}

/************************************************************/
/* Print the instruction at given memory address (in MIPS assembly format)    */
/************************************************************/
void print_instruction(uint32_t addr){
	/*IMPLEMENT THIS*/
	//printf("test");

	//printf("[0x%x]\t", addr);


	uint32_t instruction, opcode, function, rs, rt, rd, sa, immediate, target;


	instruction = mem_read_32(addr);
	//printf("instruction:0x%x \n", instruction);

	opcode = (instruction & 0xfc000000) >> 26;
	function = (instruction & 0x3f);
	rs = (instruction & 0x03e00000) >> 21;
	rt = (instruction & 0x001f0000) >> 16;
	rd = (instruction & 0x0000f800) >> 11;
	sa = (instruction & 0x000007c0) >> 6;
	immediate = (instruction & 0x0000ffff);
	target = (instruction & 0x03ffffff);
    //char reg_name[32][6] = {"$zero", "$at", "$v0", "$v1", "$a0","$a1", "$a2", "$a3", "$t0","$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
	
	char reg_name[32][6] = {"$r0", "$r1", "$r2", "$r3", "$r4","$r5", "$r6", "$r7", "$r8","$r9", "$r10", "$r11", "$r12", "$r13", "$r14", "$r15", "$r16", "$r17", "$r16", "$r19", "$r20", "$r21", "$r22", "$r23", "$r24", "$r25", "$r26", "$r27", "$r28", "$r29", "$r30", "$r31"};
	
	if(opcode == 0){ //R-type Instructions all have 000000 opcode
		switch(function){ //determine function code which is from bit 0-5
			case(0x20): //ADD
				printf("add %s %s %s", reg_name[rd], reg_name[rs], reg_name[rt]);
				break;
			case(0x21): //ADDU
				printf("addu %s %s %s", reg_name[rd], reg_name[rs], reg_name[rt]);
				break;
			case(0x22): //SUB
				printf("sub %s %s %s", reg_name[rd], reg_name[rs], reg_name[rt]);
				break;
			case(0x23): //SUBU
				printf("subu %s %s %s", reg_name[rd], reg_name[rs], reg_name[rt]);
				break;
			case(0x18): //MULT
				printf("mult %s %s", reg_name[rs], reg_name[rt]);
				break;
			case(0x19): //MULTU
				printf("multu %s %s", reg_name[rs], reg_name[rt]);
				break;
			case(0x1A): //DIV
				printf("div %s %s", reg_name[rs], reg_name[rt]);
				break;
			case(0x1B): //DIVU
				printf("divu %s %s", reg_name[rs], reg_name[rt]);
				break;
			case(0x25): //OR
				printf("or %s %s %s", reg_name[rd], reg_name[rs], reg_name[rt]);
				break;
			case(0x24): //AND
				printf("and %s %s %s", reg_name[rd], reg_name[rs], reg_name[rt]);
				break;
			case(0x26): //XOR
				printf("xor %s %s %s", reg_name[rd], reg_name[rs], reg_name[rt]);
				break;
			case(0x27): //NOR
				printf("nor %s %s %s", reg_name[rd], reg_name[rs], reg_name[rt]);
				break;
			case(0x2A): //SLT
				printf("slt %s %s %s", reg_name[rd], reg_name[rs], reg_name[rt]);
				break;
			case(0x00): //SLL
				printf("sll %s %s 0x%x", reg_name[rd], reg_name[rt], sa);
				break;
			case(0x02): //SRL
				printf("srl %s %s 0x%x", reg_name[rd], reg_name[rt], sa);
				break;
			case(0x03): //SRA
				printf("sra %s %s 0x%x", reg_name[rd], reg_name[rt], sa);
				break;
			case(0x10): //MFHI-
				printf("mfhi %s", reg_name[rd]);
				break;         
			case(0x12): //MFLO-
				printf("mflo %s", reg_name[rd]);
				break;     
			case(0x11): //MTHI-
				printf("mthi %s", reg_name[rs]);
				break;     
			case(0x13): //MTLO-
				printf("mtlo %s", reg_name[rs]);
				break;  
			case(0x08): //JR-
				printf("jr %s", reg_name[rs]);
				break;    
		
			case(0x09): //JALR-
				if(rd == 0){
					printf("jalr %s", reg_name[rs]);
				}
				else{
					printf("jalr %s %s ", reg_name[rd], reg_name[rs]);
				}
				break;                 
			case(0x0C): //SYSCALL-
				printf("SYSCALL");
				break;
		}//end switch
	}

	//System Call: SYSCALL (you should implement it to exit the program. To exit the program, the value of 10 (0xA in hex) should be in $v0 when SYSCALL is executed.

	else{ //I-type and J-type instructions have non-zero opcodes
		switch(opcode){
			case(0x08): //ADDI
				printf("addi %s %s 0x%x", reg_name[rt], reg_name[rs], immediate);
				break;
			case(0x09): //ADDIU
			printf("addiu %s %s 0x%x", reg_name[rt], reg_name[rs], immediate);
				break;
			case(0x0C): //ANDI
				printf("andi %s %s 0x%x", reg_name[rt], reg_name[rs], immediate);
				break;    
			case(0x0D): //ORI
				printf("ori %s %s 0x%x", reg_name[rt], reg_name[rs], immediate);
				break;            
			case(0x0E): //XORI
				printf("xori %s %s 0x%x", reg_name[rt], reg_name[rs], immediate);
				break;            
			case(0x0A): //SLTI
				printf("slti %s %s 0x%x", reg_name[rt], reg_name[rs], immediate);
				break;            
			case(0x23): //LW-
				printf("lw %s 0x%x(%s)", reg_name[rt], immediate, reg_name[rs]);
				break;            
			case(0x20): //LB-
				printf("lb %s 0x%x(%s)", reg_name[rt], immediate, reg_name[rs]);
				break;            
			case(0x21): //LH-
				printf("lh %s 0x%x(%s)", reg_name[rt], immediate, reg_name[rs]);
				break;            
			case(0x0F): //LUI-
				printf("lui %s 0x%x", reg_name[rt], immediate);
				break;            
			case(0x2B): //SW-
				printf("sw %s 0x%x(%s)", reg_name[rt], immediate, reg_name[rs]);
				break;     
			case(0x28): //SB-
				printf("sb %s 0x%x(%s)", reg_name[rt], immediate, reg_name[rs]);
				break;
			case(0x29): //SH-
				printf("sh %s 0x%x(%s)", reg_name[rt], immediate, reg_name[rs]);
				break;     
			case(0x04): //BEQ-
				printf("beq %s %s 0x%x", reg_name[rs], reg_name[rt], immediate);
				break;    
			case(0x05): //BNE-
				printf("bne %s %s 0x%x", reg_name[rs], reg_name[rt], immediate);
				break;             
			case(0x06): //BLEZ-
				printf("blez %s 0x%x", reg_name[rs], immediate);
				break;     
			case(0x01): //BLTZ and BGEZ- have same opcode, rt section is different
				if(rt == 0){ //BLTZ
					printf("bltz %s 0x%x", reg_name[rs], immediate);
				}
				else{ //BGEZ rt == 1
					printf("bgez %s 0x%x", reg_name[rs], immediate);
				}
				break;     
			case(0x07): //BGTZ-
				printf("bgtz %s 0x%x", reg_name[rs], immediate);
				break;   
			case(0x02): //J-
				printf("j 0x%x", target);
				break;   
			case(0x03): //JAL-
				printf("jal 0x%x", target);
				break;
		}//end switch
	}
	printf("\n");
}

/***************************************************************/
/* main                                                                                                                                   */
/***************************************************************/
int main(int argc, char *argv[]) {                              
	printf("\n**************************\n");
	printf("Welcome to MU-MIPS SIM...\n");
	printf("**************************\n\n");
	
	if (argc < 2) {
		printf("Error: You should provide input file.\nUsage: %s <input program> \n\n",  argv[0]);
		exit(1);
	}

	strcpy(prog_file, argv[1]);
	initialize();
	load_program();
	help();
	while (1){
		handle_command();
	}
	return 0;
}
