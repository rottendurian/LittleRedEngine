#ifndef hashtable_h
#define hashtable_h

#include <memory.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static int hashtable_internal_primes[] = { 31, 67, 127, 257, 509, 1021, 2053, 4099, 8191, 16381, 32771, 65537, 131071, 
    262147, 524287, 1048573, 2097143, 4194301, 8388617, 16777213, 33554467, 67108859, 134217757, 268435459, 536870909, 
    1073741827, 2147483647 };

#ifndef hashtable_default_size
#define hashtable_default_size hashtable_internal_primes[0]
#endif

#define HASHTABLEISHEAP 1
#define HASHTABLENOTHEAP 0

// static bool HASHTABLEDEFAULTCMP(x, y) {return x == y;}
// static size_t HASHTABLEDEFAULTHASH(x) {return x;}

/*
 * Hash function requires that it works with type as the first parameter,
 * and its second parameter is size_t hashSize. keyheap is a required parameter
 * where it is 1 if the key is heap allocated, (like a string) if a string is used
 * a string copy method is needed (to put it on the heap), using strdup.
*/
#define hashtable(type,hash_func,cmp_func,keyheap) \
struct hashNode_##type { \
    type key; \
    void* data; \
    struct hashNode_##type* next; \
}; \
\
\
static struct hashNode_##type* hashNode_##type##_free(struct hashNode_##type* hashNode) { \
    if (keyheap) free(hashNode->key); \
    free(hashNode->data); \
    struct hashNode_##type *next = hashNode->next; \
    free(hashNode); \
    return next; \
}  \
static struct hashNode_##type* hashNode_##type##_free_savedata(struct hashNode_##type* hashNode) { \
    struct hashNode_##type *next = hashNode->next; \
    free(hashNode); \
    return next; \
}  \
struct hashtable_##type { \
    struct hashNode_##type* nodes; \
    size_t size; \
    size_t items; \
}; \
static struct hashtable_##type* hashtable_##type_resize(struct hashtable_##type* table); \
static struct hashtable_##type* hashtable_##type##_new(); \
static struct hashtable_##type* hashtable_##type##_new_size(int size); \
static struct hashtable_##type* hashtable_##type##_insert(struct hashtable_##type* table, type key, void* data); \
static struct hashtable_##type* hashtable_##type##_remove(struct hashtable_##type* table, type key); \
static void* hashtable_##type##_get(struct hashtable_##type* table, type key); \
static void hashtable_##type##_cleanup(struct hashtable_##type* table); \
static void hashtable_##type##_cleanupSaveData(struct hashtable_##type* table); \
\
\
\
static struct hashtable_##type* hashtable_##type##_resize(struct hashtable_##type* table,bool bigger) { \
    struct hashtable_##type* newTable; \
    if (bigger == true) {\
        int k = 0; \
        while (table->size >= hashtable_internal_primes[k]) {k++;} \
        newTable = hashtable_##type##_new_size(k); \
    } else {\
        int k = 26; \
        while (table->size <= hashtable_internal_primes[k]) {k--;} \
        newTable = hashtable_##type##_new_size(k); \
    } \
    for (int i = 0; i < table->size; i++) { \
        struct hashNode_##type* node = &table->nodes[i];\
        if (node->key != 0) { \
            newTable = hashtable_##type##_insert(newTable,node->key,node->data); \
            while (node->next != NULL) { \
                node = node->next; \
                newTable = hashtable_##type##_insert(newTable,node->key,node->data); \
            } \
        } \
    } \
    hashtable_##type##_cleanupSaveData(table); \
    return newTable; \
} \
static struct hashtable_##type* hashtable_##type##_new() { \
    struct hashtable_##type* table = (struct hashtable_##type*)malloc(sizeof(struct hashtable_##type)); \
    table->nodes = (struct hashNode_##type*)malloc(hashtable_default_size*sizeof(struct hashNode_##type)); \
    memset(table->nodes,0,hashtable_default_size*sizeof(struct hashNode_##type)); \
    table->size = hashtable_default_size; \
    table->items = 0; \
    return table; \
} \
static struct hashtable_##type* hashtable_##type##_new_size(int size) { \
    struct hashtable_##type* table = (struct hashtable_##type*)malloc(sizeof(struct hashtable_##type)); \
    table->nodes = (struct hashNode_##type*)malloc(hashtable_internal_primes[size]*sizeof(struct hashNode_##type)); \
    memset(table->nodes,0,hashtable_internal_primes[size]*sizeof(struct hashNode_##type)); \
    table->size = hashtable_internal_primes[size]; \
    table->items = 0; \
    return table; \
} \
static struct hashtable_##type* hashtable_##type##_insert(struct hashtable_##type* table, type key, void* data) { \
    if (table->items >= ((float)table->size*0.8)) { \
        table = hashtable_##type##_resize(table,true); \
    } \
    size_t index = hash_func(key) % table->size; \
    struct hashNode_##type* node = &table->nodes[index]; \
    if (node->key == 0 || cmp_func(node->key,key)) { \
        if (keyheap) free(node->key);\
        free(node->data);\
        node->key = key; \
        node->data = data; \
    } else { \
        bool comparison = false; \
        while (node->next != NULL) { \
            comparison = cmp_func(node->next->key,key); \
            if (comparison) { \
                if (keyheap) free(node->next->key); \
                free(node->next->data); \
                goto hashtable_##type##_exit; \
            } \
            node = node->next; \
        } \
        node->next = (struct hashNode_##type*)malloc(sizeof(struct hashNode_##type)); \
        memset(node->next, 0, sizeof(struct hashNode_##type)); \
        hashtable_##type##_exit: \
        node->next->key = key; \
        node->next->data = data; \
    } \
    table->items++; \
    return table; \
} \
static struct hashtable_##type* hashtable_##type##_remove(struct hashtable_##type* table, type key) { \
    if (table->items <= ((float)table->size*0.2)) { \
        table = hashtable_##type##_resize(table,false); \
    } \
    size_t index = hash_func(key) % table->size; \
    struct hashNode_##type* node = &table->nodes[index]; \
    if (cmp_func(node->key,key)) { \
        if (node->next == NULL) { \
            memset(node,0,sizeof(struct hashNode_##type)); \
        } else { \
            struct hashNode_##type* storage = node->next; \
            memcpy(node,node->next,sizeof(struct hashNode_##type)); \
            hashNode_##type##_free(storage); \
        } \
        table->items--; \
    } else { \
        struct hashNode_##type* prev; \
        while (node != NULL && !cmp_func(node->key, key)) { \
            prev = node; \
            node = node->next; \
        } \
        if (node != NULL) { \
            prev->next = hashNode_##type##_free(node); \
            table->items--; \
        } \
    } \
    return table; \
} \
static void* hashtable_##type##_get(struct hashtable_##type* table, type key) { \
    size_t index = hash_func(key) % table->size; \
    if (!cmp_func(table->nodes[index].key,key)) { \
        struct hashNode_##type* node = table->nodes[index].next; \
        while (node != NULL && !cmp_func(node->key, key)) { \
            node = node->next; \
        } \
        if (node == NULL) \
            return NULL; \
        else return node->data; \
    } \
    return table->nodes[index].data; \
} \
static void hashtable_##type##_cleanup(struct hashtable_##type* table) { \
    for (int i = 0; i < table->size; i++) { \
        struct hashNode_##type* next = table->nodes[i].next;\
        if (keyheap) free(table->nodes[i].key); \
        free(table->nodes[i].data); \
        while (next != NULL) { \
            next = hashNode_##type##_free(next); \
        } \
    } \
    free(table->nodes); \
    free(table); \
} \
static void hashtable_##type##_cleanupSaveData(struct hashtable_##type* table) { \
    for (int i = 0; i < table->size; i++) { \
        struct hashNode_##type* next = table->nodes[i].next;\
        while (next != NULL) { \
            next = hashNode_##type##_free_savedata(next); \
        } \
    } \
    free(table->nodes); \
    free(table); \
}



#endif