#ifndef GERADOR_MEPA_H
#define GERADOR_MEPA_H

int iniciarMEPA(const char *nomeArquivo);
void finalizarMEPA(void);
void emitirMEPA(const char *formato, ...);

int novoRotulo(void);
void emitirRotulo(int rotulo);

#endif
