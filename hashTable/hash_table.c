#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TABLE_SIZE 100000

int chain_conflict = 0;
int simple_conflict = 0;

typedef struct Entry {
    char *key;
    int value;
    struct Entry *next;
} Entry;

Entry *table[TABLE_SIZE];

// djb2 hash function
unsigned int hash(const char *key) {
    unsigned int hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;
    return hash % TABLE_SIZE;
}

// 链地址法插入
void insert_chain(const char *key, int value) {
    unsigned int idx = hash(key);
    if (table[idx])
        chain_conflict++; // 冲突统计
    Entry *curr = table[idx];
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            curr->value = value;
            return;
        }
        curr = curr->next;
    }
    Entry *new_entry = malloc(sizeof(Entry));
    new_entry->key = strdup(key);
    new_entry->value = value;
    new_entry->next = table[idx];
    table[idx] = new_entry;
}

// 链地址法查找
int get_chain(const char *key) {
    unsigned int idx = hash(key);
    Entry *curr = table[idx];
    while (curr) {
        if (strcmp(curr->key, key) == 0)
            return curr->value;
        curr = curr->next;
    }
    return -1;
}

// 非链表法（覆盖法）插入
typedef struct {
    char *key;
    int value;
} SimpleEntry;

SimpleEntry simple_table[TABLE_SIZE];

void insert_simple(const char *key, int value) {
    unsigned int idx = hash(key);
    if (simple_table[idx].key)
        simple_conflict++; // 冲突统计
    simple_table[idx].key = strdup(key);
    simple_table[idx].value = value;
}

int get_simple(const char *key) {
    unsigned int idx = hash(key);
    if (simple_table[idx].key && strcmp(simple_table[idx].key, key) == 0)
        return simple_table[idx].value;
    return -1;
}

int main() {
    const int N = 500000;
    char key[32];

    // 链地址法插入
    clock_t start = clock();
    for (int i = 0; i < N; ++i) {
        snprintf(key, sizeof(key), "key%d", i);
        insert_chain(key, i);
    }
    clock_t end = clock();
    printf("Chain insert time: %.3f ms\n", (double)(end - start) * 1000 / CLOCKS_PER_SEC);

    // 链地址法查找
    start = clock();
    for (int i = 0; i < N; ++i) {
        snprintf(key, sizeof(key), "key%d", i);
        get_chain(key);
    }
    end = clock();
    printf("Chain get time: %.3f ms\n", (double)(end - start) * 1000 / CLOCKS_PER_SEC);
    printf("Chain conflict count: %d\n", chain_conflict);

    // 非链表法插入
    start = clock();
    for (int i = 0; i < N; ++i) {
        snprintf(key, sizeof(key), "key%d", i);
        insert_simple(key, i);
    }
    end = clock();
    printf("Simple insert time: %.3f ms\n", (double)(end - start) * 1000 / CLOCKS_PER_SEC);

    // 非链表法查找
    start = clock();
    for (int i = 0; i < N; ++i) {
        snprintf(key, sizeof(key), "key%d", i);
        get_simple(key);
    }
    end = clock();
    printf("Simple get time: %.3f ms\n", (double)(end - start) * 1000 / CLOCKS_PER_SEC);
    printf("Simple conflict count: %d\n", simple_conflict);

    return 0;
}
