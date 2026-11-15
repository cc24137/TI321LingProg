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

descricaoParametro obtemDescricao(const simbolo *subrotina, int ordem){
    assinaturaRotina assinatura = obtemAssinatura(subrotina->assinatura);
    if (ordem < 0 || ordem >= subrotina->quantidadeParametros ||
        subrotina->inicioParametros < 0 ||
        subrotina->inicioParametros + ordem >= topoParams) {
        printf("Argumento fora da assinatura de '%s'.\n", subrotina->nome);
        exit(1);
    }
    return descParametros[assinatura.inicioParametros + ordem];
}

int inserereNovoParametro(char* nomeFuncOuProc, char* tipo, tiposParametro tipoParametro){
    //printf("Insere parâmetro chamado. \n");
    if (topoParams == MAX_PARAMETROS){
        printf("Estourou o limite de parâmetros.\n");
        exit(1);  
    } 

    if (tipoParametro < parametro_por_valor || tipoParametro > parametro_procedimento){
        printf("Categoria de parâmetro inválida.\n");
        exit(1);
    }

    descParametros[topoParams] = (descricaoParametro){0};
    descParametros[topoParams].assinatura = -1;

    strcpy(descParametros[topoParams].nomeFuncOuProc, nomeFuncOuProc);
    strcpy(descParametros[topoParams].tipo, tipo);
    descParametros[topoParams].tipoParametro = tipoParametro;

    return topoParams++;
} 

void printaAi(){
    printf("Descrição dos parâmetros: \n");
    for (int i=0; i<topoParams; i++){
        printf("nomeFunc/Proc: %s; tipo: %s; categoria: %d\n", descParametros[i].nomeFuncOuProc,
        descParametros[i].tipo, descParametros[i].tipoParametro);
    }
}


static assinaturaRotina assinaturas[MAX_PARAMETROS];
static int topoAssinaturas = 0;

static int raizAssinatura(int indice) {
    if (indice < 0 || indice >= topoAssinaturas) {
        printf("Índice de assinatura inválido.\n");
        exit(1);
    }
    int raiz = indice;
    while (assinaturas[raiz].vinculo != raiz) {
        raiz = assinaturas[raiz].vinculo;
    }
    while (indice != raiz) {
        int proximo = assinaturas[indice].vinculo;
        assinaturas[indice].vinculo = raiz;
        indice = proximo;
    }
    return raiz;
}

int novaAssinatura(void) {
    if (topoAssinaturas == MAX_PARAMETROS) {
        printf("Estourou o limite de assinaturas.\n");
        exit(1);
    }
    int indice = topoAssinaturas++;
    assinaturas[indice] = (assinaturaRotina){-1, -1, 0, indice};
    return indice;
}

assinaturaRotina obtemAssinatura(int indice) {
    return assinaturas[raizAssinatura(indice)];
}

static int mesmasDescricoes(const descricaoParametro *a, const descricaoParametro *b) {
    return a->tipoParametro == b->tipoParametro && strcmp(a->tipo, b->tipo) == 0;
}

int definirAssinatura(int indice, const char *nome,
                     const descricaoParametro *parametros, int quantidade) {
    assinaturaRotina *assinatura = &assinaturas[raizAssinatura(indice)];
    if (assinatura->quantidadeParametros >= 0) {
        if (assinatura->quantidadeParametros != quantidade) {
            printf("Quantidade de argumentos incompatível para '%s'.\n", nome);
            exit(1);
        }
        for (int i = 0; i < quantidade; i++) {
            if (!mesmasDescricoes(&descParametros[assinatura->inicioParametros + i],
                                  &parametros[i])) {
                printf("Tipos de argumentos incompatíveis para '%s'.\n", nome);
                exit(1);
            }
        }
        return assinatura->inicioParametros;
    }

    assinatura->inicioParametros = topoParams;
    assinatura->quantidadeParametros = quantidade;
    assinatura->quantidadePosicoesParametros = 0;
    for (int i = 0; i < quantidade; i++) {
        descricaoParametro parametro = parametros[i];
        int posicao = inserereNovoParametro((char *)nome, parametro.tipo, parametro.tipoParametro);
        descParametros[posicao].assinatura = parametro.assinatura;
        assinatura->quantidadePosicoesParametros +=
            parametro.tipoParametro == parametro_funcao ||
            parametro.tipoParametro == parametro_procedimento ? 3 : 1;
    }
    return assinatura->inicioParametros;
}

int assinaturaSomenteValores(int indice) {
    assinaturaRotina assinatura = obtemAssinatura(indice);
    for (int i = 0; i < assinatura.quantidadeParametros; i++) {
        if (descParametros[assinatura.inicioParametros + i].tipoParametro != parametro_por_valor) {
            return 0;
        }
    }
    return 1;
}

int vincularAssinaturas(int esperada, int recebida) {
    esperada = raizAssinatura(esperada);
    recebida = raizAssinatura(recebida);
    if (esperada == recebida) {
        return 1;
    }
    assinaturaRotina a = assinaturas[esperada];
    assinaturaRotina b = assinaturas[recebida];
    if (a.quantidadeParametros < 0) {
        assinaturas[esperada].vinculo = recebida;
        return 1;
    }
    if (b.quantidadeParametros < 0) {
        assinaturas[recebida].vinculo = esperada;
        return 1;
    }
    if (a.quantidadeParametros != b.quantidadeParametros) {
        return 0;
    }
    for (int i = 0; i < a.quantidadeParametros; i++) {
        if (!mesmasDescricoes(&descParametros[a.inicioParametros + i],
                             &descParametros[b.inicioParametros + i])) {
            return 0;
        }
    }
    return 1;
}
