#include "logger.h"
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

void *job(void *attr);

int main()
{
    init_logger();
    printf("logger: %p\n", (void *)logger);
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, job, NULL); 
    pthread_create(&tid2, NULL, job, NULL); 
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    close_logger();
    return 0;
}

void *job(void *attr)
{
    attr = NULL;
    srand(time(attr));
    for (int i = 0; i < 11; i++) {
        if (rand() % 2 == 0) {
            error("error mesg");
            sleep(1);
        } else {
            info("info mesg");
        }
    }
    return NULL;
}

