#ifndef LISTAR_PEDIDOS_ORDENADO_H
#define LISTAR_PEDIDOS_ORDENADO_H

#include "fazer_pedido.h"
#include "menu.h"
#include "busca.h"

// Enumera��o para os tipos de ordena��o (mantida)
typedef enum {
    ORDEM_ID_CRESCENTE,
    ORDEM_ID_DECRESCENTE,
    ORDEM_DATA_HORA_CRESCENTE,
    ORDEM_DATA_HORA_DECRESCENTE
} TipoOrdenacao;

// Enumera��o para o tipo de status do pedido a ser listado
typedef enum {
    STATUS_SOLICITADOS,
    STATUS_FINALIZADOS
} TipoStatusPedido;


// Prot�tipo da fun��o principal para a UI de listagem ordenada
void exibir_pedidos_ordenados_ui();

#endif // LISTAR_PEDIDOS_ORDENADO_H
