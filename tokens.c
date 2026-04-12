#include "tokens.h"
#include "tipos.h"

token tokens[66] = {
    se, // if                   0
    senao, // else              1
    enquanto, // while          2
    quatro, // for - resenha    3
    faz, // do                  4
    constante, // const         5
    quebrar, // break           6
    vazio, // void              7
    wii, // switch - resenha    8
    caso, // case               9
    retornar, // return         10
    caractere, // char          11
    inteiro, // int             12
    longo, // long              13
    curto, // short             14
    flutuante, // float         15
    duplo, // double            16
    continuar, // continue
    padrao, // default
    enumeracao, // enum
    estrutura, // struct        20
    uniao, // union
    externo, // extern
    irpara, // goto
    nalinha, // inline          24
    registrar, // register
    assinado, // signed
    naoassinado, // unsigned
    tamanhode, // sizeof        28
    estatico, // static
    deftipo, // typedef
    tipode, // typeof
    volatil, // volatile        32

    programa, // program        33
    rotulo, // label
    tipo, // type
    variavel, // var            36
    procedimento, // procedure
    funcao, // function
    inicio, // begin            39
    fim, // end
    atribuicao, // :=           41
    entao, // then              42

    dividir, // div
    diferente, // <>
    menorouigual, // <=
    maiorouigual, // >=         46
    e, // and
    ou, // or
    nao, // not                 49

    /* Símbolos de 1 caractere movidos para o final */
    mais, // +                  50
    menos, // -
    vezes, // *
    igual, // =                 53
    menor, // <
    maior, // >
    abreparenteses, // (
    fechaparenteses, // )
    abrecolchetes, // [
    fechacolchetes, // ]
    virgula, // ,
    pontoevirgula, // ;         61
    doispontos, // :            62
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