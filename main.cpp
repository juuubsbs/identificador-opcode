#include <iostream>
#include <cstdint>
#include <fstream>
#include <map>     //map
#include <iomanip> //setfill
#include <vector>
#include "intruction_types_RISCV.hpp"
using namespace std;

int main() {
    int cont_line = 0;
    string instype;
    //IMPORTANTE DIGITAR ANTES DE COMPILAR
    int base = 16;

    vector<uint32_t> instruction_memory;
    ifstream meu_arquivo("dumps/nopsnopsnops.txt");
    if (!meu_arquivo.is_open()) {
        cout << "Erro ao abrir" << endl;
        return 1;
    }
    string current_line;
    while (getline(meu_arquivo, current_line)) {
        if (current_line.empty()) continue;
        instruction_memory.push_back(stoul(current_line, nullptr, base));
    }
    meu_arquivo.close();
    
    // Inicialização do mapa de contagem
    map<string, int> contador;

    // Loop de simulação principal 
    while (pc < instruction_memory.size() * 4) {
        cont_line++;

        // Busca a instrução da memória usando o PC
        uint32_t current_hexa = instruction_memory[pc / 4];
        
        // Identifica a instrução
        instype = opcode_identifier(current_hexa);
        contador[instype]++;

        cout << "\nL. " << setw(2) << setfill('0') << cont_line
             << " (PC=" << pc << ")"
             << " inst. '" << hex << current_hexa << dec
             << "' eh: " << instype;

        // Extrai registradores e imediatos
        Regs r = get_registers(current_hexa, instype);
        cout << " | rd=" << r.rd
             << " rs1=" << r.rs1
             << " rs2=" << r.rs2;

        // Imprime imediatos detalhados e valor completo
        if (instype.find("Tipo I") != string::npos) {
            cout << " | imm(I)=" << r.total
                 << " (raw=" << ((current_hexa >> 20) & 0xFFF) << ")"; 
        } 
        else if (instype.find("Tipo S") != string::npos) {
            cout << " | imm(S) = [" << ((current_hexa >> 25) & 0x7F) << " | " << ((current_hexa >> 7) & 0x1F) << "]"
                 << " = " << r.total;
        } 
        else if (instype.find("Tipo B") != string::npos) {
            cout << " | imm(B) = [" 
                 << ((current_hexa >> 31) & 0x1) << "|" << ((current_hexa >> 7) & 0x1) << "|" 
                 << ((current_hexa >> 25) & 0x3F) << "|" << ((current_hexa >> 8) & 0xF) << "]"
                 << " = " << r.total;
        } 
        else if (instype.find("Tipo U") != string::npos) {
            cout << " | imm(U)=" << (current_hexa & 0xFFFFF000)
                 << " = " << r.total;
        } 
        else if (instype.find("Tipo J") != string::npos) {
            cout << " | imm(J) = [...] = " << r.total;
        }

        // Executa a instrução decodificada
        bool pc_changed_by_branch = false;

        if (instype.find("addi") != string::npos) {
            registers[r.rd] = registers[r.rs1] + r.total;
        } else if (instype.find("add ") != string::npos) { // Espaço para diferenciar de 'addi'
            registers[r.rd] = registers[r.rs1] + registers[r.rs2];
        } else if (instype.find("sub ") != string::npos) {
            registers[r.rd] = registers[r.rs1] - registers[r.rs2];
        } else if (instype.find("lw ") != string::npos) {
            uint32_t addr = registers[r.rs1] + r.total;
            registers[r.rd] = read_word_from_memory(addr);
        } else if (instype.find("sw ") != string::npos) {
            uint32_t addr = registers[r.rs1] + r.total;
            write_word_to_memory(addr, registers[r.rs2]);
        }
        registers[0] = 0; // Garante que o registrador x0 seja sempre 0

        // Atualiza o Program Counter
        if (!pc_changed_by_branch) {
            pc += 4;
        }
    }

    cout << "\n\n================= RESUMO ============\n";
    for (auto &par : contador) {
        cout << par.first << " -> " << setw(2) << setfill('0') << par.second << " instrucoes\n";
    }

    print_final_registers();
    return 0;
}
