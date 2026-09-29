#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TOTAL_COUPS 50
#define TAILLE_BUFF 30

int nbr_coup_restant = TOTAL_COUPS;
char * buff_coup_restant[TAILLE_BUFF];
bool fin_de_partie = 0;

char ** grille_de_jeu[50][50];

struct bateau{
    int cases[5]

}
















//formatte un buffer spécifié en argument; renvoie 1 en cas d'erreur, 0 si tout se passe bien
bool print_nb_coup_restant(char * buff, int nb_coup){
    if(snprintf(buff_coup_restant, sizeof(buffer), "Il vous reste %d coups à jouer", nb_coup)){
        return 0;
    }
    else {
        return 1; 
    }
    
}

int dec_nb_coup_restant(int * nbr_coup_restant){
    nbr_coup_restant=nbr_coup_restant-1;
}



bool reste_t_il_un_bateau(char ** grille_de_jeu){

}
// 
bool fin_de_partie(int * nbr_coup_restant){
    if(nbr_coup_restant <= 0 && ){
        fin_de_partie =1;
        return 0;
    }
    else {
        return 1;
    }

}