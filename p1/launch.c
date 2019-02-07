#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main()
{
printf("Start of 'launch'\n");
execlp("./aloha", "./aloha", (char *) NULL);
printf("End of 'launch'\n");
}
