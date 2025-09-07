#include <iostream>
#include <cstdint> // biblioteca que contém o uint
#include <fstream>
#include <map>
#include <iomanip>
#include "intruction_types_RISCV.hpp"

using namespace std;

int main() {
    int cont_line = 0;
    string instype;

    ifstream meu_arquivo;
    meu_arquivo.open("oie_hexaaaaaaa.txt");

    map<string, int> contador;

    string current_line;
    uint32_t current_hexa;

    while (getline(meu_arquivo, current_line)) {
        if (current_line.empty()) continue;
        cont_line++;

        current_hexa = stoul(current_line, nullptr, 16); // converte string para inteiro

        // antes pegagva o opcode agra pega ele todo
        instype = opcode_identifier(current_hexa);

        contador[instype]++;

        cout << "\nLinha " << setw(2) << setfill('0') << cont_line 
             << " instrucao '" << current_line
             << "' eh: " << instype;

         //--------------------- EXTRAÇÃO DE REGISTRADORES (comentado por enquanto) ---------------------
         Regs r = get_registers(current_hexa, instype);
         cout << " | rd=" << r.rd
              << " rs1=" << r.rs1
              << " rs2=" << r.rs2;
    }

    cout << "\n\n=========== RESUMO ============\n";
    for (auto &par : contador) {
        cout << par.first << " -> " << setw(2) << setfill('0') << par.second << " instrucoes\n";
    }
}
