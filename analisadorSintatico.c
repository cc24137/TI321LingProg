#include <stdio.h>
#include <string.h>
#include "analisadorLexico.h"
#include "analisadorSintatico.h"
#include "tipos.h"
#include "tokens.h"
#include <stdlib.h>

anaLexReturn token_buffer;
int tem_token_no_buffer = 0;

anaLexReturn obterToken(FILE *arquivo) {
    if (tem_token_no_buffer) {
        tem_token_no_buffer = 0;
        return token_buffer;
    }
    return anaLex(arquivo);
}

void devolverToken(anaLexReturn token_lido) {
    token_buffer = token_lido;
    tem_token_no_buffer = 1;
}

int parametrosFormais(FILE *arquivo) {
    anaLexReturn token = obterToken(arquivo);
    if (token.t == abreparenteses) {
        do {
            token = obterToken(arquivo);
            int ehProcedure = 0;
            
            if (token.t == funcao) {
                token = obterToken(arquivo);
            } else if (token.t == procedimento) {
                ehProcedure = 1;
                token = obterToken(arquivo);
            } else if (token.t == variavel) {
                token = obterToken(arquivo);
            }

            if (token.t != identificador) {
                printf("Esperava-se um identificador nos parâmetros formais!\n");
                exit(1);
            }
            
            token = obterToken(arquivo); 
            while (token.t == virgula) {
                token = obterToken(arquivo); 
                if (token.t != identificador) {
                    printf("Esperava-se um identificador após a vírgula!\n");
                    exit(1);
                }
                token = obterToken(arquivo); 
            }
            
            if (!ehProcedure) {
                if (token.t != doispontos) {
                    printf("Esperava-se dois pontos na declaração dos parâmetros!\n");
                    exit(1);
                }
                
                token = obterToken(arquivo);
                if (token.t != identificador) {
                    printf("Esperava-se um identificador de tipo após os dois pontos!\n");
                    exit(1);
                }
                
                token = obterToken(arquivo);
            }
            
        } while (token.t == pontoevirgula);
        
        if (token.t != fechaparenteses) {
            printf("Esperava-se ')' para fechar os parâmetros formais!\n");
            exit(1);
        }
    } 
    else {
        devolverToken(token);
    }
    
    return 1;
}

int fator(FILE *arquivo) {
    anaLexReturn token = obterToken(arquivo);

    if (token.t == identificador) {
        token = obterToken(arquivo);
        
        while (token.t == abrecolchetes) {
            do {
                expressao(arquivo);
                token = obterToken(arquivo);
            } while (token.t == virgula);

            if (token.t != fechacolchetes) {
                printf("Esperava-se fechacolchetes!\n");
                exit(1);
            }
            token = obterToken(arquivo);
        }

        while (token.t == abreparenteses) {
            do {
                expressao(arquivo);
                token = obterToken(arquivo);
            } while (token.t == virgula);

            if (token.t != fechaparenteses) {
                printf("Esperava-se fechaparenteses!\n");
                exit(1);
            }
            token = obterToken(arquivo);
        }
        
        devolverToken(token);
    } 
    else if (token.t == numero) {
    } 
    else if (token.t == abreparenteses) {
        expressao(arquivo);
        token = obterToken(arquivo);
        if (token.t != fechaparenteses) {
            printf("Esperava-se fechaparenteses!\n");
            exit(1);
        }
    } 
    else if (token.t == nao) {
        fator(arquivo);
    } 
    else {
        printf("Esperava-se um fator valido!\n");
        exit(1);
    }

    return 1;
}

int termo(FILE *arquivo) {
    fator(arquivo);
    
    anaLexReturn token = obterToken(arquivo);
    while (token.t == asterisco || token.t == dividir || token.t == e) {
        fator(arquivo);
        token = obterToken(arquivo);
    }
     devolverToken(token);
    
    return 1;
}

int expressaoSimples(FILE *arquivo) {
    anaLexReturn token = obterToken(arquivo);
    
    if (token.t == mais || token.t == menos) {
        token = obterToken(arquivo);
    }
    else {
        devolverToken(token);
    }
    
    termo(arquivo);
    
    token = obterToken(arquivo);
    while (token.t == mais || token.t == menos || token.t == ou) {
        termo(arquivo);
        token = obterToken(arquivo);
    }
     devolverToken(token);
     
    return 1;
}

int expressao(FILE *arquivo) {
    expressaoSimples(arquivo);
    
    anaLexReturn token = obterToken(arquivo);
    if (token.t == igual || token.t == diferente || token.t == menor || token.t == maior || token.t == menorouigual || token.t == maiorouigual) {
        expressaoSimples(arquivo);
    } else {
        devolverToken(token);
    }
    
    return 1;
}

int comandoSemRotulo(FILE *arquivo) {
    anaLexReturn token = obterToken(arquivo);
    if (token.t == identificador) {
        
        token = obterToken(arquivo);
        while (token.t == abrecolchetes) {
            token = obterToken(arquivo);
            while (token.t == virgula) {
                expressao(arquivo);
                token = obterToken(arquivo);
            }
            
            if (token.t != fechacolchetes) {
                printf("Esperava-se fechacolchetes!\n");
                exit(1);
            }
            token = obterToken(arquivo);
        }
            
        if (token.t == abreparenteses) {
            token = obterToken(arquivo);
            while (token.t == virgula) {
                expressao(arquivo);
                token = obterToken(arquivo);
            }
            
            if (token.t != fechaparenteses) {
                printf("Esperava-se fechaparenteses!\n");
                exit(1);
            }
            token = obterToken(arquivo);
        }
        
        devolverToken(token);
        expressao(arquivo);

        token = obterToken(arquivo);
        if (token.t == atribuicao) {
            expressao(arquivo);
        }
        else {
            devolverToken(token);
        }
    }
    
    else if (token.t == irpara) {
        token = obterToken(arquivo);
        if (token.t != numero) {
            printf("Esperava-se um número!\n");
            exit(1);
        }
    }
    
    else if (token.t == inicio) {
        comando(arquivo);
        
        token = obterToken(arquivo);
        while (token.t != fim) {
            if (token.t != pontoevirgula) {
                printf("Esperava-se ponto e virgula!\n");
                exit(1);
            }
            comando(arquivo);
            token = obterToken(arquivo);
        }
    }
    
    else if (token.t == se) {
        expressao(arquivo);
        
        token = obterToken(arquivo);
        if (token.t != entao) {
            printf("Esperava-se então (then)!\n");
            exit(1);
        }
        
        comandoSemRotulo(arquivo);
        
        token = obterToken(arquivo);
        if (token.t == senao) {
            comandoSemRotulo(arquivo);
        }
        else {
            devolverToken(token);
        }
    }
    
    else if (token.t == enquanto) {
        expressao(arquivo);
        
        token = obterToken(arquivo);
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
    return 1;
}

int comando(FILE *arquivo) {
    anaLexReturn token = obterToken(arquivo);
    if (token.t == numero) {
        token = obterToken(arquivo);
        if (token.t != doispontos) {
            printf("Esperava-se dois pontos!\n");
            exit(1);
        }
    }
    else {
        devolverToken(token);
    }
    
    comandoSemRotulo(arquivo);
    
    return 1;
}

int compilaBloco(FILE *arquivo) {
    anaLexReturn token = obterToken(arquivo);
    
    if (token.t == rotulo) {
        while (token.t != pontoevirgula) {
            token = obterToken(arquivo);
            if (token.t != numero) {
                printf("Esperava-se um número!\n");
                exit(1);
            }

            token = obterToken(arquivo);
            if (token.t != virgula && token.t != pontoevirgula) {
                printf("Esperava-se uma vírgula ou um ponto e vírgula!\n");
                exit(1);
            }
        }
    }
    else {
        devolverToken(token);
    }

    // pelo que entendi esse bloco seguinte é um aliasing. Exemplo:
    // type Identificador = string;
    // REVISAR ESSE PRA VER SE É ISSO mesmo
    token = obterToken(arquivo);
    while (token.t == tipo) {
        token = obterToken(arquivo);
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

            token = obterToken(arquivo);
            if (token.t != atribuicao) {
                printf("Esperava-se um sinal de atribuição!\n");
                exit(1);
            }

            // REVISAR OS TIPOS ACEITOS
            token = obterToken(arquivo); // sera que pode ser algo tipo struct também nos tipos?
            if (token.t != inteiro && token.t != longo && token.t != curto && token.t != flutuante && token.t != duplo && token.t != caractere) {
                printf("Esperava-se um tipo!\n");
                exit(1);
            }

            token = obterToken(arquivo);
            if (token.t != virgula && token.t != pontoevirgula) {
                printf("Esperava-se uma vírgula ou um ponto e vírgula!\n");
                exit(1);
            }

            // le token em avanço para verificar fim do while
            token = obterToken(arquivo);
        }
        // leu um token a mais para verificar o fim do while, então volta um passo
         devolverToken(token);
         token = obterToken(arquivo);
    }
    devolverToken(token);

    token = obterToken(arquivo);
    if (token.t == variavel) {
        token = obterToken(arquivo);
        if (token.t != identificador) {
            printf("Esperava-se um identificador!\n");
            exit(1);
        }

        while (token.t == identificador) {

            token = obterToken(arquivo);
            while (token.t == virgula) {
                token = obterToken(arquivo);
                if (token.t != identificador) {
                    printf("Esperava-se um identificador!\n");
                    exit(1);
                }

                token = obterToken(arquivo);
                // preparou para proximo identificador
            }
            // devolve o que leu a mais para verificar o while
            devolverToken(token);

            token = obterToken(arquivo);
            if (token.t != doispontos) {
                printf("Esperava-se um dois pontos!\n");
                exit(1);
            }

            token = obterToken(arquivo);
            if (token.t != inteiro && token.t != longo && token.t != curto && token.t != flutuante && token.t != duplo && token.t != caractere) {
                printf("Esperava-se um tipo!\n");
                exit(1);
            }

            token = obterToken(arquivo);
            if (token.t != pontoevirgula) {
                printf("Esperava-se um ponto e vírgula!\n");
                exit(1);
            }

            token = obterToken(arquivo);
        }
        // leu um token a mais para verificar o fim do while, então volta um passo
         devolverToken(token);
    }
    else {
        devolverToken(token);
    }

    token = obterToken(arquivo);
    while (token.t == procedimento || token.t == funcao) {

        if (token.t == procedimento) {
            token = obterToken(arquivo);
            if (token.t != identificador) {
                printf("Esperava-se um identificador!\n");
                exit(1);
            }

            parametrosFormais(arquivo);
        }

        if (token.t == funcao) {
            token = obterToken(arquivo);
            if (token.t != identificador) {
                printf("Esperava-se um identificador!\n");
                exit(1);
            }

            parametrosFormais(arquivo);

            token = obterToken(arquivo);
            if (token.t != doispontos) {
                printf("Esperava-se dois pontos!\n");
                exit(1);
            }

            token = obterToken(arquivo);
            if (token.t != identificador) {
                printf("Esperava-se um identificador!\n");
                exit(1);
            }
        }

        token = obterToken(arquivo);
        if (token.t != pontoevirgula) {
            printf("Esperava-se um ponto e vírgula!\n");
            exit(1);
        }

        compilaBloco(arquivo);

        token = obterToken(arquivo);
        if (token.t != pontoevirgula) {
            printf("Esperava-se um ponto e vírgula!\n");
            exit(1);
        }

        // le para possível processo ou funcao seguinte
        token = obterToken(arquivo);
    }
    
    // ja leu o token anteriormente
    if (token.t == inicio) {
        comando(arquivo);
        
        token = obterToken(arquivo);
        while (token.t != fim) {
            devolverToken(token);
            comando(arquivo);
            token = obterToken(arquivo);
        }
    }
    
    return 1;
}

void compilaPrograma (FILE *arquivo)
{
    anaLexReturn token = obterToken(arquivo);

    if (token.t!=programa)
    {
        printf("Esperava-se a palavra PROGRAM!\n");
        exit(1);
    }

    token = obterToken(arquivo);
    if (token.t!=identificador)
    {
        printf("Esperava-se um identificador!\n");
        exit(1);
    }

    token = obterToken(arquivo);
    if (token.t!=abreparenteses)
    {
        printf("Esperava-se um abre parenteses! Token encontrado = %d\n", token.t);
        exit(1);
    }

    while (token.t!=fechaparenteses)
    {
        token = obterToken(arquivo);
        if (token.t!=identificador)
        {
            printf("Esperava-se um identificador!\n");
            exit(1);
        }
        token = obterToken(arquivo);
        if (token.t!=virgula && token.t!=fechaparenteses)
        {
            printf("Esperava-se um virgula ou um fecha parenteses!\n");
            exit(1);
        }
    }

    token = obterToken(arquivo);
    if (token.t!=pontoevirgula)
    {
        printf("Esperava-se um ponto e virgula!\n");
        exit(1);
    }

    compilaBloco(arquivo);

    token = obterToken(arquivo);
    if (token.t!=ponto)
    {
        printf("Esperava-se um ponto final!\n");
        exit(1);
    }

    token = obterToken(arquivo);
    if (token.t!=fimdearquivo)
    {
        printf("Esperava-se fim de arquivo!\n");
        exit(1);
    }

    printf("Programa sintaticamente correto!\n");
}
