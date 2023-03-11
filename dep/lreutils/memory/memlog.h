#ifndef memlog_h
#define memlog_h

extern void log_setup();

extern void* log_malloc(size_t size,char* file,int line);

extern void log_free(void* data,char* file,int line);

extern void* log_realloc(void* prev,size_t size, char* file, int line);

extern void log_cleanup();

#ifdef LOGMEMORYALLOCATIONS

#define malloc(size) log_malloc(size,__FILE__,__LINE__)

#define free(data) log_free(data,__FILE__,__LINE__)

#define realloc(data,size) log_realloc(data,size,__FILE__,__LINE__)

#endif

#endif