
#ifndef tipos_h
#define tipos_h

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
    asterisco, // *
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
    identificador,
    fimdearquivo
} token;

typedef struct {
    char palavra[100];
    token t;
    int num;
} anaLexReturn;

typedef enum {
    natureza_procedimento,
    natureza_funcao,
    natureza_parametro,
    natureza_variavel,
    natureza_nomePrograma,
    natureza_tipo,
    natureza_rotulo
} naturezas;

typedef enum {
    parametro_por_valor,
    parametro_por_referencia,
    parametro_funcao,
    parametro_procedimento
} tiposParametro;

typedef struct {
    char nome[100];
    char tipo[100];
    naturezas natureza;
    int endereco;
    char escopo;
    tiposParametro tipoParametro; // Usado quando a natureza é natureza_parametro.
    int inicioParametros;
    int quantidadeParametros;
    char rotuloMEPA[128];
    int quantidadeLocais;
} simbolo;

typedef struct {
    char tipo[100];
    char nomeFuncOuProc[100]; // o nome do proc/func desse parâmetro
    tiposParametro tipoParametro;
} descricaoParametro;

#endif
