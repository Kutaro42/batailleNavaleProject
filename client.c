// clientt.c (client TCP)

#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>



#define NBECHANGE 3

char* id=0;
short sport=0;
bool connection_closed = 0; //renseigne l'état de la connection (=indicateur de l'état de connection en tps réel)

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

    //Tant que la connection n'est pas interrompue (par un code de sortie du serveur), répéter :
	while(!connection_closed) {
		char    buf_read[1<<8], buf_write[1<<8];

		sprintf(buf_write,"[coordonées]",id,nb_question); //coordonées x;y
		printf(" client %2s: (%s:%4d) envoie a  ", id,      //debug
			inet_ntoa(moi.sin_addr),ntohs(moi.sin_port));   //debug
		printf("(%s:%4d) : %s\n",                           //debug
			inet_ntoa(serveur.sin_addr),ntohs(serveur.sin_port),
			buf_write);
		ret=write(sock,buf_write,len); //on écrit dans le descripteur standard du socket client (moi)
		if (ret<=strlen(buf_write)) {
			printf("\n%s: erreur dans write (num=%d, mess=%s)\n",
				argv[0],ret,strerror(errno));
			continue;
		}
		printf(" client %2s: (%s:%4d) recoit de ",id, 
				inet_ntoa(moi.sin_addr),ntohs(moi.sin_port));
		ret=read(sock,buf_read,len);	//on lit dans le descripteur standard du socket serveur (serveur)
		if (ret<=0) {
			printf("\n%s:  erreur dans read (num=%d, mess=%s)\n",
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
