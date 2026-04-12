#include <stdio.h>
#include <string.h>
#include "analisadorLexico.h"
#include "tokens.h"
//#include "tipos.h"

int main(){
    FILE *arquivo = fopen("codigoResenhador.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 2;
    }
    while(fgetc(arquivo) != EOF){
        fseek(arquivo, -1, SEEK_CUR);
        anaLexReturn r = anaLex(arquivo);
        if (r.t == -1) continue;
        if (r.t == -2){
            continue;
            fclose(arquivo);
            return 1;
        }
        if (r.t == numero){
            printf("Número encontrado: %d \n", r.num);
            continue;
        }
        if (r.t == identificador){
            printf("Identificador encontrado: '%s' \n", r.palavra);
            continue;
        }
        printf("Token %d encontrado: palavra-chave '%s' \n", r.t, palavras[r.t]);
    }

    fclose(arquivo);
    return 0;
}

