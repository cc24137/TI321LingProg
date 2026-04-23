#include <stdio.h>

#ifndef analisadorSintatico_h
#define analisadorSintatico_h

void compilaPrograma(FILE* arquivo);
int parametrosFormais(FILE *arquivo);
int fator(FILE *arquivo);
int termo(FILE *arquivo);
int expressaoSimples(FILE *arquivo);
int expressao(FILE *arquivo);
int comandoSemRotulo(FILE *arquivo);
int comando(FILE *arquivo);
int compilaBloco(FILE *arquivo);
#endif
