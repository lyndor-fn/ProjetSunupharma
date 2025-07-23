#include <stdio.h>
#include <time.h>
#include <string.h>

#include "rapports.h"
#include "produit.h"
#include "ventes.h"
#include "utilisateurs.h"

void menuRapport() {
    int choix;
    do {
        printf("\033[1;36m\n=== MENU RAPPORT ===\n\033[0m");
        printf("1. Imprimer le rapport\n");
        printf("2. RETOUR\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                clearScreen();
                genererRapportJournalier();
                break;
            case 2:
                clearScreen();
                menuPharmacien();
                break;
            default:
                printf("Choix invalide !\n");
        }
    } while (choix != 0);
}

void verifierStocksCritiques(Produit produits[], int nb, FILE* fRapport) {
    for (int i = 0; i < nb; i++) {
        if (produits[i].quantite < 5) {
            fprintf(fRapport, "- %s (%s) : %d unités restantes\n",
                    produits[i].designation, produits[i].code, produits[i].quantite);
        }
    }
}

void genererRapportJournalier() {
    char date[9]; // Format AAAAMMDD
    time_t now = time(NULL);
    struct tm today = *localtime(&now);
    sprintf(date, "%04d%02d%02d", today.tm_year + 1900, today.tm_mon + 1, today.tm_mday);

    char nomFichier[50];
    sprintf(nomFichier, "ETAT_%s.txt", date);

    FILE *fRapport = fopen(nomFichier, "w");
    if (!fRapport) {
        printf("[ERREUR] Impossible de créer le fichier %s\n", nomFichier);
        return;
    }

    fprintf(fRapport, "Rapport du : %04d-%02d-%02d\n", today.tm_year + 1900, today.tm_mon + 1, today.tm_mday);

    float totalVentes = 0;
    int nbMedicament = 0;
    int ventesOK = calculerVentesDuJour(date, &totalVentes, &nbMedicament);

    if (ventesOK) {
        fprintf(fRapport, "Total des ventes : %.2f FCFA\n", totalVentes);
        fprintf(fRapport, "Nombre total de médicaments vendus : %d\n\n", nbMedicament);

        // Stock critique
        chargerProduitsDepuisFichier();  // met à jour produits[] et nb_produits
        fprintf(fRapport, "Produits en stock critique (< 5 unités) :\n");
        verifierStocksCritiques(produits, nb_produits, fRapport);
    } else {
        fprintf(fRapport, "[INFO] Aucun fichier de vente trouve pour aujourd hui.\n");
    }

    fclose(fRapport);
    printf("Rapport journalier genere : %s\n", nomFichier);
}
