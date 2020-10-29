#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

struct rw_lock
{
    pthread_mutex_t mutex;
    pthread_cond_t read;
    pthread_cond_t write;
    int valid;
    int r_active;
    int w_active;
    int r_wait;
    int w_wait;
}rwlock_t;
#define RWLOCK_VALID 0xfacade

void init_rwlock(struct rw_lock * rw);
void r_lock(struct rw_lock * rw);
void r_unlock(struct rw_lock * rw);
void w_lock(struct rw_lock * rw);
void w_unlock(struct rw_lock * rw);
long *max_element(long* start, long* end);
long *min_element(long* start, long* end);
