#include <stdio.h>
#include <string.h>
#include "analisadorLexico.h"
#include "tokens.h"
#include <stdlib.h>

int compilaBloco(FILE *arquivo) {
    anaLexReturn token = anaLex(arquivo);
    
    if (token.t == rotulo) {
        while (token.t != pontoevirgula) {
            token = anaLex(arquivo);
            if (token.t != numero) {
                printf("Esperava-se um número!\n");
                exit(1);
            }
            
            token = anaLex(arquivo);
            if (token.t != virgula && token.t != pontoevirgula) {
                printf("Esperava-se uma vírgula ou um ponto e vírgula!\n");
                exit(1);
            }
        }
    }
    
    token = anaLex(arquivo);
    if (token.t == asterisco || token.t == tipo) {
        if (token.t == asterisco) {
            token = anaLex(arquivo);
            if (token.t != tipo) {
                printf("Esperava-se um tipo após o asterisco!\n");
                exit(1);
            }
        }
        
        while (token.t != identificador) {
            token = anaLex(arquivo);
            if (token.t != identificador) {
                printf("Esperava-se um identificador!\n");
                exit(1);
            }
            // continuar
        }
    }
    
}

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
    
    compilaBloco(arquivo);
    
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
