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


int compilaParametrosFormais(FILE *arquivo, char* nomeFuncOuProc) {
    anaLexReturn token = obterToken(arquivo);
    descricaoParametro descParam;
    strcpy(descParam.nomeFuncOuProc, nomeFuncOuProc);
    
    if (token.t == abreparenteses) {
        do {
            descParam.ehFunc = 0;
            descParam.ehProc = 0;
            descParam.ehRef = 0;

            token = obterToken(arquivo);
            int ehProcedure = 0;

            if (token.t == funcao) {
                descParam.ehFunc = 1;
                token = obterToken(arquivo);
            } else if (token.t == procedimento) {
                descParam.ehProc = 1;
                ehProcedure = 1;
                token = obterToken(arquivo);
            } else if (token.t == variavel) {
                descParam.ehRef = 1;
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

            // guardar o tipo dos parametros -> tem que ser vazio para procedure
            char tipoParametro[100] = "";

            if (!ehProcedure) {
                if (token.t != doispontos) {
                    printf("Esperava-se dois pontos na declaração dos parâmetros!\n");
                    exit(1);
                }

                token = obterToken(arquivo);
                if (token.t != identificador && (token.t < 11 || token.t > 16)) {
                    printf("Esperava-se um identificador de tipo após os dois pontos!\n");
                    exit(1);
                }

                // copia o nome do tipo que leu
                if (token.t == identificador) {
                    strcpy(tipoParametro, token.palavra);
                } else {
                    strcpy(tipoParametro, palavras[token.t]);
                }

                token = obterToken(arquivo);
            }

            // salva todos os parametros na tabela no escopo atual
            for (int i = 0; i < idxParams; i++) {
                adicionaNaTabelaSimbolos(paramsTemporarios[i], tipoParametro, escopo, natureza_parametro);
                inserereNovoParametro(descParam.nomeFuncOuProc, tipoParametro, descParam.ehRef, descParam.ehProc, descParam.ehFunc);
                free(paramsTemporarios[i]);
            }

        } while (token.t == pontoevirgula);

        if (token.t != fechaparenteses) {
            printf("Esperava-se ')' para fechar os parâmetros formais!\n");
            exit(1);
        }
    }
    else {
        devolverToken(token);
    }

    return 1;
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

        if (nat == natureza_variavel && token.t != abrecolchetes && token.t != abreparenteses) {
            emitirMEPA("CRVL %d, %u", declaracao->escopo - 1, declaracao->endereco);
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

        while (token.t == abreparenteses) {
            do {
                compilaExpressao(arquivo);
                token = obterToken(arquivo);
            } while (token.t == virgula);

            if (token.t != fechaparenteses) {
                printf("Esperava-se fechaparenteses!\n");
                exit(1);
            }
            token = obterToken(arquivo);
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

void compilaParametroNaChamada(FILE* arquivo, char* nomeFuncOuProc, int posicao){
    descricaoParametro dsp = obtemDescricao(nomeFuncOuProc, posicao);

    if (dsp.ehFunc || dsp.ehProc || dsp.ehRef){
        anaLexReturn token = obterToken(arquivo);
        if (token.t != identificador){
            printf("Esse parâmetro tem que ser identificador");
        }
        
        naturezas nat = obterNaturezaNaTabela(token.palavra);

        if (dsp.ehFunc && nat != natureza_funcao){
            printf("Esse parâmetro tem que ser função.");
            exit(1);
        }

        if (dsp.ehProc && nat != natureza_procedimento){
            printf("Esse parâmetro tem que ser procedimento.");
            exit(1);
        }

        if (dsp.ehRef){
            if (nat != natureza_parametro && nat != natureza_variavel){
                printf("Parâmetro por referência precisa receber variável.");
                exit(1);
            }
            char* tipo = obterTipoNaTabela(token.palavra);
            if (strcmp(tipo, dsp.tipo) != 0){
                printf("O tipo da variável passada não corresponde ao tipo esperado.");
                exit(1);
            }
        }
    }
    else{ // nesse caso, o parâmetro é um valor (variável sem ser por referência)
        compilaExpressao(arquivo);
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
            int posicao = 0;
            do {
                //compilaExpressao(arquivo);
                compilaParametroNaChamada(arquivo, nomeAlvoAtribuicao, posicao);
                token = obterToken(arquivo);
                posicao++;
            } while (token.t == virgula);

            if (token.t != fechaparenteses) {
                printf("Esperava-se fechaparenteses!\n");
                exit(1);
            }

            emitirMEPA("CHPR %s, %d", nomeAlvoAtribuicao, escopo-1);

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
            if (gerouValor && nat == natureza_variavel && !abriuColchete) {
                emitirMEPA("ARMZ %d, %u", declaracao->escopo - 1, declaracao->endereco);
            }
        }
        else {
            if (abriuColchete){
                printf("Esperava-se uma atribuição!\n");
                exit(1);
            }
            else {
                //printf("Entrou no else do identificador. Identificador: %s\n", nomeAlvoAtribuicao);
                naturezas nat = declaracao->natureza;
                if (nat != natureza_funcao && nat != natureza_procedimento) {
                    printf("Só se pode chamar funções ou procedimentos. %s não é nem um nem outro.\n", nomeAlvoAtribuicao);
                    exit(1);
                }
                if (temParametro(nomeAlvoAtribuicao)){
                    printf("Esperava-se parâmetro(s) para a função ou procedimento %s\n", nomeAlvoAtribuicao);
                    exit(1);
                }
                emitirMEPA("CHPR %s, %d", nomeAlvoAtribuicao, escopo-1);
            }
            devolverToken(token);
        }
    }

    else if (token.t == irpara) {
        token = obterToken(arquivo);
        if (token.t != numero) {
            printf("Esperava-se um número!\n");
            exit(1);
        }
        // falta verificar se o numero em questao ja foi compilado
        emitirMEPA("DSVS R%d", token.num); // desvio incondicional

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
        compilaExpressao(arquivo);
    
        // verifica se a condição do if é booleana
        if (strcmp(tipo_expressao_atual, "boolean") != 0) {
            printf("Erro Semântico: A condição do IF deve ser booleana!\n");
            exit(1);
        }
    
        token = obterToken(arquivo);
        if (token.t != entao) {
            printf("Esperava-se então (then)!\n");
            exit(1);
        }
    
        compilaComandoSemRotulo(arquivo);
    
        token = obterToken(arquivo);
        if (token.t == senao) {
            compilaComandoSemRotulo(arquivo);
        }
        else {
            devolverToken(token);
        }
    }

    else if (token.t == enquanto) {
        compilaExpressao(arquivo);

        token = obterToken(arquivo);
        if (token.t != faz) {
            printf("Esperava-se faz (do)!\n");
            exit(1);
        }

        compilaComandoSemRotulo(arquivo);
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
        // falta verificar se o numero tinha sido declarado com label

        // logica para colocar o rotulo desse label
        emitirMEPA("R%d: NADA", token.num);

        token = obterToken(arquivo);
        if (token.t != doispontos) {
            printf("Esperava-se dois pontos!\n");
            exit(1);
        }
    }
    else {
        devolverToken(token);
    }

    compilaComandoSemRotulo(arquivo);

    return 1;
}

int compilaBloco(FILE *arquivo, char ehAChamadaInicial, char* nomeDestaFuncOuProc) {
    unsigned int qtdVariaveis = 0;
    anaLexReturn token = obterToken(arquivo);

    if (token.t == rotulo) {
        while (token.t != pontoevirgula) {
            token = obterToken(arquivo);
            if (token.t != numero) {
                printf("Esperava-se um número!\n");
                exit(1);
            }

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

    token = obterToken(arquivo);

    char temSubRotina = (token.t == procedimento || token.t == funcao);
    int numRotulo = novoRotulo();

    if (temSubRotina){
        emitirMEPA("DSVS L%d", numRotulo); // desvia a execucao para depois dos codigos das subrotinas. o rotulo sera definido depois delas
    }

    while (token.t == procedimento || token.t == funcao) {

        // usa int para evitar conlito com a variavel de tokens opsss
        // guarda o tipo da sub-rotina para usar depois na hora de salvar a assinatura na tabela de simbolos
        int tipoSubrotina = token.t;
        char nomeSubrotina[100];

        if (tipoSubrotina == procedimento) {
            token = obterToken(arquivo);
            if (token.t != identificador) {
                printf("Esperava-se um identificador!\n");
                exit(1);
            }
            // salva o nome
            strcpy(nomeSubrotina, token.palavra);

            // adiciona a assinatura no escopo pai
            // entra no novo escopo
            adicionaNaTabelaSimbolos(nomeSubrotina, "", escopo, natureza_procedimento);
            escopo++;

            compilaParametrosFormais(arquivo, nomeSubrotina);

            emitirMEPA("%s: ENPR %d", nomeSubrotina, escopo-1); // nome da func/proc eh o rotulo no MEPA, ENPR fala que eh inicio de um procedimento
        }

        if (tipoSubrotina == funcao) {
            token = obterToken(arquivo);
            if (token.t != identificador) {
                printf("Esperava-se um identificador!\n");
                exit(1);
            }
            strcpy(nomeSubrotina, token.palavra);

            escopo++;
            compilaParametrosFormais(arquivo, nomeSubrotina);

            emitirMEPA("%s: ENPR %d", nomeSubrotina, escopo-1); // nome da func/proc eh o rotulo no MEPA

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
            // escopo -1 por usar o escopo da funcao para salvar a assinatura dela na tabela de simbolos, e não o escopo do pai
            adicionaNaTabelaSimbolos(nomeSubrotina, tipoDaFuncao, escopo - 1, natureza_funcao);
        }

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
        emitirMEPA("RTPR %d %d", escopo-1, qtsParametrosTem(nomeDestaFuncOuProc)); 
        // sinaliza fim de procedimento/func
        // RTPR k, n, em que k eh o escopo e n eh a quantidade de parametros da funcao
    }

    return 1;
}

void definirReadEWrite(){
    int nRotulo = novoRotulo();
    emitirMEPA("DSVS L%d", nRotulo); // pula a definicao do read e do write na execucao principal
    // func read
    emitirMEPA("read: ENPR 1");
    emitirMEPA("LEIT");
    emitirMEPA("ARMI 1, -4");
    emitirMEPA("RTPR 1, 1");
    // func write
    emitirMEPA("write: ENPR 1");
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
    adicionaNaTabelaSimbolos("read", "", escopo, natureza_procedimento);
    adicionaNaTabelaSimbolos("write", "", escopo, natureza_procedimento);

    inserereNovoParametro("read", "integer", 1, 0, 0);
    inserereNovoParametro("write", "integer", 0, 0, 0);


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
