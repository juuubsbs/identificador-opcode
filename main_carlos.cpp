#include <iostream>
#include <cstdint>
#include <fstream>
#include <map>
#include <iomanip>
#include <vector>
#include "instruction_types_RISCV_carlos.hpp"
using namespace std;

int main() {
    int cont_line = 0;
    string instype;
    int base = 16;

    vector<uint32_t> instruction_memory;
    ifstream meu_arquivo("hexa_dump.txt");
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
    
    map<string, int> contador;

    // -----------------------------------------------------------
    // 🔍 Estrutura para armazenar última instrução decodificada
    Regs prev_r = {-1, -1, -1, Immediates(), 0};
    string prev_instype = "";
    bool prev_valid = false;
    int prev_line = 0;
    // -----------------------------------------------------------

    while (pc < instruction_memory.size() * 4) {
        cont_line++;

        uint32_t current_hexa = instruction_memory[pc / 4];
        instype = opcode_identifier(current_hexa);
        contador[instype]++;

        Regs r = get_registers(current_hexa, instype);

        cout << "\nL. " << setw(2) << setfill('0') << cont_line
             << " (PC=" << pc << ")"
             << " inst. '" << hex << current_hexa << dec
             << "' eh: " << instype
             << " | rd=" << r.rd
             << " rs1=" << r.rs1
             << " rs2=" << r.rs2;

        // ---------carlin o mais lindo 
        
   
        if (prev_valid) {
            bool conflito_rs1 = (r.rs1 == prev_r.rd && r.rs1 != -1 && prev_r.rd != 0);
            bool conflito_rs2 = (r.rs2 == prev_r.rd && r.rs2 != -1 && prev_r.rd != 0);

            if (conflito_rs1 || conflito_rs2) {
                cout << "\n   Possível conflito detectado com a linha " << prev_line;
                
    if ((prev_instype.find("lw") != string::npos || 
     prev_instype.find("lh") != string::npos ||
     prev_instype.find("lb") != string::npos) && (conflito_rs1 || conflito_rs2)) 
{
    cout << " → tipo 'LOAD' anterior, exige NOP (Load-Use Hazard).";
} else {
    cout << " → poderia ser resolvido com forwarding.";
}
            }
        }
        // -----------------------------

        // Atualiza o histórico da instrução anterior
        prev_r = r;
        prev_instype = instype;
        prev_valid = (
            instype.find("add") != string::npos ||
            instype.find("sub") != string::npos ||
            instype.find("addi") != string::npos ||
            instype.find("lw") != string::npos
        );
        prev_line = cont_line;
        // -----------------------------------------------------------

        bool pc_changed_by_branch = false;

        if (instype.find("addi") != string::npos) {
            registers[r.rd] = registers[r.rs1] + r.total;
        } else if (instype.find("add ") != string::npos) {
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

        registers[0] = 0;

        if (!pc_changed_by_branch)
            pc += 4;
    }

    cout << "\n\n================= RESUMO ============\n";
    for (auto &par : contador) {
        cout << par.first << " -> " << setw(2) << setfill('0') << par.second << " instrucoes\n";
    }

    print_final_registers();
    return 0;
}
