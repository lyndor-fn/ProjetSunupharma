#ifndef UTILISATEUR_H
#define UTILISATEUR_H

typedef struct {
    char login[6];          // 5 lettres majuscules
<<<<<<< HEAD
    char motDePasse[20];    // par defaut : pharma123
=======
    char motDePasse[20];    // par d�faut : pharma123
>>>>>>> 844778b0052d396ad8e8cec2386a55730f395a34
    char nom[20];
    char prenom[30];
    char telephone[30];
    char role[15];          // ADMIN ou PHARMA
<<<<<<< HEAD
    int estActif;           // 1 = actif, 0 = bloque
=======
    int estActif;           // 1 = actif, 0 = bloqu�
>>>>>>> 844778b0052d396ad8e8cec2386a55730f395a34
    int premiereConnexion;  // 1 = oui, 0 = non
} Utilisateur;

int validerEtFormaterTelephone(const char* saisie, char* resultat);
Utilisateur saisirUtilisateur();
void ajouterUtilisateur();
void afficherUtilisateurs();
int verifierLoginUnique(const char* login);
void changerMotDePasse();
void changerStatutUtilisateur();

#endif


