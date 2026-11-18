/*
 * alimento.h
 * Tipos e funcoes basicas sobre alimentos da Tabela TACO:
 *  - enumerado das 15 categorias;
 *  - registro de alimento gravado em dados.bin;
 *  - conversao de alimentos.csv para dados.bin (usada pelo programa P1).
 */
#ifndef ALIMENTO_H
#define ALIMENTO_H

#define ARQUIVO_ALIMENTOS_CSV "alimentos.csv"
#define ARQUIVO_DADOS_BIN "dados.bin"
#define TAMANHO_DESCRICAO 128
#define TAMANHO_LINHA_CSV 512

/*
 * Enumerado com as 15 categorias oficiais da TACO.
 * Os valores 1..15 coincidem com a coluna categoria_id de alimentos.csv.
 */
typedef enum CategoriaAlimento {
    CAT_CEREAIS = 1,
    CAT_VERDURAS,
    CAT_FRUTAS,
    CAT_GORDURAS_OLEOS,
    CAT_PESCADOS,
    CAT_CARNES,
    CAT_LEITE,
    CAT_BEBIDAS,
    CAT_OVOS,
    CAT_PRODUTOS_ACUCARADOS,
    CAT_MISCELANEAS,
    CAT_OUTROS_INDUSTRIALIZADOS,
    CAT_ALIMENTOS_PREPARADOS,
    CAT_LEGUMINOSAS,
    CAT_NOZES_SEMENTES
} CategoriaAlimento;

#define CATEGORIA_MINIMA CAT_CEREAIS
#define CATEGORIA_MAXIMA CAT_NOZES_SEMENTES

/*
 * Registro de um alimento. E exatamente a unidade lida e escrita
 * (fread/fwrite) no arquivo binario dados.bin.
 */
typedef struct RegistroAlimento {
    int numero;                          /* numero do alimento na TACO (1..597) */
    char descricao[TAMANHO_DESCRICAO];   /* descricao do alimento               */
    float energia_kcal;                  /* energia em kcal por 100 g           */
    float proteina_g;                    /* proteina em gramas por 100 g        */
    CategoriaAlimento categoria;         /* categoria do alimento               */
} RegistroAlimento;

/*
 * Converte o enumerado de categoria no nome por extenso.
 * Parametro: categoria - valor do enumerado.
 * Retorna: ponteiro para string constante (nao deve ser liberada com free).
 */
const char *nome_categoria(CategoriaAlimento categoria);

/*
 * Converte uma linha do CSV (numero;descricao;energia;proteina;categoria_id)
 * em um RegistroAlimento.
 * Parametros: linha - texto da linha; registro - destino (memoria do chamador).
 * Retorna: 1 se a linha era valida, 0 caso contrario (ex.: linha de cabecalho).
 */
int linha_csv_para_registro(const char *linha, RegistroAlimento *registro);

/*
 * Le o arquivo CSV e grava todos os registros em formato binario.
 * Parametros: caminho_csv - arquivo de entrada; caminho_bin - arquivo de saida.
 * Retorna: quantidade de registros gravados. Em caso de falha ao abrir
 * qualquer arquivo, encerra o programa com exit(EXIT_FAILURE).
 */
int converter_csv_para_binario(const char *caminho_csv, const char *caminho_bin);

#endif
