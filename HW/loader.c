#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "thsque.h"
#include "copy.h"
#include "ranger.h"

// демо
//минимальный id в таблице источнике
#define MIN             111
//максимальный id в таблице источнике
#define MAX             901
// число потоков, читающих очередь
#define QUEUE_READERS   2
// число потоков, пишущих в очередь
#define QUEUE_WRITERS   3

/**
 * Перенос строк одной таблицы между двумя серверами разных вендоров sql.
 * Многопоточное чтение источника пачками.
 * Получатель данных - postgresql.
 * Запись на получателе с использованием COPY.
 * Многопоточная запись.
 * Буферизация пачек через потокобезопасный односвязный список.
 * Все строки таблицы в источнике разбиваются на группы в количестве, равном числу потоков QUEUE_WRITERS
 * Поток в своей группе еще разбивает ее на пачки.
 * По каждой пачке делается запрос в источнике.
 * Данные по пачке пушатся в очередь.
 * Источник - mysql.
 */

int main ()
{
    ThreadSafeQueue q;
    queue_init(&q);
    int err;
 
    pthread_t rthrds[QUEUE_READERS];
    struct ThreadData *rthd;
    // сначала читатели очереди
    for (int i = 0; i < QUEUE_READERS; i++) { 
        rthd = (struct ThreadData *) malloc(sizeof(struct ThreadData));
        if (rthd == NULL)
            exit(EXIT_FAILURE);
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
        if (wthd == NULL)
            exit(EXIT_FAILURE);
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

