#include "stdlib.h"
#include "string.h"

#ifdef ARRAY_STATIC
    #define ARRAY_STATIC_IMPL static inline
    #define ARRAY_IMPLEMENTATION
#endif

#ifndef ARRAY_USIZE
    #define ARRAY_USIZE uint32_t
#endif

#ifndef ARRAY_DEFAULT_VALUE
    #define ARRAY_DEFAULT_VALUE 0
#endif
#ifndef ARRAY_TYPE
    #define ARRAY_TYPE int
#endif

#ifndef ARRAY_NAME
    #define ARRAY_NAME array
#endif

typedef struct ARRAY_NAME { 
    ARRAY_USIZE items; 
    ARRAY_USIZE size; 
} ARRAY_NAME; 

//c is cringe
#define ARRAY_EXPANSION(name,arg) name##arg
#define _ARRAY_EXPANSION(name,arg) _##name##arg
#define ARRAY_EXPANSION_I(name,arg) ARRAY_EXPANSION(name,arg)
#define _ARRAY_EXPANSION_I(name,arg) _ARRAY_EXPANSION(name,arg)

#define ARRAY_NEW ARRAY_EXPANSION_I(ARRAY_NAME,_new)
#define ARRAY_RESIZE ARRAY_EXPANSION_I(ARRAY_NAME,_resize)
#define ARRAY_INSERT ARRAY_EXPANSION_I(ARRAY_NAME,_insert)
#define ARRAY_PUSH ARRAY_EXPANSION_I(ARRAY_NAME,_push)
#define ARRAY_REMOVE ARRAY_EXPANSION_I(ARRAY_NAME,_remove)
#define ARRAY_CLEANUP ARRAY_EXPANSION_I(ARRAY_NAME,_cleanup)
#define ARRAY_GET _ARRAY_EXPANSION_I(ARRAY_NAME,_get)
#define ARRAY_SET _ARRAY_EXPANSION_I(ARRAY_NAME,_set)

ARRAY_STATIC_IMPL ARRAY_NAME* ARRAY_NEW(ARRAY_USIZE size); 
ARRAY_STATIC_IMPL ARRAY_NAME* ARRAY_RESIZE(ARRAY_NAME* array); 
ARRAY_STATIC_IMPL ARRAY_NAME* ARRAY_INSERT(ARRAY_NAME* array, ARRAY_USIZE index, ARRAY_TYPE item); 
ARRAY_STATIC_IMPL ARRAY_NAME* ARRAY_PUSH(ARRAY_NAME* array,ARRAY_TYPE item); 
ARRAY_STATIC_IMPL ARRAY_TYPE ARRAY_REMOVE(ARRAY_NAME* array,ARRAY_USIZE index); 
ARRAY_STATIC_IMPL void ARRAY_CLEANUP(ARRAY_NAME* array); 

#ifdef ARRAY_IMPLEMENTATION

ARRAY_STATIC_IMPL ARRAY_TYPE* ARRAY_GET(ARRAY_NAME* array, ARRAY_USIZE index) { 
    return ((ARRAY_TYPE*)(((char*)array)+sizeof(ARRAY_NAME)))+index; 
} 
ARRAY_STATIC_IMPL void ARRAY_SET(ARRAY_NAME* array, ARRAY_USIZE index,ARRAY_TYPE item) { 
    *(((ARRAY_TYPE*)(((char*)array)+sizeof(ARRAY_NAME)))+index) = item; 
} 

ARRAY_STATIC_IMPL ARRAY_NAME* ARRAY_NEW(ARRAY_USIZE size) { 
    ARRAY_NAME* arr = (ARRAY_NAME*)malloc(sizeof(ARRAY_NAME)+size*sizeof(ARRAY_TYPE));
    memset(((char*)arr)+sizeof(ARRAY_NAME),ARRAY_DEFAULT_VALUE,sizeof(ARRAY_TYPE)*size); 
    arr->items = 0; 
    arr->size = size; 
    return arr; 
} 

ARRAY_STATIC_IMPL ARRAY_NAME* ARRAY_RESIZE(ARRAY_NAME* array) { 
    ARRAY_USIZE newSize = array->size*2; 
    array = (ARRAY_NAME*)realloc(array,sizeof(ARRAY_NAME)+newSize*sizeof(ARRAY_TYPE)); 
    memset(((char*)array)+sizeof(ARRAY_NAME)+array->size*sizeof(ARRAY_TYPE),ARRAY_DEFAULT_VALUE,sizeof(ARRAY_TYPE)*(array->size)); 
    array->size = newSize; 
    return array; 
} 

ARRAY_STATIC_IMPL ARRAY_NAME* ARRAY_PUSH(ARRAY_NAME* array,ARRAY_TYPE item) { 
    if (array->items >= array->size) { 
        array = ARRAY_RESIZE(array); 
    } 
    ARRAY_SET(array,array->items,item); 
    array->items++; 
    return array; 
} 

ARRAY_STATIC_IMPL ARRAY_NAME* ARRAY_INSERT(ARRAY_NAME* array, ARRAY_USIZE index, ARRAY_TYPE item) { 
    if (index == -1) { 
        index = array->items; 
        array->items++; 
    } 
    if (index >= array->items) { 
        array->items = index+1; 
    } 
    ARRAY_SET(array,index,item); 
    return array; 
} 

ARRAY_STATIC_IMPL ARRAY_TYPE ARRAY_REMOVE(ARRAY_NAME* array,ARRAY_USIZE index) { 
    ARRAY_TYPE* itemptr = ARRAY_GET(array,index); 
    ARRAY_TYPE item = *itemptr; 
    memset(itemptr,0,sizeof(ARRAY_TYPE)); 
    if (index == array->items-1) { 
        array->items--; 
    } 
    return item; 
} 

ARRAY_STATIC_IMPL void ARRAY_CLEANUP(ARRAY_NAME* array) { 
    free(array); 
}

#endif

#undef ARRAY_NEW
#undef ARRAY_RESIZE
#undef ARRAY_INSERT
#undef ARRAY_PUSH
#undef ARRAY_REMOVE
#undef ARRAY_CLEANUP
#undef ARRAY_GET
#undef ARRAY_SET

#undef ARRAY_STATIC_IMPL
#undef ARRAY_DEFAULT_VALUE
#undef ARRAY_TYPE
#undef ARRAY_NAME
