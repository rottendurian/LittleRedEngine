#ifndef hashtableo_h
#define hashtableo_h

#include <memory.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

extern size_t hashtable_internal_primes[92];

// static int hashtable_internal_primes[] = { 31, 67, 127, 257, 509, 1021, 2053, 4099, 8191, 16381, 32771, 65537, 131071, 
//     262147, 524287, 1048573, 2097143, 4194301, 8388617, 16777213, 33554467, 67108859, 134217757, 268435459, 536870909, 
//     1073741827, 2147483647 };
#ifndef HASHTABLEDEFAULTVALUE
#define HASHTABLEDEFAULTVALUE -1
#endif

#ifndef hashtable_default_size
#define hashtable_default_size hashtable_internal_primes[0]
#endif

#define HASHTABLEKEYDATAHEAP free(node->key); free(node->data);
#define HASHTABLEDATAHEAP free(node->data);
#define HASHTABLEKEYHEAP free(node->key);
#define HASHTABLENOHEAP 

#define HASHNULL

/*
 * 
 *
 * 
*/
#define hashtable(keytype,datatype,hashfunc,cmpfunc,cleanup) \
typedef struct { \
    size_t mem_size; \
    size_t item_count; \
    size_t max_col; \
} hashtable_##keytype; \
\
typedef struct { \
    ##datatype data; \
    ##keytype key; \
} hashtable_##keytype##_meta; \
\
\
\
\
static void _hashtable_freenode_##keytype(hashtable_##keytype##_meta* node); \
static void hashtable_##keytype##_cleanup(hashtable_##keytype* table); \
static size_t hashtable_bytes_##keytype(size_t index); \
static hashtable_##keytype* hashtable_##keytype##_create(); \
static inline hashtable_##keytype##_meta* _hashtable_##keytype##_get(hashtable_##keytype* table,size_t index); \
static inline void _hashtable_##keytype##_set(hashtable_##keytype* table, size_t index, ##keytype key, ##datatype data); \
static hashtable_##keytype* hashtable_##keytype##_insert(hashtable_##keytype* table,##keytype key, ##datatype data); \
static hashtable_##keytype* hashtable_##keytype##_remove(hashtable_##keytype* table, ##keytype key); \
static hashtable_##keytype* hashtable_##keytype##_resize(hashtable_##keytype* table,bool grow); \
\
\
\
static void _hashtable_freenode_##keytype(hashtable_##keytype##_meta* node) { \
    cleanup; \
} \
static void hashtable_##keytype##_cleanup(hashtable_##keytype* table) { \
    size_t size = hashtable_internal_primes[table->mem_size]; \
    for (size_t i = 0; i < size; i++) { \
        _hashtable_freenode_##keytype(_hashtable_##keytype##_get(table,i)); \
    } \
    free(table); \
} \
static size_t hashtable_bytes_##keytype(size_t index) { \
    return sizeof(hashtable_##keytype) + (sizeof(hashtable_##keytype##_meta)) * hashtable_internal_primes[index]; \
} \
static hashtable_##keytype* hashtable_##keytype##_create() { \
    size_t byte_size = hashtable_bytes_##keytype(0); \
    hashtable_##keytype* table = (hashtable_##keytype*)malloc(byte_size); \
    memset(table,HASHTABLEDEFAULTFILLVALUE,byte_size); \
    return table; \
} \
static hashtable_##keytype* hashtable_##keytype##_create_s(size_t index) { \
    size_t byte_size = hashtable_bytes_##keytype(index); \
    hashtable_##keytype* table = (hashtable_##keytype*)malloc(byte_size); \
    table->mem_size = index; \
    memset(((char*)table)+sizeof(size_t),HASHTABLEDEFAULTFILLVALUE,byte_size-sizeof(size_t)); \
    return table; \
} \
static inline hashtable_##keytype##_meta* _hashtable_##keytype##_get(hashtable_##keytype* table,size_t index) { \
    return (hashtable_##keytype##_meta*)(((char*)table) + sizeof(hashtable_##keytype) + sizeof(hashtable_##keytype##_meta) * (index % hashtable_internal_primes[table->mem_size])); \
} \
static inline void _hashtable_##keytype##_set(hashtable_##keytype* table, size_t index, ##keytype key, ##datatype data) { \
    hashtable_##keytype##_meta* meta = _hashtable_##keytype##_get(table,index); \
    meta->data = data; \
    meta->key = key; \
} \
\
\
\
static hashtable_##keytype* hashtable_##keytype##_insert(hashtable_##keytype* table,##keytype key, ##datatype data) { \
    if (cmpfunc(key, HASHTABLEDEFAULTTYPEVALUE)) {fprintf(stderr,"[hashtable insert] Reserved key, value not entered\n"); return table;} \
    if (table->mem_size < 89 && (table->item_count > ((float)hashtable_internal_primes[table->mem_size])*0.7)) \
        table = hashtable_##keytype##_resize(table,true); \
    table->item_count++; \
    size_t index = hashfunc(key); \
    hashtable_##keytype##_meta* meta; \
    if (cmpfunc((meta = _hashtable_##keytype##_get(table,index))->key, HASHTABLEDEFAULTTYPEVALUE)) { \
        _hashtable_##keytype##_set(table,index,key,data); \
        return table; \
    } \
    if (cmpfunc(meta->key, key)) { \
        _hashtable_freenode_##keytype(meta); \
        table->item_count--; \
        meta->data = data; \
        return table; \
    } \
    size_t original_index = index; \
    while (!cmpfunc((meta = _hashtable_##keytype##_get(table,++index))->key, HASHTABLEDEFAULTTYPEVALUE)) { \
        if (cmpfunc(meta->key, key)) { \
            table->item_count--; \
            _hashtable_freenode_##keytype(meta); \
            meta->data = data; \
            return table; \
        } \
    } \
    \
    _hashtable_##keytype##_set(table,index,key,data); \
    if (index - original_index > table->max_col) table->max_col = index-original_index; \
    return table; \
} \
static hashtable_##keytype* hashtable_##keytype##_remove(hashtable_##keytype* table, ##keytype key) { \
    if (cmpfunc(key, HASHTABLEDEFAULTTYPEVALUE)) {fprintf(stderr,"[hashtable remove] Reserved key, value not entered\n"); return table;} \
    if (table->mem_size > 0 && table->item_count < ((float)hashtable_internal_primes[table->mem_size])*0.2) \
        table = hashtable_##keytype##_resize(table,false); \
    size_t index = hashfunc(key); \
    hashtable_##keytype##_meta* cur; \
    if (cmpfunc((cur = _hashtable_##keytype##_get(table,index))->key,key)) { \
        _hashtable_freenode_##keytype(cur); \
        memset(cur,HASHTABLEDEFAULTFILLVALUE,sizeof(hashtable_##keytype##_meta)); \
        table->item_count--; \
        return table; \
    } \
    size_t original_index = index; \
    while (cmpfunc((cur = _hashtable_##keytype##_get(table,++index))->key,key) == false) { \
        if (index-original_index >= table->max_col) { \
            fprintf(stderr,"Could not find key\n");\
            return table; \
        } \
    } \
    _hashtable_freenode_##keytype(cur); \
    memset(cur,HASHTABLEDEFAULTFILLVALUE,sizeof(hashtable_##keytype##_meta)); \
    table->item_count--; \
    return table; \
} \
static hashtable_##keytype* hashtable_##keytype##_resize(hashtable_##keytype* table,bool grow) { \
    hashtable_##keytype* newTable; \
    if (grow == true) newTable = hashtable_##keytype##_create_s(table->mem_size+1); \
    else newTable = hashtable_##keytype##_create_s(table->mem_size-1); \
    \
      \
    hashtable_##keytype##_meta* iter; \
    for (size_t i = 0; i < hashtable_internal_primes[table->mem_size]; i++) { \
        iter = _hashtable_##keytype##_get(table,i); \
        if (!cmpfunc(iter->key, HASHTABLEDEFAULTTYPEVALUE)) \
            newTable = hashtable_##keytype##_insert(newTable,iter->key,iter->data); \
    } \
    free(table); \
    return newTable; \
} \
static bool hashtable_##keytype##_found(hashtable_##keytype* table,##keytype key) { \
    if (cmpfunc(key, HASHTABLEDEFAULTTYPEVALUE)) {fprintf(stderr,"[hashtable found] Reserved key, value not entered\n"); return false;} \
    size_t index = hashfunc(key); \
    hashtable_##keytype##_meta* cur; \
    if (cmpfunc((cur = _hashtable_##keytype##_get(table,index))->key,key)) { \
        return true; \
    } \
    size_t original_index = index; \
    while (cmpfunc((cur = _hashtable_##keytype##_get(table,++index))->key,key) == false) { \
        if (index-original_index >= table->max_col) { \
            return false; \
        } \
    } \
    return true; \
} \
static hashtable_##keytype##_meta* hashtable_##keytype##_get(hashtable_##keytype* table,##keytype key) { \
    if (cmpfunc(key, HASHTABLEDEFAULTTYPEVALUE)) {fprintf(stderr,"[hashtable get] Reserved key, value not entered\n"); return NULL;} \
    size_t index = hashfunc(key); \
    hashtable_##keytype##_meta* cur; \
    if (cmpfunc((cur = _hashtable_##keytype##_get(table,index))->key,key)) { \
        return cur; \
    } \
    size_t original_index = index; \
    while (cmpfunc((cur = _hashtable_##keytype##_get(table,++index))->key,key) == false) { \
        if (index-original_index >= table->max_col) { \
            return NULL; \
        } \
    } \
    return cur; \
} \


#endif