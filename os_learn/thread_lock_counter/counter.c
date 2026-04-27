#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct counter_t {
    int value;
    pthread_mutex_t lock;
} counter_t;

void init(counter_t *c) { c->value = 0; }

void increment(counter_t *c) { c->value++; }

void decrement(counter_t *c) { c->value--; }

int get(counter_t *c) {
    int rc = c->value;
    return rc;
}

void *mythread(void *arg) {
    counter_t *c = (counter_t *)arg;
    counter_t *local = malloc(sizeof(counter_t));
    init(local);
    for (int i = 0; i < 1e7; i++) {
        increment(local);
    }
    /*for (int i = 0; i < 1e7; i++) {
        decrement(&local);
    }*/
    pthread_mutex_lock(&c->lock);
    c->value += local->value;
    pthread_mutex_unlock(&c->lock);
    free(local);
    return NULL;
}

int main() {
    pthread_t p1, p2;
    counter_t counter;
    int ret;
    init(&counter);
    pthread_create(&p1, NULL, mythread, &counter);
    pthread_create(&p2, NULL, mythread, &counter);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("counter value: %d\n", get(&counter));
}
