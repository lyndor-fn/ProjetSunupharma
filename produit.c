#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "produit.h"
#include "categories.h"

#define FICHIER_PRODUITS "PRODUCTS.dat"

Produit produits[MAX_PRODUCTS];
int nb_produits = 0;

// ========== UTILITAIRES ==========

void afficherSucces(const char* msg) {
    printf("\033[32m%s\033[0m\n", msg);
}

void afficherErreur(const char* msg) {
    printf("\033[31m%s\033[0m\n", msg);
}

int estCodeValide(const char* code) {
    if (strlen(code) != 5) return 0;
    for (int i = 0; i < 5; i++) {
        if (!isalnum(code[i])) return 0;
    }
    return 1;
}

int estDateValide(const char* date) {
    int y, m, d;
    if (sscanf(date, "%4d-%2d-%2d", &y, &m, &d) != 3) return 0;
    if (y < 1900 || m < 1 || m > 12 || d < 1 || d > 31) return 0;
    return 1;
}

int verifierCodeUnique(const char* code) {
    for (int i = 0; i < nb_produits; i++) {
        if (strcmp(produits[i].code, code) == 0) return 0;
    }
    return 1;
}

void sauvegarderProduitIndividuel(Produit p) {
    // Vérifie que le code est unique avant d’ajouter
    if (!verifierCodeUnique(p.code)) {
        afficherErreur("Code déjà utilisé. Produit non ajouté.");
        return;
    }

    FILE *f = fopen(FICHIER_PRODUITS, "ab");
    if (!f) {
        afficherErreur("Erreur ouverture fichier pour ajout.");
        return;
    }

    fwrite(&p, sizeof(Produit), 1, f);
    fclose(f);

    // Met à jour le tableau en mémoire
    produits[nb_produits++] = p;

    afficherSucces(" Produit ajoute avec succes.");
}



void chargerProduitsDepuisFichier() {
    FILE *f = fopen(FICHIER_PRODUITS, "rb");
    if (!f) {
        nb_produits = 0;
        return;
    }
    nb_produits = fread(produits, sizeof(Produit), MAX_PRODUCTS, f);
    fclose(f);
}

void sauvegarderProduits() {
    FILE *f = fopen(FICHIER_PRODUITS, "wb");
    if (!f) {
        afficherErreur("Erreur ouverture fichier pour ecriture.");
        return;
    }
    fwrite(produits, sizeof(Produit), nb_produits, f);
    fclose(f);
}


// ========== GESTION PRODUITS ==========

void ajouterEtSauvegarderProduit() {
    if (nb_produits >= MAX_PRODUCTS) {
        afficherErreur("Nombre maximum de produits atteint !");
        return;
    }

    Produit p;
    printf("\n=== AJOUT PRODUIT ===\n");

    do {
        printf("Code (5 caracteres alphanumeriques) : ");
        scanf("%s", p.code);
        if (!estCodeValide(p.code)) {
            afficherErreur("Code invalide !");
        } else if (!verifierCodeUnique(p.code)) {
            afficherErreur("Code deja utilise !");
        } else break;
    } while (1);

    printf("Designation : ");
    scanf(" %[^\n]", p.designation);

    do {
        printf("Prix : ");
        scanf("%f", &p.prix);
        if (p.prix < 0)
            afficherErreur("Le prix ne peut pas etre negatif !");
    } while (p.prix < 0);

    do {
        printf("Quantite : ");
        scanf("%d", &p.quantite);
        if (p.quantite < 0)
            afficherErreur("La quantite ne peut pas etre negative !");
    } while (p.quantite < 0);

    printf("Categorie : ");
    Categorie categories[100];
    int nbCategories;

    if (!chargerCategories(categories, &nbCategories)) {
        printf("Erreur : impossible de charger les catégories.\n");
        return;
    }

    printf("\nListe des catégories disponibles :\n");
    for (int i = 0; i < nbCategories; i++) {
        printf("%d. %s\n", categories[i].id, categories[i].libelle);
    }

    int choixId;
    int valide = 0;
    do {
        printf("Choisissez l'ID de la catégorie : ");
        scanf("%d", &choixId);

        for (int i = 0; i < nbCategories; i++) {
            if (categories[i].id == choixId) {
                strcpy(p.categorie, categories[i].libelle);
                valide = 1;
                break;
            }
        }

        if (!valide) {
            printf("ID invalide. Veuillez réessayer.\n");
        }
    } while (!valide);

    do {
        printf("Date de peremption (YYYY-MM-DD) : ");
        scanf("%s", p.date_peremption);
        if (!estDateValide(p.date_peremption)) {
            afficherErreur("Date invalide !");
        } else break;
    } while (1);

    sauvegarderProduitIndividuel(p);
    //afficherSucces("[✓] Produit ajoute avec succes.");
}

void modifierEtSauvegarderProduit() {
    char code[CODE_LENGTH];
    printf("\n=== MODIFIER PRODUIT ===\n");
    printf("Code du produit a modifier : ");
    scanf("%s", code);

    for (int i = 0; i < nb_produits; i++) {
        if (strcmp(produits[i].code, code) == 0) {
            Produit* p = &produits[i];

            printf("Designation (%s) : ", p->designation);
            scanf(" %[^\n]", p->designation);

            do {
                printf("Prix (%.2f) : ", p->prix);
                scanf("%f", &p->prix);
                if (p->prix < 0)
                    afficherErreur("Le prix ne peut pas etre negatif !");
            } while (p->prix < 0);

            do {
                printf("Quantite (%d) : ", p->quantite);
                scanf("%d", &p->quantite);
                if (p->quantite < 0)
                    afficherErreur("La quantite ne peut pas etre negative !");
            } while (p->quantite < 0);

            printf("Categorie (%s) : ", p->categorie);
            scanf(" %[^\n]", p->categorie);

            do {
                printf("Date de peremption (%s) : ", p->date_peremption);
                scanf("%s", p->date_peremption);
                if (!estDateValide(p->date_peremption))
                    afficherErreur("Date invalide !");
                else break;
            } while (1);

            sauvegarderProduits();
            afficherSucces("[✓] Produit modifie avec succes.");
            return;
        }
    }

    afficherErreur("[ERREUR] Produit non trouve.");
}

void supprimerEtSauvegarderProduit() {
    char code[CODE_LENGTH];
    printf("\n=== SUPPRIMER PRODUIT ===\n");
    printf("Code du produit a supprimer : ");
    scanf("%s", code);

    for (int i = 0; i < nb_produits; i++) {
        if (strcmp(produits[i].code, code) == 0) {
            for (int j = i; j < nb_produits - 1; j++) {
                produits[j] = produits[j + 1];
            }
            nb_produits--;
            sauvegarderProduits();
            afficherSucces("[✓] Produit supprime avec succes.");
            return;
        }
    }

    afficherErreur("[ERREUR] Produit non trouve.");
}

void afficherProduits() {
    printf("\n=== LISTE DES PRODUITS ===\n");

    if (nb_produits == 0) {
        printf("Aucun produit trouve.\n");
        return;
    }

    printf("%-6s | %-20s | %-8s | %-8s | %-15s | %-12s\n",
           "Code", "Designation", "Prix", "Stock", "Categorie", "Peremption");
    printf("---------------------------------------------------------------------------------------\n");

    for (int i = 0; i < nb_produits; i++) {
        Produit p = produits[i];
        printf("%-6s | %-20s | %-8.2f | %-8d | %-15s | %-12s\n",
               p.code, p.designation, p.prix, p.quantite, p.categorie, p.date_peremption);
    }
}

// ========== MENU PRINCIPAL ==========

void menuGestionProduits() {
    chargerProduitsDepuisFichier();
    int choix;
    do {
        printf("\033[1;36m\n=== MENU GESTION DES PRODUITS ===\033[0m\n");
        printf("1. Ajouter un produit\n");
        printf("2. Modifier un produit\n");
        printf("3. Supprimer un produit\n");
        printf("4. Afficher tous les produits\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                ajouterEtSauvegarderProduit();
                break;
            case 2:
                modifierEtSauvegarderProduit();
                break;
            case 3:
                supprimerEtSauvegarderProduit();
                break;
            case 4:
                afficherProduits();
                break;
            case 0:
                printf("Retour...\n");
                break;
            default:
                afficherErreur("[ERREUR] Choix invalide !");
        }
    } while (choix != 0);
}
