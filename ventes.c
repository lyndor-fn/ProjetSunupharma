#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include "ventes.h"
#include "produit.h"
#include "utilisateurs.h"

void menuGestionVentes() {
    char login[6];
    printf("===== MODULE DE GESTION DES VENTES =====\n");
    printf("Entrez le login du pharmacien (5 lettres majuscules) : ");
    scanf("%s", login);

    effectuerVente(login);
}

void genererNumeroVente(char* numero) {
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    strftime(numero, 15, "%Y%m%d%H%M%S", tm_info);
}

void afficherMessage(const char* message, const char* couleur) {
    printf("%s%s\n", couleur, message); // Couleur ignorée si non ANSI
}

void creerFacture(ProduitVendu ventes[], int taille, float total, char* login) {
    char numero[15];
    genererNumeroVente(numero);

    char date[9];
    strncpy(date, numero, 8); // extrait AAAAMMDD

    system("mkdir BILLS");

    char nomFichier[100];
    sprintf(nomFichier, "BILLS/RECU_%s_001_%s.txt", date, login);

    FILE* f = fopen(nomFichier, "w");
    if (!f) {
        printf("[ERREUR] Impossible de creer la facture.\n");
        return;
    }

    fprintf(f, "FACTURE DE VENTE\n");
    fprintf(f, "Numero de vente : %s\n", numero);
    fprintf(f, "Pharmacien : %s\n", login);
    fprintf(f, "----------------------------------------\n");
    fprintf(f, "Code Nom Qte PU Total\n");

    for (int i = 0; i < taille; i++) {
        fprintf(f, "%s %s %d %.2f %.2f\n",
                ventes[i].code, ventes[i].nom,
                ventes[i].quantiteVendue,
                ventes[i].prixUnitaire,
                ventes[i].prixTotal);
    }

    fprintf(f, "\nTOTAL : %.2f XOF\n", total);
    fprintf(f, "----------------------------------------\n");
    fprintf(f, "Merci d'avoir choisi SUNUPHARMA.\n");

    fclose(f);

    printf("Facture enregistree : %s\n", nomFichier);
}

void mettreAJourStock(ProduitVendu ventes[], int nb) {
    FILE* f = fopen("PRODUCTS.dat", "rb+");
    if (!f) {
        printf("[ERREUR] Fichier PRODUCTS.dat introuvable.\n");
        return;
    }

    Produit p;
    while (fread(&p, sizeof(Produit), 1, f)) {
        for (int i = 0; i < nb; i++) {
            if (strcmp(p.code, ventes[i].code) == 0) {
                p.quantite -= ventes[i].quantiteVendue;
                fseek(f, -sizeof(Produit), SEEK_CUR);
                fwrite(&p, sizeof(Produit), 1, f);
                break;
            }
        }
    }

    fclose(f);
}

time_t convertirDate(const char* dateStr) {
    struct tm tm_date = {0};
    sscanf(dateStr, "%4d-%2d-%2d", &tm_date.tm_year, &tm_date.tm_mon, &tm_date.tm_mday);
    tm_date.tm_year -= 1900;
    tm_date.tm_mon -= 1;
    return mktime(&tm_date);
}

void effectuerVente(char* login) {
    ProduitVendu ventes[50];
    int nb = 0;
    float total = 0.0;
    char code[6];
    int quantite;

    while (1) {
        printf("\nEntrer le code du medicament (ou 0 pour terminer) : ");
        scanf("%s", code);
        if (strcmp(code, "0") == 0) break;

        printf("Quantite souhaitee : ");
        scanf("%d", &quantite);

        FILE* f = fopen("PRODUCTS.dat", "rb");
        if (!f) {
            printf("[ERREUR] Impossible de lire le stock.\n");
            return;
        }

        Produit p;
        int trouve = 0;
        while (fread(&p, sizeof(Produit), 1, f)) {
            if (strcmp(p.code, code) == 0) {
                trouve = 1;
                time_t maintenant = time(NULL);
                time_t datePeremption = convertirDate(p.date_peremption);
                if (difftime(datePeremption, maintenant) <= 0) {
                    printf("[ERREUR] Le medicament %s est perime.\n", p.designation);
                    break;
                }

                if (p.quantite >= quantite) {
                    strcpy(ventes[nb].code, p.code);
                    strcpy(ventes[nb].nom, p.designation);
                    ventes[nb].quantiteVendue = quantite;
                    ventes[nb].prixUnitaire = p.prix;
                    ventes[nb].prixTotal = quantite * p.prix;
                    total += ventes[nb].prixTotal;
                    nb++;
                    printf("[OK] Medicament ajoute a la commande.\n");
                } else {
                    printf("[ERREUR] Stock insuffisant pour %s.\n", p.designation);
                }

                break;
            }
        }

        fclose(f);
        if (!trouve) {
            printf("[ERREUR] Medicament introuvable.\n");
        }
    }

    if (nb > 0) {
        char confirm;
        printf("Confirmer la vente ? (O/N) : ");
        scanf(" %c", &confirm);
        if (confirm == 'O' || confirm == 'o') {
            creerFacture(ventes, nb, total, login);
            mettreAJourStock(ventes, nb);
            printf("[OK] Vente effectuee. Total : %.2f XOF\n", total);
        } else {
            printf("[INFO] Vente annulee.\n");
        }
    } else {
        printf("[INFO] Aucune vente enregistree.\n");
    }
}

int calculerVentesDuJour(const char* date, float* totalVentes, int* nbMedicament) {
    char dossier[] = "BILLS";
    char prefix[20];
    sprintf(prefix, "RECU_%s", date);

    *totalVentes = 0;
    *nbMedicament = 0;

    DIR* dir = opendir(dossier);
    if (!dir) return 0;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, prefix)) {
            char chemin[120];
            sprintf(chemin, "%s/%s", dossier, entry->d_name);

            FILE* f = fopen(chemin, "r");
            if (!f) continue;

            char ligne[200];
            while (fgets(ligne, sizeof(ligne), f)) {
                float prix;
                int qte;
                if (sscanf(ligne, "%*s %*s %d %*f %f", &qte, &prix) == 2) {
                    *totalVentes += prix;
                    *nbMedicament += qte;
                }
            }

            fclose(f);
        }
    }

    closedir(dir);
    return 1;
}

void verifierStocksCritiques(Produit produits[], int nb, FILE* fRapport) {
    for (int i = 0; i < nb; i++) {
        if (produits[i].quantite < 5) {
            fprintf(fRapport, "- %s (%s) : %d unites restantes\n",
                    produits[i].designation, produits[i].code, produits[i].quantite);
        }
    }
}

int chargerProduits(Produit produits[], int* nb) {
    FILE* f = fopen("PRODUCTS.dat", "rb");
    if (!f) return 0;

    *nb = 0;
    while (fread(&produits[*nb], sizeof(Produit), 1, f)) {
        (*nb)++;
    }

    fclose(f);
    return 1;
}
