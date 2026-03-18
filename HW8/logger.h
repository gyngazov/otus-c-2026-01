#include <stdio.h>
#include <pthread.h>

#define LOG_FILE "file.log"

struct Log_handler {
    FILE *log_file;
    pthread_mutex_t log_mutex;
};

struct Log_handler *logger;

void init_logger();
void write_log(char *msg);
