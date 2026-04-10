#include <stdio.h>
#include <string.h>
typedef enum {
    se, // if
    senao, // else
    enquanto, // while
    quatro, // for - resenha
    faz, // do
    constante, // const
    quebrar, // break
    vazio, // void
    wii, // switch - resenha
    caso, // case
    retornar, // return
    caractere, // char
    inteiro, // int
    longo, // long
    curto, // short
    flutuante, // float
    duplo, // double
    continuar, // continue
    padrao, // default
    enumeracao, // enum
    estrutura, // struct
    uniao, // union
    externo, // extern
    irpara, // goto
    nalinha, // inline
    registrar, // register
    assinado, // signed
    naoassinado, // unsigned
    tamanhode, // sizeof
    estatico, // static
    deftipo, // typedef
    tipode, // typeof
    volatil, // volatile

    programa, // program
    rotulo, // label
    tipo, // type
    variavel, // var
    procedimento, // procedure
    funcao, // function
    inicio, // begin
    fim, // end
    atribuicao, // :=
    entao, // then

    dividir, // div
    diferente, // <>
    menorouigual, // <=
    maiorouigual, // >=
    e, // and
    ou, // or
    nao, // not

    /* Símbolos de 1 caractere movidos para o final */
    mais, // +
    menos, // -
    vezes, // *
    igual, // =
    menor, // <
    maior, // >
    abreparenteses, // (
    fechaparenteses, // )
    abrecolchetes, // [
    fechacolchetes, // ]
    virgula, // ,
    pontoevirgula, // ;
    doispontos, // :
    ponto, // .

    numero,
    identificador
} token;

token tokens[66] = {
    se, // if
    senao, // else
    enquanto, // while
    quatro, // for - resenha
    faz, // do
    constante, // const
    quebrar, // break
    vazio, // void
    wii, // switch - resenha
    caso, // case
    retornar, // return
    caractere, // char
    inteiro, // int
    longo, // long
    curto, // short
    flutuante, // float
    duplo, // double
    continuar, // continue
    padrao, // default
    enumeracao, // enum
    estrutura, // struct
    uniao, // union
    externo, // extern
    irpara, // goto
    nalinha, // inline
    registrar, // register
    assinado, // signed
    naoassinado, // unsigned
    tamanhode, // sizeof
    estatico, // static
    deftipo, // typedef
    tipode, // typeof
    volatil, // volatile

    programa, // program
    rotulo, // label
    tipo, // type
    variavel, // var
    procedimento, // procedure
    funcao, // function
    inicio, // begin
    fim, // end
    atribuicao, // :=
    entao, // then

    dividir, // div
    diferente, // <>
    menorouigual, // <=
    maiorouigual, // >=
    e, // and
    ou, // or
    nao, // not

    /* Símbolos de 1 caractere movidos para o final */
    mais, // +
    menos, // -
    vezes, // *
    igual, // =
    menor, // <
    maior, // >
    abreparenteses, // (
    fechaparenteses, // )
    abrecolchetes, // [
    fechacolchetes, // ]
    virgula, // ,
    pontoevirgula, // ;
    doispontos, // :
    ponto, // .

    numero,
    identificador
};

char *palavras[64] = {
    "if",
    "else",
    "while",
    "for",
    "do",
    "const",
    "break",
    "void",
    "switch",
    "case",
    "return",
    "char",
    "int",
    "long",
    "short",
    "float",
    "double",
    "continue",
    "default",
    "enum",
    "struct",
    "union",
    "extern",
    "goto",
    "inline",
    "register",
    "signed",
    "unsigned",
    "sizeof",
    "static",
    "typedef",
    "typeof",
    "volatile",
    "program",
    "label",
    "type",
    "var",
    "procedure",
    "function",
    "begin",
    "end",
    ":=",
    "then",
    "div",
    "<>",
    "<=",
    ">=",
    "and",
    "or",
    "not",

    /* Símbolos de 1 caractere movidos para o final */
    "+",
    "-",
    "*",
    "=",
    "<",
    ">",
    "(", 
    ")",
    "[",
    "]",
    ",",
    ";",
    ":",
    "."
};

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
    

int main() {
    FILE *arquivo = fopen("codigo.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    char string[100];
    char c;
    char i;

    while ((c = fgetc(arquivo)) != EOF) {
        if (ehSeparador(c)) {
            continue;
        }

        // letra inicia palavra reservada e identificador
        if (ehLetra(c) || c == '_') {
            string[i++] = c;
            while ((c = fgetc(arquivo)) != EOF && ehCaracterDePalavra(c)) {
                string[i++] = c;
            }
            string[i] = '\0';

            token t = qualToken(string);
            printf("Token da palavra '%s': %d\n", string, t);
            if (t == identificador){
                printf("identificador encontrado: %s\n", string);
            }

            i = 0;
            string[0] = '\0';
        }
        
        // número
        if (ehNumero(c)) {
            string[i++] = c;
            while ((c = fgetc(arquivo)) != EOF && ehNumero(c)) {
                string[i++] = c;
            }
            string[i] = '\0';
            
            printf("Token do número %d : %d\n", qualNum(string), numero);

            i = 0;
            string[0] = '\0';
        }
        
        if (ehSimboloUnico(c)){
            char s[2] = {c, '\0'};
            printf("Token de símbolo: %d\n", qualToken(s));
        }
        if (ehSimboloInicial(c)){
            char prox = fgetc(arquivo);
            char s[2] = {c, prox};
            token t = qualToken(s);
            if (t != identificador){
                printf("Token de símbolo: %d\n", t);
            }
            else{
                s[1] = '\0';
                token t = qualToken(s);
                ungetc(prox, arquivo);
                printf("Token de símbolo: %d\n", t);
            }
        }

    }

    fclose(arquivo);
    return 0;
}
