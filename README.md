# Trabalho do RA2 — Programação Imperativa (Tabela TACO)

Implementação em C (padrão C99) dos programas **P1** e **P2** sobre os 597 alimentos e 15 categorias da **Tabela Brasileira de Composição de Alimentos (TACO — 4ª edição ampliada e revisada)**.

---

## 1. Organização dos Arquivos

- `alimento.h` / `alimento.c`: definição do enumerado `CategoriaAlimento` (1 a 15), da estrutura `RegistroAlimento` (144 bytes, unidade gravada em `dados.bin`), conversão do enumerado em texto e leitura de `alimentos.csv` para gravação binária.
- `estruturas.h` / `estruturas.c`: estruturas dinâmicas na memória *heap* (`NoCategoria`, `NoAlimento`, `NoArvore`), inserção diretamente em ordem alfabética durante a construção das listas ligadas, árvores binárias de indexação por energia (kcal) e por proteína (g), buscas com poda por intervalo, remoções, regravação de `dados.bin` e liberação completa da memória (`free`).
- `interface.h` / `interface.c`: menu interativo do Programa 2 (9 opções) e funções auxiliares de leitura segura (`fgets` + `strtol`/`strtod`).
- `P1.c`: contém exclusivamente a função `main` do Programa 1 (`alimentos.csv` → `dados.bin`).
- `P2.c`: contém exclusivamente a função `main` do Programa 2 (carrega `dados.bin`, executa o menu, atualiza `dados.bin` se houve remoção e libera 100% da memória *heap*).
- `Makefile`: compilação com `gcc -Wall -Wextra -pedantic -std=c99`.
- `alimentos.csv`: base com os 597 alimentos transcritos da Tabela 1 da TACO (`numero;descricao;energia_kcal;proteina_g;categoria_id`). É a fonte dos dados: o projeto é só C, o P1 lê este CSV.

---

## 2. Compilação e Execução

```bash
# Compilar P1 e P2 (Linux/macOS: make | Windows MinGW: mingw32-make)
make

# Executar o Programa 1 (gera dados.bin com 597 registros = 85.968 bytes)
./P1

# Executar o Programa 2 (menu interativo com as 9 opcoes)
./P2
```

No Windows (PowerShell/CMD), execute `chcp 65001` antes de rodar `P2.exe` para exibir corretamente os caracteres UTF-8 acentuados.

---

## 3. Tratamento de Valores Especiais da Tabela TACO em `alimentos.csv`

A Tabela 1 da TACO (4ª edição) utiliza símbolos não numéricos definidos na **nota de rodapé da página 68 do PDF oficial**. Para que todos os 597 alimentos possam ser armazenados em campos numéricos (`float energia_kcal` e `float proteina_g` em `RegistroAlimento`) e indexados nas árvores binárias de busca, adotou-se a convenção **`0.0`** para todos os campos não numéricos:

1. **`NA` (Não Aplicável) → `0.0`**
   - **Proteína em azeites e óleos (`#259`, `#260`, `#267` a `#272`)**: Azeite de dendê (`#259`), Azeite de oliva extra virgem (`#260`) e Óleos de babaçu, canola, girassol, milho, pequi e soja (`#267` a `#272`) possuem `884.0 kcal` de energia e `NA` na coluna de proteína (convertido para `0.0`).
   - **Proteína em bebida destilada (`#472`)**: Cana, aguardente (`#472`) possui `216.0 kcal` e proteína `NA` (convertida para `0.0`).
   - **Energia e proteína em sais (`#516` e `#517`)**: Sal dietético (`#516`) e Sal grosso (`#517`) possuem `NA` tanto em energia quanto em proteína na tabela oficial (ambos convertidos para `0.0`).

2. **`Tr` (Traço) → `0.0`**
   - Conforme a página 68 da TACO, `Tr` indica nutrientes presentes em quantidades vestigiais (abaixo do limite de quantificação ou arredondados para zero), como a proteína em `#258` (Uva, suco concentrado), `#263` a `#266` (Margarinas), `#473` (Cana, caldo de), entre outros. Todos os campos `Tr` foram convertidos para `0.0`.

3. **`*` (Análises sendo reavaliadas) → `0.0`**
   - Conforme a nota de rodapé da página 68 da TACO (*"\* as análises estão sendo reavaliadas"*), o PDF oficial da 4ª edição **não fornece valores numéricos** de energia nem de proteína (nem no corpo da tabela, nem no rodapé) para **4 alimentos**:
     - `#450` — `Iogurte, sabor abacaxi` (Categoria 7: Leite e derivados)
     - `#457` — `Leite, de vaca, desnatado, UHT` (Categoria 7: Leite e derivados)
     - `#458` — `Leite, de vaca, integral` (Categoria 7: Leite e derivados)
     - `#591` — `Coco, verde, cru` (Categoria 15: Nozes e sementes)
   - Como os valores reais não constam no documento oficial da TACO, esses 4 registros tiveram energia e proteína preenchidas com `0.0`.
