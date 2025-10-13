    #pragma once
    #include <string>
    #include <cstdint>
    #include <vector>
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

    std::string to_binary(uint32_t value, int bits = 3) {
        std::string result;
        for (int i = bits - 1; i >= 0; --i) {
            result += ((value >> i) & 1) ? '1' : '0';
        }
        return result;
    }

    // --------------------- IDENTIFICADOR PRINCIPAL ---------------------

    inline string opcode_identifier(uint32_t instr) {
        uint32_t opcode = instr & 0x7F;
        uint32_t funct3 = (instr >> 12) & 0x7;
        uint32_t funct7 = (instr >> 25) & 0x7F;

        if (opcode == 0x33) // R-type
            return "Tipo R - " + R_type_instruction(funct7, funct3) + "func3 = " + to_binary(funct3);

        if (opcode == 0x13 || opcode == 0x03 || opcode == 0x67 || opcode == 0x0F || opcode == 0x73)
            return "Tipo I - " + I_type_instruction(opcode, funct3) + "func3 = " + to_binary(funct3);

        if (opcode == 0x23) // S-type
            return "Tipo S - " + S_type_instruction(funct3) + "func3 = " + to_binary(funct3);

        if (opcode == 0x63) // B-type
            return "Tipo B - " + B_type_instruction(funct3) + "func3 = " + to_binary(funct3);

        if (opcode == 0x37 || opcode == 0x17) // U-type
            return "Tipo U - " + U_type_instruction(opcode);

        if (opcode == 0x6F) // J-type
            return "Tipo J - " + J_type_instruction(opcode);

        return "opcode desconhecido";
    }


    //--------------------- ESTRUTURA DE IMEDIATOS ---------------------
    struct Immediates {
        // S-type
        int imm_11_5 = 0;
        int imm_4_0  = 0;
        // B-type
        int imm_12   = 0;
        int imm_10_5 = 0;
        int imm_4_1  = 0;
        int imm_11   = 0;
        // U-type
        int imm_31_12= 0;
        // J-type
        int imm_20   = 0;
        int imm_10_1 = 0;
        int imm_11_J = 0;
        int imm_19_12= 0;
    };

    //--------------------- FUNÇÕES DE EXTRAÇÃO DE IMEDIATOS FEIOS ---------------------
    int32_t get_s_imm(uint32_t instr){
        int32_t imm = ((instr>>25) << 5) | ((instr >> 7) & 0x1F); // 11:5 | 4:0
        if(imm & 0x800) imm |= 0xFFFFF000;
        return imm;
    }

    int32_t get_b_imm(uint32_t instr){
        int32_t imm = 0;
        imm |= ((instr >> 31) & 0x1) << 12;  // imm[12]
        imm |= ((instr >> 25) & 0x3F) << 5;  // imm[10:5]
        imm |= ((instr >> 8) & 0xF) << 1;    // imm[4:1]
        imm |= ((instr >> 7) & 0x1) << 11;   // imm[11]
        if(imm & 0x1000) imm |= 0xFFFFE000;
        return imm;
    }

    int32_t get_u_imm(uint32_t instr){
        return instr & 0xFFFFF000; // bits 31:12
    }

    int32_t get_j_imm(uint32_t instr){
        int32_t imm = 0;
        imm |= ((instr >> 31) & 0x1) << 20;      // imm[20]
        imm |= ((instr >> 21) & 0x3FF) << 1;     // imm[10:1]
        imm |= ((instr >> 20) & 0x1) << 11;      // imm[11]
        imm |= ((instr >> 12) & 0xFF) << 12;     // imm[19:12]
        if(imm & 0x100000) imm |= 0xFFE00000;
        return imm;
    }

    int32_t get_i_imm(uint32_t instr) {
        int32_t imm = (instr >> 20);
        // Verifica se o bit de sinal (bit 11) está ativo
        if (imm & 0x800) { 
            imm |= 0xFFFFF000; // Estende o sinal para o resto do inteiro de 32 bits
        }
        return imm;
    }

    // ----------- EXTRAÇÃO DE REGISTRADORES ------------------
    struct Regs {
        int rd;
        int rs1;
        int rs2;
        Immediates imm;
        int total;
    };

    inline int get_field(uint32_t instr, int pos, int len) {
        return (instr >> pos) & ((1 << len) - 1);
    }

    inline Regs get_registers(uint32_t instr, const string &type) {
        Regs r = {-1,-1,-1,Immediates(),0};

        if(type.find("Tipo R") != string::npos) {
            r.rd  = get_field(instr, 7, 5);
            r.rs1 = get_field(instr, 15, 5);
            r.rs2 = get_field(instr, 20, 5);

        } else if(type.find("Tipo I") != string::npos) {
            r.rd  = get_field(instr, 7, 5);
            r.rs1 = get_field(instr, 15, 5);
            r.imm.imm_11_5 = (instr >> 20) & 0xFFF;
            r.total = get_i_imm(instr); 

        } else if(type.find("Tipo S") != string::npos) {
            r.rs1 = get_field(instr, 15, 5);
            r.rs2 = get_field(instr, 20, 5);
            r.imm.imm_11_5 = (instr >> 25) & 0x7F;
            r.imm.imm_4_0  = (instr >> 7) & 0x1F;
            r.total = get_s_imm(instr);

        } else if(type.find("Tipo B") != string::npos) {
            r.rs1 = get_field(instr, 15, 5);
            r.rs2 = get_field(instr, 20, 5);
            r.imm.imm_12   = (instr >> 31) & 0x1;
            r.imm.imm_10_5 = (instr >> 25) & 0x3F;
            r.imm.imm_4_1  = (instr >> 8) & 0xF;
            r.imm.imm_11   = (instr >> 7) & 0x1;
            r.total = get_b_imm(instr);

        } else if(type.find("Tipo U") != string::npos) {
            r.rd  = get_field(instr, 7, 5);
            r.imm.imm_31_12 = instr & 0xFFFFF000;
            r.total = get_u_imm(instr);

        } else if(type.find("Tipo J") != string::npos) {
            r.rd  = get_field(instr, 7, 5);
            r.imm.imm_20   = (instr >> 31) & 0x1;
            r.imm.imm_10_1 = (instr >> 21) & 0x3FF;
            r.imm.imm_11_J = (instr >> 20) & 0x1;
            r.imm.imm_19_12= (instr >> 12) & 0xFF;
            r.total = get_j_imm(instr);
        }

        return r;
    }

    // -------- ESTADO DO PROCESSADOR --------
    uint32_t pc = 0; // Program Counter
    vector<int32_t> registers(32, 0); // Banco de 32 registradores, inicializados em 0
    vector<uint8_t> data_memory(4096, 0); // 4KB de memória de dados, inicializada em 0

    // -------- FUNÇÕES AUXILIARES DE MEMÓRIA --------
    // Lê uma palavra de 32 bits (4 bytes) da memória
    int32_t read_word_from_memory(uint32_t address) {
        if (address + 3 >= data_memory.size()) {
            cerr << " [ERRO DE EXECUÇÃO: Leitura fora dos limites da memória no endereço " << address << "]" << endl;
            return 0;
        }
        return (int32_t)((uint32_t)data_memory[address+3] << 24 |
                        (uint32_t)data_memory[address+2] << 16 |
                        (uint32_t)data_memory[address+1] << 8  |
                        (uint32_t)data_memory[address+0]);
    }

    // Escreve uma palavra de 32 bits (4 bytes) na memória
    void write_word_to_memory(uint32_t address, int32_t value) {
        if (address + 3 >= data_memory.size()) {
            cerr << " [ERRO DE EXECUÇÃO: Escrita fora dos limites da memória no endereço " << address << "]" << endl;
            return;
        }
        data_memory[address+0] = (value & 0x000000FF);
        data_memory[address+1] = (value & 0x0000FF00) >> 8;
        data_memory[address+2] = (value & 0x00FF0000) >> 16;
        data_memory[address+3] = (value & 0xFF000000) >> 24;
    }

    // Imprime os registradores que não são zero
    void print_final_registers() {
        cout << "\n\n================= ESTADO FINAL DOS REGISTRADORES =================\n";
        for(int i = 0; i < 32; ++i) {
            if (registers[i] != 0 || i == 0) {
                cout << "x" << setw(2) << setfill(' ') << i << " = " << setw(10) << registers[i] 
                    << " (0x" << hex << setfill('0') << setw(8) << registers[i] << dec << ")" << endl;
            }
        }
    }

