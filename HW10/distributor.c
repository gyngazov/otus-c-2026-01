
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <glib.h>

#include "hash.h"
#include "parse.h"



void *worker(void * arg)
{
    struct Cache *cache = (struct Cache *) arg;
    cache->tid = pthread_self();
    FILE *fp;
    char buf[BUF_LEN];
    struct LogLine *ll;
    char *fname;

    for (int i = 0; i < cache->num; i++) {
        fname = cache->files[i];
        fp = fopen(fname, "r");
        if (fp == NULL) {
            perror("Не удалось открыть файл");
            return NULL;
        }
        while (fgets(buf, BUF_LEN, fp) != NULL) {
            ll = parse_line(buf);
            inc(cache->refs, ll->ref, 1);
            inc(cache->queries, ll->url, ll->size);
        }
        fclose(fp);
    }
    return((void *)cache);
}

// тредлист
// распределить файлы по потокам
struct Cache *divide(const int threads_n, const char *dir_name, int *m)
{
    DIR *dir;
    struct dirent *entry;

    int k = count_files(dir_name);
    if (k == -1) {
        printf("Ошибка чтения папки\n");
        return NULL;
    }
    const int base = k / threads_n;
    const int rest = k % threads_n;
    const int n = threads_n >= k ? k : threads_n;
    *m = n;
    struct Cache *divs = (struct Cache *) malloc(n * sizeof(struct Cache));
    if (divs == NULL) {
        printf("Нет памяти\n");
        return NULL;
    }
    dir = opendir(dir_name);
    if (dir == NULL) {
        perror("Папка не доступна");
        return NULL;
    }
    readdir(dir);
    readdir(dir);
    int i = 0;
    GHashTable *hash;
    for (; i < rest; i++) {
        divs[i].n = base + 1;
        divs[i].files = get_files(base + 1, dir);
        divs[i].queries = init();
        divs[i].refs = init();
    }
    for (; i < n; i++) {
        divs[i].n = base;
        divs[i].files = get_files(base, dir);
        divs[i].queries = init();
        divs[i].refs = init();
    }

    closedir(dir);
    return divs;
}

// штук файлов в папке
static int count_files(const char *dir_name)
{
    DIR *dir;
    dir = opendir(dir_name);
    if (dir == NULL) {
        perror("Папка не доступна");
        return -1;
    }
    int k = 0;
    readdir(dir); // .
    readdir(dir); // ..
    while (readdir(dir) != NULL)
        k++;
    closedir(dir);
    if (k == 0) {
        printf("Нет файлов.\n");
        return -1;
    }
    return k;
}

// список файлов для потока
static char **get_files(int num_files, DIR *dir)
{
    struct dirent *entry;
    char **fls = malloc(num_files * sizeof(char *));
    for (int j = 0; j < num_files; j++) {
        entry = readdir(dir);
        fls[j] = entry->d_name;
    } 
    return fls;
}


