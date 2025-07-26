#include "retirar_pedido.h"
#include "menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>


extern ItemCardapio cardapio[MAX_ITENS_CARDAPIO];
extern int totalItensCardapio;

// Função para carregar pedidos de um arquivo
Pedido* carregarPedidosDoArquivo(const char* nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");
    if (!f) {
        return NULL;
    }

    Pedido* lista = NULL;
    Pedido* ultimo_pedido = NULL;
    char linha[512];

    while (fgets(linha, sizeof(linha), f) != NULL) {
        linha[strcspn(linha, "\n")] = 0;

        if (strlen(linha) == 0) {
            continue;
        }

        Pedido* novoPedido = (Pedido*)malloc(sizeof(Pedido));
        if (!novoPedido) {

            liberarListaPedidos(lista);
            fclose(f);
            return NULL;
        }
        memset(novoPedido, 0, sizeof(Pedido));
        novoPedido->listaItens = NULL;
        novoPedido->prox = NULL;

        char *linha_copia = strdup(linha);
        if (!linha_copia) {

            free(novoPedido);
            liberarListaPedidos(lista);
            fclose(f);
            return NULL;
        }

        char *token;
        #ifdef _WIN32
        char *context = NULL;
        token = strtok_s(linha_copia, ";", &context);
        #else
        char *context = NULL;
        token = strtok_r(linha_copia, ";", &context);
        #endif

        // Parsing dos 3 primeiros IDs
        if (token) {
            strncpy(novoPedido->idPedido, token, ID_SIZE - 1);
            novoPedido->idPedido[ID_SIZE - 1] = '\0';
            toUpperCase(novoPedido->idPedido);
        } else { goto erro_parsing_linha; }

        #ifdef _WIN32
        token = strtok_s(NULL, ";", &context);
        #else
        token = strtok_r(NULL, ";", &context);
        #endif
        if (token) {
            strncpy(novoPedido->idCliente, token, ID_SIZE - 1);
            novoPedido->idCliente[ID_SIZE - 1] = '\0';

        } else { goto erro_parsing_linha; }

        #ifdef _WIN32
        token = strtok_s(NULL, ";", &context);
        #else
        token = strtok_r(NULL, ";", &context);
        #endif
        if (token) {
            strncpy(novoPedido->idEntrega, token, ID_SIZE - 1);
            novoPedido->idEntrega[ID_SIZE - 1] = '\0';

        } else { goto erro_parsing_linha; }


        while (1) {
            #ifdef _WIN32
            char *item_id_str = strtok_s(NULL, ";", &context); // Ex: "OP1"
            #else
            char *item_id_str = strtok_r(NULL, ";", &context);
            #endif

            if (!item_id_str || strncmp(item_id_str, "OP", 2) != 0) {
                token = item_id_str;
                break;
            }

            #ifdef _WIN32
            char *item_qty_str = strtok_s(NULL, ";", &context);
            #else
            char *item_qty_str = strtok_r(NULL, ";", &context);
            #endif

            if (!item_qty_str || strncmp(item_qty_str, "OP", 2) != 0 || strchr(item_qty_str, 'q') == NULL) {
                goto erro_parsing_linha;
            }

            int idItem, quantidade;
            sscanf(item_id_str, "OP%d", &idItem);
            sscanf(item_qty_str, "OP%*dq%d", &quantidade);

            novoPedido->listaItens = adicionarItemPedido(novoPedido->listaItens, idItem, quantidade);
        }


        if (token) {
            if (sscanf(token, "%f", &(novoPedido->precoTotal)) != 1) {
                goto erro_parsing_linha;
            }
        } else {
            goto erro_parsing_linha;
        }


        #ifdef _WIN32
        token = strtok_s(NULL, ";", &context);
        #else
        token = strtok_r(NULL, ";", &context);
        #endif
        if (token) {
            strncpy(novoPedido->dataHoraPedido, token, sizeof(novoPedido->dataHoraPedido) - 1);
            novoPedido->dataHoraPedido[sizeof(novoPedido->dataHoraPedido) - 1] = '\0';
        } else {
            goto erro_parsing_linha;
        }

        // Adiciona o novo pedido à lista encadeada
        if (lista == NULL) {
            lista = novoPedido;
        } else {
            ultimo_pedido->prox = novoPedido;
        }
        ultimo_pedido = novoPedido;

        free(linha_copia);
        continue;

    erro_parsing_linha:

        free(novoPedido);
        free(linha_copia);

    }

    fclose(f);
    return lista;
}


// Função para exibir detalhes de um único pedido
void exibirDetalhesDoPedido(Pedido* pedido) {
    if (!pedido) {
        printf("Pedido invalido.\n");
        return;
    }
    limparTela();
    printf("\n==== DETALHES DO PEDIDO %s ====\n", pedido->idPedido);
    printf("ID Cliente: %s\n", pedido->idCliente);
    printf("ID Entrega: %s\n", pedido->idEntrega);
    printf("Data/Hora do Pedido: %s\n", pedido->dataHoraPedido);

    exibirItensDoPedidoAtual(pedido, cardapio, totalItensCardapio);

}

// Função para marcar/mover pedido como retirado
void marcarPedidoComoRetirado(char* idPedido) {
    Pedido* pedidos_pendentes = carregarPedidosDoArquivo(ARQUIVO_PEDIDOS);
    if (pedidos_pendentes == NULL) {
        printf("Nenhum pedido solicitado encontrado.\n");
        return;
    }

    Pedido* pedido_encontrado = NULL;
    Pedido* atual = pedidos_pendentes;
    while (atual != NULL) {
        if (strcmp(atual->idPedido, idPedido) == 0) {
            pedido_encontrado = atual;
            break;
        }
        atual = atual->prox;
    }

    if (pedido_encontrado == NULL) {
        printf("Pedido com ID '%s' nao encontrado entre os solicitados.\n", idPedido);
        liberarListaPedidos(pedidos_pendentes);
        return;
    }

    // Tenta salvar o pedido encontrado no arquivo de finalizados
    FILE *f_finalizados = fopen(ARQUIVO_FINALIZADOS, "a");
    if (!f_finalizados) {
        printf("Erro interno ao processar a retirada. Tente novamente.\n");

        liberarListaPedidos(pedidos_pendentes);
        return;
    }

    char data_hora_retirada[20];
    strncpy(data_hora_retirada, obterDataHoraAtual(), sizeof(data_hora_retirada) - 1);
    data_hora_retirada[sizeof(data_hora_retirada) - 1] = '\0';

    fprintf(f_finalizados, "%s;%s;%s;", pedido_encontrado->idPedido, pedido_encontrado->idCliente, pedido_encontrado->idEntrega);

    ItemPedido *item_atual_pedido = pedido_encontrado->listaItens;
    while (item_atual_pedido != NULL) {
        fprintf(f_finalizados, "OP%d;OP%dq%d;", item_atual_pedido->idItem, item_atual_pedido->idItem, item_atual_pedido->quantidade);
        item_atual_pedido = item_atual_pedido->prox;
    }
    fprintf(f_finalizados, "%.2f;%s;%s\n", pedido_encontrado->precoTotal, pedido_encontrado->dataHoraPedido, data_hora_retirada);

    fclose(f_finalizados);

    FILE *f_solicitados = fopen(ARQUIVO_PEDIDOS, "w");
    if (!f_solicitados) {
        printf("Erro interno ao finalizar o pedido. Contate o suporte.\n");

        liberarListaPedidos(pedidos_pendentes);
        return;
    }

    atual = pedidos_pendentes;
    while (atual != NULL) {
        if (strcmp(atual->idPedido, idPedido) != 0) {
            fprintf(f_solicitados, "%s;%s;%s;", atual->idPedido, atual->idCliente, atual->idEntrega);
            ItemPedido *tempItem = atual->listaItens;
            while (tempItem != NULL) {
                fprintf(f_solicitados, "OP%d;OP%dq%d;", tempItem->idItem, tempItem->idItem, tempItem->quantidade);
                tempItem = tempItem->prox;
            }
            fprintf(f_solicitados, "%.2f;%s\n", atual->precoTotal, atual->dataHoraPedido);
        }
        atual = atual->prox;
    }
    fclose(f_solicitados);

    liberarListaPedidos(pedidos_pendentes);
}


// Função de interface principal para retirada de pedidos
void retirar_pedido_ui() {
    limparTela();
    printf("==== RETIRAR PEDIDO ====\n");

    Pedido* pedidos_pendentes = carregarPedidosDoArquivo(ARQUIVO_PEDIDOS);

    if (pedidos_pendentes == NULL) {
        printf("\nNao ha pedidos solicitados no momento.\n");
        pausar(2);
        return;
    }

    char id_para_retirar[ID_SIZE];
    printf("\nDigite o ID do pedido para retirar (Ex: PED0001): ");
    fgets(id_para_retirar, sizeof(id_para_retirar), stdin);
    id_para_retirar[strcspn(id_para_retirar, "\n")] = 0;

    toUpperCase(id_para_retirar);

    Pedido* pedido_selecionado = NULL;
    Pedido* atual = pedidos_pendentes;
    while (atual != NULL) {

        if (strcmp(atual->idPedido, id_para_retirar) == 0) {
            pedido_selecionado = atual;
            break;
        }
        atual = atual->prox;
    }

    if (pedido_selecionado == NULL) {
        printf("Pedido com ID '%s' nao encontrado. Verifique o ID digitado.\n", id_para_retirar);
        liberarListaPedidos(pedidos_pendentes);
        pausar(3);
        return;
    }

    exibirDetalhesDoPedido(pedido_selecionado);

    char confirmacao;
    while (1) {
        printf("\nConfirmar retirada do pedido '%s'? (S/N): ", id_para_retirar);
        if (scanf(" %c", &confirmacao) != 1) {
            while (getchar() != '\n');
            printf("Entrada invalida. Digite 'S' para sim ou 'N' para nao.\n");
            continue;
        }
        while (getchar() != '\n');

        confirmacao = toupper((unsigned char)confirmacao);
        if (confirmacao == 'S' || confirmacao == 'N') {
            break;
        } else {
            printf("Entrada invalida. Digite 'S' para sim ou 'N' para nao.\n");
        }
    }

    if (confirmacao == 'S') {
        marcarPedidoComoRetirado(id_para_retirar);
        printf("\nProcesso de retirada finalizado com sucesso!\n");
    } else {
        printf("\nRetirada do pedido cancelada.\n");
    }

    liberarListaPedidos(pedidos_pendentes);
    pausar(3);
    limparTela();
}
