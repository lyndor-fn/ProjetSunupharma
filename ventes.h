#ifndef VENTES_H
#define VENTES_H

#include <time.h>
<<<<<<< HEAD
#include "produit.h"
=======
#include "produits.h"
>>>>>>> 844778b0052d396ad8e8cec2386a55730f395a34

typedef struct {
    char code[6];
    char nom[50];
    int quantiteVendue;
    float prixUnitaire;
    float prixTotal;
} ProduitVendu;


time_t convertirDate(const char* dateStr);
void menuGestionVentes();
void genererNumeroVente(char* numero);
void creerFacture(ProduitVendu ventes[], int taille, float total, char* login);
void mettreAJourStock(ProduitVendu ventes[], int nb);
void effectuerVente(char* login);
int calculerVentesDuJour(const char* date, float* totalVentes, int* nbMedicament);
void verifierStocksCritiques(Produit produits[], int nb, FILE* fRapport);
int chargerProduits(Produit produits[], int* nb);
#endif
