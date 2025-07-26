#include "fazer_pedido.h"
#include "menu.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


ItemCardapio cardapio[MAX_ITENS_CARDAPIO] = {
    {1, "Hamburguer", 15.50},
    {2, "Pizza", 25.00},
    {3, "Refrigerante", 7.00},
    {4, "Batata Frita", 10.00},
    {5, "Milkshake", 12.00},
    {6, "Cheeseburger", 18.90},
    {7, "Hot Dog", 14.50},
    {8, "Suco Natural", 9.00},
    {9, "Combo X-Burguer + Batata", 29.90},
    {10, "Cafe Expresso", 5.00}
};

int totalItensCardapio = MAX_ITENS_CARDAPIO;
Pedido* listaPedidos = NULL;

// FUNÇÕES AUXILIARES E DE GERENCIAMENTO DE PEDIDOS

// Funcao para exibir o cardapio
void exibirCardapio() {
    printf("\n=============== CARDAPIO ===============\n");
    for (int i = 0; i < totalItensCardapio; i++) {
        printf("[%2.d] %-24s - R$%-5.2f\n", cardapio[i].id, cardapio[i].nome, cardapio[i].preco);

    }
    printf("========================================\n");
}

// Funcao para ler os ultimos IDs do arquivo
void lerUltimosIDs(int *ultimoPedidoID, int *ultimoClienteID, int *ultimaEntregaID) {
    FILE *f = fopen(ARQUIVO_IDS, "r");
    if (!f) {
        *ultimoPedidoID = 0;
        *ultimoClienteID = 0;
        *ultimaEntregaID = 0;
        return;
    }
    if (fscanf(f, "ultimo_pedido_id=%d\n", ultimoPedidoID) != 1 ||
        fscanf(f, "ultimo_cliente_id=%d\n", ultimoClienteID) != 1 ||
        fscanf(f, "ultimo_entrega_id=%d\n", ultimaEntregaID) != 1) {
        *ultimoPedidoID = 0;
        *ultimoClienteID = 0;
        *ultimaEntregaID = 0;
    }
    fclose(f);
}

// Funcao para salvar os ultimos IDs no arquivo
void salvarUltimosIDs(int pedidoID, int clienteID, int entregaID) {
    FILE *f = fopen(ARQUIVO_IDS, "w");
    if (!f) {
        perror("Erro ao salvar IDs.");
        return;
    }
    fprintf(f, "ultimo_pedido_id=%d\n", pedidoID);
    fprintf(f, "ultimo_cliente_id=%d\n", clienteID);
    fprintf(f, "ultimo_entrega_id=%d\n", entregaID);
    fclose(f);
}

// Funcao para obter a data/hora formatada
char* obterDataHoraAtual() {
    time_t timer;
    struct tm *tm_info;
    static char buffer[20];

    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}

// Funcao para adicionar um item a lista encadeada de itens de um pedido
ItemPedido* adicionarItemPedido(ItemPedido* lista, int idItem, int quantidade) {
    ItemPedido* novoItem = (ItemPedido*)malloc(sizeof(ItemPedido));
    if (!novoItem) {
        perror("Erro ao alocar memoria para item do pedido.");
        return lista;
    }
    novoItem->idItem = idItem;
    novoItem->quantidade = quantidade;
    novoItem->prox = NULL;

    if (lista == NULL) {
        return novoItem;
    } else {
        ItemPedido *temp = lista;
        while (temp->prox != NULL) {
            temp = temp->prox;
        }
        temp->prox = novoItem;
        return lista;
    }
}


// Funcao para remover um item especifico do pedido
ItemPedido* removerItemPedido(ItemPedido* listaItens, int idItemParaRemover) {
    ItemPedido *atual = listaItens;
    ItemPedido *anterior = NULL;


    if (atual != NULL && atual->idItem == idItemParaRemover) {
        listaItens = atual->prox;
        free(atual);
        printf("Item removido com sucesso!\n");
        return listaItens;
    }


    while (atual != NULL && atual->idItem != idItemParaRemover) {
        anterior = atual;
        atual = atual->prox;
    }


    if (atual == NULL) {
        printf("Item com ID %d nao encontrado no pedido.\n", idItemParaRemover);
        return listaItens;
    }


    anterior->prox = atual->prox;
    free(atual);
    printf("Item removido com sucesso!\n");
    return listaItens;
}



void exibirItensDoPedidoAtual(Pedido* pedidoAtual, ItemCardapio cardapio[], int totalItensCardapio) {

    printf("\n--- Itens no Pedido Atual ---\n");
    if (pedidoAtual == NULL || pedidoAtual->listaItens == NULL) {
        printf("O pedido atual esta vazio. Adicione itens primeiro.\n");
        printf("------------------------------\n");
        return;
    }

    ItemPedido *itemAtual = pedidoAtual->listaItens;
    float totalAtual = 0.0;

    // Cabeçalho da tabela
    printf("---------------------------------------------------------------------------\n");
    printf("%-5s | %-30s | %-5s | %-12s | %-12s\n", "ID", "Item", "Qtd", "Preco Unit.", "Subtotal");
    printf("---------------------------------------------------------------------------\n");

    while (itemAtual) {
        char nomeItem[50] = "Item Desconhecido";
        float precoUnitario = 0.0;
        for (int i = 0; i < totalItensCardapio; i++) {
            if (cardapio[i].id == itemAtual->idItem) {
                strncpy(nomeItem, cardapio[i].nome, sizeof(nomeItem) - 1);
                nomeItem[sizeof(nomeItem) - 1] = '\0';
                precoUnitario = cardapio[i].preco;
                break;
            }
        }
        float precoItemTotal = precoUnitario * itemAtual->quantidade;


        printf("%-5d | %-30s | %-5d | R$%-10.2f | R$%-10.2f\n",
               itemAtual->idItem, nomeItem, itemAtual->quantidade, precoUnitario, precoItemTotal);

        totalAtual += precoItemTotal;
        itemAtual = itemAtual->prox;
    }
    printf("---------------------------------------------------------------------------\n");
    printf(" %62s R$%.2f\n","Preco Total Atual:" , totalAtual);
    printf("---------------------------------------------------------------------------\n");
}


// Funcao para calcular o preco total do pedido
float calcularPrecoTotal(Pedido *pedido) {
    float total = 0.0;
    ItemPedido *itemAtual = pedido->listaItens;
    while (itemAtual) {
        for (int i = 0; i < totalItensCardapio; i++) {
            if (cardapio[i].id == itemAtual->idItem) {
                total += cardapio[i].preco * itemAtual->quantidade;
                break;
            }
        }
        itemAtual = itemAtual->prox;
    }
    return total;
}

// FUNÇÃO CHAVE: SALVAR PEDIDO NO NOVO FORMATO OPx;OPxQy;
void salvarPedidoArquivo(Pedido *pedido) {
    FILE *f = fopen(ARQUIVO_PEDIDOS, "a");
    if (!f) {
        perror("Erro ao abrir arquivo para salvar pedido.");
        return;
    }

    // IDs do Pedido, Cliente, Entrega
    fprintf(f, "%s;%s;%s;", pedido->idPedido, pedido->idCliente, pedido->idEntrega);

    // Itens e Quantidades no formato OPx;OPxQy;
    ItemPedido *itemAtual = pedido->listaItens;
    while (itemAtual != NULL) {
        // Formato OPx;OPxQy;
        fprintf(f, "OP%d;OP%dq%d;", itemAtual->idItem, itemAtual->idItem, itemAtual->quantidade);
        itemAtual = itemAtual->prox;
    }


    fprintf(f, "%.2f;%s\n", pedido->precoTotal, pedido->dataHoraPedido);

    fclose(f);
}

void liberarListaPedidos(Pedido *lista) {
    Pedido *atual = lista;
    while (atual != NULL) {
        Pedido *proximo_pedido = atual->prox;

        ItemPedido *item_atual = atual->listaItens;
        while (item_atual != NULL) {
            ItemPedido *proximo_item = item_atual->prox;
            free(item_atual);
            item_atual = proximo_item;
        }

        free(atual);
        atual = proximo_pedido;
    }
}

void fazer_pedido_ui() {
    limparTela();

    int ultimoPedidoID, ultimoClienteID, ultimoEntregaID;
    lerUltimosIDs(&ultimoPedidoID, &ultimoClienteID, &ultimoEntregaID);

    // Cria o novo pedido e adiciona a lista global
    Pedido *novoPedido = (Pedido*)malloc(sizeof(Pedido));
    if (!novoPedido) {
        perror("Erro ao alocar memoria para novo pedido.");
        pausar(2);
        return;
    }
    novoPedido->listaItens = NULL;
    novoPedido->prox = listaPedidos;
    listaPedidos = novoPedido;

    // Atribuir IDs ao novo pedido
    snprintf(novoPedido->idPedido, ID_SIZE, "PED%04d", ++ultimoPedidoID);
    snprintf(novoPedido->idCliente, ID_SIZE, "CLI%04d", ++ultimoClienteID);
    snprintf(novoPedido->idEntrega, ID_SIZE, "ENT%04d", ++ultimoEntregaID);

    // Atribuir data e hora
    strncpy(novoPedido->dataHoraPedido, obterDataHoraAtual(), sizeof(novoPedido->dataHoraPedido) - 1);
    novoPedido->dataHoraPedido[sizeof(novoPedido->dataHoraPedido) - 1] = '\0';

    int idItemEscolhido, quantidade;
    char comandoChar[5];

    while (1) {
        limparTela();
        printf("==== Realizar Novo Pedido ====\n");
        printf("ID do Pedido: %s\n", novoPedido->idPedido);
        printf("ID do Cliente: %s\n", novoPedido->idCliente);
        printf("ID da Entrega: %s\n", novoPedido->idEntrega);
        printf("------------------------------\n");

        exibirCardapio();
        exibirItensDoPedidoAtual(novoPedido, cardapio, totalItensCardapio);

        printf("\n---------- Opcoes do Pedido ----------\n");
        printf("Digite o NUMERO do item para ADICIONAR.\n");
        printf("R -> Remover item do pedido atual\n");
        printf("F -> Finalizar pedido atual\n");
        printf("----------------------------------------\n");
        printf("Sua escolha: ");


        if (fgets(comandoChar, sizeof(comandoChar), stdin) == NULL) {
            printf("Erro ao ler a entrada. Tente novamente.\n");
            pausar(2);
            continue;
        }
        comandoChar[strcspn(comandoChar, "\n")] = 0;


        if (sscanf(comandoChar, "%d", &idItemEscolhido) == 1) {

            int item_encontrado_no_cardapio = 0;
            for (int i = 0; i < totalItensCardapio; i++) {
                if (cardapio[i].id == idItemEscolhido) {
                    item_encontrado_no_cardapio = 1;
                    break;
                }
            }

            if (item_encontrado_no_cardapio) {
                printf("Quantidade: ");

                quantidade = lerNumeroComLimite(1, 999);

                novoPedido->listaItens = adicionarItemPedido(novoPedido->listaItens, idItemEscolhido, quantidade);
                char nome_do_item[50] = "Desconhecido";
                for (int i = 0; i < totalItensCardapio; i++) {
                    if (cardapio[i].id == idItemEscolhido) {
                        strcpy(nome_do_item, cardapio[i].nome);
                        break;
                    }
                }
                printf("\n '%s' x%d adicionado ao pedido!\n", nome_do_item, quantidade);
                pausar(1);
            } else {
                printf("\n Item com ID %d nao encontrado no cardapio! Tente novamente.\n", idItemEscolhido);
                pausar(1);
            }
        } else {

            char primeiraLetra = toupper((unsigned char)comandoChar[0]);

            if (primeiraLetra == 'F') {
                limparTela();
                if (novoPedido->listaItens == NULL) {
                    printf("\nSeu pedido esta vazio. Nao e possivel finalizar um pedido sem itens.\n");
                    char sair_confirm;
                    while (1) {
                        printf("\nDeseja sair do pedido atual (S) ou voltar para adicionar itens (N)? (S/N): ");
                        if (scanf(" %c", &sair_confirm) != 1) {
                            while (getchar() != '\n');
                            printf("Entrada invalida. Digite 'S' para sair ou 'N' para adicionar itens.\n");
                            continue;
                        }
                        while (getchar() != '\n');

                        sair_confirm = toupper((unsigned char)sair_confirm);
                        if (sair_confirm == 'S' || sair_confirm == 'N') {
                            break;
                        } else {
                            printf("Entrada invalida. Digite 'S' para sair ou 'N' para adicionar itens.\n");
                        }
                    }

                    if (sair_confirm == 'S') {
                        if (listaPedidos == novoPedido) {
                            listaPedidos = novoPedido->prox;
                        }
                        liberarListaPedidos(novoPedido);
                        novoPedido = NULL;
                        printf("\nPedido descartado. Retornando ao menu principal.\n");
                        pausar(2);
                        return;
                    } else {
                        printf("\nVoltando ao menu de adicao de itens...\n");
                        pausar(2);
                        continue;
                    }
                } else {
                    break;
                }
            } else if (primeiraLetra == 'R') {
                limparTela();
                if (novoPedido->listaItens == NULL) {
                    printf("\nO pedido esta vazio. Nao ha itens para remover.\n");
                    pausar(2);
                    continue;
                }

                printf("\n--- Itens Atuais no Pedido para Remocao ---\n");
                exibirItensDoPedidoAtual(novoPedido, cardapio, totalItensCardapio);

                printf("Digite o ID do item que deseja remover: ");
                idItemEscolhido = lerNumeroComLimite(1, MAX_ITENS_CARDAPIO);

                novoPedido->listaItens = removerItemPedido(novoPedido->listaItens, idItemEscolhido);
                printf("\nItem removido (se encontrado).\n");
                pausar(2);
                continue;
            } else {
                printf("Comando invalido! Por favor, digite o numero do item ou um comando valido (R/F).\n");
                pausar(2);
                continue;
            }
        }
    }

    // RESUMO FINAL DO PEDIDO
    limparTela();

    if (novoPedido->listaItens == NULL) {
        printf("\nErro logico: Pedido inesperadamente vazio ao tentar finalizar.\n");

        if (novoPedido != NULL) {
             if (listaPedidos == novoPedido) {
                 listaPedidos = novoPedido->prox;
             }
             free(novoPedido);
             novoPedido = NULL;
        }
        pausar(2);
        return;
    }

    printf("\n=== RESUMO DO PEDIDO ===\n");
    printf("Pedido: %s | Cliente: %s | Entrega: %s\n", novoPedido->idPedido, novoPedido->idCliente, novoPedido->idEntrega);
    printf("Itens:\n");

    exibirItensDoPedidoAtual(novoPedido, cardapio, totalItensCardapio);

    novoPedido->precoTotal = calcularPrecoTotal(novoPedido);
    printf("Total: R$%.2f\n", novoPedido->precoTotal);

    char confirmacao;
    while (1) {
        printf("\nConfirmar pedido? (S/N): ");

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
        salvarPedidoArquivo(novoPedido);
        salvarUltimosIDs(ultimoPedidoID, ultimoClienteID, ultimoEntregaID);
        printf("\nPedido finalizado e salvo!\n");
    } else {
        printf("\nPedido cancelado! Nenhum ID foi alterado.\n");
        if (listaPedidos == novoPedido) {
            listaPedidos = novoPedido->prox;
        }
        liberarListaPedidos(novoPedido);
        novoPedido = NULL;
    }
    pausar(3);
    limparTela();
}

