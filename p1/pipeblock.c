#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>  /* Needed for wait() */
#include <fcntl.h> /* Need for fcntl */

#define PIPE_READ 0
#define PIPE_WRITE 1

void main(void)
{
char msg[] = "abcdefghijklmnopqrstufwxyz";
int fd[2];
int pid;
char buffer[100];

pipe(fd); 
printf("Blocking pipe is created:\n");
printf("   Child writes 3 times:  5 sec delay per write\n");
printf("   Parent reads 5 times:  no delay per read\n");

pid = fork();
if (pid<0) {
	fprintf(stderr, "Pipe Failed");
	exit(EXIT_FAILURE);
}
else if (pid==0) { /* Child */
	close(fd[PIPE_READ]);
	for (int k=0; k<5; k++) {
		sleep(5);
		int m=write(fd[PIPE_WRITE], &msg[k], 5);
		printf("Child(k=%d): wrote %d bytes\n",k,m);
	}
	exit(EXIT_SUCCESS);
}
/* Parent */
close(fd[PIPE_WRITE]);
for (int i=0; i<3; i++) {
	int n=read(fd[PIPE_READ], buffer, 10);
	if (n<0) {
		printf("Parent(i=%d): read error\n",i);
	}
	else {
		buffer[n] = '\0';
		printf("Parent(i=%d): read %d bytes = %s\n",i,n,buffer);
	}
}
wait(NULL);
printf("Transfer to blocking pipe is completed\n");

printf("Nonblocking pipe is created\n");
printf("   Child writes 3 times:  5 sec delay per write\n");
printf("   Parent reads 20 times:  1 sec delay per read\n");
int fd2[2];
pipe(fd2);
fcntl(fd2[PIPE_READ], F_SETFL, O_NONBLOCK); /* Read-end nonblocking */  
fcntl(fd2[PIPE_WRITE], F_SETFL, O_NONBLOCK); /* Write-end nonblocking */

pid = fork();
if (pid<0) {
	fprintf(stderr, "Pipe Failed");
	exit(EXIT_FAILURE);
}
else if (pid==0) { /* Child */
	close(fd2[PIPE_READ]);
	for (int k=0; k<3; k++) {
		sleep(5);
		int m=write(fd2[PIPE_WRITE], &msg[k], 5);
		printf("Child(k=%d): wrote %d bytes\n",k,m);
	}
	exit(EXIT_SUCCESS);
}
/* Parent */
close(fd2[PIPE_WRITE]);
for (int i=0; i<20; i++) {
	int n=read(fd2[PIPE_READ], buffer, 10);
	sleep(1);
	if (n<0) {
		printf("Parent(i=%d): read error\n",i);
	}
	else {
		buffer[n] = '\0';
		printf("Parent(i=%d): read %d bytes = %s\n",i,n,buffer);
	}
}
wait(NULL);
printf("Transfer to nonblocking pipe is completed\n");


return;
}

