#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_SIZE 8
#define PRODUCE_COUNT 10
#define CONSUME_COUNT_PER_THREAD 5

typedef struct {
    int thread_id;
    int produced[PRODUCE_COUNT];
    int produced_count;
} producer_ctx_t;

typedef struct {
    int thread_id;
    int consumed[CONSUME_COUNT_PER_THREAD];
    int consumed_count;
} consumer_ctx_t;

static int buffer[BUFFER_SIZE];
static int write_idx = 0;
static int read_idx = 0;
static sem_t empty_slots;
static sem_t filled_slots;
static pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;

static void put(int value) {
    buffer[write_idx] = value;
    write_idx = (write_idx + 1) % BUFFER_SIZE;
}

static int get(void) {
    int value = buffer[read_idx];
    read_idx = (read_idx + 1) % BUFFER_SIZE;
    return value;
}

void *producer(void *arg) {
    producer_ctx_t *ctx = (producer_ctx_t *)arg;
    for (int i = 0; i < PRODUCE_COUNT; i++) {
        sem_wait(&empty_slots);
        pthread_mutex_lock(&buffer_lock);
        put(i);
        ctx->produced[i] = i;
        pthread_mutex_unlock(&buffer_lock);
        sem_post(&filled_slots);
    }
    ctx->produced_count = PRODUCE_COUNT;
    return NULL;
}

void *consumer(void *arg) {
    consumer_ctx_t *ctx = (consumer_ctx_t *)arg;
    for (int i = 0; i < CONSUME_COUNT_PER_THREAD; i++) {
        usleep(1000); // Simulate work
        sem_wait(&filled_slots);
        pthread_mutex_lock(&buffer_lock);
        ctx->consumed[i] = get();
        pthread_mutex_unlock(&buffer_lock);
        sem_post(&empty_slots);
    }
    ctx->consumed_count = CONSUME_COUNT_PER_THREAD;
    return NULL;
}

int main() {
    sem_init(&empty_slots, 0, BUFFER_SIZE);
    sem_init(&filled_slots, 0, 0);
    pthread_t producer_tid, consumer_tid, consumer_tid2;
    producer_ctx_t producer_ctx = {.thread_id = 1, .produced_count = 0};
    consumer_ctx_t consumer_ctx1 = {.thread_id = 1, .consumed_count = 0};
    consumer_ctx_t consumer_ctx2 = {.thread_id = 2, .consumed_count = 0};

    pthread_create(&producer_tid, NULL, producer, &producer_ctx);
    pthread_create(&consumer_tid, NULL, consumer, &consumer_ctx1);
    pthread_create(&consumer_tid2, NULL, consumer, &consumer_ctx2);
    pthread_join(producer_tid, NULL);
    pthread_join(consumer_tid, NULL);
    pthread_join(consumer_tid2, NULL);

    printf("Producer %d produced numbers: ", producer_ctx.thread_id);
    for (int i = 0; i < producer_ctx.produced_count; i++) {
        printf("%d ", producer_ctx.produced[i]);
    }
    printf("\n");

    printf("Consumer %d consumed numbers: ", consumer_ctx1.thread_id);
    for (int i = 0; i < consumer_ctx1.consumed_count; i++) {
        printf("%d ", consumer_ctx1.consumed[i]);
    }
    printf("\n");

    printf("Consumer %d consumed numbers: ", consumer_ctx2.thread_id);
    for (int i = 0; i < consumer_ctx2.consumed_count; i++) {
        printf("%d ", consumer_ctx2.consumed[i]);
    }
    printf("\n");

    sem_destroy(&empty_slots);
    sem_destroy(&filled_slots);
    pthread_mutex_destroy(&buffer_lock);
    return 0;
}
