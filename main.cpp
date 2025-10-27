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


    //id = 01 04c0006f
    //id = 02 ff810113
    //id = 03 0x00000033 nop
    //id = 04 00112023
    //id = 05 00200293
    //id = 06 00a2cc63
    //
    //
    //fazer uma função que lê a lista de hazards e printa que nem ali em cima
    //
    //
    //lembrando que depois felski quer 2 arquivos dumpfile, um com forwarding e outro sem


    //........................................................
    int tamanho = memoria_instrucoes.size();
    instrucoes instrucao;
    int rd_antigo;
    hazard hazard_text;
    vector<hazard> hazard_lista;
    bool forwarding;

    //varrendo a memoria de intruções e encontrando os hazards
    //considerando que o i é até a penultima linha
    for(int i = 0; i <= tamanho-1; i++){
        instrucao = memoria_instrucoes[i];

        //para j como 1 anterior a i em um range(i-2, 0)
        for(int j = i-1; j >= i - 2 && j >= 0; j--){
            rd_antigo = memoria_instrucoes[j].r.rd;

            if(forwarding == false){
                //erro de DADOS caso o rs1[j] ou rs2[j] sejam iguais ao rd[i]
                if((instrucao.r.rs1 == rd_antigo && rd_antigo != 0)  || (instrucao.r.rs2 == rd_antigo && rd_antigo != 0)){
                    hazard_text.tipoErro = "DADOS";
                    hazard_text.indice = i;
                    hazard_text.com_quem = j;
                    hazard_text.forwarding = false;
                    hazard_lista.push_back(hazard_text);
                    /// AQUI COLOCAR 2 NOP ENTRE INDICE E COM QUEM
                }
            }
            else if(j = i-1 && instrucao.tipoInst.find("lw   ")){
                if((instrucao.r.rs1 == rd_antigo && rd_antigo != 0)  || (instrucao.r.rs2 == rd_antigo && rd_antigo != 0)){
                    hazard_text.tipoErro = "DADOS";
                    hazard_text.indice = i;
                    hazard_text.com_quem = j;
                    hazard_text.forwarding = true;
                    hazard_lista.push_back(hazard_text);
                    /// AQUI, SO COLOCAR 1 NOP ENTRE INDICE E COM QUEM
                }
            }

            }
        }
        
        if(instrucao.tipoInst.find("Tipo B - ") && forwarding == false){
            hazard_text.tipoErro = "CONTROLE";
            //aqui está sem indice pois ele vai dar conflito com
            // os três seguintes, então só adiciona 3 nops após ele e deu
            hazard_text.forwarding = false;
            hazard_lista.push_back(hazard_text);
        }

    return 0;
}