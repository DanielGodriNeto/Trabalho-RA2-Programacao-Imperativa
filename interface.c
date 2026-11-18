/*
 * interface.c
 * Implementacao do menu e da leitura segura de entrada do Programa 2.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"

#define TAMANHO_ENTRADA 256
#define OPCAO_ENCERRAR 9

/*
 * Le uma linha com fgets e remove o '\n' final.
 * Parametros: mensagem - texto exibido antes (pode ser NULL); destino - buffer
 *             do chamador; tamanho - capacidade do buffer.
 * Retorna: 1 se leu uma linha; 0 se a entrada terminou (EOF).
 */
int ler_linha_segura(const char *mensagem, char *destino, size_t tamanho)
{
    int leu = 0;
    size_t comprimento;

    if (mensagem != NULL) {
        printf("%s", mensagem);
        fflush(stdout);
    }
    if (fgets(destino, (int)tamanho, stdin) != NULL) {
        comprimento = strlen(destino);
        while (comprimento > 0 && (destino[comprimento - 1] == '\n' || destino[comprimento - 1] == '\r')) {
            destino[comprimento - 1] = '\0';
            comprimento--;
        }
        leu = 1;
    } else {
        destino[0] = '\0';
    }
    return leu;
}

/*
 * Le um inteiro com strtol, validando que a linha inteira e numerica.
 * Parametros: mensagem - prompt; valor - destino do resultado.
 * Retorna: 1 se valido; 0 se invalido ou EOF.
 */
int ler_inteiro_seguro(const char *mensagem, int *valor)
{
    char entrada[TAMANHO_ENTRADA];
    char *fim = NULL;
    long convertido = 0;
    int valido = 0;

    if (ler_linha_segura(mensagem, entrada, sizeof(entrada))) {
        convertido = strtol(entrada, &fim, 10);
        if (fim != entrada && *fim == '\0') {
            *valor = (int)convertido;
            valido = 1;
        }
    }
    return valido;
}

/*
 * Le um decimal com strtod; troca virgula por ponto antes da conversao.
 * Parametros: mensagem - prompt; valor - destino do resultado.
 * Retorna: 1 se valido; 0 se invalido ou EOF.
 */
int ler_decimal_seguro(const char *mensagem, float *valor)
{
    char entrada[TAMANHO_ENTRADA];
    char *fim = NULL;
    char *virgula = NULL;
    double convertido = 0.0;
    int valido = 0;

    if (ler_linha_segura(mensagem, entrada, sizeof(entrada))) {
        virgula = strchr(entrada, ',');
        if (virgula != NULL) {
            *virgula = '.';
        }
        convertido = strtod(entrada, &fim);
        if (fim != entrada && *fim == '\0') {
            *valor = (float)convertido;
            valido = 1;
        }
    }
    return valido;
}

/*
 * Funcao de visita: imprime uma linha com os dados de um alimento.
 * Parametro: alimento - registro a exibir (somente leitura).
 */
static void imprimir_alimento(const RegistroAlimento *alimento)
{
    printf("  %3d | %-62s | %7.1f kcal | %6.1f g\n",
           alimento->numero, alimento->descricao,
           alimento->energia_kcal, alimento->proteina_g);
}

/* Exibe o menu principal com as 9 opcoes. */
static void exibir_menu(void)
{
    printf("\n===== TACO - MENU =====\n");
    printf("1 - Listar categorias\n");
    printf("2 - Listar alimentos de uma categoria (ordem alfabetica)\n");
    printf("3 - Listar alimentos de uma categoria por energia decrescente\n");
    printf("4 - Listar alimentos de uma categoria por proteina decrescente\n");
    printf("5 - Listar alimentos de uma categoria com energia entre [min, max]\n");
    printf("6 - Listar alimentos de uma categoria com proteina entre [min, max]\n");
    printf("7 - Remover uma categoria\n");
    printf("8 - Remover um alimento\n");
    printf("9 - Encerrar\n");
}

/*
 * Le a opcao do menu. Fim de entrada equivale a escolher a opcao 9.
 * Retorna: opcao digitada; 0 se invalida; 9 em EOF.
 */
static int ler_opcao_menu(void)
{
    char entrada[TAMANHO_ENTRADA];
    int opcao = 0;
    int leu = ler_linha_segura("Opcao: ", entrada, sizeof(entrada));
    char *fim = NULL;
    long convertido;

    if (leu == 0) {
        opcao = OPCAO_ENCERRAR;
    } else {
        convertido = strtol(entrada, &fim, 10);
        if (fim != entrada && *fim == '\0') {
            opcao = (int)convertido;
        }
    }
    return opcao;
}

/*
 * Opcao 1: lista as categorias na ordem da lista ligada Categorias.
 * Parametro: inicio - primeiro no da lista.
 */
static void listar_categorias(const NoCategoria *inicio)
{
    const NoCategoria *atual = inicio;

    printf("\nCategorias (ordem alfabetica):\n");
    while (atual != NULL) {
        printf("  [%2d] %s (%d alimentos)\n", (int)atual->categoria, atual->nome, contar_alimentos(atual));
        atual = atual->proximo;
    }
}

/*
 * Pede o identificador (1 a 15) e localiza a categoria na lista.
 * Parametro: inicio - primeiro no da lista.
 * Retorna: no encontrado ou NULL (mensagem de erro ja exibida).
 */
static NoCategoria *escolher_categoria(NoCategoria *inicio)
{
    int identificador = 0;
    NoCategoria *encontrada = NULL;

    listar_categorias(inicio);
    if (ler_inteiro_seguro("Informe o numero da categoria [1-15]: ", &identificador)) {
        encontrada = buscar_categoria(inicio, (CategoriaAlimento)identificador);
        if (encontrada == NULL) {
            printf("Categoria %d inexistente (ou ja removida).\n", identificador);
        }
    } else {
        printf("Entrada invalida.\n");
    }
    return encontrada;
}

/*
 * Opcao 2: lista os alimentos na ordem alfabetica da lista ligada.
 */
static void listar_alimentos_alfabetico(const NoCategoria *categoria)
{
    const NoAlimento *atual = categoria->lista_alimentos;

    printf("\n%s - ordem alfabetica:\n", categoria->nome);
    while (atual != NULL) {
        imprimir_alimento(&atual->dados);
        atual = atual->proximo;
    }
}

/*
 * Opcoes 3 e 4: lista em ordem decrescente usando a arvore indicada.
 * Parametros: categoria - para o titulo; arvore - indice a percorrer; rotulo - "energia"/"proteina".
 */
static void listar_decrescente(const NoCategoria *categoria, const NoArvore *arvore, const char *rotulo)
{
    printf("\n%s - %s decrescente:\n", categoria->nome, rotulo);
    percorrer_decrescente(arvore, imprimir_alimento);
}

/*
 * Opcoes 5 e 6: pede minimo e maximo e lista os alimentos no intervalo
 * usando a busca com poda na arvore indicada.
 */
static void listar_intervalo(const NoCategoria *categoria, const NoArvore *arvore, const char *rotulo)
{
    float minimo = 0.0f;
    float maximo = 0.0f;
    int encontrados;

    if (ler_decimal_seguro("Valor minimo: ", &minimo) && ler_decimal_seguro("Valor maximo: ", &maximo)) {
        if (minimo > maximo) {
            printf("O minimo nao pode ser maior que o maximo.\n");
        } else {
            printf("\n%s - %s entre [%.1f, %.1f]:\n", categoria->nome, rotulo, minimo, maximo);
            encontrados = percorrer_intervalo(arvore, minimo, maximo, imprimir_alimento);
            printf("Total encontrado: %d\n", encontrados);
        }
    } else {
        printf("Valor invalido.\n");
    }
}

/*
 * Opcao 7: remove uma categoria inteira e marca dados_modificados.
 */
static void tratar_remocao_categoria(NoCategoria **categorias, int *dados_modificados)
{
    NoCategoria *escolhida = escolher_categoria(*categorias);
    CategoriaAlimento identificador;
    int removidos;

    if (escolhida != NULL) {
        identificador = escolhida->categoria;   /* guarda antes de o no ser liberado */
        printf("Removendo categoria '%s'...\n", escolhida->nome);
        removidos = remover_categoria(categorias, identificador);
        printf("Categoria removida: %d alimentos liberados da memoria.\n", removidos);
        *dados_modificados = 1;
    }
}

/*
 * Opcao 8: remove um alimento por numero ou descricao exata.
 * Se a categoria ficar vazia, ela tambem e removida.
 */
static void tratar_remocao_alimento(NoCategoria **categorias, int *dados_modificados)
{
    NoCategoria *escolhida = escolher_categoria(*categorias);
    char entrada[TAMANHO_ENTRADA];
    char *fim = NULL;
    long numero = 0;
    int removido = 0;

    if (escolhida != NULL) {
        listar_alimentos_alfabetico(escolhida);
        if (ler_linha_segura("Numero ou descricao exata do alimento: ", entrada, sizeof(entrada))) {
            numero = strtol(entrada, &fim, 10);
            if (fim != entrada && *fim == '\0') {
                removido = remover_alimento(escolhida, (int)numero, NULL);
            } else {
                removido = remover_alimento(escolhida, 0, entrada);
            }
        }
        if (removido == 1) {
            printf("Alimento removido; arvores de energia e proteina reconstruidas.\n");
            *dados_modificados = 1;
            if (escolhida->lista_alimentos == NULL) {
                printf("A categoria ficou vazia e tambem foi removida.\n");
                remover_categoria(categorias, escolhida->categoria);
            }
        } else {
            printf("Alimento nao encontrado nessa categoria.\n");
        }
    }
}

/*
 * Loop principal do menu (usa apenas if / else if / else).
 * Parametros: categorias - endereco do ponteiro da lista (pode mudar com a opcao 7);
 *             dados_modificados - flag que passa a 1 se algo for removido.
 */
void executar_menu(NoCategoria **categorias, int *dados_modificados)
{
    int opcao = 0;
    NoCategoria *categoria = NULL;

    while (opcao != OPCAO_ENCERRAR) {
        exibir_menu();
        opcao = ler_opcao_menu();

        if (opcao == 1) {
            listar_categorias(*categorias);
        } else if (opcao >= 2 && opcao <= 8) {
            if (opcao == 7) {
                tratar_remocao_categoria(categorias, dados_modificados);
            } else if (opcao == 8) {
                tratar_remocao_alimento(categorias, dados_modificados);
            } else {
                categoria = escolher_categoria(*categorias);
                if (categoria != NULL) {
                    if (opcao == 2) {
                        listar_alimentos_alfabetico(categoria);
                    } else if (opcao == 3) {
                        listar_decrescente(categoria, categoria->arvore_energia, "energia (kcal)");
                    } else if (opcao == 4) {
                        listar_decrescente(categoria, categoria->arvore_proteina, "proteina (g)");
                    } else if (opcao == 5) {
                        listar_intervalo(categoria, categoria->arvore_energia, "energia (kcal)");
                    } else {
                        listar_intervalo(categoria, categoria->arvore_proteina, "proteina (g)");
                    }
                }
            }
        } else if (opcao == OPCAO_ENCERRAR) {
            printf("Encerrando...\n");
        } else {
            printf("Opcao invalida. Escolha de 1 a 9.\n");
        }
    }
}
