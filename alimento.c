/*
 * alimento.c
 * Implementacao das funcoes declaradas em alimento.h.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alimento.h"

/*
 * Nomes das categorias, indexados pelo valor do enumerado (posicao 0 nao usada).
 */
static const char *const NOMES_CATEGORIAS[CATEGORIA_MAXIMA + 1] = {
    "Categoria desconhecida",
    "Cereais e derivados",
    "Verduras, hortaliças e derivados",
    "Frutas e derivados",
    "Gorduras e óleos",
    "Pescados e frutos do mar",
    "Carnes e derivados",
    "Leite e derivados",
    "Bebidas (alcoólicas e não alcoólicas)",
    "Ovos e derivados",
    "Produtos açucarados",
    "Miscelâneas",
    "Outros alimentos industrializados",
    "Alimentos preparados",
    "Leguminosas e derivados",
    "Nozes e sementes"
};

/*
 * Converte o enumerado de categoria no nome por extenso.
 * Parametro: categoria - valor do enumerado.
 * Retorna: string constante; valores fora do intervalo geram "Categoria desconhecida".
 */
const char *nome_categoria(CategoriaAlimento categoria)
{
    const char *nome = NOMES_CATEGORIAS[0];

    if (categoria >= CATEGORIA_MINIMA && categoria <= CATEGORIA_MAXIMA) {
        nome = NOMES_CATEGORIAS[categoria];
    }
    return nome;
}

/*
 * Converte uma linha do CSV em RegistroAlimento (sem usar heap: o registro
 * pertence ao chamador e e preenchido por ponteiro).
 * Parametros: linha - texto da linha; registro - destino a preencher.
 * Retorna: 1 se valida; 0 se invalida (o registro nao e alterado).
 */
int linha_csv_para_registro(const char *linha, RegistroAlimento *registro)
{
    int numero = 0;
    int categoria_id = 0;
    float energia = 0.0f;
    float proteina = 0.0f;
    char descricao[TAMANHO_DESCRICAO];
    int campos_lidos;
    int valida = 0;

    descricao[0] = '\0';
    campos_lidos = sscanf(linha, "%d;%127[^;];%f;%f;%d",
                          &numero, descricao, &energia, &proteina, &categoria_id);

    if (campos_lidos == 5 && categoria_id >= CATEGORIA_MINIMA && categoria_id <= CATEGORIA_MAXIMA) {
        memset(registro, 0, sizeof(*registro));
        registro->numero = numero;
        snprintf(registro->descricao, TAMANHO_DESCRICAO, "%s", descricao);
        registro->energia_kcal = energia;
        registro->proteina_g = proteina;
        registro->categoria = (CategoriaAlimento)categoria_id;
        valida = 1;
    }
    return valida;
}

/*
 * Le o CSV linha a linha e grava cada registro com fwrite em dados.bin.
 * Parametros: caminho_csv - entrada; caminho_bin - saida.
 * Retorna: numero de registros gravados. Falha de abertura => exit(EXIT_FAILURE).
 */
int converter_csv_para_binario(const char *caminho_csv, const char *caminho_bin)
{
    FILE *arquivo_csv = fopen(caminho_csv, "r");
    FILE *arquivo_bin = NULL;
    char linha[TAMANHO_LINHA_CSV];
    RegistroAlimento registro;
    int total_gravados = 0;

    if (arquivo_csv == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'.\n", caminho_csv);
        exit(EXIT_FAILURE);
    }
    arquivo_bin = fopen(caminho_bin, "wb");
    if (arquivo_bin == NULL) {
        fprintf(stderr, "Erro: nao foi possivel criar '%s'.\n", caminho_bin);
        fclose(arquivo_csv);
        exit(EXIT_FAILURE);
    }

    while (fgets(linha, sizeof(linha), arquivo_csv) != NULL) {
        if (linha_csv_para_registro(linha, &registro)) {
            fwrite(&registro, sizeof(registro), 1, arquivo_bin);
            total_gravados++;
        }
    }

    fclose(arquivo_csv);
    fclose(arquivo_bin);
    return total_gravados;
}
