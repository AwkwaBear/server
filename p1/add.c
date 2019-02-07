#include <stdio.h>
#include <stdlib.h>

void main(int argc, char *argv[])
{
if (argc != 3) {
	printf("Usage:  ./add <start value> <end value>\n");
	return;
}
char *ptr;

int start = strtol(argv[1],&ptr,10);
int end = strtol(argv[2],&ptr,10);
int sum = 0;;
for (int i=start; i<= end; i++) {
    sum += i;
}
printf("Sum of %d to %d = %d\n", start,end,sum);
}
