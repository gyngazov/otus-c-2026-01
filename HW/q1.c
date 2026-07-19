#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include "thsque.h"
#include "copy.h"

ThreadSafeQueue q;

void *writer(void *data);
void *reader(void *data);

int main (int argc, char **argv)
{
    srand(time(NULL));
    queue_init(&q);
    int err;
    const int n = 3;
    pthread_t rthrds[n];
    for (int i = 0; i < n; i++) {
        err = pthread_create(&rthrds[i], NULL, reader, NULL);
        if (err != 0) {
            perror("A new thread cannot be created");
            exit(errno);
        }
    }

    const int m = 5;
    pthread_t wthrds[m];
    for (int i = 0; i < m; i++) {
        err = pthread_create(&wthrds[i], NULL, writer, NULL);
        if (err != 0) {
            perror("A new thread cannot be created");
            exit(errno);
        }
    }

    void *res;
    
    for (int i = 0; i < m; i++) {
        err = pthread_join(wthrds[i], &res);
        if (err != 0)
            printf("Поток не завершается\n");
        else if ((int*)res == PTHREAD_CANCELED)
            printf("Поток отменен");
    }
    
    queue_shutdown(&q);
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

int rnd()
{
    const int a = 17;
    return a + rand() % a;
}

struct Batch create_batch(const pthread_t pid)
{
    struct Batch b;
    b.size = rnd() % MAX_BATCH;
    struct SourceRow sr;
    for (int i = 0; i < b.size; i++) {
        sr.id = rnd() * rnd();
        snprintf(sr.code, CLEN, "%ld", pid);
        snprintf(sr.val, VLEN, "%d", i);
        b.rows[i] = sr;
    }
    return b;
}

void *writer(void *data)
{
    struct Batch *b;
    struct SourceRow *sr;
    
    const pthread_t pid = pthread_self();
    for (int i = 0; i < rnd(); i++) {
        b = (struct Batch *) malloc(sizeof(struct Batch));
        b->size = rnd() % MAX_BATCH;
        for (int i = 0; i < b->size; i++) {
            sr = (struct SourceRow *)malloc(sizeof(struct SourceRow));
            sr->id = rnd() * rnd();
            snprintf(sr->code, CLEN, "%ld", pid);
            snprintf(sr->val, VLEN, "%d", i);
            b->rows[i] = *sr;
        }
        queue_push(&q, (void *) b);
        usleep(rnd() * rnd() / 2);
    }
    printf("wsize: %d id: %d\n", b->size, b->rows[17].id);
    return NULL;
}

void *reader(void *data)
{
    PGconn *conn;
    conn = get_conn();
    struct Batch *b;
    b = (struct Batch *) queue_pop(&q);
    while(b != NULL) {
        insertp(conn, b);
        b = (struct Batch *) queue_pop(&q);
    }
    PQfinish(conn);
    return NULL;
}