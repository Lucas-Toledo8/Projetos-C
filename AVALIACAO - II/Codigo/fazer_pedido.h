#ifndef FAZER_PEDIDO_H
#define FAZER_PEDIDO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_ITENS_CARDAPIO 10

// 1. Defina o caminho para a pasta de dados
#define CAMINHO_DADOS "Dados/"

// 2. Altere os defines dos arquivos para incluir o CAMINHO_DADOS
#define ARQUIVO_IDS             CAMINHO_DADOS "ids.txt"
#define ARQUIVO_PEDIDOS         CAMINHO_DADOS "pedidos_solicitados.txt"

#define ID_SIZE 10
#define MAX_NOME_ITEM 50

// Estrutura do Cardápio (Vetor fixo)
typedef struct {
    int id;
    char nome[MAX_NOME_ITEM];
    float preco;
} ItemCardapio;

// Estrutura dos Itens do Pedido (Lista Encadeada dentro do Pedido)
typedef struct ItemPedido {
    int idItem;
    int quantidade;
    struct ItemPedido *prox;
} ItemPedido;

// Estrutura do Pedido (Lista Encadeada)
typedef struct Pedido {
    char idPedido[ID_SIZE];
    char idCliente[ID_SIZE];
    char idEntrega[ID_SIZE];
    float precoTotal;
    char dataHoraPedido[20];
    ItemPedido *listaItens;

    struct Pedido *prox;
} Pedido;

// Variáveis globais
extern ItemCardapio cardapio[MAX_ITENS_CARDAPIO];
extern int totalItensCardapio;
extern Pedido* listaPedidos;


// Protótipos de Funções

void exibirCardapio();
void fazer_pedido_ui();
void liberarListaPedidos(Pedido* lista);


Pedido* adicionarPedido(Pedido* lista, int *ultimoPedidoID, int *ultimoClienteID, int *ultimoEntregaID);

ItemPedido* adicionarItemPedido(ItemPedido* lista, int idItem, int quantidade);
void salvarPedidoArquivo(Pedido *pedido);

// Mantenha APENAS UMA ÚNICA DECLARAÇÃO para liberarListaPedidos
void liberarListaPedidos(Pedido* lista);

char* obterDataHoraAtual();
float calcularPrecoTotal(Pedido *pedido);

// Funções para gerenciar os IDs sequenciais
void lerUltimosIDs(int *pedidoID, int *clienteID, int *entregaID);
void salvarUltimosIDs(int pedidoID, int clienteID, int entregaID);

// Protótipo da nova função para remover itens
ItemPedido* removerItemPedido(ItemPedido* listaItens, int idItemParaRemover);
void exibirItensDoPedidoAtual(Pedido* pedidoAtual, ItemCardapio cardapio[], int totalItensCardapio);

// Protótipos das funções utili
void limparTela();
void pausar(int segundos);

#endif // FAZER_PEDIDO_H
