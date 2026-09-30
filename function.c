#include "function.h"



//definitition des variables globales :
int nbr_coup_restant_ = TOTAL_COUPS;
_Bool fin_de_partie_ = 0;
_Bool reste_il_un_bateau_ = 1;

//formatte un buffer spécifié en argument; renvoie 0 = faux en cas d'erreur, 1=vrai si tout se passe bien
_Bool print_nb_coup_restant(char * buff, int nb_coup){
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


//Renvoie le type du bateau spécifié en paramètre
int type_bateau(){
    if ()
}


//Renvoie 1 si toute les cases du bateau sont détruites, 0 sinon
_Bool le_bateau_est_il_coulee(Bateau * le_bateau){
    //code :
    
    for(int i=0;0<le_bateau->type;++i){
        if(le_bateau->cases_occupee[i]->etat==ACTIVE){ //cette fonction sera appelée très souvent, on cherche donc à éliminer le cas le plus probable en premier
            //code
            
        }
    }

}

//Renvoie le résultat du tir effectué par le joueur depuis le client
int resultat_du_tir(position_ *pos, _Bool * grille_de_jeu_[50][50]){

    //variable résultat
    enum RESULT_SHOOT result; 

    //vérifier si la position correspond à un bateau
    if(grille_de_jeu_[*pos->x][*pos->y] == 1){          //si une case de bateau active est présente
        result=TOUCHEE;
    }

}

_Bool reste_t_il_un_bateau(bateau_en_jeu *bateau_en_jeu,_Bool *reste_il_un_bateau_){

    //parcourir la liste des bateaux

    for(int i=0;i<NBRE_FREGATES+NBRE_PORTE_AVIONS;++i){
        //code :
        for(int j=0;j<NBRE_FREGATES+NBRE_PORTE_AVIONS;++j){
            if(bateau_en_jeu->fregate_en_jeu[i]->case_occupee[j]->etat == ACTIVE){ //si on trouve ne serait-ce qu'un seul bateau avec une case encore active, la partie continue
                *reste_il_un_bateau_=1;
                return *reste_il_un_bateau_; //bateau trouvé = reprise de la partie
            }

        }

    }

    //DANS LE CAS OU IL NE RESTE AUCUN BATEAU
    *reste_il_un_bateau_=0;// on met à jour la variable
    return *reste_il_un_bateau_; //on renvoie sa valeur comme code de sortie booléen pour traitement

}

// 
_Bool la_partie_est_elle_finie(int * nbr_coup_restant, _Bool*fin_de_partie_){
    if(nbr_coup_restant <= 0 && !reste_t_il_un_bateau ){
        *fin_de_partie_ =1;
    }
    else {
        *fin_de_partie_=0;
    }
    return *fin_de_partie_;

}