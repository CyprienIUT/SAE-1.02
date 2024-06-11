#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// constantes
#define N 4
#define TAILLE (N*N)
#define POINT '.'
#define TIRET '-'
#define BARRE '|'
#define PLUS '+'

// structure des cases
typedef int tGrille[TAILLE][TAILLE];

//procédures et fonctions (descriptions de chacune d'elles après le programme principale)
void chargerGrille(tGrille,char[]);
bool estSurLigneColonneBloc(tGrille,int,int,int);
void afficheLigneSeparateur();
void afficherGrille(tGrille);
bool backtracking(tGrille,int);

// programme principal
int main(){
    tGrille g;
    char nomFichier[30];
    bool bt;
    chargerGrille(g,nomFichier);
    printf("  Grille initiale\n");
    afficherGrille(g);

    clock_t begin = clock();
    bt = backtracking(g,0);
    clock_t end = clock();
    double tmpsCPU = (end - begin)*1.0 / CLOCKS_PER_SEC;
    if(bt == true){
        printf("  Grille finale\n");
        afficherGrille(g);
        printf("  Resolution de la grille en %.6lf sec\n\n",tmpsCPU);
    }
    else{
        printf("  La grille ne peut pas être remplis\n\n");
    }
    
    return EXIT_SUCCESS;
}

void chargerGrille(tGrille g, char nomFichier[]){
    FILE * f;
    printf("\n  Nom du fichier ? ");
    scanf("%s", nomFichier);
    f = fopen(nomFichier, "rb");
    while(f == 
    NULL){
        printf("\n ERREUR sur le fichier %s\n", nomFichier);
        printf("Nom du fichier ? ");
        scanf("%s", nomFichier);
        f = fopen(nomFichier, "rb");
    }

    for(int i = 0; i < TAILLE; i++){
        for(int j = 0; j < TAILLE; j++){
            fread(&g[i][j], sizeof(int), 1, f);
        }   
    }
    fclose(f);
}

// Vérifie la présence d'une valeur dans la ligne/colonne/bloc dans lequel il est
bool estSurLigneColonneBloc(tGrille g, int numLigne, int numColonne,int valeur){
    bool estPresent = false;
    int baseLig = (numLigne/N) * N;
    int baseCol = (numColonne/N) * N;
    int ligBloc = baseLig;
    int colBloc = baseCol;
    int i = 0;
    while((i < TAILLE) && (estPresent == false)){
        if(((g[i][numColonne] == valeur) && (i != numLigne)) || ((g[numLigne][i] == valeur) && (i != numColonne))){
            estPresent = true;
        }
        i++;
    }
    while((ligBloc < baseLig + N) && (estPresent == false)){
        while((colBloc < baseCol + N) && (estPresent == false)){
            if((g[ligBloc][colBloc] == valeur) && ((colBloc != numColonne) || (ligBloc != numLigne))){
                estPresent = true;
            }
            colBloc++;
        }
        ligBloc++;
        colBloc = baseCol;
    }
    return estPresent;
}

//Affiche les lignes séparatrices horizontales de la grille
void afficheLigneSeparateur(){
    printf("  %c",PLUS);
    for(int i = 0 ; i < N ; i++){
        for(int j = 0 ; j < N ; j++){

            printf("%c%c%c",TIRET,TIRET,TIRET);
        }
        printf("%c",PLUS);
    }
    
    printf("\n");
}

// Affiche la grille
void afficherGrille(tGrille g){
    afficheLigneSeparateur();
    for(int i = 0; i < TAILLE; i++){
        printf("  %c",BARRE);
        for(int j = 0; j < TAILLE ; j++){
            if(g[i][j] == 0){
                printf("%2c ",POINT);
            }
            else{
                printf("%2d ",g[i][j]);
            }
            
            if((j+1)%N == 0){
                printf("%c",BARRE);
            }
        }
        printf("\n");
        if((i+1)%N == 0){
            afficheLigneSeparateur();
        }
    }
    printf("\n");
}

// méthode du backtracting
bool backtracking(tGrille g, int numeroCase){
    int ligne;
    int colonne;
    bool resultat = false;
    if(numeroCase == TAILLE * TAILLE) {
        resultat = true;
    }
    else {
        ligne = numeroCase / TAILLE;
        colonne = numeroCase % TAILLE;

        if(g[ligne][colonne] != 0) {
            resultat = backtracking(g, numeroCase + 1);
        }
        else {
            for(int i = 1; i < TAILLE+1; i++) {
                if(estSurLigneColonneBloc(g,ligne,colonne,i) == false ) {
                    g[ligne][colonne] = i;
                    if (backtracking(g, numeroCase + 1) == true){
                        resultat = true;
                    }
                    else {
                        g[ligne][colonne] = 0;
                    }
                } 
            }
        }
    }
    return resultat;
}