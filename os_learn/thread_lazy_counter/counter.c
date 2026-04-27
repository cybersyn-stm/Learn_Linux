#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMCPUS 5
int synchronize_counter;

typedef struct counter_t {
    int global;                     // global count
    pthread_mutex_t glock;          // global lock
    int local[NUMCPUS];             // local count
    pthread_mutex_t llock[NUMCPUS]; // local locks
    long int threshold;             // updata frequency
} counter_t;

void init(counter_t *c, int threshold) {
    c->threshold = threshold;
    c->global = 0;
    pthread_mutex_init(&c->glock, NULL);

    int i;
    for (i = 0; i < NUMCPUS; i++) {
        c->local[i] = 0;
        pthread_mutex_init(&c->llock[i], NULL);
    }
}
void update(counter_t *c, int threadID, int amt) {
    pthread_mutex_lock(&c->llock[threadID]);
    c->local[threadID] += amt;
    // 如果本地计数器超过阈值的2倍，认为延时严重，打印"！"
    if (c->local[threadID] >= 2 * c->threshold) {
        printf("[Thread %d] Delay detected! Local: %d, Global: %d ！\n", threadID, c->local[threadID], c->global);
    }
    if (c->local[threadID] >= c->threshold) {
        pthread_mutex_lock(&c->glock);
        c->global += c->local[threadID];
        pthread_mutex_unlock(&c->glock);
        printf("[Thread %d] Flush to global! Global now: %d\n", threadID, c->global);
        c->local[threadID] = 0;
    }
    pthread_mutex_unlock(&c->llock[threadID]);
}

int get(counter_t *c) {
    pthread_mutex_lock(&c->glock);
    int val = c->global;
    pthread_mutex_unlock(&c->glock);
    return val;
}
#define NUM_THREADS NUMCPUS
#define NUM_INCREMENTS 1000000

counter_t counter;

void *worker(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < NUM_INCREMENTS; i++) {
        update(&counter, id, 1);
    }
    return NULL;
}
void destroy(counter_t *c) {
    pthread_mutex_destroy(&c->glock);
    for (int i = 0; i < NUMCPUS; i++) {
        pthread_mutex_destroy(&c->llock[i]);
    }
}

int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    init(&counter, 500000);

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Flush remaining local counts to global
    for (int i = 0; i < NUM_THREADS; i++) {
        update(&counter, i, 0);
    }

    printf("Final counter value: %d\n", get(&counter));
    destroy(&counter);
    return 0;
}
