#include <memory.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifndef HASHSET_EXPANSIONS
    #define HASHSET_EXPANSION(name,arg) name##arg
    #define _HASHSET_EXPANSION(name,arg) _##name##arg
    #define HASHSET_EXPANSION_I(name,arg) HASHSET_EXPANSION(name,arg)
    #define _HASHSET_EXPANSION_I(name,arg) _HASHSET_EXPANSION(name,arg)
#endif

#ifdef HASHSET_STATIC
    #define HASHSET_STATIC_IMPL static inline
    #define HASHSET_IMPLEMENTATION
#else
    #define HASHSET_STATIC_IMPL
#endif

#ifndef HASHSET_USIZE
    #define HASHSET_USIZE uint32_t
#endif

#ifndef HASHSET_DEFAULT_FILL_VALUE
    #define HASHSET_DEFAULT_FILL_VALUE -1
#endif

#ifndef HASHSET_DEFAULT_TYPE_VALUE
    #define HASHSET_DEFAULT_TYPE_VALUE -1
#endif

#ifndef HASHSET_NAME
    #define HASHSET_NAME hashset
#endif

#ifndef HASHSET_KEYTYPE
    #define HASHSET_KEYTYPE int
#endif

#ifndef HASHSET_DEFAULTS
    #define HASHSET_DEFAULTS

    static const size_t HASHSET_INTERNAL_PRIMES_MAX_size_t = 92;
    static const size_t HASHSET_INTERNAL_PRIMES_size_t[] = {7,17,31, 79, 149, 257, 433, 709, 1151, 1831, 2897, 4561, 7159, 11239, 17599, 27527, 43037, 67289, 105173, 164357, 256873, 401407, 627227, 980107, 1531469, 2392967, 3739051, 5842327, 9128681, 14263603, 22286917, 34823353, 54411523, 85018091, 132840809, 207563831, 324318523, 506747737, 791793391, 1237177289, 1933089577, 3020452531, 4719457163, 7374151859, 11522112347, 18003300619, 28130157371, 43953370927, 68677142119, 107308034609, 167668804133, 261982506517, 409347666491, 639605729069, 999383951717, 1561537424633, 2439902226067, 3812347228433, 5956792544551, 9307488350911, 14542950548381, 22723360231991, 35505250362539, 55476953691539, 86682740143211, 135441781473877, 211627783552979, 330668411801677, 516669393440177, 807295927250429, 1261399886328851, 1970937322388927, 3079589566232801, 4811858697238861, 7518529214435819, 11747701897556029, 18355784214931363, 28680912835830289, 44813926305984883, 70021759853101489, 109408999770471229, 170951562141361427, 267111815845877377, 417362212259183473, 652128456654974287, 1018950713523397547, 1592110489880308721, 2487672640437982507, 3886988500684347719, 6073419532319293369, 9489718019248895999, 14827684405076400079};
    static const uint32_t HASHSET_INTERNAL_PRIMES_MAX_uint32_t = 42;
    static const uint32_t HASHSET_INTERNAL_PRIMES_uint32_t[] = {7,17,31, 79, 149, 257, 433, 709, 1151, 1831, 2897, 4561, 7159, 11239, 17599, 27527, 43037, 67289, 105173, 164357, 256873, 401407, 627227, 980107, 1531469, 2392967, 3739051, 5842327, 9128681, 14263603, 22286917, 34823353, 54411523, 85018091, 132840809, 207563831, 324318523, 506747737, 791793391, 1237177289, 1933089577, 3020452531}; //4719457163, 7374151859, 11522112347, 18003300619, 28130157371, 43953370927, 68677142119, 107308034609, 167668804133, 261982506517, 409347666491, 639605729069, 999383951717, 1561537424633, 2439902226067, 3812347228433, 5956792544551, 9307488350911, 14542950548381, 22723360231991, 35505250362539, 55476953691539, 86682740143211, 135441781473877, 211627783552979, 330668411801677, 516669393440177, 807295927250429, 1261399886328851, 1970937322388927, 3079589566232801, 4811858697238861, 7518529214435819, 11747701897556029, 18355784214931363, 28680912835830289, 44813926305984883,  };

    // static inline HASHSET_USIZE HASHSET_DEFAULT_HASH(HASHSET_KEYTYPE type) {return (HASHSET_USIZE)type;}
    // static inline bool HASHSET_DEFAULT_COMPARE(HASHSET_KEYTYPE t1,HASHSET_KEYTYPE t2) {return t1==t2;}
    #define HASHSETKEYDATAHEAP free(node->key); free(node->data);
    #define HASHSETDATAHEAP free(node->data);
    #define HASHSETKEYHEAP free(node->key);
    #define HASHSETNOHEAP 
    #define HASHSET_NULL
#endif

#ifndef HASHSET_HASHFUNC
#define HASHSET_HASHFUNC HASHSET_DEFAULT_HASH
#endif

#ifndef HASHSET_COMPARE
#define HASHSET_COMPARE HASHSET_DEFAULT_COMPARE
#endif

#ifndef HASHSET_FREE
#define HASHSET_FREE HASHSET_NULL
#endif

#define HASHSET_METADATA HASHSET_EXPANSION_I(HASHSET_NAME,_metadata)
#define HASHSET_BYTES HASHSET_EXPANSION_I(HASHSET_NAME,_bytes)
#define HASHSET_NEW HASHSET_EXPANSION_I(HASHSET_NAME,_new)
#define HASHSET_FREENODE _HASHSET_EXPANSION_I(HASHSET_NAME,_freenode)
#define HASHSET_CLEANUP HASHSET_EXPANSION_I(HASHSET_NAME,_cleanup)
#define HASHSET_GET _HASHSET_EXPANSION_I(HASHSET_NAME,_get)
#define HASHSET_SET _HASHSET_EXPANSION_I(HASHSET_NAME,_set)
#define HASHSET_INSERT HASHSET_EXPANSION_I(HASHSET_NAME,_insert)
#define HASHSET_REMOVE HASHSET_EXPANSION_I(HASHSET_NAME,_remove)
#define HASHSET_RESIZE HASHSET_EXPANSION_I(HASHSET_NAME,_resize)
#define HASHSET_NEW_WITH_SIZE HASHSET_EXPANSION_I(HASHSET_NAME,_new_with_size)
#define HASHSET_FOUND HASHSET_EXPANSION_I(HASHSET_NAME,_found)
// #define HASHSET_GET_DATA HASHSET_EXPANSION_I(HASHSET_NAME,_get)

/*
 * 
 *
 * 
*/
typedef struct HASHSET_NAME { 
    HASHSET_USIZE mem_size; 
    HASHSET_USIZE item_count; 
    HASHSET_USIZE max_col; 
} HASHSET_NAME; 

typedef struct HASHSET_METADATA { 
    HASHSET_KEYTYPE key; 
} HASHSET_METADATA; 

HASHSET_STATIC_IMPL HASHSET_NAME* HASHSET_NEW();
HASHSET_STATIC_IMPL HASHSET_NAME* HASHSET_NEW_WITH_SIZE(HASHSET_USIZE size);
HASHSET_STATIC_IMPL HASHSET_NAME* HASHSET_INSERT(HASHSET_NAME* table,HASHSET_KEYTYPE key); //CHANGED 
HASHSET_STATIC_IMPL HASHSET_NAME* HASHSET_REMOVE(HASHSET_NAME* table, HASHSET_KEYTYPE key); 
HASHSET_STATIC_IMPL HASHSET_NAME* HASHSET_RESIZE(HASHSET_NAME* table,bool grow);
HASHSET_STATIC_IMPL bool HASHSET_FOUND(HASHSET_NAME* table,HASHSET_KEYTYPE key);
// HASHSET_STATIC_IMPL HASHSET_METADATA* HASHSET_GET_DATA(HASHSET_NAME* table,HASHSET_KEYTYPE key);
HASHSET_STATIC_IMPL void HASHSET_FREENODE(HASHSET_METADATA* node); 
HASHSET_STATIC_IMPL void HASHSET_CLEANUP(HASHSET_NAME* table); 
HASHSET_STATIC_IMPL HASHSET_USIZE HASHSET_BYTES(HASHSET_USIZE index); 

// #define HASHSET_IMPLEMENTATION
#ifdef HASHSET_IMPLEMENTATION

#ifdef HASHSET_USIZE
    #ifndef HASHSET_INTERNAL_PRIMES
        #define HASHSET_INTERNAL_PRIMES HASHSET_EXPANSION_I(HASHSET_INTERNAL_PRIMES_,HASHSET_USIZE)
        #define HASHSET_INTERNAL_PRIMES_MAX HASHSET_EXPANSION_I(HASHSET_INTERNAL_PRIMES_MAX_,HASHSET_USIZE)
    #endif
#endif

static inline HASHSET_METADATA* HASHSET_GET(HASHSET_NAME* table,HASHSET_USIZE index);
static inline void HASHSET_SET(HASHSET_NAME* table, HASHSET_USIZE index, HASHSET_KEYTYPE key); 

HASHSET_STATIC_IMPL void HASHSET_FREENODE(HASHSET_METADATA* node) { 
    HASHSET_FREE;
}
HASHSET_STATIC_IMPL void HASHSET_CLEANUP(HASHSET_NAME* table) { 
    HASHSET_USIZE size = HASHSET_INTERNAL_PRIMES[table->mem_size]; 
    for (HASHSET_USIZE i = 0; i < size; i++) { 
        HASHSET_FREENODE(HASHSET_GET(table,i)); 
    } 
    free(table); 
} 
HASHSET_STATIC_IMPL HASHSET_USIZE HASHSET_BYTES(HASHSET_USIZE index) { 
    return sizeof(HASHSET_NAME) + (sizeof(HASHSET_METADATA)) * HASHSET_INTERNAL_PRIMES[index]; 
} 
HASHSET_STATIC_IMPL HASHSET_NAME* HASHSET_NEW() { 
    HASHSET_USIZE byte_size = HASHSET_BYTES(0); 
    HASHSET_NAME* table = (HASHSET_NAME*)malloc(byte_size); 
    memset(((char*)table)+sizeof(HASHSET_NAME),HASHSET_DEFAULT_FILL_VALUE,byte_size-sizeof(HASHSET_NAME)); 
    table->item_count = 0;
    table->max_col = 0;
    table->mem_size = 0;
    return table; 
} 
HASHSET_STATIC_IMPL HASHSET_NAME* HASHSET_NEW_WITH_SIZE(HASHSET_USIZE size) { 
    HASHSET_USIZE byte_size = HASHSET_BYTES(size); 
    HASHSET_NAME* table = (HASHSET_NAME*)malloc(byte_size); 
    table->mem_size = size;
    table->item_count = 0;
    table->max_col = 0;
    memset(((char*)table)+sizeof(HASHSET_NAME),HASHSET_DEFAULT_FILL_VALUE,byte_size-sizeof(HASHSET_NAME)); 
    return table; 
} 
static inline HASHSET_METADATA* HASHSET_GET(HASHSET_NAME* table,HASHSET_USIZE index) { 
    return (HASHSET_METADATA*)(((char*)table) + sizeof(HASHSET_NAME) + sizeof(HASHSET_METADATA) * (index % HASHSET_INTERNAL_PRIMES[table->mem_size])); 
} 
static inline void HASHSET_SET(HASHSET_NAME* table, HASHSET_USIZE index, HASHSET_KEYTYPE key) { 
    HASHSET_METADATA* meta = HASHSET_GET(table,index);
    meta->key = key;
} 



HASHSET_STATIC_IMPL HASHSET_NAME* HASHSET_INSERT(HASHSET_NAME* table,HASHSET_KEYTYPE key) { 
    if (HASHSET_COMPARE(key, HASHSET_DEFAULT_TYPE_VALUE)) {fprintf(stderr,"[HASHSET insert] Reserved key, value not entered\n"); return table;} 
    if (table->mem_size < HASHSET_INTERNAL_PRIMES_MAX && (table->item_count > ((float)HASHSET_INTERNAL_PRIMES[table->mem_size])*0.6)) 
        table = HASHSET_RESIZE(table,true);
    table->item_count++; 
    HASHSET_USIZE index = HASHSET_HASHFUNC(key);
    HASHSET_METADATA* meta = HASHSET_GET(table,index); 
    if (HASHSET_COMPARE(meta->key, HASHSET_DEFAULT_TYPE_VALUE)) { 
        HASHSET_SET(table,index,key); 
        return table; 
    } 
    if (HASHSET_COMPARE(meta->key, key)) { 
        HASHSET_FREENODE(meta); 
        table->item_count--;
        return table; 
    } 
    HASHSET_USIZE original_index = index; 
    while (!HASHSET_COMPARE((meta = HASHSET_GET(table,++index))->key, HASHSET_DEFAULT_TYPE_VALUE)) { 
        if (HASHSET_COMPARE(meta->key, key)) { 
            table->item_count--; 
            HASHSET_FREENODE(meta);
            return table; 
        } 
    } 
    
    HASHSET_SET(table,index,key); 
    if (index - original_index > table->max_col) table->max_col = index-original_index; 
    return table; 
} 
HASHSET_STATIC_IMPL HASHSET_NAME* HASHSET_REMOVE(HASHSET_NAME* table, HASHSET_KEYTYPE key) { 
    if (HASHSET_COMPARE(key, HASHSET_DEFAULT_TYPE_VALUE)) {fprintf(stderr,"[HASHSET remove] Reserved key, value not enteredn"); return table;} 
    if (table->mem_size > 0 && table->item_count < ((float)HASHSET_INTERNAL_PRIMES[table->mem_size])*0.2) 
        table = HASHSET_RESIZE(table,false); 
    HASHSET_USIZE index = HASHSET_HASHFUNC(key); 
    HASHSET_METADATA* cur; 
    if (HASHSET_COMPARE((cur = HASHSET_GET(table,index))->key,key)) { 
        HASHSET_FREENODE(cur); 
        memset(cur,HASHSET_DEFAULT_FILL_VALUE,sizeof(HASHSET_METADATA)); 
        table->item_count--; 
        return table; 
    } 
    HASHSET_USIZE original_index = index; 
    while (HASHSET_COMPARE((cur = HASHSET_GET(table,++index))->key,key) == false) { 
        if (index-original_index >= table->max_col) { 
            fprintf(stderr,"Could not find keyn");
            return table; 
        } 
    } 
    HASHSET_FREENODE(cur); 
    memset(cur,HASHSET_DEFAULT_FILL_VALUE,sizeof(HASHSET_METADATA)); 
    table->item_count--; 
    return table; 
} 
HASHSET_STATIC_IMPL HASHSET_NAME* HASHSET_RESIZE(HASHSET_NAME* table,bool grow) { 
    HASHSET_NAME* newTable; 
    if (grow == true) newTable = HASHSET_NEW_WITH_SIZE(table->mem_size+1); 
    else newTable = HASHSET_NEW_WITH_SIZE(table->mem_size-1); 
    
      
    HASHSET_METADATA* iter; 
    for (HASHSET_USIZE i = 0; i < HASHSET_INTERNAL_PRIMES[table->mem_size]; i++) { 
        iter = HASHSET_GET(table,i); 
        if (!HASHSET_COMPARE(iter->key, HASHSET_DEFAULT_TYPE_VALUE)) 
            newTable = HASHSET_INSERT(newTable,iter->key); 
    } 
    free(table); 
    return newTable; 
} 
HASHSET_STATIC_IMPL bool HASHSET_FOUND(HASHSET_NAME* table,HASHSET_KEYTYPE key) { 
    if (HASHSET_COMPARE(key, HASHSET_DEFAULT_TYPE_VALUE)) {fprintf(stderr,"[HASHSET found] Reserved key, value not enteredn"); return false;} 
    HASHSET_USIZE index = HASHSET_HASHFUNC(key); 
    HASHSET_METADATA* cur; 
    if (HASHSET_COMPARE((cur = HASHSET_GET(table,index))->key,key)) { 
        return true; 
    } 
    HASHSET_USIZE original_index = index; 
    while (HASHSET_COMPARE((cur = HASHSET_GET(table,++index))->key,key) == false) { 
        if (index-original_index >= table->max_col) { 
            return false; 
        } 
    } 
    return true; 
} 


#endif
#undef HASHSET_IMPLEMENTATION

#undef HASHSET_STATIC
#undef HASHSET_USIZE
#undef HASHSET_DEFAULT_FILL_VALUE
#undef HASHSET_DEFAULT_TYPE_VALUE

#undef HASHSET_NAME
#undef HASHSET_KEYTYPE

#undef HASHSET_HASHFUNC
#undef HASHSET_COMPARE
#undef HASHSET_FREE

#undef HASHSET_METADATA
#undef HASHSET_BYTES
#undef HASHSET_NEW
#undef HASHSET_FREENODE
#undef HASHSET_CLEANUP
#undef HASHSET_GET
#undef HASHSET_SET
#undef HASHSET_INSERT
#undef HASHSET_REMOVE
#undef HASHSET_RESIZE
