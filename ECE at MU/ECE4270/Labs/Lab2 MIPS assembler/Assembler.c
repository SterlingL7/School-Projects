/* MIPS Assembler for ECE4270 Lab 2
 * Sterling LaBarbera and Max Willoughby
 * 
 * This program takes the input file name and output file name as arguments.
 * it scans through each line of the input file and processes them into MIPS
 * machine code. each instruction is written to the output file.
 * 
 * use: ./Assembler <input file name> <output file name>
 * when executing this program.
 * 
 * 
 */


#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

// parse register names using if-else
// register names as numeric values
int parse_register(char* reg_name) { 
	char* reg_names[] = {"$zero", "$at", "$v0", "$v1", "$a0","$a1", "$a2", "$a3", "$t0","$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
    int reg_value = -1;

	if(strcmp(reg_name, reg_names[0]) == 0){
		reg_value = 0x0;
		}
	else if(strcmp(reg_name, reg_names[1]) == 0){
		reg_value = 0x1;
		}
	else if(strcmp(reg_name, reg_names[2]) == 0){
		reg_value = 0x2;
		}
	else if(strcmp(reg_name, reg_names[3]) == 0){
		reg_value = 0x3;
		}
	else if(strcmp(reg_name, reg_names[4]) == 0){
		reg_value = 0x4;
		}
	else if(strcmp(reg_name, reg_names[5]) == 0){
		reg_value = 0x5;
		}
	else if(strcmp(reg_name, reg_names[6]) == 0){
		reg_value = 0x6;
		}
	else if(strcmp(reg_name, reg_names[7]) == 0){
		reg_value = 0x7;
		}
	else if(strcmp(reg_name, reg_names[8]) == 0){
		reg_value = 0x8;
		}
	else if(strcmp(reg_name, reg_names[9]) == 0){
		reg_value = 0x9;
		}
	else if(strcmp(reg_name, reg_names[10]) == 0){
		reg_value = 0xA;
		}
	else if(strcmp(reg_name, reg_names[11]) == 0){ 
		reg_value = 0xB;
		}
	else if(strcmp(reg_name, reg_names[12]) == 0){
		reg_value = 0xC;
		}
	else if(strcmp(reg_name, reg_names[13]) == 0){
		reg_value = 0xD;
		}
	else if(strcmp(reg_name, reg_names[14]) == 0){
		reg_value = 0xE;
		}
	else if(strcmp(reg_name, reg_names[15]) == 0){
		reg_value = 0xF;
		}
	else if(strcmp(reg_name, reg_names[16]) == 0){
		reg_value = 0x10;
		}
	else if(strcmp(reg_name, reg_names[17]) == 0){
		reg_value = 0x11;
		}
	else if(strcmp(reg_name, reg_names[18]) == 0){
		reg_value = 0x12;
		}
	else if(strcmp(reg_name, reg_names[19]) == 0){
		reg_value = 0x13;
		}
	else if(strcmp(reg_name, reg_names[20]) == 0){
		reg_value = 0x14;
		}
	else if(strcmp(reg_name, reg_names[21]) == 0){
		reg_value = 0x15;
		}
	else if(strcmp(reg_name, reg_names[22]) == 0){
		reg_value = 0x16;
		}
	else if(strcmp(reg_name, reg_names[23]) == 0){
		reg_value = 0x17;
		}
	else if(strcmp(reg_name, reg_names[24]) == 0){
		reg_value = 0x18;
		}
	else if(strcmp(reg_name, reg_names[25]) == 0){
		reg_value = 0x19;
		}
	else if(strcmp(reg_name, reg_names[26]) == 0){
		reg_value = 0x1A;
		}
	else if(strcmp(reg_name, reg_names[27]) == 0){
		reg_value = 0x1B;
		}
	else if(strcmp(reg_name, reg_names[28]) == 0){
		reg_value = 0x1C;
		}
	else if(strcmp(reg_name, reg_names[29]) == 0){
		reg_value = 0x1D;
		}
	else if(strcmp(reg_name, reg_names[30]) == 0){
		reg_value = 0x1E;
		}
	else if(strcmp(reg_name, reg_names[31]) == 0){
		reg_value = 0x1F;
		}
	else{
		printf("Unrecognized register name\n");
	}

	return reg_value;
}

//shift values for each segment
struct shift { //shift amounts for each section
    int opcode;
    int rs;
    int rt;
    int rd;
    int shamt;
};
typedef struct shift Shift;

/* parse_line
 * 
 * instructions parsed into integer values based on opcode
 * 
 * this function uses the opcode to determine how many tokens to parse,
 * and the order to place them in the output 
 * strtok(char* target, char* delemiters)
 * splits strings at the specified delemiters
 * successive calls use NULL for the target since the function
 * has internal pointer to the next character in string
 * 
 * using the tokens and parse_register()
 * convert the strings to appropriate hex values for machine code
 * the immediate, target, and shift values are processed with strtol() typecast to int
 * returns the final instruction value, or -1 if error.
 */
int parse_line(char* line, Shift* sh){
	char* token;
	char* instruction; //longest instruction: SYSCALL

    int opcode = -1;

    int r1 = 0;
    int r2 = 0;
    int r3 = 0;
	char* op[] = {"add","addu","sub","subu","mult","multu","div","divu","and","or",
			"xor","nor","sll","srl","sra","slt","mfhi","mthi","mflo","mtlo",
			"jr","jalr","syscall\n","bltz","bgez","j","jal","beq","bne","blez",
			"bgtz","addi","addiu","slti","andi","ori","xori","lui","lb","lh",
			"lw","sb","sh","sw"};
			
    token = strtok(line, " ");
    instruction = token;
    
    //opcodes for R-type instructions
	if(strcmp(instruction, op[0]) == 0){
		opcode = 0x20;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = parse_register(strtok(NULL, ", \n"));
		
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, op[1]) == 0){
		opcode = 0x21;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = parse_register(strtok(NULL, ", \n"));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, op[2]) == 0){
		opcode = 0x22;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = parse_register(strtok(NULL, ", \n"));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, op[3]) == 0){
		opcode = 0x23;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = parse_register(strtok(NULL, ", \n"));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, op[4]) == 0){
		opcode = 0x18;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", \n"));
                
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | (r2 << sh->rt);
                
	}
	else if(strcmp(instruction, op[5]) == 0){
		opcode = 0x19;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", \n"));
                
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | (r2 << sh->rt);
                
	}
	else if(strcmp(instruction, op[6]) == 0){
		opcode = 0x1A;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", \n"));
                
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | (r2 << sh->rt);
        
	}
	else if(strcmp(instruction, op[7]) == 0){
		opcode = 0x1B;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", \n"));
                
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | (r2 << sh->rt);
        
	}
	else if(strcmp(instruction, op[8]) == 0){
        opcode = 0x14;
        
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = parse_register(strtok(NULL, ", \n"));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
	}
	else if(strcmp(instruction, op[9]) == 0){
		opcode = 0x25;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = parse_register(strtok(NULL, ", \n"));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, op[10]) == 0){
		opcode = 0x26;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = parse_register(strtok(NULL, ", \n"));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, op[11]) == 0){
		opcode = 0x27;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = parse_register(strtok(NULL, ", \n"));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, op[12]) == 0){
		opcode = 0x00;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, ", "),NULL, 16);
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rt);
        opcode = opcode | (r3 << sh->shamt);
        
	}
	else if(strcmp(instruction, op[13]) == 0){
		opcode = 0x02;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, ", "),NULL, 16);
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rt);
        opcode = opcode | (r3 << sh->shamt);
        
	}
	else if(strcmp(instruction, op[14]) == 0){
		opcode = 0x03;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, ", "),NULL, 16);
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rt);
        opcode = opcode | (r3 << sh->shamt);
        
	}
	else if(strcmp(instruction, op[15]) == 0){
		opcode = 0x2A;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, ", "),NULL, 16);
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rt);
        opcode = opcode | (r3 << sh->shamt);
        
	}
    else if(strcmp(instruction, op[16]) == 0){
		opcode = 0x10;
        r1 = parse_register(strtok(NULL, ", \n"));
        
        opcode = opcode | (r1 << sh->rd);
        
	}
    else if(strcmp(instruction, op[17]) == 0){
		opcode = 0x11;
        r1 = parse_register(strtok(NULL, ", \n"));
                
        opcode = opcode | (r1 << sh->rs);
        
	}
    else if(strcmp(instruction, op[18]) == 0){
		opcode = 0x12;
        r1 = parse_register(strtok(NULL, ", \n"));
        
        opcode = opcode | (r1 << sh->rd);
        
	}
    else if(strcmp(instruction, op[19]) == 0){
		opcode = 0x13;
        r1 = parse_register(strtok(NULL, ", \n"));
        
        opcode = opcode | (r1 << sh->rs);
        
	}
    else if(strcmp(instruction, op[20]) == 0){
		opcode = 0x08;
        r1 = parse_register(strtok(NULL, ", \n"));
        
        opcode = opcode | (r1 << sh->rs);
        
	}
    else if(strcmp(instruction, op[21]) == 0){
		opcode = 0x09;
        r1 = parse_register(strtok(NULL, ", \n"));
		char* temp = strtok(NULL, ", \n");
		//printf("%s", temp);
        if(temp != NULL){
			r2 = parse_register(temp);
            opcode = opcode | (r2 << sh->rs);
			opcode = opcode | (r1 << sh->rd);
		}
        else{
            opcode = opcode | (r1 << sh->rs);
		}       
	}
    else if(strcmp(instruction, op[22]) == 0){
		opcode = 0x0c;
        
	}

	//opcodes for I,Jl-type instructions set ins_type to 1
	else if(strcmp(instruction, op[23]) == 0){
		opcode = 0x01;
        
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, " "), NULL, 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, op[24]) == 0){
		opcode = 0x01;
        
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, " "), NULL, 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | (0x01 << sh->rt);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, op[25]) == 0){
		opcode = 0x02;
		
        r1 = (int)strtol(strtok(NULL, " "), NULL, 16);
        opcode = opcode << sh->opcode;
        opcode = opcode | r1;
		}
	else if(strcmp(instruction, op[26]) == 0){
		opcode = 0x03;

        r1 = (int)strtol(strtok(NULL, " "), NULL, 16);
        opcode = opcode << sh->opcode;
        opcode = opcode | r1;
		}
	else if(strcmp(instruction, op[27]) == 0){
		opcode = 0x04;
       
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), NULL, 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction, op[28]) == 0){
		opcode = 0x05;
        
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), NULL, 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction, op[29]) == 0){
		opcode = 0x06;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, " "), NULL, 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction,  op[30]) == 0){
		opcode = 0x07;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, " "), NULL, 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction,  op[31]) == 0){
		opcode = 0x08;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), NULL, 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction,  op[32]) == 0){
		opcode = 0x09;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), NULL, 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction, op[33]) == 0){
		opcode = 0x0A;
				
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), NULL, 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction, op[34]) == 0){
		opcode = 0x0C;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), NULL, 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction, op[35]) == 0){
		opcode = 0x0D;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), NULL, 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction, op[36]) == 0){
		opcode = 0x0E;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), NULL, 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction, op[37]) == 0){
		opcode = 0x0F;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, " "), NULL, 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, op[38]) == 0){
		opcode = 0x20;
			//printf("%x", opcode);
        r1 = parse_register(strtok(NULL, ", "));
			//printf("%x", opcode);
        r2 = (int)strtol(strtok(NULL, "("), NULL, 16);
			//printf("%x", opcode);
        r3 = parse_register(strtok(NULL, "), \n"));
        	//printf("%x", opcode);
		
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r3 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, op[39]) == 0){
		opcode = 0x21;

        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, "("), NULL, 16);
        r3 = parse_register(strtok(NULL, "), \n"));
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r3 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, op[40]) == 0){
		opcode = 0x23;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, "("), NULL, 16);
        r3 = parse_register(strtok(NULL, "), \n"));
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r3 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, op[41]) == 0){
		opcode = 0x28;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, "("), NULL, 16);
        r3 = parse_register(strtok(NULL, "), \n"));
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r3 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, op[42]) == 0){
		opcode = 0x29;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, "("), NULL, 16);
        r3 = parse_register(strtok(NULL, "), \n"));
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r3 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, op[43]) == 0){
		opcode = 0x2B;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, "("), NULL, 16);
        r3 = parse_register(strtok(NULL, "), \n"));
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r3 << sh->rs);
        opcode = opcode | r2;
		}
	else{
		printf("invalid instruction entered\n");
	}
    return opcode;
}

int main(int argc, char* argv[]){
	
	FILE* fp;
	FILE* of;

	if(argc == 3){ //should have exactly 3 arguments
		fp = fopen(argv[1], "r");
		if (fp == NULL){ //check for existing input file
			printf("\nerror, cannot find input file.\n");
			exit(-1);
		}
		of = fopen(argv[2], "w");
		if (of == NULL){ //check that output file opened successfully
			printf("\nerror, cannot find output file.\n");
			exit(-1);
		}
	}
	else{
		printf("\ncorrect usage: ./Assembler <assembly file name> <output file name>\n");
		exit(-1);
	}
		
	Shift s;
	Shift* sh = &s;
	s.opcode = 26;
    s.rs = 21;
    s.rt = 16;
    s.rd = 11;
    s.shamt = 6;
	
	//printf("%x\n", fp);
	//printf("%x\n", of);
	
	char line[30];
	int lcount = 0;
	int machine_code = -1;
	
	//read line, process and write to output file
	
    while((ferror(fp) == 0) && fgets(line, 30, fp)){
        ++lcount;
		
		printf("%s\n", line);
		machine_code = parse_line(line, sh);

        //functions return -1 if invalid register or instruction name
        if(machine_code == -1){//report error line and quit
            printf("Assembling Aborted, invalid code at line %d\n", lcount);
            break;
        }
        else{//write line to machine code file
			printf("%x\n", machine_code);
			
            fprintf(of,"%x\n", machine_code);
        }
	}
	
	printf("closing files ");
	int close1 = fclose(fp); //returns non-zero if error
	printf("%d | ", close1);
	
    int close2 = fclose(of);
	printf("%d\n", close2);
	
	return 0;
}
	
