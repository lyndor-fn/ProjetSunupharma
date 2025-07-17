#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Produit produits[MAX_PRODUCTS];
int nb_produits = 0;

void ajouterProduit() {
    if (nb_produits >= MAX_PRODUCTS) {
        printf("Stock plein, impossible d'ajouter un produit.\n");
        return;
    }

    Produit p;
    printf("Entrez le code du produit (5 caracteres): ");
    scanf("%s", p.code);
    printf("Entrez la designation: ");
    scanf(" %[^\n]", p.designation);
    printf("Entrez le prix: ");
    scanf("%f", &p.prix);
    printf("Entrez la quantite en stock: ");
    scanf("%d", &p.quantite);
    printf("Entrez la categorie: ");
    scanf(" %[^\n]", p.categorie);
    printf("Entrez la date de peremption (YYYY-MM-DD): ");
    scanf("%s", p.date_peremption);

    produits[nb_produits++] = p;
    printf("Produit ajout� avec succ�s.\n");
}

void modifierProduit() {
    char code[CODE_LENGTH];
    printf("Entrez le code du produit � modifier: ");
    scanf("%s", code);

    for (int i = 0; i < nb_produits; i++) {
        if (strcmp(produits[i].code, code) == 0) {
            printf("Modifier la designation (actuel: %s): ", produits[i].designation);
            scanf(" %[^\n]", produits[i].designation);
            printf("Modifier le prix (actuel: %.2f): ", produits[i].prix);
            scanf("%f", &produits[i].prix);
            printf("Modifier la quantite (actuel: %d): ", produits[i].quantite);
            scanf("%d", &produits[i].quantite);
            printf("Modifier la categorie (actuel: %s): ", produits[i].categorie);
            scanf(" %[^\n]", produits[i].categorie);
            printf("Modifier la date de peremption (actuel: %s): ", produits[i].date_peremption);
            scanf("%s", produits[i].date_peremption);
            printf("Produit modifi� avec succ�s.\n");
            return;
        }
    }
    printf("Produit non trouv�.\n");
}

void supprimerProduit() {
    char code[CODE_LENGTH];
    printf("Entrez le code du produit � supprimer: ");
    scanf("%s", code);

    for (int i = 0; i < nb_produits; i++) {
        if (strcmp(produits[i].code, code) == 0) {
            for (int j = i; j < nb_produits - 1; j++) {
                produits[j] = produits[j + 1];
            }
            nb_produits--;
            printf("Produit supprim� avec succ�s.\n");
            return;
        }
    }
    printf("Produit non trouv�.\n");
}

void afficherProduits() {
    printf("Liste des produits:\n");
    for (int i = 0; i < nb_produits; i++) {
        printf("Code: %s, D�signation: %s, Prix: %.2f, Quantit�: %d, Cat�gorie: %s, Date de p�remption: %s\n",
               produits[i].code, produits[i].designation, produits[i].prix,
               produits[i].quantite, produits[i].categorie, produits[i].date_peremption);
    }
}

void rechercherProduitParCode() {
    char code[CODE_LENGTH];
    printf("Entrez le code du produit � rechercher : ");
    scanf("%s", code);

    for (int i = 0; i < nb_produits; i++) {
        if (strcmp(produits[i].code, code) == 0) {
            printf("Produit trouv� : %s, Prix : %.2f, Stock : %d\n",
                   produits[i].designation, produits[i].prix, produits[i].quantite);
            return;
        }
    }
    printf("Produit non trouv�.\n");
}

void mettreAJourStock() {
    char code[CODE_LENGTH];
    int quantite;

    printf("Entrez le code du produit � mettre � jour : ");
    scanf("%s", code);
    printf("Entrez la quantit� � ajouter : ");
    scanf("%d", &quantite);

    for (int i = 0; i < nb_produits; i++) {
        if (strcmp(produits[i].code, code) == 0) {
            produits[i].quantite += quantite; // Met � jour la quantit�
            printf("Stock mis � jour pour le produit : %s. Nouvelle quantit� : %d\n",
                   produits[i].designation, produits[i].quantite);
            return;
        }
    }
    printf("Produit non trouv� pour mise � jour.\n");
}
void sauvegarderProduits() {
    FILE *fichier = fopen("PRODUCTS.dat", "wb"); // Ouvre le fichier en mode binaire
    if (fichier == NULL) {
        printf("Erreur � l'ouverture du fichier PRODUCTS.dat.\n");
        return;
    }

    size_t result = fwrite(produits, sizeof(Produit), nb_produits, fichier);
    if (result < nb_produits) {
        printf("Erreur lors de l'�criture dans le fichier.\n");
    }

    fclose(fichier);
    printf("Produits sauvegard�s avec succ�s.\n");
}



void chargerProduits() {
    FILE *fichier = fopen("PRODUCTS.dat", "rb"); // Ouvre le fichier en mode binaire pour lecture
    if (fichier == NULL) {
        printf("Erreur � l'ouverture du fichier PRODUCTS.dat.\n");
        return;
    }
    nb_produits = fread(produits, sizeof(Produit), MAX_PRODUCTS, fichier); // Lit les produits du fichier
    fclose(fichier);
    printf("Produits charg�s avec succ�s. Nombre de produits : %d\n", nb_produits);
}
