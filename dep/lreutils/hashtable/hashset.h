#ifndef set_h
#define set_h

#include <memory.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define set_internal_primes hashtable_internal_primes

#ifndef HASHTABLEDEFAULTVALUE
#define HASHTABLEDEFAULTVALUE -1
#endif

extern size_t set_internal_primes[92];

#define SETISHEAP free(node->key)
#define SETNOTHEAP 

static default_compare(int x, int y) {return x == y;}

/*
 * 
 *
 * 
*/
#define hashset(type,hashfunc,cmpfunc,keyheap) \
typedef struct { \
    size_t mem_size; \
    size_t item_count; \
    size_t max_col; \
} hashset_##type; \
\
typedef struct { \
    ##type key; \
} hashset_##type##_meta; \
\
\
\
static void _hashtable_freenode_##type(hashset_##type##_meta* node); \
static void hashset_##type##_cleanup(hashset_##type* table); \
static size_t hashtable_bytes_##type(size_t index); \
static hashset_##type* hashset_##type##_create(); \
static inline hashset_##type##_meta* _hashset_##type##_get(hashset_##type* table,size_t index); \
static inline void _hashset_##type##_set(hashset_##type* table, size_t index, ##type key); \
static hashset_##type* hashset_##type##_insert(hashset_##type* table,##type key); \
static hashset_##type* hashset_##type##_remove(hashset_##type* table, ##type key); \
static hashset_##type* hashset_##type##_resize(hashset_##type* table,bool grow); \
\
\
\
static void _hashtable_freenode_##type(hashset_##type##_meta* node) { \
    keyheap; \
} \
static void hashset_##type##_cleanup(hashset_##type* table) { \
    size_t size = set_internal_primes[table->mem_size]; \
    for (size_t i = 0; i < size; i++) { \
        _hashtable_freenode_##type(_hashset_##type##_get(table,i)); \
    } \
    free(table); \
} \
static size_t hashtable_bytes_##type(size_t index) { \
    return sizeof(hashset_##type) + (sizeof(hashset_##type##_meta)) * set_internal_primes[index]; \
} \
static hashset_##type* hashset_##type##_create() { \
    size_t byte_size = hashtable_bytes_##type(0); \
    hashset_##type* table = (hashset_##type*)malloc(byte_size); \
    memset(table,0,sizeof(hashset_##type)); \
    memset(((char*)table)+sizeof(hashset_##type),HASHTABLEDEFAULTVALUE,byte_size-sizeof(hashset_##type)); \
    return table; \
} \
static hashset_##type* hashset_##type##_create_s(size_t index) { \
    size_t byte_size = hashtable_bytes_##type(index); \
    hashset_##type* table = (hashset_##type*)malloc(byte_size); \
    table->mem_size = index; \
    memset(((char*)table)+sizeof(size_t),0,sizeof(hashset_##type)-sizeof(size_t)); \
    memset(((char*)table)+sizeof(hashset_##type),HASHTABLEDEFAULTVALUE,byte_size-sizeof(hashset_##type)); \
    return table; \
} \
static inline hashset_##type##_meta* _hashset_##type##_get(hashset_##type* table,size_t index) { \
    return (hashset_##type##_meta*)(((char*)table) + sizeof(hashset_##type) + sizeof(hashset_##type##_meta) * (index % set_internal_primes[table->mem_size])); \
} \
static inline void _hashset_##type##_set(hashset_##type* table, size_t index, ##type key) { \
    hashset_##type##_meta* meta = _hashset_##type##_get(table,index); \
    meta->key = key; \
} \
\
\
\
static hashset_##type* hashset_##type##_insert(hashset_##type* table,##type key) { \
    if (key == HASHTABLEDEFAULTVALUE) {fprintf(stderr,"[hashtable insert] Reserved key, value not entered\n"); return table;} \
    if (table->mem_size < 89 && table->item_count > ((float)set_internal_primes[table->mem_size])*0.7) \
        table = hashset_##type##_resize(table,true); \
    table->item_count++; \
    size_t index = hashfunc(key); \
    hashset_##type##_meta* meta; \
    if ((meta = _hashset_##type##_get(table,index))->key == HASHTABLEDEFAULTVALUE) { \
        _hashset_##type##_set(table,index,key); \
        return table; \
    } \
    if (meta->key == key) { \
        _hashtable_freenode_##type(meta); \
        return table; \
    } \
    size_t original_index = index; \
    while ((meta = _hashset_##type##_get(table,++index))->key != HASHTABLEDEFAULTVALUE) { \
        if (meta->key == key) { \
            _hashtable_freenode_##type(meta); \
            return table; \
        } \
    } \
    \
    _hashset_##type##_set(table,index,key); \
    if (index - original_index > table->max_col) table->max_col = index-original_index; \
    return table; \
} \
static hashset_##type* hashset_##type##_remove(hashset_##type* table, ##type key) { \
    if (key == HASHTABLEDEFAULTVALUE) {fprintf(stderr,"[hashtable remove] Reserved key, value not entered\n"); return table;} \
    if (table->mem_size > 0 && table->item_count < ((float)set_internal_primes[table->mem_size])*0.2) \
        table = hashset_##type##_resize(table,false); \
    size_t index = hashfunc(key); \
    hashset_##type##_meta* cur; \
    if (cmpfunc((cur = _hashset_##type##_get(table,index))->key,key)) { \
        _hashtable_freenode_##type(cur); \
        memset(cur,HASHTABLEDEFAULTVALUE,sizeof(hashset_##type##_meta)); \
        table->item_count--; \
        return table; \
    } \
    size_t original_index = index; \
    while (cmpfunc((cur = _hashset_##type##_get(table,++index))->key,key) == false) { \
        if (index-original_index >= table->max_col) { \
            fprintf(stderr,"Could not find hashset key\n"); \
            return table; \
        } \
    } \
    _hashtable_freenode_##type(cur); \
    memset(cur,HASHTABLEDEFAULTVALUE,sizeof(hashset_##type##_meta)); \
    table->item_count--; \
    return table; \
} \
static hashset_##type* hashset_##type##_resize(hashset_##type* table,bool grow) { \
    hashset_##type* newTable; \
    if (grow == true) newTable = hashset_##type##_create_s(table->mem_size+1); \
    else newTable = hashset_##type##_create_s(table->mem_size-1); \
    \
      \
    hashset_##type##_meta* iter; \
    for (size_t i = 0; i < set_internal_primes[table->mem_size]; i++) { \
        iter = _hashset_##type##_get(table,i); \
        if (iter->key != HASHTABLEDEFAULTVALUE) \
            newTable = hashset_##type##_insert(newTable,iter->key); \
    } \
    free(table); \
    return newTable; \
} \
static bool hashset_##type##_found(hashset_##type* table,##type key) { \
    if (key == HASHTABLEDEFAULTVALUE) {fprintf(stderr,"[hashtable found] Reserved key, value not entered\n"); return false;} \
    size_t index = hashfunc(key); \
    hashset_##type##_meta* cur; \
    if (cmpfunc((cur = _hashset_##type##_get(table,index))->key,key)) { \
        return true; \
    } \
    size_t original_index = index; \
    while (cmpfunc((cur = _hashset_##type##_get(table,++index))->key,key) == false) { \
        if (index-original_index >= table->max_col) { \
            return false; \
        } \
    } \
    return true; \
} \
static type hashset_##type##_get(hashset_##type* table,##type key) { \
    if (key == HASHTABLEDEFAULTVALUE) {fprintf(stderr,"[hashtable get] Reserved key, value not entered\n"); return HASHTABLEDEFAULTVALUE;} \
    size_t index = hashfunc(key); \
    hashset_##type##_meta* cur; \
    if (cmpfunc((cur = _hashset_##type##_get(table,index))->key,key)) { \
        return cur->key; \
    } \
    size_t original_index = index; \
    while (cmpfunc((cur = _hashset_##type##_get(table,++index))->key,key) == false) { \
        if (index-original_index >= table->max_col) { \
            return HASHTABLEDEFAULTVALUE; \
        } \
    } \
    return cur->key; \
} \


#endif