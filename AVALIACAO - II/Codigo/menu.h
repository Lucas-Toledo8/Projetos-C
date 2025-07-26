#ifndef MENU_H
#define MENU_H

#include "fazer_pedido.h"


// Protótipos das funções do menu
void menu();
void exibirMenu();
int obterOpcaoMenu();
int lerNumeroComLimite(int min, int max);

void toUpperCase(char *str);


// Incluções necessárias para as funções de limpeza e pausa
#include <stdlib.h>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

// Protótipos das funções de utilidade
void limparTela();
void pausar(int segundos);


#endif
