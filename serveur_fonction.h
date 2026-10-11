
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


#define TOTAL_COUPS 10
#define TAILLE_GRILLE 20
#define TAILLE_BUFF 50
#define NBRE_PORTE_AVIONS 1
#define NBRE_FREGATES 1


extern int nb_joueurs_actifs;

//--------------------------------------------------------------------------------------------------------


typedef struct{
    int x;
    int y;
} __attribute__((packed)) position_;


//   Résultat renvoyé par le serveur au client
enum RESULT_SHOOT{
    RATEE,
    TOUCHEE,
    COULEE
};

typedef struct{
    enum RESULT_SHOOT resultat;
    int coup_restant ;
} __attribute__((packed)) ReponseServeur;

enum TYPE_BOAT{
    FREGATE=3,
    PORTE_AVION=5
};

enum STATE_CASE{
    NON_ACTIVE,
    ACTIVE
};

typedef struct Case {
    enum STATE_CASE etat;
    position_ position;
}Case;


//------------------------------------------------------
/*
typedef struct porte_avion {
    struct Case *cases_occupee[5];
    const int taille=5;
    //...

}porte_avion;
typedef struct fregate{
    struct Case *case_occupee[3];
    const int taille=3;
    //<..
}fregate;*/
//-------------------------------------------------------

typedef struct Bateau{
    enum TYPE_BOAT type;
    struct Case *cases_occupee[5]; //mettre à 5 case, on en activera que 3 pour les frégate (on met des zéro sur les deux dernières)

}Bateau;



typedef struct bateau_en_jeu_{
    struct Bateau *Bateau_en_jeu[NBRE_FREGATES+NBRE_PORTE_AVIONS];

}bateau_en_jeu_;


/*
struct bateau {
    int size;
    int *cases_occuppee[size];
}
*/

//METHODES & FONCTIONS
int afficher_ecran_titre();

void gestionnaire_signaux_fils(int sig);

_Bool la_partie_est_elle_finie(int * nbr_coup_restant, _Bool * fin_de_partie_, bateau_en_jeu_ * liste_des_bateaux,_Bool * victoire);

void envoyer_code_victoire(ReponseServeur *response);

_Bool reste_t_il_un_bateau(bateau_en_jeu_ *bateau_en_jeu);

int dec_nb_coup_restant(int * nbr_coup_restant);

_Bool print_nb_coup_restant(char * buff, int nb_coup);

Bateau * trouver_le_bateau_correspondant(position_ * pos, bateau_en_jeu_ * liste_bateau);

int resultat_du_tir(position_ *pos, bateau_en_jeu_ *liste_des_bateau);

_Bool le_bateau_est_il_coulee(Bateau * le_bateau);

//
_Bool generation_des_navires(bateau_en_jeu_ *liste_de_bateau);

_Bool liberation_memoire(bateau_en_jeu_ *ma_liste);

_Bool initialisation_de_la_partie(bateau_en_jeu_ *liste_de_bateau);