#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


#define TOTAL_COUPS 50
#define TAILLE_BUFF 50
#define NBRE_PORTE_AVIONS 1
#define NBRE_FREGATES 1


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

_Bool la_partie_est_elle_finie(int * nbr_coup_restant, _Bool*fin_de_partie_, bateau_en_jeu_ *liste_des_bateaux);

_Bool reste_t_il_un_bateau(bateau_en_jeu_ *bateau_en_jeu,_Bool *reste_il_un_bateau_);

int dec_nb_coup_restant(int * nbr_coup_restant);

_Bool print_nb_coup_restant(char * buff, int nb_coup);

Bateau * trouver_le_bateau_correspondant(position_ * pos, bateau_en_jeu_ * liste_bateau);

int resultat_du_tir(position_ *pos, bateau_en_jeu_ *liste_des_bateau);

_Bool le_bateau_est_il_coulee(Bateau * le_bateau);

//
_Bool generation_des_navires(bateau_en_jeu_ *liste_de_bateau);

_Bool liberation_memoire(bateau_en_jeu_ *ma_liste);

_Bool initialisation_de_la_partie(bateau_en_jeu_ *liste_de_bateau);