#include <stdio.h>
#include <string.h>
#include "analisadorLexico.h"
#include "tipos.h"
#include "tokens.h"

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
    return (c == ' ' || c == '\n' || c == '\r' || c == '\t');
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
    
    // pula todos os separadores
    while (c != EOF && ehSeparador(c)) {
        c = fgetc(arquivo);
    }

    // verifica EOF
    if (c == EOF) {
        ret.t = fimdearquivo;
        return ret;
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
        if (t == identificador){
            strcpy(ret.palavra, string);
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
        
        ret.t = numero;
        ret.num = qualNum(string);
        return ret;
    }
    
    if (ehSimboloUnico(c)){
        char s[2] = {c, '\0'};
        ret.t = qualToken(s);
        return ret;
    }
    
    if (ehSimboloInicial(c)){
        char prox = fgetc(arquivo);
        // precisa de 3 caracteres para incluir o \0
        char s[3] = {c, prox, '\0'}; 
        
        token t = qualToken(s);
        
        if (t != identificador){
            ret.t = t;
        }
        else {
            // Se falhou o composto
            // corta a string e devolve o prox pro buffer
            s[1] = '\0'; 
            token t = qualToken(s);
            ungetc(prox, arquivo);
            ret.t = t;
        }
        return ret;
    }

    ret.t = -2;
    return ret;
}
