#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "constants.h"

int running = 0;

void die(char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

void chatRecv(struct sockaddr_in servAddr) {
	int recvSock;
	int recvChatLen;
	unsigned int servSize;
	char recvChat[CHATMAX];
	printf("done");
	servSize = sizeof(servAddr);
	printf("done");
	/* Create a datagram/UDP socket */
	if((recvSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		die("socket() failed");
	printf("done");
	/* Server port */	
	servAddr.sin_port = htons(CLIENT_RECVPORT);
	printf("done");
	while(1) {
		printf("done");
		if(running==0)
			break;
		recvChatLen = recvfrom(recvSock, recvChat, CHATMAX, 0, (struct sockaddr *)&servAddr, &servSize);
		printf("done");
		recvChat[recvChatLen] = '\0';
		printf("done");
		printf("%s\n", recvChat);
	}
	
	close(recvSock);
	exit(EXIT_SUCCESS);
}

void chatSend(struct sockaddr_in servAddr) {
	int sendSock;
	char chat[CHATMAX];
	
	/* Create a datagram/UDP socket */
	if ((sendSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		die("socket() failed");
	/* Server port */	
	servAddr.sin_port = htons(CLIENT_SENDPORT);
	sendto(sendSock, INITCODE, CHATMAX, 0, (struct sockaddr *)&servAddr, sizeof(servAddr));
	
	while(1) {
		/* Send the string to the server */
		printf("Chat> ");
		fgets(chat, CHATMAX-10, stdin);
		if(strcmp(chat,"\\quit\n")==0)
			break;
		sendto(sendSock, strcat(CHATCODE,chat), CHATMAX, 0, (struct sockaddr *)&servAddr, sizeof(servAddr));
	}
	
	running = 0;
	sendto(sendSock, EXITCODE, CHATMAX, 0, (struct sockaddr *)&servAddr, sizeof(servAddr));
	close(sendSock);
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
	pid_t pid;
	struct sockaddr_in servAddr;

	if(argc!=2) {
		printf("Usage: %s <server_ip>\n",argv[0]);
		return EXIT_FAILURE;
	}
	
	/* Construct the server address structure */
	memset(&servAddr, 0, sizeof(servAddr));	/* Zero out structure */
	servAddr.sin_family = AF_INET;			/* Internet addr family */
	servAddr.sin_addr.s_addr = inet_addr(argv[1]);	/* Server IP address */
	
	pid = fork();
	
	running = 1;
	
	if(pid!=0)
		chatRecv(servAddr);
	else
		chatSend(servAddr);
	
	return EXIT_SUCCESS;
}
