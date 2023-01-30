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
//add shared memory for time
static volatile int finishedProcFlag = 0;

/* definition and implementation of process descriptor and queue(s) */


struct msg_buffer{
	long mesg_type;
	struct queue *q;
};

typedef struct msg_buffer message;
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
void sigchld_handler(int signo){
	int pid, status;
	
	

	while ((pid = waitpid(-1, &status, WNOHANG)) > 0){
		finishedProcFlag=1;
	}
	

	
}

double get_wtime(void)
{
  struct timeval t;
  gettimeofday(&t, NULL);
  return (double)t.tv_sec + (double)t.tv_usec*1.0e-6;
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
struct queue * pushHeadtoTail(struct queue *requestQ,int n){
	
		if (n<=1) return requestQ;
		struct process *procc = requestQ->p;
		pop(&requestQ);
		
		requestQ = gotoTail(requestQ,n-1);
		push(&requestQ,procc);
		requestQ=gotoHead(requestQ,n);
		return requestQ;
}

void rr(struct queue *q,float quantum,int size,int flag){
	struct time *time;
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);
	sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, NULL);
	
	struct queue *requestQ=NULL;
	int n =0;
	message message;
	int fd = shm_open("/my_shared_struct", O_CREAT | O_RDWR, 0666);

    // Set the size of the shared memory region
    ftruncate(fd, sizeof(struct queue*));

    // Map the shared memory region to a pointer
    requestQ = mmap(NULL, sizeof(struct queue*), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


	int pid; // process id
	
	
	// Set the size of the shared memory region
    ftruncate(fd, sizeof(struct time*));

    // Map the shared memory region to a pointer
    time = mmap(NULL, sizeof(struct time*), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


	float t_quantum = quantum/1000;
	struct timespec rqtp,rmtp = {0,quantum};
	
	time->_1=get_wtime();
	while(1){
		
			
			while( q!=NULL  ) {
				
				requestQ= gotoTail(requestQ,n);
				push(&requestQ,q->p);
				n++;
				requestQ= gotoHead(requestQ,n);
				pop(&q);
				}
				
				if(requestQ->p->pid==0){ 
					
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
					sigprocmask(SIG_BLOCK, &mask, NULL);

					time1 =get_wtime();
					kill(requestQ->p->pid,SIGCONT);
					sleep(1);
					kill(requestQ->p->pid,SIGSTOP);
					sigprocmask(SIG_UNBLOCK, &mask, NULL);
					
					time2 = get_wtime();
					
					double ntime = time2-time1;
					requestQ->p->time2 += ntime;
					if(finishedProcFlag==1&&requestQ!=NULL){
						printf("PID %d - CMD: %s\n",requestQ->p->pid,requestQ->p->name);
						printf("\t\t\t\t Elapsed time = %f\n",requestQ->p->time2-requestQ->p->time1);
						time->_2 = get_wtime() ;
						if(flag==1)
						printf("\t\t\t\t Workload time = %f\n",  time->_2-time->_1);
						pop(&requestQ);
						n--;
						finishedProcFlag=0;
				}else{
						
					requestQ =pushHeadtoTail(requestQ,n);
					}
					}
					
			if (n==0)break;	
				
	
	}
	
	munmap(requestQ, sizeof(struct queue *));
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
				
				priorityqueue= gotoTail(priorityqueue,n);
				push(&priorityqueue,q->p);
				n++;
				priorityqueue= gotoHead(priorityqueue,n);
				pop(&q);
				}
				
		if(n==1){		
			_static(priorityqueue,0);
			n=0;

		}
		else if (n>1){
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
	double time_1, time_2;
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
		time_1 = get_wtime();
		_static(Q,1);
		time_2 = get_wtime();
 		
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
		_static(Q,1);
		time_2 = get_wtime();
 
	}else if(strcmp(argv[1],"RR")==0){
		
		for(int i=0;i<n;i++) push(&Q,&PROCCS[i]);
		
		Q = gotoHead(Q,n);
		time_1 = get_wtime();
		rr(Q,quantum,n,1);
		time_2 = get_wtime();
 		

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
		
		time_1 = get_wtime();
		prio(Q,quantum,n);
		time_2 = get_wtime();
 		

	}


	
}

