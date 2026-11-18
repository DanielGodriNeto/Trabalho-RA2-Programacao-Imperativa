/*
 * P2.c - Programa 2: carrega dados.bin em listas/arvores na heap, executa o
 * menu interativo e, ao final, regrava dados.bin (se houve remocao) e libera a memoria.
 */
#include <stdio.h>
#include "estruturas.h"
#include "interface.h"

/*
 * Funcao principal do Programa 2.
 * Retorna: 0 ao terminar normalmente (falhas criticas usam exit nas rotinas chamadas).
 */
int main(void)
{
    NoCategoria *categorias = carregar_dados_bin(ARQUIVO_DADOS_BIN);
    int dados_modificados = 0;
    int total_gravado = 0;

    executar_menu(&categorias, &dados_modificados);

    if (dados_modificados == 1) {
        total_gravado = gravar_dados_bin(ARQUIVO_DADOS_BIN, categorias);
        printf("%s atualizado com %d registros.\n", ARQUIVO_DADOS_BIN, total_gravado);
    }
    liberar_todas_categorias(&categorias);
    return 0;
}
