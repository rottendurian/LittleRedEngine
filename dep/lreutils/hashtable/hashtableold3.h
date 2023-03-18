#include <memory.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifdef HASHTABLE_STATIC
    #define HASHTABLE_STATIC_IMPL static inline
    #define HASHTABLE_IMPLEMENTATION
#else
    #define HASHTABLE_STATIC_IMPL
    #undef HASHTABLE_IMPLEMENTATION
#endif

#ifndef HASHTABLE_USIZE
    #define HASHTABLE_USIZE uint32_t
#endif

#ifndef HASHTABLEDEFAULTFILLVALUE
        #define HASHTABLEDEFAULTFILLVALUE -1
    #endif

#ifndef HASHTABLEDEFAULTTYPEVALUE
    #define HASHTABLEDEFAULTTYPEVALUE -1
#endif

#ifndef HASHTABLE_NAME
    #define HASHTABLE_NAME hashtable
#endif

#ifndef HASHTABLE_KEYTYPE
    #define HASHTABLE_KEYTYPE int
#endif

#ifndef HASHTABLE_DATATYPE 
    #define HASHTABLE_DATATYPE int
#endif

#ifndef HASHTABLE_DEFAULTS
    #define HASHTABLE_DEFAULTS

    static const size_t HASHTABLE_INTERNAL_PRIMES_MAX_size_t = 92;
    static const size_t HASHTABLE_INTERNAL_PRIMES_size_t[] = {7,17,31, 79, 149, 257, 433, 709, 1151, 1831, 2897, 4561, 7159, 11239, 17599, 27527, 43037, 67289, 105173, 164357, 256873, 401407, 627227, 980107, 1531469, 2392967, 3739051, 5842327, 9128681, 14263603, 22286917, 34823353, 54411523, 85018091, 132840809, 207563831, 324318523, 506747737, 791793391, 1237177289, 1933089577, 3020452531, 4719457163, 7374151859, 11522112347, 18003300619, 28130157371, 43953370927, 68677142119, 107308034609, 167668804133, 261982506517, 409347666491, 639605729069, 999383951717, 1561537424633, 2439902226067, 3812347228433, 5956792544551, 9307488350911, 14542950548381, 22723360231991, 35505250362539, 55476953691539, 86682740143211, 135441781473877, 211627783552979, 330668411801677, 516669393440177, 807295927250429, 1261399886328851, 1970937322388927, 3079589566232801, 4811858697238861, 7518529214435819, 11747701897556029, 18355784214931363, 28680912835830289, 44813926305984883, 70021759853101489, 109408999770471229, 170951562141361427, 267111815845877377, 417362212259183473, 652128456654974287, 1018950713523397547, 1592110489880308721, 2487672640437982507, 3886988500684347719, 6073419532319293369, 9489718019248895999, 14827684405076400079};
    static const uint32_t HASHTABLE_INTERNAL_PRIMES_MAX_uint32_t = 42;
    static const uint32_t HASHTABLE_INTERNAL_PRIMES_uint32_t[] = {7,17,31, 79, 149, 257, 433, 709, 1151, 1831, 2897, 4561, 7159, 11239, 17599, 27527, 43037, 67289, 105173, 164357, 256873, 401407, 627227, 980107, 1531469, 2392967, 3739051, 5842327, 9128681, 14263603, 22286917, 34823353, 54411523, 85018091, 132840809, 207563831, 324318523, 506747737, 791793391, 1237177289, 1933089577, 3020452531}; //4719457163, 7374151859, 11522112347, 18003300619, 28130157371, 43953370927, 68677142119, 107308034609, 167668804133, 261982506517, 409347666491, 639605729069, 999383951717, 1561537424633, 2439902226067, 3812347228433, 5956792544551, 9307488350911, 14542950548381, 22723360231991, 35505250362539, 55476953691539, 86682740143211, 135441781473877, 211627783552979, 330668411801677, 516669393440177, 807295927250429, 1261399886328851, 1970937322388927, 3079589566232801, 4811858697238861, 7518529214435819, 11747701897556029, 18355784214931363, 28680912835830289, 44813926305984883,  };

    // static inline HASHTABLE_USIZE HASHTABLE_DEFAULT_HASH(HASHTABLE_KEYTYPE type) {return 0;} 
    // static inline bool HASHTABLE_DEFAULT_COMPARE(HASHTABLE_KEYTYPE t1,HASHTABLE_KEYTYPE t1) {return t1==t2;}
    #define HASHTABLEKEYDATAHEAP free(node->key); free(node->data);
    #define HASHTABLEDATAHEAP free(node->data);
    #define HASHTABLEKEYHEAP free(node->key);
    #define HASHTABLENOHEAP 
    #define HASHTABLE_NULL
#endif

#ifndef HASHTABLE_HASHFUNC
#define HASHTABLE_HASHFUNC HASHTABLE_DEFAULT_HASH
#endif

#ifndef HASHTABLE_COMPARE
#define HASHTABLE_COMPARE HASHTABLE_DEFAULT_COMPARE
#endif

#ifndef HASHTABLE_FREE
#define HASHTABLE_FREE HASHTABLE_NULL
#endif

#define HASHTABLE_EXPANSION(name,arg) name##arg
#define _HASHTABLE_EXPANSION(name,arg) _##name##arg
#define HASHTABLE_EXPANSION_I(name,arg) HASHTABLE_EXPANSION(name,arg)
#define _HASHTABLE_EXPANSION_I(name,arg) _HASHTABLE_EXPANSION(name,arg)

#define HASHTABLE_METADATA HASHTABLE_EXPANSION_I(HASHTABLE_NAME,_metadata)
#define HASHTABLE_BYTES HASHTABLE_EXPANSION_I(HASHTABLE_NAME,_bytes)
#define HASHTABLE_NEW HASHTABLE_EXPANSION_I(HASHTABLE_NAME,_new)
#define HASHTABLE_FREENODE _HASHTABLE_EXPANSION_I(HASHTABLE_NAME,_freenode)
#define HASHTABLE_CLEANUP HASHTABLE_EXPANSION_I(HASHTABLE_NAME,_cleanup)
#define HASHTABLE_GET _HASHTABLE_EXPANSION_I(HASHTABLE_NAME,_get)
#define HASHTABLE_SET _HASHTABLE_EXPANSION_I(HASHTABLE_NAME,_set)
#define HASHTABLE_INSERT HASHTABLE_EXPANSION_I(HASHTABLE_NAME,_insert)
#define HASHTABLE_REMOVE HASHTABLE_EXPANSION_I(HASHTABLE_NAME,_remove)
#define HASHTABLE_RESIZE HASHTABLE_EXPANSION_I(HASHTABLE_NAME,_resize)
#define HASHTABLE_NEW_WITH_SIZE HASHTABLE_EXPANSION_I(HASHTABLE_NAME,_new_with_size)
#define HASHTABLE_FOUND HASHTABLE_EXPANSION_I(HASHTABLE_NAME,_found)
#define HASHTABLE_GET_DATA HASHTABLE_EXPANSION_I(HASHTABLE_NAME,_get)

/*
 * 
 *
 * 
*/
typedef struct HASHTABLE_NAME { 
    HASHTABLE_USIZE mem_size; 
    HASHTABLE_USIZE item_count; 
    HASHTABLE_USIZE max_col; 
} HASHTABLE_NAME; 

typedef struct HASHTABLE_METADATA { 
    HASHTABLE_DATATYPE data; 
    HASHTABLE_KEYTYPE key; 
} HASHTABLE_METADATA; 

HASHTABLE_STATIC_IMPL HASHTABLE_NAME* HASHTABLE_NEW();
HASHTABLE_STATIC_IMPL HASHTABLE_NAME* HASHTABLE_NEW_WITH_SIZE(HASHTABLE_USIZE size);
HASHTABLE_STATIC_IMPL HASHTABLE_NAME* HASHTABLE_INSERT(HASHTABLE_NAME* table,HASHTABLE_KEYTYPE key, HASHTABLE_DATATYPE data); 
HASHTABLE_STATIC_IMPL HASHTABLE_NAME* HASHTABLE_REMOVE(HASHTABLE_NAME* table, HASHTABLE_KEYTYPE key); 
HASHTABLE_STATIC_IMPL HASHTABLE_NAME* HASHTABLE_RESIZE(HASHTABLE_NAME* table,bool grow);
HASHTABLE_STATIC_IMPL bool HASHTABLE_FOUND(HASHTABLE_NAME* table,HASHTABLE_KEYTYPE key);
HASHTABLE_STATIC_IMPL HASHTABLE_METADATA* HASHTABLE_GET_DATA(HASHTABLE_NAME* table,HASHTABLE_KEYTYPE key);
HASHTABLE_STATIC_IMPL void HASHTABLE_FREENODE(HASHTABLE_METADATA* node); 
HASHTABLE_STATIC_IMPL void HASHTABLE_CLEANUP(HASHTABLE_NAME* table); 
HASHTABLE_STATIC_IMPL HASHTABLE_USIZE HASHTABLE_BYTES(HASHTABLE_USIZE index); 

#ifdef HASHTABLE_IMPLEMENTATION

#ifdef HASHTABLE_USIZE
    #ifndef HASHTABLE_INTERNAL_PRIMES
        #define HASHTABLE_INTERNAL_PRIMES HASHTABLE_EXPANSION_I(HASHTABLE_INTERNAL_PRIMES_,HASHTABLE_USIZE)
        #define HASHTABLE_INTERNAL_PRIMES_MAX HASHTABLE_EXPANSION_I(HASHTABLE_INTERNAL_PRIMES_MAX_,HASHTABLE_USIZE)
    #endif
#endif

static inline HASHTABLE_METADATA* HASHTABLE_GET(HASHTABLE_NAME* table,HASHTABLE_USIZE index);
static inline void HASHTABLE_SET(HASHTABLE_NAME* table, HASHTABLE_USIZE index, HASHTABLE_KEYTYPE key, HASHTABLE_DATATYPE data); 

HASHTABLE_STATIC_IMPL void HASHTABLE_FREENODE(HASHTABLE_METADATA* node) { 
    HASHTABLE_FREE;
}
HASHTABLE_STATIC_IMPL void HASHTABLE_CLEANUP(HASHTABLE_NAME* table) { 
    HASHTABLE_USIZE size = HASHTABLE_INTERNAL_PRIMES[table->mem_size]; 
    for (HASHTABLE_USIZE i = 0; i < size; i++) { 
        HASHTABLE_FREENODE(HASHTABLE_GET(table,i)); 
    } 
    free(table); 
} 
HASHTABLE_STATIC_IMPL HASHTABLE_USIZE HASHTABLE_BYTES(HASHTABLE_USIZE index) { 
    return sizeof(HASHTABLE_NAME) + (sizeof(HASHTABLE_METADATA)) * HASHTABLE_INTERNAL_PRIMES[index]; 
} 
HASHTABLE_STATIC_IMPL HASHTABLE_NAME* HASHTABLE_NEW() { 
    HASHTABLE_USIZE byte_size = HASHTABLE_BYTES(0); 
    HASHTABLE_NAME* table = (HASHTABLE_NAME*)malloc(byte_size); 
    memset(table,HASHTABLEDEFAULTFILLVALUE,byte_size); 
    return table; 
} 
HASHTABLE_STATIC_IMPL HASHTABLE_NAME* HASHTABLE_NEW_WITH_SIZE(HASHTABLE_USIZE size) { 
    HASHTABLE_USIZE byte_size = HASHTABLE_BYTES(size); 
    HASHTABLE_NAME* table = (HASHTABLE_NAME*)malloc(byte_size); 
    table->mem_size = size; 
    memset(((char*)table)+sizeof(HASHTABLE_USIZE),HASHTABLEDEFAULTFILLVALUE,byte_size-sizeof(HASHTABLE_USIZE)); 
    return table; 
} 
static inline HASHTABLE_METADATA* HASHTABLE_GET(HASHTABLE_NAME* table,HASHTABLE_USIZE index) { 
    return (HASHTABLE_METADATA*)(((char*)table) + sizeof(HASHTABLE_NAME) + sizeof(HASHTABLE_METADATA) * (index % HASHTABLE_INTERNAL_PRIMES[table->mem_size])); 
} 
static inline void HASHTABLE_SET(HASHTABLE_NAME* table, HASHTABLE_USIZE index, HASHTABLE_KEYTYPE key, HASHTABLE_DATATYPE data) { 
    HASHTABLE_METADATA* meta = HASHTABLE_GET(table,index); 
    meta->data = data; 
    meta->key = key; 
} 



HASHTABLE_STATIC_IMPL HASHTABLE_NAME* HASHTABLE_INSERT(HASHTABLE_NAME* table,HASHTABLE_KEYTYPE key, HASHTABLE_DATATYPE data) { 
    if (HASHTABLE_COMPARE(key, HASHTABLEDEFAULTTYPEVALUE)) {fprintf(stderr,"[hashtable insert] Reserved key, value not enteredn"); return table;} 
    if (table->mem_size < HASHTABLE_INTERNAL_PRIMES_MAX && (table->item_count > ((float)HASHTABLE_INTERNAL_PRIMES[table->mem_size])*0.6)) 
        table = HASHTABLE_RESIZE(table,true); 
    table->item_count++; 
    HASHTABLE_USIZE index = HASHTABLE_HASHFUNC(key); 
    HASHTABLE_METADATA* meta; 
    if (HASHTABLE_COMPARE((meta = HASHTABLE_GET(table,index))->key, HASHTABLEDEFAULTTYPEVALUE)) { 
        HASHTABLE_SET(table,index,key,data); 
        return table; 
    } 
    if (HASHTABLE_COMPARE(meta->key, key)) { 
        HASHTABLE_FREENODE(meta); 
        table->item_count--; 
        meta->data = data; 
        return table; 
    } 
    HASHTABLE_USIZE original_index = index; 
    while (!HASHTABLE_COMPARE((meta = HASHTABLE_GET(table,++index))->key, HASHTABLEDEFAULTTYPEVALUE)) { 
        if (HASHTABLE_COMPARE(meta->key, key)) { 
            table->item_count--; 
            HASHTABLE_FREENODE(meta); 
            meta->data = data; 
            return table; 
        } 
    } 
    
    HASHTABLE_SET(table,index,key,data); 
    if (index - original_index > table->max_col) table->max_col = index-original_index; 
    return table; 
} 
HASHTABLE_STATIC_IMPL HASHTABLE_NAME* HASHTABLE_REMOVE(HASHTABLE_NAME* table, HASHTABLE_KEYTYPE key) { 
    if (HASHTABLE_COMPARE(key, HASHTABLEDEFAULTTYPEVALUE)) {fprintf(stderr,"[hashtable remove] Reserved key, value not enteredn"); return table;} 
    if (table->mem_size > 0 && table->item_count < ((float)HASHTABLE_INTERNAL_PRIMES[table->mem_size])*0.2) 
        table = HASHTABLE_RESIZE(table,false); 
    HASHTABLE_USIZE index = HASHTABLE_HASHFUNC(key); 
    HASHTABLE_METADATA* cur; 
    if (HASHTABLE_COMPARE((cur = HASHTABLE_GET(table,index))->key,key)) { 
        HASHTABLE_FREENODE(cur); 
        memset(cur,HASHTABLEDEFAULTFILLVALUE,sizeof(HASHTABLE_METADATA)); 
        table->item_count--; 
        return table; 
    } 
    HASHTABLE_USIZE original_index = index; 
    while (HASHTABLE_COMPARE((cur = HASHTABLE_GET(table,++index))->key,key) == false) { 
        if (index-original_index >= table->max_col) { 
            fprintf(stderr,"Could not find keyn");
            return table; 
        } 
    } 
    HASHTABLE_FREENODE(cur); 
    memset(cur,HASHTABLEDEFAULTFILLVALUE,sizeof(HASHTABLE_METADATA)); 
    table->item_count--; 
    return table; 
} 
HASHTABLE_STATIC_IMPL HASHTABLE_NAME* HASHTABLE_RESIZE(HASHTABLE_NAME* table,bool grow) { 
    HASHTABLE_NAME* newTable; 
    if (grow == true) newTable = HASHTABLE_NEW_WITH_SIZE(table->mem_size+1); 
    else newTable = HASHTABLE_NEW_WITH_SIZE(table->mem_size-1); 
    
      
    HASHTABLE_METADATA* iter; 
    for (HASHTABLE_USIZE i = 0; i < HASHTABLE_INTERNAL_PRIMES[table->mem_size]; i++) { 
        iter = HASHTABLE_GET(table,i); 
        if (!HASHTABLE_COMPARE(iter->key, HASHTABLEDEFAULTTYPEVALUE)) 
            newTable = HASHTABLE_INSERT(newTable,iter->key,iter->data); 
    } 
    free(table); 
    return newTable; 
} 
HASHTABLE_STATIC_IMPL bool HASHTABLE_FOUND(HASHTABLE_NAME* table,HASHTABLE_KEYTYPE key) { 
    if (HASHTABLE_COMPARE(key, HASHTABLEDEFAULTTYPEVALUE)) {fprintf(stderr,"[hashtable found] Reserved key, value not enteredn"); return false;} 
    HASHTABLE_USIZE index = HASHTABLE_HASHFUNC(key); 
    HASHTABLE_METADATA* cur; 
    if (HASHTABLE_COMPARE((cur = HASHTABLE_GET(table,index))->key,key)) { 
        return true; 
    } 
    HASHTABLE_USIZE original_index = index; 
    while (HASHTABLE_COMPARE((cur = HASHTABLE_GET(table,++index))->key,key) == false) { 
        if (index-original_index >= table->max_col) { 
            return false; 
        } 
    } 
    return true; 
} 
HASHTABLE_STATIC_IMPL HASHTABLE_METADATA* HASHTABLE_GET_DATA(HASHTABLE_NAME* table,HASHTABLE_KEYTYPE key) { 
    if (HASHTABLE_COMPARE(key, HASHTABLEDEFAULTTYPEVALUE)) {
        fprintf(stderr,"[hashtable get] Reserved key, value not enteredn"); 
        return NULL;
    } 
    HASHTABLE_USIZE index = HASHTABLE_HASHFUNC(key); 
    HASHTABLE_METADATA* cur; 
    if (HASHTABLE_COMPARE((cur = HASHTABLE_GET(table,index))->key,key)) { 
        return cur; 
    } 
    HASHTABLE_USIZE original_index = index; 
    while (HASHTABLE_COMPARE((cur = HASHTABLE_GET(table,++index))->key,key) == false) { 
        if (index-original_index >= table->max_col) { 
            return NULL; 
        } 
    } 
    return cur; 
}

#endif
#undef HASHTABLE_IMPLEMENTATION

#undef HASHTABLE_STATIC
#undef HASHTABLE_USIZE
#undef HASHTABLEDEFAULTFILLVALUE
#undef HASHTABLEDEFAULTTYPEVALUE

#undef HASHTABLE_NAME
#undef HASHTABLE_KEYTYPE
#undef HASHTABLE_DATATYPE

#undef HASHTABLE_HASHFUNC
#undef HASHTABLE_COMPARE
#undef HASHTABLE_FREE

#undef HASHTABLE_METADATA
#undef HASHTABLE_BYTES
#undef HASHTABLE_NEW
#undef HASHTABLE_FREENODE
#undef HASHTABLE_CLEANUP
#undef HASHTABLE_GET
#undef HASHTABLE_SET
#undef HASHTABLE_INSERT
#undef HASHTABLE_REMOVE
#undef HASHTABLE_RESIZE