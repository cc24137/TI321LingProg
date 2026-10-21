#ifndef descricaoParametros
#define descricaoParametros

#include "tipos.h"

char verificaParametro(char* nomeFuncOuProc, char* tipo, int ordem, char ehReferencia, char ehProcedure, char ehFuncao);
char temParametro(char* nomeFuncOuProc);
descricaoParametro obtemDescricao(char* nomeFuncOuProc, int ordem);
void inserereNovoParametro(char* nomeFuncOuProc, char* tipo, char ehReferencia, char ehProcedure, char ehFuncao);
void printaAi();

#endif