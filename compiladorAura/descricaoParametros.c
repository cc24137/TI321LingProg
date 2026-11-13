#include "tipos.h"
#include "tokens.h"
#include "analisadorLexico.h"
#include "descricaoParametros.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PARAMETROS 1000

descricaoParametro descParametros[MAX_PARAMETROS];
int topoParams = 0;

char verificaParametro(char* nomeFuncOuProc, char* tipo, int ordem, tiposParametro tipoParametro){

    for (int i=0; i<topoParams; i++){
        if (strcmp(descParametros[i].nomeFuncOuProc, nomeFuncOuProc) == 0){
            i += ordem;
        }
        if (strcmp(descParametros[i].nomeFuncOuProc, nomeFuncOuProc) == 0 &&
            strcmp(descParametros[i].tipo, tipo) == 0 &&
            descParametros[i].tipoParametro == tipoParametro){
                return 1;
        }
        else{
            return 0;
        }
    }
    return 0;
}

char qtsParametrosTem(char* nomeFuncOuProc){

    char qts = 0;

    for (int i=0; i<topoParams; i++){
        if (strcmp(descParametros[i].nomeFuncOuProc, nomeFuncOuProc) == 0){
            qts++;
        }
    }
    return qts;
}

char temParametro(char* nomeFuncOuProc){
    for (int i=0; i<topoParams; i++){
        if (strcmp(descParametros[i].nomeFuncOuProc, nomeFuncOuProc) == 0){
            return 1;
        }
    }
    return 0;
}

descricaoParametro obtemDescricao(char* nomeFuncOuProc, int ordem){
    //printf("Descrição do parâmetro %d da func/proc %s \n", ordem, nomeFuncOuProc);
    for (int i=0; i<topoParams; i++){

        if (strcmp(descParametros[i].nomeFuncOuProc, nomeFuncOuProc) == 0){
            //printf("%d\n", i);
            i += ordem;
            if (strcmp(descParametros[i].nomeFuncOuProc, nomeFuncOuProc) == 0){
                //printf("%d\n", i);
                return descParametros[i];
            }
            else{
                printf("Esse função ou procedimento %s não tem tantos parâmetros;\n", nomeFuncOuProc);
                exit(1);
            }
        }
    }
    printf("Esse nome de função/proc não existe\n");
    exit(1);
}

void inserereNovoParametro(char* nomeFuncOuProc, char* tipo, tiposParametro tipoParametro){
    //printf("Insere parâmetro chamado. \n");
    if (topoParams == MAX_PARAMETROS){
        printf("Estourou o limite de parâmetros.\n");
        exit(1);  
    } 

    if (tipoParametro < parametro_por_valor || tipoParametro > parametro_procedimento){
        printf("Categoria de parâmetro inválida.\n");
        exit(1);
    }

    strcpy(descParametros[topoParams].nomeFuncOuProc, nomeFuncOuProc);
    strcpy(descParametros[topoParams].tipo, tipo);
    descParametros[topoParams].tipoParametro = tipoParametro;

    topoParams++;
} 

void printaAi(){
    printf("Descrição dos parâmetros: \n");
    for (int i=0; i<topoParams; i++){
        printf("nomeFunc/Proc: %s; tipo: %s; categoria: %d\n", descParametros[i].nomeFuncOuProc,
        descParametros[i].tipo, descParametros[i].tipoParametro);
    }
}
