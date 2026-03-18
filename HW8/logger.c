#include "logger.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void init_logger()
{
    FILE *fp;
    fp = fopen(LOG_FILE, "a");
    if (fp == NULL)
        exit(errno);
    pthread_mutex_t mutex;
    int ret = pthread_mutex_init(&mutex, NULL);
    if (ret != 0)
        exit(ret);
    logger = (struct Log_handler *) malloc(sizeof(struct Log_handler));
    if (logger == NULL)
        exit(ERROR_INIT);
    logger->log_file = fp;
    logger->log_mutex = mutex;
}

void close_logger()
{
    if (fclose(logger->log_file) != 0)
        exit(errno);
    pthread_mutex_t mutex = logger->log_mutex;
    int ret = pthread_mutex_destroy(&mutex);
    if (ret != 0)
        exit(ret);
}

static char *get_date_time()
{
    time_t now;
    struct tm *timeinfo;
    time(&now);
    timeinfo = localtime(&now);
    char *template = "%d-%02d-%02d %02d:%02d:%02d";
    char *output = (char *) malloc(sizeof(template) + 1);
    if (output == NULL)
        exit(ERROR_NOMEM);
    sprintf(output, template, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
            timeinfo->tm_mday, timeinfo->tm_hour,
            timeinfo->tm_min, timeinfo->tm_sec);
    return output;
}

void append_log(int thread, int i)
{
    pthread_mutex_t mutex = logger->log_mutex;
    int ret = pthread_mutex_lock(&mutex);
    if (ret != 0)
        exit(ret);
    char *datetime = get_date_time();
    ret = fprintf(logger->log_file, "%s [thread-%ld] string: %d\n", datetime, pthread_self(), i);
    free(datetime);
    if (ret < 0) 
        exit(ERROR_WRITE);
    ret = pthread_mutex_unlock(&mutex);
    if (ret != 0)
        exit(ret);
}
