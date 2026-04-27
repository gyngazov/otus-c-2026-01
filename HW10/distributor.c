
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <glib.h>

#include "hash.h"
#include "parse.h"
#include "distributor.h"


void *worker(void * arg)
{
    struct Cache *cache = (struct Cache *) arg;
    cache->tid = pthread_self();
    FILE *fp;
    char buf[BUF_LEN];
    struct LogLine *ll;
    char *fname;

    for (int i = 0; i < cache->n; i++) {
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

static void set_cache(struct Cache *div, int list_len, DIR *dir)
{
    div->n = list_len;
    div->files = get_files(list_len, dir);
    div->queries = init();
    div->refs = init();    
}

// тредлист
// распределить файлы по потокам
struct Cache *divide(const int threads_n, const char *dir_name, int *m)
{
    DIR *dir;

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
    for (; i < rest; i++)
        set_cache(&divs[i], base + 1, dir);
    for (; i < n; i++) {
        divs[i].n = base;
        divs[i].files = get_files(base, dir);
        divs[i].queries = init();
        divs[i].refs = init();
    }

    if (closedir(dir) == -1) {
        perror("Папка не закрывается");
        return NULL;
    };
    return divs;
}

void free_thread_list(struct Cache *divs, const int n)
{
    for (int i = 0; i < n; i++) {
        free(divs[i].files);
        destroy(divs[i].queries);
        destroy(divs[i].refs);
    }
}

int read_dir(DIR *dir, struct entry *e)
{
    errno = 0;
    e = readdir(dir);
    return e == NULL && e != 0 ? errno : 0;
}


