#include "dispositivo.h"
#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define ARQUIVO "dispositivos.txt"

// Função auxiliar para capturar entrada e remover o '\n'

void capturarEntrada(char *entrada, int tamanho) {
  fgets(entrada, tamanho, stdin);
  entrada[strcspn(entrada, "\n")] = '\0'; // Remove '\n' da string

  // Limpa buffer de entrada para evitar caracteres indesejados
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}


// Valida se a string não está vazia
int validarNaoVazio(const char *str) {
  return (str[0] != '\0' && strspn(str, " ") != strlen(str));
}

// Valida se a string contém apenas números
int validarNumeros(const char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (!isdigit(str[i])) {
      return 0;
    }
  }
  return 1;
}


// Valida se a string contém apenas letras
int validarLetras(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalpha(str[i]) && str[i] != ' ') { // Permite espaços entre palavras
            return 0;
        }
    }
    return 1;
}

// Valida se a entrada é um número válido
int validarNumeroFloat(const char *str) {
  int ponto = 0, i = 0;
  if (!validarNaoVazio((char *)str)) return 0; // Cast para char *
  while (str[i] != '\0') {
    if (!isdigit(str[i])) {
      if (str[i] == ',' && ponto == 0 && i > 0) { // Garante que não comece com '.'
        ponto++;
      } else {
        return 0;
      }
    }
    i++;
  }

  return (i > 0); // Garante que ao menos um número foi inserido
}


int validarIP(const char *ip) {
  int pontos = 0;
  char copia[20];
  char anterior = '\0';

  if (!validarNaoVazio(ip)) return 0; // Evita IPs vazios

  // Garante que o tamanho seja adequado
  if (strlen(ip) < 7 || strlen(ip) > 15) return 0;

  strncpy(copia, ip, sizeof(copia) - 1);
  copia[sizeof(copia) - 1] = '\0';

  // Verifica espaços e pontos consecutivos
  for (int i = 0; copia[i] != '\0'; i++) {
    if (copia[i] == ' ') return 0;  // IPs não podem conter espaços
    if (copia[i] == '.') {
      if (anterior == '.' || anterior == '\0') return 0; // Evita pontos consecutivos ou no início
      pontos++;
      if (pontos > 3) return 0; // Mais de 3 pontos é inválido
    }
    anterior = copia[i];
  }
  if (anterior == '.') return 0; // Evita ponto no final

  // Reseta o contador de pontos para a validação final
  pontos = 0;
  char *token = strtok(copia, ".");
  while (token) {
    if (!validarNumeros(token) || atoi(token) > 255) {
      return 0;
    }
    pontos++;
    token = strtok(NULL, ".");
  }

  return (pontos == 4);  // Deve ter exatamente 4 octetos (3 pontos)
}


int validarAtivo(const char *str) {
  // Garante que a entrada seja exatamente "1" ou "0"
  return (strcmp(str, "1") == 0 || strcmp(str, "0") == 0);
}



// Função para cadastrar dispositivo com validação
Dispositivo *cadastrarDispositivo(Dispositivo *lista, int *contador) {
  Dispositivo *novo = (Dispositivo *)malloc(sizeof(Dispositivo));
  if (!novo) {
    printf("Erro ao alocar memória!\n");
    return lista;
  }

  int temp_id = *contador + 1;
  novo->id = temp_id;
    
    limparTela();
    printf("\n================================================================\n");
    printf("|                        Cadastro de Dispositivo               |\n");
    printf("================================================================\n");
   
  do {
    limparTela();
    printf("\n================================================================\n");
    printf("|                        Cadastro de Dispositivo               |\n");
    printf("================================================================\n");
    printf("Nome do Dispositivo: ");
    capturarEntrada(novo->nome, sizeof(novo->nome));

    if (!validarNaoVazio(novo->nome)) {
        printf("\nEspaço Vázio inválido! Tente novamente.\n");
        sleep(2);
    }
  } while (!validarNaoVazio(novo->nome));

  do {
    limparTela();
    printf("\n================================================================\n");
    printf("|                        Cadastro de Dispositivo               |\n");
    printf("================================================================\n");
    printf("IP: ");
    capturarEntrada(novo->ip, sizeof(novo->ip));

    if (!validarIP(novo->ip)) {
        printf("\nIP inválido! Digite um formato correto (ex: 192.168.1.1)\n");
        sleep(2);
    }
  } while (!validarIP(novo->ip));


  
  do {
    limparTela();
    printf("\n================================================================\n");
    printf("|                        Cadastro de Dispositivo               |\n");
    printf("================================================================\n");
    printf("Tipo: ");
    capturarEntrada(novo->tipo, sizeof(novo->tipo));

    if (!validarNaoVazio(novo->tipo) || !validarLetras(novo->tipo)) {
        printf("\nTipo inválido! Digite apenas letras.\n");
        sleep(2);
    }
  } while (!validarNaoVazio(novo->tipo) || !validarLetras(novo->tipo));

 
  char buffer[50];
  do {
    limparTela();
    printf("\n================================================================\n");
    printf("|                        Cadastro de Dispositivo               |\n");
    printf("================================================================\n");
    printf("Consumo de banda (Mbps): ");
    capturarEntrada(buffer, sizeof(buffer));

    if (!validarNumeroFloat(buffer) || sscanf(buffer, "%f", &novo->consumo_banda) != 1 || novo->consumo_banda <= 0) {
        printf("Valor inválido! Insira um número positivo.\n");
        sleep(2);
    }
  } while (!validarNumeroFloat(buffer) || novo->consumo_banda <= 0);

  
  do {
    limparTela();
    printf("\n================================================================\n");
    printf("|                        Cadastro de Dispositivo               |\n");
    printf("================================================================\n");
    printf("Ativo (1 - Sim, 0 - Não): ");
    capturarEntrada(buffer, sizeof(buffer));

    if (!validarAtivo(buffer)) {
      printf("Entrada inválida! Digite apenas 1 para Sim ou 0 para Não.\n");
      sleep(2);
    }
  } while (!validarAtivo(buffer));

  novo->ativo = atoi(buffer); // Conversão segura para inteiro

  // Converte strings para maiúsculo
  for(int i = 0; novo->nome[i]; i++) novo->nome[i] = toupper(novo->nome[i]);
  for(int i = 0; novo->tipo[i]; i++) novo->tipo[i] = toupper(novo->tipo[i]);
  

  
  do {
  limparTela();
    // Mostra prévia do cadastro
    printf("\nREVISÃO DO CADASTRO:\n");
    printf("====================================\n");
    printf("ID: %d\n", novo->id);
    printf("NOME: %s\n", novo->nome);
    printf("IP: %s\n", novo->ip);
    printf("TIPO: %s\n", novo->tipo);
    printf("CONSUMO DE BANDA: %.2f Mbps\n", novo->consumo_banda);
    printf("STATUS: %s\n", novo->ativo ? "ATIVO" : "INATIVO");
    printf("====================================\n");
    printf("Deseja Salvar este Cadastro? (1 - Sim, 0 - Não): ");
    capturarEntrada(buffer, sizeof(buffer));

    if (!validarAtivo(buffer)) {
      printf("Entrada Inválida! Digite apenas 1 para Sim ou 0 para Não.\n");
      sleep(2);
      
      
    }
  } while (!validarAtivo(buffer));

  if (atoi(buffer) == 1) {
    (*contador)++;  // Só incrementa o contador quando salvar
    novo->prox = lista;
    salvarDispositivos(novo);
    printf("\nCadastro Salvo com Sucesso!\n");
    sleep(2);
    menu(contador);
    return novo;
   

  } else {
    free(novo);
    printf("\nCadastro Descartado.\n");
    sleep(2);
    menu(contador);
    return lista;
   
  }
}


void salvarDispositivos(Dispositivo *lista) {
  FILE *arquivo = fopen(ARQUIVO, "w");
  if (!arquivo) {
    printf("\nErro ao abrir o arquivo para escrita!\n");
    return;
  }

  Dispositivo *atual = lista;
  while (atual) {
    fprintf(arquivo, "%d;%s;%s;%s;%.2f;%d\n", atual->id, atual->nome, atual->ip, atual->tipo, atual->consumo_banda, atual->ativo);
    atual = atual->prox;
  }

  fclose(arquivo);
}

Dispositivo *carregarDispositivos(int *contador) {
  FILE *arquivo = fopen(ARQUIVO, "r");
  if (!arquivo) {
    printf("\nNenhum arquivo encontrado, iniciando lista vazia.\n");
    return NULL;
  }

  Dispositivo *lista = NULL;
  while (!feof(arquivo)) {
    Dispositivo *novo = (Dispositivo *)malloc(sizeof(Dispositivo));
    if (!novo)
      break;

    if (fscanf(arquivo, "%d;%49[^;];%15[^;];%19[^;];%f;%d\n", &novo->id, novo->nome, novo->ip, novo->tipo, &novo->consumo_banda, &novo->ativo) == 6) {
      novo->prox = lista;
      lista = novo;
      (*contador)++;
    } else {
      free(novo);
    }
  }

  fclose(arquivo);
  return lista;
}

void listarDispositivosAtivos(Dispositivo *lista) {
  if (!lista) {
    printf("\nNenhum dispositivo cadastrado!\n");
    return;
  }

  Dispositivo *atual = lista;
  printf("\nDispositivos Ativos:\n");
  while (atual) {
    if (atual->ativo) {
      printf("ID: %d, Nome: %s, IP: %s, Tipo: %s, Consumo: %.2f Mbps\n", atual->id, atual->nome, atual->ip, atual->tipo, atual->consumo_banda);
    }
    atual = atual->prox;
  }
}

Dispositivo *buscarPorIP(Dispositivo *lista, char *ip) {
  if (!lista) {
    printf("\nNenhum dispositivo cadastrado!\n");
    return NULL;
  }

  Dispositivo *atual = lista;
  while (atual) {
    if (strcmp(atual->ip, ip) == 0) {
      return atual;
    }
    atual = atual->prox;
  }

  return NULL; 
}


void liberarMemoria(Dispositivo *lista) {
  Dispositivo *aux;
  while (lista) {
    aux = lista;
    lista = lista->prox;
    free(aux);
  }
}
