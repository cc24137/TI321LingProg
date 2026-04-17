#include <stdio.h>
#include <string.h>
#include "analisadorLexico.h"
#include "tipos.h"
#include "tokens.h"


char ehSimboloInicial(char c){
    return ( c==':' || c=='>' || c=='<');
}

char ehSimboloUnico(char c){
    if (c == '(' || c == ')' || c=='[' || c==']' || c==',' || c==';' || c=='.' || c=='=')
        return 1;
    for (int i=50; i<54; i++){
        if (c== *palavras[i])
            return 1;
    }
    return 0;
}

char ehLetra(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

char ehNumero(char c) {
    return (c >= '0' && c <= '9');
}

char ehCaracterDePalavra(char c) {
    return ehLetra(c) || ehNumero(c) || (c == '_') || (c == '-');
}

char ehSeparador(char c) {
    return (c == ' ' || c == '\n' || c == '\t');
}


token qualToken(char *palavra) {
    if (palavra[0] >= '0' && palavra[0] <= '9') 
        return numero;
    for (int i = 0; i < 64; i++) {
        if (strcmp(palavra, palavras[i]) == 0) {
            return tokens[i];
        }
    }
    return identificador;
}

int potencia(int b, int e){
    int r = 1;
    for (int i=0; i<e; i++){
        r *= b;
    }
    return r;
}

int qualNum(char *numero) {
    int num = 0;
    int l = strlen(numero);
    for (int i = l; i>0; i--){
        num += (numero[i-1] - '0') * potencia(10, l - i);         
    }
    return num;
}
    

anaLexReturn anaLex(FILE* arquivo) {

    anaLexReturn ret;

    char string[100];
    char c = fgetc(arquivo);
    char i = 0;
    
    if (c == EOF) {
        ret.t = fimdearquivo;
        return ret;
    }


    if (ehSeparador(c)) {
        c = fgetc(arquivo);
    }

    // letra inicia palavra reservada e identificador
    if (ehLetra(c) || c == '_') {
        string[i++] = c;
        while ((c = fgetc(arquivo)) != EOF && ehCaracterDePalavra(c)) {
            string[i++] = c;
        }
        fseek(arquivo, -1, SEEK_CUR);
        string[i] = '\0';

        token t = qualToken(string);
        //printf("Token da palavra '%s': %d\n", string, t);
        if (t == identificador){
            //printf("identificador encontrado: %s\n", string);
            strcpy(ret.palavra, string);
            //printf("O que foi passado para return: %s\n", ret.palavra);
        }
        
        ret.t = t;
        return ret;
    }
    
    // número
    if (ehNumero(c)) {
        string[i++] = c;
        while ((c = fgetc(arquivo)) != EOF && ehNumero(c)) {
            string[i++] = c;
        }
        fseek(arquivo, -1, SEEK_CUR);
        string[i] = '\0';
        
        //printf("Token do número %d : %d\n", qualNum(string), numero);
        ret.t = numero;
        ret.num = qualNum(string);
        return ret;
    }
    
    if (ehSimboloUnico(c)){
        char s[2] = {c, '\0'};
        //printf("Token de símbolo: %d\n", qualToken(s));
        ret.t = qualToken(s);
        return ret;
    }
    if (ehSimboloInicial(c)){
        char prox = fgetc(arquivo);
        char s[2] = {c, prox};
        token t = qualToken(s);
        if (t != identificador){
            //printf("Token de símbolo: %d\n", t);
            ret.t = t;
        }
        else{
            s[1] = '\0';
            token t = qualToken(s);
            ungetc(prox, arquivo);
            //printf("Token de símbolo: %d\n", t);
            ret.t = t;
        }
        return ret;
    }

    ret.t = -2;
    //printf("nao reconheceu nada");
    return ret;
}
