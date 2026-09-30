
#include "function.h"

//formatte un buffer spécifié en argument; renvoie 0 = faux en cas d'erreur, 1=vrai si tout se passe bien
bool print_nb_coup_restant(char * buff, int nb_coup){
    if(snprintf(buff, TAILLE_BUFF, "Il vous reste %d coups à jouer", nb_coup)){ //correction : taille définie directement par la taille maximum TAILLE_BUFF car sizeof() renvoie une taille de 8bit (comprenhensible car *buf est une adresse sur 8 bit)
        return 1;
    }
    else {
        return 0; 
    }
    
}

//renvoie le nbr de coups restants
int dec_nb_coup_restant(int * nbr_coup_restant){
    *nbr_coup_restant= *nbr_coup_restant-1;
    return *nbr_coup_restant;
}



bool reste_t_il_un_bateau(){

    //parcourir la liste des bateaux

}
// 
bool la_partie_est_elle_finie(int * nbr_coup_restant, int*fin_de_partie){
    if(nbr_coup_restant <= 0 && !reste_t_il_un_bateau ){
        *fin_de_partie =1;
        return 0;
    }
    else {
        return 1;
    }

}