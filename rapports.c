#include <stdio.h>
#include <time.h>
#include <string.h>
#include "rapports.h"
<<<<<<< HEAD
#include "produit.h"
=======
#include "produits.h"
>>>>>>> 844778b0052d396ad8e8cec2386a55730f395a34
#include "ventes.h"

void genererRapportJournalier() {
    //  Generer date AAAAMMDD
    char date[9]; // "AAAAMMDD\0"
    time_t now = time(NULL);
    struct tm today = *localtime(&now);
    sprintf(date, "%04d%02d%02d", today.tm_year + 1900, today.tm_mon + 1, today.tm_mday);

    //  Creer le nom du fichier rapport
    char nomFichier[30];
    sprintf(nomFichier, "ETAT_%s.txt", date);

    FILE *fRapport = fopen(nomFichier, "w");
    if (fRapport == NULL) {
        printf("[ERREUR] Impossible de creer le fichier %s\n", nomFichier);
        return;
    }

    //  en-tete
    fprintf(fRapport, "Rapport du : %04d-%02d-%02d\n", today.tm_year + 1900, today.tm_mon + 1, today.tm_mday);

    // Recuperer ventes du jour
    float totalVentes = 0;
    int nbMedicament = 0;
    int ventesOK = calculerVentesDuJour(date, &totalVentes, &nbMedicament);

    if (ventesOK) {
        fprintf(fRapport, "Total des ventes : %.2f XOF\n", totalVentes);
<<<<<<< HEAD
        fprintf(fRapport, "Nombre total de medicaments vendus : %d\n\n", nbMedicament);
=======
        fprintf(fRapport, "Nombre total de m�dicaments vendus : %d\n\n", nbMedicament);
>>>>>>> 844778b0052d396ad8e8cec2386a55730f395a34

        // Verifier les stocks critiques
        Produit produits[100];
        int nbProduits;
        if (chargerProduits(produits, &nbProduits)) {
            fprintf(fRapport, "stock critique :\n");
            verifierStocksCritiques(produits, nbProduits, fRapport);
        } else {
            fprintf(fRapport, "[INFO] Impossible de charger les produits pour les alertes de stock.\n");
        }
    } else {
<<<<<<< HEAD
        fprintf(fRapport, "[ERREUR] Aucun fichier de vente trouve pour aujourd hui.\n");
    }

    printf("Rapport journalier generer : %s\n", nomFichier);
    fclose(fRapport);
=======
        fprintf(fRapport, "[ERREUR] Aucun fichier de vente trouv� pour aujourd�hui.\n");
    }

    fclose(fRapport);
    printf("Rapport journalier g�n�r� : %s\n", nomFichier);
>>>>>>> 844778b0052d396ad8e8cec2386a55730f395a34
}


