
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <glib.h>

#include "hash.h"
#include "parse.h"
#include "distributor.h"

// поток на файл
void *worker(void * arg)
{
    char *name = (char *) arg;
    FILE *fp;
    char buf[BUF_LEN];
    struct LogLine *ll;

    fp = fopen(name, "r");
    if (fp == NULL) {
        perror("Не удалось открыть файл");
        return NULL;
    }
    GHashTable *refs = init();
    GHashTable *qrys = init();
    while (fgets(buf, BUF_LEN, fp) != NULL) {
        ll = parse_line(buf);
        if (ll == NULL) {
            puts(NOMEM);
            return (void *) ERR_NOMEM;
        }
        inc(refs, ll->ref, 1);
        inc(qrys, ll->url, ll->size);
    }
    fclose(fp);
    struct Caches *ret = (struct Caches *) malloc(sizeof(struct Caches));
    if (ret == NULL) {
        puts(NOMEM);
        return (void *) ERR_NOMEM;
    }
    ret->queries = qrys;
    ret->refs = refs;
    return (void *) ret;
}

// штук файлов в папке
int count_files(const char *dir_name)
{
    DIR *dir;
    dir = opendir(dir_name);
    if (dir == NULL) {
        perror("Папка не доступна");
        return -1;
    }
    int k = 0;
    errno = 0;
    while (readdir(dir) != NULL)
        k++;
    if (errno != 0) {
        perror("Не удалось прочесть папку");
        return -1;
    }    
    closedir(dir);
    return k - 2; // без ./ ../
}




