#include <stdio.h>
#include <pthread.h>

#define ERROR_OPEN      -100
#define ERROR_INIT      -99
#define ERROR_CREATE    -98
#define ERROR_JOIN      -97
#define ERROR_WRITE     -96
#define ERROR_NOMEM     -95
#define ERROR_NOSTACK   -94
#define ERROR_DATATIME  -93
#define ERROR_LEVEL     -92
#define STACK_BUF_LEN   128

#define debug(msg)     append_log("DEBUG", msg, __LINE__)
#define info(msg)      append_log("INFO", msg, __LINE__)
#define warning(msg)   append_log("WARNING", msg, __LINE__)
#define error(msg)     append_log("ERROR", msg, __LINE__)

struct Log_handler {
    FILE *log_file;
    pthread_mutex_t log_mutex;
};

char *log_level;    // не логируем критичность ниже этой

struct Log_handler *logger;
// создать logger
void init_logger(char *log_file, char *level);
// выгрузить логгер
void close_logger();
void append_log(char *level, char *msg, int line);
