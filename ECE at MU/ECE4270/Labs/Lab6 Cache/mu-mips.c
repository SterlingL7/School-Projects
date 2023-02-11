#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "mu-mips.h"
#include "mu-cache.h"

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
	//show_pipeline();
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
	printf("# cache hits\t: %u\n", cache_hits);
	printf("# cache misses\t: %u\n", cache_misses);
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
				//runAll();
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
			 if(buffer[1] == 'c' || buffer[1] == 'C')
				print_cache();
			else
				print_program();
			break;
		case 'f':
		case 'F':
			if (scanf("%d", &ENABLE_FORWARDING) != 1) {
				
				break;
			}
			ENABLE_FORWARDING == 0 ? printf("Forwarding OFF\n") : printf("Forwarding ON\n");
			//runAll();
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
	cache_misses = 0;
	cache_hits = 0;
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
	printf("\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
	WB();
	MEM();
	EX();
	ID();
	IF();
	
}

/************************************************************/
/* writeback (WB) pipeline stage:                                                                          */ 
/************************************************************/
void WB()
{
	uint32_t rs;
	uint32_t rt;
	uint32_t rd;
	/*IMPLEMENT THIS*/
	printf("\n**************************************Writeback Stage**************************************\n");
	//printf("Instruction register in memory stage=  0x%08x", MEM_WB.IR );
	//Decode the opcode from the instruction
	uint32_t opcode = (0xFC000000 & MEM_WB.IR ); 
	printf("\ninstruction = 0x%08x ",MEM_WB.IR);
	
	//get the address value of rt
	rt = (0x001F0000 & MEM_WB.IR) >> 16;
	printf("\n rt = %d", rt);
	
	//get the address value of rd
	rd = (0x0000F800 & MEM_WB.IR ) >> 11;
	printf("\n rd = %d", rd);
	
	switch (opcode) {
		
	/***********************************************************R-Type************************************************************************/
		case 0x00000000: 
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		//WB_Value=MEM_WB.ALUOutput;
		//NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		//rdd=rd;
		//if (aa==1){
		  // printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		//break;

		//get the address value of rd
		rd = (0x0000F800 & MEM_WB.IR ) >> 11;
		//printf("\n rd = %d", rd);
		function = (0x0000003F & MEM_WB.IR);
		
		switch (function){
		/**************--(JALR-Jump And Link Register)*******************************/
		case 0x00000009:
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;

		/*************--ADD function*******************/
		case 0x00000020:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/***********--ADDU function*******************/
		case 0x00000021:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/***********--SUB function********************/
		case 0x00000022:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/***********--SUBU function******************/
		case 0x00000023:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/************--AND function*****************/
		case 0x00000024:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/**************--OR function****************/
		case 0x00000025:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/****************--XOR function*************/ 
		//(if rs and rt is different, rd = 1 else if rs and rt are same then rd is 0)
		case 0x00000026:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/************NOR function ( !(rs or rt) )***/
		case 0x00000027:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/************SLT function*******************/ 
		//(Set on Less Than, if rs < rt then rd = 1, if rs > rt then rd = 0)
		case 0x0000002A:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/****SLL function (Shift Left Logical)cycle**/
		case 0x00000000:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/*****--SRL function (Shit Right Logical)****/
		case 0x00000002:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/**--SRA function (Shift Right Arithmetic)***/
		case 0x00000003:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/*****MULTU function (Multiply)***************/
		case 0x00000019:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		WB_Value_1=MEM_WB.ALUOutput1;
		NEXT_STATE.HI = MEM_WB.ALUOutput;
		NEXT_STATE.LO = MEM_WB.ALUOutput1;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/******DIVU (Divide unsigned function)****/
		case 0x0000001B:
		//WB_Value - data dependency bet ID and WB, data is transfered to EXE
		WB_Value=MEM_WB.ALUOutput;
		WB_Value_1=MEM_WB.ALUOutput1;
		NEXT_STATE.LO = MEM_WB.ALUOutput;
		NEXT_STATE.HI = MEM_WB.ALUOutput1;
		rdd=rd;
		//if (aa==1){
		   //printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/******MFHI function (Move from HI)****/
		case 0x00000010:
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/******MFLO function (Move from HI)****/
		case 0x00000012:
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/******MTHI function (Move to HI)****/
		case 0x00000011:
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.HI = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break;
		
		/******MTLO function (Move to Low)****/
		case 0x00000013:
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.LO = MEM_WB.ALUOutput;
		rdd=rd;
		if (aa==1){
		   printf("\n REGS[rd] = 0x%08x \n", NEXT_STATE.REGS[rd]);}
		break; 
     
		}
		
		break;
	
	/***********************************************************I-Type************************************************************************/
		case 0x20000000: //For ADDI 
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
		rdd=rt;
		printf("\n REGS[rt] = 0x%08x \n", NEXT_STATE.REGS[rt]);
		break;
		
		case 0x24000000: //For ADDIU 
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
		rdd=rt;
		printf("\n REGS[rt] = 0x%08x \n", NEXT_STATE.REGS[rt]);
		break;

		case 0x3C000000: //For LUI
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
		rdd=rt;
		printf("\nREGS[rt] = 0x%08x \n", NEXT_STATE.REGS[rt]);
		break;
		
		case 0x34000000: //For ORI 
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
		rdd=rt;
		printf("\nMEM_WB.ALUOutpu=0x%08x\n",MEM_WB.ALUOutput);
		printf("\nREGS[rt] = 0x%08x \n", NEXT_STATE.REGS[rt]);
		break;

		case 0x38000000: //For XORI 
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
		rdd=rt;
		printf("\nREGS[rt] = 0x%08x \n", NEXT_STATE.REGS[rt]);
		printf("\nMEM_WB.ALUOutpu=0x%08x\n",MEM_WB.ALUOutput);
		break;

		case 0x30000000: //For ANDI
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
		rdd=rt;
		printf("\n REGS[rt] = 0x%08x \n", NEXT_STATE.REGS[rt]);
		break;

		case 0x28000000: //For SLTI
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
		rdd=rt;
		printf("\n REGS[rt] = 0x%08x \n", NEXT_STATE.REGS[rt]);
		break;

	
	/***********************************************************LW/SW-Type************************************************************************/
		case 0x8C000000:	//For LW.
		WB_Value=MEM_WB.LMD;
		NEXT_STATE.REGS[rt] = MEM_WB.LMD;
		rdd=rt;
		printf("\nLMD = 0x%08x \n", NEXT_STATE.REGS[rt]);
		break;

		case  0x84000000:	//For LH.
		WB_Value=MEM_WB.LMD;
		NEXT_STATE.REGS[rt] = MEM_WB.LMD;
		rdd=rt;
		printf("\nLMD = 0x%08x \n", NEXT_STATE.REGS[rt]);
		break;

		case 0x80000000:	//For LB.
		WB_Value=MEM_WB.LMD;
		NEXT_STATE.REGS[rt] = MEM_WB.LMD;
		rdd=rt;
		printf("\nLMD = 0x%08x \n", NEXT_STATE.REGS[rt]);
		break;
	/***********************************************************j-Type************************************************************************/
		case 0x0C000000:	//For jal.
		WB_Value=MEM_WB.ALUOutput;
		NEXT_STATE.REGS[31] =MEM_WB.ALUOutput;
		//rdd=rt;
		//printf("\nLMD = 0x%08x \n", NEXT_STATE.REGS[rt]);
		break;

	}	
}

/************************************************************/
/* memory access (MEM) pipeline stage:                                                          */ 
/************************************************************/
void MEM()
{
	uint32_t rs;
	uint32_t rt;
	uint32_t rd;
	/*IMPLEMENT THIS*/
	
	MEM_WB.IR = EX_MEM.IR;	
	printf("\n**************************************Memory Stage*****************************************\n");
	printf("MEM/WB.IR=  0x%08x", MEM_WB.IR );
	
	//Decode the opcode from the instruction
	uint32_t opcode = (0xFC000000 & MEM_WB.IR ); 
	//printf("\nOpcode = 0x%08x ",opcode);

	//get the address value of rd
	rd = (0x0000F800 & MEM_WB.IR ) >> 11;
	//MEM_WB_rd -- check stall condition in decode stage
	MEM_WB_rd = rd;
	printf("\n rd=%d \n", MEM_WB_rd);
	//rt = (0x001F0000 & IF_EX.IR ) >> 16;
	rd1 = (0x001F0000 & MEM_WB.IR) >> 16;
	switch (opcode) {
		
		/***********************************************************R-Type************************************************************************/
		case 0x00000000: 
		//MEM_WB.ALUOutput = EX_MEM.ALUOutput;
		//FW_Mem=MEM_WB.ALUOutput;
		//printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
		//break;

		function = (0x0000003F & MEM_WB.IR);
		
		switch (function) {
					/****************--System Call function*************/ 
			case 0x0000000c:
			//printf("\n This is an System call function");
			RUN_FLAG = FALSE;
			break;
			/**************--(JALR-Jump And Link Register)*******************************/
			case 0x00000009:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			/*************--ADD function*******************/
			case 0x00000020:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			
			/***********--ADDU function*******************/
			case 0x00000021:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			
			/***********--SUB function********************/
			case 0x00000022:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			
			/***********--SUBU function******************/
			case 0x00000023:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			
			/************--AND function*****************/
			case 0x00000024:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			
			/**************--OR function****************/
			case 0x00000025:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			
		
			/****************--XOR function*************/ 
			//(if rs and rt is different, rd = 1 else if rs and rt are same then rd is 0)
			case 0x00000026:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;

			/************NOR function ( !(rs or rt) )***/
			case 0x00000027:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;		
			
			/************SLT function*******************/ 
			case 0x0000002A:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			
			/****SLL function (Shift Left Logical)cycle**/
			case 0x00000000:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			
			/*****--SRL function (Shit Right Logical)****/
			case 0x00000002:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			
			/**--SRA function (Shift Right Arithmetic)***/
			case 0x00000003:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			
			/*****MULTU function (Multiply)***************/
			case 0x00000019:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			MEM_WB.ALUOutput1 = EX_MEM.ALUOutput1;
			FW_Mem=MEM_WB.ALUOutput;
			FW_Mem1=MEM_WB.ALUOutput1;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			printf("\nMEM/WB.ALUOutput1 = 0x%08x \n", MEM_WB.ALUOutput1);
			break;
			
			/******DIVU (Divide unsigned function)****/
			case 0x0000001B:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			MEM_WB.ALUOutput1 = EX_MEM.ALUOutput1;
			FW_Mem=MEM_WB.ALUOutput;
			FW_Mem1=MEM_WB.ALUOutput1;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			printf("\nMEM/WB.ALUOutput1 = 0x%08x \n", MEM_WB.ALUOutput1);
			break;
			
			/******MFHI function (Move from HI)****/
			case 0x00000010:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			
			/******MFLO function (Move from low)****/
			case 0x00000012:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			
			/******MTHI function (Move TO HI)****/
			case 0x00000011:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;
			
			/******MTLO function (Move to Low)****/
			case 0x00000013:
			MEM_WB.ALUOutput = EX_MEM.ALUOutput;
			FW_Mem=MEM_WB.ALUOutput;
			printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
			break;

		}
			
		break;
		
		/***********************************************************I-Type************************************************************************/
		case 0x20000000: //For ADDI 
		MEM_WB.ALUOutput = EX_MEM.ALUOutput;
		MEM_WB_rd = rd1;
		FW_Mem=MEM_WB.ALUOutput;
		printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
		break;
		
		case 0x24000000: //For ADDIU 
		MEM_WB.ALUOutput = EX_MEM.ALUOutput;
		FW_Mem=MEM_WB.ALUOutput;
		MEM_WB_rd = rd1;
		printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
		break;

		case 0x3C000000: //For LUI
		MEM_WB.ALUOutput = EX_MEM.ALUOutput;
		FW_Mem=MEM_WB.ALUOutput;
		MEM_WB_rd = rd1;
		printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
		break;
		
		case 0x34000000: //For ORI 
		MEM_WB.ALUOutput = EX_MEM.ALUOutput;
		FW_Mem=MEM_WB.ALUOutput;
		MEM_WB_rd = rd1;
		printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
		break;

		case 0x38000000: //For XORI 
		MEM_WB.ALUOutput = EX_MEM.ALUOutput;
		MEM_WB_rd = rd1;
		FW_Mem=MEM_WB.ALUOutput;
		printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
		break;

		case 0x28000000: //For SLTI
		MEM_WB.ALUOutput = EX_MEM.ALUOutput;
		FW_Mem=MEM_WB.ALUOutput;
		MEM_WB_rd = rd1;
		printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
		break;
		
		case 0x30000000: //For ANDI
		MEM_WB.ALUOutput = EX_MEM.ALUOutput;
		FW_Mem=MEM_WB.ALUOutput;
		MEM_WB_rd = rd1;
		printf("\nMEM/WB.ALUOutput = 0x%08x \n", MEM_WB.ALUOutput);
		break;

		/***********************************************************LW/SW-Type************************************************************************/
        

		case 0xAC000000:  //For SW.
		Boff = EX_MEM.ALUOutput & 0x3;
        	Woff = EX_MEM.ALUOutput & 0xC;
		Woff = Woff >> 2;
        	Index = EX_MEM.ALUOutput & 0xf0;
        	Index = Index >> 4;
	      	tag = EX_MEM.ALUOutput & 0xFFFFFF00;
		tag = tag >> 8;
		

		printf(" %x %x index: %x %x\n", Boff, Woff, Index, tag);
		printf("Address: %x\n", EX_MEM.ALUOutput);
		
		if(L1Cache.blocks[Index].valid){
			if(L1Cache.blocks[Index].tag == tag){
				L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
				cache_hits++;
				printf("adding to cache_hits****************\n");
			}
			else{
						printf("****************was valid, tag didnt match, case is : %x\n", Woff);			
				switch(Woff){

					case 0:

						L1Cache.blocks[Index].valid = 1;
						L1Cache.blocks[Index].tag = tag;
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput + 4);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 8);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 12);
						break;
						
					case 1:
						L1Cache.blocks[Index].valid = 1;
						L1Cache.blocks[Index].tag = tag;
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput + 0);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 4);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 8);
						break;
					case 2:
						L1Cache.blocks[Index].valid = 1;
						L1Cache.blocks[Index].tag = tag;
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 8);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 4);
						break;
					case 3:
						L1Cache.blocks[Index].valid = 1;
						L1Cache.blocks[Index].tag = tag;
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 12);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 8);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						break;
				}
				
				L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
				cache_misses++;
				printf("**************************cache misses increasing\n");
				CYCLE_COUNT += 99;
				
			}
		} 
		else{
				printf("****************8wasnt valid, case is : %x\n", Woff);
			switch(Woff){

				case 0:
					L1Cache.blocks[Index].valid = 1;
						L1Cache.blocks[Index].tag = tag;
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput + 4);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 8);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 12);
					break;
					
				case 1:
					L1Cache.blocks[Index].valid = 1;
						L1Cache.blocks[Index].tag = tag;
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput + 0);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 4);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 8);
					break;
				case 2:
					L1Cache.blocks[Index].valid = 1;
						L1Cache.blocks[Index].tag = tag;
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 8);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 4);
					break;
				case 3:
					L1Cache.blocks[Index].valid = 1;
						L1Cache.blocks[Index].tag = tag;
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 12);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 8);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					break;
			}
			L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
			cache_misses++;
			printf("*****************cache misses increasing\n");
			CYCLE_COUNT += 99;
		}
		printf("\nMEM/WB.ALUOutput = 0x%08x\n", EX_MEM.ALUOutput);
		mem_write_32(EX_MEM.ALUOutput, EX_MEM.B);
		printf("\nMEM/WB.ALUOutput = 0x%08x, EX_MEM.B =0x%08x \n", EX_MEM.ALUOutput, EX_MEM.B);
		
		break;

		case 0xA4000000:  //For SH.
		Boff = EX_MEM.ALUOutput & 0x3;
        Woff = EX_MEM.ALUOutput & 0xC >> 2;
		Index = EX_MEM.ALUOutput & 0xf0 >> 4;
        tag = EX_MEM.ALUOutput & 0xFFFFFF00 >> 8;
		

		printf(" %x %x %x %x\n", Boff, Woff, Index, tag);
		
		if(L1Cache.blocks[Index].valid){
			if(L1Cache.blocks[Index].tag == tag){
				L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
				cache_hits++;
			}
			else{
				
				switch(Woff){
					case 0:
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput + 4);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 8);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 12);
						break;
						
					case 1:
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput + 0);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 4);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 8);
						break;
					case 2:
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 8);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 4);
						break;
					case 3:
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 12);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 8);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						break;
				}
				
				L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
				++cache_misses;
				CYCLE_COUNT += 99;
				
			}
		} 
		else{
			switch(Woff){
				case 0:
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput + 4);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 8);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 12);
					break;
					
				case 1:
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput + 0);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 4);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 8);
					break;
				case 2:
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 8);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 4);
					break;
				case 3:
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 12);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 8);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					break;
			}
			L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
			++cache_misses;
			CYCLE_COUNT += 99;
		}
		mem_write_32(EX_MEM.ALUOutput, EX_MEM.B);
		break;

		case 0xA0000000:  //For SB.
			
		Boff = EX_MEM.ALUOutput & 0x3;
        Woff = EX_MEM.ALUOutput & 0xC >> 2;
        Index = EX_MEM.ALUOutput & 0xf0 >> 4;
        tag = EX_MEM.ALUOutput & 0xFFFFFF00 >> 8;
		

		printf(" %x %x %x %x\n", Boff, Woff, Index, tag);
		
		if(L1Cache.blocks[Index].valid){
			if(L1Cache.blocks[Index].tag == tag){
				L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
				cache_hits++;
			}
			else{
				
				switch(Woff){
					case 0:
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput + 4);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 8);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 12);
						break;
						
					case 1:
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput + 0);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 4);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 8);
						break;
					case 2:
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 8);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 4);
						break;
					case 3:
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 12);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 8);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						break;
				}
				
				L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
				++cache_misses;
				CYCLE_COUNT += 99;
				
			}
		} 
		else{
			switch(Woff){
				case 0:
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput + 4);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 8);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 12);
					break;
					
				case 1:
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput + 0);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 4);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 8);
					break;
				case 2:
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 8);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 4);
					break;
				case 3:
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 12);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 8);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					break;
			}
			L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
			++cache_misses;
			CYCLE_COUNT += 99;
		}
		mem_write_32(EX_MEM.ALUOutput, EX_MEM.B);
		printf("\nMEM/WB.ALUOutput = 0x%08x, EX_MEM.B =0x%08x \n", EX_MEM.ALUOutput, EX_MEM.B);
		break;
		
		case 0x8C000000:	//For LW.
		Boff = EX_MEM.ALUOutput & 0x3;
        	Woff = EX_MEM.ALUOutput & 0xC;
		Woff = Woff >> 2;
        	Index = EX_MEM.ALUOutput & 0xf0;
        	Index = Index >> 4;
	      	tag = EX_MEM.ALUOutput & 0xFFFFFF00;
		tag = tag >> 8;	

		printf(" %x %x %x %x\n", Boff, Woff, Index, tag);
		
		if(L1Cache.blocks[Index].valid){
			if(L1Cache.blocks[Index].tag == tag){
				MEM_WB.LMD = L1Cache.blocks[Index].words[Woff];
				cache_hits++;
				printf("adding to cache_hits*****************\n");
			}
			else{
				
				switch(Woff){
					printf("was valid, tag didnt match, case is : %x\n", Woff);
					case 0:
						L1Cache.blocks[Index].valid = 1;
						L1Cache.blocks[Index].tag = tag;
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput + 4);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 8);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 12);
						break;
						
					case 1:
						L1Cache.blocks[Index].valid = 1;
						L1Cache.blocks[Index].tag = tag;
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput + 0);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 4);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 8);
						break;
					case 2:
						L1Cache.blocks[Index].valid = 1;
						L1Cache.blocks[Index].tag = tag;
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 8);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 4);
						break;
					case 3:
						L1Cache.blocks[Index].valid = 1;
						L1Cache.blocks[Index].tag = tag;
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 12);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 8);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						break;
				}
				
				EX_MEM.B = L1Cache.blocks[Index].words[Woff];
				++cache_misses;
				printf("************************cache misses increasing\n");
				CYCLE_COUNT += 99;
				
			}
		} 
		else{
			switch(Woff){
									printf("wasnt valid, case is : %x\n", Woff);
				case 0:
					L1Cache.blocks[Index].valid = 1;
					L1Cache.blocks[Index].tag = tag;
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput + 4);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 8);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 12);
					break;
					
				case 1:
					L1Cache.blocks[Index].valid = 1;
					L1Cache.blocks[Index].tag = tag;
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput + 0);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 4);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 8);
					break;
				case 2:
					L1Cache.blocks[Index].valid = 1;
					L1Cache.blocks[Index].tag = tag;
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 8);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 4);
					break;
				case 3:
					L1Cache.blocks[Index].valid = 1;
					L1Cache.blocks[Index].tag = tag;
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 12);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 8);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					break;
			}
			MEM_WB.LMD = L1Cache.blocks[Index].words[Woff];
			++cache_misses;
			printf("************************88cache misses increasing\n");
			CYCLE_COUNT += 99;
		}
		//MEM_WB.LMD = mem_read_32(EX_MEM.ALUOutput);
		FW_Mem=MEM_WB.LMD;
		printf("\nMEM/WB.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
		printf("\nMEM/WB.LMD = 0x%08x \n", MEM_WB.LMD);
		break;
		
		
		case 0x84000000:	//For LH.
			
		Boff = EX_MEM.ALUOutput & 0x3;
        Woff = EX_MEM.ALUOutput & 0xC >> 2;
        Index = EX_MEM.ALUOutput & 0xf0 >> 4;
        tag = EX_MEM.ALUOutput & 0xFFFFFF00 >> 8;
		

		printf(" %x %x %x %x\n", Boff, Woff, Index, tag);
		
		if(L1Cache.blocks[Index].valid){
			if(L1Cache.blocks[Index].tag == tag){
				L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
				cache_hits++;
			}
			else{
				
				switch(Woff){
					case 0:
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput + 4);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 8);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 12);
						break;
						
					case 1:
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput + 0);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 4);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 8);
						break;
					case 2:
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 8);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 4);
						break;
					case 3:
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 12);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 8);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						break;
				}
				
				L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
				++cache_misses;
				CYCLE_COUNT += 99;
				
			}
		} 
		else{
			switch(Woff){
				case 0:
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput + 4);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 8);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 12);
					break;
					
				case 1:
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput + 0);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 4);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 8);
					break;
				case 2:
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 8);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 4);
					break;
				case 3:
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 12);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 8);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					break;
			}
			L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
			++cache_misses;
			CYCLE_COUNT += 99;
		}
	 	LH_mem_value = mem_read_32(EX_MEM.ALUOutput);
		MEM_WB.LMD = ((LH_mem_value & 0x0000FFFF) & 0x8000) > 0 ? (LH_mem_value | 0xFFFF0000) : (LH_mem_value & 0x0000FFFF);
		FW_Mem=MEM_WB.LMD;
		printf("\nMEM/WB.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
		printf("\nMEM/WB.LMD = 0x%08x \n", MEM_WB.LMD);
		break;

		case 0x80000000:	//For LB.
		
		Boff = EX_MEM.ALUOutput & 0x3;
        Woff = EX_MEM.ALUOutput & 0xC >> 2;
        Index = EX_MEM.ALUOutput & 0xf0 >> 4;
        tag = EX_MEM.ALUOutput & 0xFFFFFF00 >> 8;
		

		printf(" %x %x %x %x\n", Boff, Woff, Index, tag);
		
		if(L1Cache.blocks[Index].valid){
			if(L1Cache.blocks[Index].tag == tag){
				L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
				cache_hits++;
				printf("adding to cache_hits\n");
			}
			else{
				
				switch(Woff){
					case 0:
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput + 4);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 8);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 12);
						break;
						
					case 1:
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput + 0);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 4);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 8);
						break;
					case 2:
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 8);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 4);
						break;
					case 3:
						L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 12);
						L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 8);
						L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput - 4);
						L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
						break;
				}
				
				L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
				++cache_misses;
				printf("cache misses increasing\n");
				CYCLE_COUNT += 99;
				
			}
		} 
		else{
			switch(Woff){
				case 0:
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput + 4);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 8);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 12);
					break;
					
				case 1:
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput + 0);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput + 4);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 8);
					break;
				case 2:
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 8);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					L1Cache.blocks[Index].words[3] = mem_read_32(EX_MEM.ALUOutput + 4);
					break;
				case 3:
					L1Cache.blocks[Index].words[0] = mem_read_32(EX_MEM.ALUOutput - 12);
					L1Cache.blocks[Index].words[1] = mem_read_32(EX_MEM.ALUOutput - 8);
					L1Cache.blocks[Index].words[2] = mem_read_32(EX_MEM.ALUOutput - 4);
					L1Cache.blocks[Index].words[Woff] = mem_read_32(EX_MEM.ALUOutput);
					break;
			}
			L1Cache.blocks[Index].words[Woff] = EX_MEM.B;
			++cache_misses;
			printf("cache misses increasing\n");
			CYCLE_COUNT += 99;
		}
		LH_mem_value = mem_read_32(EX_MEM.ALUOutput);
		MEM_WB.LMD = ((LH_mem_value & 0x000000FF) & 0x80) > 0 ? (LH_mem_value | 0xFFFFFF00) : (LH_mem_value & 0x000000FF);
		FW_Mem=MEM_WB.LMD;
		printf("\nMEM/WB.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
		printf("\nMEM/WB.LMD = 0x%08x \n", MEM_WB.LMD);
		break;


		/***********************************************************j-Type************************************************************************/
		case 0x0C000000:	//For jal.
		MEM_WB.ALUOutput = EX_MEM.ALUOutput;
		break;

	}
	
	
}

/************************************************************/
/* execution (EX) pipeline stage:                                                                          */ 
/************************************************************/
void EX()
{
	uint32_t rs;
	uint32_t rt;
	uint32_t rd;
	uint32_t Op_upper;
	uint32_t sign;
        EX_MEM.PC=IF_EX.PC;
	if (sh==1){ //1 stall
		if(sh_A==1){
			IF_EX.A=WB_Value;
			sh_A=0;
			sh=0;}
		if(sh_B==1){
			IF_EX.B=WB_Value;
			sh_B=0;
			sh=0;}
		}
	if (p>=1){ // 2 stall
		if(p==3){
			stallflag=0;
			p=0;
			if(A==1){
				IF_EX.A=WB_Value;
				A=0;}
			if(B==1){
				IF_EX.B=WB_Value;
				B=0;}
		}
		else
			p++;
	}
	if(p1>=1){ // load forwarding
		if(p1==2){
		FW_LW=0;
		p1=0;
			if(A==1){
				IF_EX.A=FW_Mem;
				A=0;}
			if(B==1){
				IF_EX.B=FW_Mem;
				B=0;}
		}
		else
			p1++;		
	}
	if(p3==1){
		p2=1;
		p3=0;}
	if (p ==0 && p1 ==0 && p2==0 && p5==0){
	/*IMPLEMENT THIS*/
	printf("\n**************************************Execute Stage****************************************\n");
	EX_MEM.IR = IF_EX.IR;
	printf("\nEX/MEM.IR=0x%08x\n", EX_MEM.IR );

	//Decode the opcode from the instruction
	uint32_t opcode = (0xFC000000 & EX_MEM.IR ); 
	//printf("\nOpcode = 0x%08x ",opcode);
	
	switch (opcode) {
		
		/***********************************************************R-Type************************************************************************/
		case 0x00000000:
		//printf("\n This is a R-type instruction");
		//get the address value of rs 
		rs = (0x03E00000 & EX_MEM.IR ) >> 21;
		//printf("\n rs = %d", rs);
		
		//get the address value of rt
	        rt = (0x001F0000 & EX_MEM.IR ) >> 16;
		//printf("\n rt = %d", rt);
		
		//get the address value of rd
		rd = (0x0000F800 & EX_MEM.IR ) >> 11;
		EX_MEM_rd = rd;
		printf("\n EX_MEM_rd ++++++++++++++= %d", EX_MEM_rd );
		
		//get the value of shift amount
		uint32_t sa = (0x000007C0 & EX_MEM.IR ) >> 6;
		//printf("\n sa = %d", sa);
		
		//decoding the function bit
		uint32_t function = (0x0000003F & EX_MEM.IR ); 
		//printf("\n function = 0x%08x\n", function);
		
		switch (function) {

					/***********************************ALU instruction**********************************************/
					
					/*************--ADD function*******************/
					case 0x00000020:
					//printf("\n This is an ADD function");
					EX_MEM.ALUOutput = IF_EX.A + IF_EX.B;
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					break;
					
					/*************--ADDU function*******************/
					case 0x00000021:
					//printf("\n This is an ADD function");
					EX_MEM.ALUOutput = IF_EX.A + IF_EX.B;
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					break;

					/************--AND function*****************/
					case 0x00000024:
					//printf("\n This is an AND function");
					EX_MEM.ALUOutput = IF_EX.A & IF_EX.B;
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					break;
					
					/************--OR function*****************/
					case 0x00000025:
					//printf("\n This is an OR function");
					EX_MEM.ALUOutput = IF_EX.A | IF_EX.B;
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					//MEM_WB.ALUOutput = EX_MEM.ALUOutput;
					break;

					/************NOR function ( !(rs or rt) )***/
					case 0x00000027:
					EX_MEM.ALUOutput = ~(IF_EX.A | IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					//EX_MEM.ALUOutput=!(EX_MEM.ALUOutput);
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					//MEM_WB.ALUOutput = EX_MEM.ALUOutput;
					break;

					/*************--SUB function*******************/
					case 0x00000022:
					//printf("\n This is an ADD function");
					EX_MEM.ALUOutput = IF_EX.A - IF_EX.B;
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					break;
					
					/*************--MULTU function*******************/
					case 0x00000019:
					//uint64_t mo;
					//mo=((IF_EX.A * IF_EX.B) & 0xffffffff00000000)>>32;
					EX_MEM.ALUOutput=1;
					EX_MEM.ALUOutput1=((IF_EX.A * IF_EX.B) & 0x00000000ffffffff); //declare ALUOutput1 on the header file.
					FW_Exe=EX_MEM.ALUOutput;
					FW_Exe1=EX_MEM.ALUOutput1;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					printf("\nEX/MEM.ALUOutput1 = 0x%08x \n", EX_MEM.ALUOutput1);
					aa=1;
					break;
					
					/******DIVU (Divide unsigned function)****/
					case 0x0000001B:	
								
					EX_MEM.ALUOutput=(IF_EX.A >> 2);
					EX_MEM.ALUOutput1=(IF_EX.A - (EX_MEM.ALUOutput << 2));
					FW_Exe=EX_MEM.ALUOutput;
					FW_Exe1=EX_MEM.ALUOutput1;
					to=EX_MEM.ALUOutput;
					to1=EX_MEM.ALUOutput1;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					printf("\nEX/MEM.ALUOutput1 = 0x%08x \n", EX_MEM.ALUOutput1);
					aa=1;
					break;
					
					/******MFHI function (Move from HI)****/
					case 0x00000010:
					EX_MEM.ALUOutput=(IF_EX.A);
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					break;
					
					/******MFLO function (Move from Low)****/
					case 0x00000012:
					EX_MEM.ALUOutput=(IF_EX.A);
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					break;
					
					/******MTHI function (Move TO HI)****/
					case 0x00000011:
					EX_MEM.ALUOutput=(IF_EX.A);
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					break;
					
					/******MTLO function (Move To Low)****/
					case 0x00000013:
					EX_MEM.ALUOutput=(IF_EX.A);
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					break;
					
					/*************--SLT function*******************/
					case 0x0000002A:
					//printf("\n This is an ADD function");

					if (IF_EX.A < IF_EX.B)
					{
						EX_MEM.ALUOutput = 0x00000001;

					}
					else
					{
						EX_MEM.ALUOutput = 0x00000000;

					}

					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					break;

					/*************--SRL function*******************/
					case 	0x00000002:

					EX_MEM.ALUOutput = IF_EX.B >> sa;
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					break;

					/*************--SRA function*******************/
					case 	0x00000003:

					EX_MEM.ALUOutput = IF_EX.B >> sa;
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					break;

					/*************--SLL function*******************/
					case 	0x00000000:

					EX_MEM.ALUOutput = IF_EX.B << sa;
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					break;
					
					/**************--(JALR-Jump And Link Register)*******************************/
					case 0x00000009:
					EX_offset=IF_EX.A;
					EX_MEM.ALUOutput=EX_MEM.PC;
					printf("\nPC+4 = 0x%08x \n",ID_IF_PC);
					printf("\nnext address = 0x%08x\n", EX_offset);
					jalr=1;
					break;

					/**************--(JR-Jump Register)*******************************/
					case 0x00000008:
					EX_offset=IF_EX.A;
					printf("\nnext address = 0x%08x\n", EX_offset);
					jr=1;				
					break;

					/****************--XOR function*************/ 
					//(if rs and rt is different, rd = 1 else if rs and rt are same then rd is 0)
					case 0x00000026:
					//printf("\n This is an XOR function");
					EX_MEM.ALUOutput = IF_EX.A ^ IF_EX.B;
					FW_Exe=EX_MEM.ALUOutput;
					printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
					printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
					printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
					aa=1;
					break;
					
					/****************--System Call function*************/ 
					//case 0x0000000c:
					//printf("\n This is an System call function");
					//RUN_FLAG = FALSE;
					//break;
			}
		break;
		
	/******************************************************I-Type***********************************************************************/
	//(ADDI- Add immediate unisigned)
		case 0x20000000:
		rd = (0x001f0000 & EX_MEM.IR ) >> 16;
		EX_MEM_rd = rd;
		printf("\n EX_MEM_rd ++++++++++++++= %d", EX_MEM_rd );
		EX_MEM.ALUOutput = IF_EX.A + IF_EX.imm;
		FW_Exe=EX_MEM.ALUOutput;
		printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
		printf("\nEX/MEM.B = 0x%08x \n", IF_EX.imm);
		printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
		break;
		
	//(ADDIU- Add immediate unisigned)
		case 0x24000000:
		rd = (0x001f0000 & EX_MEM.IR ) >> 16;
		EX_MEM_rd = rd;
		printf("\n EX_MEM_rd ++++++++++++++= %d", EX_MEM_rd );
		EX_MEM.ALUOutput = IF_EX.A + IF_EX.imm;
		FW_Exe=EX_MEM.ALUOutput;
		printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
		printf("\nEX/MEM.B = 0x%08x \n", IF_EX.imm);
		printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
		break;
		/*************--SLT function*******************/
			case 0x0000002A:
			//printf("\n This is an ADD function");
			rd = (0x001f0000 & EX_MEM.IR ) >> 16;
			EX_MEM_rd = rd;
			if (IF_EX.A < IF_EX.B)
			{
				EX_MEM.ALUOutput = 0x00000001;

			}
			else
			{
				EX_MEM.ALUOutput = 0x00000000;

			}

			FW_Exe=EX_MEM.ALUOutput;
			printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
			printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
			printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
			aa=1;
			break;

			/*************--SLTI function*******************/
			case 0x28000000:
			//printf("\n This is an ADD function");
			register_value = IF_EX.A ;
			register_sign_decode = IF_EX.A >>31;
			rd = (0x001f0000 & EX_MEM.IR ) >> 16;
			EX_MEM_rd = rd;
			switch(immediate_sign_decode){

			case 00000001:

			if (register_sign_decode == 00000001)
				if (register_value < IF_EX.imm  ){
					EX_MEM.ALUOutput = 00000001;}
				else{
					EX_MEM.ALUOutput = 00000000;}
			if (register_sign_decode == 00000000){
					EX_MEM.ALUOutput = 00000000;
			}
			break;

			case 00000000:

			if (register_sign_decode == 00000000)
				if (register_value < IF_EX.imm ){
					EX_MEM.ALUOutput = 00000001;}

				else{
					EX_MEM.ALUOutput = 00000000;}
			if (register_sign_decode == 00000001){
				EX_MEM.ALUOutput = 00000001;
			}
			break;
			}

			FW_Exe=EX_MEM.ALUOutput;
			printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
			printf("\nEX/MEM.B = 0x%08x \n", IF_EX.B);
			printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
			aa=1;
			break;

			//(ANDI- AND immediate)
			case 0x30000000:
			rd = (0x001f0000 & EX_MEM.IR ) >> 16;
			EX_MEM_rd = rd;
			printf("\n EX_MEM_rd ++++++++++++++= %d", EX_MEM_rd );
			EX_MEM.ALUOutput = IF_EX.A & IF_EX.imm;
			FW_Exe=EX_MEM.ALUOutput;
			printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
			printf("\nEX/MEM.B = 0x%08x \n", IF_EX.imm);
			printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
			break;

		/****************--(LUI- Load upper immediate)***************/	
		case 0x3C000000:
		rd = (0x001f0000 & EX_MEM.IR ) >> 16;
		EX_MEM_rd = rd;
		printf("\n EX_MEM_rd ++++++++++++++= %d", EX_MEM_rd );
		EX_MEM.ALUOutput = IF_EX.imm << 16;
		FW_Exe=EX_MEM.ALUOutput;
		printf("\nEX/MEM.A = 0x%08x \n", IF_EX.imm);
		//printf("\nEX/MEM.B = 0x%08x \n", IF_EX.imm);
		printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
		break;
	
	/********************--(SW- Store word)********************/
		case 0xAC000000:
		EX_MEM.ALUOutput =  IF_EX.A + IF_EX.imm;
		EX_MEM.B = IF_EX.B; //did this because the value of Rt will be needed on the next stage. 
		//FW_Exe=EX_MEM.ALUOutput;
		printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
		printf("\nEX/MEM.B = 0x%08x \n", IF_EX.imm);
		printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
		//printf("\nB = %d \n", EX_MEM.B);
		break;
		
		/********************--(SH- Store halfword)********************/
			case 0xA4000000:
			EX_MEM.ALUOutput =  IF_EX.A + IF_EX.imm;
		  	SH_rt_mask= IF_EX.B & 0x0000ffff;
			EX_MEM.B =  SH_rt_mask; //did this because the value of Rt will be needed on the next stage.
			//FW_Exe=EX_MEM.ALUOutput;
			printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
			printf("\nEX/MEM.B = 0x%08x \n", IF_EX.imm);
			printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
			//printf("\nB = %d \n", EX_MEM.B);
			break;

		/********************--(SB- Store Byte)********************/
			case 0xA0000000:
			EX_MEM.ALUOutput =  IF_EX.A + IF_EX.imm;
			SH_rt_mask= IF_EX.B & 0x000000ff;
			EX_MEM.B =  SH_rt_mask; //did this because the value of Rt will be needed on the next stage.
			//FW_Exe=EX_MEM.ALUOutput;
			printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
			printf("\nEX/MEM.B = 0x%08x \n", IF_EX.imm);
			printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
			//printf("\nB = %d \n", EX_MEM.B);
			break;

	/*****************--(LW- Load Word)******************/
		case 0x8C000000:
		rd = (0x001f0000 & EX_MEM.IR ) >> 16;
		EX_MEM_rd = rd;
		printf("\n EX_MEM_rd ++++++++++++++= %d", EX_MEM_rd );
		EX_MEM.ALUOutput =  IF_EX.A + IF_EX.imm;
		FW_Exe=EX_MEM.ALUOutput;
		FW_LW=1;
		printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
		printf("\nEX/MEM.B = 0x%08x \n", IF_EX.imm);
		printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
		
		break;

		/*****************--(LH- Load HalfWord)******************/
			case 0x84000000:
			rd = (0x001f0000 & EX_MEM.IR ) >> 16;
			EX_MEM_rd = rd;
			printf("\n EX_MEM_rd ++++++++++++++= %d", EX_MEM_rd );
			EX_MEM.ALUOutput =  IF_EX.A + IF_EX.imm;
			FW_Exe=EX_MEM.ALUOutput;
			FW_LW=1;
			printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
			printf("\nEX/MEM.B = 0x%08x \n", IF_EX.imm);
			printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);

			break;

		/*****************--(LB- Load Byte)******************/
				case 0x80000000:
				rd = (0x001f0000 & EX_MEM.IR ) >> 16;
				EX_MEM_rd = rd;
				printf("\n EX_MEM_rd ++++++++++++++= %d", EX_MEM_rd );
				EX_MEM.ALUOutput =  IF_EX.A + IF_EX.imm;
				FW_Exe=EX_MEM.ALUOutput;
				FW_LW=1;
				printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
				printf("\nEX/MEM.B = 0x%08x \n", IF_EX.imm);
				printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);

				break;

	/*****************--(beq)******************/
		case 0x10000000:
		offset_value = (IF_EX.imm) << 2; // Masking with 0000 0000 0000 0000 1111 1111 1111 1111(sign extended)
		EX_offset = (ID_IF_PC - 8 + offset_value) ;
		if (IF_EX.A == IF_EX.B)
		{
			beq=1;	
		}
		else 
		{
			beq=0;
		}
		printf("\nA = 0x%08x \n", IF_EX.A);
		printf("\nB = 0x%08x \n", IF_EX.B);
		printf("\nEX/MEM.imm = 0x%08x \n", IF_EX.imm);
		printf("\noffset_value = 0x%08x \n", offset_value);
		printf("\nPC+4 = 0x%08x \n",ID_IF_PC);
		printf("\nnext address = 0x%08x \n", EX_offset);
		p3++;
		break;
	/*****************--(bne)******************/
		case 0x14000000:
		offset_value = (IF_EX.imm) << 2; // Masking with 0000 0000 0000 0000 1111 1111 1111 1111(sign extended)
		EX_offset = (ID_IF_PC - 8+ offset_value);
		if (IF_EX.A != IF_EX.B)
		{
			bne=1;	
		}
		else 
		{
			bne=0;
		}
		printf("\nA = 0x%08x \n", IF_EX.A);
		printf("\nB = 0x%08x \n", IF_EX.B);
		printf("\nEX/MEM.imm = 0x%08x \n", IF_EX.imm);
		printf("\noffset_value = 0x%08x \n", offset_value);
		printf("\nPC = 0x%08x \n",ID_IF_PC);
		printf("\nnext address = 0x%08x, bne=%d\n", EX_offset, bne);
		p3++;
		break;
	/**************--(JAL-Jump and link instruction)*******************************/
		case 0x0C000000:
		offset_value = (IF_EX.imm) << 2; // Masking with 0000 0000 0000 0000 1111 1111 1111 1111(sign extended)
		Op_upper =  (0xf00000000 & (ID_IF_PC));
		EX_offset = (Op_upper | offset_value);
		EX_MEM.ALUOutput=ID_IF_PC-4;
		printf("\nEX/MEM.imm = 0x%08x \n", IF_EX.imm);
		printf("\noffset_value = 0x%08x \n", offset_value);
		printf("\nPC = 0x%08x \n",ID_IF_PC-4);
		printf("\nnext address = 0x%08x \n", EX_offset);
		jal=1;
		p3++;
		break;
	/**************--(Jump Instruction)*******************************/
		case 0x08000000:
		offset_value = (IF_EX.imm) << 2; // Masking with 0000 0000 0000 0000 1111 1111 1111 1111(sign extended)
		Op_upper =  (0xf00000000 & (ID_IF_PC));
		EX_offset = (Op_upper | offset_value);
		j=1;
		printf("\nEX/MEM.imm = 0x%08x \n", IF_EX.imm);
		printf("\noffset_value = 0x%08x \n", offset_value);
		//printf("\nPC+4 = 0x%08x \n",ID_IF_PC-4);
		printf("\nnext address = 0x%08x\n", EX_offset);
		p3++;
		break;
	/**************--(BGTZ- Branch on Greater than zero)*******************************/
        	case 0x1C000000:
		offset_value = (IF_EX.imm) << 2; // Masking with 0000 0000 0000 0000 1111 1111 1111 1111(sign extended)
		EX_offset = (ID_IF_PC - 8 + offset_value);
		sign = (IF_EX.A)>>31;
		if ( sign == 0x00000000 || IF_EX.A != 0x00000000 )
		{
			bgtz=1;	
		}
		else 
		{
			bgtz=0;
		}
		printf("\nA = 0x%08x \n", IF_EX.A);
		printf("\nEX/MEM.imm = 0x%08x \n", IF_EX.imm);
		printf("\noffset_value = 0x%08x \n", offset_value);
		printf("\nPC+4 = 0x%08x \n",ID_IF_PC);
		printf("\nnext address = 0x%08x \n", EX_offset);
		p3++;
		break;

	/***************--(BGEZ- Branch on Greater than or equal to zero---BLTZ- Branch on less than zero)*****************/
		case 0x04000000:
		rt = (0x001F0000 & EX_MEM.IR ) >> 16;		
		offset_value = (IF_EX.imm) << 2; // Masking with 0000 0000 0000 0000 1111 1111 1111 1111(sign extended)
		EX_offset = (ID_IF_PC - 8 + offset_value);
		sign = (IF_EX.A)>>31;
		printf("\nsign=0x%08x \n", sign);
		
		if(rt == 0x00000001){ ///BGEZ-
			if ( sign == 0x00000000 || IF_EX.A == 0x00000000 ){
				bgez=1;	
			}
			else{
				bgez=0;
			}
		}
		else if (rt == 0x00000000)///BLTZ-
		{
			if ( sign == 0x00000001 ){
				bltz=1;	
			}
			else{
				bltz=0;
			}
		}
		printf("\nA = 0x%08x \n", IF_EX.A);
		printf("\nEX/MEM.imm = 0x%08x \n", IF_EX.imm);
		printf("\noffset_value = 0x%08x \n", offset_value);
		printf("\nPC+4 = 0x%08x \n",ID_IF_PC);
		printf("\nnext address = 0x%08x \n", EX_offset);
		p3++;
		break;
	
	/***************--(BLEZ- Branch on less than or Equal to zero)*****************/
		case 0x18000000:
		offset_value = (IF_EX.imm) << 2; // Masking with 0000 0000 0000 0000 1111 1111 1111 1111(sign extended)
		EX_offset = (ID_IF_PC - 8  + offset_value);
		sign = (IF_EX.A)>>31;
		if (IF_EX.A == 0x00000000 || sign == 0x00000001 )
		{
			blez=1;	
		}
		else 
		{
			blez=0;
		}
		printf("\nA = 0x%08x \n", IF_EX.A);
		printf("\nEX/MEM.imm = 0x%08x \n", IF_EX.imm);
		printf("\noffset_value = 0x%08x \n", offset_value);
		printf("\nPC+4 = 0x%08x \n",ID_IF_PC);
		printf("\nnext address = 0x%08x\n", EX_offset);
		p3++;
		break;
	/****************--(ORI- Exclusive OR immediate)******/
		case 0x34000000:
		rd = (0x001f0000 & EX_MEM.IR ) >> 16;
		EX_MEM_rd = rd;
		printf("\n EX_MEM_rd ++++++++++++++= %d", EX_MEM_rd );
		EX_MEM.ALUOutput = IF_EX.A | IF_EX.imm;
		FW_Exe=EX_MEM.ALUOutput;
		printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
		printf("\nEX/MEM.B = 0x%08x \n", IF_EX.imm);
		printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
		break;
	/****************--(XORI- Exclusive OR immediate)******/
		case 0x38000000:
		rd = (0x001f0000 & EX_MEM.IR ) >> 16;
		EX_MEM_rd = rd;
		printf("\n EX_MEM_rd ++++++++++++++= %d", EX_MEM_rd );
		EX_MEM.ALUOutput = IF_EX.A ^ IF_EX.imm;
		FW_Exe=EX_MEM.ALUOutput;
		printf("\nEX/MEM.A = 0x%08x \n", IF_EX.A);
		printf("\nEX/MEM.B = 0x%08x \n", IF_EX.imm);
		printf("\nEX/MEM.ALUOutput = 0x%08x \n", EX_MEM.ALUOutput);
		break;

	}
	}
	p2=0;
	p5=0;
}

/************************************************************/
/* instruction decode (ID) pipeline stage:                                                             */ 
/************************************************************/
void ID()
{
        IF_EX.PC = ID_IF.PC;
	uint32_t rs;
	uint32_t rt;
	uint32_t rd;
	uint32_t immediate_value;
	uint32_t immediate_value_unsign;
	uint32_t offset_value;
	uint32_t target_value;
	uint32_t immediate_sign_decode;
	uint32_t immediate_value_decode;

	if (bne==0 && beq==0 && bgtz==0 && bgez==0 && blez==0 && bltz==0 && jalr==0 && jal==0 && jr==0 && j==0){
		p4=0;}
	else{
		p4++;}	
	if (p4==2)
		p5=1;
	printf("\n decode p42=%d\n", p4);
	if (p==0 && p1==0 && p3==0 && p4==0){

    	IF_EX.IR = ID_IF.IR;
	printf("\n**************************************Decode Stage*****************************************\n");
	printf("\nID/EX.IR=0x%08x \n", IF_EX.IR);
	print_instruction(ID_PC);
	//Decode the opcode from the instruction
	uint32_t opcode = (0xFC000000 &  IF_EX.IR); 
	//printf("\nOpcode = 0x%08x ",opcode);
	
	switch (opcode) {
		
		/***********************************************************R-Type************************************************************************/
		case 0x00000000:
		//printf("\n This is a R-type instruction");
		//get the address value of rs 
		rs = (0x03E00000 & IF_EX.IR) >> 21;
		//printf("\n rs = %d", rs);
		D_rs=rs;
		//get the address value of rt
	        rt = (0x001F0000 & IF_EX.IR ) >> 16;
		//printf("\n rt = %d", rt);
		D_rt=rt;
		//get the address value of rd
		rd = (0x0000F800 & IF_EX.IR ) >> 11;
		
		//printf("\n rd = %d", rd);
		
		//get the value of shift amount
		uint32_t sa = (0x000007C0 & IF_EX.IR ) >> 6;
		//printf("\n sa = %d", sa);
		
		//decoding the function bit
		uint32_t function = (0x0000003F & IF_EX.IR ); 
		//printf("\n function = 0x%08x\n", function);
		
		switch (function) {
					
			/***********************************ALU instruction**********************************************/
			
			/*************--ADD function*******************/
			case 0x00000020:
			//printf("\n This is an ADD function");
			
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.B = CURRENT_STATE.REGS[rt];
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;
					
			/*************--ADDU function*******************/
			case 0x00000021:
			//printf("\n This is an ADD function");
			
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.B = CURRENT_STATE.REGS[rt];
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;
			/************--AND function*****************/
			case 0x00000024:
			//printf("\n This is an AND function");
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.B = CURRENT_STATE.REGS[rt];
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){                        //check  if last inst is LW we need 1 stall
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) { //just fw no stall
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;
			/************--OR function*****************/
			case 0x00000025:
			//printf("\n This is an OR function");
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.B = CURRENT_STATE.REGS[rt];
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){                        //check  if last inst is LW we need 1 stall
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) { //just fw no stall
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;

			/************--NOR function*****************/
			case 0x00000027:
			//printf("\n This is an NOR function");
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.B = CURRENT_STATE.REGS[rt];
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){                        //check  if last inst is LW we need 1 stall
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) { //just fw no stall
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;


			/****************--XOR function*************/ 
			//(if rs and rt is different, rd = 1 else if rs and rt are same then rd is 0)
			case 0x00000026:
			//printf("\n This is an XOR function");
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.B = CURRENT_STATE.REGS[rt];
			if (ENABLE_FORWARDING == 0){
				/////we need two stall for these condition
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				/////we need one  stall for these condition
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nMEM_WB_rd=%d,   EX_MEM_rd=%d\n", MEM_WB_rd, EX_MEM_rd);
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;
			
			/*************--MULTU function*******************/
				case 0x00000019:
				IF_EX.A=CURRENT_STATE.REGS[rs];
				IF_EX.B=CURRENT_STATE.REGS[rt];
				if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;
			
			/******DIVU (Divide unsigned function)****/
			case 0x0000001B:
			IF_EX.A=CURRENT_STATE.REGS[rs];
			IF_EX.B=CURRENT_STATE.REGS[rt];
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				printf("\nrt =%d and MEM_WB_rd =%d \n", rt, MEM_WB_rd);
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;
			
			/******MFHI function (Move from HI)****/
			case 0x00000010:
			IF_EX.A= to1;
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;
			
		/******MFLO function (Move from HI)****/
			case  0x00000012:
			IF_EX.A= to;
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;
			
		
		/******MTHI function (Move to HI)****/
			case  0x00000011:
			IF_EX.A= CURRENT_STATE.REGS[rs];
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;
			
		
		/******MTLO function (Move to Low)****/
			case  0x00000013:
			IF_EX.A= CURRENT_STATE.REGS[rs];
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;
			
		/*************--SLT function*******************/
			case 0x0000002A:
			//printf("\n This is an SLT function");

			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.B = CURRENT_STATE.REGS[rt];
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;

			/************--SRL function*****************/
			case 0x00000002:
			IF_EX.B = CURRENT_STATE.REGS[rt];
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{	//forwarding
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}

			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;

			/************--SRA function*****************/
			case 0x00000003:
			IF_EX.B = CURRENT_STATE.REGS[rt];
			if (ENABLE_FORWARDING == 0){
                                if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{				//forwarding
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}

			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;

			/************--SLL function*****************/
			case 0x00000000:
			IF_EX.B = CURRENT_STATE.REGS[rt];
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			 }					                           
                         else{			//forwarding
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}

			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;	
			
			
			/*************--SUB function*******************/
			case 0x00000022:
			//printf("\n This is an ADD function");

			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.B = CURRENT_STATE.REGS[rt];
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;
			


			/**************--(JALR-Jump And Link Register)*******************************/
			
			case 0x00000009:
			IF_EX.A = CURRENT_STATE.REGS[rs];
			printf("\nrs=%d, ID/EX.A = 0x%08x \n", rs, IF_EX.A);
			jump=1;
			break;

			/**************--(JR-Jump Register)*******************************/
			case 0x00000008:
			IF_EX.A = CURRENT_STATE.REGS[rs];
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			jump=1;	
			break;

			/****************--System Call function*************/ 
			case 0x0000000c:
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.B = CURRENT_STATE.REGS[rt];
			
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			break;
		
		}
		
		break;		//This is case R-type break
		
/******************************************************I-Type***********************************************************************/
		//(ADDI- Add immediate unisigned)
		case 0x20000000:
		//get the address value of rs 
		rs = (0x03E00000 & IF_EX.IR) >> 21;
		D_rs=rs;
		immediate_value_unsign = (0x0000FFFF & IF_EX.IR);
		IF_EX.A = CURRENT_STATE.REGS[rs];
		IF_EX.imm = immediate_value_unsign;
		printf("\nEX_MEM_rd=0x%08x \n ",EX_MEM_rd );
		if (ENABLE_FORWARDING == 0){
			if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
				stallflag = 1;
				A=1;
			}
			if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
				stallflag = 1;
				B=1;
			}
			if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
				stallflag = 1;
				A=1;
			}
			if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
				stallflag = 1;
				B=1;
			}
		}
		else{
			if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
				IF_EX.A=FW_Exe;
				if(FW_LW==1){
					A=1;}
					//p1=1;}
			}
			if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
				IF_EX.B=FW_Exe;
				if(FW_LW==1){
					B=1;}
					//p1=1;}
			}

			if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
				IF_EX.A=FW_Mem;
			}
			if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
				IF_EX.B=FW_Mem;
			}
		}

		printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
		printf("\nID/EX.imm = 0x%08x \n", IF_EX.imm);
		
		break;

		//(ADDIU- Add immediate unisigned)
		case 0x24000000:
		//get the address value of rs 
		rs = (0x03E00000 & IF_EX.IR) >> 21;
		//printf("\n rs = %d", rs);
		D_rs=rs;
		IF_EX.A = CURRENT_STATE.REGS[rs];
		immediate_value_decode = (0x0000FFFF & IF_EX.IR);
		immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15; 
			switch(immediate_sign_decode){

			case 0x00000001:
			immediate_value = (immediate_value_decode | 0XFFFF0000);
			break;
			
			case 0x00000000: 
			immediate_value = (immediate_value_decode | 0X000000000);
			break;

			}

			IF_EX.imm = immediate_value;

		//printf("\nEX_MEM_rd=0x%08x \n ",EX_MEM_rd );
		if (ENABLE_FORWARDING == 0){
			if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
				stallflag = 1;
				A=1;
			}
			if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
				stallflag = 1;
				B=1;
			}
			if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
				stallflag = 1;
				sh_A=1;
				sh=1;
			}
			if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
				stallflag = 1;
				sh_B=1;
				sh=1;
			}
		}
		else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}

		printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
		printf("\nID/EX.imm = 0x%08x \n", IF_EX.imm);
		
		break;
		
	/****************--(LUI- Load upper immediate)***************/	
		case 0x3C000000:
		//get the address value of rs 
		rs = (0x03E00000 & IF_EX.IR ) >> 21;
		//printf("\n rs = %d", rs);
		D_rs=rs;
		//get the immediate value and sign extend it
		immediate_value_decode = (0x0000FFFF & IF_EX.IR);
		immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15; 
			switch(immediate_sign_decode){

			case 0x00000001:
			immediate_value = (immediate_value_decode | 0XFFFF0000);
			break;
			
			case 0x00000000: 
			immediate_value = (immediate_value_decode | 0X000000000);
			break;

			}

			IF_EX.imm = immediate_value;

			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					B=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}

			printf("\nID/EX.imm = 0x%08x \n", IF_EX.imm);
			break;
		
		
		/********************--(SW- Store word)********************/
		case 0xAC000000:
		//get the address value of rs 
		rs = (0x03E00000 & IF_EX.IR) >> 21;
		//printf("\n rs = %d", rs);
		D_rs=rs;
		//get the address value of rt
		rt = (0x001F0000 & IF_EX.IR) >> 16;
		D_rt=rt;
		//get the immediate value and sign extend it
		immediate_value_decode = (0x0000FFFF & IF_EX.IR);
		immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15; 
		//printf("\n immediate_sign_decode= 0x%08x",immediate_sign_decode );
			switch(immediate_sign_decode){

			case 0x00000001:
			immediate_value = (immediate_value_decode | 0XFFFF0000);
			break;
				
			case 0x00000000: 
			immediate_value = (immediate_value_decode | 0X000000000);
			
			break;

			}
			printf("\n immediate_value = 0x%08x", immediate_value);
			if(rs == rdd){
				CURRENT_STATE.REGS[rs]=WB_Value;
			}
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.B =CURRENT_STATE.REGS[rt];
			IF_EX.imm = immediate_value;
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
					
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;
                                                }
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
			printf("\nID/EX.imm = 0x%08x \n", IF_EX.imm);
			break;
		
/********************--(SH- Store halfword)********************/
			case  0xA4000000:
			//get the address value of rs
			rs = (0x03E00000 & IF_EX.IR) >> 21;
			//printf("\n rs = %d", rs);
			D_rs=rs;
			//get the address value of rt
			rt = (0x001F0000 & IF_EX.IR) >> 16;
			D_rt=rt;
			//get the immediate value and sign extend it
			immediate_value_decode = (0x0000FFFF & IF_EX.IR);
			immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15;
				switch(immediate_sign_decode){

				case 0x00000001:
				immediate_value = (immediate_value_decode | 0XFFFF0000);
				break;

				case 0x00000000:
				immediate_value = (immediate_value_decode | 0X000000000);

				break;

				}
				printf("\n immediate_value = 0x%08x", immediate_value);
				if(rs == rdd){
					CURRENT_STATE.REGS[rs]=WB_Value;
				}
				IF_EX.A = CURRENT_STATE.REGS[rs];
				IF_EX.B =CURRENT_STATE.REGS[rt];
				IF_EX.imm = immediate_value;
				if (ENABLE_FORWARDING == 0){
					if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
						stallflag = 1;
						A=1;
					}
					if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
						stallflag = 1;
						B=1;
					}
					if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
						stallflag = 1;
						sh_A=1;
						sh=1;
					}
					if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
						stallflag = 1;
						sh_B=1;
						sh=1;
					}
				}
				else{
					if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
						IF_EX.A=FW_Exe;
						if(FW_LW==1){
							A=1;}
							//p1=1;}
					}
					if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
						IF_EX.B=FW_Exe;
						if(FW_LW==1){
							B=1;}
							//p1=1;}
					}

					if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
						IF_EX.A=FW_Mem;
					}
					if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
						IF_EX.B=FW_Mem;
					}
				}
				printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
				printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
				printf("\nID/EX.imm = 0x%08x \n", IF_EX.imm);
				break;

				/********************--(SB- Store Byte)********************/
				case  0xA0000000:
				//get the address value of rs
				rs = (0x03E00000 & IF_EX.IR) >> 21;
				//printf("\n rs = %d", rs);
				D_rs=rs;
				//get the address value of rt
				rt = (0x001F0000 & IF_EX.IR) >> 16;
				D_rt=rt;
				//get the immediate value and sign extend it
				immediate_value_decode = (0x0000FFFF & IF_EX.IR);
				immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15;
					switch(immediate_sign_decode){

					case 0x00000001:
					immediate_value = (immediate_value_decode | 0XFFFF0000);
					break;

					case 0x00000000:
					immediate_value = (immediate_value_decode | 0X000000000);

					break;

					}
					printf("\n immediate_value = 0x%08x", immediate_value);
					if(rs == rdd){
						CURRENT_STATE.REGS[rs]=WB_Value;
					}
					IF_EX.A = CURRENT_STATE.REGS[rs];
					IF_EX.B =CURRENT_STATE.REGS[rt];
					IF_EX.imm = immediate_value;
					if (ENABLE_FORWARDING == 0){
						if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
							stallflag = 1;
							A=1;
						}
						if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
							stallflag = 1;
							B=1;
						}
						if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
							stallflag = 1;
							sh_A=1;
							sh=1;
						}
						if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
							stallflag = 1;
							sh_A=1;
							sh=1;
						}
					}
					else{
						if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
							IF_EX.A=FW_Exe;
							if(FW_LW==1){
								A=1;}
								//p1=1;}
						}
						if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
							IF_EX.B=FW_Exe;
							if(FW_LW==1){
								B=1;}
								//p1=1;}
						}

						if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
							IF_EX.A=FW_Mem;
						}
						if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
							IF_EX.B=FW_Mem;
						}
					}
					printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
					printf("\nID/EX.B = 0x%08x \n", IF_EX.B);
					printf("\nID/EX.imm = 0x%08x \n", IF_EX.imm);
					break;

		
		/*****************--(LW- Load Word)******************/
		case 0x8C000000:
			//get the address value of rs 
			rs = (0x03E00000 &IF_EX.IR) >> 21;
			//printf("\n rs = %d", rs);
			D_rt=0;
			D_rs=rs;
			//get the immediate value and sign extend it
			immediate_value_decode = (0x0000FFFF &IF_EX.IR);
			immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15;
			switch(immediate_sign_decode){

			case 0x00000001:
			immediate_value = (immediate_value_decode | 0Xffff0000);
			break;
				
			case 0x00000000: 
			immediate_value = (immediate_value_decode | 0X000000000);
			break;
			}
			if(rs == rdd){
				CURRENT_STATE.REGS[rs]=WB_Value;
			}
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.imm = immediate_value;

			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}

			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.imm = 0x%08x \n", IF_EX.imm);
			break;

			/*****************--(LH- Load HalfWord)******************/
			case 0x84000000:
				//get the address value of rs
				rs = (0x03E00000 &IF_EX.IR) >> 21;
				//printf("\n rs = %d", rs);
				D_rt=0;
				D_rs=rs;
				//get the immediate value and sign extend it
				immediate_value_decode = (0x0000FFFF &IF_EX.IR);
				immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15;
				switch(immediate_sign_decode){

				case 0x00000001:
				immediate_value = (immediate_value_decode | 0Xffff0000);
				break;

				case 0x00000000:
				immediate_value = (immediate_value_decode | 0X000000000);
				break;
				}
				if(rs == rdd){
					CURRENT_STATE.REGS[rs]=WB_Value;
				}
				IF_EX.A = CURRENT_STATE.REGS[rs];
				IF_EX.imm = immediate_value;

				if (ENABLE_FORWARDING == 0){
					if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
						stallflag = 1;
						A=1;
					}
					if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
						stallflag = 1;
						B=1;
					}
					if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
						stallflag = 1;
						sh_A=1;
						sh=1;
					}
					if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
						stallflag = 1;
						sh_B=1;
						sh=1;
					}
				}
				else{
					if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
						IF_EX.A=FW_Exe;
						if(FW_LW==1){
							A=1;}
							//p1=1;}
					}
					if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
						IF_EX.B=FW_Exe;
						if(FW_LW==1){
							B=1;}
							//p1=1;}
					}

					if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
						IF_EX.A=FW_Mem;
					}
					if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
						IF_EX.B=FW_Mem;
					}
				}

				printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
				printf("\nID/EX.imm = 0x%08x \n", IF_EX.imm);
				break;

				/*****************--(LB- Load Byte)******************/
				case 0x80000000:
					//get the address value of rs
					rs = (0x03E00000 &IF_EX.IR) >> 21;
					//printf("\n rs = %d", rs);
					D_rt=0;
					D_rs=rs;
					//get the immediate value and sign extend it
					immediate_value_decode = (0x0000FFFF &IF_EX.IR);
					immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15;
					switch(immediate_sign_decode){

					case 0x00000001:
					immediate_value = (immediate_value_decode | 0Xffff0000);
					break;

					case 0x00000000:
					immediate_value = (immediate_value_decode | 0X000000000);
					break;
					}
					if(rs == rdd){
						CURRENT_STATE.REGS[rs]=WB_Value;
					}
					IF_EX.A = CURRENT_STATE.REGS[rs];
					IF_EX.imm = immediate_value;

					if (ENABLE_FORWARDING == 0){
						if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
							stallflag = 1;
							A=1;
						}
						if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
							stallflag = 1;
							B=1;
						}
						if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
							stallflag = 1;
							sh_A=1;
							sh=1;
						}
						if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
							stallflag = 1;
							sh_B=1;
							sh=1;
						}
					}
					else{
						if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
							IF_EX.A=FW_Exe;
							if(FW_LW==1){
								A=1;}
								//p1=1;}
						}
						if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
							IF_EX.B=FW_Exe;
							if(FW_LW==1){
								B=1;}
								//p1=1;}
						}

						if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
							IF_EX.A=FW_Mem;
						}
						if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
							IF_EX.B=FW_Mem;
						}
					}

					printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
					printf("\nID/EX.imm = 0x%08x \n", IF_EX.imm);
					break;


		/***************--(BEQ- Branch on Equal)*****************/
		case 0x10000000:
			//get the address value of rs 
			rs = (0x03E00000 & IF_EX.IR) >> 21;//Masking with 0000 0011 1110 0000 0000 0000 0000 0000 (03E00000)
			printf("\n rs = %d, rd = %d\n", rs, MEM_WB_rd);
		
			//get the address value of rt
			rt = (0x001F0000 & IF_EX.IR) >> 16;//Masking with 0000 0000 0001 1111 0000 0000 0000 0000 (001F0000)
			printf("\n rt = %d, rd=%d \n", rt, EX_MEM_rd);
		
			immediate_value_decode = (0x0000FFFF & IF_EX.IR);
			immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15; // Masking with 0000 0000 0000 0000 1111 1111 1111 1111
			switch(immediate_sign_decode){

			case 0x00000001:
			immediate_value = (immediate_value_decode | 0Xffff0000);
			break;
				
			case 0x00000000: 
			immediate_value = (immediate_value_decode | 0X000000000);
			break;
				
			}
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.B = CURRENT_STATE.REGS[rt];
			IF_EX.imm = (immediate_value);	
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
					
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){                        //check  if last inst is LW we need 1 stall
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) { //just fw no stall
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}						
			branch=1;
		break;


	        //(ANDI- And immediate)
		case 0x30000000:
		//get the address value of rs
		rs = (0x03E00000 & IF_EX.IR) >> 21;
		//printf("\n rs = %d", rs);
		D_rs=rs;
		immediate_value_unsign = (0x0000FFFF & IF_EX.IR);
		IF_EX.A = CURRENT_STATE.REGS[rs];
		IF_EX.imm = immediate_value_unsign;
		printf("\nEX_MEM_rd=0x%08x \n ",EX_MEM_rd );
		if (ENABLE_FORWARDING == 0){
			if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
				stallflag = 1;
				A=1;
			}
			if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
				stallflag = 1;
				B=1;
			}
			if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
				stallflag = 1;
				sh_A=1;
				sh=1;
			}
			if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
				stallflag = 1;
				sh_B=1;
				sh=1;
			}
		}
		else{
			if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
				IF_EX.A=FW_Exe;
				if(FW_LW==1){
					A=1;}
					//p1=1;}
			}
			if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
				IF_EX.B=FW_Exe;
				if(FW_LW==1){
					B=1;}
					//p1=1;}
			}

			if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
				IF_EX.A=FW_Mem;
			}
			if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
				IF_EX.B=FW_Mem;
			}
		}

		printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
		printf("\nID/EX.imm = 0x%08x \n", IF_EX.imm);

		break;

		/****************--(SLTI- Load upper immediate)***************/
		case 0x28000000:
		//get the address value of rs
		rs = (0x03E00000 & IF_EX.IR ) >> 21;
		//printf("\n rs = %d", rs);
		D_rs=rs;
		IF_EX.A = CURRENT_STATE.REGS[rs];
		//get the immediate value and sign extend it
		immediate_value_decode = (0x0000FFFF & IF_EX.IR);
		register_value = CURRENT_STATE.REGS[rs];
		immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15;
		register_sign_decode = CURRENT_STATE.REGS[rs] >> 31;
		register_value = register_value << 1;
			switch(immediate_sign_decode){
			case 00000001:
			immediate_value = (immediate_value_decode | 0Xffff0000) << 1;
			break;

			case 00000000:
			immediate_value = (immediate_value_decode | 0X000000000) << 1;
			break;
			}

			IF_EX.imm = immediate_value;

			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) {
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}

			printf("\nID/EX.imm = 0x%08x \n", IF_EX.imm);
			break;

		
		/***************--(BNE- Branch not Equal)*****************/
		case 0x14000000:
			//get the address value of rs 
			rs = (0x03E00000 & IF_EX.IR) >> 21;//Masking with 0000 0011 1110 0000 0000 0000 0000 0000 (03E00000)
			printf("\n rs = %d", rs);
		
			//get the address value of rt
			rt = (0x001F0000 & IF_EX.IR) >> 16;//Masking with 0000 0000 0001 1111 0000 0000 0000 0000 (001F0000)
			printf("\n rt = %d", rt);
		
			immediate_value_decode = (0x0000FFFF & IF_EX.IR);
			immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15; // Masking with 0000 0000 0000 0000 1111 1111 1111 1111
			switch(immediate_sign_decode){

			case 0x00000001:
			immediate_value = (immediate_value_decode | 0Xffff0000);
			break;
				
			case 0x00000000: 
			immediate_value = (immediate_value_decode | 0X000000000);
			break;
				
			}
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.B = CURRENT_STATE.REGS[rt];
			IF_EX.imm = (immediate_value);
			printf("MEM_WB_rd=%d",MEM_WB_rd);
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){                        //check  if last inst is LW we need 1 stall
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) { //just fw no stall
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}					
			branch=1;
		break;	

		/***************--(BLEZ- Branch on less than or Equal to zero)*****************/
		case 0x18000000:
		//get the address value of rs 
			rs = (0x03E00000 & IF_EX.IR) >> 21;//Masking with 0000 0011 1110 0000 0000 0000 0000 0000 (03E00000)
				
			immediate_value_decode = (0x0000FFFF & IF_EX.IR);// Masking with 0000 0000 0000 0000 1111 1111 1111 1111(sign extended)
			immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15;
			switch(immediate_sign_decode){

				case 0x00000001:
				immediate_value = (immediate_value_decode | 0Xffff0000);
				break;
				
				case 0x00000000: 
				immediate_value = (immediate_value_decode | 0X000000000);
				break;
				
			}
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.imm = (immediate_value);
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){                        //check  if last inst is LW we need 1 stall
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) { //just fw no stall
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}			
			branch=1;
			break;
		
		
		/***************--(BGEZ- Branch on Greater than or equal to zero------BLTZ- Branch on less than zero)*****************/
		case 0x04000000:
			//get the address value of rs 
			rs = (0x03E00000 & IF_EX.IR) >> 21;//Masking with 0000 0011 1110 0000 0000 0000 0000 0000 (03E00000)
			rt = (0x001F0000 & IF_EX.IR) >> 16;
			immediate_value_decode = (0x0000FFFF & IF_EX.IR); // Masking with 0000 0000 0000 0000 1111 1111 1111 1111(sign extended)
			immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15;
			switch(immediate_sign_decode){

				case 0x00000001:
				immediate_value = (immediate_value_decode | 0Xffff0000);
				break;
				
				case 0x00000000: 
				immediate_value = (immediate_value_decode | 0X000000000);
				break;
				
			}
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.imm = (immediate_value);
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			branch=1;
		break;
		
		/***************--(BGEZ- Branch on Greater than or equal to zero)*****************/
		/*case 0x04000000:
			//get the address value of rs 
			rs = (0x03E00000 & instruction) >> 21;//Masking with 0000 0011 1110 0000 0000 0000 0000 0000 (03E00000)
			immediate_value_decode = (0x0000FFFF & IF_EX.IR); // Masking with 0000 0000 0000 0000 1111 1111 1111 1111(sign extended)
			immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15;
			switch(immediate_sign_decode){

				case 0x00000001:
				immediate_value = (immediate_value_decode | 0Xfffc0000);
				break;
				
				case 0x00000000: 
				immediate_value = (immediate_value_decode | 0X000000000);
				break;
				
			}
			IF_EX.A = CURRENT_STATE.REGS[rs];
			//IF_EX.B = CURRENT_STATE.REGS[rt];
			IF_EX.imm = (immediate_value);
		
		break; 

		
		
		/**************--(BGTZ- Branch on Greater than zero)*******************************/
        	case 0x1C000000:
			rs = (0x03E00000 & IF_EX.IR) >> 21;//Masking with 0000 0011 1110 0000 0000 0000 0000 0000 (03E00000)
			immediate_value_decode = (0x0000FFFF & IF_EX.IR); // Masking with 0000 0000 0000 0000 1111 1111 1111 1111(sign extended)
			immediate_sign_decode = (0x0000FFFF & IF_EX.IR)>>15;
			switch(immediate_sign_decode){

				case 0x00000001:
				immediate_value = (immediate_value_decode | 0Xffff0000);
				break;
				
				case 0x00000000: 
				immediate_value = (immediate_value_decode | 0X000000000);
				break;
				
			}
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.imm = (immediate_value);
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					A=1;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					B=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					sh_A=1;
					sh=1;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					sh_B=1;
					sh=1;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){                        //check  if last inst is LW we need 1 stall
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) { //just fw no stall
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			branch=1;
        	break;

		/********************************************************************J-Type*******************************************************************/

		/**************--(Jump Instruction)*******************************/
		case 0x08000000:
			IF_EX.imm = (0x03FFFFFF & IF_EX.IR);// << 2; // Masking with 0000 0111 1111 1111 1111 1111 1111 1111. (This is to get the 26 bit of the instruction)
			jump=1;
		break;
		
		/**************--(JAL-Jump and link instruction)*******************************/
		case 0x0C000000:
			IF_EX.imm = (0x03FFFFFF & IF_EX.IR);//<< 2; // Masking with 0000 0111 1111 1111 1111 1111 1111 1111. (This is to get the 26 bit of the instruction)
			jump=1;
		break;
		
//=================================================================================================================================================================================================	
	

		/****************--(XORI- Exclusive OR immediate)******/
		case 0x38000000:
			//get the address value of rs 
			rs = (0x03E00000 & IF_EX.IR ) >> 21;
			//printf("\n rs = %d", rs);
			D_rs=rs;
			D_rt=0;
			immediate_value_unsign = (0x0000FFFF & IF_EX.IR);
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.imm = immediate_value_unsign;
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					IF_EX.A=WB_Value;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					IF_EX.B=WB_Value;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					IF_EX.A=WB_Value;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					IF_EX.B=WB_Value;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.imm = 0x%08x \n", IF_EX.imm);
		break;
		/****************--(ORI- Exclusive OR immediate)******/
		case 0x34000000:
			//get the address value of rs 
			rs = (0x03E00000 & IF_EX.IR ) >> 21;
			//printf("\n rs = %d", rs);
			D_rs=rs;
			D_rt=0;
			immediate_value_unsign = (0x0000FFFF & IF_EX.IR);
			IF_EX.A = CURRENT_STATE.REGS[rs];
			IF_EX.imm = immediate_value_unsign;
			if (ENABLE_FORWARDING == 0){
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					stallflag = 1;
					IF_EX.A=WB_Value;
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					stallflag = 1;
					IF_EX.B=WB_Value;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs)) {
					stallflag = 1;
					IF_EX.A=WB_Value;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt)) {
					stallflag = 1;
					IF_EX.B=WB_Value;
				}
			}
			else{
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rs)) { 
					IF_EX.A=FW_Exe;
					if(FW_LW==1){
						A=1;}
						//p1=1;}
				}
				if ((EX_MEM_rd != 0) && (EX_MEM_rd == rt)) {
					IF_EX.B=FW_Exe;
					if(FW_LW==1){
						B=1;}
						//p1=1;}
				}

				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rs) && (EX_MEM_rd != rs)) {
					IF_EX.A=FW_Mem;
				}
				if ((MEM_WB_rd != 0) && (MEM_WB_rd == rt) && (EX_MEM_rd != rt)) {
					IF_EX.B=FW_Mem;
				}
			}
			printf("\nID/EX.A = 0x%08x \n", IF_EX.A);
			printf("\nID/EX.imm = 0x%08x \n", IF_EX.imm);
		break;
		

	}

}
}

/************************************************************/
/* instruction fetch (IF) pipeline stage:                                                              */ 
/************************************************************/
void IF()
{	
	if (p ==0 && p1 ==0 && p3==0){
		if (bne==0 && beq==0 && bgtz==0 && bgez==0 && blez==0 && bltz==0 && jalr==0 && jal==0 && jr==0 && j==0){
			ID_IF.IR  = mem_read_32(CURRENT_STATE.PC);}
		else{
			CURRENT_STATE.PC=EX_offset;
			ID_IF.IR  = mem_read_32(CURRENT_STATE.PC);
			bne=0;
			beq=0;
			bgtz=0;
			bgez=0;
			blez=0;
			bltz=0;
			jalr=0;
			jal=0;
			jr=0;
			j=0;
		}
		ID_PC = CURRENT_STATE.PC;
		printf("\n**************************************Fetch Stage******************************************\n");
	    	NEXT_STATE.PC   = CURRENT_STATE.PC + 4;
		ID_IF_PC=NEXT_STATE.PC;
                ID_IF.PC=CURRENT_STATE.PC;
		// Prints the Pipeline states- current PC. 
		printf("\nCURRENT PC=0x%08x \n", CURRENT_STATE.PC );
		// Prints the Pipeline states- IR and PC. 
		printf("\nIF/ID.IR=0x%08x \n", ID_IF.IR );
		print_instruction(CURRENT_STATE.PC);
		printf("\nIF/ID.PC=0x%08x\n", NEXT_STATE.PC);
		if (stallflag ==1){
			p=1;
		}
		if(FW_LW==1)
			p1=1; 
	}
	

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
			target = (instruction & 0x03ffffff) << 2;
		char reg_name[32][6] = {"$r0", "$r1", "$r2", "$r3", "$r4","$r5", "$r6", "$r7", "$r8","$r9", "$r10", "$r11", "$r12", "$r13", "$r14", "$r15", "$r16", "$r17", "$r18", "$r19", "$r20", "$r21", "$r22", "$r23", "$r24", "$r25", "$r26", "$r27", "$r28", "$r29", "$r30", "$r31"};//{"$zero", "$at", "$v0", "$v1", "$a0","$a1", "$a2", "$a3", "$t0","$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
	if(opcode == 0){ //R-type Instructions all have 000000 opcode
		switch(function){ //determine function code which is from bit 0-5
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

			case(0x04): //BEQ-
				printf("beq %s %s %x", reg_name[rs], reg_name[rt], immediate << 2);
				break;
			case(0x05): //BNE-
				printf("bne %s %s %x", reg_name[rs], reg_name[rt], immediate << 2);
				break;
			case(0x06): //BLEZ-
				printf("blez %s %x", reg_name[rs], immediate);
				break;
			case(0x01): //BLTZ and BGEZ- have same opcode, rt section is different
				if(rt == 0){ //BLTZ
					printf("bltz %s %x", reg_name[rs], immediate << 2);
				}
				else{ //BGEZ rt == 1
					printf("bgez %s %x", reg_name[rs], immediate << 2);
				}
				break;
			case(0x07): //BGTZ-
				printf("bgtz %s %x", reg_name[rs], immediate << 2);
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

/************************************************************/
/*Print the cache contents*/
/************************************************************/
void print_cache(){
	for(int i = 0; i < NUM_CACHE_BLOCKS; ++i){
		printf("Cache block %d: ", i);
		for(j = 0; j < WORD_PER_BLOCK; ++j){
			printf("%x ", L1Cache.blocks[i].words[j]);
			
		}
		printf("\n");
		
	}
	printf("Cache Hits: %d\nCache Misses: %d\n", cache_hits, cache_misses);
	
	
	
}
/************************************************************/
/* Print the current pipeline                                                                                    */ 
/************************************************************/
void show_pipeline(){
	/*IMPLEMENT THIS*/
int i;
for(i=0; i<PROGRAM_SIZE; i++){
printf("\n____________________________________________________________________________________________________________________________\n");
WB();
MEM();
EX();
ID();
IF();
CURRENT_STATE = NEXT_STATE;
}
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
