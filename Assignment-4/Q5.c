#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

struct ListNode{
  struct ListNode* prev;
  struct ListNode* next;
  void* data;
};

typedef struct ListNode ListNode_t;

typedef struct{
  ListNode_t* head;
  ListNode_t* tail;
  pthread_mutex_t* list_mutex;
}DoubleList_t;

DoubleList_t* makeList();

int dataLT(void* data1, void* data2);
int dataEQ(void* data1, void* data2);

/*
 * STUDENTS, implement this insert function in a thread-safe manner.
 */
void insert(DoubleList_t* ls, void* data);
ListNode_t* find(DoubleList_t* ls, void* data);
ListNode_t* delete(DoubleList_t* ls, void* data);
int isEmpty(DoubleList_t* ls);


#define NUM_KEYS 256
int* keys;

void init_keys();
int* init_idxs();

/*
 * Pthreads stuff.
 */
#define SMALL_STACK 131072 /* 128K for stack */
pthread_attr_t thread_attr;
pthread_t* make_thread(void *(*start_fn)(void *), void* arg);

void* find_fn(void* arg);
void* insert_fn(void* arg);
void* delete_fn(void* arg);

/* Usage: doubleList <num_inserts> <num_deletes> <num_finds> */
int main(int argc, char** argv){
  long num_inserts;
  long num_deletes;
  long num_finds;

  if(argc < 4){
    printf("Usage: doubleList <num_inserts> <num_deletes> <num_finds>\n");
    return 0;
  }

  sscanf(argv[1], "%ld", &num_inserts);
  sscanf(argv[2], "%ld", &num_deletes);
  sscanf(argv[3], "%ld", &num_finds);

  /* initialize thread attributes */
  pthread_attr_init(&thread_attr);
  pthread_attr_setstacksize(&thread_attr, SMALL_STACK);

  init_keys();
  DoubleList_t* ls = makeList();

  int num_threads = num_inserts + num_deletes + num_finds;
  pthread_t** threadz = malloc(sizeof(pthread_t*) * num_threads);
  int idx = 0;
  
  int i;
  for(i = 0 ; i < num_inserts ; i++){
    pthread_t* ins = make_thread(insert_fn, ls);
    threadz[idx++] = ins;
  }

  for(i = 0 ; i < num_finds ; i++){
    pthread_t* fnd = make_thread(find_fn, ls);
    threadz[idx++] = fnd;
  }

  for(i = 0 ; i < num_deletes ; i++){
    pthread_t* del = make_thread(delete_fn, ls);
    threadz[idx++] = del;
  }

  void* val;
  idx = 0;
  while(idx < num_threads){
    pthread_join(*threadz[idx], &val);
    idx++;
  }

  return 0;
}

void init_keys(){
  int i;
  keys = malloc(sizeof(int) * NUM_KEYS);
  
  srandom(42);
  for(i = 0 ; i < NUM_KEYS ; i++){
    keys[i] = random();
  }
}

int* init_idxs(){
  int i;
  int start = random() % NUM_KEYS;
  int* idxs = malloc(sizeof(int) * NUM_KEYS);

  for(i = 0 ; i < NUM_KEYS ; i++){
    idxs[i] = (start + i) % NUM_KEYS;
  }

  return idxs;
}


pthread_t* make_thread(void *(*start_fn)(void *), void* arg){
  pthread_t* thread_info = malloc(sizeof(pthread_t));

  if(!pthread_create(thread_info, &thread_attr, start_fn, arg)){
    return thread_info;
  }

  return NULL;
}

DoubleList_t* makeList(){
  DoubleList_t* ls = malloc(sizeof(DoubleList_t));

  ls->head = NULL;
  ls->tail = NULL;
  ls->list_mutex = malloc(sizeof(pthread_mutex_t));

  if(!pthread_mutex_init(ls->list_mutex, NULL)){
    return ls;
  }
  return NULL;
}

int dataLT(void* data1, void* data2){
  if((long)data1 < (long)data2){
    return 1;
  }
  return 0;
}

int dataEQ(void* data1, void* data2){
  if((long)data1 == (long)data2){
    return 1;
  }
  return 0;
}

ListNode_t* find(DoubleList_t* ls, void* data){

  ListNode_t* prev = NULL; 
  ListNode_t* current;

  if(prev == NULL){}  //to prevent warnings

  pthread_mutex_lock(ls->list_mutex);
  prev = current = ls->head;
  while(current != NULL){
    if(dataEQ(current->data, data)){
      break;
    }

    if(dataLT(data, current->data)){
      break;
    }
    prev = current;
    current = current->next;
  }

  pthread_mutex_unlock(ls->list_mutex);
  return current;
}

void insert(DoubleList_t* ls, void* data){
  
  ListNode_t* prev = NULL; 
  ListNode_t* current;

  if(prev){}  //to prevent warnings

  pthread_mutex_lock(ls->list_mutex);

  //when list is empty
  if((ls->head == NULL) && (ls->tail == NULL))
  {
    ls->head = (ListNode_t*) malloc(sizeof(ListNode_t));
    ls->head->next = NULL;
    ls->head->prev = NULL;
    ls->head->data = data; 
    ls->tail = ls->head;
    pthread_mutex_unlock(ls->list_mutex);
    return;

  }
  else if(ls->head->next == NULL)
  {
    //when there is 1 node in link list
    current = (ListNode_t*) malloc(sizeof(ListNode_t));
    current->next = NULL;
    current->prev = ls->head;
    current->data = data; 
    ls->head->next = current;
    ls->tail = current;
    pthread_mutex_unlock(ls->list_mutex);
    return;
  }
  else 
  {
    prev = current = ls->head;
    while(current != NULL)
    {
      if(dataLT(data, current->data) || dataEQ(data, current->data))
      {
        ListNode_t* temp; 
        temp = (ListNode_t*) malloc(sizeof(ListNode_t));
        temp->data = data;
        
        if(current == ls->head)
        { 
          //insert before head
          ls->head->prev = temp;
          temp->next =  ls->head;
          temp->prev = NULL;
          temp =  ls->head;
          pthread_mutex_unlock(ls->list_mutex);
          return;
        }
        else if(current ==  ls->tail)
        {
          //insert before tail
          temp->next = ls->tail;
          ls->tail->prev->next = temp;
          temp->prev = ls->tail->prev;
          pthread_mutex_unlock(ls->list_mutex);
          return;
        }
        else 
        {
          //inserting middle of the list
          temp->next = current;
          temp->prev = current->prev;
          current->prev = temp;
          pthread_mutex_unlock(ls->list_mutex);
          return;
        }
      }

      prev = current;
      current = current->next;
    }

    //insert after tail pointer
    ls->tail->next = (ListNode_t*) malloc(sizeof(ListNode_t));
    ls->tail->next->data = data;

    ls->tail->next->next = NULL;
    ls->tail->next->prev = ls->tail;
    ls->tail = ls->tail->next;


  }
  pthread_mutex_unlock(ls->list_mutex);
}

ListNode_t* delete(DoubleList_t* ls, void* data){
  ListNode_t* prev = NULL; 
  ListNode_t* current;

  if(prev){}  //to prevent warnings

  pthread_mutex_lock(ls->list_mutex);

  prev = current = ls->head;
  while(current != NULL){
    if(dataEQ(current->data, data)){
      /* This is the guy */

      if(current->next != NULL){
        current->next->prev = prev;
      }else{
	/* need to reset tail */
	ls->tail = prev;
      }

      if(prev != current){
        prev->next = current->next;
      }else{
	/* need to reset head */
	ls->head = current->next;

	if(current->next != NULL)
	  current->next->prev = NULL;
      }

      break;
    }

    if(!dataLT(data, current->data)){
      break;
    }
    prev = current;
    current = current->next;
  }

  pthread_mutex_unlock(ls->list_mutex);
  return current;
}

int isEmpty(DoubleList_t* ls){
  pthread_mutex_lock(ls->list_mutex);
  if((ls->head == NULL) && (ls->tail == NULL)){
    pthread_mutex_unlock(ls->list_mutex);
    return 1;
  }else{
    pthread_mutex_unlock(ls->list_mutex);
    return 0;
  }
}

void* find_fn(void* arg){
  int* idxs = init_idxs();
  int num;
  DoubleList_t* ls = (DoubleList_t*)arg;

  for(num = 0 ; num < NUM_KEYS ; num++){
    printf("finding %d\n", keys[idxs[num]]);
    find(ls, (void*)((long)keys[idxs[num]]));
  }

  return NULL;
}

void* insert_fn(void* arg){
  int* idxs = init_idxs();
  int num;
  DoubleList_t* ls = (DoubleList_t*)arg;

  for(num = 0 ; num < NUM_KEYS ; num++){
    printf("inserting %d\n", keys[idxs[num]]);
    insert(ls, (void*)((long)keys[idxs[num]]));
  }

  return NULL;
}

void* delete_fn(void* arg){
  int* idxs = init_idxs();
  int num;
  DoubleList_t* ls = (DoubleList_t*)arg;

  for(num = 0 ; num < NUM_KEYS ; num++){
    printf("deleting %d\n", keys[idxs[num]]);
    delete(ls, (void*)((long)keys[idxs[num]]));
  }

  return NULL;
}

