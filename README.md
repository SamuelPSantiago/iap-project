# Sistema de Gerenciamento Eleitoral

Projeto desenvolvido para a disciplina **Introdução à Programação**, no curso de **Engenharia da Computação** do **Instituto Federal do Ceará - Campus Fortaleza**.

&nbsp;

## 🧾 Descrição

Este projeto consiste em um sistema de gerenciamento de dados eleitorais utilizando a linguagem **C** com os seguintes recursos:

- **Alocação dinâmica** de memória
- Manipulação de **arquivos binários**
- Separação modular do código em **arquivos `.h` e `.c`**

&nbsp;

## 📂 Entidades e Funcionalidades

### 1. UF (Unidade Federativa)
- **Campos:** código (PK), descrição, sigla
- **Operações:** inserir, alterar, excluir, listar todos, listar individualmente

### 2. Eleição
- **Campos:** ano (PK), código_UF (PK), descrição
- **Operações:** inserir, alterar, excluir, listar todos, listar por eleição

### 3. Pessoa
- **Campos:** CPF (PK), título (K), nome, telefone, endereço, data de nascimento
- **Operações:** inserir, alterar, excluir, listar todos, buscar por título

### 4. Candidato à Eleição
- **Campos:** ano (PK), código_UF (PK), CPF (PK), número
- **Operações:** inserir, excluir, listar candidatos por UF e ano, listar candidatos por ano (ordenado por UF)

### 5. Voto
- **Campos:** ano (PK), código_UF (PK), número_candidato (PK), data/hora (PK)
- **Operações:** inserir, listar votos por candidato e eleição, listar votos por ano (ordenado por UF)

### 6. Comparecimento
- **Campos:** CPF (PK), ano (PK), código_UF (PK)
- **Operações:** mostrar quantidade de comparecimento por UF e ano

&nbsp;

## ✅ Regras de Validação

- Campos marcados como **PK** (chave primária) não podem se repetir.
- Campos obrigatórios (marcados com `*`) não podem estar vazios.
- Referências cruzadas exigem validação de existência em registros anteriores (por exemplo, uma eleição só pode ser criada com um `codigo_UF` válido).

&nbsp;

## 🛠 Estrutura do Código

O projeto está dividido em múltiplos arquivos para manter a organização e reutilização de código:

- `main.h` / `main.c`: ponto de entrada do programa, menu principal e funções gerais
- `uf.h` / `uf.c`: funcionalidades relacionadas às UFs
- `eleicao.h` / `eleicao.c`: funcionalidades das eleições
- `pessoa.h` / `pessoa.c`: funcionalidades das pessoas
- `candidato.h` / `candidato.c`: funcionalidades dos candidatos
- `voto.h` / `voto.c`: funcionalidades dos votos
- `comparecimento.h` / `comparecimento.c`: controle de presença
- `result.h` / `result.c`: funcionalidade de obter os resultados

&nbsp;

## 📌 Requisitos Técnicos

- Compilador C (GCC ou compatível)
- Terminal para entrada/saída de dados
- Sistema de arquivos local para armazenamento dos `.data` binários

&nbsp;

## 🗂 Como Executar

Compile todos os arquivos `.c` com um compilador C:

```bash
gcc -Wall -Wextra *.c -o program
./program
