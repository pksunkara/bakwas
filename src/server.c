#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "constants.h"

struct client {
	struct sockaddr_in addr;
	int running;
};

int sendSock, recvSock;

void die(char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

int initClient(struct sockaddr_in c, struct client *cs) {
	int i=0,flag=0;
	for(;i<CLIENTS;i++) {
		if(cs[i].running==0) {
			cs[i].addr = c;
			cs[i].running = 1;
			flag = 1;
			break;
		}
	}
	return flag;
}

void killClient(struct sockaddr_in c, struct client *cs) {
	int i=0;
	for(;i<CLIENTS;i++) {
		if(cs[i].running==1 && strcmp(inet_ntoa(c.sin_addr),inet_ntoa(cs[i].addr.sin_addr))==0) {
			cs[i].running = 0;
		}
	}
	return;
}

void echo(char *msg, char *code, struct client *c) {
	int i=0;
	char *buf = malloc((strlen(msg)+10)*sizeof(char));
	strcat(buf,code);
	strcat(buf,msg);
	for(;i<CLIENTS && c[i].running==1;i++) {
		sendto(sendSock, buf, CHATMAX, 0, (struct sockaddr *)&(c[i].addr), sizeof(c[i].addr));
	}
	printf("%s\n",buf);
	free(buf);
	return;
}

void sendClient(char *msg, char *code, struct sockaddr_in c) {
	char *buf = malloc((strlen(msg)+10)*sizeof(char));
	strcat(buf,code);
	strcat(buf,msg);
	sendto(sendSock, buf, CHATMAX, 0, (struct sockaddr *)&c, sizeof(c));
	free(buf);
	return;
}

int main(int argc, char **argv) {
	int i, msgLen;
	char msg[CHATMAX], code[CODELENGTH+1];
	unsigned int clientAddrSize;
	struct client clients[CLIENTS];
	struct sockaddr_in servAddr, clientAddr;

	if(argc!=1) {
		printf("Usage: %s\n",argv[0]);
		return EXIT_FAILURE;
	}
	
	for(i=0;i<CLIENTS;i++) {
		clients[i].running = 0;
	}
	
	/* Create a datagram/UDP socket */
	if((recvSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		die("socket() failed");
	/* Create a datagram/UDP socket */
	if((sendSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		die("socket() failed");
	/* Construct local address structure */
	memset(&servAddr, 0, sizeof(servAddr));	/* Zero out structure */
	servAddr.sin_family = AF_INET;			/* Internet address family */
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);	/* Any incoming interface */
	servAddr.sin_port = htons(SERVER_RECVPORT);	/* Local port */
	/* Bind to the local address */
	if(bind(recvSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
		die("bind() failed");
	
	while(1) {
		clientAddrSize = sizeof(clientAddr);
		if((msgLen = recvfrom(recvSock, msg, CHATMAX, 0, (struct sockaddr *)&clientAddr, &clientAddrSize)) < 0)
			die("recvfrom() failed");
		for(i=0;i<CODELENGTH;i++) {
			code[i] = msg[i];
		}
		code[CODELENGTH] = '\0';
		for(i=CODELENGTH;i<msgLen;i++) {
			msg[i-CODELENGTH] = msg[i];
		}
		msg[msgLen-CODELENGTH] = '\0';
		clientAddr.sin_port = htons(SERVER_SENDPORT);
		switch(atoi(code)%10000) {
			case 1:
				if(initClient(clientAddr, clients)==1) {
					strcpy(msg,inet_ntoa(clientAddr.sin_addr));
					echo(strcat(msg," joined"),SERVCODE,clients);
				} else {
					sendClient("Server is full. Try again",ERROCODE,clientAddr);
				}
				break;
			case 2:
				echo(msg,CHATCODE,clients);
				break;
			case 3:
				msg[0] = '\0';
				strcpy(msg,inet_ntoa(clientAddr.sin_addr));
				echo(strcat(msg, " quit"),SERVCODE,clients);
				killClient(clientAddr, clients);
				break;
			default:
				break;
		}
	}
	
	return EXIT_SUCCESS;
}
