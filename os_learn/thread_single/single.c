#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <strings.h>

sem_t single; // single thread

void *child(void *arg) {
    printf("child\n");
    sem_post(&single);
    return NULL;
}

int main(int argc, char *argv[]) {
    sem_init(&single, 0, 0);
    printf("parent:begin\n");
    pthread_t c;
    pthread_create(&c, NULL, child, NULL);
    sem_wait(&single);
    printf("parent:end\n");
    return 0;
}
