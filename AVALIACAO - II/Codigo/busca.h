#ifndef BUSCA_H
#define BUSCA_H


#include "fazer_pedido.h"
#include "menu.h"
// 1. Defina o caminho para a pasta de dados
#define CAMINHO_DADOS "Dados/"



#define ARQUIVO_PEDIDOS_SOLICITADOS CAMINHO_DADOS "pedidos_solicitados.txt"
#define ARQUIVO_PEDIDOS_FINALIZADOS CAMINHO_DADOS "pedidos_finalizados.txt"

// Protótipo da função principal de busca de pedidos por item
void buscar_pedidos_por_item_ui();

#endif // BUSCA_H
