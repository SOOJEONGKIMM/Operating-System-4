#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "SSU_Sem.h"

void SSU_Sem_init(SSU_Sem *s, int value) {//value=first cond value
    int status;
    pthread_mutex_init(&s->mutex, NULL);
    status=pthread_cond_init(&s->mycond, NULL);
    s->counter=value;

    if(status!=0){
    /*if unable to create write CV, destroy read CV and mutex*/
    pthread_cond_destroy(&s->mycond);
    pthread_mutex_destroy(&s->mutex);
    }

    
}

void SSU_Sem_down(SSU_Sem *s) {
       pthread_mutex_lock(&s->mutex);
  
    while(s->counter ==  0){
        pthread_cond_wait(&s->mycond, &s->mutex);
    }
    --(s->counter);
   // printf("down: counter:%d\n",s->counter);

    pthread_mutex_unlock(&s->mutex);

}

void SSU_Sem_up(SSU_Sem *s) {
 
  
    pthread_mutex_lock(&s->mutex);
    ++(s->counter);
     //printf("up: counter:%d\n",s->counter);
  
    pthread_mutex_unlock(&s->mutex);
    pthread_cond_signal(&s->mycond);

}