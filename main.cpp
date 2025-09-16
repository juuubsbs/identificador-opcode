#include <iostream>
#include <cstdint>
#include <fstream>
#include <map>     //map
#include <iomanip> //setfill
#include "intruction_types_RISCV.hpp"

using namespace std;

int main() {
    int cont_line = 0;
    string instype;
    //IMPORTANTE DIGITAR ANTES DE COMPILAR
    int base = 16;

    ifstream meu_arquivo("dumps/profs/fib_rec_hexadecimal.txt");
    if (!meu_arquivo.is_open()) {
        cout << "Erro ao abrir" << endl;
        return 1;
    }

    map<string, int> contador;
    string current_line;
    uint32_t current_hexa;

    while (getline(meu_arquivo, current_line)) {
        if (current_line.empty()) continue;
        cont_line++;

        current_hexa = stoul(current_line, nullptr, base);
        instype = opcode_identifier(current_hexa);
        contador[instype]++;

        cout << "\nL. " << setw(2) << setfill('0') << cont_line
             << " inst. '" << current_line
             << "' eh: " << instype;

        // Extrai registradores e imediatos
        Regs r = get_registers(current_hexa, instype);
        cout << " | rd=" << r.rd
             << " rs1=" << r.rs1
             << " rs2=" << r.rs2;

        // Imprime imediatos detalhados e valor completo
        if (instype.find("Tipo I") != string::npos) {
            int32_t imm_val = (int32_t)((current_hexa >> 20) & 0xFFF);
            if (imm_val & 0x800) imm_val |= 0xFFFFF000; // sinal
            cout << " | imm(I)=" << imm_val
                 << " (raw=" << r.imm.imm_11_5 << ")";
        } 
        else if (instype.find("Tipo S") != string::npos) {
            int32_t imm_val = get_s_imm(current_hexa);
            cout << " | imm(S) = [" << r.imm.imm_11_5 << " | " << r.imm.imm_4_0 << "]"
                 << " = " << imm_val;
        } 
        else if (instype.find("Tipo B") != string::npos) {
            int32_t imm_val = get_b_imm(current_hexa);
            cout << " | imm(B) = [" 
                 << r.imm.imm_12 << "|" << r.imm.imm_11 << "|" 
                 << r.imm.imm_10_5 << "|" << r.imm.imm_4_1 << "]"
                 << " = " << r.total;
        } 
        else if (instype.find("Tipo U") != string::npos) {
            int32_t imm_val = get_u_imm(current_hexa);
            cout << " | imm(U)=" << r.imm.imm_31_12
                 << " = " << imm_val;
        } 
        else if (instype.find("Tipo J") != string::npos) {
            int32_t imm_val = get_j_imm(current_hexa);
            cout << " | imm(J) = ["
                 << "imm_20= " << r.imm.imm_20
                 << ", imm_19_12= " << r.imm.imm_19_12
                 << ", imm_11= " << r.imm.imm_11_J
                 << ", imm_10_1= " << r.imm.imm_10_1
                 << "] = " << r.total;
        }
    }

    cout << "\n\n================= RESUMO ============\n";
    for (auto &par : contador) {
        cout << par.first << " -> " << setw(2) << setfill('0') << par.second << " instrucoes\n";
    }

    return 0;
}
