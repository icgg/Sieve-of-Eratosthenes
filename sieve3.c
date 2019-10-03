#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int nums[40000000];

pthread_mutex_t lock;		//Mutex declaration

int ready = 1;			//Signaling mechanism

int counter = 0;


struct Queue				//Queue data structure
{
    int front, rear, size;
    unsigned capacity;
    int* array;
};

struct Queue* createQueue(unsigned capacity);
int front(struct Queue* queue);
int isEmpty(struct Queue* queue);
void enqueue(struct Queue* queue, int item);
int dequeue(struct Queue* queue);

struct low_high{

   int id;
   int low;
   int high;
   int n;
   int k;
   struct Queue* queue;
};


void *Sieve(void *data){

   int i;
   int j;
   struct low_high *work = (struct low_high *)data;   
 
   struct Queue* queue = (struct Queue *)work->queue;
   int k = work->k;
   
   for(i = work->low; i <= work->high; i++){

	if(i % k == 0){
		nums[i] = 1;

	}
   }

   pthread_mutex_lock(&lock);
   enqueue(queue, work->id);
   pthread_mutex_unlock(&lock);

}

int main(int argc, char *argv[]){


   struct Queue* queue = createQueue(1000); 		//Queue Declaration

   if(argc != 4)
	printf("sieve [number] [num of threads] [chunk size]");

   int num_threads = atoi(argv[2]);
   pthread_t threads[num_threads];
   int n = atoi(argv[1]);
   int chunk_size = atoi(argv[3]);

   int num_chunks = (n - 1)/chunk_size;
   int i;
   int j;
   int k;	//multiples of k will be crossed out in the  num array

/*Loop to round number of chunks up because rounding in c is a pain.
  Results in a little superfluous computation*/

   for(i = n - 1; i < n + chunk_size; i++)	
	if(i % chunk_size == 0){
		num_chunks = i / chunk_size;
		break;
	}


   struct low_high work[num_threads];
   pthread_mutex_init(&lock, NULL);


   for(i = 0; i < num_threads; i++)		//queue up threads, not physical threads but rather numbers representing threadIDs
	enqueue(queue, i);

   int z = (int)sqrt((double)n) + 1;


   for(k = 2; k < z; k++){

	while(nums[k] == 1)			//skipping marked numbers
	 	k++;

	i = 0;
	while(i < num_chunks){

	   //Busy waiting, not ideal, but works
	   if(ready){

	        int process = dequeue(queue);

		work[process].id = process;
		work[process].low = 4 + (chunk_size * i);		//4 becuase 2 and 3 are primes.
		work[process].high = 4 + (chunk_size * i) + (chunk_size - 1);
		work[process].n = n;
		work[process].k = k;
		work[process].queue = queue;

		counter++;
		printf("Calling thread: %d, with number: %d..counter: %d\n", process, k, counter);   //Comment out when ready
		//printf("z = %d", z);

		pthread_create(&threads[process], NULL, Sieve, (void*)&work[process]);
		i++;
	   }

	   if(isEmpty(queue))
		ready = 0;
	   else
		ready = 1;		 //Previous BUG, was not resetting ready signal	

	}

   }

   for (i=0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
    }

   if(nums[n] == 0)
	printf("%d is Prime\n", n);
   else
	printf("%d is not Prime\n", n);

   pthread_mutex_destroy(&lock);

   return 0;

}


struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue -> capacity = capacity;
    queue -> front = queue -> size = 0; 
    queue -> rear = capacity - 1; 
    queue -> array = (int*) malloc(queue -> capacity * sizeof(int));
    return queue;
}

int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return 0;
    return queue->array[queue->front];
}
 
int isEmpty(struct Queue* queue){  
	return (queue->size == 0); 
}
 
void enqueue(struct Queue* queue, int item)
{
    queue -> rear = (queue->rear + 1)%queue->capacity;
    queue -> array[queue->rear] = item;
    queue -> size = queue->size + 1;
}
 
int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return 0;

    int item = queue -> array[queue->front];
    queue -> front = (queue -> front + 1) % queue->capacity;
    queue -> size = queue -> size - 1;
    return item;

}
