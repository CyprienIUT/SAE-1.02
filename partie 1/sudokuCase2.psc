procédure ajouterCandidat(entF/sortF laCase : tCase2, entF val:entier) c'est
début
    laCase.candidats[val] == vrai;
    laCase.nbCandidats := laCase.nbCandidats + 1;
fin

procédure retirerCandidat(entF/sortF laCase : tCase2, entF val:entier) c'est
début
    laCase.candidats[val] == faux;
    laCase.nbCandidats := laCase.nbCandidats - 1;
fin

fonction estCandidat(entF laCase : tCase2, entF val : entier) délivre booleen c'est
début
    resultat : booléen;
    resultat := laCase.candidats[val];
    retourne resultat;
fin

fonction nbCandidats(entF laCase : tCase1) délivre entier c'est
début
    retourne laCase.nbCandidats;
fin