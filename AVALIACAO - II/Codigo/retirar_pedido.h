#ifndef RETIRAR_PEDIDO_H
#define RETIRAR_PEDIDO_H


#include "fazer_pedido.h"

// 1. Defina o caminho para a pasta de dados
#define CAMINHO_DADOS "Dados/"

#define ARQUIVO_FINALIZADOS CAMINHO_DADOS "pedidos_finalizados.txt"

// --- Prot�tipos das Fun��es do M�dulo de Retirada ---

// Fun��o principal da interface do usu�rio para retirar um pedido.
void retirar_pedido_ui();


// para uma lista encadeada na mem�ria.
Pedido* carregarPedidosDoArquivo(const char* nomeArquivo);

void exibirDetalhesDoPedido(Pedido* pedido);


void marcarPedidoComoRetirado(char* idPedido);


#endif
