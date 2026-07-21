#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include "thsque.h"
#include "copy.h"
#include "ranger.h"

// MVP режим
#define MIN 111
#define MAX 901


int main (int argc, char **argv)
{
    ThreadSafeQueue q;
    queue_init(&q);
    int err;
    const int n = 2; // потоков чтения очереди
    pthread_t rthrds[n];
    struct ThreadData *rthd;
    for (int i = 0; i < n; i++) { // сначала читатели очереди
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

    const int m = 3; // потоков записи в очередь
    pthread_t wthrds[m];
    const int thread_batch = (MAX - MIN)/m;
    struct ThreadData *wthd;
    int k;
    for (int i = MIN; i <= MAX; i += thread_batch) {
        wthd = (struct ThreadData *) malloc(sizeof(struct ThreadData));
        wthd->tsq = &q;
        wthd->start = i;
        k = i +  thread_batch - 1;
        wthd->last = MAX - k < thread_batch ? MAX : k; // добавить остаток от деления на m в посл. поток
        err = pthread_create(&wthrds[i], NULL, writer, (void *) wthd);
        if (err != 0) {
            perror("A new thread cannot be created");
            exit(errno);
        }
    }

    void *res;
    
    for (int i = 0; i < m; i++) {
        err = pthread_join(wthrds[i], &res); // сначала писатели в очередь
        if (err != 0)
            printf("Поток не завершается\n");
        else if ((int*)res == PTHREAD_CANCELED)
            printf("Поток отменен");
    }
    
    queue_shutdown(&q); // выставить, что писателей нет
    puts("shutdown");

    for (int i = 0; i < n; i++) {
        err = pthread_join(rthrds[i], &res);
        if (err != 0)
            printf("Поток не завершается\n");
        else if ((int*)res == PTHREAD_CANCELED)
            printf("Поток отменен");
    }   

    queue_destroy(&q);

    return EXIT_SUCCESS;
}

