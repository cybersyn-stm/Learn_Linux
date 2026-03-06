#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define list_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)
#define container_of(ptr, type, member)                                                                                \
    ({                                                                                                                 \
        const typeof(((type *)0)->member) *__mptr = (ptr);                                                             \
        (type *)((char *)__mptr - offsetof(type, member));                                                             \
    })

typedef enum { DATA_PERSON, DATA_INT, DATA_DOUBLE, DATA_STRING } data_type_t;

// 单一类型的数据结构
typedef struct {
    char *name;
    int age;
} person_data_t;

// 链表节点
typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

// 通用数据节点：带类型标签、链表节点、联合体数据
typedef struct list_item {
    data_type_t type;
    list_node_t node;
    union {
        person_data_t person;
        int i32;
        double d64;
        char *str;
    } data;
} list_item_t;

// 基础链表操作
void list_init(list_node_t *head) {
    head->next = head;
    head->prev = head;
}

void list_insert_after(list_node_t *pos, list_node_t *new_node) {
    new_node->next = pos->next;
    new_node->prev = pos;
    pos->next->prev = new_node;
    pos->next = new_node;
}

void list_remove(list_node_t *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = node;
    node->prev = node;
}

// 在尾部追加
list_node_t *append_list_node(list_node_t *head, list_node_t *new_node) {
    list_node_t *current = head;
    while (current->next != head) {
        current = current->next;
    }
    list_insert_after(current, new_node);
    return head;
}

// 工厂函数
list_item_t *create_person_item(const char *name, int age) {
    list_item_t *item = (list_item_t *)malloc(sizeof(list_item_t));
    if (!item)
        return NULL;
    item->type = DATA_PERSON;
    list_init(&item->node);
    item->data.person.age = age;
    item->data.person.name = strdup(name);
    return item;
}

list_item_t *create_int_item(int value) {
    list_item_t *item = (list_item_t *)malloc(sizeof(list_item_t));
    if (!item)
        return NULL;
    item->type = DATA_INT;
    list_init(&item->node);
    item->data.i32 = value;
    return item;
}

list_item_t *create_double_item(double value) {
    list_item_t *item = (list_item_t *)malloc(sizeof(list_item_t));
    if (!item)
        return NULL;
    item->type = DATA_DOUBLE;
    list_init(&item->node);
    item->data.d64 = value;
    return item;
}

list_item_t *create_string_item(const char *value) {
    list_item_t *item = (list_item_t *)malloc(sizeof(list_item_t));
    if (!item)
        return NULL;
    item->type = DATA_STRING;
    list_init(&item->node);
    item->data.str = strdup(value);
    return item;
}

// 释放通用节点
void free_item(list_item_t *item) {
    if (!item)
        return;
    if (item->type == DATA_PERSON && item->data.person.name)
        free(item->data.person.name);
    if (item->type == DATA_STRING && item->data.str)
        free(item->data.str);
    free(item);
}

int main() {
    // 创建链表头
    list_node_t head;
    list_init(&head);

    // 创建不同类型的数据
    list_item_t *p1 = create_person_item("Alice", 25);
    list_item_t *p2 = create_person_item("Bob", 30);
    list_item_t *p3 = create_person_item("Charlie", 35);
    list_item_t *num = create_int_item(42);
    list_item_t *pi = create_double_item(3.14159);
    list_item_t *msg = create_string_item("Hello list");

    // 追加到链表
    append_list_node(&head, &p1->node);
    append_list_node(&head, &p2->node);
    append_list_node(&head, &p3->node);
    append_list_node(&head, &num->node);
    append_list_node(&head, &pi->node);
    append_list_node(&head, &msg->node);

    // 遍历链表
    printf("链表内容:\n");
    list_node_t *pos;
    list_for_each(pos, &head) {
        list_item_t *item = container_of(pos, list_item_t, node);
        switch (item->type) {
        case DATA_PERSON:
            printf("[Person] 姓名: %s, 年龄: %d\n", item->data.person.name, item->data.person.age);
            break;
        case DATA_INT:
            printf("[Int] 值: %d\n", item->data.i32);
            break;
        case DATA_DOUBLE:
            printf("[Double] 值: %.5f\n", item->data.d64);
            break;
        case DATA_STRING:
            printf("[String] 值: %s\n", item->data.str);
            break;
        default:
            break;
        }
    }

    // 删除 Bob
    printf("\n删除 Bob 后:\n");
    list_remove(&p2->node);

    list_for_each(pos, &head) {
        list_item_t *item = container_of(pos, list_item_t, node);
        switch (item->type) {
        case DATA_PERSON:
            printf("[Person] 姓名: %s, 年龄: %d\n", item->data.person.name, item->data.person.age);
            break;
        case DATA_INT:
            printf("[Int] 值: %d\n", item->data.i32);
            break;
        case DATA_DOUBLE:
            printf("[Double] 值: %.5f\n", item->data.d64);
            break;
        case DATA_STRING:
            printf("[String] 值: %s\n", item->data.str);
            break;
        default:
            break;
        }
    }

    // 清理内存
    free_item(p1);
    free_item(p2);
    free_item(p3);
    free_item(num);
    free_item(pi);
    free_item(msg);

    return 0;
}
