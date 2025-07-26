#ifndef MENU_H
#define MENU_H

#include "dispositivo.h"
#include "relatorio.h"



void menu(int *v);
void listarDispositivosOrdenados(Dispositivo **lista);
void listarDispositivosOpcoes(Dispositivo **lista, int *v);
void buscando(Dispositivo *lista, int *v);

void buscarRelatorio(Dispositivo *lista, int *v);

int lerNumeroComLimite(int min, int max);

void limparTela();

#endif
