#include "client_fonction.h"

void afficher_ecran_titre(){
    printf("\n============================================\n|        BATAILLE NAVALE EN LIGNE !!!       |\n============================================\nEntrer les coordonées de votre premier tir (format XX;YY)\n");
}

void afficher_ecran_fin(_Bool * victoire_){

    if(!(*victoire_)){
        printf("\n============================================\n");
        printf("|  PARTIE TERMINÉE ! Plus de coups restants. |\n");
        printf("============================================\n");
    } else {
        printf("\n============================================\n");
        printf("|  PARTIE TERMINÉE ! VOUS AVEZ GAGNÉ  !!!    |\n");
        printf("============================================\n");
    }
}

_Bool prendre_les_entree_clavier(char *buf_write,position_t *tir_joueur){

    _Bool saisie_correcte = 0; //au départ, on par du principe que c'est incorrecte
    // ALGORITHME DE SAISIE ET DE VALIDATION
    do{
        printf("Entrez les coordonnées de votre tir (Ex: X Y -> 5 3) : ");
        // Lecture sécurisée de la ligne tapée au clavier
        if (fgets(buf_write, 20, stdin) == NULL) { //j'ai mit 20 au cas ou, mais dans le traitement cela n'aura aucune importance, car on ne se préoccupe que des 6 premiers caractères.
            return 1;
        }
        saisie_correcte = traitement_entree_clavier(buf_write,tir_joueur);

    }while (!saisie_correcte); //on choisit le do...while pour permettre de prendre la toute p^remière entrée de l'utilisateur avant de lancer la vérif 
    return 0;

}

//vérifie qu'une entrée clavier est valide avant l'envoie au serveur pour traitement
_Bool entree_valide(char *buf_write,position_t *pos){

    //def des variables d'usage :
    int x_saisi, y_saisi;
    _Bool saisie_valide = 0;

    //Récuperer le x et le y:
    // Extraction des valeurs numériques et validation du format
    // sscanf renvoie le nombre d'éléments correctement trouvés
    if (sscanf(buf_write, "%d;%d", &x_saisi, &y_saisi) == 2) { //scanf est ultra utile pour parser/extraire des info dans du texte 
                                                               //int sscanf(const char *str, const char *format, ...);
        
        // Validation algorithmique locale (on vérifie que le tir est dans les limites de la grille)
        if (x_saisi >= 0 && x_saisi < TAILLE_GRILLE && y_saisi >= 0 && y_saisi < TAILLE_GRILLE) {
            
            // Remplissage de la structure à partir des valeurs validées
            pos->x = x_saisi;
            pos->y = y_saisi;
            saisie_valide = 1; // On valide pour sortir de la boucle
            
        } else {
            printf("Coordonnées hors de la grille (doivent être entre 0 et 49).\n");
        }
    } else {
        printf("Format invalide. Veuillez entrer deux nombres séparés par un point-virgule.\n");

        //Méthode bourrine pour vider le buffer jusqu'a ce qu'il n'y ait plus de caractères-poubelles dans le buffer-tampon (=éviter de faire crash la saisie quand un joueur écrit de très longue chaines)
        if (strchr(buf_write, '\n') == NULL) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF); // Tant que le caractère pris dans le buffer n'est pas un saut de ligne ou un jeton EOF (=fin du msg quand le joueur valide en appuyant sur la touche entrée)
        }
        
    }
    return saisie_valide;
}
_Bool traitement_entree_clavier(char *buf_write,position_t *pos){

 
    fflush(stdout); // Force l'affichage immédiat du texte à l'écran


    // Éliminer le '\n' à la fin de la chaîne s'il est présent
    buf_write[strcspn(buf_write, "\n")] = '\0'; //pratique pour retourner la position de la première occurence d'un \n et la remplacer par un 0 binaire
    //size_t strcspn(const char *s, const char *reject);

    //On vérifie que l'entrée est traitable. si oui, on la parque dans une struct position et on l'envoie sous forme binaire au serveur pour un traitement plus efficace
    return entree_valide(buf_write,pos);

}

//prend la réponse serveur et l'adresse d'un buffer en paramètre et le formatte en fonction des données de jeu
_Bool interpreter_la_reponse_serveur(char * buff, reponse_serveur_t * reponse,_Bool * victoire){
    //préparer le stockage des valeurs
    int size =256;//on balance une taille conséquente pour éviter l'overloading du buffer, de toute façon snprintf gère ça tout seul sur ce qui sera réellement écrit

    //interpréter le résultat du tir
    
    switch (reponse->resultat)
    {
    case 0: //raté
  
        snprintf(buff,size,"RATÉ, Coup Restant : %d",reponse->coup_restant); //ici pas besoin de forcer le zéro de fin de chaine (contrairement à sprintf classique)
        break;
    case 1: //touché
        snprintf(buff,size,"TOUCHÉ, Coup Restant : %d",reponse->coup_restant);
        break;

    case 3:// gagné
        snprintf(buff,size,"VICTOIRE !!! Vous avez gagné ! Il vous restait %d coups à jouer",reponse->coup_restant);// le case 3 est spécial : il n'existe aucun enum possèdant la valeur 3 coté serveur, car il s'agit du code victoire
        *victoire=1;
        break;
    default: //coulé
        snprintf(buff,size,"COULÉ, Coup Restant : %d",reponse->coup_restant);
        break;
    }
    
    return 1;
}
