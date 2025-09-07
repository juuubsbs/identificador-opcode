#include <iostream>
#include <cstdint> //biblioteca que contém o uint
#include <fstream>
//#include "intruction_types_RISCV.hpp"

//tentando criar um programa que recebe um arquivo, lê linha por linha,
// aplica a mascara de bits e descobre qual tipo de opcode ele é 

//considerando um arquivo de entrada em hexadecimal diga o tipo da instrução
//diga quantas instruções há de cada tipo (add, addi, sub...)
//hazard

//conflito estrutural é questão de prova


//passa pra biblioteca
uint32_t isolate_mask(uint32_t current_hexa, uint32_t itembit ){
    uint32_t result = current_hexa & itembit;
    return result;
}

//passa pra biblioteca
std::string opcode_identifier(uint32_t opcode){
    switch(opcode){
        //tipo R 
        case 0x33:
            return "Tipo R";
        //tipo I
        case 0x67:
        case 0x03:
        case 0x13:
        case 0x0F:
        case 0x73:
            return "Tipo I";
        //tipo S
        case 0x23:
            return "Tipo S";
        // tipo B
        case 0x63:
            return "Tipo B";
        // tipo U
        case 0x37:
        case 0x17:
            return "Tipo U";
        // tipo J
        case 0x6F:
            return "Tipo J";
        default:
            return "Tipo desconhecido";
    }
}




int main(){
    int cont_line = 0;
    uint32_t opcodebit = 0x0000007F; //cara que eu uso pra isolar os 7 primeiros bits (opcode)
    uint32_t opcode; //cara que vai receber os opcodes pra mim
    std::string instype;

    std::ifstream meu_arquivo;
    meu_arquivo.open("hexa_dump.txt");

    std::string current_line;
    uint32_t current_hexa;

    while(std::getline(meu_arquivo, current_line)){
        cont_line++;

        current_hexa = std::stoul(current_line, nullptr, 16); //converte pra hexadecimal a string da linha

        opcode = isolate_mask(current_hexa, opcodebit);
        instype = opcode_identifier(opcode);



        std:: cout << "\nLinha " << cont_line << " instrucao '" << current_line << "' eh: " << instype;
    }
    
}