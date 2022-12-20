#include <stdio.h>
#include <stdlib.h>
/* header files */

/* global definitions */

/* definition and implementation of process descriptor and queue(s) */
struct queue{
	int data;
	struct queue *next;
}
struct queue Q;

void insert()
void pop()
/* global variables and data structures */

/* signal handler(s) */

/* implementation of the scheduling policies, etc. batch(), rr() etc. */

int main(int argc,char **argv)
{
	printf("Argc:%d\n",argc);
	for(int i=0; i<argc;i++)
	printf("ARGV:%s \n",argv[i]);

	printf("Scheduler exits\n");
	printf("\n");
	return 0;
}
