#ifndef descricaoParametros
#define descricaoParametros

#include "tipos.h"

char verificaParametro(char* nomeFuncOuProc, char* tipo, int ordem, tiposParametro tipoParametro);
char temParametro(char* nomeFuncOuProc);
char qtsParametrosTem(char* nomeFuncOuProc);
descricaoParametro obtemDescricao(const simbolo *subrotina, int ordem);
int inserereNovoParametro(char* nomeFuncOuProc, char* tipo, tiposParametro tipoParametro);
void printaAi();
int novaAssinatura(void);
assinaturaRotina obtemAssinatura(int indice);
int definirAssinatura(int indice, const char *nome, const descricaoParametro *parametros, int quantidade);
int assinaturaSomenteValores(int indice);
int vincularAssinaturas(int esperada, int recebida);


#endif
