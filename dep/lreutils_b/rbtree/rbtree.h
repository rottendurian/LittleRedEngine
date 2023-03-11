#ifndef rbtree_h
#define rbtree_h

#include <malloc.h>
#include <stdint.h>
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef rbtree_T
#define rbtree_T uint64_t
#endif

enum rbtree_color {
    rbc_root = 1,
    rbc_black = 0,
    rbc_red = -1,
    rbc_double_black = -2,
};

struct rbtree {
    rbtree_T key;
    struct rbtree_metadata {
        struct rbtree* left;
        struct rbtree* right;
        struct rbtree* parent;
        enum rbtree_color c;
    };
};

struct rbtree* rbtree_create(rbtree_T value);

struct rbtree* rbtree_insert(struct rbtree* head,rbtree_T value);

struct rbtree* rbtree_search(struct rbtree* head,rbtree_T value);

struct rbtree* rbtree_remove(struct rbtree* head,rbtree_T value);

void rbtree_cleanUp(struct rbtree* node);

void rbtree_cleanUpAll(struct rbtree* head);

#endif