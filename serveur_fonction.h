
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>       // Pour printf, fprintf, sprintf
#include <stdlib.h>      // Pour exit, atoi
#include <string.h>      // Pour strerror
#include <errno.h>       // Pour errno
#include <unistd.h>      // Pour read, write, close, sleep
#include <sys/types.h>   // Pour les définitions de types système (socklen_t)
#include <sys/socket.h>  // Pour socket, bind, listen, accept, AF_INET, SOCK_STREAM
#include <netinet/in.h>  // Pour sockaddr_in, htons, ntohs, INADDR_ANY
#include <arpa/inet.h>   // Pour inet_ntoa
//pour la gestion multijoueur en simultané
#include <sys/wait.h> // Pour waitpid
#include <signal.h>    // Pour sigaction et SIGCHLD
#include <time.h> //pour la génération pseudo-aléatoire
#include <signal.h>


#define TOTAL_COUPS 50
#define TAILLE_GRILLE 20
#define TAILLE_BUFF 50
#define NBRE_PORTE_AVIONS 1
#define NBRE_FREGATES 1


extern volatile sig_atomic_t nb_joueurs_actifs;

//--------------------------------------------------------------------------------------------------------


typedef struct{
    int x;
    int y;
} __attribute__((packed)) position_t;


//   Résultat renvoyé par le serveur au client
enum RESULTAT_TIR{
    RATEE,
    TOUCHEE,
    COULEE,
    GAGNEE,
    PERDU
};

typedef struct{
    enum RESULTAT_TIR resultat;
    int coup_restant ;
} __attribute__((packed)) reponse_serveur_t;

enum TYPE_BATEAU{
    FREGATE=3,
    PORTE_AVION=5
};

enum ETAT_CASE{
    NON_ACTIVE,
    ACTIVE
};

typedef struct case_t{
    enum ETAT_CASE etat;
    position_t position;
}case_t;


typedef struct bateau_t{
    enum TYPE_BATEAU type;
    struct case_t *cases_occupee[5]; //mettre à 5 case_t, on en activera que 3 pour les frégate (on met des zéro sur les deux dernières)

}bateau_t;



typedef struct bateau_en_jeu_t{
    struct bateau_t *navires[NBRE_FREGATES+NBRE_PORTE_AVIONS];

}bateau_en_jeu_t;


//METHODES & FONCTIONS
int afficher_ecran_titre();

void gestionnaire_signaux_fils(int sig);

_Bool la_partie_est_elle_finie(int * nbr_coup_restant, _Bool * fin_de_partie_, bateau_en_jeu_t * liste_des_bateau_tx,_Bool * victoire);

void envoyer_code_victoire_defaite(reponse_serveur_t *reponse,_Bool *fin_de_partie,_Bool *victoire);

_Bool reste_t_il_un_bateau(bateau_en_jeu_t *bateau_en_jeu_t);

int dec_nb_coup_restant(int * nbr_coup_restant);

bateau_t * trouver_le_bateau_correspondant(position_t * pos, bateau_en_jeu_t * liste_bateau_t);

int resultat_du_tir(position_t *pos, bateau_en_jeu_t *liste_des_bateau_t);

_Bool le_bateau_est_il_coulee(bateau_t * le_bateau_t);

_Bool generation_des_navires(bateau_en_jeu_t *liste_de_bateau_t);

void liberation_memoire(bateau_en_jeu_t *ma_liste);

_Bool initialisation_de_la_partie(bateau_en_jeu_t *liste_de_bateau_t);

void afficher_grille_triche(const bateau_en_jeu_t *liste_des_bateaux_t);
 
void signal_sigterm_serveur(int num_signal);

void signal_sigterm_fils(int sig);
