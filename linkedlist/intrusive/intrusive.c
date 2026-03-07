#include "intrusive.h"
#include <stdio.h>

struct task {
    int id;
    int prio;
    struct list_head run_link;
};

int main(void) {
    struct list_head run_queue; // 头指针
    INIT_LIST_HEAD(&run_queue);

    struct task t1 = {.id = 1, .prio = 10};
    struct task t2 = {.id = 2, .prio = 20};
    struct task t3 = {.id = 3, .prio = 15};
    struct task t4 = {.id = 4, .prio = 5};

    INIT_LIST_NODE(&t1.run_link);
    INIT_LIST_NODE(&t2.run_link);
    INIT_LIST_NODE(&t3.run_link);
    INIT_LIST_NODE(&t4.run_link);

    list_add_tail(&t1.run_link, &run_queue);
    list_add_tail(&t2.run_link, &run_queue);
    list_add_tail(&t3.run_link, &run_queue);
    list_add_head(&t4.run_link, &run_queue);

    struct list_head *pos;
    list_for_each(pos, &run_queue) {
        struct task *current = container_of(pos, struct task, run_link);
        printf("Task ID: %d, Priority: %d\n", current->id, current->prio);
    }

    list_del(&t3.run_link);
    printf("\nAfter deleting task 2:\n");

    list_for_each(pos, &run_queue) {
        struct task *current = container_of(pos, struct task, run_link);
        printf("Task ID: %d, Priority: %d\n", current->id, current->prio);
    }
    return 0;
}
