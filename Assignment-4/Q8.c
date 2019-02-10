#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

struct ListNode{
  struct ListNode* prev;
  struct ListNode* next;
  void* data;
  pthread_mutex_t*  node_mutex;
};

typedef struct ListNode ListNode_t;

typedef struct{
  ListNode_t* head;
  ListNode_t* tail;
  pthread_rwlock_t* list_mutex;
}DoubleList_t;

DoubleList_t* makeList();
ListNode_t* makeNode(void* data);

int dataLT(void* data1, void* data2);
int dataEQ(void* data1, void* data2);

/*
 * STUDENTS: implement these functions.
 */
ListNode_t* find(DoubleList_t* ls, void* data); /* Q8 */
int 	    size(DoubleList_t* ls); /* Q9 */
ListNode_t* head(DoubleList_t* ls); /* Q11 */

void        insert(DoubleList_t* ls, void* data);
ListNode_t* delete(DoubleList_t* ls, void* data);
int         isEmpty(DoubleList_t* ls);

void ins_traverse(DoubleList_t* ls, void* data,
                  ListNode_t** call_prev, ListNode_t** call_current);
void del_traverse(DoubleList_t* ls, void* data,
                  ListNode_t** call_prev, ListNode_t** call_current);



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
  int num_inserts;
  int num_deletes;
  int num_finds;

  if(argc < 4){
    printf("Usage: doubleList <num_inserts> <num_deletes> <num_finds>\n");
    return 0;
  }

  sscanf(argv[1], "%d", &num_inserts);
  sscanf(argv[2], "%d", &num_deletes);
  sscanf(argv[3], "%d", &num_finds);

  /* initialize thread attributes */
  pthread_attr_init(&thread_attr);
  pthread_attr_setstacksize(&thread_attr, SMALL_STACK);

  init_keys();
  DoubleList_t* ls = makeList();
  pthread_rwlock_wrlock(ls->list_mutex);

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

  /* Release the hounds! */
  fflush(stdout);
  pthread_rwlock_unlock(ls->list_mutex);

  void* val;
  idx = 0;
  while(idx < num_threads){
    pthread_join(*threadz[idx], &val);
    idx++;
    fflush(stdout);
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
  ls->list_mutex = malloc(sizeof(pthread_rwlock_t));

  if(!pthread_rwlock_init(ls->list_mutex, NULL)){
    return ls;
  }
  return NULL;
}

ListNode_t* makeNode(void* data){
  ListNode_t* node = malloc(sizeof(ListNode_t));
  node->prev = NULL;
  node->next = NULL;
  node->data = data;
  node->node_mutex = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(node->node_mutex, NULL);

  return node;
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

  pthread_rwlock_rdlock(ls->list_mutex);
  prev = current = ls->head;
  while(current != NULL){

    pthread_mutex_lock(current->node_mutex);
    if(dataEQ(current->data, data)){
      break;
    }
    if(dataLT(data, current->data)){
      break;
    }

    if(prev != current)
      pthread_mutex_unlock(prev->node_mutex);
    
    prev = current;
    current = current->next;
  }

  if(current != NULL)
    pthread_mutex_unlock(current->node_mutex);
  if((prev != NULL) && (prev !=current))
    pthread_mutex_unlock(prev->node_mutex);

  pthread_rwlock_unlock(ls->list_mutex);
  return current;
}

/* Finds the insertion point in the list for data. Uses hand-over-hand
 * locking to ensure mutual exclusion.
 */
void ins_traverse(DoubleList_t* ls, void* data,
	          ListNode_t** call_prev, ListNode_t** call_current){
  ListNode_t* prev, *current;

  prev = current = ls->head;

  while(current != NULL){
    pthread_mutex_lock(current->node_mutex);
    if(dataLT(data, current->data)){
      break;
    }
    if(prev != current){
      pthread_mutex_unlock(prev->node_mutex);
    }
    prev = current;
    current = current->next;
  }

  *call_prev = prev;
  *call_current = current;
}

void insert(DoubleList_t* ls, void* data){
  ListNode_t* prev;
  ListNode_t* current; 
  ListNode_t* val = makeNode(data);
  int hold_wr = 0;

  pthread_rwlock_rdlock(ls->list_mutex);

  while(1){
    ins_traverse(ls, data, &prev, &current);

    val->next = current;
    val->prev = prev;

    if(((prev == NULL) || (prev == current) || 
        (current == NULL)) && (hold_wr == 0)){
      /*need to modify the whole list (need to change head and/or tail)
       *release our locks (so we don't deadlock), and then
       * upgrade our list lock
       */
      hold_wr = 1;
      /* Release node locks */
      if(prev != NULL){
        pthread_mutex_unlock(prev->node_mutex);
      }

      if(current != NULL){
        pthread_mutex_unlock(current->node_mutex);
      }

      /* upgrade the whole list lock */
      pthread_rwlock_unlock(ls->list_mutex);
      pthread_rwlock_wrlock(ls->list_mutex);
      /* re-traverse the list */
      continue;
    }

    /* Actually inserting the node at the current location */

    if((prev != NULL) && (prev != current)){
      /* normal insertion somewhere in the middle of the list */
      prev->next = val;
    }else{
      /* need to add the node to the front of the list */
      ls->head = val;
      val->prev = NULL;
    }

    if(current != NULL){
      /* normal insertion somewhere in the middle of the list */
      current->prev = val;
    }else{
      /* need to add the new node to the end of the list */
      ls->tail = val;
    }

    /* Releasing the node locks, if any are held */
    if(prev != NULL){
      pthread_mutex_unlock(prev->node_mutex);
    }

    if(current != NULL){
      pthread_mutex_unlock(current->node_mutex);
    }

    break;
  }

  pthread_rwlock_unlock(ls->list_mutex);
}

void del_traverse(DoubleList_t* ls, void* data,
		  ListNode_t** call_prev, ListNode_t** call_current){
  ListNode_t* prev, *current;

  prev = current = ls->head;
  while(current != NULL){
    pthread_mutex_lock(current->node_mutex);

    if(dataEQ(current->data, data)){
      break;
    }

    if(dataLT(data, current->data)){
      break;
    }
    if(prev != current){
      pthread_mutex_unlock(prev->node_mutex);
    }

    prev = current;
    current = current->next;
  }

  *call_prev = prev;
  *call_current = current;
}

ListNode_t* delete(DoubleList_t* ls, void* data){
  ListNode_t* prev; 
  ListNode_t* current;
  int hold_wr = 0;

  pthread_rwlock_rdlock(ls->list_mutex);

  while(1){
    del_traverse(ls, data, &prev, &current);

    if(current == NULL){
      if(prev != NULL){
        pthread_mutex_unlock(prev->node_mutex);
      }

      break;
    } 

    if(dataLT(data, current->data)){
      pthread_mutex_unlock(current->node_mutex);
      if(prev != NULL){
	pthread_mutex_unlock(prev->node_mutex);
      }
      break;
    }

    /* This is the guy we need to delete */
    if(((current->next == NULL) || (prev == current)) && 
       (hold_wr == 0)){
      /* need to modify the whole list */
      hold_wr = 1;
      if(prev != NULL){
        pthread_mutex_unlock(prev->node_mutex);
      }

      pthread_mutex_unlock(current->node_mutex);

      /* upgrade the lock */
      pthread_rwlock_unlock(ls->list_mutex);
      pthread_rwlock_wrlock(ls->list_mutex);
      /* re-traverse the list */
      continue;
    }

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

    pthread_mutex_unlock(current->node_mutex);
    if(prev != NULL){
      pthread_mutex_unlock(prev->node_mutex);
    }

    break;
  }

  pthread_rwlock_unlock(ls->list_mutex);
  return current;
}

int isEmpty(DoubleList_t* ls){
  pthread_rwlock_rdlock(ls->list_mutex);
  if((ls->head == NULL) && (ls->tail == NULL)){
    pthread_rwlock_unlock(ls->list_mutex);
    return 1;
  }else{
    pthread_rwlock_unlock(ls->list_mutex);
    return 0;
  }
}

/*
 * STUDENTS, Implement this!
 */
ListNode_t* head(DoubleList_t* ls){
  /* head ALWAYS modifies the list, unless it's empty */

  /*
   * head should remove and return the head of the list. If the
   * list is empty, head should return NULL. Head should use lock
   * upgrading so that a write lock is acquired only on non-empty
   * lists.
   */

  return NULL; //just so it compiles
}

/*
 * STUDENTS, implement this!
 */
int size(DoubleList_t* ls){
  /*
   * size should return the size of the list. In order to calculate
   * this, size will have to traverse the list from front to back
   * using hand-over-hand locking. size should lock the list appropriately
   * to ensure that the size doesn't change while calculating list size.
   */

  return -1; //just so it compiles
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

