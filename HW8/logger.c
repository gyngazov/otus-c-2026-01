#include "logger.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_logger()
{
    FILE *fp;
    fp = fopen(LOG_FILE, "a");
    if (fp == NULL) {
        printf("error opening log\n");
        exit(-1);
    }
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    logger = (struct Log_handler *) malloc(sizeof(struct Log_handler));
    if (logger == NULL) {
        printf("no mem\n");
        exit(-2);
    }
    logger->log_file = fp;
    logger->log_mutex = mutex;
}

static void *add_log(void *message)
{
    pthread_mutex_t mutex = logger->log_mutex;
    pthread_mutex_lock(&mutex);
    char *msg = (char *) message;
    fwrite(msg, 1, strlen(msg) + 1, logger->log_file);
    pthread_mutex_unlock(&mutex);
}

void write_log(char *msg)
{
    pthread_t tid;
    pthread_create(&tid, NULL, add_log, msg); 
    pthread_join(tid, NULL);   
}
