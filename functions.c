
#include "function.h"

//formatte un buffer spécifié en argument; renvoie 1 en cas d'erreur, 0 si tout se passe bien
bool print_nb_coup_restant(char * buff, int nb_coup){
    if(snprintf(buff, sizeof(buff), "Il vous reste %d coups à jouer", nb_coup)){
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

    //parcourir la liste des bateaux

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