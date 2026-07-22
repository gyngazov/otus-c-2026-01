#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "thsque.h"
#include "copy.h"
#include "ranger.h"

// MVP режим
#define MIN             111
#define MAX             901
#define QUEUE_READERS   2
#define QUEUE_WRITERS   3


int main (/* int argc, char **argv */)
{
    ThreadSafeQueue q;
    queue_init(&q);
    int err;
 
    pthread_t rthrds[QUEUE_READERS];
    struct ThreadData *rthd;
    // сначала читатели очереди
    for (int i = 0; i < QUEUE_READERS; i++) { 
        rthd = (struct ThreadData *) malloc(sizeof(struct ThreadData));
        rthd->start = 0;
        rthd->last = 0;
        rthd->tsq = &q;
        err = pthread_create(&rthrds[i], NULL, reader, (void *) rthd);
        if (err != 0) {
            perror("A new thread cannot be created");
            exit(errno);
        }
    }

    pthread_t wthrds[QUEUE_WRITERS];
    const int thread_batch = (MAX - MIN)/QUEUE_WRITERS;
    struct ThreadData *wthd;
    int k;
    for (int i = MIN; i <= MAX; i += thread_batch) {
        wthd = (struct ThreadData *) malloc(sizeof(struct ThreadData));
        wthd->tsq = &q;
        wthd->start = i;
        k = i +  thread_batch - 1;
        wthd->last = MAX - k < thread_batch ? MAX : k; // добавить остаток от деления в посл. поток
        err = pthread_create(&wthrds[i], NULL, writer, (void *) wthd);
        if (err != 0) {
            perror("A new thread cannot be created");
            exit(errno);
        }
    }

    void *res;
    // сначала писатели в очередь
    for (int i = 0; i < QUEUE_WRITERS; i++) {
        err = pthread_join(wthrds[i], &res); 
        if (err != 0)
            printf("Поток не завершается\n");
        else if ((int*)res == PTHREAD_CANCELED)
            printf("Поток отменен");
    }

    // выставить, что писателей нет
    queue_shutdown(&q); 
    puts("shutdown");

    for (int i = 0; i < QUEUE_READERS; i++) {
        err = pthread_join(rthrds[i], &res);
        if (err != 0)
            printf("Поток не завершается\n");
        else if ((int*)res == PTHREAD_CANCELED)
            printf("Поток отменен");
    }   

    queue_destroy(&q);

    return EXIT_SUCCESS;
}

