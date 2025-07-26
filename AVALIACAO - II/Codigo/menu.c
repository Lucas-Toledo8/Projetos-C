#include "menu.h"
#include "fazer_pedido.h"
#include "retirar_pedido.h"
#include "busca.h"
#include "listar_pedidos_ordenado.h"
#include "relatorio.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>



// funcão para limpar a tela ---
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// funcão para pausar
void pausar(int segundos) {
    #ifdef _WIN32
        Sleep(segundos * 1000);
    #else
        sleep(segundos);
    #endif
}




// Implementação da função para converter string para maiúsculas
void toUpperCase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = (char)toupper((unsigned char)str[i]);
    }
}


int lerNumeroComLimite(int min, int max) {
    char input[100];
    int numero;
    int valido = 0;

    while (!valido) {
        printf("Digite um numero entre %d e %d: ", min, max);
        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = 0;

            int i;
            for (i = 0; input[i] != '\0'; i++) {
                if (!isdigit((unsigned char)input[i])) {
                    break;
                }
            }

            if (input[i] == '\0' && strlen(input) > 0) {
                numero = atoi(input);
                if (numero >= min && numero <= max) {
                    valido = 1;
                } else {
                    printf("Entrada fora do limite. ");
                }
            } else {
                printf("Entrada invalida. Digite apenas numeros. ");
            }
        } else {
            printf("Erro de leitura. Tente novamente. ");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    }
    return numero;
}


// --- FUNÇÃO PARA VALIDAR A OPÇÃO DO MENU ---
int obterOpcaoMenu() {
    char input_buffer[100];
    int opcao = -1;

    printf("Escolha uma opcao: ");

    if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
        printf("\nErro de leitura.\n");
        return -1;
    }

    input_buffer[strcspn(input_buffer, "\n")] = 0;

    if (strlen(input_buffer) == 0) {
        printf("\nEntrada vazia. Por favor, digite um numero.\n");
        return -1;
    }

    if (sscanf(input_buffer, "%d", &opcao) != 1) {
        printf("\nEntrada invalida. Por favor, digite um numero inteiro.\n");
        return -1;
    }

    return opcao;
}

// --- FUNÇÃO AUXILIAR PARA EXIBIR O MENU ---
void exibirMenu() {
    printf("\n========================================\n");
    printf("  Sistema de Gerenciamento de Lanchonete\n");
    printf("========================================\n");
    printf("1 - Fazer Pedido\n");
    printf("2 - Retirar Pedido\n");
    printf("3 - Buscar Pedido\n");
    printf("4 - Consulta Geral\n");
    printf("5 - Relatorio de Vendas\n");
    printf("0 - Sair\n");
    printf("========================================\n");
}

// --- FUNÇÃO PRINCIPAL DO MENU ---
void menu() {
    int opcao;

    // Simula um Enter no buffer antes da entrada
    ungetc('\n', stdin);

    // Limpa qualquer conteúdo residual da entrada
    getchar();

    while (1) {
        limparTela();


        exibirMenu();
        opcao = obterOpcaoMenu();

        switch (opcao) {
            case 1:
                printf("Opcao 1: Seja bem vindo! Fazer Pedido selecionado.\n");
                fazer_pedido_ui();
                break;
            case 2:
                printf("Opcao 2: Retirar Pedido selecionado.\n");
                retirar_pedido_ui();
                break;
            case 3:
                printf("Opcao 3: Buscar Pedido selecionado.\n");
                buscar_pedidos_por_item_ui();
                break;
            case 4:
                printf("Opcao 4: Consulta Geral selecionado.\n");
                exibir_pedidos_ordenados_ui();
                break;
            case 5:
                printf("Opcao 6: Relatorio de Vendas selecionado.\n");
                exibir_relatorios_ui();
                break;
            case 0:
                printf("Saindo do programa...\n");

                return;
            default:
                pausar(2);
                break;
        }
    }
}
