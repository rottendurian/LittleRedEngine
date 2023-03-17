#ifndef array_h
#define array_h

#include "malloc.h"
#include "string.h"

// #ifndef ARRAY_DEFAULT_VALUE
// #define ARRAY_DEFAULT_VALUE 0
// #endif

#define array(type,ARRAY_DEFAULT_VALUE) \
typedef struct array_##type { \
    size_t items; \
    size_t size; \
} array_##type; \
\
static inline array_##type* array_##type##_new(size_t size); \
static inline array_##type* array_##type##_resize(array_##type* array); \
static inline array_##type* array_##type##_insert(array_##type* array, size_t index, type item); \
static inline array_##type* array_##type##_push(array_##type* array,type item); \
static inline type array_##type##_remove(array_##type* array,size_t index); \
static inline void array_##type##_cleanup(array_##type* array); \
\
static inline type* _array_##type##_get(array_##type* array, size_t index) { \
    return ((type*)(((char*)array)+sizeof(array_##type)))+index; \
} \
static inline void _array_##type##_set(array_##type* array, size_t index,type item) { \
    *(((type*)(((char*)array)+sizeof(array_##type)))+index) = item; \
} \
\
static inline array_##type* array_##type##_new(size_t size) { \
    array_##type* arr = (array_##type*)malloc(sizeof(array_##type)+size*sizeof(type));\
    memset(((char*)arr)+sizeof(array_##type),ARRAY_DEFAULT_VALUE,sizeof(type)*size); \
    arr->items = 0; \
    arr->size = size; \
    return arr; \
} \
\
static inline array_##type* array_##type##_resize(array_##type* array) { \
    size_t newSize = array->size*2; \
    array = (array_##type*)realloc(array,sizeof(array_##type)+newSize*sizeof(type)); \
    memset(((char*)array)+sizeof(array_##type)+array->size*sizeof(type),ARRAY_DEFAULT_VALUE,sizeof(type)*(array->size)); \
    array->size = newSize; \
    return array; \
} \
\
static inline array_##type* array_##type##_push(array_##type* array,type item) { \
    if (array->items >= array->size) { \
        array = array_##type##_resize(array); \
    } \
    _array_##type##_set(array,array->items,item); \
    array->items++; \
    return array; \
} \
\
static inline array_##type* array_##type##_insert(array_##type* array, size_t index, type item) { \
    if (index == -1) { \
        index = array->items; \
        array->items++; \
    } \
    if (index >= array->items) { \
        array->items = index+1; \
    } \
    _array_##type##_set(array,index,item); \
    return array; \
} \
\
static inline type array_##type##_remove(array_##type* array,size_t index) { \
    type* itemptr = _array_##type##_get(array,index); \
    type item = *itemptr; \
    memset(itemptr,0,sizeof(type)); \
    if (index == array->items-1) { \
        array->items--; \
    } \
    return item; \
} \
\
static inline void array_##type##_cleanup(array_##type* array) { \
    free(array); \
} \


#endif