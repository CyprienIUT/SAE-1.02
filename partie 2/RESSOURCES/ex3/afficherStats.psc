procédure afficherStats(entF nbCasesVidesInitial : entier, entF nbCasesVidesFinal : entier, entF nbCandidatsInitial : entier, entF nbCandidatsEliminés : entier) c'est
début
    nbCasesRemplis : entier;
    tauxRemplissage : réel;
    nbCandidatsElimines : réel;
    pourcentageElimines : réel;

    nbCasesRemplis := nbCasesVidesInitial - nbCasesVidesFinal;
    tauxRemplissage := nbCasesRemplis / nbCasesVidesInitial;
    nbCandidatsElimines := nbCandidatsInitial - nbCandidatsFinal;
    pourcentageElimines := (nbCandidatsElimines / nbCandidatsInitial) * 100; 

    écrireEcran("Nombre de cases remplis : ", nbCasesRemplis);
    écrireEcran("taux remplissage : ", tauxRemplissage);
    écrireEcran("Nombre de candidats éliminés : ", nbCandidatsElimines);
    écrireEcran("Pourcentage éliminés : ", pourcentageElimines);
fin