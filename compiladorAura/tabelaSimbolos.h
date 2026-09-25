#ifndef tabelaSimbolos_h
#define tabelaSimbolos_h

#include "tipos.h"

char temNaTabelaSimbolos(char *nome);
void adicionaNaTabelaSimbolos(char *nome, char* tipo, char escopo, naturezas natureza);
void apagaEscopoTabelaSimbolos(char contexto);
void printaTabela();
naturezas obterNaturezaNaTabela(char *nome);
char* obterTipoNaTabela(char *nome);

#endif // tabelaSimbolos_h
