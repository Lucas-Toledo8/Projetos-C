#include "ordenacao_basic.h"
#include <stdio.h>
#include <stdlib.h>

void insertionSort(Dispositivo **lista, int criterio) {
  if (!(*lista) || criterio != 1)
    return; 

  Dispositivo *ordenado = NULL;
  Dispositivo *atual = *lista;

  while (atual) {
    Dispositivo *prox = atual->prox;
    Dispositivo *temp = ordenado;

    while (temp && atual->id > temp->id) {
      temp = temp->prox;
    }

    if (!ordenado || atual->id < ordenado->id) {
      atual->prox = ordenado;
      ordenado = atual;
    } else {
      Dispositivo *aux = ordenado;
      while (aux->prox && aux->prox->id < atual->id) {
        aux = aux->prox;
      }
      atual->prox = aux->prox;
      aux->prox = atual;
    }

    atual = prox;
  }

  *lista = ordenado;
}
