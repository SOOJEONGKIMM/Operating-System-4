#include <pthread.h>

typedef struct SSU_Sem {
    pthread_mutex_t mutex;
    pthread_cond_t mycond;
    int counter;
    
} SSU_Sem;
//static const int value = 20;
typedef struct node{
    int thread_id;
    struct node *next;
}node;

void SSU_Sem_init(SSU_Sem *, int);
void SSU_Sem_up(SSU_Sem *);
void SSU_Sem_down(SSU_Sem *);
