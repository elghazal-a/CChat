#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

#define BUFFER_SIZE 256

int cree_socket_tcp_client(int argc, char** argv);

int main(int argc, char** argv)
{
	char bufferR[BUFFER_SIZE];
	char bufferW[BUFFER_SIZE];

	int sockClient = cree_socket_tcp_client(argc, argv);

	if (sockClient < 0)
	{
		fprintf(stderr, "Erreur connection socket client\n");
		exit(1);
	}

	fd_set rdfs;
	struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
	while(1)
	{
		FD_ZERO(&rdfs);
		FD_SET(0, &rdfs);
		FD_SET(sockClient, &rdfs);

		if(select(sockClient + 1, &rdfs, NULL, NULL, &tv) > 0)
		{
			// Un des descripteurs a changé

			if(FD_ISSET(0, &rdfs))
			{
				if (fgets(bufferW, BUFFER_SIZE, stdin) != NULL)
				{
					bufferW[strlen(bufferW)-1] = '\0';
					send(sockClient, bufferW, BUFFER_SIZE, 0);
				}
			}
			else if(FD_ISSET(sockClient, &rdfs))
			{
				if (recv(sockClient, bufferR, BUFFER_SIZE, 0) != 0)
				{
					printf("%s\n", bufferR);
				}
				
			}
		}
		else{} //timeout

	}

	return 0;
}


int cree_socket_tcp_client(int argc, char** argv)
{
	int sockClient;
	struct sockaddr_in adresse;

	if (argc != 3)
	{
		printf("Usage : %s adresse port\n", argv[0]);
		exit(0);
	}


	if( (sockClient= socket(AF_INET,SOCK_STREAM, 0)) < 0 ) 
	{
		fprintf(stderr, "Erreur socket\n");
		return -1;
	}

	adresse.sin_family = AF_INET;
	adresse.sin_port = htons(atoi(argv[2]));
	inet_aton(argv[1], &adresse.sin_addr);

	if (connect(sockClient, (struct sockaddr*) &adresse, sizeof(struct sockaddr_in)) < 0)
	{
		close(sockClient);
		fprintf(stderr, "Erreur connect\n");
		return -1;
	}

	return sockClient;
}