#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
/* header files */

/* global definitions */
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
	int pid;
	double time1;
	double time2;
	char * name;
	int data;
};

/*Struct Time is used for storing shared variable's data*/
struct time {
	double _1;
	double _2;
};

double get_wtime(void)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return (double)t.tv_sec + (double)t.tv_usec*1.0e-6;
}


int numOfProcess(FILE *fp){
	
	int count =0;
	char c;
	
	if (fp==NULL) fprintf(stderr,"ERROR1");
	fseek(fp,0,SEEK_SET);
    c = getc(fp);
	while(c!=EOF){
		if (c == '\n') count++;
	
		c = getc(fp);
	}
	return count;
}

void getProcess(FILE *fp,int n,struct process *PROCCS){
   if(fp==NULL) fprintf(stderr,"ERROR2");
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
		PROCCS[i].pid = 0;
		i++;
	}
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

void pop(struct queue **head)
{
    struct queue* temp = *head;
    if(*head != NULL)
    {
        if((*head)->prev != NULL)
        {
            *head = (*head)->prev;
            (*head)->next = NULL;
        }
        else
        {
            *head = NULL;
        }
    }
    free(temp);
}
void printList(struct queue* node){
	
	//printf("\nTraversal in forward direction \n");
	while (node != NULL) {
		fprintf(stderr,"%s %d\n", node->p->name,node->p->data);
		
		node = node->prev;
	}
}

struct queue * gotoHead(struct queue* q,int n ){
	if(n<=0) return q;
	for(int i=0;i<n-1;i++){
		q = q->next;
	}
	return q;
}
struct queue * gotoTail(struct queue* q, int n ){
	if(n<=0) return q;
	for(int i=0;i<n-1;i++){
		q = q->prev;
	}
	return q;
}

struct queue * pushHeadtoTail(struct queue *requestQ,int n){
	
		if (n<=1) return requestQ;
		struct process *procc = requestQ->p;
		pop(&requestQ);
		
		requestQ = gotoTail(requestQ,n-1);
		push(&requestQ,procc);
		requestQ=gotoHead(requestQ,n);
		return requestQ;
}

void sigchld_handler(int signo){
	int pid, status;
	
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0){
		finishedProcFlag=1;
	}
}

void _static(struct queue *q,int flag){
	int pid;
	key_t key;	
	struct time *time;
	struct sigaction sact;

	key = ftok("../scheduler", 65);
	sact.sa_handler = sigchld_handler;
	sigemptyset(&sact.sa_mask);
	sact.sa_flags =0;
	int shmid = shmget(key,sizeof(struct time),0666|IPC_CREAT);
	sigaction(SIGCHLD,&sact,NULL);
	time = shmat(shmid,NULL,0);
	double time1 =get_wtime();
	while(q!=NULL){
		pid = fork();
		
		if (pid>0){
			while(finishedProcFlag==0){}
			double time2 = time->_2;
			time->_2= get_wtime();
			time2 = time->_2-time2;
			printf("PID %d - CMD: %s\n",pid,q->p->name);
			printf("\t\t\t\t Elapsed time = %f\n",time->_2-time->_1);
			if(flag==1)
			printf("\t\t\t\t Workload time = %f\n",  time->_2-time1);
				
			finishedProcFlag=0;
			pop(&q);
		}else if (pid ==0){
    		time->_1 = get_wtime();
			execl(q->p->name,NULL);
			exit(1);
		}
	}
	if (flag==1)
	printf("WORKLOAD TIME: %f seconds\n",time->_2-time1);
}


void rr(struct queue *q,float quantum,int size,int flag){
	struct time *time;
    struct sigaction sa;
	struct queue *requestQ=NULL;
	int n =0;
	pid_t pid;
	int fd = shm_open("/my_shared_struct", O_CREAT | O_RDWR, 0666);
	//struct timespec rqtp,rmtp = {0,quantum};
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);
	sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, NULL);
	float t_quantum = quantum/1000;
	
	//Shared memory is used only for process to have access to the time and pid values
    // Set the size of the shared memory region
    ftruncate(fd, sizeof(struct queue*));
    // Map the shared memory region to a pointer
    requestQ = mmap(NULL, sizeof(struct queue*), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);		
	// Set the size of the shared memory region
    ftruncate(fd, sizeof(struct time*));
    // Map the shared memory region to a pointer
    time = mmap(NULL, sizeof(struct time*), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	time->_1=get_wtime();
	while(1){

			while( q!=NULL  ) {
				//add process to requestQ
				requestQ= gotoTail(requestQ,n);
				push(&requestQ,q->p);
				n++;
				requestQ= gotoHead(requestQ,n);
				pop(&q);
				}
				if(requestQ->p->pid==0){ 
					//if command has no pid
					pid =fork();
					if(pid>0){
 
						requestQ->p->pid = pid;
						requestQ->p->time2 =0;
						requestQ->p->time1 = get_wtime();
						requestQ->p->time2 =get_wtime();
						sleep(1);
						kill(requestQ->p->pid,SIGSTOP);
						requestQ = pushHeadtoTail(requestQ,n);
					}
					if(pid==0){
						execl(requestQ->p->name,NULL);
						exit(1);
					}
				}		
				else if(requestQ!=NULL && requestQ->p->pid!=0){
					double time1,time2;
					//block SIGCHLD
					sigprocmask(SIG_BLOCK, &mask, NULL);

					time1 =get_wtime();
					//Continue the process 
					kill(requestQ->p->pid,SIGCONT);
					sleep(1);
					//Stop the process
					kill(requestQ->p->pid,SIGSTOP);
					//Unblock the process inorder to receive SIGCHLD signals
					sigprocmask(SIG_UNBLOCK, &mask, NULL);
					time2 = get_wtime();

					double ntime = time2-time1;
					requestQ->p->time2 += ntime;

					if(finishedProcFlag==1&&requestQ!=NULL){
						//if finishedProcFlag ==1 then a process has finished
						printf("PID %d - CMD: %s\n",requestQ->p->pid,requestQ->p->name);
						printf("\t\t\t\t Elapsed time = %f\n",requestQ->p->time2-requestQ->p->time1);
						time->_2 = get_wtime() ;

						//flag is only for output purposes
						if(flag==1)
						printf("\t\t\t\t Workload time = %f\n",  time->_2-time->_1);
						pop(&requestQ);
						n--;
						finishedProcFlag=0;
				}else{
					//After the the execution of a process add it to the tail
					requestQ =pushHeadtoTail(requestQ,n);
					}
				}
			if (n==0)break;	// Escape if no process in requestQ
	}
	//clear the shared memory
	munmap(requestQ, sizeof(struct queue *));

	//flag is only for output purposes
	if (flag==1)
	printf("WORKLOAD TIME: %f seconds\n",time->_2-time->_1);

}

void prio(struct queue *q,int quantum,int size){
	int priority=q->p->data;
	int n=0;
	
	double time1 = get_wtime();
	while(1){
		struct queue *priorityqueue=NULL;
		while( q!=NULL && q->p->data<=priority ) {
				//add the elements to the queue with the specified number
				priorityqueue= gotoTail(priorityqueue,n);
				push(&priorityqueue,q->p);
				n++;
				priorityqueue= gotoHead(priorityqueue,n);
				pop(&q);
				}
				
		if(n==1){		
			//if there is only one cmds in priority queue, 
			//run it with static scheduling algorithm
			_static(priorityqueue,0);
			n=0;

		}
		else if (n>1){
			//if more than one cmds, run RR
			rr(priorityqueue,quantum,n,0);
			n=0;
		}

		priority++;
		if(q==NULL&&priorityqueue==NULL) break;
	}
	printf("WORKLOAD TIME: %f seconds\n",get_wtime()-time1);

	
}
int main(int argc,char **argv)
{
	
	struct process* PROCCS =NULL;
	struct queue *Q =NULL;
	FILE *fp;
	char * FILENAME;
	int n;
	float quantum;
	sscanf(argv[2], "%f", &quantum);

	FILENAME = argv[argc-1];
	fp = fopen(FILENAME,"r");
	
	n=numOfProcess(fp);
	PROCCS=(struct process*)malloc(n*sizeof(struct process));
	
	getProcess(fp,n,PROCCS);

	
	printf("#scheduler %s %s\n",argv[1],argv[argc-1]);
	if(strcmp(argv[1],"BATCH")==0){
		
		
		for(int i=0;i<n;i++) push(&Q,&PROCCS[i]);
		
		Q = gotoHead(Q,n);
		
		_static(Q,1);
		
 		
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
		
		_static(Q,1);
		
 
	}else if(strcmp(argv[1],"RR")==0){
		
		for(int i=0;i<n;i++) push(&Q,&PROCCS[i]);
		
		Q = gotoHead(Q,n);
	
		rr(Q,quantum,n,1);
	
 		

	}else if(strcmp(argv[1],"PRIO")==0){
		MinHeap* heap = init_minheap(n);
		for(int i=0;i<n;i++) insert_minheap(heap,PROCCS[i]);
	
		for(int i=0;i<n;i++){
		struct process *temp = malloc(sizeof(struct process));
		(*temp) = heap->p[0];
		push(&Q,temp);
		heap = delete_minimum(heap);
		}
		Q = gotoHead(Q,n);
		prio(Q,quantum,n);

	}
}