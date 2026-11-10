#ifndef GERADOR_MEPA_H
#define GERADOR_MEPA_H

#include "tipos.h"

int iniciarMEPA(const char *nomeArquivo);
void finalizarMEPA(void);
void emitirMEPA(const char *formato, ...);
void emitirOperacaoMEPA(token operador);

int novoRotulo(void);
void emitirRotulo(int rotulo);

#endif
