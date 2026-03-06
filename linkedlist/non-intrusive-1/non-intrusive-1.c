#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* =========================
 * Non-intrusive Singly Linked List (malloc/free)
 * - Each node holds a pointer to user data (void*).
 * - The list allocates/frees nodes, but does NOT free user data.
 * ========================= */

typedef struct SListNode {
    struct SListNode *next;
    void *data;
} SListNode;

typedef struct SList {
    SListNode *head;
    SListNode *tail;
    size_t size;
} SList;

/* ---------- Initialization ---------- */

void slist_init(SList *list) {
    if (!list)
        return;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

/* ---------- Internal helper ---------- */

static SListNode *slist_node_create(void *data) {
    SListNode *node = (SListNode *)malloc(sizeof(SListNode));
    if (!node)
        return NULL;

    node->next = NULL;
    node->data = data;
    return node;
}

/* ---------- Push operations ---------- */

int slist_push_front(SList *list, void *data) {
    if (!list)
        return -1;

    SListNode *new_node = slist_node_create(data);
    if (!new_node)
        return -1;

    new_node->next = list->head;
    list->head = new_node;

    if (list->tail == NULL) {
        /* list was empty before insert */
        list->tail = new_node;
    }

    list->size++;
    return 0;
}

int slist_push_back(SList *list, void *data) {
    if (!list)
        return -1;

    SListNode *new_node = slist_node_create(data);
    if (!new_node)
        return -1;

    if (list->tail) {
        /* list not empty */
        list->tail->next = new_node;
        list->tail = new_node;
    } else {
        /* list empty */
        list->head = new_node;
        list->tail = new_node;
    }

    list->size++;
    return 0;
}

/* ---------- Pop operations ---------- */

/* Removes the first node and returns its data. Returns NULL if empty. */
void *slist_pop_front(SList *list) {
    if (!list || list->head == NULL)
        return NULL;

    SListNode *node_to_remove = list->head;
    void *data = node_to_remove->data;

    list->head = node_to_remove->next;

    if (list->head == NULL) {
        /* list becomes empty after removal */
        list->tail = NULL;
    }

    free(node_to_remove);
    list->size--;
    return data;
}

/* ---------- Peek ---------- */

void *slist_peek_front(const SList *list) {
    if (!list || list->head == NULL)
        return NULL;
    return list->head->data;
}

/* ---------- Traversal ---------- */

typedef void (*slist_visit_fn)(void *data, void *user_context);

void slist_for_each(SList *list, slist_visit_fn visit, void *user_context) {
    if (!list || !visit)
        return;

    for (SListNode *current = list->head; current != NULL; current = current->next) {
        visit(current->data, user_context);
    }
}

/* ---------- Clear ---------- */
/* Frees all nodes. If you want to free user data too, pass a callback. */

typedef void (*slist_free_data_fn)(void *data, void *user_context);

void slist_clear(SList *list, slist_free_data_fn free_data, void *user_context) {
    if (!list)
        return;

    SListNode *current = list->head;
    while (current) {
        SListNode *next = current->next;

        if (free_data) {
            free_data(current->data, user_context);
        }

        free(current);
        current = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

/* ---------- Optional demo ---------- */
#include <stdio.h>

typedef struct {
    int id;
    int prio;
} Task;

static void print_task(void *data, void *user_context) {
    (void)user_context;
    Task *t = (Task *)data;
    printf("Task{id=%d, prio=%d}\n", t->id, t->prio);
}

int main(void) {
    SList list;
    slist_init(&list);

    Task t1 = {.id = 1, .prio = 10};
    Task t2 = {.id = 2, .prio = 20};
    Task t3 = {.id = 3, .prio = 30};

    slist_push_back(&list, &t1);
    slist_push_back(&list, &t2);
    slist_push_front(&list, &t3);

    printf("size=%zu\n", list.size);
    slist_for_each(&list, print_task, NULL);

    Task *popped = (Task *)slist_pop_front(&list);
    printf("popped id=%d\n", popped ? popped->id : -1);

    printf("size=%zu\n", list.size);
    slist_for_each(&list, print_task, NULL);

    slist_clear(&list, NULL, NULL);
    return 0;
}
