#ifndef descricaoParametros
#define descricaoParametros

#include "tipos.h"

char verificaParametro(char* nomeFuncOuProc, char* tipo, int ordem, tiposParametro tipoParametro);
char temParametro(char* nomeFuncOuProc);
char qtsParametrosTem(char* nomeFuncOuProc);
descricaoParametro obtemDescricao(const simbolo *subrotina, int ordem);
int inserereNovoParametro(char* nomeFuncOuProc, char* tipo, tiposParametro tipoParametro);
void printaAi();

#endif
