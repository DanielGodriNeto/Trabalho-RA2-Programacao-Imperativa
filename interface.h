/*
 * interface.h
 * Interacao com o usuario do Programa 2: menu em loop (9 opcoes) e
 * funcoes auxiliares de leitura segura de entrada.
 */
#ifndef INTERFACE_H
#define INTERFACE_H

#include <stddef.h>
#include "estruturas.h"

/* Le uma linha do teclado sem estourar o buffer; devolve 0 em fim de entrada (EOF). */
int ler_linha_segura(const char *mensagem, char *destino, size_t tamanho);

/* Converte texto em inteiro; devolve 1 se o texto inteiro e numerico, 0 caso contrario. */
int texto_para_inteiro(const char *texto, int *valor);

/* Le um inteiro validado; devolve 1 se valido, 0 se invalido, -1 em fim de entrada (EOF). */
int ler_inteiro_seguro(const char *mensagem, int *valor);

/* Le um numero decimal (aceita ponto ou virgula); devolve 1 se valido, 0 caso contrario. */
int ler_decimal_seguro(const char *mensagem, float *valor);

/* Executa o loop do menu ate o usuario escolher a opcao 9 (ou a entrada acabar). */
void executar_menu(NoCategoria **categorias, int *dados_modificados);

#endif
