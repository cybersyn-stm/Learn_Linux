#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct thread_t {
    int *value;
    int value_capacity;
    int value_count;
    int thread_id;
    const char *role;
} thread_t;

int *buff = NULL;
int buffer_size = 0;
int items_per_consumer = 0;
int total_items = 0;
int fill = 0;
int use = 0;

void put(int value) {
    buff[fill] = value;
    fill = (fill + 1) % buffer_size;
}

int get() {
    int tmp = buff[use];
    use = (use + 1) % buffer_size;
    return tmp;
}

sem_t empty;
sem_t full;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producer(void *arg) {
    thread_t *thread = (thread_t *)arg;
    for (int i = 0; i < thread->value_capacity; i++) {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        put(i);
        thread->value[i] = i;
        thread->value_count++;
        printf("producer %d: put %d\n", thread->thread_id, i);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    return NULL;
}

void *consumer(void *arg) {
    thread_t *thread = (thread_t *)arg;
    int value;
    for (int i = 0; i < thread->value_capacity; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        value = get();
        thread->value[i] = value;
        thread->value_count++;
        printf("consumer %d: get %d\n", thread->thread_id, value);
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
    return NULL;
}

thread_t *init_thread(int thread_id, int value_capacity, const char *role) {
    thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
    thread->value = (int *)malloc(sizeof(int) * value_capacity);
    if (thread->value == NULL) {
        free(thread);
        return NULL;
    }
    for (int i = 0; i < value_capacity; i++) {
        thread->value[i] = -1;
    }
    thread->value_capacity = value_capacity;
    thread->value_count = 0;
    thread->thread_id = thread_id;
    thread->role = role;
    return thread;
}

thread_t *init_consumer_thread(int thread_id) {
    return init_thread(thread_id, items_per_consumer, "consumer");
}

thread_t *init_producer_thread(int thread_id) {
    return init_thread(thread_id, total_items, "producer");
}

void print_thread_values(const thread_t *thread) {
    printf("%s %d values:", thread->role, thread->thread_id);
    for (int i = 0; i < thread->value_count; i++) {
        printf(" %d", thread->value[i]);
    }
    printf("\n");
}

void destroy_thread(thread_t *thread) {
    if (thread == NULL) {
        return;
    }
    free(thread->value);
    free(thread);
}

int parse_positive_int(const char *s, int *out) {
    char *end = NULL;
    long v = strtol(s, &end, 10);
    if (end == s || *end != '\0' || v <= 0 || v > 1000000) {
        return -1;
    }
    *out = (int)v;
    return 0;
}

int main(int argc, char **argv) {
    int consumer_count = 2;
    int exit_code = 0;
    pthread_t producer_thread;
    pthread_t *consumer_thread = NULL;
    thread_t *producer_thread_data = NULL;
    thread_t **consumer_thread_data = NULL;

    if (argc == 4) {
        if (parse_positive_int(argv[1], &buffer_size) != 0 ||
            parse_positive_int(argv[2], &consumer_count) != 0 ||
            parse_positive_int(argv[3], &items_per_consumer) != 0) {
            printf("usage: %s [buffer_size consumer_count items_per_consumer]\n", argv[0]);
            return 1;
        }
    } else {
        buffer_size = 10;
        items_per_consumer = 10;
    }

    total_items = consumer_count * items_per_consumer;
    buff = (int *)malloc(sizeof(int) * buffer_size);
    consumer_thread = (pthread_t *)malloc(sizeof(pthread_t) * consumer_count);
    consumer_thread_data = (thread_t **)malloc(sizeof(thread_t *) * consumer_count);
    if (buff == NULL || consumer_thread == NULL || consumer_thread_data == NULL) {
        free(buff);
        free(consumer_thread);
        free(consumer_thread_data);
        return 1;
    }
    memset(consumer_thread_data, 0, sizeof(thread_t *) * consumer_count);

    producer_thread_data = init_producer_thread(0);
    if (producer_thread_data == NULL) {
        exit_code = 1;
        goto cleanup;
    }
    for (int i = 0; i < consumer_count; i++) {
        consumer_thread_data[i] = init_consumer_thread(i + 1);
        if (consumer_thread_data[i] == NULL) {
            exit_code = 1;
            goto cleanup;
        }
    }

    sem_init(&empty, 0, buffer_size);
    sem_init(&full, 0, 0);

    pthread_create(&producer_thread, NULL, producer, (void *)producer_thread_data);
    for (int i = 0; i < consumer_count; i++) {
        pthread_create(&consumer_thread[i], NULL, consumer, (void *)consumer_thread_data[i]);
    }

    pthread_join(producer_thread, NULL);
    for (int i = 0; i < consumer_count; i++) {
        pthread_join(consumer_thread[i], NULL);
    }

    sem_destroy(&empty);
    sem_destroy(&full);

    print_thread_values(producer_thread_data);
    int consumed_count = 0;
    for (int i = 0; i < consumer_count; i++) {
        print_thread_values(consumer_thread_data[i]);
        consumed_count += consumer_thread_data[i]->value_count;
    }
    if (producer_thread_data->value_count != consumed_count) {
        exit_code = 1;
    }

cleanup:
    destroy_thread(producer_thread_data);
    if (consumer_thread_data != NULL) {
        for (int i = 0; i < consumer_count; i++) {
            destroy_thread(consumer_thread_data[i]);
        }
    }
    free(consumer_thread_data);
    free(consumer_thread);
    free(buff);
    return exit_code;
}
