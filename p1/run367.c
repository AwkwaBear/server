#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main(int argc, char * argv[])
{
if (argc < 2) {
	printf("Usage:  ./run367  <program> <program parameters>\n");
	return;
}
printf("About to launch '%s'\n",argv[1]);
char ** a = (char **) malloc(argc*sizeof(char *));
for (int k=0; k<argc-1; k++) {
	a[k] = argv[k+1];
}
a[argc-1] = (char *) NULL;
execvp(a[0], a);
printf("Launch failed\n");
}
