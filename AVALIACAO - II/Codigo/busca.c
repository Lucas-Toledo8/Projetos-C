#include "busca.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern ItemCardapio cardapio[MAX_ITENS_CARDAPIO];
extern int totalItensCardapio;



static ItemPedido* adicionarItemPedidoBusca(ItemPedido* head, int idItem, int quantidade) {
    ItemPedido* newItem = (ItemPedido*)malloc(sizeof(ItemPedido));
    if (newItem == NULL) {
        perror("Erro ao alocar memoria para ItemPedido (busca)");
        return head;
    }
    newItem->idItem = idItem;
    newItem->quantidade = quantidade;
    newItem->prox = NULL;

    if (head == NULL) {
        return newItem;
    } else {
        ItemPedido* current = head;
        while (current->prox != NULL) {
            current = current->prox;
        }
        current->prox = newItem;
        return head;
    }
}


static void exibirItensDoPedidoAtualBusca(Pedido* pedido, ItemCardapio cardapio[], int totalItensCardapio) {
    if (!pedido || !pedido->listaItens) {
        printf("-------------------------------------------\n");
        printf("|              Nenhum item neste pedido.                |\n");
        printf("-------------------------------------------\n");
        return;
    }

    printf("------------------------------------------------------\n");
    printf("| %-4s | %-25s | %-3s | %-9s |\n", "ID", "Nome do Item", "QT", "Preco R$");
    printf("------------------------------------------------------\n");

    ItemPedido* current_item = pedido->listaItens;
    while (current_item != NULL) {
        char nome_item[MAX_NOME_ITEM] = "Desconhecido";
        float preco_unitario = 0.0;

        // Encontrar o nome do item no cardápio global
        for (int i = 0; i < totalItensCardapio; i++) {
            if (cardapio[i].id == current_item->idItem) {
                strncpy(nome_item, cardapio[i].nome, sizeof(nome_item) - 1);
                nome_item[sizeof(nome_item) - 1] = '\0';
                preco_unitario = cardapio[i].preco;
                break;
            }
        }


        float preco_total_item = preco_unitario * current_item->quantidade;

        // Imprime a linha da tabela
        printf("| %-4d | %-25s | %3d | %9.2f |\n",
               current_item->idItem,
               nome_item,
               current_item->quantidade,
               preco_total_item);

        current_item = current_item->prox;
    }
    printf("------------------------------------------------------\n");
}


static void exibirDetalhesDoPedidoBusca(Pedido* pedido) {
    if (!pedido) {
        printf("Pedido invalido.\n");
        return;
    }

    printf("\n============== DETALHES DO PEDIDO %s =============\n", pedido->idPedido);
    printf("ID Cliente: %s\n", pedido->idCliente);
    printf("ID Entrega: %s\n", pedido->idEntrega);
    printf("Data/Hora do Pedido: %s\n", pedido->dataHoraPedido);

    exibirItensDoPedidoAtualBusca(pedido, cardapio, totalItensCardapio);
    printf("Preco Total: R$ %.2f\n", pedido->precoTotal);
    printf("======================================================\n");
}


static void liberarListaPedidosBusca(Pedido* head) {
    Pedido* current_pedido = head;
    while (current_pedido != NULL) {
        Pedido* next_pedido = current_pedido->prox;

        ItemPedido* current_item = current_pedido->listaItens;
        while (current_item != NULL) {
            ItemPedido* next_item = current_item->prox;
            free(current_item);
            current_item = next_item;
        }
        free(current_pedido);
        current_pedido = next_pedido;
    }
}


static Pedido* carregarPedidosParaBusca(const char* nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");
    if (!f) {
        return NULL;
    }

    Pedido* head = NULL;
    Pedido* current_tail = NULL;
    char linha[512];

    while (fgets(linha, sizeof(linha), f) != NULL) {
        linha[strcspn(linha, "\n")] = 0;

        if (strlen(linha) == 0) {
            continue;
        }

        Pedido* novoPedido = (Pedido*)malloc(sizeof(Pedido));
        if (!novoPedido) {
            perror("Erro ao alocar memoria para novo pedido na busca");
            liberarListaPedidosBusca(head);
            fclose(f);
            return NULL;
        }
        memset(novoPedido, 0, sizeof(Pedido));
        novoPedido->listaItens = NULL;
        novoPedido->prox = NULL;

        char *linha_copia = strdup(linha);
        if (!linha_copia) {
            perror("Erro ao alocar memoria para linha_copia na busca");
            free(novoPedido);
            liberarListaPedidosBusca(head);
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

        #ifdef _WIN32
        token = strtok_s(NULL, ";", &context);
        #else
        token = strtok_r(NULL, ";", &context);
        #endif

        while (token != NULL && strncmp(token, "OP", 2) == 0) {
            int idItem;
            char *item_id_str = token;

            #ifdef _WIN32
            char *item_qty_str = strtok_s(NULL, ";", &context);
            #else
            char *item_qty_str = strtok_r(NULL, ";", &context);
            #endif

            if (item_qty_str == NULL || strchr(item_qty_str, 'q') == NULL) {
                goto erro_parsing_linha;
            }

            int quantidade;
            sscanf(item_id_str, "OP%d", &idItem);
            sscanf(item_qty_str, "OP%*dq%d", &quantidade);


            novoPedido->listaItens = adicionarItemPedidoBusca(novoPedido->listaItens, idItem, quantidade);

            #ifdef _WIN32
            token = strtok_s(NULL, ";", &context);
            #else
            token = strtok_r(NULL, ";", &context);
            #endif
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

        if (head == NULL) {
            head = novoPedido;
            current_tail = novoPedido;
        } else {
            current_tail->prox = novoPedido;
            current_tail = novoPedido;
        }

        free(linha_copia);
        continue;

    erro_parsing_linha:
        free(novoPedido);
        free(linha_copia);
    }

    fclose(f);
    return head;
}

// --- Função Principal de Busca de Pedidos por Item ---
void buscar_pedidos_por_item_ui() {
    char continuar_pesquisando;

    do {
        limparTela();
        printf("==== BUSCAR PEDIDOS POR ITEM ====\n");

        char termoBusca[MAX_NOME_ITEM];
        printf("\nDigite o nome (ou parte do nome) do item que deseja buscar: ");
        fgets(termoBusca, sizeof(termoBusca), stdin);
        termoBusca[strcspn(termoBusca, "\n")] = 0;

        if (strlen(termoBusca) == 0) {
            printf("Termo de busca vazio. Cancelando a operacao.\n");
            pausar(2);
            continuar_pesquisando = 'N';
            continue;
        }

        Pedido* pedidos_solicitados = carregarPedidosParaBusca(ARQUIVO_PEDIDOS);
        if (pedidos_solicitados == NULL) {
            printf("\nNenhum pedido solicitado encontrado ou erro ao carregar os dados.\n");
            pausar(2);
            continuar_pesquisando = 'N';
            continue;
        }

        int encontrados = 0;
        printf("\n--- Resultados da Busca por '%s' ---\n", termoBusca);

        Pedido* atual_pedido = pedidos_solicitados;
        while (atual_pedido != NULL) {
            ItemPedido* item_do_pedido = atual_pedido->listaItens;
            int contem_item = 0;

            while (item_do_pedido != NULL) {
                char nomeItemCardapio[MAX_NOME_ITEM] = "";
                for (int i = 0; i < totalItensCardapio; i++) {
                    if (cardapio[i].id == item_do_pedido->idItem) {
                        strncpy(nomeItemCardapio, cardapio[i].nome, sizeof(nomeItemCardapio) - 1);
                        nomeItemCardapio[sizeof(nomeItemCardapio) - 1] = '\0';
                        break;
                    }
                }

                char nomeItemCardapioUpper[MAX_NOME_ITEM];
                strncpy(nomeItemCardapioUpper, nomeItemCardapio, sizeof(nomeItemCardapioUpper) - 1);
                nomeItemCardapioUpper[sizeof(nomeItemCardapioUpper) - 1] = '\0';
                toUpperCase(nomeItemCardapioUpper);

                char termoBuscaUpper[MAX_NOME_ITEM];
                strncpy(termoBuscaUpper, termoBusca, sizeof(termoBuscaUpper) - 1);
                termoBuscaUpper[sizeof(termoBuscaUpper) - 1] = '\0';
                toUpperCase(termoBuscaUpper);

                if (strstr(nomeItemCardapioUpper, termoBuscaUpper) != NULL) {
                    contem_item = 1;
                    break;
                }
                item_do_pedido = item_do_pedido->prox;
            }

            if (contem_item) {
                exibirDetalhesDoPedidoBusca(atual_pedido);
                encontrados++;
            }
            atual_pedido = atual_pedido->prox;
        }

        if (encontrados == 0) {
            printf("\nNenhum pedido encontrado contendo '%s'.\n", termoBusca);
        } else {
            printf("\nTotal de %d pedido(s) encontrado(s) contendo '%s'.\n", encontrados, termoBusca);
        }

        liberarListaPedidosBusca(pedidos_solicitados);


        do {
            printf("\n-----------------------------------\n");
            printf("Deseja realizar outra busca? (S/N): ");
            char input[10];
            if (fgets(input, sizeof(input), stdin) == NULL) {
                continuar_pesquisando = 'N';
                break;
            }
            continuar_pesquisando = toupper(input[0]);

            if (continuar_pesquisando != 'S' && continuar_pesquisando != 'N') {
                printf("Opcao invalida. Por favor, digite 'S' para sim ou 'N' para nao.\n");
            }
        } while (continuar_pesquisando != 'S' && continuar_pesquisando != 'N');


    } while (continuar_pesquisando == 'S');

    printf("\nRetornando ao menu principal...\n");
    pausar(2);
}
