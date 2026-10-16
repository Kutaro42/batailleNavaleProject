//serveurt.c (serveur TCP)

#include "serveur_fonction.h"

//VARIABLES GLOBALES (static pour les contraindre à une portée de ce fichier uniquement)
static char* id = 0;
static short port = 0;
static int socket_principal = 0;	/* socket de communication */




int main(int argc, char** argv)
{

	// Association du signal SIGINT au gestionnaire du serveur
    if (signal(SIGINT, signal_sigterm_serveur) == SIG_ERR) {
        perror("Erreur du signal ");
        exit(EXIT_FAILURE);
	}


    // Configuration de la structure signal
    struct sigaction signal_d_action;

	//associer notre fonction en tant que controleur de signaux 
	signal_d_action.sa_handler = gestionnaire_signaux_fils; 

	//on nettoie l'éventuelle garbage value qui pourrait créer des bug (= vider la table de filtrage de signaux de notre structure)
	sigemptyset(&signal_d_action.sa_mask);

	//on ajoute le flag SA_RESTART pour éviter de bloquer le père dans son appel système du accept() et de faire planter la boucle de connexion des nouveaux joueur
	signal_d_action.sa_flags = SA_RESTART;

	//si sigaction renvoie le code de sortie -1, il y a un problème
	if (sigaction(SIGCHLD, &signal_d_action, NULL) == -1) { // int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
		fprintf(stderr, "Erreur sigaction\n"); //on signale l'erreur sur la sortie d'erreur standard
		exit(EXIT_FAILURE); 
	}
	struct  sockaddr_in serveur; /* Service Access Point du serveur */

	if (argc!=3) {
		fprintf(stderr,"usage: %s id port\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	id= argv[1];
	port= atoi(argv[2]);

	if ((socket_principal = socket (AF_INET,SOCK_STREAM,0)) == -1) {
		fprintf(stderr,"%s: socket %s\n",argv[0],strerror(errno));
		exit(EXIT_FAILURE);
	}

	serveur.sin_family = AF_INET;
	serveur.sin_port = htons(port);
	serveur.sin_addr.s_addr = INADDR_ANY;
	
	//Ouvrir un descripteur avec le socket configuré
	if (bind (socket_principal,(struct sockaddr*)&serveur,sizeof(serveur))<0) {
		fprintf(stderr,"%s: bind %s\n",argv[0],strerror(errno));
		exit(EXIT_FAILURE);
	}

	//écouter au maximum 5 tentative de communication en simultané
   if (listen (socket_principal,5)!=0) {
		fprintf(stderr,"%s: listen %s\n",argv[0],strerror(errno));
		exit(EXIT_FAILURE);
   }


   //BOUCLE DE CONNECTION (chaque client doit avoir droit à ses 50 coups !)
	while (1) {
		struct  sockaddr_in client; /* SAP du client */
		int     longeur_du_socket=sizeof(client);
		int     socket_dialogue_fils;          /* socket de dialogue */
		int     ret; //ret lit le format binaire dans laquelle la position reçue est encodée (=plus simple et performant), 

    	socket_dialogue_fils = accept(socket_principal,(struct sockaddr*)&client,(socklen_t*)&longeur_du_socket); //on accepte le traitement des demande du client courant

		//VÉRIFICATION DE LA LIMITE DE 5 JOUEURS

		//dès qu'un 6eme client tente de se connecter, on rentre dans cette condition :
		if (nb_joueurs_actifs >= 5) {
			printf("[Serveur] Limite de 5 joueurs atteinte. Connexion refusée pour (%s,%4d)\n", 
				inet_ntoa(client.sin_addr), ntohs(client.sin_port));
			
			// fermer direct la socket de ce client
			close(socket_dialogue_fils);
			continue; // On retourne au début de la boucle pour en attendre un autre
		}

		// Incrémenter le nombre de joueurs actifs :
		nb_joueurs_actifs++;


		//PARTIE SUBPROCESSING

		pid_t pid = fork(); //subdiviser le programme en sous-processus

		if (pid == -1) {
			// Le clonage a échoué (plus de mémoire, etc.)
			close(socket_dialogue_fils);
			
		} else if (pid == 0) {
			// le FILS
			// Tout ce qui est écrit ici sera exécuté uniquement par le clone
			//fermer le socket copié du père :
			close(socket_principal); //le fils sert juste à jouer avec le client, ça lui sert à rien
			
			signal(SIGINT, SIG_IGN);
			signal(SIGTERM, signal_sigterm_fils);
			//DECLARATION DES VARIABLES GLOBALES DE LA PARTIE

			_Bool fin_de_partie_ = 0;
			_Bool victoire_ = 0;

			//CHARGEMENT ET INITIALISATION DES PARAMETRES DE JEU
	
			int nbr_coup_restant_ = TOTAL_COUPS;//nb_coup_restant sert au dénombrement des essais restants

			//liste des bateaux en jeu
			bateau_en_jeu_t liste_des_bateaux;

			// CHARGER LA MAP :

			// La fonction reçoit bien un pointeur (bateau_en_jeu_t *)
			if (generation_des_navires(&liste_des_bateaux) == 0) {
				printf("Les navires ont été générés avec succès !\n");
			}


			if (initialisation_de_la_partie(&liste_des_bateaux) == 0) {
				printf("Les navires ont été positionnés sur la grille avec succès !\n");//on fait toujours référence à une grille, même si elle est fictive
				afficher_grille_triche(&liste_des_bateaux);
			}


			position_t tir_client; //stocker la position reçu du client

			//créer la structure d'envoi
			reponse_serveur_t reponse;

			//JEU
			while (!la_partie_est_elle_finie(&nbr_coup_restant_, &fin_de_partie_, &liste_des_bateaux, &victoire_)) {

				// ON LIT LE MSG DU CLIENT
				ret=read(socket_dialogue_fils,&tir_client,sizeof(position_t));
				if (ret< 0) {
					printf("%s: read=%d: %s\n",argv[0],ret,
						strerror(errno));
					break;
				}
				else if (ret == 0) {
    				// Si le client s'est déconnecté proprement (ret == 0), on ne fait RIEN (pas de printf)
    				break;
				}
				
				printf("serveur %s recu de (%s,%4d)\n", id, inet_ntoa(client.sin_addr), ntohs(client.sin_port));

				//TRAITEMENT DU MSG CLIENT

				//affichage DEBUG
				printf("[Serveur] Tir reçu du joueur ! Coordonnées : X = %d, Y = %d\n", tir_client.x, tir_client.y);


				//vérifier si une case de bateau_t existe à la position correspondante, si active (renvoi "touché")
				reponse.resultat =resultat_du_tir(&tir_client, &liste_des_bateaux);
				//(void)res;

				//on finit de remplir la structure d'envoi tout en mettant à jour la variable de jeu, de façon élégante.
				reponse.coup_restant = dec_nb_coup_restant(&nbr_coup_restant_);


				//A CE STADE, ON vERIFIE QUE LE CLIENT n'A PAS DETRUIT LE DERNIER bateau_t
				if(la_partie_est_elle_finie(&nbr_coup_restant_, &fin_de_partie_, &liste_des_bateaux, &victoire_)){
					envoyer_code_victoire_defaite(&reponse,&fin_de_partie_,&victoire_);
				}
				//--------------------------------------------------------------------------
				// Note : cette logique à ses limites : on réécrit sur la structure les informations de victoire : le client ne peut que supposer qu'il à couler le dernier bateau_t, car il recevra directement le code de victoire au lieu de son résultat de tir

				//on balance directement en binaire la valeur entière de res = TOUCHEE RATEE COULEE
				ret=write(socket_dialogue_fils,&reponse,sizeof(reponse_serveur_t));

				//si le retour s'est mal passé, on traite l'erreur
				if (ret <= 0) {
					printf("%s: write=%d: %s\n",argv[0],ret,
						strerror(errno));
					break;
				}
				sleep(2);
			

			}

			close(socket_dialogue_fils);
			//nettoyer la liste des bateaux allouée
			liberation_memoire(&liste_des_bateaux);
			//msg dans la console pour signifier la mort du fils après que la partie joué avec sont client soit interrompue ou finie
			printf("Fin de la partie avec le client(%s,%4d)\n", 
				inet_ntoa(client.sin_addr), ntohs(client.sin_port));

			exit(EXIT_SUCCESS);//fermer avant que le fils fasse n'importe quoi (remonte dans la main et commence à intercepter les client en se dédoublant)	
	
		} else {
			// le PÈRE
			// La variable 'pid' contient l'identifiant du fils qui vient de naître
			close(socket_dialogue_fils);

		}

	}
		
	return 0;
}
