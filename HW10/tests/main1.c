#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "hash.h"
#include "parse.h"
#include "distributor.h"

int main(int argc, char **argv)
{
    int m;
    struct Cache *divs = divide(5, "xs", &m);
    
    if (divs == NULL)
        exit(EXIT_FAILURE);
    int len;
    for (int i = 0; i < m; i++) {
        len = divs[i].n;
        printf("%d: n->%d fsl->", i, len);
        for (int k = 0; k < len; k++)
            printf("%s ", divs[i].files[k]);
    }
    // free_thread_list(divs, m);

    pthread_t thrds[m];
    int err;
    struct Cache *cache;
    char *files[] = {"a.log", "b.log"};
    for (int i = 0; i < m; i++) {
        err = pthread_create(&thrds[i], NULL, worker, files[i]);
        if (err != 0) {
            perror("A new thread cannot be created");
            exit(errno);
        }
    }
    void *res;
    
    for (int i = 0; i < m; i++) {
        err = pthread_join(thrds[i], &res);
        if (err != 0)
            printf("Поток не завершается\n");
        else if ((int*)res == PTHREAD_CANCELED)
            printf("Поток отменен");
        // else {
        //     cache = (struct Cache *) res;
        //     printf("%d: %ld\n", i, cache->tid);
        // }
    }
        
    return EXIT_SUCCESS;
}



