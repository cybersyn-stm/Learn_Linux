#include <stdio.h>
#include <stdlib.h>
#define container_of(ptr, type, member)                                                                                \
    ({                                                                                                                 \
        const typeof(((type *)0)->member) *__mptr = (ptr);                                                             \
        (type *)((char *)__mptr - offsetof(type, member));                                                             \
    })

typedef struct LinkedList {
    char *name;
    struct LinkedList *next;               // next node
    struct LinkedList *prev;               // previous node
    void (*func)(struct LinkedList *node); // function pointer
} LinkedList;
LinkedList *CreateLinedList(char *name) // creat void node
{
    LinkedList *newNode = (LinkedList *)malloc(sizeof(LinkedList)); // allocate memory
    if (newNode == NULL) {
        return NULL; // memory allocation failed
    }
    newNode->name = name;
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->func = NULL;
    return newNode;
}
LinkedList *AppendLinedList(LinkedList *head, LinkedList *newNode, void (*func)(LinkedList *node)) // append void node
{
    if (newNode == NULL)
        return head; // if newNode is NULL return head
    if (head == NULL)
        return newNode;     // if head is NULL return newNode
    LinkedList *p = head;   // point to head
    while (p->next != NULL) // find the last empty node next = NULL
    {
        p = p->next; // point to next
    }
    p->next = newNode; // connect new node
    newNode->prev = p; // new node link previous node
    newNode->func = func;
    func(newNode);
    return head;
}
void func(LinkedList *node) { printf("Function called for node: %s\n", node->name); }
void func1(LinkedList *node) { printf("Function called for node: %s\n", node->name); }
void func2(LinkedList *node) { printf("Function called for node: %s\n", node->name); }
void func3(LinkedList *node) { printf("Function called for node: %s\n", node->name); }
void func4(LinkedList *node) { printf("Function called for node: %s\n", node->name); }
void func5(LinkedList *node) { printf("Function called for node: %s\n", node->name); }
void func6(LinkedList *node) { printf("Function called for node: %s\n", node->name); }

int main() {
    LinkedList *Node;
    Node = CreateLinedList("Node1");
    AppendLinedList(Node, CreateLinedList("Node"), func);
    AppendLinedList(Node, CreateLinedList("Node1"), func1);
    AppendLinedList(Node, CreateLinedList("Node2"), func2);
    AppendLinedList(Node, CreateLinedList("Node3"), func3);
    AppendLinedList(Node, CreateLinedList("Node4"), func4);
    AppendLinedList(Node, CreateLinedList("Node5"), func5);
    AppendLinedList(Node, CreateLinedList("Node6"), func6);

    LinkedList *current = Node;
    while (current != NULL) {
        current->func;
        current = current->next;
    }
}
