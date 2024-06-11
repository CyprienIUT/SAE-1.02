#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
/*******************************************************************
*
* @brief programme de TRI A BULLES sur un tableau de 100 000 entiers
* initialis� avec des valeurs al�atoires
*
*******************************************************************/
#define TAILLE 100000

typedef int typeTab[TAILLE];

void triABulles(typeTab tab);
void remplirTableau(typeTab tab);

int main () {
    typeTab leTableau;

    srand(time(NULL));
    remplirTableau(leTableau);
    
    clock_t begin = clock();
    triABulles(leTableau);
    clock_t end = clock();
    double  tmpsCPU = (end - begin)*1.0 / CLOCKS_PER_SEC;
    printf( "Temps CPU = %.3f secondes\n",tmpsCPU);

    return EXIT_SUCCESS;
}

void triABulles(typeTab t){
    int i, j;
    int tmp;

    for(i=0;i<TAILLE-1;i++){
        for(j=0;j<TAILLE-i-1;j++){
            if (t[j]>t[j+1]){
                tmp = t[j];
                t[j] = t[j+1];
                t[j+1] = tmp;
            }
        }
    }
}

void remplirTableau(typeTab tab){
    int i;
    for (i=0 ; i<TAILLE ; i++){
        tab[i] = rand();
    }
}
