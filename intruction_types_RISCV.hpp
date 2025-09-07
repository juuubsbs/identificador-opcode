#include <iostream>
#include <cstdint>
#include <fstream>
#include <map>
#include <string>
using namespace std;

// --------------------- DEFINIÇÕES DE MÁSCARAS ---------------------
//Usei nomes distintos para as duas ocorrências de imm[11] (uma no formato B 
//e outra no J) para evitar ambiguidade: IMM_11_B (bit 7) e IMM_11_J (bit 20).
//IMM_12 (B-type) e IMM_20 (J-type) apontam para o mesmo bit físico ([31]) — por
// isso a mesma máscara 0x80000000.

#define OPCODE   0x7F     // bits [6:0]
#define RD       0xF80    // bits [11:7]
#define FUNCT3   0x7000   // bits [14:12]
#define RS1      0xF8000  // bits [19:15]
#define RS2      0x1F00000 // bits [24:20]
#define FUNCT7   0xFE000000 // bits [31:25]

#define IMM_11_0   0xFFF00000 // bits [31:20]  (imm[11:0], I-type)
#define IMM_11_5   0xFE000000 // bits [31:25]  (imm[11:5], S-type)
#define IMM_4_0    0xF80      // bits [11:7]   (imm[4:0], S-type)
#define IMM_12     0x80000000 // bit  [31]     (imm[12], B-type)
#define IMM_10_5   0x7E000000 // bits [30:25]  (imm[10:5], B-type)
#define IMM_4_1    0xF00      // bits [11:8]   (imm[4:1], B-type)
#define IMM_11_B   0x80       // bit  [7]      (imm[11], B-type)
#define IMM_31_12  0xFFFFF000 // bits [31:12]  (imm[31:12], U-type)
#define IMM_20     0x80000000 // bit  [31]     (imm[20], J-type)  
#define IMM_10_1   0x7FE00000 // bits [30:21]  (imm[10:1], J-type)
#define IMM_11_J   0x00100000 // bit  [20]     (imm[11], J-type) 
#define IMM_19_12  0xFF000    // bits [19:12]  (imm[19:12], J-type)


// --------------------- ESTRUTURA PARA REGISTRADORES ---------------------
struct Regs {
    int rd;
    int rs1;
    int rs2;
};

// --------------------- IDENTIFICAÇÃO DE TIPO ---------------------
string opcode_identifier(uint32_t opcode){
    switch(opcode){
        case 0x33: return "Tipo R";
        case 0x67: case 0x03: case 0x13: case 0x0F: case 0x73: return "Tipo I";
        case 0x23: return "Tipo S";
        case 0x63: return "Tipo B";
        case 0x37: case 0x17: return "Tipo U";
        case 0x6F: return "Tipo J";
        default: return "Tipo desconhecido";
    }
}

// --------------------- FUNÇÕES AUXILIARES ---------------------
uint32_t isolate_mask(uint32_t current_hexa, uint32_t itembit ){
    uint32_t result = current_hexa & itembit;
    return result;
}

int get_field(uint32_t instr, uint32_t mask, int shift){
    return (instr & mask) >> shift;
}

// --------------------- IDENTIFICAÇÃO DE OPERAÇÃO ---------------------
string R_type_instruction(uint32_t funct7, uint32_t funct3){
    if (funct3 == 0x0 && funct7 == 0x00) return "add";
    if (funct3 == 0x0 && funct7 == 0x20) return "sub";
    if (funct3 == 0x7 && funct7 == 0x00) return "and";
    if (funct3 == 0x6 && funct7 == 0x00) return "or";
    if (funct3 == 0x4 && funct7 == 0x00) return "xor";
    if (funct3 == 0x1 && funct7 == 0x00) return "sll";
    if (funct3 == 0x5 && funct7 == 0x00) return "srl";
    if (funct3 == 0x5 && funct7 == 0x20) return "sra";
    if (funct3 == 0x2 && funct7 == 0x00) return "slt";
    if (funct3 == 0x3 && funct7 == 0x00) return "sltu";
    return "R-type desconhecida";
}

string i_type_instruction(uint32_t opcode, uint32_t funct3){
    if(opcode == 0x13){ // ALU imediata
        if(funct3 == 0x0) return "addi";
        if(funct3 == 0x2) return "slti";
        if(funct3 == 0x3) return "sltiu";
        if(funct3 == 0x4) return "xori";
        if(funct3 == 0x6) return "ori";
        if(funct3 == 0x7) return "andi";
        if(funct3 == 0x1) return "slli";
        if(funct3 == 0x5) return "srli/srai";
    }
    if(opcode == 0x03){ // Loads
        if(funct3 == 0x0) return "lb";
        if(funct3 == 0x1) return "lh";
        if(funct3 == 0x2) return "lw";
        if(funct3 == 0x4) return "lbu";
        if(funct3 == 0x5) return "lhu";
    }
    if(opcode == 0x67) return "jalr";
    if(opcode == 0x0F) return "fence";
    if(opcode == 0x73) return "ecall/ebreak";
    return "I-type desconhecida";
}

string s_type_instruction(uint32_t funct3){
    if(funct3 == 0x0) return "sb";
    if(funct3 == 0x1) return "sh";
    if(funct3 == 0x2) return "sw";
    return "S-type desconhecida";
}

string b_type_instruction(uint32_t funct3){
    if(funct3 == 0x0) return "beq";
    if(funct3 == 0x1) return "bne";
    if(funct3 == 0x4) return "blt";
    if(funct3 == 0x5) return "bge";
    if(funct3 == 0x6) return "bltu";
    if(funct3 == 0x7) return "bgeu";
    return "B-type desconhecida";
}

string u_type_instruction(uint32_t opcode){
    if(opcode == 0x37) return "lui";
    if(opcode == 0x17) return "auipc";
    return "U-type desconhecida";
}

string j_type_instruction(uint32_t opcode){
    if(opcode == 0x6F) return "jal";
    return "J-type desconhecida";
}

// --------------------- EXTRAÇÃO DE REGISTRADORES ---------------------
Regs get_registers(uint32_t instr, string type){
    Regs r = {-1,-1,-1};

    if(type=="Tipo R"){
        r.rd  = get_field(instr,RD,7);
        r.rs1 = get_field(instr,RS1,15);
        r.rs2 = get_field(instr,RS2,20);

    } else if(type=="Tipo I"){
        r.rd  = get_field(instr,RD,7);
        r.rs1 = get_field(instr,RS1,15);

    } else if(type=="Tipo S" || type=="Tipo B"){
        r.rs1 = get_field(instr,RS1,15);
        r.rs2 = get_field(instr,RS2,20);

    } else if(type=="Tipo U" || type=="Tipo J"){
        r.rd  = get_field(instr,RD,7);
    }
    return r;
}
