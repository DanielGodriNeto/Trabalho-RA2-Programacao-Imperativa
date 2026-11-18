/*
 * estruturas.c
 * Implementacao das listas ligadas e arvores binarias na memoria heap.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estruturas.h"

/*
 * Letra base (sem acento) das letras latinas UTF-8 iniciadas por 0xC3.
 * Indexada por (segundo_byte - 0x80) & 0x1F.
 */
static const char LETRAS_BASE_LATIN1[] = "aaaaaa?ceeeeiiiidnoooooxouuuuy?s";

/*
 * Encerra o programa quando malloc/calloc falha (memoria insuficiente).
 */
static void abortar_sem_memoria(void)
{
    fprintf(stderr, "Erro critico: memoria insuficiente (malloc/calloc falhou).\n");
    exit(EXIT_FAILURE);
}

/*
 * Extrai o proximo caractere "logico" do texto, ja minusculo e sem acento,
 * e avanca o ponteiro (1 byte, ou 2 bytes para letras acentuadas UTF-8).
 * Parametro: texto - endereco do ponteiro de leitura (avancado aqui).
 * Retorna: codigo do caractere normalizado; 0 no fim do texto.
 */
static int proximo_caractere_normalizado(const char **texto)
{
    const unsigned char *posicao = (const unsigned char *)*texto;
    int caractere = 0;

    if (posicao[0] == 0xC3 && posicao[1] >= 0x80 && posicao[1] <= 0xBF) {
        caractere = LETRAS_BASE_LATIN1[(posicao[1] - 0x80) & 0x1F];
        *texto += 2;
    } else if (posicao[0] != '\0') {
        caractere = tolower(posicao[0]);
        *texto += 1;
    }
    return caractere;
}

/*
 * Compara dois textos alfabeticamente, ignorando maiusculas e acentos.
 * Em caso de empate usa strcmp para desempate deterministico.
 * Parametros: texto_a, texto_b - strings terminadas em '\0'.
 * Retorna: <0 se a vem antes de b, 0 se iguais, >0 se a vem depois.
 */
int comparar_texto_alfabetico(const char *texto_a, const char *texto_b)
{
    const char *leitura_a = texto_a;
    const char *leitura_b = texto_b;
    int caractere_a = proximo_caractere_normalizado(&leitura_a);
    int caractere_b = proximo_caractere_normalizado(&leitura_b);
    int resultado;

    while (caractere_a != 0 && caractere_a == caractere_b) {
        caractere_a = proximo_caractere_normalizado(&leitura_a);
        caractere_b = proximo_caractere_normalizado(&leitura_b);
    }
    resultado = caractere_a - caractere_b;
    if (resultado == 0) {
        resultado = strcmp(texto_a, texto_b);
    }
    return resultado;
}

/*
 * Procura uma categoria na lista (percorrendo ponteiros, sem alocar).
 * Parametros: inicio - primeiro no; categoria - valor do enumerado procurado.
 * Retorna: no encontrado ou NULL.
 */
NoCategoria *buscar_categoria(NoCategoria *inicio, CategoriaAlimento categoria)
{
    NoCategoria *atual = inicio;

    while (atual != NULL && atual->categoria != categoria) {
        atual = atual->proximo;
    }
    return atual;
}

/*
 * Obtem o no de uma categoria; se nao existir, aloca (calloc) um novo no e o
 * liga diretamente na posicao alfabetica correta (sem ordenar depois).
 * Parametros: inicio - endereco do ponteiro para o primeiro no (pode mudar);
 *             categoria - categoria desejada.
 * Retorna: no da categoria (existente ou recem-criado).
 */
NoCategoria *obter_ou_criar_categoria(NoCategoria **inicio, CategoriaAlimento categoria)
{
    const char *nome = nome_categoria(categoria);
    NoCategoria **cursor = inicio;
    NoCategoria *resultado = NULL;
    NoCategoria *novo = NULL;

    /* Avanca ate a primeira posicao cujo nome nao seja menor que o novo */
    while (*cursor != NULL && comparar_texto_alfabetico((*cursor)->nome, nome) < 0) {
        cursor = &(*cursor)->proximo;
    }

    if (*cursor != NULL && (*cursor)->categoria == categoria) {
        resultado = *cursor;
    } else {
        novo = (NoCategoria *)calloc(1, sizeof(NoCategoria));
        if (novo == NULL) {
        abortar_sem_memoria();
    }
        novo->categoria = categoria;
        snprintf(novo->nome, TAMANHO_NOME_CATEGORIA, "%s", nome);
        novo->proximo = *cursor;   /* o novo no aponta para o sucessor */
        *cursor = novo;            /* o antecessor (ou o inicio) aponta para o novo */
        resultado = novo;
    }
    return resultado;
}

/*
 * Aloca (malloc) um no de alimento e o insere na posicao alfabetica da
 * descricao dentro da lista da categoria.
 * Parametros: categoria - no dono da lista; registro - dados a copiar.
 */
void inserir_alimento_ordenado(NoCategoria *categoria, const RegistroAlimento *registro)
{
    NoAlimento **cursor = &categoria->lista_alimentos;
    NoAlimento *novo = (NoAlimento *)malloc(sizeof(NoAlimento));

    if (novo == NULL) {
        abortar_sem_memoria();
    }
    novo->dados = *registro;

    while (*cursor != NULL &&
           comparar_texto_alfabetico((*cursor)->dados.descricao, registro->descricao) <= 0) {
        cursor = &(*cursor)->proximo;
    }
    novo->proximo = *cursor;
    *cursor = novo;
}

/*
 * Insere recursivamente na arvore binaria de busca (alocando com malloc).
 * Chaves menores vao para a esquerda; maiores ou iguais, para a direita.
 * Parametros: raiz - raiz da (sub)arvore; chave - valor indexado;
 *             alimento - no da lista ligada correspondente.
 * Retorna: raiz da (sub)arvore apos a insercao.
 */
NoArvore *inserir_na_arvore(NoArvore *raiz, float chave, NoAlimento *alimento)
{
    if (raiz == NULL) {
        raiz = (NoArvore *)malloc(sizeof(NoArvore));
        if (raiz == NULL) {
            abortar_sem_memoria();
        }
        raiz->chave = chave;
        raiz->alimento = alimento;
        raiz->esquerda = NULL;
        raiz->direita = NULL;
    } else if (chave < raiz->chave) {
        raiz->esquerda = inserir_na_arvore(raiz->esquerda, chave, alimento);
    } else {
        raiz->direita = inserir_na_arvore(raiz->direita, chave, alimento);
    }
    return raiz;
}

/*
 * Libera (free) todos os nos de uma arvore em pos-ordem.
 * Os alimentos apontados NAO sao liberados (pertencem a lista ligada).
 * Parametro: raiz - raiz da arvore (NULL e aceito).
 */
void liberar_arvore(NoArvore *raiz)
{
    if (raiz != NULL) {
        liberar_arvore(raiz->esquerda);
        liberar_arvore(raiz->direita);
        free(raiz);
    }
}

/*
 * Libera (free) cada no de uma lista ligada de alimentos.
 * Parametro: inicio - primeiro no (NULL e aceito).
 */
void liberar_lista_alimentos(NoAlimento *inicio)
{
    NoAlimento *atual = inicio;
    NoAlimento *proximo = NULL;

    while (atual != NULL) {
        proximo = atual->proximo;   /* guarda o sucessor antes do free */
        free(atual);
        atual = proximo;
    }
}

/*
 * Libera as arvores de energia e proteina da categoria e zera os ponteiros.
 * Parametro: categoria - no da categoria.
 */
void liberar_arvores_categoria(NoCategoria *categoria)
{
    liberar_arvore(categoria->arvore_energia);
    liberar_arvore(categoria->arvore_proteina);
    categoria->arvore_energia = NULL;
    categoria->arvore_proteina = NULL;
}

/*
 * (Re)constroi as duas arvores de indexacao da categoria: libera as antigas
 * e insere cada alimento da lista nas arvores de energia e de proteina.
 * Parametro: categoria - no da categoria.
 */
void construir_arvores_categoria(NoCategoria *categoria)
{
    NoAlimento *alimento = NULL;

    liberar_arvores_categoria(categoria);
    alimento = categoria->lista_alimentos;
    while (alimento != NULL) {
        categoria->arvore_energia = inserir_na_arvore(categoria->arvore_energia,
                                                      alimento->dados.energia_kcal, alimento);
        categoria->arvore_proteina = inserir_na_arvore(categoria->arvore_proteina,
                                                       alimento->dados.proteina_g, alimento);
        alimento = alimento->proximo;
    }
}

/*
 * Le dados.bin e constroi toda a estrutura na heap: lista de categorias
 * alfabetica, listas de alimentos alfabeticas e arvores de indexacao.
 * Parametro: caminho_bin - arquivo binario gerado por P1.
 * Retorna: primeiro no da lista Categorias. Falha de abertura => exit.
 */
NoCategoria *carregar_dados_bin(const char *caminho_bin)
{
    FILE *arquivo = fopen(caminho_bin, "rb");
    NoCategoria *inicio = NULL;
    NoCategoria *categoria = NULL;
    RegistroAlimento registro;

    if (arquivo == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'. Execute o P1 antes.\n", caminho_bin);
        exit(EXIT_FAILURE);
    }

    while (fread(&registro, sizeof(registro), 1, arquivo) == 1) {
        categoria = obter_ou_criar_categoria(&inicio, registro.categoria);
        inserir_alimento_ordenado(categoria, &registro);
    }
    fclose(arquivo);

    categoria = inicio;
    while (categoria != NULL) {
        construir_arvores_categoria(categoria);
        categoria = categoria->proximo;
    }
    return inicio;
}

/*
 * Percurso em ordem decrescente: direita, raiz, esquerda.
 * Parametros: raiz - arvore; visitar - funcao chamada para cada alimento.
 */
void percorrer_decrescente(const NoArvore *raiz, VisitanteAlimento visitar)
{
    if (raiz != NULL) {
        percorrer_decrescente(raiz->direita, visitar);
        visitar(&raiz->alimento->dados);
        percorrer_decrescente(raiz->esquerda, visitar);
    }
}

/*
 * Busca por intervalo [minimo, maximo] com poda: so desce a esquerda se
 * raiz->chave >= minimo e so desce a direita se raiz->chave <= maximo.
 * Os alimentos saem em ordem crescente de chave.
 * Retorna: quantidade de alimentos visitados.
 */
int percorrer_intervalo(const NoArvore *raiz, float minimo, float maximo, VisitanteAlimento visitar)
{
    int encontrados = 0;

    if (raiz != NULL) {
        if (raiz->chave >= minimo) {
            encontrados += percorrer_intervalo(raiz->esquerda, minimo, maximo, visitar);
        }
        if (raiz->chave >= minimo && raiz->chave <= maximo) {
            visitar(&raiz->alimento->dados);
            encontrados++;
        }
        if (raiz->chave <= maximo) {
            encontrados += percorrer_intervalo(raiz->direita, minimo, maximo, visitar);
        }
    }
    return encontrados;
}

/*
 * Conta os alimentos da lista de uma categoria.
 */
int contar_alimentos(const NoCategoria *categoria)
{
    int total = 0;
    const NoAlimento *atual = categoria->lista_alimentos;

    while (atual != NULL) {
        total++;
        atual = atual->proximo;
    }
    return total;
}

/*
 * Remove uma categoria completa: desliga o no da lista e libera (free) a
 * arvore de energia, a de proteina, todos os nos de alimentos e o proprio no.
 * Parametros: inicio - endereco do ponteiro do primeiro no; categoria - alvo.
 * Retorna: quantidade de alimentos liberados (0 se a categoria nao existia).
 */
int remover_categoria(NoCategoria **inicio, CategoriaAlimento categoria)
{
    NoCategoria **cursor = inicio;
    NoCategoria *alvo = NULL;
    int removidos = 0;

    while (*cursor != NULL && (*cursor)->categoria != categoria) {
        cursor = &(*cursor)->proximo;
    }
    if (*cursor != NULL) {
        alvo = *cursor;
        *cursor = alvo->proximo;            /* desliga o no da lista */
        removidos = contar_alimentos(alvo);
        liberar_arvores_categoria(alvo);
        liberar_lista_alimentos(alvo->lista_alimentos);
        free(alvo);
    }
    return removidos;
}

/*
 * Remove um alimento da categoria, liberando seu no, e reconstroi as duas
 * arvores (antigas liberadas, novas alocadas) para manter a indexacao correta.
 * Parametros: categoria - no da categoria; numero - numero TACO (se > 0);
 *             descricao - descricao exata (usada quando numero <= 0).
 * Retorna: 1 se removeu, 0 se nao encontrou.
 */
int remover_alimento(NoCategoria *categoria, int numero, const char *descricao)
{
    NoAlimento **cursor = &categoria->lista_alimentos;
    NoAlimento *alvo = NULL;
    int achou = 0;

    while (*cursor != NULL && achou == 0) {
        if (numero > 0) {
            achou = ((*cursor)->dados.numero == numero);
        } else {
            achou = (descricao != NULL &&
                     comparar_texto_alfabetico((*cursor)->dados.descricao, descricao) == 0);
        }
        if (achou == 0) {
            cursor = &(*cursor)->proximo;
        }
    }
    if (achou == 1) {
        liberar_arvores_categoria(categoria);   /* as arvores apontam para o no a ser liberado */
        alvo = *cursor;
        *cursor = alvo->proximo;                /* desliga o no da lista */
        free(alvo);
        construir_arvores_categoria(categoria);
    }
    return achou;
}

/*
 * Regrava dados.bin percorrendo categorias e alimentos restantes.
 * Parametros: caminho_bin - arquivo a sobrescrever; inicio - lista Categorias.
 * Retorna: total de registros gravados. Falha de abertura => exit.
 */
int gravar_dados_bin(const char *caminho_bin, const NoCategoria *inicio)
{
    FILE *arquivo = fopen(caminho_bin, "wb");
    const NoCategoria *categoria = inicio;
    const NoAlimento *alimento = NULL;
    int total = 0;

    if (arquivo == NULL) {
        fprintf(stderr, "Erro: nao foi possivel gravar '%s'.\n", caminho_bin);
        exit(EXIT_FAILURE);
    }
    while (categoria != NULL) {
        alimento = categoria->lista_alimentos;
        while (alimento != NULL) {
            fwrite(&alimento->dados, sizeof(RegistroAlimento), 1, arquivo);
            total++;
            alimento = alimento->proximo;
        }
        categoria = categoria->proximo;
    }
    fclose(arquivo);
    return total;
}

/*
 * Libera toda a memoria heap: para cada categoria libera as duas arvores,
 * a lista de alimentos e o no; ao final zera o ponteiro inicial.
 * Parametro: inicio - endereco do ponteiro para a lista Categorias.
 */
void liberar_todas_categorias(NoCategoria **inicio)
{
    NoCategoria *atual = *inicio;
    NoCategoria *proxima = NULL;

    while (atual != NULL) {
        proxima = atual->proximo;
        liberar_arvores_categoria(atual);
        liberar_lista_alimentos(atual->lista_alimentos);
        free(atual);
        atual = proxima;
    }
    *inicio = NULL;
}
