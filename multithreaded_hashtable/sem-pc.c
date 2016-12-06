#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

int max;
int loops;
int *buffer;
int pshared = 0;

int use  = 0;
int fill = 0;

// Declare three semaphores
sem_t empty;
sem_t full;
sem_t mutex;

#define CMAX (10)
int consumers = 1;

void do_fill(int value) {
    buffer[fill] = value;
    fill++;
    if (fill == max)
	fill = 0;
}


int do_get() {
    int tmp = buffer[use];
    use++;
    if (use == max)
	use = 0;
    return tmp;
}


void * producer(void *arg) {

    int i;
    for (i = 0; i < loops; i++) {

       // Wait for a buffer spot to open
	   sem_wait(&empty);

       // Lock the buffer
	   sem_wait(&mutex);

       // Insert a new item
	   do_fill(i);

       // Release the buffer lock
	   sem_post(&mutex);

       // Signal that a new item is present
	   sem_post(&full);
    }

    // Write -1s to the buffer to signal the
    // consumers to end
    for (i = 0; i < consumers; i++) {
	   sem_wait(&empty);
	   sem_wait(&mutex);
	   do_fill(-1);
	   sem_post(&mutex);
	   sem_post(&full);
    }

    return NULL;
}
                                                                               
void * consumer(void *arg) {
    int tmp = 0;
    int num = * ((int *) arg);
    
    while (tmp != -1) {

       // Wait for an item to appear in the buffer
	   sem_wait(&full);

       // Lock the buffer
	   sem_wait(&mutex);

       // Get the next item
	   tmp = do_get();

       // Release the buffer lock
	   sem_post(&mutex);

       // Signal that a free spot has opened
	   sem_post(&empty);
	   printf("%d %d\n", num, tmp);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
	   fprintf(stderr, "usage: %s <buffersize> <loops> <consumers>\n", argv[0]);
	   exit(1);
    }
    max   = atoi(argv[1]);
    loops = atoi(argv[2]);
    consumers = atoi(argv[3]);
    assert(consumers <= CMAX);

    buffer = (int *) malloc(max * sizeof(int));
    int i;
    for (i = 0; i < max; i++) {
	   buffer[i] = 0;
    }

    // Initialize semaphores
    sem_init(&empty, pshared, max); // max are empty 
    sem_init(&full, pshared, 0);    // 0 are full
    sem_init(&mutex, pshared, 1);   // mutex

    // Create threads
    pthread_t pid, cid[CMAX];
    pthread_create(&pid, NULL, producer, NULL); 
    for (i = 0; i < consumers; i++) {
	   pthread_create(&cid[i], NULL, consumer, &i); 
    }

    // Wait for all threads to complete
    pthread_join(pid, NULL); 
    for (i = 0; i < consumers; i++) {
	   pthread_join(cid[i], NULL); 
    }
    return 0;
}