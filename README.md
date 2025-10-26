# Identificador de Opcode RISC-V — Resumo do Projeto e Progresso

Este repositório contém um analisador/identificador simples de instruções RISC-V escrito em C++.
Este README tem como objetivo listar o que já está implementado, como rodar rapidamente, problemas conhecidos e um plano curto para as próximas duas tardes de trabalho.

## Estrutura dos arquivos

- `main.cpp` — programa principal: lê arquivos em `dumps/` com linhas em hexadecimal, converte para inteiro, chama as rotinas de identificação e imprime um resumo por tipo de instrução.
- `intruction_types_RISCV.hpp` — contém todas as funções e estruturas para identificação de tipos de instrução, extração de campos (rd, rs1, rs2) e imediatos. Observação: o arquivo inclui implementações completas (não apenas declarações).
- `dumps/` — exemplos de dumps usados como entrada (hex strings por linha).

## O que já está implementado (tópicos)

- Identificação de formato/opcode:
	- `opcode_identifier(uint32_t instr)` — identifica o opcode e classifica como Tipo R, I, S, B, U ou J.
	- Funções auxiliares para nomes das instruções por tipo: `R_type_instruction`, `I_type_instruction`, `S_type_instruction`, `B_type_instruction`, `U_type_instruction`, `J_type_instruction`.

- Extração de campos/registradores:
	- `get_field(instr, pos, len)` — extrai um campo de bits genérico.
	- `get_registers(instr, type)` — retorna um struct `Regs` com `rd`, `rs1`, `rs2` e os imediatos decodificados com detalhes.

- Estruturas de dados:
	- `struct Immediates` — campos individuais para cada formato (S, B, U, J) e subcampos (por exemplo `imm_11_5`, `imm_4_0`, `imm_12`, etc.).
	- `struct Regs` — contém registradores, a struct `Immediates` e `total` (valor do imediato com extensão de sinal quando aplicável).

- Decodificação de imediatos (com extensão de sinal):
	- `get_s_imm`, `get_b_imm`, `get_u_imm`, `get_j_imm` — calculam o imediato completo conforme o formato.

- Saída/relatórios:
	- `main.cpp` contabiliza cada tipo de instrução e imprime uma linha por instrução com `rd`, `rs1`, `rs2` e representação dos imediatos.

## Como compilar e executar (rápido)

Observação: estes comandos assumem que você tem um compilador compatível com g++ no Windows (MinGW/MSYS2) ou similar. Se usar MSVC, diga que eu adapto os comandos.

Powershell (exemplo com g++):

```powershell
g++ -std=c++17 -O2 main.cpp -o identificador.exe
./identificador.exe
```

Exemplo de execução (o `main` lê por padrão `dumps/nopsnopsnops.txt`) — ajuste `main.cpp` se quiser outro arquivo.

## Entradas esperadas

- Arquivos em `dumps/` contendo uma instrução por linha em hexadecimal (sem prefixo `0x`). O `main` atualmente usa base 16 (`int base = 16;`) antes de chamar `stoul`.

## Problemas conhecidos / observações técnicas

- O header `intruction_types_RISCV.hpp` contém implementações completas e faz `using namespace std;`. Recomenda-se remover esse `using` de headers para evitar poluição de namespace.
- O nome do arquivo tem uma grafia provável incorreta: `intruction_types_RISCV.hpp` (faltando o "s" em `instruction`). Renomear é recomendado, mas requer atualizar includes.
- Atualmente não há tratamento robusto para linhas inválidas no dump (por exemplo, espaços, caracteres não-hex). Podemos melhorar com verificação e mensagens de erro mais claras.

## Plano curto (hoje e amanhã) — meta para ser rápida e eficiente

- Hoje (tarde):
	1. Criar este README e alinhar entendimento do código (feito).
	2. Aplicar mudanças seguras e rápidas: remover `using namespace std;` do header e encapsular em `namespace riscv { ... }` (opcional). Testar compilação rápida. — preciso do seu OK para aplicar.
	3. Adicionar um `build.bat` simples que execute o comando de compilação (opcional).

- Amanhã (tarde):
	1. Integrar os arquivos do último integrante (quando você os adicionar) e mapear diferenças de estilo/funcionalidade.
	2. Separar implementações grandes em `intruction_types_RISCV.cpp` e deixar o header com declarações (se quisermos seguir convenção), criar `CMakeLists.txt` se for necessário para facilitar builds multiplataforma.
	3. Adicionar pequenos testes unitários (por exemplo um `tests/` com 3 instruções de cada tipo) para validar imediatos e decodificação.

## Como você pode ajudar agora (rápido)

1. Confirme se quer que eu aplique as mudanças seguras agora (remover `using namespace std;` no header e prefixar `std::` — opção recomendada).
2. Se já tem os arquivos do último integrante prontos, faça o upload aqui (ou cole) para que eu inspecione e integre.

## Contato/Anotações de colaboração

- Caso queira que eu faça a refatoração segura, escreva `OK refatorar agora`.
- Caso prefira que eu crie primeiro o `build.bat` ou `CMakeLists.txt`, diga qual prefere (bat ou cmake).

---

Arquivo gerado automaticamente para ajudar no progresso rápido. Se quiser que eu aplique a refatoração agora, responda com `OK refatorar agora`.

sw x5, 12(x10)  rs2, r.total(rs1) , ELE PEGA O VALOR DE X5 (RS2) e guarda assim: registers[r.rs2] e com "write_word_to_memory(addr, registers[r.rs2]);" ele basicamente pega o endereço, e o valor e guarda na memoria 
lw x6, 12(x10)   rd, r.total(rs1) faz o mesmo calculo e guarda no registrador de destino