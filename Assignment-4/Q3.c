#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define SMALL_STACK 131072 //128K for stack
#define BUF_MAX 64

typedef struct{
  void* buf[BUF_MAX];
  int len;
  int start;
} BoundedBuffer;

pthread_mutex_t buf_mutex; //the buffer mutex

BoundedBuffer buffer;

pthread_attr_t thread_attr;

/*
 * STUDENTS: extend and modify these functions.
 */
void* consume(void* arg);
void* produce(void* arg);

int prod_val = 17;

pthread_t* make_thread(void *(*start_fn)(void *), void* arg);

int main(int argc, char** argv){
  long num_ops = 0;
  long num_prods_per = 0;
  long num_cons_per = 0;
  long num_cons, num_prods;

  if(argc < 4){
    printf("Usage: boundedBuffer <num_ops> <num_consumers> <num_producers>\n");
    return 0;
  }

  sscanf(argv[1], "%ld", &num_ops);
  sscanf(argv[2], "%ld", &num_cons);
  sscanf(argv[3], "%ld", &num_prods);

  num_prods_per = num_ops / num_prods;
  num_cons_per = num_ops / num_cons;

  /*
   * Initializing the thread attributes.
   */
  pthread_attr_init(&thread_attr);
  pthread_attr_setstacksize(&thread_attr, SMALL_STACK);

  pthread_mutex_init(&buf_mutex, NULL);

  /*
   * Creating the buffer.
   */
  buffer.len = 0;
  buffer.start = 0;
  int i = 0;
  for(i = 0 ; i < BUF_MAX ; i++){
    buffer.buf[i] = NULL;
  }

  int num_threads = num_cons + num_prods;
  pthread_t** threadz = malloc(sizeof(pthread_t*) * num_threads);
  int idx = 0;

  int num = 0;
  for(num = 0 ; num < num_prods ; num++){
    pthread_t* prod = make_thread(produce, (void*)num_prods_per);
    threadz[idx] = prod;
    idx++;
  }

  for(num = 0 ; num < num_cons ; num++){
    pthread_t* cons = make_thread(consume, (void*)num_cons_per);
    threadz[idx] = cons;
    idx++;
  }

  void* val;
  idx = 0;
  while(idx < num_threads){
    pthread_join(*threadz[idx], &val);
    idx++;
  }

  return 0;
}

pthread_t* make_thread(void *(*start_fn)(void *), void* arg){
  pthread_t* thread_info = malloc(sizeof(pthread_t));

  if(!pthread_create(thread_info, &thread_attr, start_fn, arg)){
    return thread_info;
  }

  return NULL;
}

/*
 * STUDENTS! Add lock/unlock and wait/signal code to this method.
 * Think carefully about exactly where to put the calls.
 */
void* consume(void* arg){
  long num_consumes = (long)arg;

  while(num_consumes > 0){

    pthread_mutex_lock(&buf_mutex);
    buffer.len--;
    buffer.buf[buffer.start] = NULL;
    buffer.start = (buffer.start + 1) % BUF_MAX;
    
    printf("Consumed\n");
    num_consumes--;
    pthread_mutex_unlock(&buf_mutex);
  }

  return NULL;
}

/*
 * STUDENTS! Add lock/unlock and wait/signal code to this method.
 * Think carefully about exactly where to put the calls.
 */
void* produce(void* arg){
  long num_produces = (long)arg;

  void* val = (void*)(&prod_val);

  while(num_produces > 0){

    //Check for full queue and wait

    pthread_mutex_lock(&buf_mutex);
    int idx = (buffer.start + buffer.len) % BUF_MAX;
    buffer.buf[idx] = val;
    buffer.len++;

    printf("Produced\n");
    num_produces--;
    pthread_mutex_unlock(&buf_mutex);
  }

  return NULL;
}
