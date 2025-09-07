#include <cstdint>



//Usei nomes distintos para as duas ocorrências de imm[11] (uma no formato B e outra no J) para evitar ambiguidade: IMM_11_B (bit 7) e IMM_11_J (bit 20).
//IMM_12 (B-type) e IMM_20 (J-type) apontam para o mesmo bit físico ([31]) — por isso a mesma máscara 0x80000000.


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





uint32_t R_type_mask( uint32_t hexadecimal){
    uint32_t r_funct7, r_rs2, r_rs1, r_funct3, r_rd, r_opcode;

    func7 = idolate sdncksbck(hexadecimal, FUNCT7);
    rs2 = idcibsdchbskc(hexadecimal);

    R_type_instruction(func7, funct3);
}

std::string R_type_instruction(uint32_t  funct7, uint32_t funct3){
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
}

//intrução e r_instrução
uint32_t inst;
R_type_format r_inst;

