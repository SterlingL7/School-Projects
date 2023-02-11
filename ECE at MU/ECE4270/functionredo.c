// r1 = parse_register(rs);
// r2 = parse_register(rt);
// r3 = parse_register(rd);
// 
// parseline()
//     if-else(op);
//     
//     
    
int parse_line(char* line, Shift* sh){
	char* token;
	char* instruction; //longest instruction: SYSCALL

    int opcode = 0x0;

    int r1 = 0;
    int r2 = 0;
    int r3 = 0;

    int type = 0;
    token = strtok(line, " ");
    instruction = token;
	puts(instruction);
    
    //opcodes for R-type instructions
	if(strcmp(instruction, "add")){
		opcode = 0x20;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = parse_register(strok(NULL, ", "));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, "addu")){
		opcode = 0x21;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = parse_register(strok(NULL, ", "));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, "sub")){
		opcode = 0x22;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = parse_register(strok(NULL, ", "));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, "subu")){
		opcode = 0x23;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = parse_register(strok(NULL, ", "));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, "mult")){
		opcode = 0x18;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
                
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | (r2 << sh->rt);
                
	}
	else if(strcmp(instruction, "multu")){
		opcode = 0x19;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
                
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | (r2 << sh->rt);
                
	}
	else if(strcmp(instruction, "div")){
		opcode = 0x1A;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
                
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | (r2 << sh->rt);
        
	}
	else if(strcmp(instruction, "divu")){
		opcode = 0x1B;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
                
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | (r2 << sh->rt);
        
	}
	else if(strcmp(instruction, "and")){
        opcode = 0x14;
        
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = parse_register(strok(NULL, ", "));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        {
	else if(strcmp(instruction, "or")){
		opcode = 0x25;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = parse_register(strok(NULL, ", "));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, "xor")){
		opcode = 0x26;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = parse_register(strok(NULL, ", "));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, "nor")){
		opcode = 0x27;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = parse_register(strok(NULL, ", "));
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | (r3 << sh->rt);
        
	}
	else if(strcmp(instruction, "sll")){
		opcode = 0x00;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = (int)strtol(strok(NULL, ", "), 16);
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rt);
        opcode = opcode | (r3 << sh->shamt);
        
	}
	else if(strcmp(instruction, "srl")){
		opcode = 0x02;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = (int)strtol(strok(NULL, ", "), 16);
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rt);
        opcode = opcode | (r3 << sh->shamt);
        
	}
	else if(strcmp(instruction, "sra")){
		opcode = 0x03;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = (int)strtol(strok(NULL, ", "), 16);
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rt);
        opcode = opcode | (r3 << sh->shamt);
        
	}
	else if(strcmp(instruction, "slt")){
		opcode = 0x2A;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = (int)strtol(strok(NULL, ", "), 16);
        
        opcode = opcode | (r1 << sh->rd);
        opcode = opcode | (r2 << sh->rt);
        opcode = opcode | (r3 << sh->shamt);
        
	}
    else if(strcmp(instruction, "mfhi")){
		opcode = 0x10;
        r1 = parse_register(strtok(NULL, ", "));
        
        opcode = opcode | (r1 << sh->rd);
        
	}
    else if(strcmp(instruction, "mthi")){
		opcode = 0x11;
        r1 = parse_register(strtok(NULL, ", "));
                
        opcode = opcode | (r1 << sh->rs);
        
	}
    else if(strcmp(instruction, "mflo")){
		opcode = 0x12;
        r1 = parse_register(strtok(NULL, ", "));
        
        opcode = opcode | (r1 << sh->rd);
        
	}
    else if(strcmp(instruction, "mtlo")){
		opcode = 0x13;
        r1 = parse_register(strtok(NULL, ", "));
        
        opcode = opcode | (r1 << sh->rs);
        
	}
    else if(strcmp(instruction, "jr")){
		opcode = 0x08;
        r1 = parse_register(strtok(NULL, ", "));
        
        opcode = opcode | (r1 << sh->rs);
        
	}
    else if(strcmp(instruction, "jalr")){
		opcode = 0x09;
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));

        if(r2 != NULL){
            opcode = opcode | (r1 << sh->rd);
            opcode = opcode | (r2 << sh->rs);
        }
        else opcode = opcode | (r1 << sh->rs);
        
	}
    else if(strcmp(instruction, "syscall")){
		opcode = 0x0c;
        
	}

	//opcodes for I,Jl-type instructions set ins_type to 1
	else if(strcmp(instruction, "bltz")){
		opcode = 0x01;
        
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, " "), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, "bgez")){
		opcode = 0x01;
        
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, " "), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | (0x01 << sh->rt);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, "j")){
		opcode = 0x02;
		
        r1 = (int)strtol(strtok(NULL, " "), 16);
        opcode = opcode << sh.opcode;
        opcode = opcode | r1;
		}
	else if(strcmp(instruction, "jal")){
		opcode = 0x03;

        r1 = (int)strtol(strtok(NULL, " "), 16);
        opcode = opcode << sh.opcode;
        opcode = opcode | r1;
		}
	else if(strcmp(instruction, "beq")){
		opcode = 0x04;
       
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction, "bne")){
		opcode = 0x05;
        
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction, "blez")){
		opcode = 0x06;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, " "), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction,  "bgtz")){
		opcode = 0x07;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, " "), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction,  "addi")){
		opcode = 0x08;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction,  "addiu")){
		opcode = 0x09;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction, "slti")){
		opcode = 0x0A;
		
		}
	else if(strcmp(instruction, "andi")){
		opcode = 0x0C;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction, "ori")){
		opcode = 0x0D;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction, "xori")){
		opcode = 0x0E;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = parse_register(strtok(NULL, ", "));
        r3 = (int)strtol(strtok(NULL, " "), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r2 << sh->rs);
        opcode = opcode | r3;
		}
	else if(strcmp(instruction, "lui")){
		opcode = 0x0F;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, " "), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, "lb")){
		opcode = 0x20;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, "("), 16);
        r3 = (int)strtol(strtok(NULL, ")"), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r3 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, "lh")){
		opcode = 0x21;

        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, "("), 16);
        r3 = (int)strtol(strtok(NULL, ")"), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r3 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, "lw")){
		opcode = 0x23;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, "("), 16);
        r3 = (int)strtol(strtok(NULL, ")"), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r3 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, "sb")){
		opcode = 0x28;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, "("), 16);
        r3 = (int)strtol(strtok(NULL, ")"), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r3 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, "sh")){
		opcode = 0x29;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, "("), 16);
        r3 = (int)strtol(strtok(NULL, ")"), 16);
        
        opcode = opcode << sh->opcode;
        opcode = opcode | (r1 << sh->rt);
        opcode = opcode | (r3 << sh->rs);
        opcode = opcode | r2;
		}
	else if(strcmp(instruction, "sw")){
		opcode = 0x2B;
		
        r1 = parse_register(strtok(NULL, ", "));
        r2 = (int)strtol(strtok(NULL, "("), 16);
        r3 = (int)strtol(strtok(NULL, ")"), 16);
        
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
