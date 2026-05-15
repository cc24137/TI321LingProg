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

void adicionaNaTabelaSimbolos(char *nome, char* tipo, char escopo, naturezas natureza) {
    if (topo >= MAX_SIMBOLOS) {
        printf("Tabela de simbolos cheia!\n");
        exit(1);
    }
    
    
    // insere direto no topo
    strcpy(tabela_simbolos[topo].nome, nome);
    strcpy(tabela_simbolos[topo].tipo, tipo); // se não tiver tipo, espera-se receber ""(string vazia)
    //printf("Colocou as strings\n");
    tabela_simbolos[topo].escopo = escopo;
    tabela_simbolos[topo].natureza = natureza;
    // colocar endereço depois;
    
    topo++;
}

void apagaEscopoTabelaSimbolos(char escopo) {
    // recua o topo ate o fim do escopo para "apagar o escopo"
    while (topo > 0 && tabela_simbolos[topo - 1].escopo == escopo) {
        topo--; 
    }
}

void printaTabela(){
    printf("Print da tabela para debug\n");
    for (int i=0; i<topo; i++){
        printf("%d: %s, %s, %d, %d\n", i, tabela_simbolos[i].nome, tabela_simbolos[i].tipo, tabela_simbolos[i].escopo, tabela_simbolos[i].natureza);
    }
}

