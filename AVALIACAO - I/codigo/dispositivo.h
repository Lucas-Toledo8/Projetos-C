#ifndef DISPOSITIVO_H
#define DISPOSITIVO_H



typedef struct Dispositivo {
  int id;
  char nome[50];
  char ip[16];
  char tipo[20];
  float consumo_banda;
  int ativo;
  struct Dispositivo *prox;
} Dispositivo;

Dispositivo *cadastrarDispositivo(Dispositivo *lista, int *contador);
void listarDispositivosAtivos(Dispositivo *lista);
Dispositivo *buscarPorIP(Dispositivo *lista, char *ip);
void liberarMemoria(Dispositivo *lista);
void salvarDispositivos(Dispositivo *lista);
Dispositivo *carregarDispositivos(int *contador);

void limparTela();

#endif
