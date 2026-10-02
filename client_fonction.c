#include "client_fonction.h"

int afficher_ecran_titre(){
    return write(1,"\n============================================\n|        BATAILLE NAVALE EN LIGNE !!!       |\n============================================\nEntrer les coordonées de votre premier tir (format XX;YY)\n",197);
}

_Bool prendre_les_entree_clavier(char *buf_write,position_ *tir_joueur){
    //char input_buff[1<<3]; // on à théoriquement besoin d'entrer seulement 5 caractères mais on peux en rajouter pour + de place pour d'éventuel information supp
		
	//int inputing =read(0,buf_write,sizeof(buf_write));//lit l'entree standard et stocke la chaine traitée dans le buffer write_buf 
	//buf_write[inputing] = '\0'; 

    // ALGORITHME DE SAISIE ET DE VALIDATION
    while (traitement_entree_clavier(buf_write,tir_joueur)) {
        printf("Entrez les coordonnées de votre tir (Ex: X Y -> 5 3) : ");
        // Lecture sécurisée de la ligne tapée au clavier
        if (fgets(buf_write, 20, stdin) == NULL) { //j'ai mit 20 au cas ou, mais dans le traitement cela n'aura aucune importance, car on ne se préoccupe que des 6 premiers caractères.
            continue;
        }

    }
    return 1;

}


_Bool traitement_entree_clavier(char *buff,position_ *pos){

 
    fflush(stdout); // Force l'affichage immédiat du texte à l'écran


    // Éliminer le '\n' à la fin de la chaîne s'il est présent
    buf_write[strcspn(buf_write, "\n")] = '\0';

    // Si l'utilisateur a juste appuyé sur Entrée (chaîne vide), on passe
    if (strlen(buf_write) == 0) {
        continue;
    }

    //On vérifie que l'entrée est traitable. si oui, on la parque dans une struct position et on l'envoie sous forme binaire au serveur pour un traitement plus efficace
    return entree_valide(buff,pos);

}


//vérifie qu'une entrée clavier est valide avant l'envoie au serveur pour traitement
_Bool entree_valide(char *buff,position_ *pos){

    //def des variables d'usage :
    int x_saisi, y_saisi;
    _Bool saisie_valide = 0;

    //Récuperer le x et le y:
    // Extraction des valeurs numériques et validation du format
    // sscanf renvoie le nombre d'éléments correctement trouvés
    if (sscanf(buf_write, "%d;%d", &x_saisi, &y_saisi) == 2) {
        
        // Validation algorithmique locale (on vérifie que le tir est dans les limites de la grille)
        if (x_saisi >= 0 && x_saisi < 50 && y_saisi >= 0 && y_saisi < 50) {
            
            // Remplissage de la structure à partir des valeurs validées
            *pos.x = x_saisi;
            *pos.y = y_saisi;
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
    
}