#ifndef INTRUSIVE_LIST_LINUX_STYLE_H
#define INTRUSIVE_LIST_LINUX_STYLE_H

#include <stddef.h>
#include <stdint.h>

struct list_head {
    struct list_head *next;
    struct list_head *prev;
};

#ifndef container_of
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

static inline void INIT_LIST_HEAD(struct list_head *head) {
    head->next = head;
    head->prev = head;
}

static inline int list_empty(const struct list_head *head) { return head->next == head; }

static inline void __list_add(struct list_head *new_node, struct list_head *prev, struct list_head *next) {
    next->prev = new_node;
    prev->next = new_node;

    new_node->prev = prev;
    new_node->next = next;
}

static inline void __list_del(struct list_head *prev, struct list_head *next) {
    next->prev = prev;
    prev->next = next;
}

/*============API============*/

static inline void list_add_head(struct list_head *new_node, struct list_head *head) {
    __list_add(new_node, head, head->next);
}

static inline void list_add_tail(struct list_head *new_node, struct list_head *head) {
    __list_add(new_node, head->prev, head);
}

static inline void list_del(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
    entry->next = NULL;
    entry->prev = NULL;
}

static inline void INIT_LIST_NODE(struct list_head *node) {
    node->next = NULL;
    node->prev = NULL;
}

static inline int list_node_is_linked(const struct list_head *node) {
    return (node->next != NULL && node->prev != NULL);
}

/* ========== 取回宿主对象 ========== */
#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_first_entry(head, type, member) list_entry((head)->next, type, member)

#define list_last_entry(head, type, member) list_entry((head)->prev, type, member)

/* ========== 遍历宏（学习时最常用） ========== */
#define list_for_each(pos, head) for ((pos) = (head)->next; (pos) != (head); (pos) = (pos)->next)

/* 遍历宿主结构体（更像你平时写业务代码的感觉） */
#define list_for_each_entry(pos, head, member)                                                                         \
    for ((pos) = list_entry((head)->next, typeof(*(pos)), member); &((pos)->member) != (head);                         \
         (pos) = list_entry((pos)->member.next, typeof(*(pos)), member))

/* 安全遍历：允许在遍历中删除当前节点 */
#define list_for_each_entry_safe(pos, tmp, head, member)                                                               \
    for ((pos) = list_entry((head)->next, typeof(*(pos)), member),                                                     \
        (tmp) = list_entry((pos)->member.next, typeof(*(pos)), member);                                                \
         &((pos)->member) != (head); (pos) = (tmp), (tmp) = list_entry((tmp)->member.next, typeof(*(tmp)), member))

#endif /* INTRUSIVE_LIST_LINUX_STYLE_H */
