#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "menu.h"
#include "dispositivo.h"
#include "ordenacao_basic.h"
#include "ordenacao_sof.h"

char *normalizarTexto(const char *texto, int max_len) {
  static char buffer[50];
  int j = 0;

  for (int i = 0; texto[i] != '\0' && j < max_len - 1; i++) {
    unsigned char c = (unsigned char)texto[i];

    if (c == 195) { // Primeiro byte UTF-8
      i++;
      switch ((unsigned char)texto[i]) {
        // Minúsculas
        case 167: buffer[j++] = 'C'; break; // ç
        case 163: buffer[j++] = 'A'; break; // ã
        case 161: buffer[j++] = 'A'; break; // á
        case 162: buffer[j++] = 'A'; break; // â
        case 169: buffer[j++] = 'E'; break; // é
        case 170: buffer[j++] = 'E'; break; // ê
        case 173: buffer[j++] = 'I'; break; // í
        case 179: buffer[j++] = 'O'; break; // ó
        case 180: buffer[j++] = 'O'; break; // ô
        case 181: buffer[j++] = 'O'; break; // õ
        case 186: buffer[j++] = 'U'; break; // ú
        // Maiúsculas
        case 135: buffer[j++] = 'C'; break; // Ç
        case 131: buffer[j++] = 'A'; break; // Ã
        case 129: buffer[j++] = 'A'; break; // Á
        case 130: buffer[j++] = 'A'; break; // Â
        case 137: buffer[j++] = 'E'; break; // É
        case 138: buffer[j++] = 'E'; break; // Ê
        case 141: buffer[j++] = 'I'; break; // Í
        case 147: buffer[j++] = 'O'; break; // Ó
        case 148: buffer[j++] = 'O'; break; // Ô
        case 149: buffer[j++] = 'O'; break; // Õ
        case 154: buffer[j++] = 'U'; break; // Ú
        default: continue; // Ignora caracteres UTF-8 desconhecidos
      }
    } else if (isprint(c)) { // Verifica se é um caractere imprimível
      buffer[j++] = toupper(c);
    }
  }

  // Preenche o resto com espaços e finaliza a string
  while (j < max_len - 1) {
    buffer[j++] = ' ';
  }
  buffer[j] = '\0';

  return buffer;
}


void listarExibir_ID(Dispositivo **lista, int status) {
  Dispositivo *atual = *lista;

  printf("\nDispositivos listados por ID:\n");
  printf("===============================================================================================================\n");
  printf("| %-5s | %-30s | %-16s | %-20s | %-12s | %-8s |\n", "ID", "Dispositivo", "IP", "Tipo", "Consumo Banda", "Status");
  printf("===============================================================================================================\n");

  while (atual) {
    if ((status == 1 && atual->ativo) || (status == 2 && !atual->ativo) ||
        (status == 3)) {
      // Normaliza o nome e tipo antes da impressão
      char nome_norm[50], tipo_norm[50];
      strncpy(nome_norm, normalizarTexto(atual->nome, 30), 49);
      strncpy(tipo_norm, normalizarTexto(atual->tipo, 20), 49);
      nome_norm[49] = '\0';
      tipo_norm[49] = '\0';

      printf("| %-5d | %-30s | %-16s | %-20s | %8.2f Mbps | %-8s |\n", atual->id, nome_norm, atual->ip, tipo_norm, atual->consumo_banda, atual->ativo ? "Ativo" : "Inativo");
      printf("---------------------------------------------------------------------------------------------------------------\n");
    }
    atual = atual->prox;
  }
}

void listarExibir_IP(Dispositivo **lista, int status) {
  Dispositivo *atual = *lista;

  printf("\nDispositivos listados por IP:\n");
  printf("===============================================================================================================\n");
  printf("| %-5s | %-16s | %-30s | %-20s | %-12s | %-8s |\n", "ID", "IP", "Dispositivo", "Tipo", "Consumo Banda", "Status");
  printf("===============================================================================================================\n");

  while (atual) {
    if ((status == 1 && atual->ativo) || (status == 2 && !atual->ativo) || (status == 3)) {
      // Normaliza o nome e tipo antes da impressão
      char nome_norm[50], tipo_norm[50];
      strncpy(nome_norm, normalizarTexto(atual->nome, 30), 49);
      strncpy(tipo_norm, normalizarTexto(atual->tipo, 20), 49);
      nome_norm[49] = '\0';
      tipo_norm[49] = '\0';

      printf("| %-5d | %-16s | %-30s | %-20s | %8.2f Mbps | %-8s |\n", atual->id, atual->ip, nome_norm, tipo_norm, atual->consumo_banda, atual->ativo ? "Ativo" : "Inativo");
      printf("---------------------------------------------------------------------------------------------------------------\n");
    }
    atual = atual->prox;
  }
}

void listarExibir_CB(Dispositivo **lista, int status) {
  Dispositivo *atual = *lista;

  printf("\nDispositivos listados por Consumo Banda:\n");
  printf("===============================================================================================================\n");
  printf("| %-5s | %-12s | %-16s | %-30s | %-20s | %-8s |\n", "ID", "Consumo Banda", "IP", "Dispositivo", "Tipo", "Status");
  printf("===============================================================================================================\n");

  while (atual) {
    if ((status == 1 && atual->ativo) || (status == 2 && !atual->ativo) ||
        (status == 3)) {
      // Normaliza o nome e tipo antes da impressão
      char nome_norm[50], tipo_norm[50];
      strncpy(nome_norm, normalizarTexto(atual->nome, 30), 49);
      strncpy(tipo_norm, normalizarTexto(atual->tipo, 20), 49);
      nome_norm[49] = '\0';
      tipo_norm[49] = '\0';

      printf("| %-5d | %8.2f Mbps | %-16s | %-30s | %-20s | %-8s |\n", atual->id, atual->consumo_banda, atual->ip, nome_norm, tipo_norm, atual->ativo ? "Ativo" : "Inativo");
      printf("---------------------------------------------------------------------------------------------------------------\n");
    }
    atual = atual->prox;
  }
}



void exibirResumoEstatistico(Dispositivo *lista, int *v) {
  if (!lista) {
    printf("\nNenhum dispositivo cadastrado!\n");
    return;
  }

  int total = 0, ativos = 0, inativos = 0;
  float somaConsumoAtivos = 0;

  Dispositivo *atual = lista;
  while (atual) {
    total++;
    if (atual->ativo) {
      ativos++;
      somaConsumoAtivos += atual->consumo_banda;
    } else {
      inativos++;
    }
    atual = atual->prox;
  }

  float mediaConsumoAtivos = (ativos > 0) ? (somaConsumoAtivos / ativos) : 0;
  float percentualAtivos = (total > 0) ? ((ativos / (float)total) * 100) : 0;
  float percentualInativos = (total > 0) ? ((inativos / (float)total) * 100) : 0;

  printf("\nResumo da Rede:\n");
  printf("================================================\n");
  printf("| %-35s | %-6s |\n", "Analise", "Valor");
  printf("================================================\n");
  printf("| Total de dispositivos               | %-6d |\n", total);
  printf("| Dispositivos ativos                 | %-6d |\n", ativos);
  printf("| Dispositivos inativos               | %-6d |\n", inativos);
  printf("| Porcentagem de ativos               | %-3.2f%% |\n", percentualAtivos);
  printf("| Porcentagem de inativos             | %-3.2f%% |\n", percentualInativos);
  printf("| Total da Media Consumo Banda (Mbps) | %-6.2f |\n", mediaConsumoAtivos);
  printf("================================================\n");

  // Opção de retorno ao menu principal ou submenu de relatórios
  int escolhaRetorno = -1;
  while (escolhaRetorno < 0 || escolhaRetorno > 1) {
        printf("\n------------------------\n");
        printf("|   Deseja Voltar?     |\n");
        printf("| [0] Menu Principal   | \n");
        printf("| [1] Menu Relatório   |\n");
        printf("------------------------\n");

    escolhaRetorno = lerNumeroComLimite(0, 1);
  }

  if (escolhaRetorno == 0) {
    menu(v); 
  } else {
    buscarRelatorio(lista, v);
  }
}



float calcularMediaConsumo(Dispositivo *lista) {
  int total = 0;
  float somaConsumo = 0;

  Dispositivo *atual = lista;
  while (atual) {
    total++;
    somaConsumo += atual->consumo_banda;
    atual = atual->prox;
  }

  return (total > 0) ? (somaConsumo / total) : 0;
  
}  
  
void analisarConsumoRede(Dispositivo *lista, int *v) {
    if (!lista) {
        printf("Nenhum dispositivo cadastrado!\n");
        return;
    }

    float mediaUsuario;

    // Solicita entrada segura da média de consumo ao usuário
    do {
        printf("\nDigite a média de consumo desejada (Mbps): ");
        if (scanf("%f", &mediaUsuario) != 1 || mediaUsuario <= 0) {
            printf("\nEntrada inválida! Digite um valor numérico positivo.\n");
            while (getchar() != '\n'); // Limpa o buffer do teclado
        } else {
            break; 
        }
    } while (1);

    // Cria cópia da lista para análise
    Dispositivo *copiaLista = NULL;
    Dispositivo *atual = lista;
    while (atual) {
        Dispositivo *novo = (Dispositivo *)malloc(sizeof(Dispositivo));
        if (novo) {
            *novo = *atual;
            novo->prox = copiaLista;
            copiaLista = novo;
        }
        atual = atual->prox;
    }

    // Ordena cópia da lista por consumo de banda (decrescente)
    mergeSortConsumo(&copiaLista);

    printf("\nAnálise de Consumo de Banda (Ordenado por Consumo):\n");
    printf("================================================================================================\n");
    printf("| %-5s | %-30s | %-20s | %-28s |\n", "ID", "Nome", "Consumo Banda (Mbps)", "Status");
    printf("================================================================================================\n");

    Dispositivo *atualCopia = copiaLista;
    while (atualCopia) {
        char nome_norm[50];
        strncpy(nome_norm, normalizarTexto(atualCopia->nome, 30), 49);
        nome_norm[49] = '\0';

        const char *categoria;
        float consumo = atualCopia->consumo_banda;

        if (consumo > mediaUsuario * 2) {
            categoria = "\xF0\x9F\x94\xB4 -> Muito Acima da Média"; // 🔴
        } else if (consumo > mediaUsuario * 1.3) {
            categoria = "\xF0\x9F\x9F\xA0 -> Acima da Média"; // 🟠
        } else if (consumo >= mediaUsuario * 0.7 && consumo <= mediaUsuario * 1.3) {
            categoria = "\xF0\x9F\x9F\xA2 -> Dentro do Padrão"; // 🟢
        } else if (consumo < mediaUsuario * 0.5) {
            categoria = "\xF0\x9F\x94\xB5 -> Muito Abaixo da Média"; // 🔵
        } else {
            categoria = "\xF0\x9F\x9F\xA1 -> Abaixo da Média"; // 🟡
        }

        printf("| %-5d | %-30s | %-20.2f | %-32s |\n", atualCopia->id, nome_norm, consumo, categoria);
        atualCopia = atualCopia->prox;
    }

    getchar();
    printf("================================================================================================\n");

    liberarMemoria(copiaLista);

    // Adiciona opção de retorno correto
    int escolhaRetorno = -1;
    while (escolhaRetorno < 0 || escolhaRetorno > 1) {
        printf("\n------------------------\n");
        printf("|   Deseja Voltar?     |\n");
        printf("| [0] Menu Principal   | \n");
        printf("| [1] Menu Relatório   |\n");
        printf("------------------------\n");

        escolhaRetorno = lerNumeroComLimite(0, 1);
    }

    if (escolhaRetorno == 0) {
        menu(v);
        return;
    } else {
        buscarRelatorio(lista, v);
    }
}
  
  


void verificarIPsDuplicados(Dispositivo *lista, int *v) {
    if (!lista) {
        printf("\nNenhum dispositivo cadastrado!\n");
        return;
    }
    
    Dispositivo *atual, *comparado;
    int encontrouDuplicado = 0;

    printf("\n🔍 Verificando dispositivos com IPs iguais...\n");

    for (atual = lista; atual != NULL; atual = atual->prox) {
        comparado = atual->prox; 
        while (comparado != NULL) {
            if (strcmp(atual->ip, comparado->ip) == 0) {
                printf("\nIPs duplicados encontrados!\n");
                printf("========================================================================================\n");
                printf("| %-5s | %-20s | %-30s | %-20s |\n", "ID", "IPs", "Dispositivo", "Tipo");
                printf("========================================================================================\n");
                printf("| %5d | %-20s | %-30s | %-20s |\n", atual->id, atual->ip, atual->nome, atual->tipo);
                printf("| %5d | %-20s | %-30s | %-20s |\n", comparado->id, comparado->ip, comparado->nome, comparado->tipo);
                encontrouDuplicado = 1;
            }
            comparado = comparado->prox;
        }
    }

    
    if (!encontrouDuplicado) {
        printf("\n✅ Nenhum IP duplicado encontrado!\n");
    }
    

    int escolhaRetorno = -1;
    while (escolhaRetorno < 0 || escolhaRetorno > 1) {
        printf("\n------------------------\n");
        printf("|   Deseja Voltar?     |\n");
        printf("| [0] Menu Principal   | \n");
        printf("| [1] Menu Relatório   |\n");
        printf("------------------------\n");
    
        escolhaRetorno = lerNumeroComLimite(0, 1);
    }

    if (escolhaRetorno == 0) {
        menu(v);
        return;} 
    else {
        buscarRelatorio(lista, v); 
    }
    
}




