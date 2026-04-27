#include <pthread.h>
#include <glib.h>
#include <stdio.h>

// тредлист
// список файлов для обработки одним потоком
// + кеши
struct Cache {      
    pthread_t tid;         
    int n;                  // файлов в списке
    char **files;           // список имен файлов
    GHashTable *queries;    // кеш суммарных пейлоадов по пути
    GHashTable *refs;       // кеш частот по рефереру
};
// распределить файлы по потокам
// не более одного потока на файл
struct Cache *divide(const int threads_n, const char *dir_name, int *n);
void *worker(void * arg);
void free_thread_list(struct Cache *divs, const int n);
