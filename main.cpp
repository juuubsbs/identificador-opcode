#include <iostream>
#include <cstdint> //biblioteca que contém o uint
#include <fstream>
#include "intruction_types_RISCV.hpp"

using namespace std;

//tentando criar um programa que recebe um arquivo, lê linha por linha,
// aplica a mascara de bits e descobre qual tipo de opcode ele é 

//considerando um arquivo de entrada em hexadecimal diga o tipo da instrução
//diga quantas instruções há de cada tipo (add, addi, sub...)
//hazard

//conflito estrutural é questão de prova

int main(){
    int cont_line = 0;
    uint32_t opcodebit = 0x0000007F; //cara que eu uso pra isolar os 7 primeiros bits (opcode)
    uint32_t opcode; //cara que vai receber os opcodes pra mim
    string instype;

    ifstream meu_arquivo;
    meu_arquivo.open("hexa_dump.txt");

    //map: contador, recebe um valor de instype pra somar
    map<string, int> contador;

    string current_line;
    uint32_t current_hexa;

    while(getline(meu_arquivo, current_line)){
      if(current_line.empty()) continue; 
        cont_line++;

        current_hexa = stoul(current_line, nullptr, 16); //converte pra hexadecimal a string da linha

        opcode = isolate_mask(current_hexa, opcodebit);
        instype = opcode_identifier(opcode);

        contador[instype]++;

        cout << "\nLinha " << cont_line << " instrucao '" << current_line << "' eh: " << instype;
    }

    cout << "\n\n===== RESUMO =====\n";
    
    //par é um ponteiro de tipo automático que navega pelo vetor map 
    for(auto &par : contador){
        cout << par.first << " -> " << par.second << " instrucoes\n";
    }

    
}