#include "menu.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>


//versao 1.8

int main() {



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
