#include <stdint.h>

#define FALSE 0
#define TRUE  1

/******************************************************************************/
/* MIPS memory layout                                                                                                                                      */
/******************************************************************************/
#define MEM_TEXT_BEGIN  0x00400000
#define MEM_TEXT_END      0x0FFFFFFF
/*Memory address 0x10000000 to 0x1000FFFF access by $gp*/
#define MEM_DATA_BEGIN  0x10010000
#define MEM_DATA_END   0x7FFFFFFF

#define MEM_KTEXT_BEGIN 0x80000000
#define MEM_KTEXT_END  0x8FFFFFFF

#define MEM_KDATA_BEGIN 0x90000000
#define MEM_KDATA_END  0xFFFEFFFF

/*stack and data segments occupy the same memory space. Stack grows backward (from higher address to lower address) */
#define MEM_STACK_BEGIN 0x7FFFFFFF
#define MEM_STACK_END  0x10010000

typedef struct {
	uint32_t begin, end;
	uint8_t *mem;
} mem_region_t;

/* memory will be dynamically allocated at initialization */
mem_region_t MEM_REGIONS[] = {
	{ MEM_TEXT_BEGIN, MEM_TEXT_END, NULL },
	{ MEM_DATA_BEGIN, MEM_DATA_END, NULL },
	{ MEM_KDATA_BEGIN, MEM_KDATA_END, NULL },
	{ MEM_KTEXT_BEGIN, MEM_KTEXT_END, NULL }
};

#define NUM_MEM_REGION 4
#define MIPS_REGS 32

typedef struct CPU_State_Struct {

  uint32_t PC;		                   /* program counter */
  uint32_t REGS[MIPS_REGS]; /* register file. */
  uint32_t HI, LO;                          /* special regs for mult/div. */
} CPU_State;

typedef struct CPU_Pipeline_Reg_Struct{
	uint32_t PC;
	uint32_t IR;
	uint32_t A;
	uint32_t B;
	uint32_t imm;
	uint32_t ALUOutput;
	double ALUdiv;
	uint32_t ALUOutput1;
	uint32_t LMD;
	
} CPU_Pipeline_Reg;

/***************************************************************/
/* CPU State info.                                                                                                               */
/***************************************************************/

CPU_State CURRENT_STATE, NEXT_STATE;
int RUN_FLAG;	/* run flag*/
uint32_t ID_PC;
uint32_t ID_IF_PC;
uint32_t register_value;
uint32_t register_sign_decode;
uint32_t immediate_sign_decode;
uint32_t EX_offset;
uint32_t EX_MEM_rd;
uint32_t MEM_WB_rd;
uint32_t FW_Mem;
uint32_t FW_Mem1;
uint32_t FW_Exe;
uint32_t FW_Exe1;
uint32_t WB_Value;
uint32_t WB_Value_1;
uint32_t offset_value;
int stallflag = 0;
uint32_t D_rs;
uint32_t D_rt;
uint32_t to,to1;
uint32_t rd1;
uint32_t rdd;
uint32_t function;
uint32_t LH_mem_value;
uint32_t SH_rt_mask;
int ENABLE_FORWARDING=0;
int zero;
int FW_LW=0;
int A=0;
int B=0;
int sh=0;
int sh_A=0;
int sh_B=0;
int p=0;
int p1=0;
int p2=0;
int p3=0;
int p4=0;
int p5=0;
int beq=0;
int be=0;
int bne=0;
int bnq=0;
int bgez=0;
int blez=0;
int bgtz=0;
int bltz=0;
int j=0;
int jr=0;
int jal=0;
int jalr=0;
int branch=0;
int jump=0;
int aa;
int write=0;
uint32_t Boff, Woff, tag, Index;
uint32_t INSTRUCTION_COUNT;
uint32_t CYCLE_COUNT;
uint32_t PROGRAM_SIZE; /*in words*/


/***************************************************************/
/* Pipeline Registers.                                                                                                        */
/***************************************************************/
CPU_Pipeline_Reg ID_IF;
CPU_Pipeline_Reg IF_EX;
CPU_Pipeline_Reg EX_MEM;
CPU_Pipeline_Reg MEM_WB;

char prog_file[32];


/***************************************************************/
/* Function Declerations.                                                                                                */
/***************************************************************/
void help();
uint32_t mem_read_32(uint32_t address);
void mem_write_32(uint32_t address, uint32_t value);
void cycle();
void run(int num_cycles);
void runAll();
void mdump(uint32_t start, uint32_t stop) ;
void rdump();
void handle_command();
void reset();
void init_memory();
void load_program();
uint32_t checking_cache( uint32_t address);
void handle_pipeline(); /*IMPLEMENT THIS*/
void WB();/*IMPLEMENT THIS*/
void MEM();/*IMPLEMENT THIS*/
void EX();/*IMPLEMENT THIS*/
void ID();/*IMPLEMENT THIS*/
void IF();/*IMPLEMENT THIS*/
void show_pipeline();/*IMPLEMENT THIS*/
void initialize();
void print_program(); /*IMPLEMENT THIS*/

