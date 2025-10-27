#include <iostream>
#include <cstdint>
#include <fstream>
#include <map>     //map
#include <iomanip> //setfill
#include "instruction_types_RISCV.hpp"

/*vetor{
    rs1
    rs2
    rd
}
*/

//memoria de instruções                           

using namespace std;

struct instrucoes{
    uint32_t id; //linha atual
    string tipoInst; //addi, beq
    Regs r; //imediato, rs1, rs2, rd
    uint32_t hex_value;
};

int main() {
    //dita a base do código que vai ser lido
    int base = 16;
    vector<instrucoes> memoria_instrucoes;
    //abre o arquivo
    ifstream meu_arquivo("dumps/nopsnopsnops.txt");
    if (!meu_arquivo.is_open()) {
        cout << "Erro ao abrir" << endl;
        return 1;
    }

    string current_line; //variavel que contém a linha de leitura
    int linha_atual = 0; //contador que contém a linha atual

    vector<uint32_t> instruction_memory;//vetor IMPORTANTEEE
    //!!!!aqui fosse catalogado a memoria de intruções
    while (getline(meu_arquivo, current_line)) {
        if (current_line.empty()) continue;
        // 1. Converte a linha de texto para um número hexadecimal
        uint32_t hexa = stoul(current_line, nullptr, base);
        // 2. Decodifica o tipo da instrução e seus registradores
        string nome_instrucao = opcode_identifier(hexa);
        Regs registradores_instrucao = get_registers(hexa, nome_instrucao);
        // 3. Cria um objeto 'instrucoes' temporario para guardar tudo
        instrucoes nova_instrucao;
        nova_instrucao.id = linha_atual;
        nova_instrucao.tipoInst = nome_instrucao;
        nova_instrucao.r = registradores_instrucao;
        nova_instrucao.hex_value = hexa;
        // 4. Adiciona a instrução já decodificada ao seu vetor principal
        memoria_instrucoes.push_back(nova_instrucao);
        linha_atual++;
    }
    meu_arquivo.close();
    
    // Inicialização do mapa de contagem
    map<string, int> contador;
    
//..................................................
//...bool conflito_rs1 (memoria_instrucoes[0].r.rd == memoria_instrucoes[1].r.rs1);
//..................................................

    // Loop de simulação principal 
    size_t instruction_index = 0; // utiliza indice para percorrer a nova memoria

    while (instruction_index < memoria_instrucoes.size()) {
        // Busca a instrução já decodificada da nova memoria
        const instrucoes& current_inst = memoria_instrucoes[instruction_index];
        
        // Extrai as informações que a ju pediu
        uint32_t current_hexa = current_inst.hex_value;
        string instype = current_inst.tipoInst;
        Regs r = current_inst.r;
        pc = instruction_index * 4; // Atualizamos o pc para manter os prints corretos

        contador[instype]++;

        //set fill pra preencher as linhas com o 0 e deixar o print mais arrumado
        cout << "\nL. " << setw(2) << setfill('0') << (instruction_index + 1)
             << " (PC=" << pc << ")"
             << " inst. '" << hex << current_hexa << dec
             << "' eh: " << instype;
        // Extrai registradores e imediatos
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
       //1
        if (instype.find("addi") != string::npos) {
            //adiciona um conflito aqui
            registradores[r.rd] = registradores[r.rs1] + r.total;
        } 

        //2
        else if (instype.find("add ") != string::npos) { // Espaço para diferenciar de 'addi'
            //adiciona um conflito aqui
            registradores[r.rd] = registradores[r.rs1] + registradores[r.rs2];
        } 

        //3
        else if (instype.find("sub ") != string::npos) {
            //adiciona um conflito aqui
            registradores[r.rd] = registradores[r.rs1] - registradores[r.rs2];
        } 

        //4
        else if (instype.find("lw ") != string::npos) {
            //adiciona um conflito aqui
            uint32_t addr = registradores[r.rs1] + r.total;
            registradores[r.rd] = read_word_from_memory(addr);
        } 

        //5
        else if (instype.find("sw ") != string::npos) {
            //adiciona um conflito aqui
            uint32_t addr = registradores[r.rs1] + r.total;
            write_word_to_memory(addr, registradores[r.rs2]);
        }

        //6
        // BEQ (Desvia se rs1 == rs2)
        else if (instype.find("beq") != string::npos) {
            if (registradores[r.rs1] == registradores[r.rs2]) {
                pc = pc + r.total;
                pc_changed_by_branch = true;
            }
        } 

        //7
        // BNE (Desvia se rs1 != rs2)
        else if (instype.find("bne") != string::npos) {
            if (registradores[r.rs1] != registradores[r.rs2]) {
                pc = pc + r.total;
                pc_changed_by_branch = true;
            }
        } 

        //8
        // BLT (COM SINAL)
        else if (instype.find("blt") != string::npos) {
            if (registradores[r.rs1] < registradores[r.rs2]) {
                pc = pc + r.total;
                pc_changed_by_branch = true;
            }
        } 

        //9
        // BGE (COM SINAL)
        else if (instype.find("bge") != string::npos) {
            if (registradores[r.rs1] >= registradores[r.rs2]) {
                pc = pc + r.total;
                pc_changed_by_branch = true;
            }
        } 

        //10
        // BLTU (SEM SINAL)
        else if (instype.find("bltu") != string::npos) {
            // utilizamos uint32_t para comparação sem sinal
            if ((uint32_t)registradores[r.rs1] < (uint32_t)registradores[r.rs2]) {
                pc = pc + r.total;
                pc_changed_by_branch = true;
            }
        } 

        //11
        // BGEU (SEM SINAL)
        else if (instype.find("bgeu") != string::npos) {
            // utilizamos uint32_t para comparação sem sinal
            if ((uint32_t)registradores[r.rs1] >= (uint32_t)registradores[r.rs2]) {
                pc = pc + r.total;
                pc_changed_by_branch = true;
            }
        }

        registradores[0] = 0; // Garante que o registrador x0 seja sempre 0 

        // 4. ATUALIZAÇÃO DO CONTADOR DO LOOP
        if (!pc_changed_by_branch) {
            // Se não houve desvio, simplesmente vai pra proxima instrução no vetor
            instruction_index++;
        } else {
            // Se houve um desvio, o pc foi atualizado
            // precisa encontrar o novo indice que corresponde ao novo pc
            instruction_index = pc / 4;
        }
    }
    cout << "\n\n================= RESUMO =================\n";
    for (auto &par : contador) {
        cout << par.first << " -> " << setw(2) << setfill('0') << par.second << " instrucoes\n";
    }

    print_final_registers();
    return 0;
}