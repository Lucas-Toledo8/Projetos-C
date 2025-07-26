# Programa Gerenciamento de Sistema Interno de Monitoramento

A aplicação deve implementar os seguintes serviços, disponibilizados em telas aos usuário por meio de itens do menu de opções:

- Cadastro de dispositivos em um vetor de registros (máximo de 100 dispositivos).

        Listagem dos dispositivos ativos, com exibição de todos os campos.

- Busca por endereço IP utilizando busca sequencial e, posteriormente, binária (exigindo a ordenação prévia por IP).

- Ordenação dos dispositivos por:

        1. Endereço IP (alfabeticamente)

        2. Consumo de banda (ordem decrescente)

        3. Tipo de dispositivo (alfabética ou por categoria prioritária)

        4. Filtragem por tipo de dispositivo (mostrar todos os dispositivos do tipo "roteador" por exemplo).

- Detecção automática de anomalias, como:

- Dispositivos com consumo de banda acima de um valor-limite definido pelo usuário.

- Dispositivos duplicados (com o mesmo IP).

- Resumo estatístico da rede:

- Número total de dispositivos.

- Média de consumo de banda.

- Porcentagem de dispositivos ativos.
