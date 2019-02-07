#include <stdio.h>
#include <stdlib.h>

void main(int argc, char *argv[])
{
if (argc != 3) {
	printf("0\n");
	return;
}
char *ptr;

int start = strtol(argv[1],NULL,10);
int end = strtol(argv[2],NULL,10);
int sum = 0;;
for (int i=start; i<= end; i++) {
    sum += i;
}
printf("%d\n", sum);
}
