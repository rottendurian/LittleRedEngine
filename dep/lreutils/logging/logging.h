#ifndef logging_h
#define logging_h

#include <stdio.h>
#include <stdarg.h>

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
} LogLevel;

extern void LOGSELECTFILE(const char* file);
extern void LOGCLOSEFILE();
extern void LOGINFO(LogLevel level,char* file,int line,const char* format,...);

#define LOGTOFILE(level,...) LOGINFO(level,__FILE__,__LINE__,__VA_ARGS__);

#undef LOGINFO

#define ASSERT(c,...) if (!c) {LOGTOFILE(LOG_LEVEL_ERROR,__VA_ARGS__);}



#endif
