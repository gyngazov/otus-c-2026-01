#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "hash.h"
#include "parse.h"
#include "distributor.h"

#define USAGE       "Usage: %s -d <log dir> -n <threads number>\n"
#define FAIL        printf(USAGE, argv[0])
#define NOMEM       "Не выделена память"
#define ERR_BUF     -7

int getopt(int argc, char** argv, char *params);
void check_err(int err);

int main(int argc, char **argv)
{
    if (argc == 1) {
        FAIL;
        exit(EXIT_FAILURE);
    }
    char *dir;
    int thrn;
    int opt;
    extern char *optarg;
    extern int optind;
    while((opt = getopt(argc, argv, ":d:n:")) != -1) { 
        switch(opt) {  
            case 'd': 
                dir = optarg;
                break;
            case 'n': 
                thrn = atoi(optarg);
                break;
            case ':': 
                FAIL;
                exit(EXIT_FAILURE);
            case 'h':
            case '?': 
            default:
                FAIL;
                exit(EXIT_FAILURE);
        } 
    }
    if (optind < argc) {
        FAIL;
        exit(EXIT_FAILURE);
    }
    const int dir_len = strlen(dir);
    int n = count_files(dir);
    if (thrn <= n)
        n = thrn;
    struct dirent *de;
    DIR *dr = opendir(dir);
    if (dr == NULL) {
        perror("Не открылась папка");
        exit(errno);
    }
    int i = 0;
    int len;
    char *files[n];
    readdir(dr);
    readdir(dr);
    while ((de = readdir(dr)) != NULL) {
        len = dir_len + 1 + strlen(de->d_name) + 1;
        files[i] = (char *) malloc(len);
        if (files[i] == NULL) {
            puts(NOMEM);
            exit(EXIT_FAILURE);
        }
        if (snprintf(files[i], len, "%s%s%s", dir, "/", de->d_name) < 0) {
            puts("Ошибка копирования");
            exit(EXIT_FAILURE);
        }
        i++;
    }
    closedir(dr);

    pthread_t thrds[n];
    for (int i = 0; i < n; i++) {
        if (pthread_create(&thrds[i], NULL, worker, files[i]) != 0) {
            perror("Нельзя создать поток");
            exit(errno);
        }
    }

    void *res;
    struct Caches *ret;
    GHashTable *totalquer = init();
    GHashTable *totalrefs = init();
    for (int i = 0; i < n; i++) {
        if (pthread_join(thrds[i], &res) != 0)
            puts("Поток не завершается");
        else if ((int*)res == PTHREAD_CANCELED)
            puts("Поток отменен");
        else if ((int*)res == (int *)ERR_NOMEM)
            puts(NOMEM);
        else {
            ret = (struct Caches *) res;
            merge(totalquer, ret->queries);
            merge(totalrefs, ret->refs);
            destroy(ret->refs);
            destroy(ret->queries);
            free(ret);
        }
    }
    for (int i = 0; i < n; i++)
        free(files[i]);
    int total = sum(totalquer);
    printf("\nОтдано байт: %d\n", total);
    puts("\n10 самых тяжелых по трафику url-ов:");
    int err = get_top(totalquer);
    check_err(err);
    puts("\n10 наиболее часто встречающихся Referer'ов:");
    err = get_top(totalrefs);
    
    destroy(totalquer);
    destroy(totalrefs);
    return EXIT_SUCCESS;
}

void check_err(int err)
{
    if (err == ERR_NOMEM || err == ERR_BUF)
        exit(EXIT_FAILURE);
}



