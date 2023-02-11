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
	printf("show\t-- print the current content of the pipeline registers\n");
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
	handle_pipeline();
	CURRENT_STATE = NEXT_STATE;
	CYCLE_COUNT++;
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
	printf("# Cycles Executed\t: %u\n", CYCLE_COUNT);
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
			if (buffer[1] == 'h' || buffer[1] == 'H'){
				show_pipeline();
			}else {
				runAll(); 
			}
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
/* maintain the pipeline                                                                                           */ 
/************************************************************/
void handle_pipeline()
{
	/*INSTRUCTION_COUNT should be incremented when instruction is done*/
	/*Since we do not have branch/jump instructions, INSTRUCTION_COUNT should be incremented in WB stage */

	
	if(CYCLE_COUNT >= 4){	
	WB();	
	}
	if(CYCLE_COUNT >= 3){
	MEM();
	}
	if(CYCLE_COUNT >= 2){
	EX();
	}
	if(CYCLE_COUNT >= 1){
	ID();
	}
	IF();

}

/************************************************************/
/* writeback (WB) pipeline stage:                                                                          */ 
/************************************************************/
void WB()
{
	uint32_t opcode, function, rd, rt;
	opcode = (MEM_WB.IR & 0xfc000000) >> 26;
	function = (MEM_WB.IR & 0x3f);

	
	rt = (MEM_WB.IR & 0x001f0000) >> 16;

	rd = (MEM_WB.IR & 0x0000f800) >> 11;

	if(opcode == 0x00){
		switch(function){
			case 0x20: //ADD
			case 0x21: //ADDU
			case 0x22: //SUB
			case 0x23: //SUBU
			case 0x18: //MULT
			case 0x19: //MULTU
			case 0x1A: //DIV
			case 0x1B: //DIVU
			case 0x14: //AND
			case 0x25: //OR
			case 0x26: //XOR
			case 0x27: //NOR
			case 0x2A: //SLT
			case 0x00: //SLL
			case 0x02: //SRL
			case 0x03: //SRA
				NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
				break;
			case 0x10: //MFHI
			case 0x12: //MFLO
				NEXT_STATE.REGS[rt] = MEM_WB.LMD;
				break;
			case 0x0C: //SYSCALL
				RUN_FLAG = FALSE;
				break;
		}
	}
	else{
		switch(opcode){
			case 0x08: //ADDI
			case 0x09: //ADDIU
			case 0x0C: //ANDI
			case 0x0D: //ORI
			case 0x0E: //XORI
			case 0x0A: //SLTI
				NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
				break;
			case 0x23: //LW
			case 0x20: //LB
			case 0x21: //LH
			case 0x0f: //LUI
				NEXT_STATE.REGS[rt] = MEM_WB.LMD;
				break;
		}

	}

	INSTRUCTION_COUNT++;	

	/*
	if(reg-reg)
	REGS[MEM/WB.IR] = MEM/WB.ALUOutput;

	if(reg-imm)
	REGS[MEM/WB.IR] = MEM/WB.ALUOutput;

	if(load)
	REGS[MEM/WB.IR] = MEM/WB.LMD;
	*/
}

/************************************************************/
/* memory access (MEM) pipeline stage:                                                          */ 
/************************************************************/
void MEM()
{

	uint32_t opcode, function;

	opcode = (EX_MEM.IR & 0xfc000000) >> 26;
	function = (EX_MEM.IR & 0x3f);

	MEM_WB.IR = EX_MEM.IR;


	if(opcode == 0x00){
		switch(function){
			case 0x20: //ADD
			case 0x21: //ADDU
			case 0x22: //SUB
			case 0x23: //SUBU
			case 0x18: //MULT
			case 0x19: //MULTU
			case 0x1A: //DIV
			case 0x1B: //DIVU
			case 0x14: //AND
			case 0x25: //OR
			case 0x26: //XOR
			case 0x27: //NOR
			case 0x2A: //SLT
			case 0x00: //SLL
			case 0x02: //SRL
			case 0x03: //SRA
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
			case 0x10: //MFHI
			case 0x12: //MFLO
				MEM_WB.LMD = mem_read_32(EX_MEM.ALUOutput);
				break;
			case 0x11: //MTHI
			case 0x13: //MTLO
				mem_write_32(EX_MEM.ALUOutput, EX_MEM.B);
				break;
		}
	}
	else{
		switch(opcode){
			case 0x08: //ADDI
			case 0x09: //ADDIU
			case 0x0C: //ANDI
			case 0x0D: //ORI
			case 0x0E: //XORI
			case 0x0A: //SLTI
				MEM_WB.ALUOutput = EX_MEM.ALUOutput;
				break;
 			case 0x23: //LW
				MEM_WB.LMD = mem_read_32(EX_MEM.ALUOutput);
				//printf("MEM_WB.LMD after lw = %u\n", MEM_WB.LMD);
				break;
			case 0x20: //LB
				MEM_WB.LMD = mem_read_32(EX_MEM.ALUOutput) & 0x000000ff;
				break;
			case 0x21: //LH
				MEM_WB.LMD = mem_read_32(EX_MEM.ALUOutput) & 0x0000ffff;
				break;
			case 0x0f: //LUI
				MEM_WB.LMD = EX_MEM.ALUOutput << 16;
				//printf("MEM_WB.LMD after lui = %u\n", MEM_WB.LMD);
				//printf("EX_MEM.ALUOutput = %u\n", EX_MEM.ALUOutput);
				break;
			case 0x2B: //SW
				mem_write_32(EX_MEM.ALUOutput, EX_MEM.B);
				//printf("sw writing %u into %u\n", EX_MEM.B, EX_MEM.ALUOutput);
				break;
			case 0x28: //SB
				mem_write_32(EX_MEM.ALUOutput, EX_MEM.B & 0x000000ff);
				//printf("sb writing %u into %u\n", EX_MEM.B, EX_MEM.ALUOutput);
				break;
			case 0x29: //SH
				mem_write_32(EX_MEM.ALUOutput, EX_MEM.B & 0x0000ffff);
				//printf("sh writing %u into %u\n", EX_MEM.B, EX_MEM.ALUOutput);
				break;
		}
	}
	
}

/************************************************************/
/* execution (EX) pipeline stage:                                                                          */ 
/************************************************************/
void EX()
{
	uint32_t opcode, function, tmpA, tmpB, sa;
	uint64_t product, p1, p2;

	opcode = (ID_EX.IR & 0xfc000000) >> 26;
	function = (ID_EX.IR & 0x3f);
	sa = (ID_EX.IR & 0x000007c0) >> 6;

	tmpA = ID_EX.A;
	tmpB = ID_EX.B;

	EX_MEM.IR = ID_EX.IR;

	if(opcode == 0){
		switch(function){
			case 0x20: //ADD
				EX_MEM.ALUOutput = tmpA + tmpB;
				break;
			case 0x21:
				EX_MEM.ALUOutput = tmpA + tmpB;
				break;
			case 0x22: //SUB
				EX_MEM.ALUOutput = tmpA - tmpB;
				break;
			case 0x23:
				EX_MEM.ALUOutput = tmpA - tmpB;
				break;
			case 0x18: //MULT
				p1 = (uint64_t)tmpA;
				p2 = (uint64_t)tmpB;

				product = p1 * p2;

				NEXT_STATE.LO = (uint32_t)product & 0xffffffff;
				NEXT_STATE.HI = (uint32_t)((product & 0xffffffff00000000) >> 32);
				break;
			case 0x19:
				p1 = (uint64_t)(0x00000000 | tmpA);
				p2 = (uint64_t)(0x00000000 | tmpB);

				product = p1 * p2;

				NEXT_STATE.LO = (uint32_t)(product & 0xffffffff);
				NEXT_STATE.HI = (uint32_t)(product & 0xffffffff00000000);
				break;
			case 0x1A: //DIV
				NEXT_STATE.LO = tmpA / tmpB;
				NEXT_STATE.HI = tmpA % tmpB;
				break;
			case 0x1B:
				NEXT_STATE.LO = (0x00000000 | tmpA) / (0x00000000 | tmpB);
				NEXT_STATE.HI = (0x00000000 | tmpA) % (0x00000000 | tmpB);
				break;
			case 0x14: //AND
				EX_MEM.ALUOutput = tmpA & tmpB;
				break;
			case 0x25: //OR
				EX_MEM.ALUOutput = tmpA | tmpB;
				break;
			case 0x26: //XOR
				EX_MEM.ALUOutput = tmpA ^ tmpB;
				break;
			case 0x27: //NOR
				EX_MEM.ALUOutput = ~(tmpA | tmpB);
				break;
			case 0x2A: //SLT
				if(tmpA < tmpB) EX_MEM.ALUOutput = 0x00000001;
				else EX_MEM.ALUOutput = 0x00000000;
				break;
			case 0x00: //SLL
				EX_MEM.ALUOutput = tmpB << sa;
				break;
			case 0x02: //SRL
				EX_MEM.ALUOutput = tmpB >> sa;
				break;
			case 0x03: //SRA
				if((tmpB & 0x10) == 0x10){
					for(int i = 0; i < sa; ++i){
						tmpB = tmpB >> 1;
						tmpB = tmpB | 0x10;
					}
				}
				else{
					for(int i = 0; i < sa; ++i){
						tmpB = tmpB >> 1;
					}
				}
				EX_MEM.ALUOutput = tmpB;
				break;
			case 0x10: //MFHI
			case 0x12: //FLO
			case 0x11: //MTHI
			case 0x13: //MTLO
				EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
				EX_MEM.B = ID_EX.B;
				break;
		}
	}
	
	else{
		switch(opcode){
				case 0x08: //ADDI
					EX_MEM.ALUOutput = tmpA + ID_EX.imm;
					break;
				case 0x09:
					EX_MEM.ALUOutput = tmpA + ID_EX.imm;
					break;
				case 0x0C: //ANDI
					EX_MEM.ALUOutput = (ID_EX.imm & 0x0000ffff) & tmpA;
					break;
				case 0x0D: //ORI
					EX_MEM.ALUOutput = ((tmpA & 0xffff0000) | ((ID_EX.imm | (tmpA & 0xffff))));
					break;
				case 0x0E: //XORI
					EX_MEM.ALUOutput = tmpA ^ (ID_EX.imm & 0x0000ffff);
					break;
				case 0x0A: //SLTI
					if(tmpA < ID_EX.imm) EX_MEM.ALUOutput = 0x0001;
					else EX_MEM.ALUOutput = 0x0000;
					break;
				case 0x23: //LW
				case 0x20: //LB
				case 0x21: //LH
				case 0x0F: //LUI
				case 0x2B: //SW
				case 0x28: //SB
				case 0x29: //SH
					EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
					//printf("\nID_EX.A is %u and ID_EX.imm is %u\n", ID_EX.A, ID_EX.imm);
					//printf("the value at EX_MEM.ALUOutput is %u\n", mem_read_32(EX_MEM.ALUOutput));
					EX_MEM.B = ID_EX.B;
					break;

			}
	}
}

/************************************************************/
/* instruction decode (ID) pipeline stage:                                                         */ 
/************************************************************/
void ID()
{
	ID_EX.IR = IF_ID.IR;
	
	uint32_t rs = (IF_ID.IR & 0x03E00000) >> 21;
	ID_EX.A = CURRENT_STATE.REGS[rs];
	//printf("rs in ID is %u\n", rs);
	//printf("ID_EX.A in ID is %u\n", ID_EX.A);
	
	uint32_t rt = (IF_ID.IR & 0x001F0000) >> 16;
	ID_EX.B = CURRENT_STATE.REGS[rt];
	
	
	ID_EX.imm = IF_ID.IR & 0x0000FFFF;
	
	if(ID_EX.imm & 8000){
		ID_EX.imm = ID_EX.imm | 0xFFFF0000;
	}
}

/************************************************************/
/* instruction fetch (IF) pipeline stage:                                                              */ 
/************************************************************/
void IF()
{
	IF_ID.IR = mem_read_32(CURRENT_STATE.PC);
	IF_ID.PC = CURRENT_STATE.PC + 4;

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

void print_instruction(uint32_t addr){
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
    char reg_name[32][6] = {"$zero", "$at", "$v0", "$v1", "$a0","$a1", "$a2", "$a3", "$t0","$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
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
	printf("sll %s %s %x", reg_name[rd], reg_name[rt], sa);
            break;
        case(0x02): //SRL
            printf("srl %s %s %x", reg_name[rd], reg_name[rt], sa);
            break;
        case(0x03): //SRA
            printf("sra %s %s %x", reg_name[rd], reg_name[rt], sa);
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
            printf("addi %s %s %x", reg_name[rt], reg_name[rs], immediate);
            break;
        case(0x09): //ADDIU
           printf("addiu %s %s %x", reg_name[rs], reg_name[rt], immediate);
            break;
        case(0x0C): //ANDI
            printf("andi %s %s %x", reg_name[rt], reg_name[rs], immediate);
            break;
        case(0x0D): //ORI
            printf("ori %s %s %x", reg_name[rt], reg_name[rs], immediate);
            break;
        case(0x0E): //XORI
            printf("xori %s %s %x", reg_name[rt], reg_name[rs], immediate);
            break;
        case(0x0A): //SLTI
            printf("slti %s %s %x", reg_name[rt], reg_name[rs], immediate);
            break;
        case(0x23): //LW-
            printf("lw %s %s(%x)", reg_name[rt], reg_name[rs], immediate);
            break;
        case(0x20): //LB-
            printf("lb %s %s(%x)", reg_name[rt], reg_name[rs], immediate);
            break;
        case(0x21): //LH-
            printf("lh %s %s(%x)", reg_name[rt], reg_name[rs], immediate);
	break;
        case(0x0F): //LUI-
            printf("lui %s %x", reg_name[rt], immediate);
            break;
        case(0x2B): //SW-
            printf("sw %s %s(%x)", reg_name[rt], reg_name[rs], immediate);
            break;
        case(0x28): //SB-
            printf("sb %s %s(%x)", reg_name[rt], reg_name[rs], immediate);
            break;
        case(0x29): //SH-
            printf("sh %s %s(%x)", reg_name[rt], reg_name[rs], immediate);
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
        case(0x04): //BEQ-
            printf("beq %s %s %x", reg_name[rs], reg_name[rt], immediate);
            break;
        case(0x05): //BNE-
            printf("bne %s %s %x", reg_name[rs], reg_name[rt], immediate);
            break;
        case(0x06): //BLEZ-
            printf("blez %s %x", reg_name[rs], immediate);
            break;
        case(0x01): //BLTZ and BGEZ- have same opcode, rt section is different
            if(rt == 0){ //BLTZ
                printf("bltz %s %x", reg_name[rs], immediate);
            }
            else{ //BGEZ rt == 1
                printf("bgez %s %x", reg_name[rs], immediate);
            }
            break;
        case(0x07): //BGTZ-
            printf("bgtz %s %x", reg_name[rs], immediate);
            break;
        case(0x02): //J-
            printf("j %x", target);
            break;
        case(0x03): //JAL-
            printf("jal %x", target);
            break;
    }//end switch
}


printf("\n");






}
/************************************************************/
/* Print the current pipeline                                                                                    */ 
/************************************************************/
void show_pipeline(){
	
	printf("Current PC: %08x\n", CURRENT_STATE.PC); 
	printf("IF/ID.IR: %08x\n", IF_ID.IR);
	printf("instruction: "); print_instruction(CURRENT_STATE.PC);
	printf("IF/ID.PC: %08x\n", IF_ID.PC);//notice that it contains the next PC
	printf("ID/EX.IR: %08x ", ID_EX.IR); print_instruction(CURRENT_STATE.PC - 4);
	printf("ID/EX.A: %02x\n", ID_EX.A);
	printf("ID/EX.B: %02x\n", ID_EX.B);
	printf("ID/EX.imm: %04x\n", ID_EX.imm);
	printf("EX/MEM.IR: %08x\n", EX_MEM.IR);
	printf("EX/MEM.A: %02x\n", EX_MEM.A);
	printf("EX/MEM.B: %02x\n", EX_MEM.B);
	printf("EX/MEM.ALUOutput: %08x\n", EX_MEM.ALUOutput);
	printf("MEM/WB.IR: %08x\n", MEM_WB.IR);
	printf("MEM/WB.ALUOutput: %08x\n", MEM_WB.ALUOutput);
	printf("MEM/WB.LMD: %08x\n", MEM_WB.LMD);

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
