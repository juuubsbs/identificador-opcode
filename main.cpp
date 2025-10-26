#include <iostream>
#include <cstdint>
#include <fstream>
#include <map>     //map
#include <iomanip> //setfill
#include "instruction_types_RISCV.hpp"

//precisa da memoria de dados pra conseguir entender oq tá dentro do rd pra ver oq dá errado
using namespace std;

int main() {
    int cont_line = 0;
    string instype;

    //dita a base do código que vai ser lido
    int base = 16;

    //abre o arquivo
    ifstream meu_arquivo("dumps/nopsnopsnops.txt");
    if (!meu_arquivo.is_open()) {
        cout << "Erro ao abrir" << endl;
        return 1;
    }

    string current_line; //variavel que contém a linha atual de leitura
    vector<uint32_t> instruction_memory;//vetor IMPORTANTEEE
    while (getline(meu_arquivo, current_line)) {
        if (current_line.empty()) continue;
        instruction_memory.push_back(stoul(current_line, nullptr, base));
    }
    meu_arquivo.close();
    
    // Inicialização do mapa de contagem
    map<string, int> contador;
    uint32_t current_hexa;

    //nossa lista de conflitos
    

    // Loop de simulação principal 
    while (pc < instruction_memory.size() * 4) {
        cont_line++;

        // Busca a instrução da memória usando o PC
        current_hexa = instruction_memory[pc / 4];
        
        // Identifica a instrução
        instype = opcode_identifier(current_hexa);
        contador[instype]++;

        //set fill pra preencher as linhas com o 0 e deixar o print mais arrumado
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
            //adiciona um conflito aqui
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
//---------------------------------------------------------------------------
        // Executa a instrução decodificada
        bool pc_changed_by_branch = false;

        //essa parte realiza a soma, subtração entre outros dos valores
        /*
            registradores[] = meu vetor que salva os valores resultates das operações
                -> possui 32 "espaços" para salvar cada um dos valores na posição correta
                -> as posições são ditadas pelo valor provindo de rd, que indica a posição
            r.total = possui o imediato, depois daquelas operações de concatenações e tudo mais
        */
        if (instype.find("addi") != string::npos) {
            //adiciona um conflito aqui
            registradores[r.rd] = registradores[r.rs1] + r.total;
        } 
        else if (instype.find("add ") != string::npos) { // Espaço para diferenciar de 'addi'
            //adiciona um conflito aqui
            registradores[r.rd] = registradores[r.rs1] + registradores[r.rs2];
        } 
        else if (instype.find("sub ") != string::npos) {
            //adiciona um conflito aqui
            registradores[r.rd] = registradores[r.rs1] - registradores[r.rs2];
        } 
        else if (instype.find("lw ") != string::npos) {
            //adiciona um conflito aqui
            uint32_t addr = registradores[r.rs1] + r.total;
            registradores[r.rd] = read_word_from_memory(addr);
        } 
        else if (instype.find("sw ") != string::npos) {
            //adiciona um conflito aqui
            uint32_t addr = registradores[r.rs1] + r.total;
            write_word_to_memory(addr, registradores[r.rs2]);
        }
        registradores[0] = 0; // Garante que o registrador x0 seja sempre 0

        // Atualiza o Program Counter
        if (!pc_changed_by_branch) {
            pc += 4;
        }
    }

    cout << "\n\n================= RESUMO =================\n";
    for (auto &par : contador) {
        cout << par.first << " -> " << setw(2) << setfill('0') << par.second << " instrucoes\n";
    }

    print_final_registers();
    return 0;
}
