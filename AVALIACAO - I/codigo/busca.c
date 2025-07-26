#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "busca.h"
#include "menu.h"
#include <ctype.h>
#include "ordenacao_sof.h"

char *normalizarTextoBusca(const char *texto, int max_len) {
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



void buscaBinariaPorIP(Dispositivo **lista, const char *ip_busca) {
    if (*lista == NULL) {
        printf("\n❌ Lista vazia! Busca não pode ser realizada.\n");
        return;
    }

    // Aplica a ordenação pelo critério 2 (IP em ordem crescente)
    DispositivoOrdenado *listaNumerica = copiarListaNumerica(*lista);
    quickSortNumerico(&listaNumerica);
    restaurarListaOriginal(lista, listaNumerica);

    Dispositivo *inicio = *lista, *fim = NULL, *meio;
    int encontrou = 0;

    while (inicio != fim) {
        meio = inicio;
        Dispositivo *temp = inicio;

        while (temp != fim && temp->prox != fim) {
            temp = temp->prox->prox ? temp->prox->prox : temp->prox;
            meio = meio->prox;
        }

        if (strncmp(meio->ip, ip_busca, strlen(ip_busca)) == 0) {
            // Só exibe o cabeçalho se for encontrado algo
            if (!encontrou) {
                printf("\n🔍 Dispositivos encontrados por IP:\n");
                printf("===============================================================================================================\n");
                printf("| %-5s | %-30s | %-16s | %-20s | %-12s | %-8s |\n", "ID", "Dispositivo", "IP", "Tipo", "Consumo Banda", "Status");
                printf("===============================================================================================================\n");
            }
            encontrou = 1;

            char nome_norm[50], tipo_norm[50];
            strncpy(nome_norm, normalizarTextoBusca(meio->nome, 30), 49);
            strncpy(tipo_norm, normalizarTextoBusca(meio->tipo, 20), 49);
            nome_norm[49] = '\0';
            tipo_norm[49] = '\0';

            printf("| %-5d | %-30s | %-16s | %-20s | %8.2f Mbps | %-8s |\n",
                   meio->id, nome_norm, meio->ip, tipo_norm,
                   meio->consumo_banda, meio->ativo ? "Ativo" : "Inativo");
            printf("---------------------------------------------------------------------------------------------------------------\n");

            Dispositivo *aux = meio->prox;
            while (aux != NULL && strncmp(aux->ip, ip_busca, strlen(ip_busca)) == 0) {
                strncpy(nome_norm, normalizarTextoBusca(aux->nome, 30), 49);
                strncpy(tipo_norm, normalizarTextoBusca(aux->tipo, 20), 49);
                nome_norm[49] = '\0';
                tipo_norm[49] = '\0';

                printf("| %-5d | %-30s | %-16s | %-20s | %8.2f Mbps | %-8s |\n",
                       aux->id, nome_norm, aux->ip, tipo_norm,
                       aux->consumo_banda, aux->ativo ? "Ativo" : "Inativo");
                printf("---------------------------------------------------------------------------------------------------------------\n");

                aux = aux->prox;
            }
            break;
        } else if (strcmp(meio->ip, ip_busca) < 0) {
            inicio = meio->prox;
        } else {
            fim = meio;
        }
    }

    if (!encontrou) {
        printf("\n❌ Nenhum dispositivo encontrado com o IP '%s'.\n", ip_busca);
    }
    getchar();
}


void buscaSequencialPorNome(Dispositivo *lista, const char *nome_inicial) {
    int encontrou = 0;

    while (lista != NULL) {
        char nome_lista_norm[50];
        strncpy(nome_lista_norm, normalizarTextoBusca(lista->nome, 30), 49);
        nome_lista_norm[49] = '\0';

        if (strncmp(nome_lista_norm, nome_inicial, strlen(nome_inicial)) == 0) {
            // Exibe cabeçalho apenas se encontrar algo
            if (!encontrou) {
                printf("\n🔍 Dispositivos encontrados por Nome:\n");
                printf("===============================================================================================================\n");
                printf("| %-5s | %-30s | %-16s | %-20s | %-12s | %-8s |\n", "ID", "Dispositivo", "IP", "Tipo", "Consumo Banda", "Status");
                printf("===============================================================================================================\n");
            }
            encontrou = 1;

            char tipo_norm[50];
            strncpy(tipo_norm, normalizarTextoBusca(lista->tipo, 20), 49);
            tipo_norm[49] = '\0';

            printf("| %-5d | %-30s | %-16s | %-20s | %8.2f Mbps | %-8s |\n",
                   lista->id, nome_lista_norm, lista->ip, tipo_norm,
                   lista->consumo_banda, lista->ativo ? "Ativo" : "Inativo");
            printf("---------------------------------------------------------------------------------------------------------------\n");
        }
        lista = lista->prox;
    }

    if (!encontrou) {
        printf("\n❌ Nenhum dispositivo encontrado com '%s'.\n", nome_inicial);
    }
    getchar();
}



