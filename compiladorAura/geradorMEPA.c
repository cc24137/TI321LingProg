#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "geradorMEPA.h"

static FILE *arquivoMEPA = NULL;
static int contadorRotulos = 0;

// Abre o arquivo que receberá as instruções
int iniciarMEPA(const char *nomeArquivo) {
    arquivoMEPA = fopen(nomeArquivo, "w");

    if (arquivoMEPA == NULL) {
        return 0;
    }

    contadorRotulos = 0;
    return 1;
}

// Escreve uma instrução no arquivo de saída
void emitirMEPA(const char *formato, ...) {
    if (arquivoMEPA == NULL) {
        fprintf(stderr, "Erro: arquivo MEPA nao inicializado!\n");
        exit(1);
    }

    va_list argumentos;
    va_start(argumentos, formato);

    vfprintf(arquivoMEPA, formato, argumentos);

    va_end(argumentos);

    fprintf(arquivoMEPA, "\n");
}

// Retorna um novo identificador de rótulo
int novoRotulo(void) {
    return contadorRotulos++;
}

// Escreve um rótulo no arquivo
void emitirRotulo(int rotulo) {
    emitirMEPA("L%d NADA", rotulo);
}

// Fecha o arquivo de saída
void finalizarMEPA(void) {
    if (arquivoMEPA != NULL) {
        fclose(arquivoMEPA);
        arquivoMEPA = NULL;
    }
}