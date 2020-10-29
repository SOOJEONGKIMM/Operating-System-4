#include "rw_lock.h"

int init_rwlock(struct rw_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  int status;
  rw->r_active=0;
  rw->r_wait=rw->w_wait=0;
  rw->w_active=0;
  status=pthread_mutex_init(&rw->mutex,NULL);
  if(status!=0)
    return status;
  status=pthread_cond_init(&rw->read,NULL);
  if(status!=0){
    /*if unable to create read CV, destroy mutex */
    pthread_mutex_destroy(&rw->mutex);
    return status;
  }
  status=pthread_cond_init(&rw->write,NULL);
  if(status!=0){
    /*if unable to create write CV, destroy read CV and mutex*/
    pthread_cond_destroy(&rw->read);
    pthread_mutex_destroy(&rw->mutex);
    return status;
  }
  rw->valid=RWLOCK_VALID;
  return 0;

}

int r_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
  int status;
  if(rw->valid != RWLOCK_VALID)
    return EINVAL;
  status=pthread_mutex_lock(&rw->mutex);
  if(status!=0)
    return status;
  if(rw->w_active){
    rw->r_wait++;
    //pthread_cleanup_push(rw_readcleanup, (void*)rw);
  //  rwlock_t *rw = (rwlock_t *)arg;
  rw->r_wait--;
  pthread_mutex_unlock(&rw->mutex);

    while(rw->w_active){
      status=pthread_cond_wait(&rw->read, &rw->mutex);
      if(status!=0)
       break;
    }
    pthread_cleanup_pop(0);
    rw->r_wait--;
  }
  if(status==0)
    rw->r_active++;
    pthread_mutex_unlock(&rw->mutex);
    return status;
}

int r_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
  int status, status2;
  if(rw->valid != RWLOCK_VALID)
    return EINVAL;
  status=pthread_mutex_lock(&rw->mutex);
  if(status!=0)
    return status;
  rw->r_active--;
  if(rw->r_active==0 && rw->w_wait>0)
    status=pthread_cond_signal(&rw->write);
  status2=pthread_mutex_unlock(&rw->mutex);
  return (status2==0 ? status : status2);
}

int w_lock(struct rw_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
  int status;
  if(rw->valid!=RWLOCK_VALID)
    return EINVAL;
  status=pthread_mutex_lock(&rw->mutex);
  if(status!=0)
    return status;
  if(rw->w_active||rw->r_active>0){
    rw->w_wait++;
    //pthread_cleanup_push(rw_writecleanup, (void*)rw);
    //rwlock_t *rw = (rwlock_t *)arg;
  rw->w_wait--;
  pthread_mutex_unlock(&rw->mutex);
    while(rw->w_active || rw->r_active >0){
      status=pthread_cond_wait(&rw->write, &rw->mutex);
      if(status!=0)
        break;
    }
    pthread_cleanup_pop(0);
    rw->w_wait--;
  }
  if(status==0)
    rw->w_active=1;
  pthread_mutex_unlock(&rw->mutex);
  return status;
}

int w_unlock(struct rw_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
  int status;
  if(rw->valid != RWLOCK_VALID)
    return EINVAL;
  status = pthread_mutex_lock(&rw->mutex);
  if(status!=0)
    return status;
  rw->w_active=0;
  if(rw->r_wait > 0){
    status = pthread_cond_broadcast(&rw->read);
    if(status!=0){
      pthread_mutex_unlock(&rw->mutex);
      return status;
    }
  }
  else if(rw->w_wait > 0){
    status=pthread_cond_signal(&rw->write);
    if(status!=0){
      pthread_mutex_unlock(&rw->mutex);
      return status;
    }
  }
  status=pthread_mutex_unlock(&rw->mutex);
  return status;
}
