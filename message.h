#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define MAX_CLIENTS 255
#define LONG_MSG 256
#define NBR_MSG_MAX 1000

typedef enum {false=0, true=1} boolean;

typedef struct Messages Messages;

struct Messages
{
	int sock;
	char msg[LONG_MSG];
	int hours;
	int min;
	int sec;
};


typedef struct Clients Clients;

struct Clients
{
	int sock;
	char pseudo[LONG_MSG];
	boolean initie;
	boolean libre;
};








#endif // MESSAGE_H_INCLUDED