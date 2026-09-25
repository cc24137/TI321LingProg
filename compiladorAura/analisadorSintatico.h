#include <stdio.h>

#ifndef analisadorSintatico_h
#define analisadorSintatico_h

void compilaPrograma(FILE* arquivo);
int compilaParametrosFormais(FILE *arquivo);
int compilaFator(FILE *arquivo);
int compilaTermo(FILE *arquivo);
int compilaExpressaoSimples(FILE *arquivo);
int compilaExpressao(FILE *arquivo);
int compilacomandoSemRotulo(FILE *arquivo);
int compilaComando(FILE *arquivo);
int compilaBloco(FILE *arquivo);
#endif
