#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <wait.h>
/* header files */

/* global definitions */
//add shared memory for time
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

/*Struct Time is used for storing shared variable's data*/
struct time {
	double _1;
	double _2;
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


	//printf("%s",Q->p->name);
	


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
	
	else {*head = (*head)->next;} 
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

	while ((pid = waitpid(-1, &status, WNOHANG)) > 0);
}
double get_wtime(void)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return (double)t.tv_sec + (double)t.tv_usec*1.0e-6;
}



void FCFS(struct queue *q){
	int pid;
	
	key_t key;
	
	struct time *time;

	key = ftok("../scheduler", 65);

	int shmid = shmget(key,sizeof(struct time),0666|IPC_CREAT);
	signal(SIGCHLD, handler);
	time = shmat(shmid,NULL,0);

	while(q!=NULL){
		pid = fork();
		
		if (pid>0){
			while(finishedProcFlag==0){
				sleep(0.5);
			}
			time->_2= get_wtime();
			printf("=====================================\n");
			printf("For process: %d\n",pid);
			printf("\t Executes:%s\n",q->p->name);
			printf("\t Has Execution Time:%d\n",q->p->data);
			printf("\t Elapsed Time:%f\n",time->_2-time->_1);
			printf("=====================================\n");
			finishedProcFlag=0;
			pop(&q);
		}else if (pid ==0){
    		time->_1 = get_wtime();
			execl(q->p->name,NULL);
			exit(1);
		}
	}
}

void SJF(struct queue *q){}
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
	double time_1, time_2;
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

	time_1 = get_wtime();
	FCFS(Q);
	time_2 = get_wtime();

	printf("\n For File = %s needed Time:%f\n",FILENAME,time_2-time_1);
	//printf("%s \n",Q->next->next->p->name);


}
