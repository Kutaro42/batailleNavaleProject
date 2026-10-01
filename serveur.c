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

   // CHARGER LA MAP :



   //BOUCLE DE CONNECTION & JEU
	while (1) {
		struct  sockaddr_in client; /* SAP du client */
		int     len=sizeof(client);
		int     sock_pipe;          /* socket de dialogue */
		int     ret,nb_question;

    	sock_pipe = accept(sock,(struct sockaddr*)&client,(socklen_t*)&len); //on accepte le traitement des demande du client courant


		//JEU
		while (!la_partie_est_elle_finie(&nbr_coup_restant_, &fin_de_partie_)) {
			char    buf_read[5], buf_write[24]="[STATUT ; COUPS RESTANT]";// pour nos premiers test 24 caractère sont suffisant, mais on se garde la possibilité d'en rajouter


			buf_read[ret] = '\0'; //POUR GERER LA DECOUPE DU MESSAGE


			// ON LIT LE MSG DU CLIENT
			ret=read(sock_pipe,buf_read,sizeof(buf_read));
			if (ret<=0) {
				printf("%s: read=%d: %s\n",argv[0],ret,
					strerror(errno));
				break;
			}
			printf("serveur %s recu de (%s,%4d) : %s\n",id,
				inet_ntoa(client.sin_addr),
				ntohs(client.sin_port),buf_read);

			//TRAITEMENT DU MSG CLIENT

			//1) récupérer les deux valeurs numérique séparée par un point-virgule
			//2) vérifier si une case de bateau existe à la position correspondante, si active (renvoi "touché")
			//3) démarrer le système de questions-réponses client-serveur :








			//--------------------------------------------------------------------------
			

			// ON RÉPOND AU CLIENT
			//sprintf(buf_write); --> pas besoin de mise en forme
			ret=write(sock_pipe,buf_write,sizeof(buf_write));

			//si le retour s'est mal passé, on traite l'erreur
			if (ret<=0) {
				printf("%s: write=%d: %s\n",argv[0],ret,
					strerror(errno));
				break;
			}
			sleep(2);
		}
		close(sock_pipe);
	}
	return 0;
}
