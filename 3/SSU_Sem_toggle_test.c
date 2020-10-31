#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "SSU_Sem.h"

#define NUM_THREADS 3
#define NUM_ITER 10

struct SSU_Sem t[NUM_THREADS];

void *justprint(void *data)
{
  int thread_id = *((int *)data);
  
  for(int i=0; i < NUM_ITER; i++)
    {
      SSU_Sem_up(&t[i]);
      printf("This is thread %d\n", thread_id);
      SSU_Sem_down(&t[i+1]);
    }
  
  return 0;
}

int main(int argc, char *argv[])
{

  pthread_t mythreads[NUM_THREADS];
  int mythread_id[NUM_THREADS];

  for(int i =0; i < NUM_THREADS; i++){
    SSU_Sem_init(&t[i],0);
  }
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      mythread_id[i] = i;
      pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
          SSU_Sem_down(&t[0]); 
    }
     
      for(int i=0; i < NUM_ITER; i++){
      SSU_Sem_up(&t[i]);
      sleep(1);
       SSU_Sem_down(&t[i]);
     }
     for(int i=0; i < NUM_THREADS; i++){
     sleep(1);
     SSU_Sem_up(&t[NUM_ITER]);
     }
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      pthread_join(mythreads[i], NULL);
    }
  
  return 0;
}
