#include "serveur_fonction.h"

int nb_joueurs_actifs =0;

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


//Renvoie 1 si toute les cases du bateau_t sont détruites, 0 sinon
_Bool le_bateau_est_il_coulee(bateau_t * le_bateau){
    //code :
    
    for(int i=0;i<(int)le_bateau->type;++i){// astuce de conception : j'ai eu l'idée de placer directement la taille active du bateau_t dans l'enum de sont type, rendant la taille implicite au type de bateau_t.
        if(le_bateau->cases_occupee[i]->etat==ACTIVE){ //cette fonction sera appelée très souvent, on cherche donc à éliminer le cas le plus probable en premier
            //code
            return 0;

        }
    }
    //si pas une seule fois la boucle n'a détecté de case active :
    return 1; //on est alors certain que le bateau_t est détruit.

}

//Renvoie le résultat du tir effectué par le joueur depuis le client
int resultat_du_tir(position_t *pos, bateau_en_jeu_t *liste_des_bateau){

    //variable résultat
    enum RESULT_SHOOT result; 

    //recherche de l'éventuel bateau_t
    bateau_t * cible=trouver_le_bateau_correspondant(pos,liste_des_bateau);

    //vérifier si la position correspond à un bateau_t
    if(cible == NULL){          //si aucun bateau_t n'a cette case (cas le plus probable)
        result=RATEE;
    }
    if(cible != NULL){          //si une case de bateau_t est présente
        result=TOUCHEE;

        if (le_bateau_est_il_coulee(cible))
        {
            result=COULEE;
        }
        
    }
    return result;
}

//Identifie le bateau_t correspondant à la case spécifiée en paramètre, renvoie un pointeur vers la cible si trouvé, nullptr sinon.
bateau_t * trouver_le_bateau_correspondant(position_t * pos, bateau_en_jeu_t * liste_bateau){
    //stocker l'adresse du bateau_t trouvé ici
    bateau_t * bateau_cible=NULL;

    //code
    for(int i=0;i<NBRE_FREGATES+NBRE_PORTE_AVIONS;++i){
        for(int j=0;j<5;++j){
            if(liste_bateau->bateau_en_jeu_t[i]->cases_occupee[j]->position.x == pos->x &&
               liste_bateau->bateau_en_jeu_t[i]->cases_occupee[j]->position.y == pos->y &&
               liste_bateau->bateau_en_jeu_t[i]->cases_occupee[j]->etat == ACTIVE //parce qu'on voudrait éviter au joueur de frapper la même case en boucle
            ){
                bateau_cible=liste_bateau->bateau_en_jeu_t[i];// renvoyer le bateau_t trouvé
                liste_bateau->bateau_en_jeu_t[i]->cases_occupee[j]->etat=NON_ACTIVE; //on désactive au passage la case (car on utilise pour le moment cette fonction uniquement pour désactiver des cases, mais cette logique pourrait poser des problème si on décide de rajouter un système d'affichage des cases)

            }
        }
    }

    return bateau_cible;
}

_Bool reste_t_il_un_bateau(bateau_en_jeu_t *bateau_en_jeu_t){

    //parcourir la liste des bateaux

    for(int i=0;i<NBRE_FREGATES+NBRE_PORTE_AVIONS;++i){
        //code :
        for(int j=0;j<5;++j){// avec la logique des 5 cases partagées, on n'a plus besoin de s'embêter avec les tailles
            if(bateau_en_jeu_t->bateau_en_jeu_t[i]->cases_occupee[j]->etat == ACTIVE){ //si on trouve ne serait-ce qu'un seul bateau_t avec une case encore active, la partie continue               
                return 1; //bateau_t trouvé = reprise de la partie
            }

        }

    }

    //DANS LE CAS OU IL NE RESTE AUCUN bateau_t

    return 0; //on renvoie sa valeur comme code de sortie booléen pour traitement

}

// 
_Bool la_partie_est_elle_finie(int * nbr_coup_restant, _Bool * fin_de_partie_, bateau_en_jeu_t * liste_des_bateaux,_Bool * victoire) {
    
    // On récupère directement le résultat via la valeur de retour de la fonction
    _Bool encore_des_bateaux = reste_t_il_un_bateau(liste_des_bateaux);;
    
    // Vérification des conditions de fin de partie
    if (*nbr_coup_restant <= 0 && encore_des_bateaux) { //cas 1 : il ne reste plus de coups à jouer et il reste des bateaux
        *fin_de_partie_ = 1; //la fin de la partie
        *victoire = 0; //le client a perdu
    }
    else if (*nbr_coup_restant >0 && !encore_des_bateaux){ //cas 2 : il reste encore des coups à jouer, mais le client à détruit tout les navires
        *victoire = 1;// le client à gagné
        *fin_de_partie_ =1;
    }
    else {
        *fin_de_partie_ = 0;// cas 3 : la partie n'est pas finie !
        *victoire = 0;
    }
    
    return *fin_de_partie_;
}

//Insérer un chiffre spécial pour glisser le code de victoire dans la trame sans changer la nature de la structure renvoyée de façon élégante
void envoyer_code_victoire(reponse_serveur_t *response){
    response->resultat = 3;// le code spécial de victoire

    // on a pas besoin de toucher à la partie contenant le nbr d'essais restant, car le joueur est obligé de faire quelques actions avant de gagner, ce qui garantit que le nbre d'essais restant stocké sera toujours correct.
    // Car s'appuyant sur la dernière valeur du tour précédent.
}

//Alloue l'espace mémoire nécéssaire à la création d'une liste de bateau_t en jeu : les instancies, renvoie 0 si tout s'est bien passé, 1 en cas d'erreur
_Bool generation_des_navires(bateau_en_jeu_t *liste_de_bateau){
if (liste_de_bateau == NULL) return 1;

    int index = 0;

    // Instanciation des portes-avions (taille 5)
    for (int i = 0; i < NBRE_PORTE_AVIONS; i++) {
        bateau_t *b = (bateau_t *)malloc(sizeof(bateau_t));
        if (b == NULL) return 1;

        b->type = PORTE_AVION; 

        for (int j = 0; j < 5; j++) {
            b->cases_occupee[j] = (Case *)malloc(sizeof(Case));
            if (b->cases_occupee[j] == NULL) return 1;
            b->cases_occupee[j]->etat = ACTIVE;
        }
        liste_de_bateau->bateau_en_jeu_t[index++] = b;
    }

    // Instanciation des frégates (taille 3 active + 2 cases inactives)
    for (int i = 0; i < NBRE_FREGATES; i++) {
        bateau_t *b = (bateau_t *)malloc(sizeof(bateau_t));
        if (b == NULL) return 1;

        b->type = FREGATE; // Type = 3

        for (int j = 0; j < 5; j++) {
            b->cases_occupee[j] = (Case *)malloc(sizeof(Case));
            if (b->cases_occupee[j] == NULL) return 1;

            if (j < 3) {
                b->cases_occupee[j]->etat = ACTIVE;
            } else {
                b->cases_occupee[j]->etat = NON_ACTIVE;
            }
        }
        liste_de_bateau->bateau_en_jeu_t[index++] = b;
    }

    return 0;
}

//Attribue aux navires des coordonées sur la grille de jeu fictive, renvoie 0 si tout s'est bien passé, 1 en cas d'erreur
_Bool initialisation_de_la_partie(bateau_en_jeu_t *liste_de_bateau){
if (liste_de_bateau == NULL) return 1;


    // Initialisation du générateur de nombres aléatoires
    static int rand_init = 0;
    if (!rand_init) {
        srand(time(NULL));
        rand_init = 1;
    }

    for (int i = 0; i < NBRE_PORTE_AVIONS + NBRE_FREGATES; i++) {
        bateau_t *b = liste_de_bateau->bateau_en_jeu_t[i];
        if (b == NULL) return 1;
        int taille = 0;
        if(b->type == PORTE_AVION){
            taille = 5;
        }
        else{
            taille = 3;
        }

        int orientation = rand() % 2;                  // 0 = Horizontal, 1 = Vertical
        int x, y;

        // Génèration des coordonnées
        if (orientation == 0) {
            x = rand() % (TAILLE_GRILLE - taille);
            y = rand() % TAILLE_GRILLE;
        } else {
            x = rand() % TAILLE_GRILLE;
            y = rand() % (TAILLE_GRILLE - taille);
        }

        for (int j = 0; j < 5; j++) {
            if (j < taille) {
                int pos_x ;
                int pos_y ;

                if (orientation == 0) {
                    pos_x = x + j;
                    pos_y = y;
                } 
                else { 
                    pos_x = x;
                    pos_y = y + j;
                }

                b->cases_occupee[j]->position.x = pos_x;
                b->cases_occupee[j]->position.y = pos_y;
                b->cases_occupee[j]->etat = ACTIVE;

            } else {
                b->cases_occupee[j]->position.x = -1;
                b->cases_occupee[j]->position.y = -1;
                b->cases_occupee[j]->etat = NON_ACTIVE;
            }
        }
    }

    return 0; // Succès
}

void liberation_memoire(bateau_en_jeu_t *ma_liste){
    for (int i = 0; i < (NBRE_FREGATES + NBRE_PORTE_AVIONS); i++) {
        if (ma_liste->bateau_en_jeu_t[i] != NULL) {
            for (int j = 0; j < 5; j++) {
                free(ma_liste->bateau_en_jeu_t[i]->cases_occupee[j]);
            }
            free(ma_liste->bateau_en_jeu_t[i]);
        }
    }
}


// Définition du gestionnaire de signal pour nettoyer les processus fils
void gestionnaire_signaux_fils(int sig) {
    (void)sig; //on fait une void cast pour que le compilateur comprenne qu'on se moque de cette variable (handler lié à SIGCHLD)
    
    pid_t pid;

    //tant que le père arrive à récupérer le signal de mort de l'un de ses fils (peut importe les raisons de la mort), màj le compteur de fils en ligne sans arrêter d'écouter les nouveau clients
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) { //Wait No Hang = attend sans interrompre le programme (ne bloque pas tout)
        nb_joueurs_actifs--;
        if (nb_joueurs_actifs < 0) nb_joueurs_actifs = 0;
    }
}


void afficher_grille_triche(const bateau_en_jeu_t *liste_des_bateaux_t) {
        printf("\n=== MODE TRICHE : COORDONNÉES DES BATEAUX ===\n");
        if (liste_des_bateaux_t == NULL) return;

        for (int i = 0; i < NBRE_PORTE_AVIONS + NBRE_FREGATES; i++) {
            const bateau_t *b = liste_des_bateaux_t->bateau_en_jeu_t[i];
            if (b != NULL) {
                printf("Bateau %d (Type: %d) :\n", i, b->type);
                for (int j = 0; j < 5; j++) {
                    if (b->cases_occupee[j]->etat == ACTIVE || b->cases_occupee[j]->position.x != -1) {
                        printf("  -> Case %d : X = %d, Y = %d\n", j, 
                        b->cases_occupee[j]->position.x, 
                        b->cases_occupee[j]->position.y);
                    }
                }
            }
    }
printf("===========================================\n\n");
}

void signal_sigterm_serveur(int num_signal) {
    printf("\n[Serveur] Signal reçu (Ctrl-C détecté).\n");
    
    printf("[Serveur] Envoi du signal de terminaison aux parties en cours.\n");
    printf("[Serveur] Arrêt du serveur.\n");
    kill(0, SIGTERM); 
}