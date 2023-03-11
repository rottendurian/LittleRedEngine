// #ifndef lre_helper_h
// #define lre_helper_h

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdbool.h>
// #include <stdint.h>
// #include "lre_render.h"

// // #ifdef NDEBUG
// // #else
// //     #define LOGMEMORYALLOCATIONS
// // #endif

// // #ifdef LOGMEMORYALLOCATIONS
// //     #include "memory/memlog.h"
// // #endif

// #define LRE_SET_DEFAULT_VALUE -1

// typedef struct lreSet {
//     uint32_t* set;
//     uint32_t size;
//     uint32_t filled;
// } lreSet;

// static inline lreSet lreCreateSet(uint32_t setSize);
// static inline void lreInsertSet(lreSet* set,uint32_t value);
// static inline void lreResizeSet(lreSet* set);
// static inline void lreCleanUpSet(lreSet* set);

// static inline void lreResizeSet(lreSet* set) {
//     uint32_t* originalSet = set->set;
//     uint32_t originalSize = set->size;

//     set->set = malloc(set->size*2*sizeof(uint32_t));
//     if (set->set == NULL) {
//         fprintf(stderr,"[lreResizeSet] Failed to resize set");
//         exit(EXIT_FAILURE);
//     }
//     set->filled = 0;
//     set->size = set->size*2;
//     memset(&set->set,LRE_SET_DEFAULT_VALUE,set->size*sizeof(uint32_t));

//     for (int i = 0; i < originalSize; i++) {
//         if (originalSet[i] != LRE_SET_DEFAULT_VALUE) {
//             lreInsertSet(set,originalSet[i]);
//         }
//     }
    
//     free(originalSet);
// }

// static inline void lreInsertSet(lreSet* set,uint32_t value) {
//     if (value == LRE_SET_DEFAULT_VALUE) {
//         fprintf(stderr, "[LRE SET] Error inputting a default value into set, sets default value is %i",LRE_SET_DEFAULT_VALUE);
//         exit(EXIT_FAILURE);
//     }
//     if (set->filled == set->size) {
//         lreResizeSet(set);
//     }

//     // printf("set size %u\n",set->size);
//     uint32_t index = 0;
//     if (value != 0) {
//         index = (uint32_t)(set->size % value)-1;
//     }
    
//     // printf("index %u, size %u\n",index,set->size);

//     if (set->set[index] != LRE_SET_DEFAULT_VALUE) {
//         if (set->set[index] == value) {
//             return;
//         }
        
//         lreResizeSet(set);
//         lreInsertSet(set,value);
//         return;
//     }

//     set->filled+=1;
//     set->set[index] = value;
    
//     // printf("monak\n");
// }

// static inline lreSet lreCreateSet(uint32_t setSize) {
//     lreSet set; //memset(&set,0,sizeof(set));
//     set.size = setSize;
//     set.filled = 0;
//     set.set = (uint32_t*)malloc((setSize)*sizeof(uint32_t));
//     if (set.set == NULL) {
//         fprintf(stderr,"[lreCreateSet] Failed to allocate set memory");
//         exit(EXIT_FAILURE);
//     }
    
//     memset(set.set,LRE_SET_DEFAULT_VALUE,set.size*sizeof(uint32_t));

//     return set;
// }

// static inline void lreCleanUpSet(lreSet* set) {
//     free(set->set);
// }

// static inline uint32_t lreGetSet(lreSet* set,uint32_t index) {
//     return set->set[index];
// }


// #endif
