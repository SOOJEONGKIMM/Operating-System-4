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
  if(status!=0){
    /*if unable to create read CV, destroy mutex */
    pthread_mutex_destroy(&rw->mutex);
  }
  status=pthread_cond_init(&rw->write,NULL);
  if(status!=0){
    /*if unable to create write CV, destroy read CV and mutex*/
    pthread_cond_destroy(&rw->read);
    pthread_mutex_destroy(&rw->mutex);
  }
  rw->valid=RWLOCK_VALID;
}

void r_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
  int status;
  status=pthread_mutex_lock(&rw->mutex);
 
    while(rw->w_active>0){//|| rw->w_wait>0){//1nd method
      rw->r_wait++;
      status=pthread_cond_wait(&rw->read, &rw->mutex);
      if(status!=0)
       break;
    }
     rw->r_wait--;
  
  if(status==0)
    rw->r_active++;

    pthread_mutex_unlock(&rw->mutex);

}

void r_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
  pthread_mutex_lock(&rw->mutex);
  
  rw->r_active--;
  if(rw->r_active==0 && rw->w_wait>0)
    pthread_cond_signal(&rw->write);
  pthread_mutex_unlock(&rw->mutex);

}

void w_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
  int status;
  pthread_mutex_lock(&rw->mutex);
  //printf("w_lock: locked\n");
  rw->w_wait++;
  int i=0;
  while(rw->r_active==0 && rw->w_active==0){//first lock_reader prior
    i++;
    pthread_cond_wait(&rw->write, &rw->mutex);
    if(i==1)
    break;
  }
  while(rw->w_active > 0||rw->r_active > 0){
     status=pthread_cond_wait(&rw->write, &rw->mutex);
      if(status!=0)
        break;
    }
  rw->w_wait--; rw->w_active++;
   if(status==0)
    rw->w_active=1;
  pthread_mutex_unlock(&rw->mutex);

}

void w_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
  int status;
  status = pthread_mutex_lock(&rw->mutex);
  
  rw->w_active=0;
  
 
  if(rw->r_wait>0 || rw->r_wait>rw->w_wait){//reader prior
    status=pthread_cond_broadcast(&rw->read);
    if(status!=0){
      pthread_mutex_unlock(&rw->mutex);
    }
  }
  else if(rw->w_wait>0){
    status = pthread_cond_signal(&rw->write);
    if(status!=0)
      pthread_mutex_unlock(&rw->mutex);
  }
 
  pthread_mutex_unlock(&rw->mutex);
 
}
