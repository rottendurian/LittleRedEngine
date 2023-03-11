#ifndef stack_h
#define stack_h

#include "malloc.h"
#include "string.h"

#ifndef STACKDEFAULTVALUE
#define STACKDEFAULTVALUE 0
#endif


#define stack(type) \
typedef struct stack_##type { \
    type* ptr; \
    size_t size; \
} stack_##type; \
\
static inline type* stack_##type##_start(stack_##type* stack); \
static inline stack_##type* stack_##type##_create(size_t size); \
static inline type stack_##type##_pop(stack_##type* stack); \
static inline stack_##type* stack_##type##_resize(stack_##type* stack); \
static inline stack_##type* stack_##type##_push(stack_##type* stack,type item); \
static inline stack_##type##_cleanup(stack_##type* stack); \
\
static inline type* stack_##type##_start(stack_##type* stack) { \
    return (type*)(((char*)stack)+sizeof(stack_##type)); \
} \
static inline stack_##type* stack_##type##_create(size_t size) { \
    stack_##type* stack = (stack_##type*)malloc(sizeof(stack_##type)+sizeof(type)*size); \
    stack->ptr = stack_##type##_start(stack) + size; \
    stack->size = size; \
    return stack; \
} \
static inline stack_##type* stack_##type##_resize(stack_##type* stack) { \
    size_t new_size = stack->size*2; \
    stack_##type* new_stack = (stack_##type*)malloc(sizeof(stack_##type)+sizeof(type)*new_size); \
    new_stack->size = new_size; \
    new_stack->ptr = stack_##type##_start(new_stack) + stack->size; \
    memcpy(new_stack->ptr,stack->ptr,stack->size*sizeof(type)); \
    free(stack); \
    return new_stack; \
} \
static inline type stack_##type##_pop(stack_##type* stack) { \
    if (stack->ptr >= stack_##type##_start(stack)+stack->size) { \
        return STACKDEFAULTVALUE; \
    } else { \
        type item = *stack->ptr; \
        stack->ptr += 1; \
        return item; \
    } \
} \
static inline type stack_##type##_get(stack_##type* stack) { \
    if (stack->ptr >= stack_##type##_start(stack)+stack->size) { \
        return STACKDEFAULTVALUE; \
    } else { \
        return *stack->ptr; \
    } \
} \
static inline stack_##type* stack_##type##_push(stack_##type* stack,type item) { \
    if (stack->ptr <= stack_##type##_start(stack)) { \
        stack = stack_##type##_resize(stack); \
    } \
    stack->ptr -= 1; \
    *stack->ptr = item; \
    return stack; \
} \
static inline stack_##type##_cleanup(stack_##type* stack) { \
    free(stack); \
} \


#endif



//  stack_size_t* stack = stack_size_t_create(3);
//     printf("push\n");
//     stack = stack_size_t_push(stack,5);
//     printf("push\n");
//     stack = stack_size_t_push(stack,4);
//     printf("push\n");
//     stack = stack_size_t_push(stack,3);
//     printf("push\n");
//     stack = stack_size_t_push(stack,2);
//     printf("push\n");
//     stack = stack_size_t_push(stack,1);
//     printf("push\n");
//     stack = stack_size_t_push(stack,1);

//     print_out_struct((size_t*)stack,stack->size);

//     // size_t* itemptr = stack_size_t_start(stack);
//     // for (int i = 0; i <= stack->size+1; i++) {
//     //     printf("itemptr %zu\nitem %zu\n",itemptr,*itemptr);
//     //     itemptr++;
//     // }

//     size_t item = stack_size_t_pop(stack);
//     printf("item %zu\n",item);
//     item = stack_size_t_pop(stack);
//     printf("item %zu\n",item);
//     item = stack_size_t_pop(stack);
//     printf("item %zu\n",item);
//     item = stack_size_t_pop(stack);
//     printf("item %zu\n",item);
//     item = stack_size_t_pop(stack);
//     printf("item %zu\n",item);
//     item = stack_size_t_pop(stack);
//     printf("item %zu\n",item);
//     item = stack_size_t_pop(stack);
//     printf("item %zu\n",item);

//     printf("push\n");
//     stack = stack_size_t_push(stack,1);

//     item = stack_size_t_pop(stack);
//     printf("item %zu\n",item);

//     print_out_struct((size_t*)stack,stack->size);


//     stack_size_t_cleanup(stack);