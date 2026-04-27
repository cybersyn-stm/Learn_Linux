#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node_t {
    int key;
    struct node_t *next;
} node_t;

typedef struct list_t {
    node_t *head;
    pthread_mutex_t lock;
} list_t;

typedef int (*fetch_thread_value_fn)(void *ctx, int thread_id, int *value);

typedef struct value_provider_t {
    fetch_thread_value_fn fetch;
    void *ctx;
} value_provider_t;

typedef struct thread_arg_t {
    list_t *list;
    value_provider_t *provider;
    int thread_id;
} thread_arg_t;

void List_Init(list_t *L) {
    L->head = NULL;
    pthread_mutex_init(&L->lock, NULL);
}

int List_Insert(list_t *L, int key) {
    pthread_mutex_lock(&L->lock);
    node_t *new = (node_t *)malloc(sizeof(node_t));
    if (new == NULL) {
        perror("malloc");
        pthread_mutex_unlock(&L->lock);
        return -1;
    }
    new->key = key;
    new->next = L->head;
    L->head = new;
    pthread_mutex_unlock(&L->lock);
    return 0;
}

int List_Lookup(list_t *L, int key) {
    pthread_mutex_lock(&L->lock);
    node_t *curr = L->head;
    while (curr) {
        if (curr->key == key) {
            pthread_mutex_unlock(&L->lock);
            return 0;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&L->lock);
    return -1;
}

void *thread_func(void *arg) {
    thread_arg_t *targ = (thread_arg_t *)arg;
    int value = 0;
    if (targ->provider->fetch(targ->provider->ctx, targ->thread_id, &value) != 0) {
        fprintf(stderr, "线程 %d 获取数据失败\n", targ->thread_id);
        return NULL;
    }
    List_Insert(targ->list, value);
    if (List_Lookup(targ->list, value) == 0) {
        printf("线程 %d 找到元素 %d\n", targ->thread_id, value);
    }
    return NULL;
}

typedef struct array_source_t {
    int *values;
    int count;
} array_source_t;

int fetch_from_array(void *ctx, int thread_id, int *value) {
    array_source_t *source = (array_source_t *)ctx;
    if (thread_id < 0 || thread_id >= source->count) {
        return -1;
    }
    *value = source->values[thread_id];
    return 0;
}

int main() {
    pthread_t p1, p2;
    list_t mylist;
    int values[] = {100, 200};
    array_source_t source = {.values = values, .count = 5};
    value_provider_t provider = {.fetch = fetch_from_array, .ctx = &source};
    thread_arg_t arg1 = {.list = &mylist, .provider = &provider, .thread_id = 0};
    thread_arg_t arg2 = {.list = &mylist, .provider = &provider, .thread_id = 1};

    List_Init(&mylist);
    pthread_create(&p1, NULL, thread_func, &arg1);
    pthread_create(&p2, NULL, thread_func, &arg2);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    return 0;
}
