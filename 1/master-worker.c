#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#define MAX 100000000000
int item_to_produce, curr_buf_size, consume_buf_size;
int total_items, max_buf_size, num_workers, num_masters;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_pro = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_con = PTHREAD_COND_INITIALIZER;


int *buffer;

void print_produced(int num, int master) {

  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);
  
}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);
  curr_buf_size=0;
        
  for(int i=1;i<=MAX;i++)
    {
      pthread_mutex_lock(&mutex);
      printf("producer got lock\n");
     
      if(item_to_produce >= total_items) {//overflow
       printf("wherr\n");
        pthread_mutex_unlock(&mutex);
	     break;
      }
   
      while(curr_buf_size==max_buf_size){
        consume_buf_size=curr_buf_size;
        printf("wait! maxbuf: %d\n",consume_buf_size);
        pthread_cond_wait(&cond_pro, &mutex);
        printf("produce: wake--\n");
      }
       //  pthread_cond_wait(&cond1, &mutex1);
      buffer[curr_buf_size++] = item_to_produce;
      print_produced(item_to_produce, thread_id);
      item_to_produce++;
      

      pthread_cond_signal(&cond_con);
      pthread_mutex_unlock(&mutex);

    }
  
     printf("producer ends\n"); fflush(stdout);
    //   pthread_exit(0);
  return 0;
}

void *generate_workers_loop(void *data)//consumer
{
  int thread_id = *((int *)data);
  int overflowflag=0;
 for(int i=1;i<=MAX;i++)
  {
      pthread_mutex_lock(&mutex);
   //   printf("consumer got lock\n");
   //  printf("item:%d. buf:%d\n",item_to_produce,curr_buf_size);
   printf(" ");
   fflush(stdout);
       if(item_to_produce >= total_items) {//overflow
      // printf("consumer enough\n");fflush(stdout);
        pthread_mutex_unlock(&mutex);
	     break;
      }
      if(curr_buf_size<0){//overflow  
    //  printf("currsize <0\n");
      pthread_mutex_unlock(&mutex);fflush(stdout);
        break;
      }
    while(curr_buf_size == 0){
      printf("consume:wait! empty\n");
       pthread_cond_wait(&cond_con, &mutex);
      printf("consume:wake--.\n");fflush(stdout);
    }
    
     --curr_buf_size;
    print_consumed(buffer[curr_buf_size], thread_id);
   printf("conbuf:%d\n",curr_buf_size);
   
   pthread_cond_signal(&cond_pro);
    pthread_mutex_unlock(&mutex);
    
  }
  printf("consumer ends\n"); fflush(stdout);
  //pthread_exit(0);
  return 0;
}
//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item

int main(int argc, char *argv[])
{
 // pthread_cond_init(cond, NULL);
  pthread_mutex_init(&mutex, NULL);

  int *master_thread_id;
  pthread_t *master_thread;
  int *worker_thread_id;
  pthread_t *worker_thread;
  item_to_produce = 0;
  curr_buf_size = 0;
  
  int i;
  
   if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);//P
    num_workers = atoi(argv[3]);//C
    total_items = atoi(argv[1]);//M
    max_buf_size = atoi(argv[2]);//N
  }
   

   buffer = (int *)malloc (sizeof(int) * max_buf_size);

   //create master producer threads
   master_thread_id = (int *)malloc(sizeof(int) * num_masters);
   master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);

  for (i = 0; i < num_masters; i++){
    master_thread_id[i] = i;
  }

  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
  
  //create worker consumer threads
   worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
   worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);

    for (i = 0; i < num_workers; i++)
    worker_thread_id[i] = i;
  for (i = 0; i < num_workers; i++)
    pthread_create(&worker_thread[i], NULL, generate_workers_loop, (void *)&worker_thread_id[i]);
       
  //wait for all threads to complete
    for (i = 0; i < num_masters; i++)
    {

      pthread_join(master_thread[i], NULL);
      printf("master %d joined\n", i);
    }
   
   
  for (i = 0; i < num_workers; i++)
    {
      pthread_join(worker_thread[i], NULL);
      printf("worker %d joined\n", i);
    }

    //pthread_mutex_destroy(&mutex);
  /*  pthread_cond_destroy(cond_pro);
    pthread_cond_destroy(cond_con);*/
  /*----Deallocating Buffers---------------------*/
 /* free(buffer);
  free(master_thread_id);
  free(master_thread);
  free(worker_thread_id);
  free(worker_thread);*/
  
  return 0;
}
