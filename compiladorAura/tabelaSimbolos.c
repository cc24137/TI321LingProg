#include "tipos.h"
#include "tokens.h"
#include "analisadorLexico.h"
#include "tabelaSimbolos.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIMBOLOS 1000
simbolo tabela_simbolos[MAX_SIMBOLOS];
int topo = 0;

simbolo *buscarNaTabelaSimbolos(const char *nome) {
    for (int i = topo - 1; i >= 0; i--) {
        if (strcmp(tabela_simbolos[i].nome, nome) == 0) {
            return &tabela_simbolos[i];
        }
    }
    return NULL;
}

char temNaTabelaSimbolos(char *nome) {
    return buscarNaTabelaSimbolos(nome) != NULL;
}

simbolo *adicionaNaTabelaSimbolos(char *nome, char* tipo, char escopo, naturezas natureza) {
    if (topo >= MAX_SIMBOLOS) {
        printf("Tabela de simbolos cheia!\n");
        exit(1);
    }

    // insere direto no topo
    simbolo *novo = &tabela_simbolos[topo];
    *novo = (simbolo){0};
    strcpy(novo->nome, nome);
    strcpy(novo->tipo, tipo);
    novo->escopo = escopo;
    novo->natureza = natureza;
    novo->endereco = 0;
    novo->tipoParametro = parametro_por_valor;
    novo->inicioParametros = -1;
    novo->quantidadeParametros = 0;
    topo++;
    return novo;
}

void prepararRotulosDoBloco(char escopo, int quantidadeLocais) {
    for (int i = topo - 1; i >= 0; i--) {
        if (tabela_simbolos[i].escopo == escopo &&
            tabela_simbolos[i].natureza == natureza_rotulo) {
            tabela_simbolos[i].quantidadeLocais = quantidadeLocais;
        }
    }
}

void atribuirEnderecosParametros(char escopo) {
    // A lista acaba de ser inserida. O último parâmetro fica antes dos três
    // campos de controle da chamada, no deslocamento -4.
    int deslocamento = -4;
    for (int i = topo - 1; i >= 0 && tabela_simbolos[i].escopo == escopo; i--) {
        if (tabela_simbolos[i].natureza == natureza_parametro) {
            tabela_simbolos[i].endereco = deslocamento--;
        }
    }
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

naturezas obterNaturezaNaTabela(char *nome) {
    const simbolo *encontrado = buscarNaTabelaSimbolos(nome);
    if (encontrado != NULL) {
        return encontrado->natureza;
    }
    return -1;
}

char* obterTipoNaTabela(char *nome) {
    simbolo *encontrado = buscarNaTabelaSimbolos(nome);
    return encontrado != NULL ? encontrado->tipo : "";
}
