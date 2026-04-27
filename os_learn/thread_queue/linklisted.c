#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct __node_t {
    int value;
    struct __node_t *next;
} node_t;

typedef struct queue_t {
    node_t *head;
    node_t *tail;
    pthread_mutex_t head_mutex;
    pthread_mutex_t tail_mutex;
} queue_t;

void Queue_Init(queue_t *q) {
    node_t *tmp = (node_t *)malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->next = NULL;
    q->head = q->tail = tmp;
    pthread_mutex_init(&q->head_mutex, NULL);
    pthread_mutex_init(&q->tail_mutex, NULL);
}

void Queue_Enqueue(queue_t *q, int value) {
    node_t *tmp = (node_t *)malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = value;
    tmp->next = NULL;

    pthread_mutex_lock(&q->tail_mutex);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tail_mutex);
}

int Queue_Dequeue(queue_t *q, int *value) {
    pthread_mutex_lock(&q->head_mutex);
    // tmp指向哨兵节点
    node_t *tmp = q->head;
    // new_head指向哨兵节点的下一个节点
    node_t *new_head = tmp->next;
    if (new_head == NULL) {
        pthread_mutex_unlock(&q->head_mutex);
        return -1; // queue is empty
    }
    // 取出头兵节点的下一个节点的值，并将头兵节点释放掉
    *value = new_head->value;
    q->head = new_head;
    pthread_mutex_unlock(&q->head_mutex);
    free(tmp);
    return 0;
}

void Queue_Destroy(queue_t *q) {
    int value;
    while (Queue_Dequeue(q, &value) == 0) {
    }
    free(q->head);
    pthread_mutex_destroy(&q->head_mutex);
    pthread_mutex_destroy(&q->tail_mutex);
}

typedef struct {
    queue_t *q;
    int count;
} thread_arg_t;

static void *producer_thread(void *arg) {
    thread_arg_t *ctx = (thread_arg_t *)arg;
    for (int i = 1; i <= ctx->count; i++) {
        Queue_Enqueue(ctx->q, i);
        printf("producer enqueue: %d\n", i);
    }
    return NULL;
}

static void *consumer_thread(void *arg) {
    thread_arg_t *ctx = (thread_arg_t *)arg;
    int done = 0;
    while (done < ctx->count) {
        int value = 0;
        if (Queue_Dequeue(ctx->q, &value) == 0) {
            printf("consumer dequeue: %d\n", value);
            done++;
        }
    }
    return NULL;
}

int main(void) {
    queue_t q;
    pthread_t p1, p2;
    thread_arg_t arg = {.q = &q, .count = 10};

    Queue_Init(&q);
    assert(pthread_create(&p1, NULL, producer_thread, &arg) == 0);
    assert(pthread_create(&p2, NULL, consumer_thread, &arg) == 0);
    assert(pthread_join(p1, NULL) == 0);
    assert(pthread_join(p2, NULL) == 0);
    Queue_Destroy(&q);

    return 0;
}
