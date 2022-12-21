#include <stdio.h>
#include <stdlib.h>
/* header files */

/* global definitions */

/* definition and implementation of process descriptor and queue(s) */
struct queue{
	struct process *p;
	struct queue *next;
	struct queue *prev;
};

struct process{
	char * name;
	int data;
};


void push(struct queue **head,struct process PROCC){
	struct queue* Q = (struct queue*)malloc(sizeof(struct queue));
	//head->1->2->end 
	Q->p = &PROCC;
	printf("%s",Q->p->name);
	Q->next = (*head);
	Q->prev = NULL;


	if ((*head) != NULL)
		(*head)->prev = Q;

	(*head) = Q;
}

void pop(struct queue *head){
	head->next = head;
}

void printList(struct queue* node){
	struct queue* last;
	printf("\nTraversal in forward direction \n");
	while (node != NULL) {
		printf("%s\n", node->p->name);
		last = node;
		node = node->next;
	}
	
	
}

int numOfProcess(FILE *fp){
	
	int count =0;
	char c;
	
	if (fp==NULL) fprintf(stderr,"ERROR");
	fseek(fp,0,SEEK_SET);
    c = getc(fp);
	while(c!=EOF){
		if (c == '\n') count++;
	
		c = getc(fp);
	}
	return count;
}
void getProcess(FILE *fp,int n,struct process *PROCCS){
   if(fp==NULL) fprintf(stderr,"ERROR");
    int k=0;
    int *numOfChars;
   
    int n1=0;

    char c;
   
    char *buf;
    
    fseek(fp,0,SEEK_SET);


    numOfChars=(int*)malloc(n*sizeof(int));// A dynamic array that counts the chars before '}'
    numOfChars[k]=0;

    c = getc(fp);
    
    while(c!=EOF){

        if(c != '\n')  numOfChars[k]++; // It counts the chars before '}'/
            else if(c=='\n') k++;   //The "i" takes numOfChars in the next element of the array

        c = getc(fp);//Restarts the pointer fp1
    }
    
    int i=0;
  
    fseek(fp,0,SEEK_SET);
   

    while(i<n){
		
        buf=malloc(numOfChars[i]*sizeof(char)+2);
        fscanf(fp,"%s %d",buf,&n1);//Read the file until \n    
		
		PROCCS[i].data = n1;
		PROCCS[i].name = buf;

		i++;
	}

 
}

/* global variables and data structures */

/* signal handler(s) */

/* implementation of the scheduling policies, etc. batch(), rr() etc. */

int main(int argc,char **argv)
{
	struct process* PROCCS =NULL;
	struct queue *Q =NULL;
	FILE *fp;
	char * FILENAME;
	int n;

	FILENAME = argv[argc-1];
	fp = fopen(FILENAME,"r");
	n=numOfProcess(fp);
	PROCCS=(struct process*)malloc(n*sizeof(struct process));
	
	
	
	getProcess(fp,n,PROCCS);
	
	
	
	for(int i=0;i<=n;i++) push(&Q,PROCCS[i]);
	
	printf("%s \n",Q->p->name);
	//printList(Q);
	return 0;
}
