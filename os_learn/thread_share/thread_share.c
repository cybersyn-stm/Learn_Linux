#include <pthread.h>
#include <stdio.h>

static volatile int counter = 0;
static pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER;

void *mythread_lock(void *arg) {
    pthread_mutex_lock(&counter_lock);
    printf("%s:begin\n", (char *)arg);
    int i;
    for (i = 0; i < 1e7; i++) {
        counter = counter + 1;
    }
    printf("%s: done\n", (char *)arg);
    pthread_mutex_unlock(&counter_lock);
    return NULL;
}

void *mythread(void *arg) {
    printf("%s:begin\n", (char *)arg);
    int i;
    for (i = 0; i < 1e7; i++) {
        counter = counter + 1;
    }
    printf("%s: done\n", (char *)arg);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p1, p2, p3, p4;
    printf("main: begin (counter = %d)\n", counter);
    pthread_create(&p1, NULL, mythread, "A");
    pthread_create(&p2, NULL, mythread, "B");

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    printf("main: done with both (counter = %d)\n", counter);
    counter = 0;

    pthread_create(&p3, NULL, mythread_lock, "C");
    pthread_create(&p4, NULL, mythread_lock, "D");

    pthread_join(p4, NULL);
    pthread_join(p3, NULL);

    printf("main: done with both (counter = %d)\n", counter);
    return 0;
}
