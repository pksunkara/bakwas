#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "constants.h"
#include "langs.h"

WINDOW *room;
WINDOW *input;

void die(char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

int getLang(char *lang) {
	if(strcmp(lang,"hin")==0) {
		return HIN;
	} else if(strcmp(lang,"tel")==0) {
		return TEL;
	} else if(strcmp(lang,"tam")==0) {
		return TAM;
	}
	return ENG;
}

void transliterate(char *chat, int lang) {
	int i=0,j=0;
	char *buf = malloc(strlen(chat)*sizeof(char));
	char *charBuf = malloc(sizeof(char));
	buf[0] = '\0';
	for(;i<strlen(chat);i++) {
		if((int)chat[i]>0) {
			charBuf[0] = chat[i];
			strcat(buf,charBuf);
		} else if((int)chat[i]==-32) {
			char *tmp = malloc(5*sizeof(char));
			j = 0;
			tmp[j] = chat[i];
			j++;
			while((int)chat[i+j]<0 && (int)chat[i+j]!=-32) {
				tmp[j] = chat[i+j];
				j++;
			}
			tmp[j--] = '\0';
			a2b(tmp,lang);
			strcat(buf,tmp);
			if(tmp!=NULL)
				free(tmp);
			i+=j;
		}
	}
	strcpy(chat,buf);
	if(buf!=NULL)
		free(buf);
	return;
}

void chatRecv(char *ip, char *lang) {
	int recvSock, recvChatLen;
	int i;
	unsigned int clientAddrSize;
	char recvChat[CHATMAX], code[CODELENGTH+1];
	struct sockaddr_in servAddr;
	struct sockaddr_in clientAddr;

	clientAddrSize = sizeof(clientAddr);
	/* Create a datagram/UDP socket */
	if((recvSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		die("socket() failed");
	/* Construct the server address structure */
	memset(&servAddr, 0, sizeof(servAddr));	/* Zero out structure */
	servAddr.sin_family = AF_INET;			/* Internet addr family */
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);	/* Server IP address */	
	servAddr.sin_port = htons(CLIENT_RECVPORT);	/* Server port */
	/* Bind to the local address */
	if(bind(recvSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
		die("bind() failed");

	while(1) {
		recvChatLen = recvfrom(recvSock, recvChat, CHATMAX, 0, (struct sockaddr *)&clientAddr, &clientAddrSize);
		recvChat[recvChatLen] = '\0';
		for(i=0;i<CODELENGTH;i++) {
			code[i] = recvChat[i];
		}
		code[CODELENGTH] = '\0';
		for(i=CODELENGTH;i<recvChatLen;i++) {
			recvChat[i-CODELENGTH] = recvChat[i];
		}
		switch(atoi(code)%10000) {
			case 2:
				transliterate(recvChat,getLang(lang));
				printf("%s\n",recvChat);
				break;
			case 3:
				printf("%s\n",recvChat);
				exit(EXIT_SUCCESS);
				break;
			case 4:
				printf("%s\n",recvChat);
				break;
			case 5:
				printf("Error: %s\n",recvChat);
				exit(EXIT_SUCCESS);
				break;
			default:
				break;
		}
	}
	
	close(recvSock);
	return;
}

void chatSend(char *ip, char *name) {
	int sendSock;
	char chat[CHATMAX];
	char *buf;
	struct sockaddr_in servAddr;
	
	/* Create a datagram/UDP socket */
	if ((sendSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		die("socket() failed");
	/* Construct the server address structure */
	memset(&servAddr, 0, sizeof(servAddr));	/* Zero out structure */
	servAddr.sin_family = AF_INET;			/* Internet addr family */
	servAddr.sin_addr.s_addr = inet_addr(ip);	/* Server IP address */	
	servAddr.sin_port = htons(CLIENT_SENDPORT);	/* Server port */
	
	buf = malloc((strlen(name)+10)*sizeof(char));
	strcpy(buf,INITCODE);
	strcat(buf,name);
	sendto(sendSock, strcat(buf,"\n"), CHATMAX, 0, (struct sockaddr *)&servAddr, sizeof(servAddr));
	if(buf!=NULL)
		free(buf);
	
	while(1) {
		/* Send the string to the server */
		fgets(chat,CHATMAX-20,stdin);
		if(strcmp(chat,"\\quit\n")==0) {	
			sendto(sendSock, EXITCODE, CHATMAX-10, 0, (struct sockaddr *)&servAddr, sizeof(servAddr));
			exit(EXIT_SUCCESS);
		}
		buf = malloc((strlen(chat)+10)*sizeof(char));
		strcpy(buf,CHATCODE);
		strcat(buf,chat);
		sendto(sendSock, buf, CHATMAX-10, 0, (struct sockaddr *)&servAddr, sizeof(servAddr));
		if(buf!=NULL)
			free(buf);
	}

	close(sendSock);
	return;
}

int main(int argc, char **argv) {
	pid_t pid;

	if(argc!=4) {
		printf("Usage: %s <server_ip> <username> <lang>\n",argv[0]);
		printf("\nLanguages available:\n\n\t'hin' -> Hindi\n\t'tel' -> Telugu\n\t'tam' -> Tamil\n");
		return EXIT_FAILURE;
	}
	
	initscr();
	start_color();
	
	pid = fork();
	if(pid==0)
		chatSend(argv[1],argv[2]);
	else
		chatRecv(argv[1],argv[3]);

	endwin();	
	return EXIT_SUCCESS;
}
