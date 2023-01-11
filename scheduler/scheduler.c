#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <wait.h>
#include <string.h>
#include <time.h>
/* header files */

/* global definitions */
//add shared memory for time
static volatile int finishedProcFlag = 0;

/* definition and implementation of process descriptor and queue(s) */

typedef struct MinHeap MinHeap;
struct MinHeap {
    struct process *p;
    int size;
    int capacity;
};

struct queue{
	struct process *p;
	struct queue *next;
	struct queue *prev;
};

struct process{
	int proc;
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


int parent(int i) {return (i - 1) / 2;}
int left_child(int i){return (2*i + 1);}
int right_child(int i) {return (2*i + 2);}
struct process get_min(MinHeap* heap) {return heap->p[0];}

MinHeap* init_minheap(int capacity) {
    MinHeap* minheap = (MinHeap*) malloc (1 *sizeof(MinHeap));
    minheap->p = (struct process*) malloc (capacity*sizeof(struct process));
    minheap->capacity = capacity;
    minheap->size = 0;
    return minheap;
}

MinHeap* insert_minheap(MinHeap* heap, struct process element) {
    if (heap->size == heap->capacity) {
        fprintf(stderr, "Cannot insert %s. Heap is already full!\n", element.name);
      return heap;
    }
    heap->size++;
    heap->p[heap->size - 1] = element;
    int curr = heap->size - 1;
    while (curr > 0 && heap->p[parent(curr)].data > heap->p[curr].data) {
        struct process temp = heap->p[parent(curr)];
        heap->p[parent(curr)] = heap->p[curr];
        heap->p[curr] = temp;
        curr = parent(curr);
    }
    return heap; 
}
MinHeap* heapify(MinHeap* heap, int index) ;
MinHeap* delete_minimum(MinHeap* heap) {
    // Deletes the minimum element, at the root
    if (!heap || heap->size == 0)
        return heap;

    int size = heap->size;
    struct process last_element = heap->p[size-1];
    
	
    // Update root value with the last element
    heap->p[0] = last_element;

    // Now remove the last element, by decreasing the size
    heap->size--;
    size--;

    // We need to call heapify(), to maintain the min-heap
    // property
    heap = heapify(heap,0);
    return heap;
}

MinHeap* heapify(MinHeap* heap, int index) {
    
    if (heap->size <= 1)
        return heap;
    
    int left = left_child(index); 
    int right = right_child(index); 
    int smallest = index; 
    if (left < heap->size && heap->p[left].data < heap->p[index].data) 
        smallest = left; 
    if (right < heap->size && heap->p[right].data < heap->p[smallest].data) 
        smallest = right; 
    if (smallest != index) 
    { 
        struct process temp = heap->p[index];
        heap->p[index] = heap->p[smallest];
        heap->p[smallest] = temp;
        heap = heapify(heap, smallest); 
    }

    return heap;
}
void print_heap(MinHeap* heap) {
    
    printf("Min Heap:\n");
    for (int i=0; i<heap->size; i++) {
        printf("%d -> ", heap->p[i].data);
    }
    printf("\n");
}

void pop(struct queue **head){
	if(*head ==NULL) return;
	
	else {*head = (*head)->prev;} 
}

void printList(struct queue* node){
	
	printf("\nTraversal in forward direction \n");
	while (node != NULL) {
		printf("%s %d\n", node->p->name,node->p->data);
		
		node = node->prev;
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
		PROCCS[i].proc = 0;
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



void _static(struct queue *q){
	int pid;
	
	key_t key;
	
	struct time *time;
	struct sigaction sact;
	key = ftok("../scheduler", 65);
	sact.sa_handler = handler;
	sigemptyset(&sact.sa_mask);
	sact.sa_flags =0;
	int shmid = shmget(key,sizeof(struct time),0666|IPC_CREAT);
	sigaction(SIGCLD,&sact,NULL);
	time = shmat(shmid,NULL,0);

	while(q!=NULL){
		pid = fork();
		
		if (pid>0){
			while(finishedProcFlag==0){}
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



void rr(struct queue *q,int quantum){
	int pid;
	struct time *time;
	struct sigaction sact;
	key_t key;

	sact.sa_handler = handler;
	sigemptyset(&sact.sa_mask);
	sact.sa_flags =0;
	sigaction(SIGCLD,&sact,NULL);

	key = ftok("../scheduler", 65);
	int shmid = shmget(key,sizeof(struct time),0666|IPC_CREAT);
	time = shmat(shmid,NULL,0);

	printf("FIRST AND FOREMOST");
	printList(q);
	while(q!=NULL){
		pid = fork();
		
		if (pid>0){
			struct timespec  request;
			request.tv_sec=quantum/1000;
			int response = nanosleep(&request,NULL);
			kill(pid,SIGSTOP);
	
			q->p->proc = pid; 
			printList(q);
			pop(&q);
			printList(q);
			push(&q,&q->p);
			printList(q);
			//change running process
			time->_2= get_wtime();
			
			
		}else if (pid ==0){
			if (q->p->proc ==0){
				fprintf(stderr,"EDQ\n\n\n\n");
    		time->_1 = get_wtime();
			execl(q->p->name,NULL);}
			else{
				kill(q->p->proc,SIGCONT);
				time->_1 = get_wtime();
				execl(q->p->name,NULL);

			}
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
struct queue * gotoHead(struct queue* q,int n ){
	for(int i=0;i<n-1;i++){
		q = q->next;
	}
	return q;
}

int main(int argc,char **argv)
{
	double time_1, time_2;
	struct process* PROCCS =NULL;
	struct queue *Q =NULL;
	FILE *fp;
	
	char * FILENAME;
	int n;
	printf("FILE%s\n",argv[argc-1] );

	
	FILENAME = argv[argc-1];
	fp = fopen(FILENAME,"r");
	
	n=numOfProcess(fp);
	PROCCS=(struct process*)malloc(n*sizeof(struct process));
	
	getProcess(fp,n,PROCCS);

	

	if(strcmp(argv[1],"BATCH")==0){
		
		
		for(int i=0;i<n;i++) push(&Q,&PROCCS[i]);
		
		Q = gotoHead(Q,n);
		time_1 = get_wtime();
		_static(Q);
		time_2 = get_wtime();
		printf("\n For File = %s and Method =%s needed Time:%f\n",FILENAME,argv[1],time_2-time_1);
		
	}
	else if(strcmp(argv[1],"SJF")==0){
		MinHeap* heap = init_minheap(n);
		for(int i=0;i<n;i++) insert_minheap(heap,PROCCS[i]);
	
		for(int i=0;i<n;i++){
		struct process *temp = malloc(sizeof(struct process));
		(*temp) = heap->p[0];
		push(&Q,temp);
		heap = delete_minimum(heap);
		
	}
		
		Q = gotoHead(Q,n);
		time_1 = get_wtime();
		_static(Q);
		time_2 = get_wtime();
		printf("\n For File = %s and Method =%s needed Time:%f\n",FILENAME,argv[1],time_2-time_1);

	}else if(strcmp(argv[1],"RR")==0){
		
		for(int i=0;i<n;i++) push(&Q,&PROCCS[i]);
	
		Q = gotoHead(Q,n);
		time_1 = get_wtime();
		rr(Q,argv[2]);
		time_2 = get_wtime();
		printf("\n For File = %s and Method =%s needed Time:%f\n",FILENAME,argv[1],time_2-time_1);
		

	}

	
}

