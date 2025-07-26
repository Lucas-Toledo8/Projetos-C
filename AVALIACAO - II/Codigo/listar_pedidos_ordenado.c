#include "listar_pedidos_ordenado.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>


extern ItemCardapio cardapio[MAX_ITENS_CARDAPIO];
extern int totalItensCardapio;


static Pedido* carregarPedidosParaOrdenacao(const char* nomeArquivo) {
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
            perror("Erro ao alocar memoria para novo pedido na ordenacao");
            liberarListaPedidos(head);
            fclose(f);
            return NULL;
        }
        memset(novoPedido, 0, sizeof(Pedido));
        novoPedido->listaItens = NULL;
        novoPedido->prox = NULL;

        char *linha_copia = strdup(linha);
        if (!linha_copia) {
            perror("Erro ao alocar memoria para linha_copia na ordenacao");
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
                perror("Erro ao alocar memoria para ItemPedido (carregarPedidosParaOrdenacao)");
                goto erro_parsing_linha;
            }
            newItem->idItem = idItem;
            newItem->quantidade = quantidade;
            newItem->prox = NULL;

            if (novoPedido->listaItens == NULL) {
                novoPedido->listaItens = newItem;
            } else {
                ItemPedido* current = novoPedido->listaItens;
                while (current->prox != NULL) {
                    current = current->prox;
                }
                current->prox = newItem;
            }

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
    }

    fclose(f);
    return head;
}

// Funções auxiliares de exibição
static void exibirItensDoPedidoFormatado(Pedido* pedido, ItemCardapio cardapio[], int totalItensCardapio) {
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

        for (int i = 0; i < totalItensCardapio; i++) {
            if (cardapio[i].id == current_item->idItem) {
                strncpy(nome_item, cardapio[i].nome, sizeof(nome_item) - 1);
                nome_item[sizeof(nome_item) - 1] = '\0';
                preco_unitario = cardapio[i].preco;
                break;
            }
        }

        float preco_total_item = preco_unitario * current_item->quantidade;

        printf("| %-4d | %-25s | %-3d | %-9.2f |\n",
               current_item->idItem,
               nome_item,
               current_item->quantidade,
               preco_total_item);

        current_item = current_item->prox;
    }

    printf("------------------------------------------------------\n");
}

static void exibirDetalhesDoPedido(Pedido* pedido) {
    if (!pedido) {
        printf("Pedido invalido.\n");
        return;
    }
	printf("\n============== DETALHES DO PEDIDO %s =============\n", pedido->idPedido);
    printf("ID Cliente: %s\n", pedido->idCliente);
    printf("ID Entrega: %s\n", pedido->idEntrega);
    printf("Data/Hora do Pedido: %s\n", pedido->dataHoraPedido);

    exibirItensDoPedidoFormatado(pedido, cardapio, totalItensCardapio);
    printf("Preco Total: R$ %.2f\n", pedido->precoTotal);
    printf("======================================================\n");
}


static time_t converterDataHoraParaTimeT(const char* dataHoraStr) {
    struct tm tm_struct;
    memset(&tm_struct, 0, sizeof(struct tm));

    int year, month, day, hour, min, sec;
    if (sscanf(dataHoraStr, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec) == 6) {
        tm_struct.tm_year = year - 1900;
        tm_struct.tm_mon = month - 1;
        tm_struct.tm_mday = day;
        tm_struct.tm_hour = hour;
        tm_struct.tm_min = min;
        tm_struct.tm_sec = sec;
        tm_struct.tm_isdst = -1;
        return mktime(&tm_struct);
    }
    return (time_t)-1;
}

// --- Funções de Comparação (para o Merge Sort) ---
static int compararPedidosPorIDCrescente(const Pedido* a, const Pedido* b) {
    return strcmp(a->idPedido, b->idPedido);
}

static int compararPedidosPorIDDecrescente(const Pedido* a, const Pedido* b) {
    return strcmp(b->idPedido, a->idPedido);
}

static int compararPedidosPorDataHoraCrescente(const Pedido* a, const Pedido* b) {
    time_t timeA = converterDataHoraParaTimeT(a->dataHoraPedido);
    time_t timeB = converterDataHoraParaTimeT(b->dataHoraPedido);

    if (timeA == (time_t)-1 || timeB == (time_t)-1) {
        return strcmp(a->idPedido, b->idPedido);
    }

    if (timeA < timeB) return -1;
    if (timeA > timeB) return 1;
    return 0;
}

static int compararPedidosPorDataHoraDecrescente(const Pedido* a, const Pedido* b) {
    time_t timeA = converterDataHoraParaTimeT(a->dataHoraPedido);
    time_t timeB = converterDataHoraParaTimeT(b->dataHoraPedido);

    if (timeA == (time_t)-1 || timeB == (time_t)-1) {
        return strcmp(b->idPedido, a->idPedido);
    }

    if (timeA < timeB) return 1;
    if (timeA > timeB) return -1;
    return 0;
}

// --- Implementação do Algoritmo Merge Sort ---
static void merge(Pedido** arr, int l, int m, int r, int (*comparar)(const Pedido*, const Pedido*)) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    Pedido** L = (Pedido**)malloc(n1 * sizeof(Pedido*));
    Pedido** R = (Pedido**)malloc(n2 * sizeof(Pedido*));

    if (L == NULL || R == NULL) {
        perror("Erro ao alocar memoria para arrays temporarios no Merge Sort");
        if (L) free(L);
        if (R) free(R);
        return;
    }

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;

    while (i < n1 && j < n2) {
        if (comparar(L[i], R[j]) <= 0) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

static void mergeSortPedidos(Pedido** arr, int l, int r, int (*comparar)(const Pedido*, const Pedido*)) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSortPedidos(arr, l, m, comparar);
        mergeSortPedidos(arr, m + 1, r, comparar);
        merge(arr, l, m, r, comparar);
    }
}


// --- Função Principal para a UI de Listagem Ordenada ---
void exibir_pedidos_ordenados_ui() {
    int opcao_status;
    char continuar_geral = 'S';

    while (toupper(continuar_geral) == 'S') {
        limparTela();
        printf("==== LISTAR PEDIDOS ORDENADOS ====\n");
        printf("Selecione o tipo de pedido para listar:\n");
        printf("1. Pedidos Solicitados\n");
        printf("2. Pedidos Finalizados\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("-----------------------------------\n");
        printf("Escolha uma opcao: ");

        char input_status[10];
        if (fgets(input_status, sizeof(input_status), stdin) == NULL || sscanf(input_status, "%d", &opcao_status) != 1) {
            opcao_status = -1;
        }

        if (opcao_status == 0) {
            continuar_geral = 'N';
            break;
        }

        const char* nomeArquivo;
        if (opcao_status == 1) {
            nomeArquivo = ARQUIVO_PEDIDOS_SOLICITADOS;
            printf("\nCarregando Pedidos Solicitados...\n");
        } else if (opcao_status == 2) {
            nomeArquivo = ARQUIVO_PEDIDOS_FINALIZADOS;
            printf("\nCarregando Pedidos Finalizados...\n");
        } else {
            printf("Opcao invalida. Por favor, digite 1, 2 ou 0.\n");
            pausar(2);
            continue;
        }

        Pedido* pedidos_head = carregarPedidosParaOrdenacao(nomeArquivo);
        if (pedidos_head == NULL) {
            printf("\nNenhum pedido encontrado no arquivo '%s' ou erro ao carregar os dados.\n", nomeArquivo);
            pausar(2);

            do {
                printf("\n-----------------------------------\n");
                printf("Deseja tentar outro tipo de pedido ou voltar ao menu principal? (S/N): ");
                char input_continuar[10];
                if (fgets(input_continuar, sizeof(input_continuar), stdin) == NULL) {
                    continuar_geral = 'N';
                    break;
                }
                continuar_geral = toupper(input_continuar[0]);

                if (continuar_geral != 'S' && continuar_geral != 'N') {
                    printf("Opcao invalida. Por favor, digite 'S' para sim ou 'N' para nao.\n");
                }
            } while (continuar_geral != 'S' && continuar_geral != 'N');
            continue;
        }

        int num_pedidos = 0;
        Pedido* current = pedidos_head;
        while (current != NULL) {
            num_pedidos++;
            current = current->prox;
        }

        if (num_pedidos == 0) {
            printf("\nNenhum pedido encontrado para ordenar no arquivo '%s'.\n", nomeArquivo);
            liberarListaPedidos(pedidos_head);
            pausar(2);

            do {
                printf("\n-----------------------------------\n");
                printf("Deseja tentar outro tipo de pedido ou voltar ao menu principal? (S/N): ");
                char input_continuar[10];
                if (fgets(input_continuar, sizeof(input_continuar), stdin) == NULL) {
                    continuar_geral = 'N';
                    break;
                }
                continuar_geral = toupper(input_continuar[0]);

                if (continuar_geral != 'S' && continuar_geral != 'N') {
                    printf("Opcao invalida. Por favor, digite 'S' para sim ou 'N' para nao.\n");
                }
            } while (continuar_geral != 'S' && continuar_geral != 'N');
            continue;
        }

        Pedido** array_pedidos = (Pedido**)malloc(num_pedidos * sizeof(Pedido*));
        if (array_pedidos == NULL) {
            perror("Erro ao alocar memoria para array de ponteiros de pedidos");
            liberarListaPedidos(pedidos_head);
            pausar(2);

            do {
                printf("\n-----------------------------------\n");
                printf("Deseja tentar outro tipo de pedido ou voltar ao menu principal? (S/N): ");
                char input_continuar[10];
                if (fgets(input_continuar, sizeof(input_continuar), stdin) == NULL) {
                    continuar_geral = 'N';
                    break;
                }
                continuar_geral = toupper(input_continuar[0]);

                if (continuar_geral != 'S' && continuar_geral != 'N') {
                    printf("Opcao invalida. Por favor, digite 'S' para sim ou 'N' para nao.\n");
                }
            } while (continuar_geral != 'S' && continuar_geral != 'N');
            continue;
        }

        current = pedidos_head;
        for (int i = 0; i < num_pedidos; i++) {
            array_pedidos[i] = current;
            current = current->prox;
        }

        int opcao_ordenacao;
        char continuar_ordenacao = 'S';

        while (toupper(continuar_ordenacao) == 'S') {
            limparTela();
            printf("==== PEDIDOS %s (%d encontrados) ====\n",
                   (opcao_status == 1 ? "SOLICITADOS" : "FINALIZADOS"), num_pedidos);
            printf("Selecione a opcao de ordenacao:\n");
            printf("1. Por ID (Crescente)\n");
            printf("2. Por ID (Decrescente)\n");
            printf("3. Por Data/Hora (Crescente)\n");
            printf("4. Por Data/Hora (Decrescente)\n");
            printf("0. Voltar para selecao de tipo de pedido / Menu Principal\n");
            printf("-----------------------------------\n");
            printf("Escolha uma opcao: ");

            char input_ordenacao[10];
            if (fgets(input_ordenacao, sizeof(input_ordenacao), stdin) == NULL || sscanf(input_ordenacao, "%d", &opcao_ordenacao) != 1) {
                opcao_ordenacao = -1;
            }

            if (opcao_ordenacao == 0) {
                continuar_ordenacao = 'N';
                break;
            }

            if (opcao_ordenacao < 1 || opcao_ordenacao > 4) {
                printf("Opcao invalida. Por favor, digite um numero entre 0 e 4.\n");
                pausar(2);
                continue;
            }

            // Chamada para o Merge Sort
            switch (opcao_ordenacao) {
                case 1:
                    mergeSortPedidos(array_pedidos, 0, num_pedidos - 1, compararPedidosPorIDCrescente);
                    printf("\nPedidos ordenados por ID (Crescente):\n");
                    break;
                case 2:
                    mergeSortPedidos(array_pedidos, 0, num_pedidos - 1, compararPedidosPorIDDecrescente);
                    printf("\nPedidos ordenados por ID (Decrescente):\n");
                    break;
                case 3:
                    mergeSortPedidos(array_pedidos, 0, num_pedidos - 1, compararPedidosPorDataHoraCrescente);
                    printf("\nPedidos ordenados por Data/Hora (Crescente):\n");
                    break;
                case 4:
                    mergeSortPedidos(array_pedidos, 0, num_pedidos - 1, compararPedidosPorDataHoraDecrescente);
                    printf("\nPedidos ordenados por Data/Hora (Decrescente):\n");
                    break;
            }

            // Exibir os pedidos ordenados
            for (int i = 0; i < num_pedidos; i++) {
                exibirDetalhesDoPedido(array_pedidos[i]);
            }

            // Pergunta se quer visualizar com outra ordenação ou voltar
            do {
                printf("\n-----------------------------------\n");
                printf("Deseja visualizar com outra ordenacao ou voltar a selecao de tipo de pedido? (S/N): ");
                char input_continuar[10];
                if (fgets(input_continuar, sizeof(input_continuar), stdin) == NULL) {
                    continuar_ordenacao = 'N';
                    break;
                }
                continuar_ordenacao = toupper(input_continuar[0]);

                if (continuar_ordenacao != 'S' && continuar_ordenacao != 'N') {
                    printf("Opcao invalida. Por favor, digite 'S' para sim ou 'N' para nao.\n");
                }
            } while (continuar_ordenacao != 'S' && continuar_ordenacao != 'N');
        }


        free(array_pedidos);
        liberarListaPedidos(pedidos_head);

        if (opcao_ordenacao == 0) {
            if (opcao_status == 0) {
                continuar_geral = 'N';
            }
        } else {

            do {
                printf("\n-----------------------------------\n");
                printf("Deseja listar outro tipo de pedido ou voltar ao menu principal? (S/N): ");
                char input_final_continuar[10];
                if (fgets(input_final_continuar, sizeof(input_final_continuar), stdin) == NULL) {
                    continuar_geral = 'N';
                    break;
                }
                continuar_geral = toupper(input_final_continuar[0]);

                if (continuar_geral != 'S' && continuar_geral != 'N') {
                    printf("Opcao invalida. Por favor, digite 'S' para sim ou 'N' para nao.\n");
                }
            } while (continuar_geral != 'S' && continuar_geral != 'N');
        }


    }

    printf("\nRetornando ao menu principal...\n");
    pausar(2);
}
