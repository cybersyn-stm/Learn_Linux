#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

typedef struct {
    char *key; // 键值
    int value; // 值
} Entry;

Entry *hashTable[TABLE_SIZE]; // 哈希表

// 哈希函数
unsigned int hash(const char *key) {
    unsigned int hash = 5381;
    while (*key) {
        hash = (hash * 31) + *key++;
    }
    return hash % TABLE_SIZE;
}

void insert(const char *key, int value) {
    unsigned int idx = hash(key);
    Entry *entry = malloc(sizeof(Entry));
    entry->key = strdup(key);
    entry->value = value;
    hashTable[idx] = entry;
}
int search(const char *key) {
    unsigned int idx = hash(key);
    if (hashTable[idx] && strcmp(hashTable[idx]->key, key) == 0) {
        return hashTable[idx]->value;
    }
    return -1; // Not found
}

int main() {
    insert("apple", 1);
    insert("banana", 2);
    insert("orange", 13);
    insert("milk", 4);
    insert("dick", 5);
    insert("fuck", 6);
    insert("math", 7);
    insert("english", 8);

    printf("apple: %d\n", search("apple"));
    printf("banana: %d\n", search("banana"));
    printf("orange: %d\n", search("orange"));   // Not found
    printf("milk: %d\n", search("milk"));       // Not found
    printf("dick: %d\n", search("dick"));       // Not found
    printf("fuck: %d\n", search("fuck"));       // Not found
    printf("math: %d\n", search("math"));       // Not found
    printf("english: %d\n", search("english")); // Not found

    return 0;
}
