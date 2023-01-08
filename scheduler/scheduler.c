#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
/* header files */

/* global definitions */

static volatile int finishedProcFlag = 0;
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


void push(struct queue **head,struct process *PROCC){
	struct queue* Q = (struct queue*)malloc(sizeof(struct queue));
	//head->1->2->end 
	Q->p = PROCC;
	//printf("%s",Q->p->name);
	Q->next = (*head);
	Q->prev = NULL;


	if ((*head) != NULL)
		(*head)->prev = Q;

	(*head) = Q;
}

void ordered_push(struct queue **head,struct process *PROCC){
	struct queue* Q = (struct queue*)malloc(sizeof(struct queue));
	//head->1->2->end 
	Q->p = PROCC;
	//printf("%s",Q->p->name);
	Q->next = (*head);
	Q->prev = NULL;


	if ((*head) != NULL)
		(*head)->prev = Q;

	(*head) = Q;
}

void pop(struct queue **head){
	if(*head ==NULL) return;
	
	else {printf("POPED: %s \n",(*head)->p->name);*head = (*head)->next;} 
}

void printList(struct queue* node){
	
	printf("\nTraversal in forward direction \n");
	while (node != NULL) {
		printf("%s\n", node->p->name);
		
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

void handler(int signo){
	finishedProcFlag =1;
	int pid, status;

	printf("hello from handler (%d)\n", signo);

	while ((pid = waitpid(-1, &status, WNOHANG)) > 0);
}
double get_wtime(void)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return (double)t.tv_sec + (double)t.tv_usec*1.0e-6;
}
void FCFS(struct queue *q,int n){
	int pid;
	double t,t_end;
	signal(SIGCHLD, handler);
	

	while(q!=NULL){
		pid = fork();
		
		if (pid>0){
			while(finishedProcFlag==0){
				sleep(1);
			}
			printf("%s\n",q->p->name);
			printf("Time passed %d\n",t-t_end);

			finishedProcFlag=0;

			pop(&q);
		}else if (pid ==0){
    		t = get_wtime();
			execl(q->p->name,NULL);
			t_end = get_wtime();
			exit(1);
		}
		
	}
	

}


//pid = fork();
//	if (pid == 0) { /* child */
//		child();
//	}
//	printf("parent (%d) waits for child (%d)\n", getpid(), pid);
//	waitpid(pid, &status, 0);
//	if (WIFEXITED(status))
//		printf("child exited normally with value %d\n", WEXITSTATUS(status));
//	else
//		printf("child was terminated abnormally\n");
//	return 0;
//}
//
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
	
	for(int i=n-1;i>=0;i--) push(&Q,&PROCCS[i]);

	
	FCFS(Q,n);
	//printf("%s \n",Q->next->next->p->name);


}
