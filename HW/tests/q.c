#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include "thsque.h"

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

void *writer(void *data)
{
    char buf[64];
    int s;
    const pthread_t pid = pthread_self();
    for (int i = 0; i < rnd(); i++) {
        if (snprintf(buf, 64, "%d ttt %d", i, rnd()) == -1)
            exit(EXIT_FAILURE);
        s = queue_push(&q, (void *)buf);
        if (s == -1)
            exit(EXIT_FAILURE);
        printf("%lu put %s qsize %d\n", pid, buf, s);
        usleep(321 + rnd());
    }
    return NULL;
}

void *reader(void *data)
{
    pthread_t pid = pthread_self();
    void *d = (void *) "1";
    char *c;
    while(d != NULL) {
        c = (char *) d;
        printf("%lu pop %s len %ld\n", pid, c, strlen(c));
        d = queue_pop(&q);
    }
    return NULL;
}