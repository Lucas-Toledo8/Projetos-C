#ifndef ORDENACAO_SOF_H
#define ORDENACAO_SOF_H

#include "dispositivo.h"
#include <stdint.h>

Dispositivo *ultimoNo(Dispositivo *lista);

void quickSortIP(Dispositivo *inicio, Dispositivo *fim);



// Definição da estrutura auxiliar para ordenação por IP numérico
typedef struct DispositivoOrdenado {
    Dispositivo *original;
    uint32_t ipNumerico;
    struct DispositivoOrdenado *prox;
} DispositivoOrdenado;

// Declaração das funções
uint32_t converterIPParaNumero(const char *ip);
DispositivoOrdenado *copiarListaNumerica(Dispositivo *listaOriginal);
void quickSortNumerico(DispositivoOrdenado **lista);
void restaurarListaOriginal(Dispositivo **lista, DispositivoOrdenado *listaOrdenada);



void mergeSortConsumo(Dispositivo **lista);

#endif
