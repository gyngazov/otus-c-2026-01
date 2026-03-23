#include "logger.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <execinfo.h>
#include <bool.h>

bool is_level_ok(char *lvl)
{
    return *lvl == 'D' || *lvl == 'I' || *lvl == 'W' || *lvl == 'E';
}
/*
 * Инитим:
 * - именем лога
 * - приоритетом, ниже которого не логировать.
 */

void init_logger(char *log_file, char *lvl)
{
    if (!is_level_ok(lvl))
        exit(ERROR_LEVEL);
    log_level = lvl;
    FILE *fp;
    fp = fopen(log_file, "a");
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
    int ret = sprintf(output, template, timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
            timeinfo->tm_mday, timeinfo->tm_hour,
            timeinfo->tm_min, timeinfo->tm_sec);
    if (ret < 0)
        exit(ERROR_DATATIME);
    return output;
}

void f_print(char *format, char *msg)
{
    if (fprintf(logger->log_file, format, msg) < 0)
        exit(ERROR_WRITE);
}

static void trace()
{
    void *buffer[STACK_BUF_LEN];
    char **strings;

    int nptrs = backtrace(buffer, STACK_BUF_LEN);
    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL)
        exit(ERROR_NOSTACK);
    f_print(" %s", "stack_trace: {"); 
    for (int j = 0; j < nptrs; j++)
        f_print(" %s", strings[j]);
    f_print("%s", "}");
    free(strings);
}

// надо ли логировать сообщение с данным приоритетом
static bool is_log(char *lvl)
{
    bool res;
    if (*lvl == 'E')
        res = true;
    else if (*log_level == 'E')
        res = false;
    else if (*lvl < *log_level)
        res = false;
    else
        res = true;
    return res;
}

/*
 * Запись в лог в формате:
 * - метка времени
 * - приоритет
 * - id потока
 * - номер строки
 * - сообщение
 * - стек трейс для ERROR
 */
void append_log(char *level, char *msg, int line)
{
    if (!is_log(level))
        return;
    pthread_mutex_t mutex = logger->log_mutex;
    char *datetime = get_date_time();
    int ret = pthread_mutex_lock(&mutex);
    if (ret != 0)
        exit(ret);
    ret = fprintf(logger->log_file, "%s %s [thread-%ld] [line-%d] %s", 
                  datetime, level, pthread_self(), line, msg);
    free(datetime);
    if (ret < 0)
        exit(ERROR_WRITE);
    if (*level == 'E')
        trace();
    f_print("%s", "\n");
    ret = pthread_mutex_unlock(&mutex);
    if (ret != 0)
        exit(ret);
}
