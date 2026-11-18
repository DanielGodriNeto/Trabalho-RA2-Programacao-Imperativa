# Trabalho do RA2 — Programação Imperativa

Trabalho da disciplina **Programação Imperativa** (Prof. Alcides Calsavara), em **C (C99)**.
Usa a **Tabela Brasileira de Composição de Alimentos (TACO, 4ª edição)**: 597 alimentos em 15 categorias.

## O que o trabalho pede

| Etapa | O que fazer | Onde está |
|-------|-------------|-----------|
| A | Transcrever número, descrição, energia (kcal), proteína (g) e categoria dos 597 alimentos para um CSV | `alimentos.csv` (gerado por `extrair_taco.py`) |
| B | **P1**: ler o CSV e gravar tudo em binário no arquivo `dados.bin` | `P1.c` |
| C | **P2**: ler `dados.bin`, montar as estruturas na memória e oferecer um menu | `P2.c` |

### Estruturas montadas pelo P2

```
Lista de categorias (ordem alfabética)
  └─ cada categoria tem sua lista de alimentos (ordem alfabética)

Árvore binária por energia   ─┐  cada nó guarda a chave (kcal ou g)
Árvore binária por proteína  ─┘  e um ponteiro para o nó da lista
```

As listas são montadas **já em ordem** (inserção ordenada, sem algoritmo de ordenação).

### Menu do P2

1. Listar categorias
2. Listar alimentos de uma categoria (alfabética)
3. Listar alimentos por energia, do maior para o menor
4. Listar alimentos por proteína, da maior para a menor
5. Listar alimentos com energia dentro de um intervalo
6. Listar alimentos com proteína dentro de um intervalo
7. Remover uma categoria (e seus alimentos)
8. Remover um alimento
9. Encerrar (se algo foi removido, regrava `dados.bin`)

### Regras de código do enunciado

1. Categorias em um `enum`
2. Código dividido em funções
3. Arquivos `.h` e `.c`; `P1.c` e `P2.c` só com a `main`
4. Listas com `malloc`/`calloc`
5. Toda a memória é liberada no fim (`free`)
6. Alimentos inseridos já em ordem alfabética
7. Categorias inseridas já em ordem alfabética
8. Nó da árvore: chave + ponteiro para o nó da lista
9. Sem `break` e sem `continue`
10. Um único `return` por função
11. Nomes de variáveis e funções significativos
12. Código comentado

## Arquivos

| Arquivo | Conteúdo |
|---------|----------|
| `alimento.h` / `alimento.c` | `enum CategoriaAlimento`, registro gravado no `dados.bin` (144 bytes) e conversão CSV → binário |
| `estruturas.h` / `estruturas.c` | listas, árvores, buscas, remoções, regravação do `dados.bin` e liberação da memória |
| `interface.h` / `interface.c` | menu e leitura segura de entrada (`fgets` + `strtol`/`strtod`) |
| `P1.c` | `main` do Programa 1 |
| `P2.c` | `main` do Programa 2 |
| `extrair_taco.py` | extrai os dados do PDF da TACO e gera o CSV |
| `Makefile` | compila com `gcc -Wall -Wextra -pedantic -std=c99` |

## Como compilar e rodar

```bash
make          # Windows com MinGW: mingw32-make
./P1          # gera dados.bin (597 registros = 85.968 bytes)
./P2          # abre o menu
```

No Windows, rode `chcp 65001` antes do `P2.exe` para os acentos aparecerem certos.
