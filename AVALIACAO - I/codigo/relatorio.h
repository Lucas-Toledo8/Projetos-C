#ifndef RELATORIO_H
#define RELATORIO_H

#include "dispositivo.h"

void listarExibir_ID(Dispositivo **lista, int status);
void listarExibir_IP(Dispositivo **lista, int status);
void listarExibir_CB(Dispositivo **lista, int status);


void exibirResumoEstatistico(Dispositivo *lista, int *v);
void analisarConsumoRede(Dispositivo *lista, int *v);
void verificarIPsDuplicados(Dispositivo *lista, int *v);

#endif
