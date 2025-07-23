#ifndef RAPPORTS_H
#define RAPPORTS_H
#include "produit.h"

void genererRapportJournalier();
void verifierStocksCritiques(Produit produits[], int nb, FILE* fRapport);
void menuRapport();


#endif
