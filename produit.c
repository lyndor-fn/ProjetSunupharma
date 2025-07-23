#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "produit.h"
#include "categories.h"

#define FICHIER_PRODUITS "PRODUCTS.dat"

// ======== GLOBAL =========
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

void consulterStock(Produit produits[], int nbProduits) {
    chargerProduitsDepuisFichier();
              //  printf("DEBUG : %d produits chargés\n", nbProduits);

    printf("\n======= STOCK ACTUEL =======\n");
    for (int i = 0; i < nb_produits; i++) {
        if (strlen(produits[i].code) == 0) continue;  // ignorer les lignes vides
        printf("%s - %s : %d unités | Prix : %.2f FCFA\n",
               produits[i].code, produits[i].designation,
               produits[i].quantite, produits[i].prix);
    }
    printf("=============================\n");
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

void chargerProduitsDepuisFichier() {
    FILE *f = fopen(FICHIER_PRODUITS, "rb");
    if (!f) {
        nb_produits = 0;
        return;
    }

    nb_produits = 0;
    while (fread(&produits[nb_produits], sizeof(Produit), 1, f)) {
        nb_produits++;
    }
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
        printf("Erreur : impossible de charger les categories.\n");
        return;
    }

    printf("\nListe des categories disponibles :\n");
    for (int i = 0; i < nbCategories; i++) {
        printf("%d. %s\n", categories[i].id, categories[i].libelle);
    }

    int choixId;
    int valide = 0;
    do {
        printf("Choisissez l'ID de la categorie : ");
        scanf("%d", &choixId);

        for (int i = 0; i < nbCategories; i++) {
            if (categories[i].id == choixId) {
                strcpy(p.categorie, categories[i].libelle);
                valide = 1;
                break;
            }
        }

        if (!valide) {
            printf("ID invalide. Veuillez reessayer.\n");
        }
    } while (!valide);

    do {
        printf("Date de peremption (YYYY-MM-DD) : ");
        scanf("%s", p.date_peremption);
        if (!estDateValide(p.date_peremption)) {
            afficherErreur("Date invalide !");
        } else break;
    } while (1);

    produits[nb_produits++] = p;
    sauvegarderProduits();
    afficherSucces("[ok] Produit ajoute avec succes.");
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
            afficherSucces("[ok] Produit modifie avec succes.");
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
            afficherSucces("[ok] Produit supprime avec succes.");
            return;
        }
    }

    afficherErreur("[ERREUR] Produit non trouve.");
}

void afficherProduits() {
    printf("\n=== LISTE DES PRODUITS ===\n");

    if (nb_produits == 0) {
        printf("Aucun produit trouvé.\n");
        return;
    }

    printf("%-6s | %-20s | %-8s | %-8s | %-15s | %-12s\n",
           "Code", "Désignation", "Prix", "Stock", "Catégorie", "Péremption");
    printf("---------------------------------------------------------------------------------------\n");

    int produitsVisibles = 0;

    for (int i = 0; i < nb_produits; i++) {
        Produit p = produits[i];

        // Ignorer les entrées avec code ou désignation vide
        if (strlen(p.code) == 0 || strlen(p.designation) == 0)
            continue;

        printf("%-6s | %-20s | %-8.2f | %-8d | %-15s | %-12s\n",
               p.code, p.designation, p.prix, p.quantite, p.categorie, p.date_peremption);

        produitsVisibles++;
    }

    if (produitsVisibles == 0) {
        printf("Aucun produit valide a afficher.\n");
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
            case 1:clearScreen();
                ajouterEtSauvegarderProduit();
                break;
            case 2:clearScreen();
                modifierEtSauvegarderProduit();
                break;
            case 3:clearScreen();
                supprimerEtSauvegarderProduit();
                break;
            case 4:clearScreen();
                afficherProduits();
                break;
            case 0:clearScreen();
                printf("Retour...\n");
                break;
            default:
                afficherErreur("[ERREUR] Choix invalide !");
        }
    } while (choix != 0);
}

