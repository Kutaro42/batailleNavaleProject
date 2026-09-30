#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOTAL_COUPS 50
#define TAILLE_BUFF 50
#define NBRE_PORTE_AVIONS 1
#define NBRE_FREGATES 1

//DECLARATION DES VARIABLES GLOBALES
extern int nbr_coup_restant_ ;
extern char * buff_coup_restant_[TAILLE_BUFF];
extern bool fin_de_partie_ ;
extern bool reste_il_un_bateau_;
extern bool ** grille_de_jeu_[50][50];
extern struct bateau_ bateau_en_jeu;

typedef struct position_ {
    int x;
    int y;
}position_;

//   Résultat renvoyé par le serveur au client
enum RESULT_SHOOT{
    TOUCHEE,
    RATEE,
    COULEE
};

enum STATE_CASE{
    NON_ACTIVE,
    ACTIVE
};

typedef struct Case {
    enum STATE_CASE etat;
    struct position_ position;
}Case;

typedef struct porte_avion {
    struct Case *cases_occupee[5];
    //...

}porte_avion;

typedef struct fregate{
    struct Case *case_occupee[3];
    //<..
}fregate;

typedef struct bateau_{
    struct fregate *fregate_en_jeu[NBRE_FREGATES];
    struct porte_avion *porte_avion_en_jeu[NBRE_PORTE_AVIONS];
}bateau_;


/*
struct bateau {
    int size;
    int *cases_occuppee[size];
}
*/

//METHODES & FONCTIONS
bool la_partie_est_elle_finie(int * nbr_coup_restant, int*fin_de_partie);
bool reste_t_il_un_bateau();
int dec_nb_coup_restant(int * nbr_coup_restant);
bool print_nb_coup_restant(char * buff, int nb_coup);



