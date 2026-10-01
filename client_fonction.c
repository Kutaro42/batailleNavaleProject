#include "client_fonction.h"

int afficher_ecran_titre(){
    return write(1,"\n============================================\n|        BATAILLE NAVALE EN LIGNE !!!       |\n============================================\nEntrer les coordonées de votre premier tir (format XX;YY)\n",197);
}


void prendre_les_entree_clavier(char *buf_write){
    //char input_buff[1<<3]; // on à théoriquement besoin d'entrer seulement 5 caractères mais on peux en rajouter pour + de place pour d'éventuel information supp
		
	//int inputing =read(0,buf_write,sizeof(buf_write));//lit l'entree standard et stocke la chaine traitée dans le buffer write_buf 
	//buf_write[inputing] = '\0'; 

    position_ tir_joueur;
    int x_saisi, y_saisi;
    int saisie_valide = 0;

    // ALGORITHME DE SAISIE ET DE VALIDATION
    while (!saisie_valide) {
        printf("Entrez les coordonnées de votre tir (Ex: X Y -> 5 3) : ");
        // Lecture sécurisée de la ligne tapée au clavier
        if (fgets(buf_write, 20, stdin) == NULL) {
            continue;
        }

        fflush(stdout); // Force l'affichage immédiat du texte à l'écran


        // Éliminer le '\n' à la fin de la chaîne s'il est présent
        buf_write[strcspn(buf_write, "\n")] = '\0';

        // Si l'utilisateur a juste appuyé sur Entrée (chaîne vide), on passe
        if (strlen(buf_write) == 0) {
            continue;
        }
        
        // Extraction des valeurs numériques et validation du format
        // sscanf renvoie le nombre d'éléments correctement trouvés
        if (sscanf(buf_write, "%d;%d", &x_saisi, &y_saisi) == 2) {
            
            // Validation algorithmique locale (on vérifie que le tir est dans les limites de la grille)
            if (x_saisi >= 0 && x_saisi < 50 && y_saisi >= 0 && y_saisi < 50) {
                
                // Remplissage de la structure à partir des valeurs validées
                tir_joueur.x = x_saisi;
                tir_joueur.y = y_saisi;
                saisie_valide = 1; // On valide pour sortir de la boucle
                
            } else {
                printf("Coordonnées hors de la grille (doivent être entre 0 et 49).\n");
            }
        } else {
            printf("Format invalide. Veuillez entrer deux nombres séparés par un point-virgule.\n");
        }
    }

}





_Bool traitement_entree_clavier(char *buff[]){
    //Récuperer le x et le y:
    //short x=atoi(buff[0],buff[1]), y=atoi(buff[3],buff[4]);

}


//vérifie qu'une entrée clavier est valide avant l'envoie au serveur pour traitement
_Bool entree_valide(char *buff){
    //extraire les chiffres :
    //...

    //...

}