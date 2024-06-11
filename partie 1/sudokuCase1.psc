procédure ajouterCandidat(entF/sortF laCase : tCase1, entF val:entier) c'est
début
    si (laCase.nbCandidats < TAILLE) alors
        laCase.nbCandidats++;
        laCase.candidats[nbCandidats] := val;
    finsi
fin

procédure retirerCandidat(entF/sortF laCase : tCase1, entF val:entier) c'est
début
    i : entier;
    estRetiré : booleen;
    i := 1;
    estRetiré := faux;
    tant que ((estRetiré == faux) && (i <= laCase.nbCandidats)) faire
        si (laCase.candidats[i] == val) alors
            pour j de i à TAILLE pas de 1 faire
                si ((j+1 <= TAILLE) && (laCase.candidats[j+1] != 0)) alors
                    laCase.candidats[j] := laCase.candidats[j+1];
                finsi
            finfaire
            estRetiré := vrai;
        sinon
            i++;
        finsi
    finfaire
    laCase.nbCandidats--;
fin

fonction estCandidat(entF laCase : tCase1, entF val : entier) délivre booleen c'est
début
    resultat : booleen;
    resultat := faux;
    i : entier;
    i := 1;

    tant que ((resultat == faux) && (i <= TAILLE)) faire
        si (laCase.candidats[i] == val) alors
            resultat := vrai;
        finsi
        i++;
    finfaire

    retourne resultat;
fin

fonction nbCandidats(entF laCase : tCase1) délivre entier c'est
début
    retourne laCase.nbCandidats;
fin