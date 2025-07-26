#include "menu.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

//versao 2.5

int main() {
  setlocale(LC_ALL, "pt_BR.utf8");
  setlocale(LC_ALL, "pt_BR.UTF-8");

  int *v = malloc(sizeof(int));
  if (!v) {
    printf("Erro ao alocar memoria!\n");
    return 1;
  }

  *v = 0;
  menu(v);

  free(v);

  return 0;
}