#include "ordenacao_sof.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Converter IP para número inteiro
uint32_t converterIPParaNumero(const char *ip) {
  unsigned int byte1, byte2, byte3, byte4;
  sscanf(ip, "%u.%u.%u.%u", &byte1, &byte2, &byte3, &byte4);
  return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

// Criar uma cópia da lista original com IPs numéricos
DispositivoOrdenado *copiarListaNumerica(Dispositivo *listaOriginal) {
  DispositivoOrdenado *novaLista = NULL, *ultimo = NULL;

  while (listaOriginal) {
    DispositivoOrdenado *novo = (DispositivoOrdenado *)malloc(sizeof(DispositivoOrdenado));
    novo->original = listaOriginal;
    novo->ipNumerico = converterIPParaNumero(listaOriginal->ip);
    novo->prox = NULL;

    if (!novaLista) {
      novaLista = novo;
    } else {
      ultimo->prox = novo;
    }
    ultimo = novo;
    listaOriginal = listaOriginal->prox;
  }

  return novaLista;
}

// Função auxiliar para trocar dispositivos na lista numérica
void trocarDispositivosOrdenados(DispositivoOrdenado *a, DispositivoOrdenado *b) {
  uint32_t tempIp = a->ipNumerico;
  Dispositivo *tempOriginal = a->original;

  a->ipNumerico = b->ipNumerico;
  a->original = b->original;

  b->ipNumerico = tempIp;
  b->original = tempOriginal;
}

// Particionamento para Quick Sort baseado na conversão numérica dos IPs
DispositivoOrdenado *particionarNumerico(DispositivoOrdenado *inicio, DispositivoOrdenado *fim) {
  uint32_t pivoValor = fim->ipNumerico;
  DispositivoOrdenado *i = inicio, *j = inicio;

  while (j != fim) {
    if (j->ipNumerico < pivoValor) {
      trocarDispositivosOrdenados(i, j);
      i = i->prox;
    }
    j = j->prox;
  }

  trocarDispositivosOrdenados(i, fim);
  return i;
}

void quickSortNumerico(DispositivoOrdenado **lista) {
  if (!lista || !*lista || !(*lista)->prox)
    return;

  DispositivoOrdenado *fim = *lista;
  while (fim->prox)
    fim = fim->prox;

  DispositivoOrdenado *pivo = particionarNumerico(*lista, fim);

  if (*lista != pivo) {
    DispositivoOrdenado *aux = *lista;
    while (aux->prox != pivo)
      aux = aux->prox;
    aux->prox = NULL;
    quickSortNumerico(lista);
    aux->prox = pivo;
  }

  quickSortNumerico(&pivo->prox);
}

void restaurarListaOriginal(Dispositivo **lista, DispositivoOrdenado *listaOrdenada) {
  Dispositivo *novaLista = NULL, *ultimo = NULL;

  while (listaOrdenada) {
    Dispositivo *dispositivo = listaOrdenada->original;
    dispositivo->prox = NULL;

    if (!novaLista) {
      novaLista = dispositivo;
    } else {
      ultimo->prox = dispositivo;
    }
    ultimo = dispositivo;
    listaOrdenada = listaOrdenada->prox;
  }

  *lista = novaLista;
}

// Funcao Merge
Dispositivo *mergeConsumo(Dispositivo *esquerda, Dispositivo *direita) {
  if (!esquerda)
    return direita;
  if (!direita)
    return esquerda;

  Dispositivo *resultado = NULL;

  if (esquerda->consumo_banda >= direita->consumo_banda) {
    resultado = esquerda;
    resultado->prox = mergeConsumo(esquerda->prox, direita);
  } else {
    resultado = direita;
    resultado->prox = mergeConsumo(esquerda, direita->prox);
  }

  return resultado;
}

// Função para dividir a lista ao meio (Merge Sort)
void dividirLista(Dispositivo *origem, Dispositivo **esquerda, Dispositivo **direita) {
  Dispositivo *rapido = origem->prox, *lento = origem;

  while (rapido && rapido->prox) {
    rapido = rapido->prox->prox;
    lento = lento->prox;
  }

  *esquerda = origem;
  *direita = lento->prox;
  lento->prox = NULL;
}

// Implementação do Merge Sort para ordenar por Consumo de Banda
void mergeSortConsumo(Dispositivo **lista) {
  if (!(*lista) || !(*lista)->prox)
    return;

  Dispositivo *esquerda, *direita;
  dividirLista(*lista, &esquerda, &direita);

  mergeSortConsumo(&esquerda);
  mergeSortConsumo(&direita);

  *lista = mergeConsumo(esquerda, direita);
}
