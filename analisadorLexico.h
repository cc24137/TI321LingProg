#include "tipos.h"
#include <stdio.h>

#ifndef analisadorLexico_h
#define analisadorLexico_h

anaLexReturn anaLex(FILE* arquivo);
anaLexReturn obterToken(FILE *arquivo);
void devolverToken(anaLexReturn token_lido);

#endif