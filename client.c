// clientt.c (client TCP)

#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "client_fonction.h"


#define NBECHANGE 3

char* id=0;
short sport=0;
_Bool connection_closed = 0; //renseigne l'état de la connection (=indicateur de l'état de connection en tps réel)

int sock=0;	/* socket de communication */

int main(int argc, char** argv)
{
struct  sockaddr_in moi; /* SAP du client ---> service accès point */
struct  sockaddr_in serveur; /* SAP du serveur */
int nb_question=0;
int ret,len;

	if (argc!=4) {
		fprintf(stderr,"usage: %s id serveur port\n",argv[0]); //si il n'ya pas 4 arg, rediriger vers la sortie d'erreur le msg et quitter avec un code 1
		exit(1);
	}
	id= argv[1];	//id=argument 1
	sport= atoi(argv[3]); // prendre le 3eme arg et le convertir en valeur entière

	if ((sock = socket (AF_INET,SOCK_STREAM,0)) == -1) { //configurer un socket en ipv4, avec un TCP, avec la valeur par défaut 0;
		fprintf(stderr,"%s: socket %s\n",argv[0],
			strerror(errno));
		exit(1); //si le programme rentre dans ce if, la création du socket s'est mal passée
	}
	serveur.sin_family = AF_INET; //configuration du protocole de communication IPv4
	serveur.sin_port = htons(sport); // direction du port de sortie (s) (renversé dans l'ordre Big endian pour protocole réseau)
	inet_aton(argv[2],&serveur.sin_addr); //communique avec l'interface serveur
	
	if (connect(sock,(struct sockaddr *)&serveur,sizeof(serveur))<0) { //si la connection échoue, traiter l'erreur
		fprintf(stderr,"%s: connect %s\n",argv[0],strerror(errno));
		perror("bind");
		exit(1);
	}
	len=sizeof(moi); 
	getsockname(sock,(struct  sockaddr  *)&moi,&len); //on met à jour les information du socket


	//AFFICHER L'ÉCRAN TITRE DU JEU :
	int titre =afficher_ecran_titre();
	



    //Tant que la connection n'est pas interrompue (par un code de sortie du serveur), répéter :
	while(!connection_closed) {
		char    buf_read[1<<8], buf_write[6];// 5 caractères en réalité, car le 6ème est remplacé par un zéro (ligne 62) pour gérer la fin de chaine

		// PRISE D'ENTREE CLAVIER POUR LE JOUEUR
		//on prend le flux clavier et on en écrit le contenu dans le buffer
		//forme de l'input : [x;y]
		
		position_ position_visee;
		if(prendre_les_entree_clavier(buf_write,&position_visee)){
			continue;
		}

		//	REMARQUE IMPORTANTE : -----------------------------------------------------------------------------------------------------------------------------------------------------------
		//	- Dans l'état actuel, la lecture de l'entrée utilisateur se heurte à un problème majeur : 
		//	Au delà de 5 caractères écrit dans le flux standard, le surplus reste en mémoire-tampon et le système l'envoie automatiquement pour remplir les itération suivante du buffer.
		//	Ce comportement implique un découpage forcé qui oblige le client à envoyer ces caractères inutiles dans des requêtes à la chaine, ce qui pollue le flux et paralyse le serveur.
		//	Pour le moment nous nous concentrerons sur le fonctionnement pur de la connection, mais la robustesse du code impliquerait une boucle de vidage qui lit les caractère poubelle et
		//	les place dans un buffer de vidage avant de vider ce buffer. Le principe étant de vider et remplir le buffer poubelle de ces caractères jusqu'a épuisement de ces derniers.
		// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


		
		//int writting =write(,input_buff,sizeof(inputing));//on fait la taille de inputing pour lire le nbre de caractères réellement lus.

        // PARTIE GESTION CLIENT |==> on vise à long terme une gestion sécurisé des entrées clavier du joueur, pour éviter et empêcher les erreurs de saisie avant qu'elles soient envoyées au serveur.

		//sprintf(buf_write,":%s",id); //coordonées x;y ==> temporairement 'buf_write' pour vérifier, résultat de la couche de sécurité dans un second temps.
		printf(" client %2s: (%s:%4d) envoie a  ", id,      //debug
			inet_ntoa(moi.sin_addr),ntohs(moi.sin_port));   //debug
		printf("(%s:%4d) : %s\n",                           //debug
			inet_ntoa(serveur.sin_addr),ntohs(serveur.sin_port),
			buf_write);
		ret=write(sock,buf_write,strlen(buf_write)); //on écrit dans le descripteur standard du socket client (moi)
		if (ret<strlen(buf_write)) {
			printf("\n%s: erreur dans write (num=%d, mess=%s)\n",
				argv[0],ret,strerror(errno));
			continue;
		}

        // PARTIE GESTION SERVEUR

		printf(" client %2s: (%s:%4d) recoit de ",id, 
				inet_ntoa(moi.sin_addr),ntohs(moi.sin_port));
		ret=read(sock,buf_read,sizeof(buf_read));	//on lit dans le descripteur standard du socket serveur (serveur)
		if (ret<=0) {
			printf("\n%s:  erreur dans read (num=%d, mess=%s)\n",// on traite l'erreur en cas d'échec
				argv[0],ret,strerror(errno));
			continue;
		}
		printf("(%s:%4d) : %s\n",inet_ntoa(serveur.sin_addr),
			ntohs(serveur.sin_port),buf_read);
	}
	close(sock);
	printf(" j ai fini, bye\n");
	return 0;
}
