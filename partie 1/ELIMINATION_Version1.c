#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// constantes
#define N 3
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
int chargerGrille(tGrille,bool*,char[]);
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
bool pairesCacheIndividuel(tGrille,int,int,int,int);
void pairesCache(tGrille,bool*);
int quantiteCasesVides(tGrille);
int quantiteCandidatsGrille(tGrille);
void afficheResultats(char[],int,int,int,int);

// programme principal
int main(){
    tGrille g;
    char nomFichier[30];
    bool fichierValide;
    bool progressionCases = true;
    bool progressionCandidats = true;
    /*int nbCasesVidesInitial = */chargerGrille(g,&fichierValide,nomFichier);
    //int nbCasesVidesCourant = nbCasesVidesInitial;
    
    remplirCandidat(g);
    //int nbCandidatsInitial = quantiteCandidatsGrille(g);
    //int nbCandidatsFinal;
    afficherGrille(g);

    clock_t begin = clock();

    while((progressionCases == true) && (progressionCandidats == true)){
        progressionCases = singletonNu(g);
        singletonCache(g,&progressionCases);
        progressionCandidats = progressionCases;
        pairesNues(g,&progressionCandidats);
        pairesCache(g,&progressionCandidats);
        //afficherGrille(g);
        //nbCasesVidesCourant = quantiteCasesVides(g);
    }

    clock_t end = clock();
    double tmpsCPU = (end - begin)*1.0 / CLOCKS_PER_SEC;
    printf("  Resolution de la grille en %.6lf sec\n\n",tmpsCPU);

    // utiliser pour voir les candidats de chaque case à la fin de la completion
    /*for(int i = 0 ; i < TAILLE ; i++){
        for(int j = 0 ; j < TAILLE; j++){
           printf("\n(%d,%d) : %d ",i,j,g[i][j].nbCandidats);
            for(int k = 0 ; k < g[i][j].nbCandidats ; k++){
                printf("[%d]",g[i][j].candidats[k]);
            }
        }
        printf("\n");
    }*/

    //nbCandidatsFinal = quantiteCandidatsGrille(g);
    //afficheResultats(nomFichier,nbCasesVidesInitial,nbCasesVidesCourant,nbCandidatsInitial,nbCandidatsFinal);

    return EXIT_SUCCESS;
}

// La fonction génère la grille à partir d'un nom de fichier s'il est valide, s'il ne l'est pas on redemande le nom du fichier
// La fonction renvoie le nombre de cases vides initiales
int chargerGrille(tGrille g, bool *fichierValide, char nomFichier[]){
    int nbCasesVides = 0;
    FILE * f;
    printf("Nom du fichier ? ");
    scanf("%s", nomFichier);
    f = fopen(nomFichier, "rb");
    while (f==NULL){
        printf("\n ERREUR sur le fichier %s\n", nomFichier);
        printf("Nom du fichier ? ");
        scanf("%s", nomFichier);
        f = fopen(nomFichier, "rb");
    } 

    for(int i = 0; i < TAILLE; i++){
        for(int j = 0; j < TAILLE; j++){
            fread(&g[i][j].valeur, sizeof(int), 1, f);
            if(g[i][j].valeur == 0){
                nbCasesVides++;                
            }
        }   
    }
    fclose(f);
    return nbCasesVides;
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

// La procédure retire le candidat en paramètre aux cases sur la même ligne, colonne et bloc que la case en coordonnée (numLigne,numColonne) en paramètres
void retirerCandidatLigneColonneBloc(tGrille g, int numLigne, int numColonne, int candidat){
    int baseLig = (numLigne/N) * N;
    int baseCol = (numColonne/N) * N;
    for (int i = 0; i < TAILLE; i++){
        retirerCandidat(&g[numLigne][i],candidat);
        retirerCandidat(&g[i][numColonne],candidat);
    }
    
    for(int ligBloc = baseLig ; ligBloc < baseLig + N ; ligBloc++){
        for(int colBloc = baseCol ; colBloc < baseCol + N ; colBloc++){
            retirerCandidat(&g[ligBloc][colBloc],candidat);
        }
    }
}

// La procédure utilise les 2 candidats en commun aux 2 cases afin de retirer aux cases dans le même bloc que les 2 case en coordonnée (numLigne1,numColonne1 et numLigne2,numColonne2) en paramètres
// La procédure n'enlève pas les 2 candidats aux 2 cases de coordonnées en paramètres (méthode Paires Nues)
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

// la fonction vérifie si la valeur en paramètre se trouve dans la ligne, colonne ou bloc dans laquelle se trouve la case en coordonées (numLigne, numColonne)
// retourne true si elle se trouve déjà sur la ligne/colonne/bloc ou renvoie false sinon
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

// La procédure place tous les candidats possibles dans chaque case vide de la grille (utilisée une seule fois au début du programme)
void remplirCandidat(tGrille g){
    for (int i = 0; i < TAILLE; i++){
        for(int j = 0; j < TAILLE; j++){
            for (int iCandidats = 0 ; iCandidats < TAILLE; iCandidats++){
                g[i][j].candidats[iCandidats] = 0;
            }
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

// utilisée dans la procédure afficherGrille, la procédure affiche les lignes séparatrices de blocs de la grille 
void afficheLigneSeparateur(){
    printf("   %c",PLUS);
    for(int i = 0 ; i < N ; i++){
        for(int j = 0 ; j < N ; j++){
            printf("%c%c%c",TIRET,TIRET,TIRET);
        }
        printf("%c",PLUS);
    }
    
    printf("\n");
}

// La procédure affiche la grille
void afficherGrille(tGrille g){
    printf("     1  2  3   4  5  6   7  8  9\n");
    afficheLigneSeparateur();
    for(int i = 0; i < TAILLE; i++){
        printf("%d  %c",i+1,BARRE);
        for(int j = 0; j < TAILLE ; j++){
            if(g[i][j].valeur == 0){
                printf(" %c ",POINT);
            }
            else{
                printf(" %d ",g[i][j].valeur);
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
    int iColonne = 0;
    int repetitionCandidatColonne = 0;
    int ligneSingletonCache = 0;
    int iLigne = 0;
    int repetitionCandidatBloc = 0;
    int blocLigneSingletonCache = 0;
    int blocColonneSingletonCache = 0;
    int baseLig = (numLigne/N) * N;
    int baseCol = (numColonne/N) * N;
    int ligBloc = baseLig;
    int colBloc = baseCol;

    // Ligne
    while((iColonne < TAILLE) && (repetitionCandidatLigne < 2)){
        if(estCandidat(g[numLigne][iColonne],candidat) == true){
            if(repetitionCandidatLigne == 0){
                colonneSingletonCache = iColonne;
            }
            repetitionCandidatLigne++;
        }
        iColonne++;
    }
    if(repetitionCandidatLigne == 1){
        viderCandidatCase(&g[numLigne][colonneSingletonCache]);
        g[numLigne][colonneSingletonCache].valeur = candidat;
        retirerCandidatLigneColonneBloc(g,numLigne,colonneSingletonCache,candidat);
        progres = true;
    }

    // Colonne
    while((iLigne < TAILLE) && (repetitionCandidatColonne < 2)){
        if(estCandidat(g[iLigne][numColonne],candidat) == true){
            if(repetitionCandidatColonne == 0){
                ligneSingletonCache = iLigne;
            }
            repetitionCandidatColonne++;
        }
        iLigne++;
    }
    if(repetitionCandidatColonne == 1){
        viderCandidatCase(&g[ligneSingletonCache][numColonne]);
        g[ligneSingletonCache][numColonne].valeur = candidat;
        retirerCandidatLigneColonneBloc(g,ligneSingletonCache,numColonne,candidat);
        progres = true;
    }

    // Bloc
    while((ligBloc < baseLig + N) && (repetitionCandidatBloc < 2)){
        while((colBloc < baseCol + N) && (repetitionCandidatBloc < 2)){
            if(estCandidat(g[ligBloc][colBloc],candidat) == true){
                if(repetitionCandidatBloc == 0){
                    blocLigneSingletonCache = ligBloc;
                    blocColonneSingletonCache = colBloc;
                }
                repetitionCandidatBloc++;
            }
            colBloc++;
        }
        ligBloc++;
        colBloc = baseCol;
    }
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
    int i = 0;
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

// La fonction retourne le nombre de cases de la grille qui ont leur valeur à 0
int quantiteCasesVides(tGrille g){
    int nbCasesVides = 0;

    for(int i = 0; i < TAILLE ; i++){
        for(int j = 0 ; j < TAILLE ; j++){
            if(g[i][j].valeur == 0)
                nbCasesVides++;
        }
    }
    return nbCasesVides;
}

// La fontion retourne le nombre de candidats total de la grille
int quantiteCandidatsGrille(tGrille g){
    int nbCandidats = 0;
    for(int i = 0 ; i < TAILLE ; i++){
        for (int j = 0 ; j < TAILLE ; j++){
            if(g[i][j].valeur == 0){
                nbCandidats = nbCandidats + g[i][j].nbCandidats;
            }
        }
    }
    return nbCandidats;
}

// La procédure affiche les résultat de complétion du tableau et d'éliminations de candidats
void afficheResultats(char nomFichier[], int nbCasesVidesInitial, int nbCasesVidesFinal, int nbCandidatsInitial, int nbCandidatsFinal){
    int nbCasesRemplis;
    float pourcentageRemplissage;
    int nbCandidatsElimination;
    float pourcentageElimination;

    nbCasesRemplis = nbCasesVidesInitial - nbCasesVidesFinal;
    pourcentageRemplissage = ((1.0 * nbCasesRemplis) / nbCasesVidesInitial) * 100;
    nbCandidatsElimination = nbCandidatsInitial - nbCandidatsFinal;
    pourcentageElimination = ((1.0 * nbCandidatsElimination) / nbCandidatsInitial) * 100; 

    printf("\t******   RESULTATS POUR %s   ******\n\n",nomFichier);
    printf("\tNombre de cases remplies = %d sur %d \t Taux de remplissage = %.3f %%\n\n",nbCasesRemplis,nbCasesVidesInitial,pourcentageRemplissage);
    printf("\tNombre de candidats elimines = %d \t Pourcentage d'elimination = %.3f %%\n\n",nbCandidatsElimination,pourcentageElimination);
}