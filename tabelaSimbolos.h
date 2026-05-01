#ifndef tabelaSimbolos_h
#define tabelaSimbolos_h

#include "tipos.h"

char temNaTabelaSimbolos(char *nome);
void adicionaNaTabelaSimbolos(char *nome, token tipo, char contexto);
void apagaContextoTabelaSimbolos(char contexto);

#endif // tabelaSimbolos_h
