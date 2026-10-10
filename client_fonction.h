
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
}__attribute__((packed)) position_;

//2 int = 8 octets en mémoire

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

// METHODES ET FONCTIONS

extern void afficher_ecran_titre();
extern void afficher_ecran_fin(_Bool * victoire_);
extern _Bool prendre_les_entree_clavier(char *buf_write, position_* tir_joueur);
extern _Bool traitement_entree_clavier(char *buff,position_* tir_joueur);
extern _Bool interpreter_la_reponse_serveur(char * buff, ReponseServeur * reponse,_Bool *victoire);