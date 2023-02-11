#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

struct reg_name { // register names as numeric values
    long $zero = 0;
    long $at = 1;
    long $v0 = 2;
    long $v1 = 3;
    long $a0 = 4;
    long $a1 = 5;
    long $a2 = 6;
    long $a3 = 7;
    long $t0 = 8;
    long $t1 = 9;
    long $t2 = 10;
    long $t3 = 11;
    long $t4 = 12;
    long $t5 = 13;
    long $t6 = 14;
    long $t7 = 15;
    long $s0 = 16;
    long $s1 = 17;
    long $s2 = 18;
    long $s3 = 19;
    long $s4 = 20;
    long $s5 = 21;
    long $s6 = 22;
    long $s7 = 23;
    long $t8 = 24;
    long $t9 = 25;
    long $k0 = 26;
    long $k1 = 27;
    long $gp = 28;
    long $sp = 29;
    long $fp = 30;
    long $ra = 31;
};
    
struct Rtype { //function codes for R-type instructions
    long add = 0x20;
    long addu = 0x21;
    long sub = 0x22;
    long subu = 0x23;
    long mult = 0x18;
    long multu = 0x19;
    long div = 0x1A;
    long divu = 0x1B;
    long or = 0x25;
    long xor = 0x26;
    long nor = 0x27;
    long sll = 0x00;
    long srl = 0x02;
    long sra = 0x03;
    long slt = 0x2A;
    long mfhi = 0x10;
    long mthi = 0x11;
    long mflo = 0x12;
    long mtlo = 0x13;
    long and = 0x14;
    long jr = 0x08;
    long jalr = 0x09;
    long syscall = 0x0C;
};

struct Itype { //opcodes for I-type instructions
    long bltz = 0x01;
    long bgez = 0x01;
    long j = 0x02;
    long jal = 0x03;
    long beq = 0x04;
    long bne = 0x05;
    long blez = 0x06;
    long bgtz = 0x07;
    long addi = 0x08;
    long addiu = 0x09;
    long slti = 0x0A;
    long andi = 0x0C;
    long ori = 0x0D;
    long xori = 0x0E;
    long lui = 0x0F;
    long lb = 0x20;
    long lh = 0x21;
    long lw = 0x23;
    long sb = 0x28;
    long sh = 0x29;
    long sw = 0x2B;
};

struct shift { //shift amounts for each section
    int opcode = 26;
    int rs = 21;
    int rt = 16;
    int rd = 11;
    int shamt = 6; //included, but all implemented commands have 0x00 in this segment
}
    
    
int main(){
    FILE* fp;
    
