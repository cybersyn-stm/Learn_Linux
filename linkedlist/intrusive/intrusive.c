#include <stddef.h>
#include <stdint.h>

typedef struct IntrusiveListNode {
    struct IntrusiveListNode *prev;
    struct IntrusiveListNode *next;
} IntrusiveListNode;

typedef struct INtrusiveList {
    IntrusiveListNode head;
    size_t size;
} IntrusiveList;

static inline void ilist_init(IntrusiveList *list) {
    list->head.next = &list->head;
    list->head.prev = &list->head;
}
