#include <stdio.h>
#include <string.h>
#include "analisadorLexico.h"
#include "tipos.h"
#include "tokens.h"
#include <stdlib.h>

int parametrosFormais(FILE *arquivo) {
    // opcional sempre
    anaLexReturn token = anaLex(arquivo);
    if (token.t == abreparenteses) {
        // faz o resto
    }
    
    return 0;
}

int fator(FILE *arquivo) {
    return 0;
}

int termo(FILE *arquivo) {
    return 0;
}

int expressaoSimples(FILE *arquivo) {
    return 0;
}

int expressao(FILE *arquivo) {
    return 0;
}

int comandoSemRotulo(File *arquivo) {
    anaLexReturn token = anaLex(arquivo);
    if (token.t == identificador) {
        // dois caminhos
        
        token = anaLex(arquivo);
        if (token.t == asterisco || token.t == abrecolchetes) {
            if (token.t == asterisco) {
                token = anaLex(arquivo);
                if (token.t != abrecolchetes) {
                    printf("Esperava-se uma abrecolchetes!\n");
                    exit(1);
                }
            }   
            
            token = anaLex(arquivo);
            while (token.t == virgula) {
                expressao(arquivo);
                token = anaLex(arquivo);
            }
            
            if (token.t != fechacolchetes) {
                printf("Esperava-se fechacolchetes!\n");
                exit(1);
            }
        }
            
        else if (token.t == abreparenteses) {
            token = anaLex(arquivo);
            while (token.t == virgula) {
                expressao(arquivo);
                token = anaLex(arquivo);
            }
            
            if (token.t != fechaparenteses) {
                printf("Esperava-se fechaparenteses!\n");
                exit(1);
            }
        }
        
        expressao(arquivo);

        
        // convergem em
        token = anaLex(arquivo);
        if (token.t == atribuicao) {
            expressao(arquivo);
        }
        else {
            fseek(arquivo, -1, SEEK_CUR);
        }
    }
    
    else if (token.t == irpara) {
        token = anaLex(arquivo);
        if (token.t != numero) {
            printf("Esperava-se um número!\n");
            exit(1);
        }
    }
    
    else if (token.t == inicio) {
        comando(arquivo);
        
        token = anaLex(arquivo);
        while (token != fim) {
            if (token != pontoevirgula) {
                printf("Esperava-se ponto e virgula!\n");
                exit(1);
            }
            comando(arquivo);
        }
    }
    
    else if (token.t == se) {
        expressao(arquivo);
        
        token = anaLex(arquivo);
        if (token.t != entao) {
            printf("Esperava-se então (then)!\n");
            exit(1);
        }
        
        comandoSemRotulo(arquivo);
        
        token = anaLex(arquivo);
        if (token.t == senao) {
            comandoSemRotulo(arquivo);
        }
        else {
            fseek(arquivo, -1, SEEK_CUR);
        }
    }
    
    else if (token.t == enquanto) {
        expressao(arquivo);
        
        token = anaLex(arquivo);
        if (token.t != faz) {
            printf("Esperava-se faz (do)!\n");
            exit(1);
        }
        
        comandoSemRotulo(arquivo);
    }
    
    else {
        printf("Esperava-se um comando sem rótulo válido!\n");
        exit(1);
    }
}

int comando(FILE *arquivo) {
    anaLexReturn token = anaLex(arquivo);
    if (token.t == numero) {
        anaLexReturn token = anaLex(arquivo);
        if (token.t != doispontos) {
            printf("Esperava-se dois pontos!\n");
            exit(1);
        }
    }
    else {
        fseek(arquivo, -1, SEEK_CUR);
    }
    
    comandoSemRotulo(arquivo);
    
    return 0;
}

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
    else {
        fseek(arquivo, -1, SEEK_CUR);
    }

    // pelo que entendi esse bloco seguinte é um aliasing. Exemplo:
    // type Identificador = string;
    // REVISAR ESSE PRA VER SE É ISSO MESMO
    token = anaLex(arquivo);
    if (token.t == asterisco || token.t == tipo) {
        if (token.t == asterisco) {
            token = anaLex(arquivo);
            if (token.t != tipo) {
                printf("Esperava-se um tipo após o asterisco!\n");
                exit(1);
            }
        }
        token = anaLex(arquivo);
        if (token.t != identificador) {
            printf("Esperava-se um identificador!\n");
            exit(1);
        }

        while (token.t == identificador) {
            // ja leu o proximo token
            if (token.t != identificador) {
                printf("Esperava-se um identificador!\n");
                exit(1);
            }

            token = anaLex(arquivo);
            if (token.t != atribuicao) {
                printf("Esperava-se um sinal de atribuição!\n");
                exit(1);
            }

            // REVISAR OS TIPOS ACEITOS
            token = anaLex(arquivo); // sera que pode ser algo tipo struct também nos tipos?
            if (token.t != inteiro && token.t != longo && token.t != curto && token.t != flutuante && token.t != duplo && token.t != caractere) {
                printf("Esperava-se um tipo!\n");
                exit(1);
            }

            token = anaLex(arquivo);
            if (token.t != virgula && token.t != pontoevirgula) {
                printf("Esperava-se uma vírgula ou um ponto e vírgula!\n");
                exit(1);
            }

            // le token em avanço para verificar fim do while
            token = anaLex(arquivo);
        }
        // leu um token a mais para verificar o fim do while, então volta um passo
         fseek(arquivo, -1, SEEK_CUR);
    }
    else {
        fseek(arquivo, -1, SEEK_CUR);
    }

    token = anaLex(arquivo);
    if (token.t == variavel) {
        token = anaLex(arquivo);
        if (token.t != identificador) {
            printf("Esperava-se um identificador!\n");
            exit(1);
        }

        while (token.t == identificador) {

            token = anaLex(arquivo);
            while (token.t == virgula) {
                token = anaLex(arquivo);
                if (token.t != identificador) {
                    printf("Esperava-se um identificador!\n");
                    exit(1);
                }

                token = anaLex(arquivo);
                // preparou para proximo identificador
            }
            // devolve o que leu a mais para verificar o while
            fseek(arquivo, -1, SEEK_CUR);

            token = anaLex(arquivo);
            if (token.t != doispontos) {
                printf("Esperava-se um dois pontos!\n");
                exit(1);
            }

            token = anaLex(arquivo);
            if (token.t != inteiro && token.t != longo && token.t != curto && token.t != flutuante && token.t != duplo && token.t != caractere) {
                printf("Esperava-se um tipo!\n");
                exit(1);
            }

            token = anaLex(arquivo);
            if (token.t != pontoevirgula) {
                printf("Esperava-se um ponto e vírgula!\n");
                exit(1);
            }

            token = anaLex(arquivo);
        }
        // leu um token a mais para verificar o fim do while, então volta um passo
         fseek(arquivo, -1, SEEK_CUR);
    }
    else {
        fseek(arquivo, -1, SEEK_CUR);
    }

    token = anaLex(arquivo);
    while (token.t == procedimento || token.t == funcao) {

        if (token.t == procedimento) {
            token = anaLex(arquivo);
            if (token.t != identificador) {
                printf("Esperava-se um identificador!\n");
                exit(1);
            }

            parametrosFormais(arquivo);
        }

        if (token.t == funcao) {
            token = anaLex(arquivo);
            if (token.t != identificador) {
                printf("Esperava-se um identificador!\n");
                exit(1);
            }

            parametrosFormais(arquivo);

            token = anaLex(arquivo);
            if (token.t != doispontos) {
                printf("Esperava-se dois pontos!\n");
                exit(1);
            }

            token = anaLex(arquivo);
            if (token.t != identificador) {
                printf("Esperava-se um identificador!\n");
                exit(1);
            }
        }

        token = anaLex(arquivo);
        if (token.t != pontoevirgula) {
            printf("Esperava-se um ponto e vírgula!\n");
            exit(1);
        }

        compilaBloco();

        token = anaLex(arquivo);
        if (token.t != pontoevirgula) {
            printf("Esperava-se um ponto e vírgula!\n");
            exit(1);
        }

        // le para possível processo ou funcao seguinte
        token = anaLex(arquivo);
    }
    
    // ja leu o token anteriormente
    if (token.t == inicio) {
        comando(arquivo);
        
        token = anaLex(arquivo);
        while (token.t != fim) {
            fseek(arquivo, -1, SEEK_CUR);
            comando(arquivo);
            token = anaLex(arquivo);
        }
    }
    
    return 0;
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
