#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_CONNECTION_QUEUE_LEN 1024
#define PORT 10237
#define MAX_BUFFER 10
#define MAX_WORKERS 10

/* Simple error check for read/write */
void check(int r, char *s) {
  if(r < 0) {
    perror(s);
    exit(1);
  }
}

/* Hashtable structure definitions */
typedef struct _hashnode_t {
  char *key;
  char *value;
  struct _hashnode_t *next;
} hashnode_t;

typedef struct _hash_t {
  hashnode_t **buckets;
  int size;
  pthread_mutex_t *locks;
} hashtable_t;

/*
* Read a null-terminated string from a socket
*
* Inputs:
*     int fd: the socket representing the connection to the client
*     char *buffer: the buffer that will hold the string
*
*/
void readString(int fd, char *buffer) {
    int i = 0;
    while (1) {
      if(i > sizeof(buffer))
        break;
      
      check(read(fd, &buffer[i], sizeof(char)), "read");

      if (buffer[i] == '\0')
        break;
      else
        i++;
    }
}

void writeString(char *s, int socket_fd) {
  check(write(socket_fd, s, strlen(s) + 1), "write");
}

/*
* Hash a string to an integer
*
* Input
*    char *s: the string to hash
* Output
*    The integer hash code
*/
unsigned long int hash(char *s) {
  unsigned long int h = 0;

  int i;
  for (i = 0; i < strlen(s); i++) {
    h += (unsigned long int) s[i];
  }

  return h;
}


/*
* Create a new hashtable_t
*
* Input
*     tableSize: the number of buckets in the new table
* Output
*    A pointer to the new table
*/
hashtable_t *hashtableInit(int tableSize) {
  
  if(tableSize < 1)
    return NULL;
  
  hashtable_t *newTable = malloc(sizeof(hashtable_t));
  newTable->size = tableSize;
  
  //initialize buckets and locks
  newTable->buckets = malloc(sizeof(hashnode_t) * tableSize);
  newTable->locks = malloc(sizeof(pthread_mutex_t) * tableSize);

  int i;
  for(i = 0; i < tableSize; i++) {
    //newTable->buckets[i] = malloc(sizeof(hashnode_t));
    newTable->buckets[i] = NULL;
    
    //Initialize locks
    int check = pthread_mutex_init(&newTable->locks[i], NULL);
    if(check != 0) {
      perror("Error: mutex initialization\n");
      exit(1);
    }
  }
  
  return newTable;
}


/*
* Insert a key-value pair into a table
*
* Inputs
*    hashtable *h: the hashtable performing the insertion
*    int connection_fd: the socket connection to the client
* Output
*    Nothing; a response indicating success (0) or failure (-1) is sent
*    back to the client
*/
void hashtableInsert(hashtable_t *h, int connection_fd) {
  //Set up check
  int err = 0;
  
  //Hash the input and determine the bucket that will hold the new pair
  char *buffKey = malloc(sizeof(char) * MAX_BUFFER);
  char *buffVal = malloc(sizeof(char) * MAX_BUFFER);
  readString(connection_fd, buffKey);
  readString(connection_fd, buffVal);
  
  unsigned long int hashCode = hash(buffKey);

  //Create a new hashnode_t
  hashnode_t *newNode = malloc(sizeof(hashnode_t));
  newNode->key = buffKey;
  newNode->value = buffVal;
  newNode->next = NULL;
  
  //Find appropriate bucket for node
  int b;
  if(h->size != 0)
    b = hashCode % h->size;
  else
    err = 1;

  //Acquire the bucket's lock before inserting
  pthread_mutex_lock(&h->locks[b]);

  //If bucket is empty, new node becomes head of linked list
  if(h->buckets[b] == NULL)
    h->buckets[b] = newNode;
  
  //Else, insert the node at the end of the bucket's list
  else {
    //Keep track of place in list
    hashnode_t *curr = h->buckets[b];

    //Find end of list
    while(curr->next != NULL)
      curr = curr->next;
    
    //Insert node at end
    curr->next = newNode;
  }
  
  //Release the bucket's lock after the insertion is done
  pthread_mutex_unlock(&h->locks[b]);
  
  //Write success to client
  check(write(connection_fd, &err, sizeof(int)), "Hashtable insertion confirmation");
}


/*
* Lookup the value for a given key and send it back to the client
*
* Inputs
*    hashtable *h: the hashtable performing the insertion
*    int connection_fd: the socket representing the connection to the client
* Output
*    Nothing; the value string is sent to the client over the socket
     returns a NULL string if the key is not in the table
*/
void hashtableLookup(hashtable_t *h, int connection_fd) {
  //Get key from client
  char buffKey[MAX_BUFFER];
  readString(connection_fd, buffKey);
  
  //Default reply to client
  char *reply = "\0";
  
  //Find bucket where the node would be
  unsigned long int hashCode = hash(buffKey);
  int b = hashCode % h->size;
  
  pthread_mutex_lock(&h->locks[b]);

  //If bucket is empty, return NULL
  if(h->buckets[b] == NULL) {
    writeString(reply, connection_fd);
    pthread_mutex_unlock(&h->locks[b]);
    return;
  }
  
  //Else search for key in bucket
  hashnode_t *curr = h->buckets[b];
  while(curr != NULL) {
    if(strcmp(curr->key, buffKey) == 0) {
      writeString(curr->value, connection_fd);
      pthread_mutex_unlock(&h->locks[b]);
      return;
    }
      
    else
      curr = curr->next;
  }
  
  //If no match found return NULL
  writeString(reply, connection_fd);
  pthread_mutex_unlock(&h->locks[b]);
}


/*
* Remove the key-value pair with the given key from the table. The value string
* is returned to the client, or a NULL string is returned if the key is not
* in the table.
*
* Inputs
*    hashtable *h: the hashtable performing the insertion
*    int connection_fd: the socket representing the connection to the client
* Output
*    Nothing; the value string is sent to the client over the socket
*/
void hashtableRemove(hashtable_t *h, int connection_fd) {
  //Read key from client
  char buffKey[MAX_BUFFER];
  readString(connection_fd, buffKey);
  
  //Default eply to client
  char *reply = "\0";
  
  //Find bucket where the node would be
  unsigned long int hashCode = hash(buffKey);
  int b = hashCode % h->size;
  
  //lock bucket
  pthread_mutex_lock(&h->locks[b]);

  //If bucket is empty, return NULL
  if(h->buckets[b] == NULL) {
    writeString(reply, connection_fd);
    pthread_mutex_unlock(&h->locks[b]);
    return;
  }
  
  //Else search for key in bucket
  hashnode_t *curr = h->buckets[b];
  hashnode_t *prev = malloc(sizeof(hashnode_t));
  prev = NULL;
  
  while(curr != NULL) {
    
    //if value is found
    if(strcmp(curr->key, buffKey) == 0) {

      writeString(curr->value, connection_fd);

      //Remove node's reference and free it
      if(prev != NULL) {
        prev->next = curr->next;
        free(curr);
      }
        
      //If node is the head of the list
      else if(prev == NULL) {
        free(curr);
        h->buckets[b] = NULL;
      }
      
      //unlock bucket
      pthread_mutex_unlock(&h->locks[b]);
      return;
    }
      
    else
      prev = curr;
      curr = curr->next;
  }
  
  //If no match found return NULL and unlock bucket
  pthread_mutex_unlock(&h->locks[b]);
  
  writeString(reply, connection_fd);
}


/*
* Print the hastable to the server's console
*
* Input:
*     hastable_t *h: pointer to the table
*/
void hashtablePrint(hashtable_t *h) {
  int i;
  for (i = 0; i < h->size; i++) {
    hashnode_t *node = h->buckets[i];
    printf("Contents of bucket %d:\n", i);
    while (node != NULL) {
      printf("\t<%s, %s>\n", node->key, node->value);
      node = node->next;
    }
  }
}

void processRequest(hashtable_t *table, int connection_fd) {
    //Read the first char from connection_fd to identify the command
    char command;
    check(read(connection_fd, &command, sizeof(char)), "read command");
    
    //Decode the command and call the appropriate hash table function
    switch(command) {
      //Insert
      case 'I':
        printf("Client request: hashtable insert...\n");
        hashtableInsert(table, connection_fd);
      break;
      
      //Lookup
      case 'L':
        printf("Client request: hashtable lookup...\n");
        hashtableLookup(table, connection_fd);
      break;
      
      //Remove
      case 'R':
        printf("Client request: hashtable remove...\n");
        hashtableRemove(table, connection_fd);
      break;
      
      //Print
      case 'P':
        printf("Client request: hashtable print...\n");
        hashtablePrint(table);
      break;
      
      //Unrecognized command
      default:
        printf("Unrecognized command recieved\n");
        check(write(connection_fd, "Please use a recognized command\n", MAX_BUFFER), "write");
    }

    //Close the connection
    check(close(connection_fd), "close");
    printf("Client closed\n");
}

int *buffer;

//Mark current fill and use positions in buffer
int posUse = 0;
int posFill = 0;

//Declare three semaphores
sem_t buffEmpty;
sem_t buffFull;
sem_t buffMutex;
int pshared = 0;

//Fills the buffer
void buffer_fill(int connection_fd) {
    buffer[posFill] = connection_fd;
    posFill++;
    
    //return to beginning of buffer if full
    if(posFill == MAX_BUFFER)
        posFill = 0;
}

//Retrieves from the buffer
int buffer_take() {
    int tmp = buffer[posUse];
    posUse++;
    if (posUse == MAX_BUFFER)
	    posUse = 0;
    return tmp;
}

//Worker threads process requests from buffer
void * worker(void *arg) {
    int tmp = 0;
    hashtable_t *h = (hashtable_t *) arg;
    
    //Take from buffer
    while(1) {
        //wait for item to appear in buffer
        sem_wait(&buffFull);
        
        //lock the buffer
        sem_wait(&buffMutex);
        
        //get the next item
        tmp = buffer_take();
        
        //process item
        processRequest(h, tmp);
        
        //release buffer lock
        sem_post(&buffMutex);
        
        //signal that a free spot has opened
        sem_post(&buffEmpty);
    }
    
    return NULL;
}

/* Main initializes the server and has it listen for clients. Clients are put into a buffer. */
int main() {
    //Create the socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return(-1);
    }
    
    //Initialize the sockaddr structure
    int port = PORT;
    
    struct sockaddr_in sa;
    bzero((char *) &sa, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port = htons((unsigned short) port);
    
    //Bind the socket descriptor to the address
    int rc = bind(server_fd, (struct sockaddr*) &sa, sizeof(sa));
    if (rc < 0) {
        perror("bind");
        return(-1);
    }
    
    //Make the socket listen for incoming requests
    int max_queue_length = MAX_CONNECTION_QUEUE_LEN;
    
    rc = listen(server_fd, max_queue_length);
    if (rc < 0) {
        perror("listen");
        return(-1);
    }
    
    //Initialize a new hash table
    hashtable_t *h = hashtableInit(25);
    
    //Initialize buffer
    buffer = (int *) malloc(MAX_BUFFER * sizeof(int));
    int i;
    for (i = 0; i < MAX_BUFFER; i++)
	   buffer[i] = 0;
    
    //Initialize semaphores
    sem_init(&buffEmpty, pshared, MAX_BUFFER); // max are empty 
    sem_init(&buffFull, pshared, 0);    // 0 are full
    sem_init(&buffMutex, pshared, 1);   // mutex
    
    //Loop, accepting and processing incoming messages
    printf("Server is running...\n\n");
    
    //Create threads to process incoming requests
    pthread_t workers[MAX_WORKERS];
    for(i = 0; i < MAX_WORKERS; i++)
        pthread_create(&workers[i], NULL, worker, h);
    
    while (1) {
        //accept blocks until a client connection arrives
        socklen_t sa_len = sizeof(sa);
        int connection_fd = accept(server_fd, (struct sockaddr*) &sa, &sa_len);
        if (connection_fd < 0) {
          perror("accept");
          return(-1);
        }
        
        /*Add clients to buffer*/

        //wait for buffer spot to open
        sem_wait(&buffEmpty);
        
        //lock the buffer
        sem_wait(&buffMutex);
        
        //Insert a new item
        buffer_fill(connection_fd);
        
        //Release the buffer lock
        sem_post(&buffMutex);
        
        //Signal that a new item is present
        sem_post(&buffFull);
    }
    
    printf("No more clients\n\n");
    
    /*
    //wait for all threads to complete
    for(i = 0; i < MAX_WORKERS; i++)
        pthread_join(workers[i], NULL);
    */
    
    return 0;
}