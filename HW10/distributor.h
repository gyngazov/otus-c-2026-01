
#include <glib.h>

#define ERR_NOMEM   -3
#define NOMEM       "Не выделена память"

// кеши
struct Caches {      
    GHashTable *queries;    // кеш суммарных пейлоадов по пути
    GHashTable *refs;       // кеш частот по рефереру
};

void *worker(void * arg);
int count_files(const char *dir_name);
