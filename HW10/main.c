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
    if (argc != 2)
        return 1;
    char *dir = argv[1];
    int dir_len = strlen(dir);
    int n = count_files(dir);
    struct dirent *de;
    DIR *dr = opendir(dir);
    int i = 0;
    int len;
    char *files[n];
    readdir(dr);
    readdir(dr);
    while ((de = readdir(dr)) != NULL) {
        len = dir_len + 1 + strlen(de->d_name) + 1;
        files[i] = (char *) malloc(len);
        snprintf(files[i], len, "%s%s%s", dir, "/", de->d_name);
        i++;
    }
    closedir(dr);
    for (int i = 0; i < n; i++)
        printf("%s\n", files[i]);
    


    pthread_t thrds[n];
    int err;
    for (int i = 0; i < n; i++) {
        err = pthread_create(&thrds[i], NULL, worker, files[i]);
        if (err != 0) {
            perror("A new thread cannot be created");
            exit(errno);
        }
    }
    void *res;
    
    for (int i = 0; i < n; i++) {
        err = pthread_join(thrds[i], &res);
        if (err != 0)
            printf("Поток не завершается\n");
        else if ((int*)res == PTHREAD_CANCELED)
            printf("Поток отменен");
        else {
            GHashTable *refs = (GHashTable *) res;
            printf("%d\n", i);
            get_top(refs);
            destroy(refs);
        }
    }
    for (int i = 0; i < n; i++)
        free(files[i]);
    return EXIT_SUCCESS;
}



