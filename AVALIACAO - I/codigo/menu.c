#include "menu.h"
#include "busca.h"
#include "dispositivo.h"
#include "ordenacao_basic.h"
#include "ordenacao_sof.h"
#include "relatorio.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Função para limpar a tela, compatível com Windows e Linux/macOS
void limparTela() {
#ifdef _WIN32
  system("cls"); // Para Windows
#else
  system("clear"); // Para Linux/macOS
#endif
}

// Valida se a entrada contém apenas números, sem espaços e sem vazios
int validarEntradaNumerica(const char *str) {
  if (str[0] == '\0')
    return 0; // Impede entrada vazia

  for (int i = 0; str[i] != '\0'; i++) {
    if (!isdigit(str[i]))
      return 0; // Se não for número, retorna inválido
  }

  return 1; 
}


int lerNumeroComLimite(int min, int max) {
  char buffer[10];
  int numero;

  // Simula um Enter no buffer antes da entrada
  ungetc('\n', stdin);

  // Limpa qualquer conteúdo residual da entrada
  getchar();

  printf("Escolha uma opção (%d a %d): ", min, max);
  fgets(buffer, sizeof(buffer), stdin);
  buffer[strcspn(buffer, "\n")] = '\0';

  if (!validarEntradaNumerica(buffer)) {
    printf("\nOpção inválida! Tente novamente...\n");
    sleep(2);
    limparTela();
    return -1;
  }

  numero = atoi(buffer);
  if (numero < min || numero > max) {
    printf("\nDigite um número válido!.\n");
    sleep(2);
    limparTela();
    return -1;
  }

  return numero;
}

void menu(int *v) {
  Dispositivo *lista = carregarDispositivos(v);
  int opcao = -1;
  insertionSort(&lista, 1);

  while (opcao != 5) {
    limparTela();
    printf("=========================================\n");
    printf("|             MENU DE OPÇÕES            |\n");
    printf("=========================================\n");
    printf("|  [1] Cadastrar Dispositivo            |\n");
    printf("|  [2] Lista dos Dispositivos           |\n");
    printf("|  [3] Buscar                           |\n");
    printf("|  [4] Exibir Resumo Estatístico        |\n");
    printf("|  [5] Sair                             |\n");
    printf("=========================================\n");


    opcao = lerNumeroComLimite(1, 5);
    if (opcao == -1)
      continue;

    if (opcao >= 1 && opcao <= 5) {
      break;
    }
  }
  
  switch (opcao) {
    case 1:
      lista = cadastrarDispositivo(lista, v);
      salvarDispositivos(lista);
      break;
    case 2:
      listarDispositivosOpcoes(&lista, v);
      break;
    case 3:
      buscando(lista, v);
      break;
    case 4:
        buscarRelatorio(lista, v);
        break;

    case 5:
      insertionSort(&lista, 1);
      salvarDispositivos(lista);
      liberarMemoria(lista);
      printf("\nEncerrando...\n");
      break;
    }

  
}


void listarDispositivosOpcoes(Dispositivo **lista, int *v) {
  int status = -1, criterio = -1;
  int continuarnaOperacao = -1;
  
  while (status < 1 || status > 3){
    limparTela();
    printf("\n==========================================\n");
    printf("|         SELEÇÃO DE DISPOSITIVOS         |\n");
    printf("==========================================\n");
    printf("| [1] Somente dispositivos ativos         |\n");
    printf("| [2] Somente dispositivos inativos       | \n");
    printf("| [3] Todos os dispositivos               | \n");
    printf("==========================================\n");
 


    status = lerNumeroComLimite(1, 3);
    
  }
 

  if (*lista == NULL) {
    printf("\nNenhum dispositivo cadastrado!\n");
    return;
  }

  
  while (criterio < 1 || criterio > 3){
    limparTela();
    printf("\n============================================\n");
    printf("|          CRITÉRIO DE LISTAGEM            |\n");
    printf("============================================\n");
    printf("| [1] ID - Ordem Crescente                 |\n");
    printf("| [2] IP - Ordem Crescente                 |\n");
    printf("| [3] Consumo de Banda - Ordem Decrescente | \n");
    printf("============================================\n");

    criterio = lerNumeroComLimite(1, 3);
    
  }

  // Aplica a ordenação correta antes de exibir os dispositivos
  if (criterio == 1) {
    insertionSort(lista, criterio);
  } else if (criterio == 2) {
    DispositivoOrdenado *listaNumerica = copiarListaNumerica(*lista);
    quickSortNumerico(&listaNumerica);
    restaurarListaOriginal(lista, listaNumerica);
  } else if (criterio == 3) {
    mergeSortConsumo(lista);
  }

  // Exibe os dispositivos conforme o status e critério escolhido
  if (criterio == 1) {
    listarExibir_ID(lista, status);
  }
  if (criterio == 2) {
    listarExibir_IP(lista, status);
  }
  if (criterio == 3) {
    listarExibir_CB(lista, status);
  }


  while (continuarnaOperacao < 0 || continuarnaOperacao > 1){
    printf("\n------------------------\n");
    printf("|   Deseja Voltar?     |\n");
    printf("| [0] Menu Principal   | \n");
    printf("| [1] Menu Lista       |\n");
    printf("------------------------\n");
    
    continuarnaOperacao = lerNumeroComLimite(0, 1);
    
  }
  if (continuarnaOperacao == 1){listarDispositivosOpcoes(lista, v);}
  else if(continuarnaOperacao == 0){
    menu(v);
  }
  
}


void buscando(Dispositivo *lista, int *v) {
  int escolhaBusca = -1;
  int continuarnaOperacao_busca = -1;
  int resultadoBusca = -1;

  while (escolhaBusca < 1 || escolhaBusca > 2) {
    limparTela();
    printf("\n==========================================\n");
    printf("|              TIPO DE BUSCA             |\n");
    printf("==========================================\n");
    printf("|           [1] Buscar por IP            |\n");
    printf("|           [2] Buscar por Nome          |\n");
    printf("==========================================\n");

    escolhaBusca = lerNumeroComLimite(1, 2);
  }
  resultadoBusca = escolhaBusca;

  // Realiza a busca conforme a escolha do usuário
  if (resultadoBusca == 1) {
    char ipBusca[16];
    printf("\nDigite o IP a ser buscado: ");
    scanf("%s", ipBusca);
    buscaBinariaPorIP(&lista, ipBusca);
   
  } else if (resultadoBusca == 2) {
    char nomeBusca[50];
    printf("\nDigite a primeira letra ou nome completo: ");
    scanf("%s", nomeBusca);
    char upperCaseName[2];
    upperCaseName[0] = toupper(nomeBusca[0]);
    upperCaseName[1] = '\0';
    buscaSequencialPorNome(lista, upperCaseName); 
    
  }

  // Adiciona a opção para o usuário continuar ou voltar ao menu principal
  while (continuarnaOperacao_busca < 0 || continuarnaOperacao_busca > 1) {
    printf("\n------------------------\n");
    printf("|   Deseja Voltar?     |\n");
    printf("| [0] Menu Principal   | \n");
    printf("| [1] Menu Busca       |\n");
    printf("------------------------\n");

    continuarnaOperacao_busca = lerNumeroComLimite(0, 1);
  }

  // Se o usuário quiser voltar ao menu principal, retorna para `menu()`
  if (continuarnaOperacao_busca == 0) {
    menu(v);
  } else {
    buscando(lista, v); // Chama novamente a função de busca
  }
}


void buscarRelatorio(Dispositivo *lista, int *v) { 
    int opcaoRelatorio = -1;
    
    while (opcaoRelatorio < 1 || opcaoRelatorio > 3) {
        limparTela();
        printf("\n==========================================\n");
        printf("|               RELATÓRIOS               |\n");
        printf("==========================================\n");
        printf("| [1] Visão Geral da Rede                |\n"); 
        printf("| [2] Análise de Consumo de Banda        |\n"); 
        printf("| [3] Verificação de IPs Duplicados      |\n"); 
        printf("==========================================\n");

        opcaoRelatorio = lerNumeroComLimite(1, 3);
    }

    if (opcaoRelatorio == 1) {
        exibirResumoEstatistico(lista, v); 
    } else if (opcaoRelatorio == 2) {
        analisarConsumoRede(lista, v); 
    } else if (opcaoRelatorio == 3) {
        verificarIPsDuplicados(lista, v); 
    }
}



