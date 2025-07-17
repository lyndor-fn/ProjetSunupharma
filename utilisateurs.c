#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utilisateurs.h"
#include "menu.h"

#define FICHIER_UTILISATEURS "USERS.dat"

int verifierLoginUnique(const char* login) {
    Utilisateur u;
    FILE *f = fopen(FICHIER_UTILISATEURS, "rb");
    if (!f) return 1; // Fichier vide, login dispo
    while (fread(&u, sizeof(Utilisateur), 1, f)) {
        if (strcmp(u.login, login) == 0) {
            fclose(f);
            return 0; // Login d�j� existant
        }
    }
    fclose(f);
    return 1; // Login unique
}
void nettoyerSaisie(char* chaine) {
    size_t len = strlen(chaine);
    if (len > 0 && chaine[len - 1] == '\n') {
        chaine[len - 1] = '\0';
    }
}
int validerEtFormaterTelephone(const char* saisie, char* resultat) {
    char chiffres[10];
    int j = 0;

    // Extraire uniquement les chiffres
    for (int i = 0; saisie[i] != '\0'; i++) {
        if (isdigit(saisie[i])) {
            if (j < 9) {
                chiffres[j++] = saisie[i];
            } else {
                return 0; // Trop de chiffres
            }
        }
    }
    chiffres[j] = '\0';

    // Vérifie qu'on a bien 9 chiffres
    if (strlen(chiffres) != 9) return 0;

    // Formater en +221 XX XXX XX XX
    sprintf(resultat, "+221 %.2s %.3s %.2s %.2s",
            chiffres,
            chiffres + 2,
            chiffres + 5,
            chiffres + 7);

    return 1;
}
Utilisateur saisirUtilisateur() {
    Utilisateur u;
    char saisie[100];

    clearScreen();

    printf("=== SAISIE UTILISATEUR ===\n");

    // Login
    do {
        printf("Login (5 lettres MAJ) : ");
        fgets(saisie, sizeof(saisie), stdin);
        nettoyerSaisie(saisie);
        if (strlen(saisie) != 5) {
            printf("Login doit avoir 5 lettres !\n");
            continue;
        }
        int ok = 1;
        for (int i = 0; i < 5; i++) {
            if (!isupper(saisie[i])) ok = 0;
        }
        if (!ok) {
            printf("Login invalide (MAJ uniquement)\n");
        } else if (!verifierLoginUnique(saisie)) {
            printf("Login déjà utilisé !\n");
        } else {
            strcpy(u.login, saisie);
            break;
        }
    } while (1);

    // Nom
    printf("Nom : ");
    fgets(u.nom, sizeof(u.nom), stdin);
    nettoyerSaisie(u.nom);

    // Prénom
    printf("Prenom : ");
    fgets(u.prenom, sizeof(u.prenom), stdin);
    nettoyerSaisie(u.prenom);

    // Téléphone
    do {
        printf("Telephone (ex: 779876543 ou 77 987 65 43) : ");
        fgets(saisie, sizeof(saisie), stdin);
        nettoyerSaisie(saisie);
        if (!validerEtFormaterTelephone(saisie, u.telephone)) {
            printf("\033[31mFormat invalide ! Reessayez.\033[0m\n");
        } else {
            break;
        }
    } while (1);

    // Rôle
    do {
        printf("Role (ADMIN ou PHARMACIEN) : ");
        fgets(saisie, sizeof(saisie), stdin);
        nettoyerSaisie(saisie);
        if (strcmp(saisie, "ADMIN") != 0 && strcmp(saisie, "PHARMACIEN") != 0) {
            printf("Rôle invalide !\n");
        } else {
            strcpy(u.role, saisie);
            break;
        }
    } while (1);

    // Valeurs par défaut
    strcpy(u.motDePasse, "pharma123");
    u.estActif = 1;
    u.premiereConnexion = 1;

    return u;
}
void ajouterUtilisateur() {
    clearScreen();
    Utilisateur u = saisirUtilisateur();

    FILE *f = fopen(FICHIER_UTILISATEURS, "ab");
    if (f) {
        fwrite(&u, sizeof(Utilisateur), 1, f);
        fclose(f);
        printf("\033[32m[OK] Utilisateur ajoute avec succes.\033[0m\n");
    } else {
        printf("\033[31m[ERREUR] Impossible d'écrire dans le fichier.\033[0m\n");
    }
}

// LISTER UTILISATEURS

void afficherUtilisateurs() {
    FILE *f = fopen("USERS.dat", "rb");
    if (!f) {
        printf("\033[31m[ERREUR] Impossible d'ouvrir USERS.dat\033[0m\n");
        return;
    }

    Utilisateur u;
    int cpt= 0;


    printf("\033[1;34m\n=== LISTE DES UTILISATEURS ===\033[0m\n");
    printf("\033[0;33m%-6s | %-15s | %-15s | %-20s | %-12s | %-6s | %-15s\033[0m\n",
           "Login", "Nom", "Prenom", "Telephone", "Role", "Actif", "1ere Connexion");
    printf("-------------------------------------------------------------------------------------------------------------\n");

    while (fread(&u, sizeof(Utilisateur), 1, f)) {
        printf("%-6s | %-15s | %-15s | %-20s | %-12s | %-6s | %-15s\n",
               u.login,
               u.nom,
               u.prenom,
               u.telephone,
               u.role,
               u.estActif ? "Oui" : "Non",
               u.premiereConnexion ? "Oui" : "Non"

        );
                cpt++;
    }



    if (cpt == 0) {
        printf("Aucun utilisateur trouve.\n");
    }

    fclose(f);
}
// CHANGER DE MDP
void changerMotDePasse(const char* login) {
    FILE *f = fopen("USERS.dat", "r+b"); // mode lecture+�criture binaire
    if (!f) {
        printf("\033[31m[ERREUR] Impossible d'ouvrir USERS.dat\033[0m\n");
        return;
    }

    Utilisateur u;
    int trouve = 0;

    while (fread(&u, sizeof(Utilisateur), 1, f)) {
        if (strcmp(u.login, login) == 0) {
            trouve = 1;
            break;
        }
    }

    if (!trouve) {
        printf("\033[31mUtilisateur non trouve.\033[0m\n");
        fclose(f);
        return;
    }

    char nouveauMDP[20];
    char confirmation[20];

    do {
        printf("Nouveau mot de passe : ");
        scanf("%19s", nouveauMDP);
        printf("Confirmer mot de passe : ");
        scanf("%19s", confirmation);

        if (strcmp(nouveauMDP, confirmation) != 0) {
            printf("\033[31mLes mots de passe ne correspondent pas. Reessayez.\033[0m\n");
        } else if (strlen(nouveauMDP) < 6) {
            printf("\033[31mLe mot de passe doit contenir au moins 6 caracteres.\033[0m\n");
        } else {
            break;
        }
    } while (1);

    // Mettre � jour la structure utilisateur
    strcpy(u.motDePasse, nouveauMDP);
    u.premiereConnexion = 0;

    // Reculer le curseur au d�but de cet enregistrement pour l'�craser
    fseek(f, -sizeof(Utilisateur), SEEK_CUR);
    fwrite(&u, sizeof(Utilisateur), 1, f);

    fclose(f);

    printf("\033[32mMot de passe modifie avec succes !\033[0m\n");
}

// CHANGER DE STATUT

void changerStatutUtilisateur() {
    char login[6];
    int nouveauStatut;
    int trouve = 0;

    printf("\n\033[1;34m=== BLOCAGE / DEBLOCAGE UTILISATEUR ===\033[0m\n");
    printf("Login de l'utilisateur a modifier : ");
    scanf("%5s", login);

    FILE *f = fopen("USERS.dat", "r+b");
    if (!f) {
        printf("\033[31m[ERREUR] Impossible d'ouvrir USERS.dat\033[0m\n");
        return;
    }

    Utilisateur u;
    while (fread(&u, sizeof(Utilisateur), 1, f)) {
        if (strcmp(u.login, login) == 0) {
            trouve = 1;
            printf("Utilisateur trouve : %s %s (%s)\n", u.nom, u.prenom, u.login);
            printf("Statut actuel : %s\n", u.estActif ? "Actif" : "Bloque");

            do {
                printf("Nouveau statut (1 = Actif, 0 = Bloque) : ");
                scanf("%d", &nouveauStatut);
            } while (nouveauStatut != 0 && nouveauStatut != 1);

            u.estActif = nouveauStatut;

            // Revenir � la bonne position pour �craser
            fseek(f, -sizeof(Utilisateur), SEEK_CUR);
            fwrite(&u, sizeof(Utilisateur), 1, f);

            printf("\033[32m[OK] Statut mis a jour avec succes.\033[0m\n");
            break;
        }
    }

    fclose(f);

    if (!trouve) {
        printf("\033[31m[ERREUR] Utilisateur non trouve.\033[0m\n");
    }
}







