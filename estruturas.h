/*
 * estruturas.h
 * Estruturas dinamicas (heap) do Programa 2:
 *  - lista ligada de categorias (ordem alfabetica);
 *  - lista ligada de alimentos por categoria (ordem alfabetica da descricao);
 *  - duas arvores binarias de busca por categoria (energia e proteina).
 */
#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include "alimento.h"

#define TAMANHO_NOME_CATEGORIA 64

/*
 * No da lista ligada de alimentos de uma categoria.
 * Alocado com malloc em inserir_alimento_ordenado e liberado com free.
 */
typedef struct NoAlimento {
    RegistroAlimento dados;          /* copia dos dados do alimento     */
    struct NoAlimento *proximo;      /* proximo alimento (ordem alfabetica) */
} NoAlimento;

/*
 * No de arvore binaria de busca que indexa uma categoria.
 * Guarda somente a chave (energia ou proteina), o ponteiro para o no
 * correspondente da lista de alimentos e os filhos.
 */
typedef struct NoArvore {
    float chave;                     /* energia_kcal ou proteina_g       */
    NoAlimento *alimento;            /* aponta para o no na lista ligada */
    struct NoArvore *esquerda;       /* chaves menores                   */
    struct NoArvore *direita;        /* chaves maiores ou iguais         */
} NoArvore;

/*
 * No da lista ligada Categorias (ordem alfabetica do nome).
 */
typedef struct NoCategoria {
    CategoriaAlimento categoria;                 /* identificacao (enumerado)       */
    char nome[TAMANHO_NOME_CATEGORIA];           /* nome da categoria               */
    NoAlimento *lista_alimentos;                 /* lista de alimentos da categoria */
    NoArvore *arvore_energia;                    /* indice por energia (kcal)       */
    NoArvore *arvore_proteina;                   /* indice por proteina (g)         */
    struct NoCategoria *proximo;                 /* proxima categoria               */
} NoCategoria;

/* Tipo de funcao chamada para cada alimento visitado em um percurso de arvore. */
typedef void (*VisitanteAlimento)(const RegistroAlimento *alimento);

/* Compara dois textos em ordem alfabetica (ignora maiusculas e acentos UTF-8). */
int comparar_texto_alfabetico(const char *texto_a, const char *texto_b);

/* Busca a categoria na lista; devolve NULL se nao existir. */
NoCategoria *buscar_categoria(NoCategoria *inicio, CategoriaAlimento categoria);

/* Devolve o no da categoria, criando-o na posicao alfabetica se ainda nao existir. */
NoCategoria *obter_ou_criar_categoria(NoCategoria **inicio, CategoriaAlimento categoria);

/* Insere um alimento ja na posicao alfabetica da lista da categoria. */
void inserir_alimento_ordenado(NoCategoria *categoria, const RegistroAlimento *registro);

/* Insere um no na arvore binaria de busca; devolve a raiz (nova ou a mesma). */
NoArvore *inserir_na_arvore(NoArvore *raiz, float chave, NoAlimento *alimento);

/* Libera todos os nos de uma arvore (nao libera os alimentos apontados). */
void liberar_arvore(NoArvore *raiz);

/* Libera todos os nos de uma lista de alimentos. */
void liberar_lista_alimentos(NoAlimento *inicio);

/* Libera as duas arvores de uma categoria e zera os ponteiros. */
void liberar_arvores_categoria(NoCategoria *categoria);

/* Descarta as arvores antigas e cria duas novas a partir da lista de alimentos. */
void construir_arvores_categoria(NoCategoria *categoria);

/* Le dados.bin e monta lista de categorias, listas de alimentos e arvores. */
NoCategoria *carregar_dados_bin(const char *caminho_bin);

/* Percorre a arvore em ordem decrescente de chave (direita, raiz, esquerda). */
void percorrer_decrescente(const NoArvore *raiz, VisitanteAlimento visitar);

/* Visita os nos com chave em [minimo, maximo], podando subarvores; devolve a quantidade. */
int percorrer_intervalo(const NoArvore *raiz, float minimo, float maximo, VisitanteAlimento visitar);

/* Remove e libera uma categoria inteira; devolve a quantidade de alimentos liberados (0 se nao existia). */
int remover_categoria(NoCategoria **inicio, CategoriaAlimento categoria);

/* Remove um alimento (por numero se numero > 0, senao por descricao); reconstroi as arvores. */
int remover_alimento(NoCategoria *categoria, int numero, const char *descricao);

/* Conta os alimentos de uma categoria. */
int contar_alimentos(const NoCategoria *categoria);

/* Regrava dados.bin com o que restou nas listas; devolve o total de registros. */
int gravar_dados_bin(const char *caminho_bin, const NoCategoria *inicio);

/* Libera 100% da memoria heap das categorias e zera o ponteiro inicial. */
void liberar_todas_categorias(NoCategoria **inicio);

#endif
