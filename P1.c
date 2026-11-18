/*
 * P1.c - Programa 1: converte alimentos.csv em dados.bin.
 */
#include <stdio.h>
#include "alimento.h"

/*
 * Funcao principal do Programa 1.
 * Chama a rotina de alimento.h que le o CSV e grava o arquivo binario.
 * Retorna: 0 (falhas criticas encerram com exit dentro da rotina chamada).
 */
int main(void) {
    int total = converter_csv_para_binario(ARQUIVO_ALIMENTOS_CSV, ARQUIVO_DADOS_BIN);

    printf("P1: %d alimentos gravados em %s\n", total, ARQUIVO_DADOS_BIN);
    return 0;
}
