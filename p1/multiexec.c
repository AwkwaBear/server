#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void main()
{
for (int k=0; k<4; k++) {
    if (fork()==0) { /* Child */
	int length = snprintf(NULL,0,"%d",k); /* Compute char string length to represent int k*/
	char* start = malloc(length+1);  /* Allocate string including terminal char NULL */
	snprintf(start,length+1,"%d",k); /* Convert int k to string */
	length = snprintf(NULL,0,"%d",k+9);  /* Repeat */
	char* end = malloc(length+1);
	snprintf(end,length+1,"%d",k+9);
	execlp("./add","./add",start,end,(char *)NULL);
	free(start); /* You won't get this far unless execlp fails */
	free(end);
	exit(0);
    }
}
for (int k=0; k<4; k++) {
	wait(NULL);
}
printf("Parent:  all children have terminated\n");
}

