#pragma once
#include <string>
#include <cstdint>

using namespace std;

// --------------------- IDENTIFICAÇÃO DE OPERAÇÃO ---------------------

string R_type_instruction(uint32_t funct7, uint32_t funct3) {
    if (funct3 == 0x0 && funct7 == 0x00) return "add  ";
    if (funct3 == 0x0 && funct7 == 0x20) return "sub  ";
    if (funct3 == 0x7 && funct7 == 0x00) return "and  ";
    if (funct3 == 0x6 && funct7 == 0x00) return "or   ";
    if (funct3 == 0x4 && funct7 == 0x00) return "xor  ";
    if (funct3 == 0x1 && funct7 == 0x00) return "sll  ";
    if (funct3 == 0x5 && funct7 == 0x00) return "srl  ";
    if (funct3 == 0x5 && funct7 == 0x20) return "sra  ";
    if (funct3 == 0x2 && funct7 == 0x00) return "slt  ";
    if (funct3 == 0x3 && funct7 == 0x00) return "sltu ";
    if (funct3 == 0x0 && funct7 == 0x1 ) return "mul  ";
    if (funct3 == 0x4 && funct7 == 0x1 ) return "div  ";
    if (funct3 == 0x6 && funct7 == 0x1 ) return "rem  ";
    return "R-type desconhecida";
}

string I_type_instruction(uint32_t opcode, uint32_t funct3) {
    if (opcode == 0x13) { // ALU imediata
        if (funct3 == 0x0) return "addi ";
        if (funct3 == 0x2) return "slti ";
        if (funct3 == 0x3) return "sltiu";
        if (funct3 == 0x4) return "xori ";
        if (funct3 == 0x6) return "ori  ";
        if (funct3 == 0x7) return "andi ";
        if (funct3 == 0x1) return "slli ";
        if (funct3 == 0x5) return "srli/srai";
    }
    if (opcode == 0x03) { // Loads
        if (funct3 == 0x0) return "lb   ";
        if (funct3 == 0x1) return "lh   ";
        if (funct3 == 0x2) return "lw   ";
        if (funct3 == 0x4) return "lbu  ";
        if (funct3 == 0x5) return "lhu  ";
    }
    if (opcode == 0x67) return "jalr ";
    if (opcode == 0x0F) return "fence";
    if (opcode == 0x73) return "ecall/ebreak";
    return "I-type desconhecida";
}

string S_type_instruction(uint32_t funct3) {
    if (funct3 == 0x0) return "sb   ";
    if (funct3 == 0x1) return "sh   ";
    if (funct3 == 0x2) return "sw   ";
    return "S-type desconhecida";
}

string B_type_instruction(uint32_t funct3) {
    if (funct3 == 0x0) return "beq  ";
    if (funct3 == 0x1) return "bne  ";
    if (funct3 == 0x4) return "blt  ";
    if (funct3 == 0x5) return "bge  ";
    if (funct3 == 0x6) return "bltu ";
    if (funct3 == 0x7) return "bgeu ";
    return "B-type desconhecida";
}

string U_type_instruction(uint32_t opcode) {
    if (opcode == 0x37) return "lui ";
    if (opcode == 0x17) return "auipc";
    return "U-type desconhecida";
}

string J_type_instruction(uint32_t opcode) {
    if (opcode == 0x6F) return "jal  ";
    return "J-type desconhecida";
}

// --------------------- IDENTIFICADOR PRINCIPAL ---------------------

inline string opcode_identifier(uint32_t instr) {
    uint32_t opcode = instr & 0x7F;
    uint32_t funct3 = (instr >> 12) & 0x7;
    uint32_t funct7 = (instr >> 25) & 0x7F;

    if (opcode == 0x33) // R-type
        return "Tipo R - " + R_type_instruction(funct7, funct3);

    if (opcode == 0x13 || opcode == 0x03 || opcode == 0x67 || opcode == 0x0F || opcode == 0x73)
        return "Tipo I - " + I_type_instruction(opcode, funct3);

    if (opcode == 0x23) // S-type
        return "Tipo S - " + S_type_instruction(funct3);

    if (opcode == 0x63) // B-type
        return "Tipo B - " + B_type_instruction(funct3);

    if (opcode == 0x37 || opcode == 0x17) // U-type
        return "Tipo U - " + U_type_instruction(opcode);

    if (opcode == 0x6F) // J-type
        return "Tipo J - " + J_type_instruction(opcode);

    return "opcode desconhecido";
}

// ----------- EXTRAÇÃO DE REGISTRADORES (comentado por enquanto) ------------------

 struct Regs {
     int rd;
     int rs1;
     int rs2;
 };

 inline int get_field(uint32_t instr, int pos, int len) {
     return (instr >> pos) & ((1 << len) - 1);
 }

 inline Regs get_registers(uint32_t instr, const std::string &type) {
     Regs r = {-1, -1, -1};

     if (type.find("Tipo R") != std::string::npos) {
         r.rd  = get_field(instr, 7, 5);
         r.rs1 = get_field(instr, 15, 5);
         r.rs2 = get_field(instr, 20, 5);

     } else if (type.find("Tipo I") != std::string::npos) {
         r.rd  = get_field(instr, 7, 5);
         r.rs1 = get_field(instr, 15, 5);

     } else if (type.find("Tipo S") != std::string::npos ||
                type.find("Tipo B") != std::string::npos) {
         r.rs1 = get_field(instr, 15, 5);
         r.rs2 = get_field(instr, 20, 5);

     } else if (type.find("Tipo U") != std::string::npos ||
                type.find("Tipo J") != std::string::npos) {
         r.rd  = get_field(instr, 7, 5);
     }

     return r;
 }
