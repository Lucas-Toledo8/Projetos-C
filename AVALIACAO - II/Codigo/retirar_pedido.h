#ifndef RETIRAR_PEDIDO_H
#define RETIRAR_PEDIDO_H


#include "fazer_pedido.h"

// 1. Defina o caminho para a pasta de dados
#define CAMINHO_DADOS "Dados/"

#define ARQUIVO_FINALIZADOS CAMINHO_DADOS "pedidos_finalizados.txt"

// --- Protótipos das Funções do Módulo de Retirada ---

// Função principal da interface do usuário para retirar um pedido.
void retirar_pedido_ui();


// para uma lista encadeada na memória.
Pedido* carregarPedidosDoArquivo(const char* nomeArquivo);

void exibirDetalhesDoPedido(Pedido* pedido);


void marcarPedidoComoRetirado(char* idPedido);


#endif
