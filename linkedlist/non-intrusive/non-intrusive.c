#include <stddef.h>
#include <stdint.h>

typedef struct SlistNode {
    struct SlistNode *next;
    void *data;
} SlistNode;

typedef struct {
    SlistNode *head;
    SlistNode *tail;
    uint32_t size;
} Slist;

#ifndef SLIST_POOL_CAP
#define SLIST_POOL_CAP 32
#endif

typedef struct {
    SlistNode nodes[SLIST_POOL_CAP];
    SlistNode *free_list;
} SlistNodePool;

// 清空SlistNodePoll
static void pool_init(SlistNodePool *p) {
    p->free_list = NULL;
    for (uint32_t i = 0; i < SLIST_POOL_CAP; i++) {
        p->nodes[i].next = p->free_list;
        p->nodes[i].data = NULL;
        p->free_list = &p->nodes[i];
    }
}

// 分配一个SlistNode
static SlistNode *pool_alloc(SlistNodePool *p) {
    if (p->free_list == NULL)
        return NULL;
    SlistNode *n = p->free_list;
    p->free_list = p->free_list->next;
    n->next = NULL;
    n->data = NULL;
    return n;
}

// 释放一个SlistNode，并放回SlistNodePool
static void pool_free(SlistNodePool *p, SlistNode *node) {
    if (!node)
        return;
    node->data = NULL;
    node->next = p->free_list;
    p->free_list = node;
}

// 初始化Slist
static void slist_init(Slist *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

static int slist_push_front(Slist *list, SlistNodePool *p, void *data) {
    SlistNode *node = pool_alloc(p);
    if (!node)
        return -1;

    node->data = data;
    node->next = list->head;
    list->head = node;

    if (list->tail == NULL) {
        list->tail = node;
    }
    list->size++;
    return 0;
}

static int slist_push_back(Slist *list, SlistNodePool *p, void *data) {
    SlistNode *node = pool_alloc(p);
    if (!node)
        return -1;

    node->data = data;
    node->next = NULL;

    if (list->tail) {
        list->tail->next = node;
        list->tail = node;
    } else {
        /* 空链表 */
        list->head = list->tail = node;
    }
    list->size++;
    return 0;
}

static void *slist_pop_front(Slist *list, SlistNodePool *p) {
    if (list->head == NULL)
        return NULL;

    SlistNode *node = list->head;
    void *data = node->data;

    list->head = node->next;
    if (list->head == NULL) {
        list->tail = NULL;
    }
    list->size--;

    pool_free(p, node);
    return data;
}

typedef void (*slist_visit_fn)(void *data, void *user);

static void slist_foreach(Slist *l, slist_visit_fn fn, void *user) {
    for (SlistNode *cur = l->head; cur != NULL; cur = cur->next) {
        fn(cur->data, user);
    }
}

#include <stdio.h>

typedef struct {
    int id;
    int prio;
} Task;

static void print_task(void *data, void *user) {
    (void)user;
    Task *t = (Task *)data;
    printf("Task{id=%d, prio=%d}\n", t->id, t->prio);
}

int main(void) {
    Slist list;
    SlistNodePool pool;

    slist_init(&list);
    pool_init(&pool);

    Task t1 = {.id = 1, .prio = 10};
    Task t2 = {.id = 2, .prio = 20};
    Task t3 = {.id = 3, .prio = 30};
    Task t4 = {.id = 4, .prio = 40};

    slist_push_back(&list, &pool, &t1);
    slist_push_back(&list, &pool, &t2);
    slist_push_back(&list, &pool, &t3);
    slist_push_back(&list, &pool, &t4);

    printf("size=%u\n", (unsigned)list.size);
    slist_foreach(&list, print_task, NULL);

    Task *popped = (Task *)slist_pop_front(&list, &pool);
    printf("popped id=%d\n", popped ? popped->id : -1);

    printf("size=%u\n", (unsigned)list.size);
    slist_foreach(&list, print_task, NULL);

    return 0;
}
