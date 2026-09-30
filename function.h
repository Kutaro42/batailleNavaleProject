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

int nbr_coup_restant = TOTAL_COUPS;
char * buff_coup_restant[TAILLE_BUFF];
bool fin_de_partie = 0;
bool reste_il_un_bateau = 1;
bool ** grille_de_jeu[50][50];

typedef struct position {
    int x;
    int y;
}position;

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

struct bateau_ bateau_en_jeu;
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



