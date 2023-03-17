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

static const size_t set_internal_primes[] = {7,17,31, 79, 149, 257, 433, 709, 1151, 1831, 2897, 4561, 7159, 11239, 17599, 27527, 43037, 67289, 105173, 164357, 256873, 401407, 627227, 980107, 1531469, 2392967, 3739051, 5842327, 9128681, 14263603, 22286917, 34823353, 54411523, 85018091, 132840809, 207563831, 324318523, 506747737, 791793391, 1237177289, 1933089577, 3020452531, 4719457163, 7374151859, 11522112347, 18003300619, 28130157371, 43953370927, 68677142119, 107308034609, 167668804133, 261982506517, 409347666491, 639605729069, 999383951717, 1561537424633, 2439902226067, 3812347228433, 5956792544551, 9307488350911, 14542950548381, 22723360231991, 35505250362539, 55476953691539, 86682740143211, 135441781473877, 211627783552979, 330668411801677, 516669393440177, 807295927250429, 1261399886328851, 1970937322388927, 3079589566232801, 4811858697238861, 7518529214435819, 11747701897556029, 18355784214931363, 28680912835830289, 44813926305984883, 70021759853101489, 109408999770471229, 170951562141361427, 267111815845877377, 417362212259183473, 652128456654974287, 1018950713523397547, 1592110489880308721, 2487672640437982507, 3886988500684347719, 6073419532319293369, 9489718019248895999, 14827684405076400079};

// extern size_t set_internal_primes[92];

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