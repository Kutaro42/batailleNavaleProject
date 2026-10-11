//serveurt.c (serveur TCP)

#include "serveur_fonction.h"

//VARIABLES GLOBALES
char* id=0;
short port=0;
int sock=0;	/* socket de communication */

_Bool connection_closed = 0; //renseigne l'état de la connection (=indicateur de l'état de connection)


int main(int argc, char** argv)
{
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
		exit(1); 
	}
	struct  sockaddr_in serveur; /* Service Access Point du serveur */

	if (argc!=3) {
		fprintf(stderr,"usage: %s id port\n",argv[0]);
		exit(1);
	}
	id= argv[1];
	port= atoi(argv[2]);

	if ((sock = socket (AF_INET,SOCK_STREAM,0)) == -1) {
		fprintf(stderr,"%s: socket %s\n",argv[0],strerror(errno));
		exit(1);
	}

	serveur.sin_family = AF_INET;
	serveur.sin_port = htons(port);
	serveur.sin_addr.s_addr = INADDR_ANY;
	
	//Ouvrir un descripteur avec le socket configuré
	if (bind (sock,(struct sockaddr*)&serveur,sizeof(serveur))<0) {
		fprintf(stderr,"%s: bind %s\n",argv[0],strerror(errno));
		exit(1);
	}

	//écouter au maximum 5 tentative de communication en simultané
   if (listen (sock,5)!=0) {
		fprintf(stderr,"%s: listen %s\n",argv[0],strerror(errno));
		exit(1);
   }


   //BOUCLE DE CONNECTION (chaque client doit avoir droit à ses 50 coups !)
	while (1) {
		struct  sockaddr_in client; /* SAP du client */
		int     len=sizeof(client);
		int     sock_pipe;          /* socket de dialogue */
		int     ret; //ret lit le format binaire dans laquelle la position reçue est encodée (=plus simple et performant), 

    	sock_pipe = accept(sock,(struct sockaddr*)&client,(socklen_t*)&len); //on accepte le traitement des demande du client courant

		//VÉRIFICATION DE LA LIMITE DE 5 JOUEURS

		//dès qu'un 6eme client tente de se connecter, on rentre dans cette condition :
		if (nb_joueurs_actifs >= 5) {
			printf("[Serveur] Limite de 5 joueurs atteinte. Connexion refusée pour (%s,%4d)\n", 
				inet_ntoa(client.sin_addr), ntohs(client.sin_port));
			
			// fermer direct la socket de ce client
			close(sock_pipe);
			continue; // On retourne au début de la boucle pour en attendre un autre
		}

		// Incrémenter le nombre de joueurs actifs :
		nb_joueurs_actifs++;


		//PARTIE SUBPROCESSING

		pid_t pid = fork(); //subdiviser le programme en sous-processus

		if (pid == -1) {
			// Le clonage a échoué (plus de mémoire, etc.)
			close(sock_pipe);
			
		} else if (pid == 0) {
			// le FILS
			// Tout ce qui est écrit ici sera exécuté uniquement par le clone

			//fermer le socket copié du père :
			close(sock); //le fils sert juste à jouer avec le client, ça lui sert à rien

			//DECLARATION DES VARIABLES GLOBALES DE LA PARTIE

			_Bool fin_de_partie_ =0;
			_Bool victoire_=0;
			_Bool reste_il_un_bateau_=0;

			//CHARGEMENT ET INITIALISATION DES PARAMETRES DE JEU
	
			int nbr_coup_restant_ = TOTAL_COUPS;//nb_coup_restant sert au dénombrement des essais restants

			//liste des bateaux en jeu
			bateau_en_jeu_ liste_des_bateaux;

			// CHARGER LA MAP :

			// La fonction reçoit bien un pointeur (bateau_en_jeu_ *)
			if (generation_des_navires(&liste_des_bateaux) == 0) {
				printf("Les navires ont été générés avec succès !\n");
			}


			if (initialisation_de_la_partie(&liste_des_bateaux) == 0) {
				printf("Les navires ont été positionnés sur la grille avec succès !\n");//on fait toujours référence à une grille, même si elle est fictive
			}


			position_ tir_client; //stocker la position reçu du client

			//créer la structure d'envoi
			ReponseServeur response;

			//JEU
			while (!la_partie_est_elle_finie(&nbr_coup_restant_, &fin_de_partie_, &liste_des_bateaux, &victoire_)) {

				// ON LIT LE MSG DU CLIENT
				ret=read(sock_pipe,&tir_client,sizeof(position_));
				if (ret<=0) {
					printf("%s: read=%d: %s\n",argv[0],ret,
						strerror(errno));
					break;
				}
				printf("serveur %s recu de (%s,%4d)\n", id, inet_ntoa(client.sin_addr), ntohs(client.sin_port));

				//TRAITEMENT DU MSG CLIENT

				//affichage DEBUG
				printf("[Serveur] Tir reçu du joueur ! Coordonnées : X = %d, Y = %d\n", tir_client.x, tir_client.y);


				//vérifier si une case de bateau existe à la position correspondante, si active (renvoi "touché")
				response.resultat =resultat_du_tir(&tir_client, &liste_des_bateaux);
				//(void)res;

				//on finit de remplir la structure d'envoi tout en mettant à jour la variable de jeu, de façon élégante.
				response.coup_restant = dec_nb_coup_restant(&nbr_coup_restant_);


				//A CE STADE, ON vERIFIE QUE LE CLIENT n'A PAS DETRUIT LE DERNIER BATEAU
				if(la_partie_est_elle_finie(&nbr_coup_restant_, &fin_de_partie_, &liste_des_bateaux, &victoire_)){
					if (fin_de_partie_ && victoire_){
						envoyer_code_victoire(&response);
					}
				}
				//--------------------------------------------------------------------------
				// Note : cette logique à ses limites : on réécrit sur la structure les informations de victoire : le client ne peut que supposer qu'il à couler le dernier bateau, car il recevra directement le code de victoire au lieu de son résultat de tir

				// ON RÉPOND AU CLIENT
				//sprintf(buf_write); --> pas besoin de mise en forme

				//on balance directement en binaire la valeur entière de res = TOUCHEE RATEE COULEE
				ret=write(sock_pipe,&response,sizeof(ReponseServeur));

				//si le retour s'est mal passé, on traite l'erreur
				if (ret<=0) {
					printf("%s: write=%d: %s\n",argv[0],ret,
						strerror(errno));
					break;
				}
				sleep(2);
			

			}

			close(sock_pipe);
			//nettoyer la liste des bateaux allouée
			liberation_memoire(&liste_des_bateaux);

			exit(0);//fermer avant que le fils fasse n'importe quoi (remonte dans la main et commence à intercepter les client en se dédoublant)
		
		} else {
			// le PÈRE
			// La variable 'pid' contient l'identifiant du fils qui vient de naître
			close(sock_pipe);

		}

	}
		
	return 0;
}
