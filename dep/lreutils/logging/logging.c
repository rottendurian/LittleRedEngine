#include <stdio.h>
#include <stdarg.h>

#include "logging.h"

static FILE* LOGGING_FILE;

void LOGSELECTFILE(const char* file) {
    LOGGING_FILE = fopen(file,"w");
}
void LOGCLOSEFILE() {
    fclose(LOGGING_FILE);
}

void LOGINFO(LogLevel level,char* file,int line,const char* format,...) {
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
