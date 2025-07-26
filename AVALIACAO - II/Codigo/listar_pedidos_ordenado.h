#ifndef LISTAR_PEDIDOS_ORDENADO_H
#define LISTAR_PEDIDOS_ORDENADO_H

#include "fazer_pedido.h"
#include "menu.h"
#include "busca.h"

// Enumeração para os tipos de ordenação (mantida)
typedef enum {
    ORDEM_ID_CRESCENTE,
    ORDEM_ID_DECRESCENTE,
    ORDEM_DATA_HORA_CRESCENTE,
    ORDEM_DATA_HORA_DECRESCENTE
} TipoOrdenacao;

// Enumeração para o tipo de status do pedido a ser listado
typedef enum {
    STATUS_SOLICITADOS,
    STATUS_FINALIZADOS
} TipoStatusPedido;


// Protótipo da função principal para a UI de listagem ordenada
void exibir_pedidos_ordenados_ui();

#endif // LISTAR_PEDIDOS_ORDENADO_H
