/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3506" // the port client will be connecting to

#define MAXDATASIZE 1000 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	char command;
	char filename[255];
	char server_filename[255];
	int match;
	char user_input[MAXDATASIZE];


	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	while(command != 'q') {

		if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}

		// loop through all the results and connect to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
				perror("client: socket");
				continue;
			}

			if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				perror("client: connect");
				continue;
			}

			break;
		}

		if (p == NULL) {
			fprintf(stderr, "client: failed to connect\n");
			return 2;
		}

		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
				s, sizeof s);
		printf("client: connecting to %s\n", s);

		freeaddrinfo(servinfo); // all done with this structure

		command = getchar();
		while(getchar() != '\n') {}

		switch(command) {

			//help
			case 'h':
				printf("-----------------------------------------------------------------------\n");
				printf("l: List contents of the directory of the server\n");
				printf("c: Check if the server has a file\n");
				printf("d: Download a file on the server\n");
				printf("p: Display a file on the server\n");
				printf("q: Quit\n");
				printf("-----------------------------------------------------------------------\n");
				break;
			//list
			case 'l':
				//Send command to the server
				send(sockfd, "l", strlen("l"),  0);

				//Receive list from the server
				if((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
					perror("recv");
					exit(1);
				}

				//Array to strring
				buf[numbytes] = '\0';
				printf("-----------------------------------------------------------------------\n");
				printf("%s\n", buf);
				printf("-----------------------------------------------------------------------\n");
				break;

			//check
			case 'c':
				//Send command to the server
				send(sockfd, "c", 1, 0);

				//Ask the client for the name
				printf("Enter the file name: ");
				scanf("%s", &filename);
				while(getchar() != '\n'){;}

				//send filename to the server
				send(sockfd, filename, 254, 0);

				//Receive 1 for found or 0 for not found
				match = recv(sockfd, server_filename, 254, 0);

				//Turn character awway into string by appending \0 at end
				server_filename[match] = '\0';

				//If file is not found
				if(server_filename[0] == '0') {
					printf("--------------------------------------------------------------");
					printf("---------\n");
					printf("File <%s> not found\n", filename);
					printf("--------------------------------------------------------------");
					printf("---------\n");
				}

				//If the file is found
				else {
					printf("--------------------------------------------------------------");
					printf("---------\n");
					printf("File <%s> exists\n", filename);
					printf("--------------------------------------------------------------");
					printf("---------\n");
				}

				break;

			//download
			case 'd':
				//Send command to the server
				send(sockfd, "d", 1, 0);

				//Ask the client for the name
				printf("Enter the file name: ");
				scanf("%s", &filename);
				while(getchar() != '\n'){;}
				//check if fiile exists locally
				int ifile;
				char file[255];
				file[ifile] = '\0';

				//send filename to the server
				send(sockfd, filename, 254, 0);
				match = recv(sockfd, server_filename, 254, 0);

				if((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
					perror("recv");
					exit(1);
				}

				//Receive 1 for found or 0 for not found
				//match = recv(sockfd, server_filename, 254, 0);

				if(fopen(filename, "r") != NULL) {
					printf("--------------------------------------------------------------");
					printf("---------\n");
					printf("File <%s> exists\n", filename);
					printf("Overwrite file? (y/n)\n");
					char overwrite = getchar();
					while(getchar() != '\n') {}

					//if no overwrite then break
					if(overwrite != 'y') break;

					else {
						//Recieve file
						//numbytes = recv(sockfd, buf, MAXDATASIZE, 0);

						FILE *fp;
						fp = fopen(filename, "w");

						//int inew_file;
						//char new_file[MAXDATASIZE];
						//new_file[inew_file] = '\0';
						//printf("%s", new_file);

						fputs(buf, fp);
						fclose(fp);
						printf("\n----------------------------------------------------");
						printf("-------------------\n");
						printf("File downloaded\n");
						printf("----------------------------------------------------");
						printf("-------------------\n");
					}

				}
				else {
					printf("--------------------------------------------------------------");
					printf("---------\n");
					printf("File <%s> not found\n", filename);
					printf("Create a new file? (y/n)\n");
					char overwrite = getchar();
					while(getchar() != '\n') {}

					//if no overwrite then break
					if(overwrite != 'y') break;

					else {
						FILE *fp;
						fp = fopen(filename, "w");

						//int inew_file;
						//char new_file[MAXDATASIZE];
						//new_file[inew_file] = '\0';
						//printf("%s", new_file);

						fputs(buf, fp);
						fclose(fp);
						printf("\n----------------------------------------------------");
						printf("-------------------\n");
						printf("File downloaded\n");
						printf("----------------------------------------------------");
						printf("-------------------\n");
					}
					close(sockfd);
				}

				break;
			//display
			case 'p':
				//send the command to the server
				send(sockfd, "p", 1, 0);


				//ask the client for the name
				printf("Please enter file name: ");
				scanf("%s", &filename);
				while(getchar() != '\n'){;}

				//send filename to the server
				send(sockfd, filename, 254, 0);

				//Receive 1 for found or 0 for not found
				match = recv(sockfd, server_filename, 254, 0);

				//Turn character awway into string by appending \0 at end
				server_filename[match] = '\0';

				//If file is not found
				if(server_filename[0] == '0') {
					printf("--------------------------------------------------------------");
					printf("---------\n");
					printf("File <%s> not found\n", filename);
					printf("--------------------------------------------------------------");
					printf("---------\n");
				}

				//If the file is found
				else {
					//Receive list from the server
					if((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
						perror("recv");
						exit(1);
					}
					printf("--------------------------------------------------------------");
					printf("---------\n");
					printf("File <%s> exists\n", filename);
					printf("CONTENT OF THE FILE: \n\n");
					printf("%s \n", buf);
					printf("--------------------------------------------------------------");
					printf("---------\n");
				}

				break;

			//quit
			case 'q':
				command = 'q';
				break;

			default:
				printf("-----------------------------------------------------------------------\n");
				printf("Not a valid command\n");
				printf("-----------------------------------------------------------------------\n");
				break;
		}

	}

}
/*
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("client: received \n '%s'\n",buf);

	close(sockfd);

	return 0;
}
*/
