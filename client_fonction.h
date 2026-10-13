#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TAILLE_GRILLE 50

//STRUCTURE DE DONNÉES (=la même que le serveur)
typedef struct{ //l'attribut "packed" supprime la mémoire vide qui pourrait se glisser entre les variables (=padding)
    int x;//4 octets
    int y;
}__attribute__((packed)) position_t;

//2 int = 8 octets en mémoire

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

// METHODES ET FONCTIONS

extern void afficher_ecran_titre();
extern void afficher_ecran_fin(_Bool * victoire_);
extern _Bool prendre_les_entree_clavier(char *buf_write, position_t* tir_joueur);
extern _Bool traitement_entree_clavier(char *buff,position_t* tir_joueur);
extern _Bool interpreter_la_reponse_serveur(char * buff, reponse_serveur_t * reponse,_Bool *victoire);