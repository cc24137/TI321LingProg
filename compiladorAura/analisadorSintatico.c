#include <stdio.h>
#include <string.h>
#include "analisadorLexico.h"
#include "analisadorSintatico.h"
#include "tipos.h"
#include "tokens.h"
#include <stdlib.h>
#include "tabelaSimbolos.h"
#include "descricaoParametros.h"
#include "geradorMEPA.h"

char escopo = 0;
char tipo_expressao_atual[100] = ""; // var para checkagem de tipos

static simbolo *declararSubrotina(char *nome, naturezas natureza) {
    simbolo *subrotina = adicionaNaTabelaSimbolos(nome, "", escopo, natureza);
    snprintf(subrotina->rotuloMEPA, sizeof subrotina->rotuloMEPA,
             "L%d_%s", novoRotulo(), nome);
    return subrotina;
}

static simbolo *buscarRotuloFonte(int numero) {
    char nome[32];
    snprintf(nome, sizeof nome, "%d", numero);
    simbolo *rotulo = buscarNaTabelaSimbolos(nome);
    if (rotulo == NULL || rotulo->natureza != natureza_rotulo) {
        printf("Rótulo %d não declarado em um bloco visível.\n", numero);
        exit(1);
    }
    return rotulo;
}

void compilaParametroNaChamada(FILE *arquivo, const simbolo *subrotina, int posicao);

static void compilaChamada(FILE *arquivo, const simbolo *subrotina,
                          anaLexReturn token, int descartarResultado) {
    if (subrotina->natureza != natureza_funcao &&
        subrotina->natureza != natureza_procedimento) {
        printf("'%s' não é uma função ou procedimento.\n", subrotina->nome);
        exit(1);
    }

    int ehFuncao = subrotina->natureza == natureza_funcao;
    if (ehFuncao) {
        emitirMEPA("AMEM 1");
    }

    int quantidadeArgumentos = 0;
    if (token.t == abreparenteses) {
        token = obterToken(arquivo);
        if (token.t != fechaparenteses) {
            devolverToken(token);
            do {
                if (quantidadeArgumentos >= subrotina->quantidadeParametros) {
                    printf("Argumentos em excesso para '%s'.\n", subrotina->nome);
                    exit(1);
                }
                compilaParametroNaChamada(arquivo, subrotina, quantidadeArgumentos);
                quantidadeArgumentos++;
                token = obterToken(arquivo);
            } while (token.t == virgula);

            if (token.t != fechaparenteses) {
                printf("Esperava-se fechar os argumentos de '%s'.\n", subrotina->nome);
                exit(1);
            }
        }
    }
    else {
        devolverToken(token);
    }

    if (quantidadeArgumentos != subrotina->quantidadeParametros) {
        printf("Quantidade de argumentos incorreta para '%s'.\n", subrotina->nome);
        exit(1);
    }
    emitirMEPA("CHPR %s, %d", subrotina->rotuloMEPA, escopo - 1);
    if (ehFuncao && descartarResultado) {
        emitirMEPA("DMEM 1");
    }
}

int compilaParametrosFormais(FILE *arquivo, char* nomeFuncOuProc) {
    simbolo *subrotina = buscarNaTabelaSimbolos(nomeFuncOuProc);
    int quantidadeParametros = 0;
    anaLexReturn token = obterToken(arquivo);
    
    if (token.t == abreparenteses) {
        token = obterToken(arquivo);
        if (token.t == fechaparenteses) {
            return 0;
        }
        devolverToken(token);
        do {
            tiposParametro tipoDoParametro = parametro_por_valor;

            token = obterToken(arquivo);

            if (token.t == funcao || token.t == procedimento) {
                printf("Parâmetros que recebem sub-rotinas ainda não são suportados na convenção MEPA atual.\n");
                exit(1);
            }
            if (token.t == variavel) {
                tipoDoParametro = parametro_por_referencia;
                token = obterToken(arquivo);
            }

            if (token.t != identificador) {
                printf("Esperava-se um identificador nos parâmetros formais!\n");
                exit(1);
            }

            // cria vetor para guardar identificadores dos parametros da mesma linha
            // faz isso para só depois guardar todos eles na tabela de símbolos
            int idxParams = 0;
            char* paramsTemporarios[20];
            paramsTemporarios[idxParams] = (char*) malloc(100);
            strcpy(paramsTemporarios[idxParams++], token.palavra);

            token = obterToken(arquivo);
            while (token.t == virgula) {
                token = obterToken(arquivo);
                if (token.t != identificador) {
                    printf("Esperava-se um identificador após a vírgula!\n");
                    exit(1);
                }

                // guarda os proximos identificadores
                paramsTemporarios[idxParams] = (char*) malloc(100);
                strcpy(paramsTemporarios[idxParams++], token.palavra);

                token = obterToken(arquivo);
            }

            char tipoParametro[100] = "";

            if (token.t != doispontos) {
                printf("Esperava-se dois pontos na declaração dos parâmetros!\n");
                exit(1);
            }

            token = obterToken(arquivo);
            if (token.t != identificador && (token.t < 11 || token.t > 16)) {
                printf("Esperava-se um identificador de tipo após os dois pontos!\n");
                exit(1);
            }
            if (token.t == identificador) {
                strcpy(tipoParametro, token.palavra);
            } else {
                strcpy(tipoParametro, palavras[token.t]);
            }
            token = obterToken(arquivo);

            // salva todos os parametros na tabela no escopo atual
            for (int i = 0; i < idxParams; i++) {
                simbolo *parametro = adicionaNaTabelaSimbolos(
                    paramsTemporarios[i], tipoParametro, escopo, natureza_parametro);
                parametro->tipoParametro = tipoDoParametro;
                int indice = inserereNovoParametro(nomeFuncOuProc, tipoParametro, tipoDoParametro);
                if (quantidadeParametros == 0) {
                    subrotina->inicioParametros = indice;
                }
                quantidadeParametros++;
                free(paramsTemporarios[i]);
            }

        } while (token.t == pontoevirgula);

        if (token.t != fechaparenteses) {
            printf("Esperava-se ')' para fechar os parâmetros formais!\n");
            exit(1);
        }
        atribuirEnderecosParametros(escopo);
    }
    else {
        devolverToken(token);
    }

    return quantidadeParametros;
}

int compilaFator(FILE *arquivo) {
    anaLexReturn token = obterToken(arquivo);
    int gerouValor = 0;

    if (token.t == identificador) {

        // verificação semântica no fator -> garannte que o identificador existe e que não é um programa ou procedimento
        const simbolo *declaracao = buscarNaTabelaSimbolos(token.palavra);
        if (declaracao == NULL) {
            printf("Erro Semântico: Identificador '%s' não foi declarado!\n", token.palavra);
            exit(1);
        }

        naturezas nat = declaracao->natureza;
        if (nat == natureza_nomePrograma || nat == natureza_procedimento) {
            printf("Erro Semântico: Uso inválido! O identificador '%s' não pode ser usado dentro de uma expressão matemática/lógica.\n", token.palavra);
            exit(1);
        }

        char tipoOriginalFator[100];
        strcpy(tipoOriginalFator, declaracao->tipo);

        token = obterToken(arquivo);

        if (nat == natureza_funcao) {
            compilaChamada(arquivo, declaracao, token, 0);
            strcpy(tipo_expressao_atual, tipoOriginalFator);
            return 1;
        }

        if ((nat == natureza_variavel ||
             (nat == natureza_parametro &&
              (declaracao->tipoParametro == parametro_por_valor ||
               declaracao->tipoParametro == parametro_por_referencia))) &&
            token.t != abrecolchetes && token.t != abreparenteses) {
            const char *instrucao = nat == natureza_parametro &&
                declaracao->tipoParametro == parametro_por_referencia ? "CRVI" : "CRVL";
            emitirMEPA("%s %d, %d", instrucao, declaracao->escopo - 1, declaracao->endereco);
            gerouValor = 1;
        }

        while (token.t == abrecolchetes) {
            do {
                compilaExpressao(arquivo);
                token = obterToken(arquivo);
            } while (token.t == virgula);

            if (token.t != fechacolchetes) {
                printf("Esperava-se fechacolchetes!\n");
                exit(1);
            }
            token = obterToken(arquivo);
        }

        if (token.t == abreparenteses) {
            printf("'%s' não é uma função.\n", declaracao->nome);
            exit(1);
        }

        // restaura o tipo original ao final do fator
        strcpy(tipo_expressao_atual, tipoOriginalFator);

        devolverToken(token);
    }
    else if (token.t == numero) {
        // tipo inteiro para numeros
        strcpy(tipo_expressao_atual, palavras[inteiro]);
        emitirMEPA("CRCT %d", token.num);
        gerouValor = 1;
    }
    else if (token.t == abreparenteses) {
        gerouValor = compilaExpressao(arquivo);
        token = obterToken(arquivo);
        if (token.t != fechaparenteses) {
            printf("Esperava-se fechaparenteses!\n");
            exit(1);
        }
    }
    else if (token.t == nao) {
        gerouValor = compilaFator(arquivo);
        if (gerouValor) {
            emitirMEPA("NEGA");
        }
    }
    else {
        printf("Esperava-se um fator valido!\n");
        exit(1);
    }

    return gerouValor;
}

int compilaTermo(FILE *arquivo) {
    int gerouValor = compilaFator(arquivo);

    // salva o tipo esquerdo antes de possíveis multiplicações/divisões/and
    char tipoEsq[100];
    strcpy(tipoEsq, tipo_expressao_atual);

    anaLexReturn token = obterToken(arquivo);
    while (token.t == asterisco || token.t == dividir || token.t == e) {
        int gerouDireito = compilaFator(arquivo);

        // verifica com o tipo direito
        char tipoDir[100];
        strcpy(tipoDir, tipo_expressao_atual);
        if (strcmp(tipoEsq, tipoDir) != 0) {
            printf("Erro Semântico: Tipos incompativeis no termo! Nao se pode operar '%s' com '%s'.\n", tipoEsq, tipoDir);
            exit(1);
        }

        gerouValor = gerouValor && gerouDireito;
        if (gerouValor) {
            emitirOperacaoMEPA(token.t);
        }

        token = obterToken(arquivo);
    }
     devolverToken(token);

    return gerouValor;
}

int compilaExpressaoSimples(FILE *arquivo) {
    anaLexReturn token = obterToken(arquivo);
    int inverteSinal = token.t == menos;

    if (token.t != mais && token.t != menos) {
       devolverToken(token);
    }

    int gerouValor = compilaTermo(arquivo);
    if (gerouValor && inverteSinal) {
        emitirMEPA("INVR");
    }

    // salva o tipo esquerdo
    char tipoEsq[100];
    strcpy(tipoEsq, tipo_expressao_atual);

    token = obterToken(arquivo);
    while (token.t == mais || token.t == menos || token.t == ou) {
        int gerouDireito = compilaTermo(arquivo);

        // verifica com o tipo direito da expressao
        char tipoDir[100];
        strcpy(tipoDir, tipo_expressao_atual);
        if (strcmp(tipoEsq, tipoDir) != 0) {
            printf("Erro Semântico: Tipos incompativeis na expressao! Nao se pode operar '%s' com '%s'.\n", tipoEsq, tipoDir);
            exit(1);
        }

        gerouValor = gerouValor && gerouDireito;
        if (gerouValor) {
            emitirOperacaoMEPA(token.t);
        }

        token = obterToken(arquivo);
    }
     devolverToken(token);

    return gerouValor;
}

int compilaExpressao(FILE *arquivo) {
    int gerouValor = compilaExpressaoSimples(arquivo);

    // tipo esquerdo
    char tipoEsq[100];
    strcpy(tipoEsq, tipo_expressao_atual);

    anaLexReturn token = obterToken(arquivo);
    if (token.t == igual || token.t == diferente || token.t == menor || token.t == maior || token.t == menorouigual || token.t == maiorouigual) {
        int gerouDireito = compilaExpressaoSimples(arquivo);

        // compara tipos e transforma resultado em booleano
        char tipoDir[100];
        strcpy(tipoDir, tipo_expressao_atual);
        if (strcmp(tipoEsq, tipoDir) != 0) {
            printf("Erro Semântico: Tipos incompativeis! Nao e possivel comparar '%s' com '%s'.\n", tipoEsq, tipoDir);
            exit(1);
        }
        strcpy(tipo_expressao_atual, "boolean");

        gerouValor = gerouValor && gerouDireito;
        if (gerouValor) {
            emitirOperacaoMEPA(token.t);
        }
    } else {
        devolverToken(token);
    }

    return gerouValor;
}

void compilaParametroNaChamada(FILE* arquivo, const simbolo *subrotina, int posicao){
    descricaoParametro dsp = obtemDescricao(subrotina, posicao);

    if (dsp.tipoParametro != parametro_por_valor){
        anaLexReturn token = obterToken(arquivo);
        if (token.t != identificador){
            printf("Esse parâmetro tem que ser identificador");
            exit(1);
        }

        const simbolo *declaracao = buscarNaTabelaSimbolos(token.palavra);
        if (declaracao == NULL) {
            printf("Erro Semântico: Identificador '%s' não foi declarado!\n", token.palavra);
            exit(1);
        }
        naturezas nat = declaracao->natureza;

        if (dsp.tipoParametro == parametro_funcao && nat != natureza_funcao){
            printf("Esse parâmetro tem que ser função.");
            exit(1);
        }

        if (dsp.tipoParametro == parametro_procedimento && nat != natureza_procedimento){
            printf("Esse parâmetro tem que ser procedimento.");
            exit(1);
        }

        if (dsp.tipoParametro == parametro_por_referencia){
            int ehParametroDeDados = nat == natureza_parametro &&
                (declaracao->tipoParametro == parametro_por_valor ||
                 declaracao->tipoParametro == parametro_por_referencia);
            if (nat != natureza_variavel && !ehParametroDeDados){
                printf("Parâmetro por referência precisa receber variável.");
                exit(1);
            }
            if (strcmp(declaracao->tipo, dsp.tipo) != 0){
                printf("O tipo da variável passada não corresponde ao tipo esperado.");
                exit(1);
            }
            // Uma referência já guarda o endereço da variável original.
            const char *instrucao = nat == natureza_parametro &&
                declaracao->tipoParametro == parametro_por_referencia ? "CRVL" : "CREN";
            emitirMEPA("%s %d, %d", instrucao, declaracao->escopo - 1, declaracao->endereco);
        }
    }
    else{ // nesse caso, o parâmetro é um valor (variável sem ser por referência)
        if (!compilaExpressao(arquivo)) {
            printf("O argumento precisa produzir um valor MEPA.\n");
            exit(1);
        }
        if (strcmp(tipo_expressao_atual, dsp.tipo) != 0){
            printf("A expressão não é compatível com o tipo de parâmetro esperado.");
            exit(1);
        }
    }

}


int compilaComandoSemRotulo(FILE *arquivo) {
    anaLexReturn token = obterToken(arquivo);

    //printf("Token inicial do comando sem rótulo atual: %d\n", token.t);
    if (token.t == 40){
        printf("Token 40: %s\n", palavras[40]);
    }

    if (token.t == identificador) {

        // verificação semântica no comando de atribuição -> garante que o identificador existe
        char nomeAlvoAtribuicao[100];
        strcpy(nomeAlvoAtribuicao, token.palavra);

        //printf("Comando sem rótulo começando com identificador. Palavra: %s\n", nomeAlvoAtribuicao);

        const simbolo *declaracao = buscarNaTabelaSimbolos(nomeAlvoAtribuicao);
        if (declaracao == NULL) {
            printf("Erro Semântico: Variável ou Procedimento '%s' não declarado!\n", nomeAlvoAtribuicao);
            exit(1);
        }

        char abriuColchete = 0;

        token = obterToken(arquivo);
        while (token.t == abrecolchetes) {
            abriuColchete = 1;
            do {
                compilaExpressao(arquivo);
                token = obterToken(arquivo);
            } while (token.t == virgula);

            if (token.t != fechacolchetes) {
                printf("Esperava-se fechacolchetes!\n");
                exit(1);
            }
            token = obterToken(arquivo);
        }

        if (token.t == abreparenteses) {
            compilaChamada(arquivo, declaracao, token, 1);
            return 1;
        }

        if (token.t == atribuicao) {
            // verificacao semantica de atribuição -> garante que o identificador é uma variável e não um procedimento ou programa
            naturezas nat = declaracao->natureza;
            if (nat == natureza_nomePrograma || nat == natureza_procedimento) {
                printf("Erro Semântico: Impossível atribuir valor a '%s', pois ele é um Programa ou Procedimento!\n", nomeAlvoAtribuicao);
                exit(1);
            }

            int gerouValor = compilaExpressao(arquivo);

            // verifica o tipo da atribuição
            const char *tipoVariavelDestino = declaracao->tipo;
            if (strcmp(tipoVariavelDestino, tipo_expressao_atual) != 0) {
                printf("Erro Semântico: Atribuicao invalida! A variavel '%s' e do tipo '%s', mas a expressao enviada a ela e do tipo '%s'.\n",
                        nomeAlvoAtribuicao, tipoVariavelDestino, tipo_expressao_atual);
                exit(1);
            }
            if (gerouValor && nat == natureza_funcao && !abriuColchete) {
                // O escopo da declaração é o nível MEPA do corpo da função.
                emitirMEPA("ARMZ %d, %d", declaracao->escopo, declaracao->endereco);
            }
            else if (gerouValor && (nat == natureza_variavel ||
                (nat == natureza_parametro &&
                 (declaracao->tipoParametro == parametro_por_valor ||
                  declaracao->tipoParametro == parametro_por_referencia))) &&
                !abriuColchete) {
                const char *instrucao = nat == natureza_parametro &&
                    declaracao->tipoParametro == parametro_por_referencia ? "ARMI" : "ARMZ";
                emitirMEPA("%s %d, %d", instrucao, declaracao->escopo - 1, declaracao->endereco);
            }
        }
        else {
            if (abriuColchete){
                printf("Esperava-se uma atribuição!\n");
                exit(1);
            }
            else {
                compilaChamada(arquivo, declaracao, token, 1);
            }
        }
    }

    else if (token.t == irpara) {
        token = obterToken(arquivo);
        if (token.t != numero) {
            printf("Esperava-se um número!\n");
            exit(1);
        }
        simbolo *rotulo = buscarRotuloFonte(token.num);
        int nivelDestino = rotulo->escopo - 1;
        int nivelOrigem = escopo - 1;
        if (nivelDestino == nivelOrigem) {
            emitirMEPA("DSVS %s", rotulo->rotuloMEPA);
        }
        else {
            emitirMEPA("DSVR %s, %d, %d", rotulo->rotuloMEPA, nivelDestino, nivelOrigem);
        }

    }

    else if (token.t == inicio) {
        compilaComando(arquivo);

        token = obterToken(arquivo);
        while (token.t != fim) {
            if (token.t != pontoevirgula) {
                printf("Esperava-se ponto e virgula!\n");
                exit(1);
            }
            compilaComando(arquivo);
            token = obterToken(arquivo);
        }
    }

    else if (token.t == se) {
        int gerouCondicao = compilaExpressao(arquivo);
    
        // verifica se a condição do if é booleana
        if (!gerouCondicao || strcmp(tipo_expressao_atual, "boolean") != 0) {
            printf("Erro Semântico: A condição do IF deve ser booleana!\n");
            exit(1);
        }

        int rotuloSenao = novoRotulo();
        emitirMEPA("DSVF L%d", rotuloSenao);
    
        token = obterToken(arquivo);
        if (token.t != entao) {
            printf("Esperava-se então (then)!\n");
            exit(1);
        }
    
        compilaComando(arquivo);
    
        token = obterToken(arquivo);
        if (token.t == senao) {
            int rotuloFim = novoRotulo();
            emitirMEPA("DSVS L%d", rotuloFim);
            emitirRotulo(rotuloSenao);
            compilaComando(arquivo);
            emitirRotulo(rotuloFim);
        }
        else {
            emitirRotulo(rotuloSenao);
            devolverToken(token);
        }
    }

    else if (token.t == enquanto) {
        int rotuloTeste = novoRotulo();
        int rotuloFim = novoRotulo();

        emitirRotulo(rotuloTeste);
        int gerouCondicao = compilaExpressao(arquivo);

        if (!gerouCondicao || strcmp(tipo_expressao_atual, "boolean") != 0) {
            printf("Erro Semântico: A condição do WHILE deve ser booleana!\n");
            exit(1);
        }

        emitirMEPA("DSVF L%d", rotuloFim);

        token = obterToken(arquivo);
        if (token.t != faz) {
            printf("Esperava-se faz (do)!\n");
            exit(1);
        }

        compilaComando(arquivo);
        emitirMEPA("DSVS L%d", rotuloTeste);
        emitirRotulo(rotuloFim);
    }

    else {
        printf("Esperava-se um comando sem rótulo válido!\n");
        exit(1);
    }

    return 1;
}

int compilaComando(FILE *arquivo) {
    anaLexReturn token = obterToken(arquivo);

    if (token.t == fim){
        devolverToken(token);
        return 0;
    }

    if (token.t == numero) {
        simbolo *rotulo = buscarRotuloFonte(token.num);
        if (rotulo->escopo != escopo) {
            printf("Rótulo %d definido fora do bloco de declaração.\n", token.num);
            exit(1);
        }
        token = obterToken(arquivo);
        if (token.t != doispontos) {
            printf("Esperava-se dois pontos!\n");
            exit(1);
        }
        emitirMEPA("%s: ENRT %d, %d", rotulo->rotuloMEPA,
                   rotulo->escopo - 1, rotulo->quantidadeLocais);
    }
    else {
        devolverToken(token);
    }

    compilaComandoSemRotulo(arquivo);

    return 1;
}

int compilaBloco(FILE *arquivo, char ehAChamadaInicial, char* nomeDestaFuncOuProc) {
    const simbolo *declaracaoDoBloco = ehAChamadaInicial ? NULL :
        buscarNaTabelaSimbolos(nomeDestaFuncOuProc);
    unsigned int qtdVariaveis = 0;
    anaLexReturn token = obterToken(arquivo);

    if (token.t == rotulo) {
        while (token.t != pontoevirgula) {
            token = obterToken(arquivo);
            if (token.t != numero) {
                printf("Esperava-se um número!\n");
                exit(1);
            }

            char nomeRotulo[32];
            snprintf(nomeRotulo, sizeof nomeRotulo, "%d", token.num);
            simbolo *rotuloFonte = adicionaNaTabelaSimbolos(
                nomeRotulo, "", escopo, natureza_rotulo);
            snprintf(rotuloFonte->rotuloMEPA, sizeof rotuloFonte->rotuloMEPA,
                     "R%d_%d", token.num, novoRotulo());
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

    token = obterToken(arquivo);
    while (token.t == tipo) {
        token = obterToken(arquivo);
        if (token.t != identificador) {
            printf("Esperava-se um identificador!\n");
            exit(1);
        }

        while (token.t == identificador) {
            char* nomeDoTipoNovo = (char*) malloc(100);
            strcpy(nomeDoTipoNovo, token.palavra);

            if (token.t != identificador) {
                printf("Esperava-se um identificador!\n");
                exit(1);
            }

            token = obterToken(arquivo);
            if (token.t != atribuicao) {
                printf("Esperava-se um sinal de atribuição!\n");
                exit(1);
            }

            token = obterToken(arquivo);
            if (token.t != inteiro && token.t != longo && token.t != curto && token.t != flutuante && token.t != duplo && token.t != caractere) {
                printf("Esperava-se um tipo! 1\n");
                exit(1);
            }

            adicionaNaTabelaSimbolos(nomeDoTipoNovo, palavras[token.t], escopo, natureza_tipo);
            free(nomeDoTipoNovo);


            token = obterToken(arquivo);
            if (token.t != virgula && token.t != pontoevirgula) {
                printf("Esperava-se uma vírgula ou um ponto e vírgula!\n");
                exit(1);
            }

            // le token em avanço para verificar fim do while
            token = obterToken(arquivo);
        }
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
            int idxVariaveis = 0;
            char* variaveis[20];
            char* nomeDaVariavel = (char*) malloc(100);
            strcpy(nomeDaVariavel, token.palavra);
            variaveis[idxVariaveis++] = nomeDaVariavel;

            token = obterToken(arquivo);
            while (token.t == virgula) {
                token = obterToken(arquivo);
                if (token.t != identificador) {
                    printf("Esperava-se um identificador!\n");
                    exit(1);
                }
                char* nomeDaVariavel = (char*) malloc(100);
                strcpy(nomeDaVariavel, token.palavra);
                variaveis[idxVariaveis++] = nomeDaVariavel;

                token = obterToken(arquivo);
            }
            devolverToken(token);

            token = obterToken(arquivo);
            if (token.t != doispontos) {
                printf("Esperava-se um dois pontos!\n");
                exit(1);
            }

            token = obterToken(arquivo);
            if (token.t != identificador && token.t != inteiro && token.t != longo && token.t != curto && token.t != flutuante && token.t != duplo && token.t != caractere) {
                printf("Esperava-se um tipo! 2\n");
                exit(1);
            }
            char nomeDoTipoDasVariaveis[100];
            if (token.t == identificador){
                strcpy(nomeDoTipoDasVariaveis, token.palavra);
            } else{
                strcpy(nomeDoTipoDasVariaveis, palavras[token.t]);
            }

            for (int i=0; i<idxVariaveis; i++){
                simbolo *novaVariavel = adicionaNaTabelaSimbolos(
                    variaveis[i], nomeDoTipoDasVariaveis, escopo, natureza_variavel);
                novaVariavel->endereco = qtdVariaveis++;
                free(variaveis[i]);
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

    if (qtdVariaveis > 0) {
        emitirMEPA("AMEM %u", qtdVariaveis);
    }
    prepararRotulosDoBloco(escopo, qtdVariaveis);

    token = obterToken(arquivo);

    char temSubRotina = (token.t == procedimento || token.t == funcao);
    int numRotulo = novoRotulo();

    if (temSubRotina){
        emitirMEPA("DSVS L%d", numRotulo); // desvia a execucao para depois dos codigos das subrotinas. o rotulo sera definido depois delas
    }

    while (token.t == procedimento || token.t == funcao) {
        int tipoSubrotina = token.t;
        char nomeSubrotina[100];
        token = obterToken(arquivo);
        if (token.t != identificador) {
            printf("Esperava-se um identificador para a sub-rotina!\n");
            exit(1);
        }
        strcpy(nomeSubrotina, token.palavra);
        simbolo *subrotina = declararSubrotina(nomeSubrotina,
            tipoSubrotina == funcao ? natureza_funcao : natureza_procedimento);
        escopo++;
        subrotina->quantidadeParametros = compilaParametrosFormais(arquivo, nomeSubrotina);

        if (tipoSubrotina == funcao) {
            subrotina->endereco = -(subrotina->quantidadeParametros + 4);

            token = obterToken(arquivo);
            if (token.t != doispontos) {
                printf("Esperava-se dois pontos!\n");
                exit(1);
            }

            token = obterToken(arquivo);
            if (token.t != identificador && (token.t < 11 || token.t > 16)) {
                printf("Esperava-se um identificador!\n");
                exit(1);
            }
            // adiciona o tipo da função na tabela de simbolos para usar na verificação de chamadas e atribuições
            char tipoDaFuncao[100];
            if (token.t == identificador) {
                strcpy(tipoDaFuncao, token.palavra);
            } else {
                strcpy(tipoDaFuncao, palavras[token.t]);
            }
            strcpy(subrotina->tipo, tipoDaFuncao);
        }
        emitirMEPA("%s: ENPR %d", subrotina->rotuloMEPA, escopo - 1);

        token = obterToken(arquivo);
        if (token.t != pontoevirgula) {
            printf("Esperava-se um ponto e vírgula!\n");
            exit(1);
        }

        compilaBloco(arquivo, 0, nomeSubrotina);

        token = obterToken(arquivo);
        if (token.t != pontoevirgula) {
            printf("Esperava-se um ponto e vírgula!\n");
            exit(1);
        }

        // limpa o escopo atual
        apagaEscopoTabelaSimbolos(escopo);
        escopo--;

        // le para possível processo ou funcao seguinte
        token = obterToken(arquivo);
    }

    if (temSubRotina){
        // agora que ja leu todas as subrotinas (saiu do while), coloca o rotulo do que deve ser executado de fato
        emitirRotulo(numRotulo);
    }

    // ja leu o token anteriormente
    if (token.t == inicio) {
        compilaComando(arquivo);

        token = obterToken(arquivo);
        while (token.t != fim) {
            //printf("Token que não é fim: %s\n", palavras[token.t]);
            if (token.t != pontoevirgula) {
                if (token.t == doispontos) {
                    printf("Esperava-se atribuição!\n");
                    exit(1);
                }
                printf("Esperava-se ponto e virgula!\n");
                exit(1);
            }
            compilaComando(arquivo);
            token = obterToken(arquivo);
        }
    }
    else {
        devolverToken(token);
    }

    if (qtdVariaveis > 0) {
        emitirMEPA("DMEM %u", qtdVariaveis);
    }

    if (!ehAChamadaInicial){
        emitirMEPA("RTPR %d, %d", escopo-1, declaracaoDoBloco->quantidadeParametros);
        // sinaliza fim de procedimento/func
        // RTPR k, n, em que k eh o escopo e n eh a quantidade de parametros da funcao
    }

    return 1;
}

void definirReadEWrite(){
    int nRotulo = novoRotulo();
    emitirMEPA("DSVS L%d", nRotulo); // pula a definicao do read e do write na execucao principal
    // func read
    emitirMEPA("%s: ENPR 1", buscarNaTabelaSimbolos("read")->rotuloMEPA);
    emitirMEPA("LEIT");
    emitirMEPA("ARMI 1, -4");
    emitirMEPA("RTPR 1, 1");
    // func write
    emitirMEPA("%s: ENPR 1", buscarNaTabelaSimbolos("write")->rotuloMEPA);
    emitirMEPA("CRVL 1, -4");
    emitirMEPA("IMPR");
    emitirMEPA("RTPR 1, 1");
    
    emitirRotulo(nRotulo); //
}

void compilaPrograma(FILE *arquivo) {
    anaLexReturn token = obterToken(arquivo);

    if (token.t != programa) {
        printf("Esperava-se a palavra PROGRAM!\n");
        exit(1);
    }

    token = obterToken(arquivo);
    if (token.t != identificador) {
        printf("Esperava-se um identificador para o nome do programa!\n");
        exit(1);
    }

    // nome do programa
    adicionaNaTabelaSimbolos(token.palavra, "", escopo, natureza_nomePrograma);
    // funcoes nativas
    simbolo *rotinaRead = declararSubrotina("read", natureza_procedimento);
    rotinaRead->inicioParametros = inserereNovoParametro(
        "read", "integer", parametro_por_referencia);
    rotinaRead->quantidadeParametros = 1;
    simbolo *rotinaWrite = declararSubrotina("write", natureza_procedimento);
    rotinaWrite->inicioParametros = inserereNovoParametro(
        "write", "integer", parametro_por_valor);
    rotinaWrite->quantidadeParametros = 1;


    token = obterToken(arquivo);
    if (token.t != abreparenteses) {
        printf("Esperava-se um abre parenteses!\n");
        exit(1);
    }

    while (token.t != fechaparenteses) {
        token = obterToken(arquivo);
        if (token.t != identificador) {
            printf("Esperava-se um identificador!\n");
            exit(1);
        }

        // adiciona parametros do programa
        adicionaNaTabelaSimbolos(token.palavra, "", escopo, natureza_parametro);


        token = obterToken(arquivo);
        if (token.t != virgula && token.t != fechaparenteses) {
            printf("Esperava-se um virgula ou um fecha parenteses!\n");
            exit(1);
        }
    }

    token = obterToken(arquivo);
    if (token.t != pontoevirgula) {
        printf("Esperava-se um ponto e virgula!\n");
        exit(1);
    }

    // O bloco principal entra no escopo 1 da tabela, que corresponde ao nível 0 da MEPA.
    // Para acessar uma variável na MEPA, use o escopo da declaração menos 1.
    escopo++;
    definirReadEWrite();
    compilaBloco(arquivo, 1, ""); // compila o bloco principal do programa
    apagaEscopoTabelaSimbolos(escopo);
    escopo--;

    token = obterToken(arquivo);
    if (token.t != ponto) {
        printf("Esperava-se um ponto final ao término do programa!\n");
        exit(1);
    }

    token = obterToken(arquivo);
    if (token.t!=fimdearquivo)
    {
        printf("Esperava-se fim de arquivo!\n");
        exit(1);
    }

    printf("Programa sintática e semanticamente correto!\n");

    //printaAi();
}
