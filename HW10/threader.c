#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "hash.h"
#include "parse.h"

struct Cache {
    int num;      
    pthread_t tid;         
    FILE **files; 
    GHashTable *queries;
    GHashTable *refs;
};

void * worker(void * arg)
{
    struct Cache *cache = (struct Cache *) arg;
    cache->tid = pthread_self();
    FILE *fp;
    char buf[BUF_LEN];
    struct LogLine *ll;

    for (int i = 0; i < cache->num; i++) {
        fp = cache->files + i;
        while (fgets(buf, BUF_LEN, fp) != NULL) {
            ll = parse_line(buf);
            inc(cache->refs, ll->ref, 1);
            inc(cache->queries, ll->url, ll->size);
        }
    }
    return((void *)cache);
}


int main(int argc, char **argv)
{
    // if (argc != 3) {
    //     printf("usage: %s {logs_dir} {number_of_threads}");
    //     return EXIT_FAILURE;
    // }

    int n = 7;
    pthread_t thrds[n];
    int err;
    for (int i = 0; i < n; i++) {
        err = pthread_create(&thrds[i], NULL, thr_fcn, NULL);
        if (err != 0) {
            perror("A new thread cannot be created");
            return errno;
        }

    }
    sleep(7);
    void *res;
    for (int i = 0; i < n; i++) {
        err = pthread_join(thrds[i], &res);
        if (err != 0)
            printf("A slave thread is not joinable because it is detached. \n");
        else if ((int*)res == PTHREAD_CANCELED)
            printf("A slave thread was canceled");
        else
            printf("A slave thread returned: %d %lu \n", i, (long*)res);
    }
    return EXIT_SUCCESS;
}

