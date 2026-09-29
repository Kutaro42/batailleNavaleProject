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

#define NBECHANGE 3

char* id=0;
short port=0;
int sock=0;	/* socket de communication */

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

	while (1) {
		struct  sockaddr_in client; /* SAP du client */
		int     len=sizeof(client);
		int     sock_pipe;          /* socket de dialogue */
		int     ret,nb_question;

    	sock_pipe = accept(sock,(struct sockaddr*)&client,(socklen_t*)&len);
		for ( nb_question=0 ; nb_question<NBECHANGE ; nb_question++) {
			char    buf_read[1<<8], buf_write[1<<8];

			ret=read(sock_pipe,buf_read,sizeof(buf_read));
			if (ret<=0) {
				printf("%s: read=%d: %s\n",argv[0],ret,
					strerror(errno));
				break;
			}
			printf("serveur %s recu de (%s,%4d) : %s\n",id,
				inet_ntoa(client.sin_addr),
				ntohs(client.sin_port),buf_read);
			sprintf(buf_write,"#%2s=%03d#",id,nb_reponse++);
			ret=write(sock_pipe,buf_write,sizeof(buf_write));
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
