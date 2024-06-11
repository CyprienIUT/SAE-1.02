#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
/*******************************************************************
*
* @brief programme de TRI RAPIDE sur un tableau de 100 000 entiers
* initialis� avec des valeurs al�atoires
*
*******************************************************************/
#define TAILLE 100000

typedef int typeTab[TAILLE];

void triRapide(typeTab t, int debut, int fin);
int partitionner(typeTab t, int debut, int fin, int pivot);
void remplirTableau(typeTab tab);


int main () {
    typeTab leTableau;

    srand(time(NULL));
    remplirTableau(leTableau);
    time_t deb = time(NULL);
    triRapide(leTableau, 0, TAILLE-1);
    time_t fin = time(NULL);
    double duree = difftime(fin,deb);
    printf("%.2lfs\n",duree);

    return EXIT_SUCCESS;
}

int partitionner(typeTab t, int debut, int fin, int pivot){
    int tmp;
    int j;

    tmp = t[pivot];
    t[pivot] = t[fin];
    t[fin] = tmp;

    j = debut;
    for (int i=debut ; i<fin ; i++){
        if (t[i]<=t[fin]){
            tmp = t[i];
            t[i] = t[j];
            t[j] = tmp;
            j++;
        }
    }
    tmp = t[fin];
    t[fin] = t[j];
    t[j] = tmp;

    return j;
}

void triRapide(typeTab t, int debut, int fin){
    int pivot;
    if (debut < fin){
        pivot = (debut+fin) / 2;
        pivot = partitionner(t, debut, fin, pivot);
        triRapide(t, debut, pivot-1);
        triRapide(t, pivot+1, fin);
    }
}

void remplirTableau(typeTab tab){
    int i;
    for (i=0 ; i<TAILLE ; i++){
        tab[i] = rand();
    }
}
