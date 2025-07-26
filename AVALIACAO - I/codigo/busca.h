#ifndef BUSCA_H
#define BUSCA_H

#include "dispositivo.h"

// Busca binária para IP com modificação na lista
void buscaBinariaPorIP(Dispositivo **lista, const char *ip_busca);

// Busca sequencial por nome
void buscaSequencialPorNome(Dispositivo *lista, const char *nome_inicial);

#endif // BUSCA_H
