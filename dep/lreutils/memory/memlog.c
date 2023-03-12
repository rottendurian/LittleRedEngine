#ifndef memlog_c
#define memlog_c

#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>

#include "../hashtable/hashset.h"
#include "../hashtable/hashfunctions.h"

#include <stdarg.h>

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
} LogLevel;

static FILE* LOGGING_FILE;

static void LOGSELECTFILE(const char* file) {
    LOGGING_FILE = fopen(file,"w");
}
static void LOGCLOSEFILE() {
    fclose(LOGGING_FILE);
}

static void LOGINFO(LogLevel level,char* file,int line,const char* format,...) {
    switch (level) { \
        case LOG_LEVEL_DEBUG: 
            fprintf(LOGGING_FILE,"[DEBUG] "); 
            break; 
        case LOG_LEVEL_INFO: \
            fprintf(LOGGING_FILE,"[INFO] "); 
            break; 
        case LOG_LEVEL_WARNING: 
            fprintf(LOGGING_FILE,"[WARNING] "); 
            break; 
        case LOG_LEVEL_ERROR: 
            fprintf(LOGGING_FILE,"[ERROR] "); 
            break; 
    }
    va_list args;
    va_start(args,format);
    vfprintf(LOGGING_FILE,format,args);
    va_end(args);
    fprintf(LOGGING_FILE," [%s,line:%d]\n",file,line);

    if (level == LOG_LEVEL_ERROR) exit(1);

}



#define LOGTOFILE(level,...) LOGINFO(level,file,line,__VA_ARGS__);

#undef LOGINFO

#define ASSERT(c,...) if (!c) {LOGTOFILE(LOG_LEVEL_ERROR,__VA_ARGS__);}

#undef malloc
#undef realloc
#undef free



static size_t hashFunction(size_t x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

static bool hashset_cmp(size_t x, size_t y) {return x == y;}

hashset(size_t,jenkins_hash,hashset_cmp,SETNOTHEAP);

static char are_logs_setup = 0;
static hashset_size_t* table;

void log_setup() {
    are_logs_setup = 1;
    LOGSELECTFILE("memlog.log");
    table = hashset_size_t_create();
}

void* log_malloc(size_t size,char* file,int line) {
    if (are_logs_setup == 0) log_setup();
    void* memory = malloc(size);
    if (memory == NULL) {
        LOGINFO(LOG_LEVEL_ERROR,file,line,"Failed to allocate memory: ");
    }
    LOGINFO(LOG_LEVEL_DEBUG,file,line,"Malloc(%llu)",(size_t)memory);
    // fprintf(stderr,"Malloc(%llu) in %s, line %d\n",(size_t)memory,file,line);

    table = hashset_size_t_insert(table,(size_t)memory);

    return memory;
}

void log_free(void* data,char* file,int line) {
    if (data != NULL) {
        LOGINFO(LOG_LEVEL_DEBUG,file,line,"Free(%llu)",(size_t)data);
        // fprintf(stderr,"Free(%llu) in %s, line %d\n",(size_t)data,file,line);
        table = hashset_size_t_remove(table,(size_t)data);
    }
    free(data);
}

void* log_realloc(void* prev,size_t size, char* file, int line) {
    LOGINFO(LOG_LEVEL_DEBUG,file,line,"Removing (%llu) to realloc",(size_t)prev);
    hashset_size_t_remove(table,(size_t)prev);
    void* memory = realloc(prev,size);
    if (memory == NULL) {
        LOGINFO(LOG_LEVEL_WARNING,file,line,"Failed to realloc memory");
        // fprintf(stderr,"Failed to reallocate memory: \n");
    }
    LOGINFO(LOG_LEVEL_DEBUG,file,line,"Realloc(%llu)",(size_t)memory);
    // fprintf(stderr,"Realloc(%llu) in %s, line %d\n",(size_t)memory,file,line);
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
            LOGINFO(LOG_LEVEL_WARNING,"memlog.c",133,"Failed to free %llu",meta->key);
        }
    }
    // fprintf(stderr,"Max collisions %llu\n",table->max_col);

    hashset_size_t_cleanup(table);
    LOGCLOSEFILE();
}

#endif