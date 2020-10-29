#include "rw_lock.h"
#include <pthread.h>

void init_rwlock(struct rw_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  int status;
  rw->r_active=0;
  rw->r_wait=rw->w_wait=0;
  rw->w_active=0;
  status=pthread_mutex_init(&rw->mutex,NULL);
  status=pthread_cond_init(&rw->read,NULL);
  printf("init_\n");
  if(status!=0){
    /*if unable to create read CV, destroy mutex */
    pthread_mutex_destroy(&rw->mutex);
  }
  status=pthread_cond_init(&rw->write,NULL);
  if(status!=0){
    /*if unable to create write CV, destroy read CV and mutex*/
    pthread_cond_destroy(&rw->read);
  //   pthread_cond_destroy(&rw->write);
    pthread_mutex_destroy(&rw->mutex);
   // return status;
  }
  rw->valid=RWLOCK_VALID;
}

void r_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
  int status;
  status=pthread_mutex_lock(&rw->mutex);
  printf("r_lock: locked\n");
  //if(rw->w_active){
    //rw->r_wait++;
  
    while(rw->w_active>0 ){//|| rw->w_wait>0){//1nd method
      rw->r_wait++;
      status=pthread_cond_wait(&rw->read, &rw->mutex);
       rw->r_wait--;
      if(status!=0)
       break;
    }
   
  //}
  if(status==0)
    rw->r_active++;

    pthread_mutex_unlock(&rw->mutex);
 printf("r_lock: unlocked\n");
}

void r_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
  pthread_mutex_lock(&rw->mutex);
  printf("r_unlock: locked\n");
  rw->r_active--;
  if(rw->r_active==0 && rw->w_wait>0)
    pthread_cond_signal(&rw->write);
  pthread_mutex_unlock(&rw->mutex);
  printf("r_unlock: unlocked\n");
}

void w_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
  int status;
  status=pthread_mutex_lock(&rw->mutex);
  printf("w_lock: locked\n");
  rw->w_wait++;
  while(rw->w_active > 0||rw->r_active > 0){
     status=pthread_cond_wait(&rw->write, &rw->mutex);
      if(status!=0)
        break;
    }
    //pthread_cleanup_push(rw_writecleanup, (void*)rw);
    //rwlock_t *rw = (rwlock_t *)arg;
  rw->w_wait--; rw->w_active++;
  // if(status==0)
   // rw->w_active=1;
  pthread_mutex_unlock(&rw->mutex);
  printf("w_lock: unlocked\n");
}

void w_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
  int status;
  status = pthread_mutex_lock(&rw->mutex);
  printf("w_unlock: locked\n");
  rw->w_active--;
  
  /*else if(rw->w_wait > 0){
    status=pthread_cond_signal(&rw->write);
    if(status!=0){
      pthread_mutex_unlock(&rw->mutex);
   //   return status;
    }
  }*/
  if(rw->w_wait>0){
    status = pthread_cond_signal(&rw->write);
    printf("w_status:%d\n",status);
    if(status!=0)
      pthread_mutex_unlock(&rw->mutex);
  }
  else if(rw->r_wait>0){
    status=pthread_cond_broadcast(&rw->read);
    printf("r_status:%d\n",status);
    if(status!=0){
      pthread_mutex_unlock(&rw->mutex);
    }
  }
 
  pthread_mutex_unlock(&rw->mutex);
  printf("w_unlock: unlocked\n");
 // return status;
}
