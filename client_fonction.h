
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

//STRUCTURE DE DONNÉES
typedef struct{ //l'attribut "packed" supprime la mémoire vide qui pourrait se glisser entre les variables (=padding)
    int x;//4 octets
    int y;
}__attribute__((packed)) position_;

//2 int = 8 octets en mémoire


// METHODES ET FONCTIONS

extern int afficher_ecran_titre();
extern _Bool prendre_les_entree_clavier(char *buf_write, position_ tir_joueur);
extern _Bool traitement_entree_clavier(char *buff,position_ tir_joueur);