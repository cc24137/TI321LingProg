#include "tipos.h"
#include "tokens.h"
#include "analisadorLexico.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIMBOLOS 1000
simbolo tabela_simbolos[MAX_SIMBOLOS];
int topo = 0; 

char temNaTabelaSimbolos(char *nome) {
    for (int i = topo - 1; i >= 0; i--) {
        if (strcmp(tabela_simbolos[i].nome, nome) == 0) {
            return 1;
        }
    }
    return 0;
}

void adicionaNaTabelaSimbolos(char *nome, token tipo, char contexto) {
    if (topo >= MAX_SIMBOLOS) {
        printf("Tabela de simbolos cheia!\n");
        exit(1);
    }
    
    // insere direto no topo
    strcpy(tabela_simbolos[topo].nome, nome);
    tabela_simbolos[topo].tipo = tipo;
    tabela_simbolos[topo].contexto = contexto;
    
    topo++;
}

void apagaContextoTabelaSimbolos(char contexto) {
    // recua o topo ate o fim do contexto para "apagar o contexto"
    while (topo > 0 && tabela_simbolos[topo - 1].contexto == contexto) {
        topo--; 
    }
}
