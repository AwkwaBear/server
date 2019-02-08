/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3530"  // the port users will be connecting to
#define MAXDATASIZE 1000
#define BACKLOG 10	 // how many pending connections queue will hold

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

char* data = "Input Data\n";
int main(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);
		if (!fork()) {

			int in[2], out[2], n, pid;
			char buf[MAXDATASIZE];
			char store[255];
			char input;

			if(pipe(in) < 0) perror("pipe in");
			if(pipe(out) < 0) perror("pipe out");

			int command = recv(new_fd, store, 255, 0);
			input = store[0];

			if (input == 'l') {
				if((pid=fork()) == 0) {
					close(0);
					close(1);
					close(2);
					dup2(in[0],0);
					dup2(out[1],1);
					dup2(out[1],2);
					close(in[1]);
					close(out[0]);
					execl("/usr/bin/ls", "ls", (char *)NULL);
					perror("Could not exc ls");
				}
				close(in[0]);
				close(out[1]);
				write(in[1], data, strlen(data));
				close(in[1]);
				n = read(out[0], buf, 1000);
				buf[n] = 0;
				close(sockfd);
				send(new_fd, buf, 1000, 0);
				close(new_fd);
				exit(0);
			}

			else if (input == 'p' || input == 'd') {

				char file[255];

				int ifile = recv(new_fd, file, 254, 0);
				file[ifile] = '\0';


				if(fopen(file, "r") != NULL) {
					send(new_fd, "1", strlen("1"), 0);
					if((pid=fork()) == 0) {

						close(0);
						close(1);
						close(2);

						dup2(in[0],0);
						dup2(out[1],1);
						dup2(out[1],2);

						close(in[1]);
						close(out[0]);
						execl("/usr/bin/cat", "cat ", file, (char *)NULL);
						perror("Could not exc cat");
					}

					close(in[0]);
					close(out[1]);

					write(in[1], data, strlen(data));
					close(in[1]);

					n = read(out[0], buf, 1000);
					buf[n] = 0;

					close(sockfd);
					send(new_fd, buf, 1000, 0);
					close(new_fd);
					exit(0);
				}


				else {
					send(new_fd, "0", strlen("0"), 0);
				}
				close(new_fd);


			}
			else if (input == 'c') {
				char file[255];

				int ifile = recv(new_fd, file, 254, 0);
				file[ifile] = '\0';

				if(fopen(file, "r") != NULL) {
					send(new_fd, "1", strlen("1"), 0);
				}

				else {
					send(new_fd, "0", strlen("0"), 0);
				}
				close(new_fd);
			}
		}
		close(new_fd);  
	}

	return 0;
}
