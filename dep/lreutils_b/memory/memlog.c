#ifndef memlog_c
#define memlog_c

#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>

#include "../hashtable/hashset.h"
#include "../hashtable/hashfunctions.h"

#undef malloc
#undef realloc
#undef free

#ifndef NDEBUG

// static size_t hashFunction(size_t x) {
//     x = ((x >> 16) ^ x) * 0x45d9f3b;
//     x = ((x >> 16) ^ x) * 0x45d9f3b;
//     x = (x >> 16) ^ x;
//     return x;
// }

static bool hashset_cmp(size_t x, size_t y) {return x == y;}

hashset(size_t,jenkins_hash,hashset_cmp,SETNOTHEAP);

static char are_logs_setup = 0;
static hashset_size_t* table;

void log_setup() {
    are_logs_setup = 1;
    freopen("stderr.log", "w", stderr);
    table = hashset_size_t_create();
}

void* log_malloc(size_t size,char* file,int line) {
    if (are_logs_setup == 0) log_setup();
    void* memory = malloc(size);
    if (memory == NULL) {
        fprintf(stderr,"Failed to allocate memory: ");
    }
    fprintf(stderr,"Malloc(%llu) in %s, line %d\n",(size_t)memory,file,line);

    table = hashset_size_t_insert(table,(size_t)memory);

    return memory;
}

void log_free(void* data,char* file,int line) {
    if (data != NULL) {
        fprintf(stderr,"Free(%llu) in %s, line %d\n",(size_t)data,file,line);
        table = hashset_size_t_remove(table,(size_t)data);
    }
    free(data);
}

void* log_realloc(void* prev,size_t size, char* file, int line) {
    fprintf(stderr,"Removing (%llu) to realloc\n",(size_t)prev);
    hashset_size_t_remove(table,(size_t)prev);
    void* memory = realloc(prev,size);
    if (memory == NULL) {
        fprintf(stderr,"Failed to reallocate memory: \n");
    }
    fprintf(stderr,"Realloc(%llu) in %s, line %d\n",(size_t)memory,file,line);
    table = hashset_size_t_insert(table,(size_t)memory);
    return memory;
}

void log_cleanup() {
    if (table == NULL) return;
    size_t size = set_internal_primes[table->mem_size]; 
    hashset_size_t_meta* meta;
    for (size_t i = 0; i < size; i++) { 
        meta = _hashset_size_t_get(table,i); 
        if (meta->key != HASHTABLEDEFAULTVALUE) {
            fprintf(stderr, "Failed to free %llu",meta->key);
        }
    }
    fprintf(stderr,"Max collisions %llu\n",table->max_col);

    hashset_size_t_cleanup(table);
}
#endif

#endif