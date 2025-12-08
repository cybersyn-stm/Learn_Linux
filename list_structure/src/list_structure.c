// 1.建立链表结构体和数据结构体
// 2.每个数据结构体中建立链表结构体
// 3.从链表通过container_of访问数据结构体
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
// 遍历链表
#define list_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)
// container_of通过成员地址指向父结构体
#define container_of(ptr, type, member)                                                                                \
    ({                                                                                                                 \
        const typeof(((type *)0)->member) *__mptr = (ptr);                                                             \
        (type *)((char *)__mptr - offsetof(type, member));                                                             \
    })
// 定义链表节点结构
typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

// 用户数据结构
typedef struct person_data {
    char *name;
    int age;
    list_node_t node;
} person_data_t;

// 链表初始化
void list_init(list_node_t *head) {
    head->next = head;
    head->prev = head;
}

// 指定节点插入新节点
void list_insert_after(list_node_t *pos, list_node_t *new_node) {
    new_node->next = pos->next;
    new_node->prev = pos;
    pos->next->prev = new_node;
    pos->next = new_node;
}

// 删除链表节点
void list_remove(list_node_t *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = node;
    node->prev = node;
}

// 创建数据并添加链表
person_data_t *create_data(char *name, int age) {
    person_data_t *Domain = (person_data_t *)malloc(sizeof(person_data_t)); // create memoryspace
    // snprintf(p->name, sizeof(p->name), "%s", name);
    Domain->name = name;
    Domain->age = age;
    list_init(&Domain->node);
    return Domain;
}
// 对指定链表末尾添加数据
list_node_t *Append_list_node(list_node_t *head, list_node_t *new_node) {
    list_node_t *current = head;
    while (current->next != head) {
        current = current->next;
    }
    list_insert_after(current, new_node);
    return head;
}
int main() {
    // 创建链表头
    list_node_t head;
    list_init(&head);

    // 创建几个人
    person_data_t *p1 = create_data("Alice", 25);
    person_data_t *p2 = create_data("Bob", 30);
    person_data_t *p3 = create_data("Charlie", 35);

    // 添加到链表
    /*list_insert_after(&head, &p1->node);
    list_insert_after(&p1->node, &p2->node);
    list_insert_after(&p2->node, &p3->node);*/
    Append_list_node(&head, &p1->node);
    Append_list_node(&head, &p2->node);
    Append_list_node(&head, &p3->node);

    // 遍历链表
    printf("链表内容:\n");
    list_node_t *pos;
    list_for_each(pos, &head) {
        // 通过节点指针获取person结构体
        person_data_t *p = container_of(pos, person_data_t, node);
        printf("姓名: %s, 年龄: %d\n", p->name, p->age);
    }

    // 删除Bob
    printf("\n删除Bob后:\n");
    list_remove(&p2->node);

    // 再次遍历
    list_for_each(pos, &head) {
        person_data_t *p = container_of(pos, person_data_t, node);
        printf("姓名: %s, 年龄: %d\n", p->name, p->age);
    }

    // 清理内存
    free(p1);
    free(p2);
    free(p3);

    return 0;
}
