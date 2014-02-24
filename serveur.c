#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "message.h"

#define BUFFER_SIZE 256

#define MAX(a,b) (((a)>(b))?(a):(b))

int cree_socket_tcp_ip();
void creer_client(int sockClient, struct Clients* clients, int* nbrClient, fd_set* rdfs, int* maxfd);
void afficherClients(struct Clients* clients, int nbrClient);
void broadcast(struct Clients* clients, int nbrClient, int from, char* buffer);

int main(int argc, char const *argv[])
{
	int sockServeur, sockClient, maxfd, i;

	ssize_t length;

	char bufferR[LONG_MSG];
	Clients clients[MAX_CLIENTS];
	Messages msgs[NBR_MSG_MAX];
	int nbrMessage = 0;
	int nbrClient = 0;

	struct sockaddr_in adresse;
	socklen_t longueur = sizeof(struct sockaddr_in);
	pid_t pid_fils;

	sockServeur = cree_socket_tcp_ip();
	if (sockServeur < 0)
		return -1;


	listen(sockServeur, MAX_CLIENTS);



	fd_set rdfs;
	struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
	FD_ZERO(&rdfs);
	//FD_SET(0, &rdfs);  Desactiver l'ecoute sur l'entrée standard
	FD_SET(sockServeur, &rdfs);
	maxfd = sockServeur;

	while(1)
	{
		fd_set fdset = rdfs;
		if(select(maxfd + 1, &fdset, NULL, NULL, &tv) > 0)
		{
			// Un des descripteurs a changé 
			// Soit une connexion au serveur soit un message d'un client

			if(FD_ISSET(sockServeur, &fdset))
			{
				// Un nouveau client
				sockClient = accept(sockServeur, (struct sockaddr*) &adresse, &longueur);
				creer_client(sockClient, clients, &nbrClient, &rdfs, &maxfd);
				send(sockClient, "Pseudo: ", 9, 0);
			}
			else
			{
				for (i = 0; i < nbrClient; i++)
				{
					if(!clients[i].libre)
					{
						int fd = clients[i].sock;
						if(FD_ISSET(fd, &fdset))
						{
							if (recv(fd, bufferR, LONG_MSG, 0) != 0)
							{
								if(clients[i].initie)
								{
									broadcast(clients, nbrClient, i, bufferR);
								}
								else
								{
									strcpy(clients[i].pseudo, bufferR);
									clients[i].initie = true;
									send(fd, "-------Bienvenue au chat------- ", 32, 0);
									//afficherClients(clients, nbrClient);
								}
							}

						}

					}
				}
			}

		}
		else{} //timeout
	}

}



int cree_socket_tcp_ip()
{
	int sockServeur;
	struct sockaddr_in adresse;

	if( (sockServeur = socket(AF_INET,SOCK_STREAM, 0)) < 0 ) 
	{
		fprintf(stderr, "Erreur socket\n");
		return -1;
	}

	adresse.sin_family = AF_INET;
	adresse.sin_addr.s_addr = INADDR_ANY;
	adresse.sin_port = htons(3232);

	if( bind( sockServeur, (struct sockaddr*) &adresse, sizeof(struct sockaddr_in) ) < 0 )
	{
		printf(" bind error\n");
		exit(1);
	}
	return sockServeur;
}


void creer_client(int sockClient, struct Clients* clients, int* nbrClient, fd_set* rdfs, int* maxfd)
{
	clients[*nbrClient].sock = sockClient;
	clients[*nbrClient].initie = false;
	clients[*nbrClient].libre = false;
	*nbrClient = *nbrClient + 1;

	FD_SET(sockClient, rdfs);
	*maxfd = MAX(*maxfd,sockClient);
}

void afficherClients(struct Clients* clients, int nbrClient)
{
	int i;

	printf("----------------La liste des clients---------------\n");
	for (i = 0; i < nbrClient; i++)
	{
		if(!clients[i].libre && clients[i].initie)
			printf("Client: %d, socket: %d, initie: %d\n", i, clients[i].sock, clients[i].initie);
	}
	printf("---------------------------------------------------\n");
}

void broadcast(struct Clients* clients, int nbrClient, int from, char* buffer)
{
	int i;
	//construire le message
	char message[LONG_MSG];
	strcpy(message, clients[from].pseudo);
	strcat(message, " > ");
	strcat(message, buffer);

	for (i = 0; i < nbrClient; i++)
	{
		if(!clients[i].libre &&  clients[i].initie && i != from)
		{
			send(clients[i].sock, message, LONG_MSG, 0);
		}
	}
}