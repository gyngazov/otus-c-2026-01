#include <pthread.h>
#include <glib.h>
#include <stdio.h>

struct Cache {      
    pthread_t tid;         
    int n;
    char **files;
    GHashTable *queries;
    GHashTable *refs;
};
// распределить файлы по потокам
// не более одного потока на файл
struct Cache *divide(const int threads_n, const char *dir_name, int *n);
void *worker(void * arg);
