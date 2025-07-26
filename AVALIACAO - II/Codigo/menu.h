#ifndef MENU_H
#define MENU_H

#include "fazer_pedido.h"


// Prot�tipos das fun��es do menu
void menu();
void exibirMenu();
int obterOpcaoMenu();
int lerNumeroComLimite(int min, int max);

void toUpperCase(char *str);


// Inclu��es necess�rias para as fun��es de limpeza e pausa
#include <stdlib.h>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

// Prot�tipos das fun��es de utilidade
void limparTela();
void pausar(int segundos);


#endif
