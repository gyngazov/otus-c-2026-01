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

    int n;
    struct Cache *divs = divide(3, "xs", &n);
    
    if (divs == NULL)
        exit(EXIT_FAILURE);
    int len;
    for (int i = 0; i < n; i++) {
        len = divs[i].n;
        printf("%d: n->%d fsl->", i, len);
        for (int k = 0; k < len; k++)
            printf("%s ", divs[i].files[k]);
    }
    free_thread_list(divs, n);
    // if (argc != 3) {
    //     printf("usage: %s {logs_dir} {number_of_threads}");
    //     return EXIT_FAILURE;
    // }

    // int n = 7;
    // pthread_t thrds[n];
    // int err;
    // for (int i = 0; i < n; i++) {
    //     err = pthread_create(&thrds[i], NULL, thr_fcn, NULL);
    //     if (err != 0) {
    //         perror("A new thread cannot be created");
    //         return errno;
    //     }

    // }
    // sleep(7);
    // void *res;
    // for (int i = 0; i < n; i++) {
    //     err = pthread_join(thrds[i], &res);
    //     if (err != 0)
    //         printf("A slave thread is not joinable because it is detached. \n");
    //     else if ((int*)res == PTHREAD_CANCELED)
    //         printf("A slave thread was canceled");
    //     else
    //         printf("A slave thread returned: %d %lu \n", i, (long*)res);
    // }
    return EXIT_SUCCESS;
}



