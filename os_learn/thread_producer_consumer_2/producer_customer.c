#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;

int buffer[MAX];
int fill = 0;
int use = 0;
int count = 0;
int loops = 0;

void put(int value) {
    buffer[fill] = value;
    fill = (fill + 1) % MAX;
    count++;
}

int get() {
    int tmp = buffer[use];
    use = (use + 1) % MAX;
    count--;
    return tmp;
}

void *producer(void *arg) {
    int i;
    for (i = 0; i < loops * 2; i++) {
        pthread_mutex_lock(&mutex);
        while (count == MAX)
            pthread_cond_wait(&empty, &mutex);
        put(i);
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumer(void *arg) {
    int i;
    int thread_id = *(int *)(arg);
    for (i = 0; i < loops; i++) {
        pthread_mutex_lock(&mutex);
        while (count == 0)
            pthread_cond_wait(&full, &mutex);
        int tmp = get();
        printf("The consumer-%d, it get : %d\n", thread_id, tmp);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t p1, c1, c2;
    int c1_id = 1, c2_id = 2;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <loops>\n", argv[0]);
        return 1;
    }

    errno = 0;
    long num = strtol(argv[1], NULL, 10);
    if (errno != 0 || num <= 0 || num > 1000000000L) {
        fprintf(stderr, "Invalid loops: %s\n", argv[1]);
        return 1;
    }
    loops = (int)num;
    printf("The loops: %d\n", loops);

    if (pthread_create(&p1, NULL, producer, NULL) != 0) {
        perror("pthread_create producer");
        return 1;
    }
    if (pthread_create(&c1, NULL, consumer, &c1_id) != 0) {
        perror("pthread_create consumer");
        return 1;
    }

    if (pthread_create(&c2, NULL, consumer, &c2_id) != 0) {
        perror("pthread_create consumer");
        return 1;
    }

    pthread_join(p1, NULL);
    pthread_join(c1, NULL);
    pthread_join(c2, NULL);

    return 0;
}
