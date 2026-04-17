#include <stdio.h>
#include <string.h>
#include "analisadorLexico.h"
#include "tokens.h"
#include <stdlib.h>

int compilaPrograma (FILE *arquivo)
{
    anaLexReturn token = anaLex(arquivo);
    
    if (token.t!=programa)
    {
        printf("Esperava-se a palavra PROGRAM!\n");
        exit(1);
    }
    
    token = anaLex(arquivo);
    if (token.t!=identificador)
    {
        printf("Esperava-se um identificador!\n");
        exit(1);
    }
    
    token = anaLex(arquivo);
    if (token.t!=abreparenteses)
    {
        printf("Esperava-se um abre parenteses! Token encontrado = %d\n", token.t);
        exit(1);
    }
    
    while (token.t!=fechaparenteses)
    {
        token = anaLex(arquivo);
        if (token.t!=identificador)
        {
            printf("Esperava-se um identificador!\n");
            exit(1);
        }
        token = anaLex(arquivo);
        if (token.t!=virgula && token.t!=fechaparenteses)
        {
            printf("Esperava-se um virgula ou um fecha parenteses!\n");
            exit(1);
        }
    }
    
    token = anaLex(arquivo);
    if (token.t!=pontoevirgula)
    {
        printf("Esperava-se um ponto e virgula!\n");
        exit(1);
    }
    
    //compila_bloco();
    
    token = anaLex(arquivo);
    if (token.t!=ponto)
    {
        printf("Esperava-se um ponto final!\n");
        exit(1);
    }
    
    token = anaLex(arquivo);
    if (token.t!=fimdearquivo)
    {
        printf("Esperava-se fim de arquivo!\n");
        exit(1);
    }
    
    printf("Programa sintaticamente correto!\n");
}
