#include <iostream>
#include <cstdint>
#include <fstream>
#include <map>     //map
#include <iomanip> //setfill
#include "instruction_types_RISCV.hpp"                     

using namespace std;

struct instrucoes{
    int id; //linha atual
    string tipoInst; //addi, beq
    Regs r; //imediato, rs1, rs2, rd
    uint32_t hex_value;
};

struct hazard{
    string tipoErro; //dado ou controle
    int com_quem; //indice do cara de cima
    int indice;
    bool forwarding;
};

void imprimirComHazardsENops(const vector<instrucoes>& instrucoesMem, const vector<hazard>& hazard_lista) {
    cout << "== CODIGO COM INSERCAO DE NOPS ==" << endl;
    // vector que diz quantos nops precisa inserir antes de cada instrução
    vector<int> nopsAntesDaInstrucao(instrucoesMem.size(), 0);
    // Analisa a lista de hazards e marca onde inserir os nops
    for (const auto& h : hazard_lista) {
        if (h.tipoErro == "DADOS") {
            int nops_necessarios = 0;
            int distancia = h.indice - h.com_quem;

            // Verifica o hazard é um 'lw'
            bool eh_load_use = instrucoesMem[h.com_quem].tipoInst.find("lw") != string::npos;

            if (h.forwarding) {
                // Com forwarding, só o Load-Use com distância 1 precisa de NOP
                if (eh_load_use && distancia == 1) {
                    nops_necessarios = 1;
                }
            } else {
                // Sem forwarding, a quantidade de nops depende da distância
                if (distancia == 1) {
                    nops_necessarios = eh_load_use ? 1 : 2;
                } else if (distancia == 2) { 
                    nops_necessarios = 1;
                }
            }
            nopsAntesDaInstrucao[h.indice] = max(nopsAntesDaInstrucao[h.indice], nops_necessarios);

        } else if (h.tipoErro == "CONTROLE") {
            // Um branch causa 1 nop na instrução seguinte
             nopsAntesDaInstrucao[h.indice] = max(nopsAntesDaInstrucao[h.indice], 3);
        }
    }

    // imprime instrucoes com nops inseridos
    for (size_t i = 0; i < instrucoesMem.size(); ++i) {
        for (int k = 0; k < nopsAntesDaInstrucao[i]; ++k) {
            cout << "      00000013  nop (Hazard)" << endl;
      }

        cout << "id = " << setfill('0') << setw(2) << instrucoesMem[i].id
             << "  " << hex << setw(8) << setfill('0') << instrucoesMem[i].hex_value
             << "  " << instrucoesMem[i].tipoInst << dec << endl;
    }

    cout << "========================================" << endl;
}

void gerarDumpfile(const string& nomeArquivo,
                   const vector<instrucoes>& instrucoesMem,
                   const vector<hazard>& hazard_lista) {
    
    ofstream arquivo_saida(nomeArquivo);
    if (!arquivo_saida.is_open()) {
        cout << "ERRO: Nao foi possivel criar o arquivo " << nomeArquivo << endl;
        return;
    }

    vector<int> nopsAntesDaInstrucao(instrucoesMem.size(), 0);

    for (const auto& h : hazard_lista) {
        if (h.tipoErro == "DADOS") {
            int nops_necessarios = 0;
            int distancia = h.indice - h.com_quem;
            bool eh_load_use = instrucoesMem[h.com_quem].tipoInst.find("lw") != string::npos;

            if (h.forwarding) {
                if (eh_load_use && distancia == 1) {
                    nops_necessarios = 1;
                }
            } else {
                if (distancia == 1) {
                    nops_necessarios = eh_load_use ? 1 : 2;
                } else if (distancia == 2) {
                    nops_necessarios = 1;
                }
            }
            nopsAntesDaInstrucao[h.indice] = max(nopsAntesDaInstrucao[h.indice], nops_necessarios);

        } else if (h.tipoErro == "CONTROLE") {
             nopsAntesDaInstrucao[h.indice] = max(nopsAntesDaInstrucao[h.indice], 3);
        }
    }

    for (size_t i = 0; i < instrucoesMem.size(); ++i) {
        for (int k = 0; k < nopsAntesDaInstrucao[i]; ++k) {
            arquivo_saida << hex << setfill('0') << setw(8) << 0x13 << endl;
        }
        arquivo_saida << hex << setfill('0') << setw(8) << instrucoesMem[i].hex_value << endl;
    }

    arquivo_saida.close();
    cout << "Arquivo '" << nomeArquivo << "' gerado com sucesso." << endl;
}


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

    cout << "\n--- Gerando arquivo SEM Forwarding ---" << endl;
    {
        bool forwarding = false; 
        vector<hazard> hazard_lista;
        int tamanho = memoria_instrucoes.size();

        //varrendo a memoria de intruções e encontrando os hazards
        for (int i = 0; i < tamanho; i++) {
            const auto& instrucao_atual = memoria_instrucoes[i];

            // hazard de dados
            if (instrucao_atual.r.rs1 > 0 || instrucao_atual.r.rs2 > 0) {
                for (int j = i - 1; j >= i - 2 && j >= 0; j--) {
                    const auto& instrucao_anterior = memoria_instrucoes[j];
                    bool anterior_escreve_reg = instrucao_anterior.r.rd > 0 && instrucao_anterior.tipoInst.find("sw") == string::npos && instrucao_anterior.tipoInst.find("Tipo B") == string::npos;
                    if (anterior_escreve_reg && (instrucao_atual.r.rs1 == instrucao_anterior.r.rd || instrucao_atual.r.rs2 == instrucao_anterior.r.rd)) {
                        hazard h = {"DADOS", j, i, forwarding};
                        hazard_lista.push_back(h);
                        break;
                    }
                }
            }

            // hazard de controle
            if (i > 0 && memoria_instrucoes[i - 1].tipoInst.find("Tipo B") != string::npos) {
                hazard h = {"CONTROLE", i - 1, i, forwarding};
                hazard_lista.push_back(h);
            }
        }
        imprimirComHazardsENops(memoria_instrucoes, hazard_lista);
        gerarDumpfile("dumpfile_sem_forwarding.txt", memoria_instrucoes, hazard_lista);
    }

    cout << "\n--- Gerando arquivo COM Forwarding ---" << endl;
    {
        bool forwarding = true; 
        vector<hazard> hazard_lista;
        int tamanho = memoria_instrucoes.size();

        //varrendo a memoria de intruções e encontrando os hazards
        for (int i = 0; i < tamanho; i++) {
            const auto& instrucao_atual = memoria_instrucoes[i];

            // hazard de dados
            if (instrucao_atual.r.rs1 > 0 || instrucao_atual.r.rs2 > 0) {
                for (int j = i - 1; j >= i - 2 && j >= 0; j--) {
                    const auto& instrucao_anterior = memoria_instrucoes[j];
                    bool anterior_escreve_reg = instrucao_anterior.r.rd > 0 && instrucao_anterior.tipoInst.find("sw") == string::npos && instrucao_anterior.tipoInst.find("Tipo B") == string::npos;
                    if (anterior_escreve_reg && (instrucao_atual.r.rs1 == instrucao_anterior.r.rd || instrucao_atual.r.rs2 == instrucao_anterior.r.rd)) {
                        hazard h = {"DADOS", j, i, forwarding};
                        hazard_lista.push_back(h);
                        break;
                    }
                }
            }

            // hazard de controle
            if (i > 0 && memoria_instrucoes[i - 1].tipoInst.find("Tipo B") != string::npos) {
                hazard h = {"CONTROLE", i - 1, i, forwarding};
                hazard_lista.push_back(h);
            }
        }
        imprimirComHazardsENops(memoria_instrucoes, hazard_lista);
        gerarDumpfile("dumpfile_com_forwarding.txt", memoria_instrucoes, hazard_lista);
    }
    return 0;
}