#include <stdio.h>
#include <string.h>
#include "analisadorLexico.h"
#include "tokens.h"
#include "analisadorSintatico.h"

void escreverTokens(FILE *arquivo) {
    anaLexReturn r = anaLex(arquivo);
    
    while (r.t != fimdearquivo) {
        if (r.t == -1) {
            r = anaLex(arquivo);
            continue;
        }
        if (r.t == -2){
            fclose(arquivo);
            return;
        }
        if (r.t == numero){
            printf("Número encontrado: %d \n", r.num);
        }
        if (r.t == identificador){
            printf("Identificador encontrado: '%s' \n", r.palavra);
        }
        else {
            printf("Token %d encontrado: palavra-chave '%s' \n", r.t, palavras[r.t]);
        }
        
        r = anaLex(arquivo);
    }
}

int main(){
    printf("inicio do programa main.c\n");
    fflush(stdout);
    FILE *arquivo = fopen("codigoResenhador.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 2;
    }
    
    compilaPrograma(arquivo);

    fclose(arquivo);
    return 0;
}
