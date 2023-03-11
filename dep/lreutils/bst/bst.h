#ifndef bst_h
#define bst_h

#include <malloc.h>
#include <stdint.h>
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>

/*
 * key type can be set prior to importing/using the library to have
 * some sort of custom typing, only will work if the data is comparable
 */
#ifndef bst_T
#define bst_T uint64_t
#endif

struct bst {
    bst_T key;
    struct bst_metadata {
        struct bst* left;
        struct bst* right;
    };
};

struct bst* bst_create(bst_T value);

struct bst* bst_insert(struct bst* head, bst_T value);

struct bst* bst_search(struct bst* head, bst_T value);

struct bst* bst_remove(struct bst* head, bst_T value);

void bst_cleanUp(struct bst* node);

void bst_cleanUpAll(struct bst* head);

void bst_displayTree(struct bst* head, int depth, char edge);



#ifdef debug 
#include bst.c
#endif

#endif