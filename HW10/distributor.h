//#include <pthread.h>
#include <glib.h>
//#include <stdio.h>

// кеши
struct Caches {      
    GHashTable *queries;    // кеш суммарных пейлоадов по пути
    GHashTable *refs;       // кеш частот по рефереру
};

void *worker(void * arg);
int count_files(const char *dir_name);
