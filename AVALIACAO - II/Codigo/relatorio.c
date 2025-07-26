#include "relatorio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern ItemCardapio cardapio[MAX_ITENS_CARDAPIO];
extern int totalItensCardapio;


extern void limparTela();
extern int obterOpcaoMenu();
extern void pausar(int segundos);
extern void toUpperCase(char *str);
extern void liberarListaPedidos(Pedido* head);


static Pedido* carregarPedidos(const char* nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");
    if (!f) {
        return NULL;
    }

    Pedido* head = NULL;
    Pedido* current_tail = NULL;
    char linha[512];

    // Loop para ler cada linha do arquivo
    while (fgets(linha, sizeof(linha), f) != NULL) {

        linha[strcspn(linha, "\n")] = 0;

        if (strlen(linha) == 0) {
            continue;
        }


        Pedido* novoPedido = (Pedido*)malloc(sizeof(Pedido));
        if (!novoPedido) {
            perror("Erro ao alocar memoria para novo pedido (relatorio)");
            liberarListaPedidos(head);
            fclose(f);
            return NULL;
        }
        memset(novoPedido, 0, sizeof(Pedido));
        novoPedido->listaItens = NULL;
        novoPedido->prox = NULL;


        char *linha_copia = strdup(linha);
        if (!linha_copia) {
            perror("Erro ao alocar memoria para linha_copia (relatorio)");
            free(novoPedido);
            liberarListaPedidos(head);
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


            ItemPedido* newItem = (ItemPedido*)malloc(sizeof(ItemPedido));
            if (newItem == NULL) {
                perror("Erro ao alocar memoria para ItemPedido (relatorio)");
                goto erro_parsing_linha;
            }
            newItem->idItem = idItem;
            newItem->quantidade = quantidade;
            newItem->prox = NULL;


            if (novoPedido->listaItens == NULL) {
                novoPedido->listaItens = newItem;
            } else {
                ItemPedido* current_item = novoPedido->listaItens;
                while (current_item->prox != NULL) {
                    current_item = current_item->prox;
                }
                current_item->prox = newItem;
            }

            #ifdef _WIN32
            token = strtok_s(NULL, ";", &context);
            #else
            token = strtok_r(NULL, ";", &context);
            #endif
        }

        // Lê o preço total do pedido
        if (token) {
            if (sscanf(token, "%f", &(novoPedido->precoTotal)) != 1) {
                goto erro_parsing_linha;
            }
        } else {
            goto erro_parsing_linha;
        }

        // Lê a data e hora do pedido
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

        if (novoPedido) {
            ItemPedido* current_item = novoPedido->listaItens;
            while(current_item != NULL) {
                ItemPedido* next_item = current_item->prox;
                free(current_item);
                current_item = next_item;
            }
            free(novoPedido);
        }
        free(linha_copia);
        printf("Aviso: Linha invalida encontrada e ignorada no arquivo %s: %s\n", nomeArquivo, linha);
    }

    fclose(f);
    return head;
}


static void processarListaPedidos(Pedido* head, int* contagem, float* valores, int maxIdParaArray) {
    Pedido* currentPedido = head;
    while (currentPedido != NULL) {
        ItemPedido* currentItem = currentPedido->listaItens;
        while (currentItem != NULL) {
            float precoUnitario = 0.0;

            for (int i = 0; i < totalItensCardapio; i++) {
                if (cardapio[i].id == currentItem->idItem) {
                    precoUnitario = cardapio[i].preco;
                    break;
                }
            }


            if (currentItem->idItem >= 0 && currentItem->idItem <= maxIdParaArray) {
                contagem[currentItem->idItem] += currentItem->quantidade;
                valores[currentItem->idItem] += (currentItem->quantidade * precoUnitario);
            } else {
                printf("AVISO: Item com ID %d fora do range de itens carregados ou invalido. Ignorado no relatorio.\n", currentItem->idItem);
            }
            currentItem = currentItem->prox;
        }
        currentPedido = currentPedido->prox;
    }
}


void gerar_relatorio_item_mais_vendido() {
    limparTela(); // Limpa a tela do console
    printf("==== Relatorio: Desempenho de Vendas por Item do Cardapio ====\n\n");

    Pedido* pedidosSolicitados = carregarPedidos(ARQUIVO_PEDIDOS_SOLICITADOS);
    Pedido* pedidosFinalizados = carregarPedidos(ARQUIVO_PEDIDOS_FINALIZADOS);

    if (pedidosSolicitados == NULL && pedidosFinalizados == NULL) {
        printf("Nenhum pedido encontrado nos arquivos de pedidos solicitados ou finalizados.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }


    if (totalItensCardapio == 0) {
        printf("Nenhum item encontrado no cardapio ou cardapio nao carregado. Nao e possivel gerar o relatorio.\n");
        liberarListaPedidos(pedidosSolicitados);
        liberarListaPedidos(pedidosFinalizados);
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }


    int maxIdExistente = -1;
    for (int i = 0; i < totalItensCardapio; i++) {
        if (cardapio[i].id > maxIdExistente) {
            maxIdExistente = cardapio[i].id;
        }
    }


    if (maxIdExistente < 0) {
        printf("Nenhum item valido com ID nao negativo encontrado no cardapio. Nao e possivel gerar o relatorio.\n");
        liberarListaPedidos(pedidosSolicitados);
        liberarListaPedidos(pedidosFinalizados);
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }


    int *contagemItens = (int*)calloc(maxIdExistente + 1, sizeof(int));
    float *valorTotalItensVendidos = (float*)calloc(maxIdExistente + 1, sizeof(float));

    if (contagemItens == NULL || valorTotalItensVendidos == NULL) {
        perror("Erro ao alocar memoria para contagem/valores de itens (relatorio)");
        free(contagemItens);
        free(valorTotalItensVendidos);
        liberarListaPedidos(pedidosSolicitados);
        liberarListaPedidos(pedidosFinalizados);
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }


    processarListaPedidos(pedidosSolicitados, contagemItens, valorTotalItensVendidos, maxIdExistente);
    processarListaPedidos(pedidosFinalizados, contagemItens, valorTotalItensVendidos, maxIdExistente);

    // --- Exibir a Tabela de Vendas por Item ---
    printf("----------------------------------------------------------------------------------------\n");
    printf("| %-4s | %-30s | %-10s | %-13s | %-15s |\n", "ID", "Nome do Item", "Unidades", "Preco Unit.", "Valor Total R$");
    printf("----------------------------------------------------------------------------------------\n");

    int maxContagem = 0;
    float totalGeralVendido = 0.0;

    for (int i = 0; i < totalItensCardapio; i++) {
        int idAtual = cardapio[i].id;


        if (idAtual >= 0 && idAtual <= maxIdExistente) {
            if (contagemItens[idAtual] > maxContagem) {
                maxContagem = contagemItens[idAtual];
            }
            totalGeralVendido += valorTotalItensVendidos[idAtual];
        } else {

            printf("AVISO: Item de cardapio com ID %d fora do range esperado. Ignorado na soma geral.\n", idAtual);
        }
    }

    for (int i = 0; i < totalItensCardapio; i++) {
        int idAtual = cardapio[i].id;
        char nomeAtual[MAX_NOME_ITEM];
        strncpy(nomeAtual, cardapio[i].nome, sizeof(nomeAtual) - 1);
        nomeAtual[sizeof(nomeAtual) - 1] = '\0';
        float precoUnitario = cardapio[i].preco;

        int unidadesVendidas = 0;
        float valorTotalDoItem = 0.0;

        if (idAtual >= 0 && idAtual <= maxIdExistente) {
            unidadesVendidas = contagemItens[idAtual];
            valorTotalDoItem = valorTotalItensVendidos[idAtual];
        }

        // Imprime a linha da tabela formatada
        printf("| %-4d | %-30s | %-10d | R$ %-10.2f | R$ %-12.2f |\n", idAtual, nomeAtual, unidadesVendidas, precoUnitario, valorTotalDoItem);
    }
    printf("----------------------------------------------------------------------------------------\n");
    // Imprime o total geral de vendas no rodapé da tabela
    printf("| %-66s | R$ %-12.2f |\n", "TOTAL GERAL DE VENDAS DE ITENS:", totalGeralVendido);
    printf("----------------------------------------------------------------------------------------\n");


    printf("\n======= RESUMO =======\n");
    if (maxContagem > 0) {
        printf("\nO LANCHE MAIS VENDIDO FOI (com %d unidades cada):\n", maxContagem);

        for (int i = 0; i < totalItensCardapio; i++) {
            int idAtual = cardapio[i].id;

            if (idAtual >= 0 && idAtual <= maxIdExistente && contagemItens[idAtual] == maxContagem) {
                char nomeItem[MAX_NOME_ITEM];
                strncpy(nomeItem, cardapio[i].nome, sizeof(nomeItem) - 1);
                nomeItem[sizeof(nomeItem) - 1] = '\0';
                printf("\n  ID: [%d] - %s\n", idAtual, nomeItem);
                printf("--------------------------------------------\n");
            }
        }
    } else {
        printf("Nao ha dados suficientes para determinar o item mais vendido (ou nenhum item foi vendido).\n");
    }


    free(contagemItens);
    free(valorTotalItensVendidos);
    liberarListaPedidos(pedidosSolicitados);
    liberarListaPedidos(pedidosFinalizados);

    printf("\nPressione Enter para continuar...");
    getchar();

}


void gerar_relatorio_total_vendas() {
    limparTela();
    printf("======= Relatorio: Total de Vendas =======\n");

    Pedido* pedidosSolicitados = carregarPedidos(ARQUIVO_PEDIDOS_SOLICITADOS);
    Pedido* pedidosFinalizados = carregarPedidos(ARQUIVO_PEDIDOS_FINALIZADOS);

    float totalSolicitadas = 0.0;
    float totalFinalizadas = 0.0;


    Pedido* currentPedido = pedidosSolicitados;
    while (currentPedido != NULL) {
        totalSolicitadas += currentPedido->precoTotal;
        currentPedido = currentPedido->prox;
    }


    currentPedido = pedidosFinalizados;
    while (currentPedido != NULL) {
        totalFinalizadas += currentPedido->precoTotal;
        currentPedido = currentPedido->prox;
    }

    printf("Total de Vendas Solicitadas: R$ %.2f\n", totalSolicitadas);
    printf("Total de Vendas Finalizadas: R$ %.2f\n", totalFinalizadas);
    printf("------------------------------------------\n");
    printf("Total Geral de Vendas:       R$ %.2f\n", totalSolicitadas + totalFinalizadas);

    liberarListaPedidos(pedidosSolicitados);
    liberarListaPedidos(pedidosFinalizados);

    printf("\nPressione Enter para continuar...");
    getchar();
}


void exibir_relatorios_ui() {
    int opcao;
    while (1) {
        limparTela();
        printf("\n================================================\n");
        printf("      		MENU DE RELATORIOS\n");
         printf("================================================\n");
        printf("\n1 - Item de Lanche Mais Escolhido\n");
        printf("2 - Total de Vendas (Solicitadas e Finalizadas)\n");
        printf("0 - Voltar ao Menu Principal\n");
        printf("\n================================================\n");

        opcao = obterOpcaoMenu();

        switch (opcao) {
            case 1:
                gerar_relatorio_item_mais_vendido();
                break;
            case 2:
                gerar_relatorio_total_vendas();
                break;
            case 0:
                printf("Retornando ao Menu Principal...\n");
                pausar(1);
                return;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                pausar(2);
                break;
        }
    }
}
