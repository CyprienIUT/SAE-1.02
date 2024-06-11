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
typedef struct {
    int valeur;
    int candidats[TAILLE];
    int nbCandidats;
} tCase1;
typedef tCase1 tGrille[TAILLE][TAILLE];

//procédures et fonctions (descriptions de chacune d'elles après le programme principale)
void chargerGrille(tGrille,char[]);
void ajouterCandidat(tCase1*,int);
void retirerCandidat(tCase1*,int);
void retirerCandidatLigneColonneBloc(tGrille,int,int,int);
void retirerCandidatBlocPairesNues(tGrille,int,int,int,int,int,int);
bool estCandidat(tCase1,int);
void viderCandidatCase(tCase1*);
bool estSurLigneColonneBloc(tGrille,int,int,int);
void remplirCandidat(tGrille);
void afficheLigneSeparateur();
void afficherGrille(tGrille);
bool singletonNu(tGrille);
bool verifSingletonCache(tGrille,int,int,int);
void singletonCache(tGrille,bool*);
bool sontTableauxIdentiques(tCase1,tCase1);
bool paireNuesIndividuel(tGrille, int,int,int,int);
void pairesNues(tGrille,bool*);
void pairesCache(tGrille,bool*);
bool backtracking(tGrille,int);

// programme principal
int main(){
    tGrille g;
    char nomFichier[30];
    bool bt;
    bool progressionCases = true;
    bool progressionCandidats = true;
    chargerGrille(g,nomFichier);

    printf("  Grille initiale\n");
    afficherGrille(g);
    clock_t begin = clock();
    remplirCandidat(g);
    while((progressionCases == true) && (progressionCandidats == true)){
        progressionCases = singletonNu(g);
        progressionCandidats = progressionCases;
        singletonCache(g,&progressionCases);
        progressionCandidats = progressionCases;
        pairesNues(g,&progressionCandidats);
        pairesCache(g,&progressionCandidats);
    }
    bt = backtracking(g,0);
    if(bt){
        printf("blabla\n");
    }
    clock_t end = clock();
    double tmpsCPU = (end - begin)*1.0 / CLOCKS_PER_SEC;   
    afficherGrille(g);
    printf("  Resolution de la grille en %.6lf sec\n\n",tmpsCPU);
    /*if(bt == true){
        printf("  Grille finale\n");
        afficherGrille(g);
        printf("  Resolution de la grille en %.6lf sec\n\n",tmpsCPU);
    }
    else{
        printf("  La grille ne peut pas être remplis\n\n");
    }*/
    return EXIT_SUCCESS;
}

void chargerGrille(tGrille g, char nomFichier[]){
    FILE * f;
    printf("\n  Nom du fichier ? ");
    scanf("%s", nomFichier);
    f = fopen(nomFichier, "rb");
    while(f == NULL){
        printf("\n ERREUR sur le fichier %s\n", nomFichier);
        printf("Nom du fichier ? ");
        scanf("%s", nomFichier);
        f = fopen(nomFichier, "rb");
    }

    for(int i = 0; i < TAILLE; i++){
        for(int j = 0; j < TAILLE; j++){
            fread(&g[i][j].valeur, sizeof(int), 1, f);
        }   
    }
    fclose(f);
}

// La procédure ajoute le candidat en paramètre à la case en paramètre
void ajouterCandidat(tCase1 *laCase, int val){
    if((*laCase).nbCandidats < TAILLE){
        (*laCase).candidats[(*laCase).nbCandidats] = val;
        (*laCase).nbCandidats++;
    }
}

// La procédure retire le candidat en paramètre à la case en paramètre
void retirerCandidat(tCase1 *laCase, int val){
    int i = 0;
    int j;
    bool estRetire = false;
    while ((estRetire == false) && (i < (*laCase).nbCandidats)){
        if((*laCase).candidats[i] == val){
            j = i;
            while(j < (*laCase).nbCandidats){
                (*laCase).candidats[j] = (*laCase).candidats[j+1];
                j++;
            }
            (*laCase).nbCandidats--;
            estRetire = true;
            
        } else{
            i++;
        }
    }
}

// Vérifie la présence d'une valeur dans la ligne/colonne/bloc dans lequel il est
bool estSurLigneColonneBloc(tGrille g, int numLigne, int numColonne,int valeur){
    bool estPresent = false;
    int baseLig = (numLigne/N) * N;
    int baseCol = (numColonne/N) * N;
    int ligBloc;
    int colBloc;
    int i = 0;
    while((i < TAILLE) && (estPresent == false)){
        if(((g[i][numColonne].valeur == valeur) && (i != numLigne)) || ((g[numLigne][i].valeur == valeur) && (i != numColonne))){
            estPresent = true;
        }
        ligBloc = baseLig + i / N;
        colBloc = baseCol + i % N;
        if((g[ligBloc][colBloc].valeur == valeur) && ((colBloc != numColonne) || (ligBloc != numLigne))){
            estPresent = true;
        }
        i++;
    }
    return estPresent;
}

// La procédure retire le candidat en paramètre aux cases sur la même ligne, colonne et bloc que la case en coordonnée (numLigne,numColonne) en paramètres
void retirerCandidatLigneColonneBloc(tGrille g, int numLigne, int numColonne, int candidat){
    int baseLig = (numLigne/N) * N;
    int baseCol = (numColonne/N) * N;
    int ligBloc;
    int colBloc;
    for (int i = 0; i < TAILLE; i++){
        retirerCandidat(&g[numLigne][i],candidat);
        retirerCandidat(&g[i][numColonne],candidat);
        ligBloc = baseLig + i / N;
        colBloc = baseCol + i % N;
        retirerCandidat(&g[ligBloc][colBloc],candidat);
    }
}

// La procédure utilise les 2 candidats en commun aux 2 cases afin de retirer aux cases dans le même bloc que les 2 case en coordonnée (numLigne1,numColonne1 et numLigne2,numColonne2) en paramètres
// Le programme n'enlève pas les 2 candidats aux 2 cases de coordonnées en paramètres (méthode bloc paires Nues)
void retirerCandidatBlocPairesNues(tGrille g, int numLigne1, int numColonne1,int numLigne2, int numColonne2, int candidat1, int candidat2){
    int baseLig = (numLigne1/N) * N;
    int baseCol = (numColonne1/N) * N;
    for(int ligBloc = baseLig ; ligBloc < baseLig + N ; ligBloc++){
        for(int colBloc = baseCol ; colBloc < baseCol + N ; colBloc++){
            if(((numLigne1 != ligBloc ) || (numColonne1 != colBloc)) && ((numLigne2 != ligBloc) || (numColonne2 != colBloc))){
                retirerCandidat(&g[ligBloc][colBloc],candidat1);
                retirerCandidat(&g[ligBloc][colBloc],candidat2);
            }
        }
    }
}

// La fonction renvoie true si la valeur en paramètre est un candidat de la case en paramètre ou false sinon
bool estCandidat(tCase1 laCase, int valeur){
    bool trouve = false;
    int i = 0;
    while((i < laCase.nbCandidats) && (trouve == false)){
        if(laCase.candidats[i] == valeur){
            trouve = true;
            //printf("%d est candidat\n",laCase.candidats[i]);
        }
        i++;
    }
    return trouve;
}

// La procédure retirer tous les candidats contenu dans une case
void viderCandidatCase(tCase1* laCase){
    for (int i = 0; i < (*laCase).nbCandidats; i++){
        (*laCase).candidats[i] = 0;
    }
    (*laCase).nbCandidats = 0;
}

// La procédure place tous les candidats possibles dans chaque case vide de la grille (utilisée une seule fois au début du programme)
void remplirCandidat(tGrille g){
    for (int i = 0; i < TAILLE; i++){
        for(int j = 0; j < TAILLE; j++){
            /*for (int iCandidats = 0 ; iCandidats < TAILLE; iCandidats++){
                g[i][j].candidats[iCandidats] = 0;
            }*/
            g[i][j].nbCandidats = 0;
            for(int k = 1; k < TAILLE+1; k++){
                if((g[i][j].valeur == 0) && (estSurLigneColonneBloc(g,i,j,k) == false)){
                    ajouterCandidat((&g[i][j]),k);
                }
            }
            if(g[i][j].nbCandidats > 0){
                g[i][j].valeur = 0;
            }
        }
    }
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
            if(g[i][j].valeur == 0){
                printf("%2c ",POINT);
            }
            else{
                printf("%2d ",g[i][j].valeur);
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

// La fonction est la méthode de singleton nu appliquée à toute la grille
// La fonction place le seul candidat de la case en tant que valeur définitive et vide les candidats de la case
// Elle retourne true si la grille a été modifié et false sinon
bool singletonNu(tGrille g){
    bool progres = false;
    for (int i = 0 ; i < TAILLE ; i++){
        for(int j = 0 ; j < TAILLE ; j++){
            if(g[i][j].nbCandidats == 1){
                progres = true;
                g[i][j].nbCandidats = 0;
                g[i][j].valeur = g[i][j].candidats[0];
                retirerCandidatLigneColonneBloc(g,i,j,g[i][j].valeur);
            }
        }
    }
    return progres;
}

// Utilisée dans la procédure singletonCache, elle permet de traiter la méthode du singleton caché case par case
// Elle retourne true si la grille a été modifié et false sinon
bool singletonCacheIndividuel(tGrille g, int numLigne, int numColonne, int candidat){
    bool progres = false;
    int repetitionCandidatLigne = 0;
    int colonneSingletonCache = 0;
    int i = 0;
    int repetitionCandidatColonne = 0;
    int ligneSingletonCache = 0;
    int repetitionCandidatBloc = 0;
    int blocLigneSingletonCache = 0;
    int blocColonneSingletonCache = 0;
    int baseLig = (numLigne/N) * N;
    int baseCol = (numColonne/N) * N;
    int ligBloc;
    int colBloc;

    // Ligne et Colonne et Bloc
    while((i < TAILLE) && ((repetitionCandidatLigne < 2) || (repetitionCandidatColonne < 2) || (repetitionCandidatBloc < 2))){
        if(estCandidat(g[numLigne][i],candidat) == true){
            if(repetitionCandidatLigne == 0){
                colonneSingletonCache = i;
            }
            repetitionCandidatLigne++;
        }
        if(estCandidat(g[i][numColonne],candidat) == true){
            if(repetitionCandidatColonne == 0){
                ligneSingletonCache = i;
            }
            repetitionCandidatColonne++;
        }
        ligBloc = baseLig + i / N;
        colBloc = baseCol + i % N;
        if(estCandidat(g[ligBloc][colBloc],candidat) == true){
            if(repetitionCandidatBloc == 0){
                blocLigneSingletonCache = ligBloc;
                blocColonneSingletonCache = colBloc;
            }
            repetitionCandidatBloc++;
        }
        i++;
    }
    // Ligne
    if(repetitionCandidatLigne == 1){
        viderCandidatCase(&g[numLigne][colonneSingletonCache]);
        g[numLigne][colonneSingletonCache].valeur = candidat;
        retirerCandidatLigneColonneBloc(g,numLigne,colonneSingletonCache,candidat);
        progres = true;
    }
    // Colonne
    if(repetitionCandidatColonne == 1){
        viderCandidatCase(&g[ligneSingletonCache][numColonne]);
        g[ligneSingletonCache][numColonne].valeur = candidat;
        retirerCandidatLigneColonneBloc(g,ligneSingletonCache,numColonne,candidat);
        progres = true;
    }
    // Bloc
    if(repetitionCandidatBloc == 1){
        viderCandidatCase(&g[blocLigneSingletonCache][blocColonneSingletonCache]);
        g[blocLigneSingletonCache][blocColonneSingletonCache].valeur = candidat;
        retirerCandidatLigneColonneBloc(g,blocLigneSingletonCache,blocColonneSingletonCache,candidat);
        progres = true;
    }
    
    return progres;
}

// La fonction est la méthode de singleton caché appliquée à toute la grille
// La fonction place le seul candidat non présent dans les autres cases de la ligne/colonne/bloc en tant que valeur définitive à la case en coordonée (numLigne,numColonne) et vide les candidats de la case
void singletonCache(tGrille g, bool *progres){
    bool test = false;
    for (int i = 0 ; i < TAILLE ; i++){
        for(int j = 0 ; j < TAILLE ; j++){
            for(int k = 1 ; k < TAILLE+1 ; k++){
                test = singletonCacheIndividuel(g,i,j,k);
                if((*progres == false) && (test == true)){
                    *progres = true;
                }
            }
        }
    }
}

// La fonction compare 2 tableaux (de candidats dans notre cas)
// Elle retourne true s'ils sont identiques ou sinon false
bool sontTableauxIdentiques(tCase1 laCase1,tCase1 laCase2){
    bool estIdentique = true;
    int i = 1;
    if(laCase1.nbCandidats == laCase2.nbCandidats){
        while((i < laCase1.nbCandidats) && (estIdentique == true)){
            if(laCase1.candidats[i] != laCase2.candidats[i]){
                estIdentique = false;
            }
            else{
                i++;
            }
        }
    }
    else{
        estIdentique = false;
    }
    return estIdentique;
}

// Utilisée dans la procédure paireNues, la fonction permet de traiter la méthode des paires nues case par case
// Elle retourne true s'il y a des tableaux de candidats qui ont été modifiés, false sinon
bool paireNuesIndividuel(tGrille g, int numLigne, int numColonne,int baseBlocLigne, int baseBlocColonne){
    bool progres = false;
    int nbCasesVides = 1;
    for (int iBloc = baseBlocLigne ; iBloc < baseBlocLigne + N ; iBloc++){
        for(int jBloc = baseBlocColonne ; jBloc < baseBlocColonne + N ; jBloc++){
            if((g[iBloc][jBloc].valeur == 0) && ((iBloc != numLigne) || (jBloc != numColonne))){
                nbCasesVides++;
                if((nbCasesVides > 2) && (g[numLigne][numColonne].nbCandidats == 2) && (sontTableauxIdentiques(g[iBloc][jBloc],g[numLigne][numColonne]) == true)){
                    retirerCandidatBlocPairesNues(g,numLigne,numColonne,iBloc,jBloc,g[numLigne][numColonne].candidats[0],g[numLigne][numColonne].candidats[1]);
                    progres = true;
                }
            }
        }
    }
    return progres;
}

// La fonction est la méthode de singleton caché appliquée à toute la grille
// Si les 2 candidats présents dans 2 cases différentes dans le même bloc sont identiques, on les enlève des autres cases du bloc
void pairesNues(tGrille g, bool *progres){
    bool test = false;
    for(int numBloc = 0 ; numBloc < TAILLE ; numBloc++){
        int minBlocLigne = (numBloc / N) * N;
        int minBlocColonne = (numBloc % N) * N;
        for(int i = minBlocLigne ; i < (minBlocLigne + N) ; i++){
            for(int j = minBlocColonne ; j < (minBlocColonne + N) ; j++){
                if(g[i][j].valeur == 0){
                    test = paireNuesIndividuel(g,i,j,minBlocLigne,minBlocColonne);
                    if((*progres == false) && (test == true)){
                        *progres = test;
                    }
                }
            }
        }
    }
}

// Utilisée dans la procédure pairesCache, la fonction permet de traiter la méthode des paires cachées case par case
// Elle retourne true s'il y a des tableaux de candidats qui ont été modifiés, false sinon
bool pairesCacheIndividuel(tGrille g, int numLigne, int numColonne, int baseBlocLigne,int baseBlocColonne){
    bool progres = false;
    int candidat1 = 0;
    int ligneCandidat1;
    int colonneCandidat1;
    int candidat2 = 0;
    int iColonne = 0;
    int iLigne = 0;
    int iBloc;
    int jBloc;

    //verif Ligne
    colonneCandidat1 = numColonne;
    for(int i = 0 ; i < g[numLigne][numColonne].nbCandidats ; i++){
        while((iColonne < TAILLE) && (candidat1 != -1)){
            if(estCandidat(g[numLigne][iColonne],g[numLigne][numColonne].candidats[i]) == true){
                if(candidat1 == 0){
                    candidat1 = g[numLigne][numColonne].candidats[i];
                    colonneCandidat1 = iColonne;
                }   
                else{
                    candidat1 = -1;
                    colonneCandidat1 = 0;
                }
            }
            iColonne++;
        }
        if((candidat1 != 0) && (candidat1 != -1)){

            if(candidat2 == 0){
                candidat2 = candidat1;
                candidat1 = 0;
            }
            else{
                if((estCandidat(g[numLigne][colonneCandidat1],candidat1) == true) && (estCandidat(g[numLigne][colonneCandidat1],candidat2) == true)){
                    for(int iCandidatRetirer = 1; iCandidatRetirer < TAILLE + 1 ; iCandidatRetirer++){
                        if((estCandidat(g[numLigne][numColonne],iCandidatRetirer) == true) && (iCandidatRetirer != candidat1) && (iCandidatRetirer != candidat2)){
                            retirerCandidat(&g[numLigne][numColonne],iCandidatRetirer);
                            retirerCandidat(&g[numLigne][colonneCandidat1],iCandidatRetirer);
                        }
                    }
                    progres = true;
                }
                candidat1 = 0;
                candidat2 = 0;
            }
        }
        iColonne = 0;
    }
    // verif Colonne
    ligneCandidat1 = numLigne;
    for(int i = 0 ; i < g[numLigne][numColonne].nbCandidats ; i++){
        while((iColonne < TAILLE) && (candidat1 != -1)){
            if(estCandidat(g[ligneCandidat1][numColonne],g[numLigne][numColonne].candidats[i]) == true){
                if(candidat1 == 0){
                    candidat1 = g[numLigne][numColonne].candidats[i];
                    ligneCandidat1 = iLigne;
                }   
                else{
                    candidat1 = -1;
                    ligneCandidat1 = 0;
                }
            }
            iLigne++;
        }
        if((candidat1 != 0) && (candidat1 != -1)){
            if(candidat2 == 0){
                candidat2 = candidat1;
                candidat1 = 0;
            }
            else{
                if((estCandidat(g[ligneCandidat1][numColonne],candidat1) == true) && (estCandidat(g[ligneCandidat1][numColonne],candidat2) == true)){
                    for(int iCandidatRetirer = 1; iCandidatRetirer < TAILLE + 1 ; iCandidatRetirer++){
                        if((estCandidat(g[numLigne][numColonne],iCandidatRetirer) == true) && (iCandidatRetirer != candidat1) && (iCandidatRetirer != candidat2)){
                            retirerCandidat(&g[numLigne][numColonne],iCandidatRetirer);
                            retirerCandidat(&g[ligneCandidat1][numColonne],iCandidatRetirer);
                        }
                    }
                    progres = true;
                }
                candidat1 = 0;
                candidat2 = 0;
            }
        }
        iColonne = 0;
    }
    //verif Bloc
    candidat1 = 0;
    iBloc = baseBlocLigne;
    jBloc = baseBlocColonne;
    for(int i = 0 ; i < g[numLigne][numColonne].nbCandidats ; i++){
        while((iBloc < (baseBlocLigne + N)) && (candidat1 != -1)){
            while((jBloc < (baseBlocColonne + N)) && (candidat1 != -1)){
                if(estCandidat(g[iBloc][jBloc],g[numLigne][numColonne].candidats[i]) == true){
                    if(candidat1 == 0){
                        candidat1 = g[numLigne][numColonne].candidats[i];
                        ligneCandidat1 = iBloc;
                        colonneCandidat1 = jBloc;
                    }   
                    else{
                        candidat1 = -1;
                        ligneCandidat1 = 0;
                        colonneCandidat1 = 0;
                    }
                }
                jBloc++;
            }
            jBloc = baseBlocColonne;
            iBloc++;
        }
        iBloc = baseBlocLigne;
        if((candidat1 != 0) && (candidat1 != -1)){
            if(candidat2 == 0){
                candidat2 = candidat1;
                candidat1 = 0;
            }
            else{
                if((estCandidat(g[iBloc][jBloc],candidat1) == true) && (estCandidat(g[iBloc][jBloc],candidat2) == true)){
                    for(int iCandidatRetirer = 1; iCandidatRetirer < TAILLE + 1 ; iCandidatRetirer++){
                        if((estCandidat(g[numLigne][numColonne],iCandidatRetirer) == true) && (iCandidatRetirer != candidat1) && (iCandidatRetirer != candidat2)){
                            retirerCandidat(&g[numLigne][numColonne],iCandidatRetirer);
                            retirerCandidat(&g[iBloc][jBloc],iCandidatRetirer);
                        }
                    }
                    progres = true;
                }
                candidat1 = 0;
                candidat2 = 0;
            }
        }
    }
    return progres;
}

// La fonction est la méthode de singleton paires cachées appliquée à toute la grille
// Si parmi les candidats présents dans 2 cases différentes dans le même bloc/ligne/colonne ont 2 candidats identiques
// et qui ne sont pas dans les autres cases, on les enlève des autres cases du bloc/ligne/colonne
void pairesCache(tGrille g,bool *progres){
    bool test = false;
    for(int numBloc = 0 ; numBloc < TAILLE ; numBloc++){
        int minBlocLigne = (numBloc / N) * N;
        int minBlocColonne = (numBloc % N) * N;
        for(int i = minBlocLigne ; i < (minBlocLigne + N) ; i++){
            for(int j = minBlocColonne ; j < (minBlocColonne + N) ; j++){
                if((g[i][j].valeur == 0) && (g[i][j].nbCandidats > 1)){
                    test = pairesCacheIndividuel(g,i,j,minBlocLigne,minBlocColonne);
                    if((*progres == false) && (test == true)){
                        *progres = test;
                    }
                }
            }
        }
    }
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

        if(g[ligne][colonne].valeur != 0) {
            resultat = backtracking(g, numeroCase + 1);
        }
        else {
            for(int i = 1; i < TAILLE+1; i++) {
                if(estSurLigneColonneBloc(g,ligne,colonne,i) == false ) {
                    g[ligne][colonne].valeur = i;
                    if (backtracking(g, numeroCase + 1) == true){
                        resultat = true;
                    }
                    else {
                        g[ligne][colonne].valeur = 0;
                    }
                } 
            }
        }
    }
    return resultat;
}