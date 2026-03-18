#include "logger.h"
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

void *job(void *attr);

void main()
{
    init_logger();
    printf("logger: %p\n", logger);
    int a = 1, b = 2;
    pthread_t tid1;
    pthread_create(&tid1, NULL, job, &a); 
    pthread_t tid2;
    pthread_create(&tid2, NULL, job, &b); 
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    close_logger();
}

void *job(void *attr)
{
    srand(time(NULL));
    int *t = (int *) attr;
    for (int i = 0; i < 11; i++) {
        if (rand() % 2 == 0)
            sleep(1);
        append_log(*t, i);
    }
}

