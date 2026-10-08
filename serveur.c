//serveurt.c (serveur TCP)

#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>       // Pour printf, fprintf, sprintf
#include <stdlib.h>      // Pour exit, atoi
#include <string.h>      // Pour strerror
#include <errno.h>       // Pour errno
#include <unistd.h>      // Pour read, write, close, sleep
#include <sys/types.h>   // Pour les définitions de types système (socklen_t)
#include <sys/socket.h>  // Pour socket, bind, listen, accept, AF_INET, SOCK_STREAM
#include <netinet/in.h>  // Pour sockaddr_in, htons, ntohs, INADDR_ANY
#include <arpa/inet.h>   // Pour inet_ntoa

#include "serveur_fonction.h"

#define NBECHANGE 3

char* id=0;
short port=0;
int sock=0;	/* socket de communication */

_Bool connection_closed = 0; //renseigne l'état de la connection (=indicateur de l'état de connection)
int  nb_reponse=0;

int main(int argc, char** argv)
{
struct  sockaddr_in serveur; /* SAP du serveur */

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
	if (bind (sock,(struct sockaddr*)&serveur,sizeof(serveur))<0) {
		fprintf(stderr,"%s: bind %s\n",argv[0],strerror(errno));
		exit(1);
	}

   if (listen (sock,5)!=0) {
		fprintf(stderr,"%s: listen %s\n",argv[0],strerror(errno));
		exit(1);
   }

   //CHARGEMENT ET INITIALISATION DES PARAMETRES DE JEU
   
   //liste des bateaux en jeu
   bateau_en_jeu_ liste_des_bateaux;

   // CHARGER LA MAP :

    // 2. On donne l'ADRESSE de la structure avec le symbole '&'
    // La fonction reçoit bien un pointeur (bateau_en_jeu_ *)
    if (generation_des_navires(&liste_des_bateaux) == 0) {
        printf("Les navires ont été générés avec succès !\n");
    }
   //int grille_de_jeu_[50][50];
   //int grille_de_jeu_[50][50] =malloc(sizeof())
   // 2. Déclarer la grille de jeu et positionner les navires !
    _Bool *grille_de_jeu_[50][50]; 
    if (initialisation_de_la_partie(grille_de_jeu_, &liste_des_bateaux) == 0) {
        printf("Les navires ont été positionnés sur la grille avec succès !\n");
    }



   //generation_des_navires(bateau_en_jeu_ *liste_de_bateau);
   //initialisation_de_la_partie(_Bool * grille_de_jeu_[50][50]);



   //BOUCLE DE CONNECTION & JEU
	while (1) {
		struct  sockaddr_in client; /* SAP du client */
		int     len=sizeof(client);
		int     sock_pipe;          /* socket de dialogue */
		int     ret,nb_question; //ret lit le format binaire dans laquelle la position reçue est encodée (=plus simple et performant), nb_question sert au dénombrement des essais restants

    	sock_pipe = accept(sock,(struct sockaddr*)&client,(socklen_t*)&len); //on accepte le traitement des demande du client courant

		position_ tir_client; //stocker la position reçu du client

		//JEU
		while (!la_partie_est_elle_finie(&nbr_coup_restant_, &fin_de_partie_, &liste_des_bateaux)) {
			char    buf_read[5], buf_write[25]="[STATUT ; COUPS RESTANT]\0";// pour nos premiers test 24 caractère sont suffisant, mais on se garde la possibilité d'en rajouter 1 pour le \0


			//buf_read[ret] = '\0'; //POUR GERER LA DECOUPE DU MESSAGE


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
			int res =resultat_du_tir(&tir_client, &liste_des_bateaux);
			//(void)res;


			//--------------------------------------------------------------------------
			

			// ON RÉPOND AU CLIENT
			//sprintf(buf_write); --> pas besoin de mise en forme

			//on balance directement en binaire la valeur entière de res = TOUCHEE RATEE COULEE
			ret=write(sock_pipe,&res,sizeof(res));

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
	}
	return 0;
}
