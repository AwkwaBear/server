#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void main(void)
{
int pid;
int fd[4][2];
for (int k=0; k<4; k++) {
	pipe(fd[k]);  /* Create a pipe for child k */
	if ((pid = fork())<0) {
		printf("Could not fork child %d\n",k);
		exit(EXIT_FAILURE);
	}
	else if (pid==0) { /* Child */
		close(fd[k][0]); /* close read-end of pipe */
		dup2(fd[k][1], STDOUT_FILENO); /* Replace stdout with pipe */
		int length = snprintf(NULL, 0, "%d", 25*k);
		char *start = (char *) malloc(length+1);
		snprintf(start, length+1, "%d", 25*k);
		length = snprintf(NULL, 0, "%d", 25*k+24); 
		char *end = (char *) malloc(length+1);
		snprintf(end, length+1, "%d", 25*k+24);
		execlp("./add2","./add2",start,end,(char *)NULL);
		free(start);
		free(end);
		exit(EXIT_FAILURE);
	}
	close(fd[k][1]);
}
for (int k=0; k<4; k++) {
	wait(NULL);
}
int sum=0;
char buffer[100];
for (int k=0; k<4; k++) {
	int j=read(fd[k][0], buffer, 100);
	if (j<100) {
		buffer[j] = '\0';
		sum += (int) strtol(buffer, NULL, 10);
	}
}
printf("Parent: sum=%d\n",sum);
}
